/******************************************************************************************************************************
*              MODULE IMPLEMENTATION FILE
******************************************************************************************************************************
*  COPYRIGHT (C) 2019 ASR Corporation.
*
*  This file and the software in it is furnished under
*  license and may only be used or copied in accordance with the terms of the
*  license. The information in this file is furnished for informational use
*  only, is subject to change without notice, and should not be construed as
*  a commitment by ASR Corporation. ASR Corporation assumes no
*  responsibility or liability for any errors or inaccuracies that may appear
*  in this document or any software that may be provided in association with
*  this document.
*  Except as permitted by such license, no part of this document may be
*  reproduced, stored in a retrieval system, or transmitted in any form or by
*  any means without the express written consent of ASR Corporation.
*
*  Title:   HTTP Package HTTP API
*
*  Filename:    libhttpclient.c
*  Target, subsystem: Common Platform, HAL
*
*  Authors:	Roy Shi
*
*  Description: This file includes all the API that the http Package support
*
*  Last Modified: <Initial> <date>
*
*  Notes:
*********************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sockets.h"
#include "netdb.h"
#include "libhttpclient.h"
#include "http_client_base64.h"
#include "digest.h"
#include "ssl_http_client.h"
#include "httpclient_sys.h"
#include "netif.h"
#include "tcp.h"

struct server_addr {
	char * url; // dup from user setting
	char * host; //
	char * path; //
	char * name;
	char * password;
	unsigned short int port;
	int auth_type;
	int https;
	char * auth_value;
	struct sockaddr_in ip;
	struct sockaddr_in6 ip6;
	int use_ip6;
	int url_is_ip6_address;
	int fd;
};

struct response {
  struct http_client_list * response_header;
  int result_code;
  int response_data_length;
};

struct http_client_t {
	struct server_addr server; // http server address information
	client_response_cb rsp_cb; // write http response to client application
	void * rsp_cb_data;
	const char * post_data; // user post data without http header
	int post_length;
	struct http_client_list * cookie;
	struct http_client_list * private_header; // user specify http header
	struct response response; // response from http server, like result code and http header
	int request_type;
	struct ssl_client * ssl;
	int connection_close;
	int http1_0;
	char * location;
	int cid;
	unsigned short port;
};

struct http_header_parse_table {
	const char * tag;
	const char * format;
	const char * full;
	int (*callback) (struct http_client_t *, struct http_header_parse_table *, char *);
};

#define TABLE_ELEMENT(TAG, FORMAT, CALLBACK) {TAG, FORMAT, TAG##FORMAT, CALLBACK}


#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#define PRINT_BUF_SIZE 65
static char print_buf[PRINT_BUF_SIZE];
static void print_string(const char * string)
{
	int len = strlen(string);
	int printed = 0;

	while (printed != len) {
		if ((len - printed) > (PRINT_BUF_SIZE - 1)) {
			memcpy(print_buf, string + printed, (PRINT_BUF_SIZE - 1));
			printed += (PRINT_BUF_SIZE - 1);
			print_buf[PRINT_BUF_SIZE - 1] = '\0';
		} else {
			sprintf(print_buf, "%s", string + printed);
			printed = len;
		}
		http_debug("[value]%s", print_buf);
	}
}

char * __http_client_strdup(const char *s)
{
	int len;
	char * p=NULL;

	if (s == NULL)
		return NULL;

	len = strlen(s) + 1;
	if ((p = HTTP_CLIENT_MALLOC(len))==NULL) {
		http_debug("Strdup failed! Cannot malloc memory");
		return NULL;
	}
	memcpy(p, s, len);
	return p;
}

void * __http_client_malloc(int size)
{
	void * p = malloc(size);
	//LOG("malloc @%p = 0x%p", __return_address(), p);
	return p;
}

void __http_client_free(void * p)
{
	//LOG("free @%p = 0x%p", __return_address(), p);
	if (p) free(p);
}

static OSMsgQRef httpclient_msgq;
static OSTaskRef httpclient_task_ref = NULL;
#define MAX_HTTPCLIENT_MSG_SIZE (sizeof(void *))
#define MAX_HTTPCLIENT_MSGQ_SIZE 64

struct http_task_command {
	int command_id;
};

static int timer_flag = 0;
static void http_task(void * data)
{
	struct http_task_command * msg = NULL;
	int fd = -1;
	while (1) {
		OSAMsgQRecv(httpclient_msgq, (void *)&msg, MAX_HTTPCLIENT_MSG_SIZE, 
			timer_flag ? 40 : OSA_SUSPEND);
		http_debug("Task running(%p)...", msg);
		if (msg) {
			switch (msg->command_id) {
				case 3:
					timer_flag--;
					http_debug("Handle task stop(%d)", timer_flag);
					break;
				case 2:
					timer_flag++;
					http_debug("Handle task start(%d)", timer_flag);
					break;
				default:
					http_debug("!!! UNKNOW COMMAND !!!");
					break;
			}
			free(msg);
			msg = NULL;
		}
		fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		OSATaskSleep(40);
		closesocket(fd);
		fd = -1;
	}
}

static void httpclient_task_init(void)
{
	void* ptr_task_stack = NULL;
	#undef STACK_SIZE
	#define STACK_SIZE 1024
	OSA_STATUS status;

	if (httpclient_task_ref) {
		return;
	}
	http_debug("Create task");
	// Init system message queue
	status = OSAMsgQCreate(&httpclient_msgq, "http_msgq",
				MAX_HTTPCLIENT_MSG_SIZE, MAX_HTTPCLIENT_MSGQ_SIZE, OS_FIFO);
	DIAG_ASSERT(status == OS_SUCCESS);
	ptr_task_stack = (void*)malloc(STACK_SIZE);
	OSATaskCreate(&httpclient_task_ref, ptr_task_stack,
				STACK_SIZE, 100, "httpclient",
				http_task, NULL);
	OSATaskSleep(1 * 1000 / 5);
}

static void task_start_stop(int start)
{
	struct http_task_command * command = NULL;
	OSA_STATUS status;
	if (!httpclient_msgq) {
		http_error("!!! FAILED %s:%d !!!", __func__, __LINE__);
		return;
	}
	command = malloc(sizeof(struct http_task_command));
	command->command_id = start ? 2 : 3;
	status = OSAMsgQSend(httpclient_msgq, MAX_HTTPCLIENT_MSG_SIZE, (void *)&command, OSA_NO_SUSPEND);
	if (status != OS_SUCCESS) {
		http_error("!!! COMMNAND LOSE %d@%p !!!", command->command_id, __return_address());
		free(command);
	}
	return;
}

struct http_client * http_client_init(void)
{
	struct http_client_t * client;
	//httpclient_task_init();
	client = (struct http_client_t *)HTTP_CLIENT_MALLOC(sizeof(struct http_client_t));
	if (!client) {
		http_error("Cannot get memory!");
		return NULL;
	}
	memset(client, 0, sizeof(struct http_client_t));
	client->server.fd = INVALID_SOCKET;
	//task_start_stop(1);
	return (struct http_client *)client;
}

struct http_client_list * http_client_list_append(struct http_client_list * list, const char * data)
{
	struct http_client_list * new_list;

	if (data == NULL) {
		http_error("List append failed, data is NULL");
		return list; // No change
	}

	new_list = HTTP_CLIENT_MALLOC(sizeof(struct http_client_list));
	if (new_list == NULL) {
		http_error("List append failed, cannot malloc memory");
		return NULL;
	}
	new_list->data = strdup(data);
	new_list->next = list;
	return new_list;
}

void http_client_list_destroy(struct http_client_list * list)
{
	struct http_client_list * temp = list;
	while (list) {
		temp = list->next;
		HTTP_CLIENT_FREE(list->data);
		HTTP_CLIENT_FREE(list);
		list = temp;
	}
}

static const char * get_request_string(struct http_client_t * client)
{
	switch(client->request_type) {
	case HTTPCLIENT_REQUEST_GET:
		return "GET";
	case HTTPCLIENT_REQUEST_POST:
		return "POST";
	case HTTPCLIENT_REQUEST_PUT:
		return "PUT";
	default:
		// ASSERT
		return "NULL";
	}
}

static int set_header(struct http_client_t * client, void * arg)
{
	// Check http header?
	client->private_header = (struct http_client_list *)arg;
	return HTTP_CLIENT_OK;
}

static int set_postdata(struct http_client_t * client, void * arg)
{
	client->post_data = (char *)arg;
	return HTTP_CLIENT_OK;
}

static int set_postdata_len(struct http_client_t * client, int arg)
{
	client->post_length = arg;
	return HTTP_CLIENT_OK;
}

static int build_new_socket(struct http_client_t * client)
{
	int ret, optval;
	int has_ssl = 0;
	struct sockaddr_in local4;
	struct sockaddr_in6 local6;

	
	if (client->ssl) {
		http_debug("build_new_socket: Clear ssl context");
		ssl_client_shutdown(client->ssl);
		client->ssl = NULL;
		has_ssl = 1;
	}
	if (client->server.fd > 0) {
		closesocket(client->server.fd);
	}
	http_debug("build_new_socket use_ip6(%d)", client->server.use_ip6);
	
	if (client->server.use_ip6) {
		client->server.fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	} else {
	    client->server.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	if (client->server.fd < 0) {
		http_error("build_new_socket failed(%d)", client->server.fd);
		return HTTP_CLIENT_ERROR;
	}
	OSATaskSleep(1 * 1000 / 5);
	optval = 1;
	ret = setsockopt(client->server.fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));
	if (ret < 0) {
		http_error("build_new_socket set tcp no delay failed(%d)", ret);
	}
	
    optval = 20 * 1000; // 20s
    ret = setsockopt(client->server.fd, SOL_SOCKET, SO_RCVTIMEO, &optval, sizeof(optval));
	if (ret < 0) {
		http_error("build_new_socket set tcp recv timeout failed(%d)", ret);
	}

    if(client->port){
        //optval = 1;
        //ret = setsockopt(client->server.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
        struct linger lin;
        lin.l_onoff = 1;
        lin.l_linger = 0;
        ret = setsockopt(client->server.fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
        if (ret < 0) {
            http_error("Set socket SO_LINGER failed(%d)", ret);
        }
        

        
        if(client->server.use_ip6)
        {
            local6.sin6_family = AF_INET6;
            local6.sin6_port = htons(client->port);
            local6.sin6_len = sizeof(struct sockaddr_in6);
            memset(&local6.sin6_addr, 0, 16);
            if(bind(client->server.fd,(struct sockaddr *)&local6,sizeof(struct sockaddr))<0){
                http_error("ip6 bind port[%d] failed", client->port);
            }else{
                http_error("ip6 bind port[%d] success", client->port);
            }
        }else{
            local4.sin_family = AF_INET;
            local4.sin_port = htons(client->port);
            local4.sin_addr.s_addr = 0;
            if(bind(client->server.fd,(struct sockaddr *)&local4,sizeof(struct sockaddr))<0){
                http_error("ip4 bind port[%d] failed", client->port);
            }else{
                http_error("ip4 bind port[%d] success", client->port);
            }
        }
    
    }   


	
	if (client->server.use_ip6) {
		ret = connect(client->server.fd, (struct sockaddr *)&client->server.ip6, sizeof(client->server.ip6));
	} else {
	    ret = connect(client->server.fd, (struct sockaddr *)&client->server.ip, sizeof(client->server.ip));
	}
	if (ret < 0) {
		http_error("build_new_socket connect failed(%d)", ret);
		return HTTP_CLIENT_ERROR;
	}
	http_debug("Socket connect successful");
	/* Reset connetion_close */
	client->connection_close = 0;
	if (has_ssl) {
		client->ssl = ssl_client_init(client->server.fd);
		if (client->ssl == NULL) {
			http_error("Try build new SSL client failed");
			return HTTP_CLIENT_ERROR;
		}
	}
	return HTTP_CLIENT_OK;
}

static int is_valid_ip46_address(char *ip_string)
{
	int a,b,c,d;
	char* temp; 
	int flag=0;
	if((sscanf(ip_string,"%d.%d.%d.%d",&a,&b,&c,&d)==4)
		&& a>=0&&a<=255
		&& b>=0&&b<=255
		&& c>=0&&c<=255
		&& d>=0&&d<=255)
	{
		return 1;
	}else{
		temp=ip_string;
		while(*temp!='\0')
		{
			if(*temp==':')
			{
				flag++;
				temp++;
				continue;
			}else{
				if(*temp=='0'
					||(*temp>='1'&&*temp<='9')
					||(*temp>='a'&&*temp<='f')
					||(*temp>='A'&&*temp<='F')){
					temp++;
				}else{
					return 0;
				}	
			}
		}
		return 2;
	}
}
static int set_url(struct http_client_t * client, void * arg)
{
	const char * data = (char *)arg;
	char * old_host, * pos;
	int ret;
	struct netif *netif_cfg = NULL;
	struct sockaddr_in local4;
	struct sockaddr_in6 local6;

	if (!arg) {
		http_error("BAD arg!");
		return HTTP_CLIENT_ERROR;
	}
    http_debug("Set url version:2016-02-04 1918");

	http_debug("Set url raw data:");
	print_string(data);

	if (client->server.url != NULL) {
		HTTP_CLIENT_FREE(client->server.url);
	}

	if (client->server.path != NULL) {
		HTTP_CLIENT_FREE(client->server.path);
	}

	client->server.url = strdup(data);

	// If prefix is 'http://' ?
	if (!memcmp(data, "http", 4)) {
		if (*(data + 4) == 's') {
			// https ?
			if (!memcmp(data, "https://", 8)) {
				client->server.https = 1;
				data += 8;
			}
		} else if (*(data + 4) == ':') {
			if (!memcmp(data, "http://", 7)) {
				data += 7;
			}
		}
	}

	client->server.url_is_ip6_address=0;
	if((*data == '[')&&(strstr(data, "]:")))
	{
		data += 1;
		client->server.url_is_ip6_address=1;
	}
	// If set port number ?
	if (client->server.https) {
		client->server.port = 443;
	} else {
		client->server.port = 80;
	}

	old_host = client->server.host;
	client->server.host = strdup(data);
	pos = strchr(client->server.host, '/');
	if (pos == NULL) {
		client->server.path = strdup("/");
	} else {
		client->server.path = strdup(pos);
		*pos = '\0'; // Set server.host value end
	}
	if(client->server.url_is_ip6_address)
	{
		pos = strstr(client->server.host, "]:");
		if (pos) {
			if (1 == sscanf(pos, "]:%hu", &client->server.port)) {
				http_debug("User use special http port %d", client->server.port);
			}
			*pos = '\0'; //Need check
			*(pos+1) = '\0'; //Need check
		} else {
            http_debug("User does not use special http port");
		}
	}else{
	    pos = strchr(client->server.host, ':');
	    if (pos) {
    		if (1 == sscanf(pos, ":%hu", &client->server.port)) {
    			http_debug("User use special http port %d", client->server.port);
    		}
    		*pos = '\0'; //Need check
		}
	}

	http_debug("Http client parse address result:");
	http_debug("  host:");
	print_string(client->server.host);
	http_debug("  path:");
	print_string(client->server.path);

	if (old_host && !strcmp(old_host, client->server.host)) {
		// If the old host is present and value same with current, not need do socket connect again
		HTTP_CLIENT_FREE(old_host);
		return HTTP_CLIENT_OK;
	}
	// Get IP address by DNS
	{
		struct hostent * host_entry;
		int optval;
		struct addrinfo * pres = NULL;
		if (client->server.fd != INVALID_SOCKET) {
			closesocket(client->server.fd);
		}

		#if 0
		host_entry = gethostbyname(client->server.host);
		if (host_entry == NULL) {
			LOG("DNS failed!\n");
			return HTTP_CLIENT_ERROR;
		}
		LOG("Get %s ip %d.%d.%d.%d\n", client->server.host, host_entry->h_addr_list[0][0] & 0xff,
			host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);
		#endif
		int valid_ip_flag=0;
		ret= is_valid_ip46_address(client->server.host);
		if(ret){
			valid_ip_flag=1;
			if(ret==1)
			{
				struct sockaddr_in adr_inet;
				unsigned int temp;
				client->server.use_ip6 = 0;
				inet_aton(client->server.host, &adr_inet.sin_addr);
				client->server.ip.sin_addr = adr_inet.sin_addr;
				client->server.ip.sin_family = AF_INET;
				client->server.ip.sin_port = htons(client->server.port);
				temp = (unsigned int)client->server.ip.sin_addr.s_addr;
				http_debug("Get %s: %s", client->server.host, inet_ntoa(temp));
			}
			else
			{
				struct sockaddr_in6 sa;
				inet6_aton(client->server.host, &sa.sin6_addr);
				client->server.ip6.sin6_addr=sa.sin6_addr;
				client->server.use_ip6 = 1;
				client->server.ip6.sin6_family = AF_INET6;
				client->server.ip6.sin6_port = htons(client->server.port);
				client->server.ip6.sin6_len = sizeof(struct sockaddr_in6);
				http_debug("Get IPV6:%s: %s", client->server.host, inet6_ntoa(client->server.ip6.sin6_addr));
			}
		}else{
			ret = getaddrinfo(client->server.host, NULL, NULL, &pres);
			if (ret < 0) {
				http_error("DNS getaddrinfo failed(%d)!\n",ret);
				return HTTP_CLIENT_ERROR;
			}
			if (pres == NULL) {
				http_error("DNS getaddrinfo faield?!\n");
				return HTTP_CLIENT_ERROR;
			}
			if (pres->ai_addrlen == sizeof(struct sockaddr_in)) {
				client->server.use_ip6 = 0;
				unsigned int temp;
				struct sockaddr_in * sin_res = (struct sockaddr_in *)pres->ai_addr;
				client->server.ip.sin_addr = sin_res->sin_addr;
				client->server.ip.sin_family = AF_INET;
				client->server.ip.sin_port = htons(client->server.port);
				temp = (unsigned int)client->server.ip.sin_addr.s_addr;
				http_debug("DNS getaddrinfo,Get %s: %s", client->server.host, inet_ntoa(temp));
			} else {
				client->server.use_ip6 = 1;
				client->server.ip6.sin6_family = AF_INET6;
				client->server.ip6.sin6_port = htons(client->server.port);
				client->server.ip6.sin6_len = sizeof(struct sockaddr_in6);
				memcpy(&client->server.ip6.sin6_addr, pres->ai_addr->sa_data, 16);
				http_debug("DNS getaddrinfo,Get IPV6:%s: %s", client->server.host, inet6_ntoa(client->server.ip6.sin6_addr));
			}
			freeaddrinfo(pres);
		}
loop:
		// Get socket handle
		if (client->server.use_ip6) {
			client->server.fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		} else {
		    client->server.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		if (client->server.fd < 0) {
			http_error("Create socket failed(%d)", client->server.fd);
			return HTTP_CLIENT_ERROR;
		}
		OSATaskSleep(1 * 1000 / 5);
		optval = 1;
		ret = setsockopt(client->server.fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));
		if (ret < 0) {
			http_error("Set tcp no delay failed(%d)", ret);
		}

		optval = 20 * 1000; // 20s
        ret = setsockopt(client->server.fd, SOL_SOCKET, SO_RCVTIMEO, &optval, sizeof(optval));

		if (ret < 0) {
			http_error("Set tcp recv timeout failed(%d)", ret);
		}
		#if 0
		if(client->cid){
            netif_cfg=netif_find_by_pcid(client->cid-1);
            if(netif_cfg){
               if(client->server.use_ip6)
               {
                    if(netif_is_ip6_cfg(netif_cfg)){
                        local6.sin6_family = AF_INET6;
                        local6.sin6_port = 0;
                        local6.sin6_len = sizeof(struct sockaddr_in6);
                        memcpy(&local6.sin6_addr, netif_get_global_ip6addr(netif_cfg), 16);
                        if(bind(client->server.fd,(struct sockaddr *)&local6,sizeof(struct sockaddr))){
                            http_error("bind cid[%d] failed(ip6 %s)", client->cid-1,inet6_ntoa(local6.sin6_addr));
                        }else{
                            http_error("bind cid[%d] success(ip6 %s)", client->cid-1,inet6_ntoa(local6.sin6_addr));
                        }
                    }else{
                        http_error("bind cid[%d] failed(ip6 have not ready)", client->cid-1);

                    }

               }else{
                   local.sin_family = AF_INET;
                   local.sin_port = 0;
                   local.sin_addr.s_addr = netif_get_ipaddr_u32(netif_cfg);
                   if(bind(client->server.fd,(struct sockaddr *)&local,sizeof(struct sockaddr))){
                       http_error("bind cid[%d] failed(ip %s)", client->cid-1,inet_ntoa(local.sin_addr.s_addr));
                   }else{
                        http_debug("bind cid[%d] success(ip %s)", client->cid-1,inet_ntoa(local.sin_addr.s_addr));
                   }
               }
            }else{
                http_error("bind cid[%d] failed(not find netif by cid)", client->cid-1);
            }
		}
		#endif

        if(client->port){
            //optval = 1;
            //ret = setsockopt(client->server.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
            struct linger lin;
        	lin.l_onoff = 1;
        	lin.l_linger = 0;
            ret = setsockopt(client->server.fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
            if (ret < 0) {
                http_error("Set socket SO_LINGER failed(%d)", ret);
            }
            
            if(client->server.use_ip6)
            {
                local6.sin6_family = AF_INET6;
                local6.sin6_port = htons(client->port);
                local6.sin6_len = sizeof(struct sockaddr_in6);
                memset(&local6.sin6_addr, 0, 16);
                if(bind(client->server.fd,(struct sockaddr *)&local6,sizeof(struct sockaddr))<0){
                    http_error("ip6 bind port[%d] failed", client->port);
                }else{
                    http_error("ip6 bind port[%d] success", client->port);
                }
            }else{
                local4.sin_family = AF_INET;
                local4.sin_port = htons(client->port);
                local4.sin_addr.s_addr = 0;
                if(bind(client->server.fd,(struct sockaddr *)&local4,sizeof(struct sockaddr))<0){
                    http_error("ip4 bind port[%d] failed", client->port);
                }else{
                    http_error("ip4 bind port[%d] success", client->port);
                }
            }

        }	

		http_debug("Get socket fd %d", client->server.fd);
		// Do DNS
		if (client->server.use_ip6) {
			ret = connect(client->server.fd, (struct sockaddr *)&client->server.ip6, sizeof(client->server.ip6));
		} else {
			ret = connect(client->server.fd, (struct sockaddr *)&client->server.ip, sizeof(client->server.ip));
		}
		if (ret < 0) {
			http_debug("Socket connect failed(%d),use_ip6(%d),valid_ip_flag(%d)", ret,
										client->server.use_ip6,
										valid_ip_flag);
			closesocket(client->server.fd);
			client->server.fd = INVALID_SOCKET;
			if(client->server.use_ip6&&valid_ip_flag==0)
			{
        		host_entry = gethostbyname(client->server.host);
        		if (host_entry == NULL) {
        			http_error("DNS gethostbyname failed!\n");
        			return HTTP_CLIENT_ERROR;
        		}
				client->server.use_ip6=0;
				http_debug("DNS gethostbyname,Get %s ip %d.%d.%d.%d\n", client->server.host, host_entry->h_addr_list[0][0] & 0xff,
			        host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);
        		client->server.ip.sin_addr = * (struct in_addr *) host_entry->h_addr_list[0];
        		client->server.ip.sin_family = AF_INET;
        		client->server.ip.sin_port = htons(client->server.port);
				goto loop;
			}
			return HTTP_CLIENT_ERROR;
		}
		http_debug("Socket connect successful");
	}
	return HTTP_CLIENT_OK;
}

static int set_responsecb(struct http_client_t * client, void * arg)
{
	client->rsp_cb = (client_response_cb)arg;
	return HTTP_CLIENT_OK;
}

static int set_responsecb_data(struct http_client_t * client, void * arg)
{
	client->rsp_cb_data = (void *)arg;
	return HTTP_CLIENT_OK;
}

static int set_auth_type(struct http_client_t * client, int arg)
{
	client->server.auth_type = arg;
	return HTTP_CLIENT_OK;
}

static int set_auth_username(struct http_client_t * client, void * arg)
{
	const char * username = (char *)arg;
	client->server.name = strdup(username);
	return (client->server.name ? HTTP_CLIENT_OK : HTTP_CLIENT_ERROR);
}

static int set_auth_password(struct http_client_t * client, void * arg)
{
	const char * password = (char *)arg;
	client->server.password = strdup(password);
	return (client->server.password ? HTTP_CLIENT_OK : HTTP_CLIENT_ERROR);
}

static int set_method(struct http_client_t * client, int arg)
{
	client->request_type = arg;
	return HTTP_CLIENT_OK;
}

static int set_http1_0(struct http_client_t * client, int arg)
{
	client->http1_0 = arg;
	return HTTP_CLIENT_OK;
}

static int set_pdp_cid(struct http_client_t * client, int arg)
{
	client->cid = arg+1;
	http_debug("set_pdp_cid %d",arg);
	return HTTP_CLIENT_OK;
}

static int set_bind_port(struct http_client_t * client, int arg)
{
	client->port = arg;
	http_debug("set_bind port %d",arg);
	return HTTP_CLIENT_OK;
}



int http_client_setopt(struct http_client *client, int tag, ...)
{
	int err = HTTP_CLIENT_ERROR;
	va_list arg;
	struct http_client_t * client_t = (struct http_client_t *)client;
	if (!client) {
		http_error("No client!");
		return HTTP_CLIENT_ERROR;
	}

	va_start(arg, tag);
	switch (tag) {
	case HTTPCLIENT_OPT_URL:
		err = set_url(client_t, va_arg(arg, char *));
		break;
	case HTTPCLIENT_OPT_METHOD:
		err = set_method(client_t, va_arg(arg, int));
		break;
	case HTTPCLIENT_OPT_HTTP1_0:
		err = set_http1_0(client_t, va_arg(arg, int));
		break;
	case HTTPCLIENT_OPT_HTTPHEADER:
		err = set_header(client_t, va_arg(arg, void *));
		break;
	case HTTPCLIENT_OPT_POSTDATA:
		err = set_postdata(client_t, va_arg(arg, void *));
		break;
	case HTTPCLIENT_OPT_POSTLENGTH:
		err = set_postdata_len(client_t, va_arg(arg, int));
		break;
	case HTTPCLIENT_OPT_RESPONSECB:
		err = set_responsecb(client_t, va_arg(arg, void *));
		break;
	case HTTPCLIENT_OPT_RESPONSECB_DATA:
		err = set_responsecb_data(client_t, va_arg(arg, void *));
		break;
	case HTTPCLIENT_OPT_AUTH_TYPE:
		err = set_auth_type(client_t, va_arg(arg, int));
		break;
	case HTTPCLIENT_OPT_AUTH_USERNAME:
		err = set_auth_username(client_t, va_arg(arg, char *));
		break;
	case HTTPCLIENT_OPT_AUTH_PASSWORD:
		err = set_auth_password(client_t, va_arg(arg, char *));
		break;
	case HTTPCLIENT_OPT_PDP_CID:
	    err = set_pdp_cid(client_t, va_arg(arg, int));
	    break;
	case HTTPCLIENT_OPT_BIND_PORT:
	    err = set_bind_port(client_t, va_arg(arg, int));
	    break;
	default:
		break;
	}
	va_end(arg);
	return err;
}

enum { STATE_DONE, STATE_STARTLINE, STATE_HEADER };

static int process_startline(struct http_client_t *client, char ** data, int *len)
{
	char * string = *data;
	char * at;
	char * pos;
	if (strncmp(string, "HTTP", 4)) {
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}
	at = memchr(string, '\r', *len);
	if (at == NULL) {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}
	if (*(++at) != '\n') {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}
	*at++ = '\0'; // Need check!
	if ((at - string) > *len) {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}
	http_debug("Get response start line:");
	print_string(string);
	pos = strchr(string, ' ');
	if (pos == NULL) {
		http_error("Fail parse start line!");
	} else {
		int cnt = sscanf(++pos, "%d", &client->response.result_code);
		if (cnt != 1) {
			http_error("Fail parse start line for response code");
		} else {
			http_debug("Response code %d", client->response.result_code);
		}
	}
	*data = at;
	*len = (*len - (at - string));
	return 0;
}

static int header_length(struct http_client_t *client, struct http_header_parse_table *table, char * data)
{
	if (sscanf(data, table->full, &client->response.response_data_length) != 1) {
		http_error("Failed @%s:%d", __func__, __LINE__);
	}
	http_debug("Get http content length %d", client->response.response_data_length);
	return 0;
}

static int header_cookie(struct http_client_t *client, struct http_header_parse_table *table, char * data)
{
	char * temp = strdup(data);
	char * pos = strchr(temp, '-'); /* Find '-' in 'Set-Cookie' */
	client->cookie = http_client_list_append(client->cookie, ++pos);
	http_debug("Handle cookie:");
	print_string(pos);
	if (temp) HTTP_CLIENT_FREE(temp);
	return 0;
}

static int header_location(struct http_client_t *client, struct http_header_parse_table *table, char * data)
{
	if (client->location) {
		HTTP_CLIENT_FREE(client->location);
	}
	http_debug("%s", data);
	client->location = strdup(data + 10);
	return 0;
}

static int header_connection(struct http_client_t *client, struct http_header_parse_table *table, char * data)
{
	if (strstr(data, "lose")) {
		client->connection_close = 1;
		http_debug("Handle connection setting: %s", data);
	}
	return 0;
}

static int header_auth(struct http_client_t *client, struct http_header_parse_table *table, char * data)
{
	struct digestdata * digest = NULL;
	const char * string_digest = "Digest";
	char * p;
	int len;

	http_debug("Headle auth:");
	print_string(data);
	p = data + strlen(table->tag); /* Skip HTTP header tag */
	while (*string_digest != '\0') {
		if (*p != *string_digest) {
			http_debug("HTTP client get auth header, but not match 'Digest', maybe is BASE auth, so adjust type");
			client->server.auth_type = HTTP_AUTH_TYPE_BASE;
			return -1;
		}
		p++; string_digest++;
	}
	if (client->server.auth_type == HTTP_AUTH_TYPE_BASE) {
		http_debug("User set auth type is BASE, but handle DIGEST auth response, so adjust auth type");
		client->server.auth_type = HTTP_AUTH_TYPE_DIGEST;
	}
	p = strchr(p, ' ');
	digest_input(p, &digest);
	digest_output(client->server.name, client->server.password, client->request_type == HTTPCLIENT_REQUEST_POST ? "POST" : "GET",
		client->server.path, digest, &client->server.auth_value, &len);
	clear_digestdata(digest);
	digest = NULL;
	return 0;
}

//not using TABLE_ELEMENT for delete build warning
static struct http_header_parse_table header_table[] = {
	//TABLE_ELEMENT("Content-Length: ", "%d", header_length),
	//TABLE_ELEMENT("Set-Cookie: ", "%s", header_cookie),
	//TABLE_ELEMENT("WWW-Authenticate: ", "%s", header_auth),
	//TABLE_ELEMENT("Connection: ", "%s", header_connection),
	//TABLE_ELEMENT("Location: ", "%s", header_location),
	{"Content-Length: ", "%d", "Content-Length: %d", header_length},
	{"Set-Cookie: ", "%s", "Set-Cookie: %s", header_cookie},
    {"WWW-Authenticate: ", "%s", "WWW-Authenticate: %s", header_auth},
    {"Connection: ", "%s", "Connection: %s", header_connection},
    {"Location: ", "%s", "Location: %s", header_location},	
	{NULL, NULL, NULL, NULL}
};

static int process_httpheader(struct http_client_t *client, char ** data, int *len)
{
	char * string = *data;
	char * at;
	int i = 0;

	http_debug("Enter %s", __func__);
	at = memchr(string, '\r', *len);
	if (at == NULL) {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}
	if (*(++at) != '\n') {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}

	*at++ = '\0'; // Need check!
	if ((at - string) > *len) {
		//ASSERT
		http_error("Failed @%s:%d", __func__, __LINE__);
		return -1;
	}

	//printf("Get http header: %s\n", string);
	// Call header parse callback
	i = 0;
	while (header_table[i].tag) {
		if (!memcmp(string, header_table[i].tag, strlen(header_table[i].tag))) {
			header_table[i].callback(client, &header_table[i], string);
			break;
		} else {
			// Record response header, if we need
			client->response.response_header = http_client_list_append(client->response.response_header, string);
		}
		i++;
	}
	*data = at;
	*len = (*len - (at - string));
	// Maybe need check remain length
	if ((*at == '\r') && (*(at+1) == '\n')) {
		// Http header end
		// Updata length and data pointer
		*len -= 2;
		*data = (at + 2);
	} else {
		// Continue process http header
		process_httpheader(client, data, len);
	}
	return 0;
}

static void clear_response(struct http_client_t *client)
{
	// Free all response header
	http_client_list_destroy(client->response.response_header);
	memset(&client->response, 0, sizeof(struct response));
}

static int check_http_header(const char * string, int len)
{
	int i = 0;
	enum {
		FOUND_NULL,
		FOUND_R,
		FOUND_RN,
		FOUND_RNR,
		FOUND_RNRN,
	};
	int state = FOUND_NULL;
	while (i < len) {
	    state = FOUND_NULL;
		switch (state) {
			case FOUND_NULL:
				if (string[i++] == '\r') {
					state = FOUND_R;
				}else{
                    break;
				}
        		//break;
        		//case FOUND_R:
				if (string[i++] == '\n') {
					state = FOUND_RN;
				}else{
                    break;
				}
				//break;
			    //case FOUND_RN:
				if (string[i++] == '\r') {
					state = FOUND_RNR;
				}else{
                    break;
				}
				//break;
			    //case FOUND_RNR:
				if (string[i++] == '\n') {
					return 1;
				}
				break;
			default:
				i++;
				break;
		}
	}
	return 0;
}


int http_client_perform(struct http_client *client)
{
	char * tempdata = NULL;
	struct http_client_list * templist;
	struct http_client_t * client_t = (struct http_client_t *)client;
	struct http_client_list * temp_cookie;
	const char * write_data;
	int pos = 0, ret, keep_state, try_cnt = 0, copied = 0;
	/* Check all mandatory */
	if (client_t->server.fd < 0) {
		http_debug("Perform with bad fd!");
		if (build_new_socket(client_t)) {
			return HTTP_CLIENT_BAD_SOCKET_ID;
		}
	}

	if (!client_t->ssl && client_t->server.https) {
		http_debug("Perform ssl_client_init...");
		client_t->ssl = ssl_client_init(client_t->server.fd);
	}

	/* Get temp buffer */
	tempdata = HTTP_CLIENT_MALLOC(HTTPCLIENT_MEM_SIZE);
	if (tempdata == NULL) {
		http_error("Cliet perform failed, cannnot malloc memory");
		return HTTP_CLIENT_NO_MEMORY;
	}

	/* Check http request type */
	if (client_t->request_type < HTTPCLIENT_REQUEST_GET ||
		client_t->request_type >= HTTPCLIENT_REQUEST_MAX) {
		http_error("Bad http request!");
		ret = HTTP_CLIENT_BAD_HTTP_REQUEST;
		goto done;
	}

again:
	if (client_t->server.name) http_debug("username: %s", client_t->server.name);
	if (client_t->server.password) http_debug("password: %s", client_t->server.password);
	/* Build http requeset message */
	pos = 0;
	memset(tempdata, 0, HTTPCLIENT_MEM_SIZE);
	if (client_t->http1_0) {
		pos = sprintf(tempdata, "%s %s HTTP/1.0\r\n", get_request_string(client_t), client_t->server.path);
	} else {
		pos = sprintf(tempdata, "%s %s HTTP/1.1\r\n", get_request_string(client_t), client_t->server.path);
	}
	if(client_t->server.use_ip6&&client_t->server.url_is_ip6_address){
		pos += sprintf(&tempdata[pos], "Referer: [%s]\r\n", client_t->server.host);
		pos += sprintf(&tempdata[pos], "Host: [%s]:%d\r\n", client_t->server.host, client_t->server.port);
	}else{
	pos += sprintf(&tempdata[pos], "Referer: %s\r\n", client_t->server.host);
	pos += sprintf(&tempdata[pos], "Host: %s:%d\r\n", client_t->server.host, client_t->server.port);
	}
	{
		char * ptr;
		int message_len;
		if (client_t->server.auth_type == HTTP_AUTH_TYPE_BASE) {
			char * temp_user_pwd = malloc(512);
			if (temp_user_pwd) {
				http_debug("User set use auth type is base");
				_snprintf(temp_user_pwd, 512, "%s:%s", client_t->server.name, client_t->server.password);
				http_client_base64_encode(temp_user_pwd, &ptr, &message_len,0);
				pos += sprintf(&tempdata[pos], "Authorization: %s %s\r\n",
					client_t->server.auth_type == HTTP_AUTH_TYPE_BASE ? "Basic" : "Digest", ptr);
				HTTP_CLIENT_FREE(ptr);
				free(temp_user_pwd);
				temp_user_pwd = NULL;
			}
		} else if (client_t->server.auth_value != NULL) {
			http_debug("Server set use auth type is digest");
			pos += sprintf(&tempdata[pos], "%s", client_t->server.auth_value);
		}
	}
	temp_cookie = client_t->cookie;
	while (temp_cookie) {
		pos += sprintf(&tempdata[pos], "%s", temp_cookie->data);
		http_debug("Add cookie header:");
		print_string(temp_cookie->data);
		temp_cookie = temp_cookie->next;
	}
	pos += sprintf(&tempdata[pos], "User-Agent: Marvell http client\r\n");
	pos += sprintf(&tempdata[pos], "Connection: keep-alive\r\n");
	pos += sprintf(&tempdata[pos], "Content-Length: %d\r\n", client_t->post_data == NULL ? 0 : client_t->post_length);
	templist = client_t->private_header;
	while (templist) {
		pos += sprintf(&tempdata[pos], "%s", templist->data);
		templist = templist->next;
		// Check if overflow, Add code here
		if (pos > (HTTPCLIENT_MEM_SIZE - 3)) {
			// ASSERT
			http_debug("http buffer overflow...");
		}
	}
	pos += sprintf(&tempdata[pos], "\r\n");

	/* Try send http message to server */
	copied = 0;
	#if 0
	// Send user data in first HTTP packet, if use this function open it
	if ((HTTPCLIENT_MEM_SIZE - pos) > 0) {
		copied = client_t->post_length > (HTTPCLIENT_MEM_SIZE - pos) ?
			(HTTPCLIENT_MEM_SIZE - pos) : client_t->post_length;
		memcpy(&tempdata[pos], client_t->post_data, copied);
		pos += copied;
	}
	#endif

	write_data = tempdata;
	http_debug("Need write header length %d", pos);
	print_string(write_data);
	while (pos > 0) {
		http_debug("Try send %d(%d)", pos, client_t->server.fd);
		if (client_t->server.https) {
			ret = ssl_client_write(&client_t->ssl->ssl, (const unsigned char *)write_data, pos);
		} else {
			ret = send(client_t->server.fd, write_data, pos, 0);
		}
		if (ret < 0) {
			http_error("Send socket failed(%d)@%s:%d!", ret, __func__, __LINE__);
			ret = HTTP_CLIENT_SEND_FAILED;
			goto done;
		}
		http_debug("Sent %d", ret);
		pos -= ret;
		write_data += ret;
	}
	/* Try send user data */
	pos = client_t->post_length - copied;
	write_data = client_t->post_data + copied;
	http_debug("Need write data length %d", pos);
	print_string(write_data);
	while (pos > 0) {
		http_debug("Try send %d(%d)", pos, client_t->server.fd);
		if (client_t->server.https) {
			ret = ssl_client_write(&client_t->ssl->ssl, (const unsigned char *)write_data, pos > 1024 ? 1024 : pos); // 1.5K
		} else {
			ret = send(client_t->server.fd, write_data, pos > 1024 ? 1024 : pos, 0);
		}
		if (ret < 0) {
			http_error("Send socket failed(%d)@%s:%d!", ret, __func__, __LINE__);
			ret = HTTP_CLIENT_SEND_FAILED;
			goto done;
		}
		http_debug("Sent %d", ret);
		pos -= ret;
		write_data += ret;
	}
	/* Receive and post data to user callback */
	keep_state = STATE_STARTLINE;
	pos = 0;

	/*roy add for memory leak issue 20160322*/
	if (client_t->response.result_code == 401 && (try_cnt == 1)) {
		if (client_t->server.auth_value){
			HTTP_CLIENT_FREE(client_t->server.auth_value);
			client_t->server.auth_value=NULL;
			http_debug("Clear old auth response!");
		}
	}
	

	/* Clear old response */
	clear_response(client_t);
	do {
		char *start;
		int tempdata_offset = 0;
		int result = 0;
		memset(tempdata, 0, HTTPCLIENT_MEM_SIZE);
retry_recv:
		if (client_t->server.https) {
			ret = ssl_client_read(&client_t->ssl->ssl, (unsigned char*)(tempdata + tempdata_offset), HTTPCLIENT_MEM_SIZE - tempdata_offset);
		} else {
			ret = recv(client_t->server.fd, tempdata + tempdata_offset, HTTPCLIENT_MEM_SIZE - tempdata_offset, 0);
		}
		if (ret <= 0) {
			http_error("Socket receive failed(%d) tempdata_offset(%d)!", ret,tempdata_offset);
			ret = HTTP_CLIENT_RECV_FAILED;
			goto done;
		}
		print_string(tempdata);
		start = tempdata;
		switch (keep_state) {
		case STATE_STARTLINE:
			if (check_http_header(tempdata, tempdata_offset + ret) == 0) {
				http_debug("data incomplete");
				tempdata_offset += ret;
				if (tempdata_offset >= HTTPCLIENT_MEM_SIZE) {
					http_debug("Retry give up, tempdata_offset[%d]",tempdata_offset);
					ret = HTTP_CLIENT_HEADER_SIZE_TOO_LARGE;
					goto done;
				} else {
					goto retry_recv;
				}
			}
			ret += tempdata_offset;
			tempdata_offset = 0;
			if (process_startline(client_t, &start, &ret)) {
				// Process start line failed!
				// Will assert in process_startline func
				ret = HTTP_CLIENT_GET_INVALID_HEADER;
				goto done;
			}
			keep_state = STATE_HEADER;
		case STATE_HEADER:
			if (process_httpheader(client_t, &start, &ret)) {
				// Process http header list failed!
				// Will assert in process_httpheader func
				ret = HTTP_CLIENT_PROCESS_HEADER_FAILED;
				goto done;
			}
			// Check if http header not parse complete, goto failed
			// goto done;
			if (client_t->response.response_data_length == 0) {
				// Handler empty http response
				break;
			}
			keep_state = STATE_DONE;
		default :
			// Raw data, post to user by callback
			if (client_t->rsp_cb && ret && client_t->response.result_code != 401) {
				int callbcak_ret = 
					client_t->rsp_cb(start, ret, client_t->response.response_data_length, client_t->rsp_cb_data);
				if (callbcak_ret < 0) {
					ret = HTTP_CLIENT_CALLBACK_FAILED;
					goto done;
				}
			}
			pos += ret;
			break;
		}
	} while (pos != client_t->response.response_data_length);
	// If server unauthorized, try touch again, and must receive and discard dirty data
	// Dirty data maybe is unautorized info html message.
	// So we need check state and do action after receive all data which maybe normal or dirty
	if (client_t->response.result_code == 401 && (try_cnt < 1)) {
		try_cnt++;
		http_debug("Server Unauthorized. Try again...");
		if (client_t->cookie) {
			http_client_list_destroy(client_t->cookie);
			client_t->cookie = NULL;
		}
		if (1) {//if (client_t->connection_close) {
			if (build_new_socket(client_t)){
		        ret = HTTP_CLIENT_CREATE_SOCKET_FAILED;
		        goto done;
			}
		}
		goto again;
	} else if (client_t->response.result_code == 307 || client_t->response.result_code == 302) {
		http_debug("Server has redirections");
		do {
			if (client_t->location == NULL) {
				http_error("ERROR, get response code 307, but not handle location");
				ret = HTTP_CLIENT_INVALID_LOCATION;
				break;
			}
			char *index = strstr(client_t->location,"\r\n");
			if(index){
                *index=0;
			}
			set_url(client_t, (void*)client_t->location);
			if (client_t->server.fd < 0) {
				http_error("Try build socket failed (for redirections)");
				ret = HTTP_CLIENT_BAD_SOCKET_ID;
				break;
			}
			goto again;
		} while (0);
		goto done;
	}
	ret = HTTP_CLIENT_OK;
done:
	if (tempdata) {
		HTTP_CLIENT_FREE(tempdata);
	}
	return ret;
}

void http_client_shutdown(struct http_client * client)
{
	struct http_client_t * client_t = (struct http_client_t *)client;
	http_debug("http_client_shutdown");

	//task_start_stop(0);
	
	clear_response(client_t);
	if (client_t->server.host) HTTP_CLIENT_FREE(client_t->server.host);
	if (client_t->server.path) HTTP_CLIENT_FREE(client_t->server.path);
	if (client_t->server.url) HTTP_CLIENT_FREE(client_t->server.url);
	if (client_t->server.auth_value) HTTP_CLIENT_FREE(client_t->server.auth_value);
	if (client_t->server.name) HTTP_CLIENT_FREE(client_t->server.name);
	if (client_t->server.password) HTTP_CLIENT_FREE(client_t->server.password);
	if (client_t->cookie) http_client_list_destroy(client_t->cookie);
	if (client_t->location) HTTP_CLIENT_FREE(client_t->location);
	if (client_t->ssl) {
		http_debug("http_client_shutdown: Clear ssl context");
		ssl_client_shutdown(client_t->ssl);
	}
	if (client_t->server.fd >= 0) {
		closesocket(client_t->server.fd);
		client_t->server.fd = INVALID_SOCKET;
	}
	HTTP_CLIENT_FREE(client);
}

int http_client_get_tcp_info(struct http_client_t * client_t)
{
    struct tcp_info info;
    int state=HTTP_TCP_NOT_ESTABLISHED;
    socklen_t len=0;
    memset(&info,0,sizeof(info));
    
    if(client_t == NULL){
        http_debug("%s: invalid http ctx",__func__);
        return state;
    }
    if(client_t->server.fd <= 0){
        http_debug("%s: invalid http socket",__func__);
        return state;
    }

    if(client_t->connection_close == 1){
        http_debug("%s:http will be closed by server side",__func__);
        return state;
    }
    
    len = sizeof(info);
    
    getsockopt(client_t->server.fd, IPPROTO_TCP, TCP_INFO, (void *)&info, &len);
    http_debug("%s:get IPPROTO_TCP state %d",__func__,info.tcpi_state);
    if(info.tcpi_state == ESTABLISHED)
    {
        state = HTTP_TCP_ESTABLISHED;
    }
    
    return state;
}

int http_client_getinfo(struct http_client * client, int tag, void * result)
{
	int err = HTTP_CLIENT_OK;
	struct http_client_t * client_t = (struct http_client_t *)client;

	switch (tag) {
		case HTTPCLIENT_GETINFO_RESPONSE_CODE:
			*(int *)result = client_t->response.result_code;
			break;
		case HTTPCLIENT_GETINFO_TCP_STATE:
		    *(int *)result = http_client_get_tcp_info(client_t);
			break; 
		default:
			err = HTTP_CLIENT_ERROR;
	}
	return err;
}

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
*  Filename:    httpclient_test.c
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
#include "osa.h"
#include "libhttpclient.h"
#include "httpclient_sys.h"
/*
static void * __malloc(int size)
{
	void * p = malloc(size);

	http_debug("[http_client_test]malloc @%p = 0x%p", __return_address(), p);
	return p;
}

static void __free(void * p)
{
    http_debug("[http_client_test]free @%p = 0x%p", __return_address(), p);
	free(p);
}
*/

#define PRINT_BUF_SIZE 65
static char print_buf[PRINT_BUF_SIZE];
static void print_string(const char * string)
{
	int len = strlen(string);
	int printed = 0;
	if (!string) return;
	while (printed != len) {
		if ((len - printed) > (PRINT_BUF_SIZE - 1)) {
			memcpy(print_buf, string + printed, (PRINT_BUF_SIZE - 1));
			printed += (PRINT_BUF_SIZE - 1);
			print_buf[printed] = '\0';
		} else {
			sprintf(print_buf, "%s", string + printed);
			printed = len;
		}

		http_debug("[http_client_test][value]%s", print_buf);
	}
}

static char * response_data = NULL;
static int response_len = 0;

/*
data: the data is received http context data
len: the length of http context data 
num: the http response header "Content-Length" value
cbdata: user private data
*/

static void response_cb(char * data, int len, int num, void *cbdata)
{
	char * temp;
	char *private_data=(char *)cbdata;

    http_debug("[http_client_test]Get private_data %s", private_data);

	http_debug("[http_client_test]Get data %d", len);

	temp = (void*)malloc(response_len + len);
	if (!temp) {
        http_debug("[http_client_test]Malloc failed...");
		return;
	}
	memset(temp, 0, response_len + len);
	if (response_data) {
		memcpy(temp, response_data, response_len);
		free(response_data);
	}
	memcpy(&temp[response_len], data, len);
	response_data = temp;
	response_len += len;
}

static void __http_client_test(void *arg)
{
    char private_data[]="hello world!";
    int response_code,trynum=0;
    struct http_client * client = NULL;
    struct http_client_list * header = NULL;
    OSATaskSleep(2000); // 10 SEC
    client = http_client_init();
    
	while (trynum<10) {
	    trynum++;
		OSATaskSleep(2000); // 10 SEC
		//http_client_setopt(client, HTTPCLIENT_OPT_PDP_CID, 0);              /*set PDP cid,if not set,using default PDP*/
		
        //http_client_setopt(client, HTTPCLIENT_OPT_BIND_PORT, 56521);            /*set tcp src port,if not set,using random port*/
        
		http_client_setopt(client, HTTPCLIENT_OPT_URL, "https://www.baidu.com");    /*set URL:support http/https and ip4/ip6*/
		http_client_setopt(client, HTTPCLIENT_OPT_HTTP1_0, 0);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/
		
		http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, response_cb);     /*response callback*/
		http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, private_data);           /*set user private data,*/
		http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);  /*set method,support GET/POST/PUT*/
		http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, NULL);          /*post data is http context*/
		http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, 0);           /*http context length*/
		http_client_setopt(client, HTTPCLIENT_OPT_AUTH_TYPE, HTTP_AUTH_TYPE_BASE);  /*auth type support base and digest*/
		http_client_setopt(client, HTTPCLIENT_OPT_AUTH_USERNAME, "admin");           /*username*/
		http_client_setopt(client, HTTPCLIENT_OPT_AUTH_PASSWORD, "admin");           /*password*/
#ifdef USER_PRIVATE_HEADER
        // Add private HTTP header
        header = http_client_list_append(header, "Content-Type: text/xml;charset=UTF-8\r\n");
        header = http_client_list_append(header, "SOAPAction:\r\n");
#endif
		/*set http private header,our http stack already support Referer/Host/Authorization/User-Agent/Connection/cookie/Content-Length,
		this header is other http header,ex: Content-Type..., we call it private header,*/
		http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

        http_client_getinfo(client, HTTPCLIENT_GETINFO_TCP_STATE, &response_code);
        http_debug("[http_client_test]Get tcp state %d", response_code);
        
        if(response_code != HTTP_TCP_ESTABLISHED){
            http_debug("[http_client_test]http tcp not connecting");
            http_client_shutdown(client); /*release http resources*/
            client = http_client_init();
            continue;
        }

		
		response_code=http_client_perform(client);    /*execute http send and recv*/
		if(response_code != HTTP_CLIENT_OK){
            http_debug("[http_client_test]http perform have some wrong[%d]",response_code);
            http_client_shutdown(client); /*release http resources*/
            client = http_client_init();
            continue;

		}
		
		http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
		if (response_code != 200 && response_code != 204) {
			http_error("[http_client_test]response is not 'HTTP OK'(%d)", response_code);
		}
		
		http_debug("[http_client_test]Get total data %d", response_len);
		print_string(response_data);
		if (response_data) free(response_data);
		response_data = NULL;
		response_len = 0;
		
#ifdef USER_PRIVATE_HEADER
        // Free private HTTP header
		if (header) {
			http_client_list_destroy(header);
			header = NULL;
		}
#endif
	}
	http_client_shutdown(client); /*release http resources*/
	
}


void http_client_test(void)
{
    OSTaskRef TaskRef = NULL;
    void* ptr_task_stack = NULL;
	http_debug("[http_client_test] test init...");
    ptr_task_stack = (void*)malloc(10240);
    if(ptr_task_stack==NULL){
        http_debug("[http_client_test] malloc failed...");
        return;
    }
    OSATaskCreate(&TaskRef, ptr_task_stack,
                10240, 80, "httpclient",
                __http_client_test, NULL);

}


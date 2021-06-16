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
*  Filename:    libhttpclient.h
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
#ifndef __LIBHTTPCLIENT_H__
#define __LIBHTTPCLIENT_H__

/*
buffer: the buffer is received http context data
size: the length of http context data 
nitems: the http response header "Content-Length" value
private_data: user private data
*/
typedef int (*client_response_cb)(char *buffer, int size, int nitems, void *private_data);
typedef void http_client;

//#define HTTP_CLIENT_ERROR -1
//#define HTTP_CLIENT_OK     0

#define HTTPCLIENT_MEM_SIZE 2048

enum {
	HTTPCLIENT_OPT_URL,
	HTTPCLIENT_OPT_METHOD,
	HTTPCLIENT_OPT_HTTP1_0,
	HTTPCLIENT_OPT_HTTPHEADER,
	HTTPCLIENT_OPT_POSTDATA,
	HTTPCLIENT_OPT_POSTLENGTH,
	HTTPCLIENT_OPT_RESPONSECB,
	HTTPCLIENT_OPT_RESPONSECB_DATA,
	HTTPCLIENT_OPT_AUTH_TYPE,
	HTTPCLIENT_OPT_AUTH_USERNAME,
	HTTPCLIENT_OPT_AUTH_PASSWORD,
	HTTPCLIENT_OPT_PDP_CID,
	HTTPCLIENT_OPT_BIND_PORT,
};

enum {
	HTTPCLIENT_GETINFO_RESPONSE_CODE,
	HTTPCLIENT_GETINFO_TCP_STATE,
};

enum {
	HTTPCLIENT_REQUEST_GET,
	HTTPCLIENT_REQUEST_POST,
	HTTPCLIENT_REQUEST_PUT,
	HTTPCLIENT_REQUEST_MAX
};

enum {
	HTTP_AUTH_TYPE_BASE = 1,
	HTTP_AUTH_TYPE_DIGEST
};

enum {
	HTTP_TCP_NOT_ESTABLISHED = 0,
	HTTP_TCP_ESTABLISHED
};

enum {
	HTTP_CLIENT_ERROR = -1,
	HTTP_CLIENT_OK = 0,
	HTTP_CLIENT_BAD_SOCKET_ID,
	HTTP_CLIENT_NO_MEMORY,
	HTTP_CLIENT_BAD_HTTP_REQUEST,
	HTTP_CLIENT_SEND_FAILED,
	HTTP_CLIENT_RECV_FAILED,
	HTTP_CLIENT_HEADER_SIZE_TOO_LARGE,
	HTTP_CLIENT_GET_INVALID_HEADER,
	HTTP_CLIENT_PROCESS_HEADER_FAILED,
	HTTP_CLIENT_CALLBACK_FAILED,
	HTTP_CLIENT_CREATE_SOCKET_FAILED,
	HTTP_CLIENT_INVALID_LOCATION,
};


struct http_client_list {
	char * data;
	struct http_client_list * next;
};

extern struct http_client * http_client_init(void);

extern void http_client_shutdown(struct http_client *);

extern struct http_client_list * http_client_list_append(struct http_client_list *, const char *);

extern void http_client_list_destroy(struct http_client_list *);

extern int http_client_setopt(struct http_client *, int, ...);

extern int http_client_perform(struct http_client *);

extern int http_client_getinfo(struct http_client *, int, void *);

#endif
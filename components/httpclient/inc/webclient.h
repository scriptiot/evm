/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-05-05     Bernard      the first version
 * 2013-06-10     Bernard      fix the slow speed issue when download file.
 * 2015-11-14     aozima       add content_length_remainder.
 * 2017-12-23     aozima       update gethostbyname to getaddrinfo.
 * 2018-01-04     aozima       add ipv6 address support.
 * 2018-07-26     chenyong     modify log information
 * 2018-08-07     chenyong     modify header processing
 */

#ifndef __WEBCLIENT_H__
#define __WEBCLIENT_H__


#include <stdio.h>
#include <string.h>
#include "rt_port.h"
#include "ssl_http_client.h"

#if defined(WEBCLIENT_USING_MBED_TLS) || defined(WEBCLIENT_USING_SAL_TLS)
#include <tls_client.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef web_malloc
#define web_malloc                     malloc
#endif

#ifndef web_calloc
#define web_calloc                     calloc
#endif

#ifndef web_realloc
#define web_realloc                    realloc
#endif

#ifndef web_free
#define web_free                       free
#endif

#ifndef web_strdup
#define web_strdup                     strdup
#endif

#define WEBCLIENT_SW_VERSION           "2.2.0"
#define WEBCLIENT_SW_VERSION_NUM       0x20200

#define WEBCLIENT_HEADER_BUFSZ         10240
#define WEBCLIENT_RESPONSE_BUFSZ       10240

enum WEBCLIENT_STATUS
{
    WEBCLIENT_OK,
    WEBCLIENT_ERROR,
    WEBCLIENT_TIMEOUT,
    WEBCLIENT_NOMEM,
    WEBCLIENT_NOSOCKET,
    WEBCLIENT_NOBUFFER,
    WEBCLIENT_CONNECT_FAILED,
    WEBCLIENT_CONNECT_SSL_FAILED,
    WEBCLIENT_DISCONNECT,
    WEBCLIENT_FILE_ERROR,

    WEBCLIENT_FIELDS_ADD_ERROR,

    WEBCLIENT_RESOLVE_ADDRESS_NOT_HTTP,
    WEBCLIENT_RESOLVE_ADDRESS_IPV6_ERROR,
    WEBCLIENT_RESOLVE_ADDRESS_LENGTH_ERROR,
    WEBCLIENT_RESOLVE_ADDRESS_MALLOC_ERROR,
    WEBCLIENT_RESOLVE_ADDRESS_GETADDRINFO_ERROR,

    WEBCLIENT_CONNECT_ADDRESS_ERROR,
    WEBCLIENT_CONNECT_FAILED_ERROR,

    WEBCLIENT_READ_LINE_ERROR,

    WEBCLIENT_GET_POSITION_ERROR,

    WEBCLIENT_POST_DATA_LENGTH_ERROR,
    WEBCLIENT_REQUEST_HEADER_ADD_ERROR,
    WEBCLIENT_REQUEST_HEADER_ADD_BUFFER_ERROR,

    WEBCLIENT_REQUEST_ERROR_1,
    WEBCLIENT_REQUEST_ERROR_2,
    WEBCLIENT_REQUEST_ERROR_3,
    WEBCLIENT_REQUEST_ERROR_4,
    WEBCLIENT_REQUEST_ERROR_5,
    WEBCLIENT_REQUEST_ERROR_6,
    WEBCLIENT_REQUEST_ERROR_7,
};

enum WEBCLIENT_METHOD
{
    WEBCLIENT_USER_METHOD,
    WEBCLIENT_GET,
    WEBCLIENT_POST,
};

struct  webclient_header
{
    char *buffer;
    size_t length;                      /* content header buffer size */

    size_t size;                        /* maximum support header size */
};

struct webclient_session
{
    struct webclient_header *header;    /* webclient response header information */
    int socket;
    int resp_status;

    char *host;                         /* server host */
    char *req_url;                      /* HTTP request address*/

    int chunk_sz;
    int chunk_offset;

    int content_length;
    size_t content_remainder;           /* remainder of content length */

    struct ssl_client * ssl;
    int is_https;

    rt_bool_t is_tls;                   /* HTTPS connect */
#ifdef WEBCLIENT_USING_MBED_TLS
    MbedTLSSession *tls_session;        /* mbedtls connect session */
#endif
};

/* create webclient session and set header response size */
struct webclient_session *webclient_session_create(size_t header_sz);

/* send HTTP GET request */
int webclient_get(struct webclient_session *session, const char *URI);
int webclient_get_position(struct webclient_session *session, const char *URI, int position);

/* send HTTP POST request */
int webclient_post(struct webclient_session *session, const char *URI, const void *post_data, size_t data_len);

/* close and release wenclient session */
int webclient_close(struct webclient_session *session);

int webclient_set_timeout(struct webclient_session *session, int millisecond);

/* send or receive data from server */
int webclient_read(struct webclient_session *session, void *buffer, size_t size);
int webclient_write(struct webclient_session *session, const void *buffer, size_t size);

/* webclient GET/POST header buffer operate by the header fields */
int webclient_header_fields_add(struct webclient_session *session, const char *fmt, ...);
const char *webclient_header_fields_get(struct webclient_session *session, const char *fields);

/* send HTTP POST/GET request, and get response data */
int webclient_response(struct webclient_session *session, void **response, size_t *resp_len);
int webclient_request(const char *URI, const char *header, const void *post_data, size_t data_len, void **response, size_t *resp_len);
int webclient_request_header_add(char **request_header, const char *fmt, ...);
int webclient_resp_status_get(struct webclient_session *session);
int webclient_content_length_get(struct webclient_session *session);

#ifdef RT_USING_DFS
/* file related operations */
int webclient_get_file(const char *URI, const char *filename);
int webclient_post_file(const char *URI, const char *filename, const char *form_data);
#endif

#ifdef  __cplusplus
    }
#endif

#endif

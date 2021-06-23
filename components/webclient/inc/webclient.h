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

#ifdef WEBCLIENT_USING_TLS
#include <tls_client.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __linux__

#ifndef web_malloc
#define web_malloc malloc
#endif

#ifndef web_free
#define web_free free
#endif

#ifndef closesocket
#define closesocket close
#endif

#elif FREERTOS

#ifndef web_malloc
#define web_malloc pvPortMalloc
#endif

#ifndef web_free
#define web_free vPortFree
#endif

#ifndef strdup
#define strdup web_strdup
#endif

#else
#define web_malloc NULL
#define web_free NULL
#endif

#define WEBCLIENT_SW_VERSION "2.0.1"
#define WEBCLIENT_SW_VERSION_NUM 0x20001

#define WEBCLIENT_HEADER_BUFSZ 4096
#define WEBCLIENT_RESPONSE_BUFSZ 4096

    enum WEBCLIENT_STATUS
    {
        WEBCLIENT_OK,
        WEBCLIENT_ERROR,
        WEBCLIENT_TIMEOUT,
        WEBCLIENT_NOMEM,
        WEBCLIENT_NOSOCKET,
        WEBCLIENT_NOBUFFER,
        WEBCLIENT_CONNECT_FAILED,
        WEBCLIENT_DISCONNECT,
        WEBCLIENT_FILE_ERROR,
    };

    enum WEBCLIENT_METHOD
    {
        WEBCLIENT_USER_METHOD,
        WEBCLIENT_GET,
        WEBCLIENT_POST,
    };

    struct webclient_header
    {
        char *buffer;
        size_t length; /* content header buffer size */
        size_t size; /* maximum support header size */
    };

    struct webclient_session
    {
        struct webclient_header *header; /* webclient response header information */
        int socket;
        int resp_status;

        char *host;    /* server host */
        char *req_url; /* HTTP request address*/

        int chunk_sz;
        int chunk_offset;

        int content_length;
        size_t content_remainder; /* remainder of content length */

#ifdef WEBCLIENT_USING_TLS
        MbedTLSSession *tls_session; /* mbedtls connect session */
#endif
    };

    /* create webclient session and set header response size */
    struct webclient_session *webclient_session_create(size_t header_sz);

    /* send HTTP GET request */
    int webclient_get(struct webclient_session *session, const char *URI);
    int webclient_get_position(struct webclient_session *session, const char *URI, int position);

    /* send HTTP POST request */
    int webclient_post(struct webclient_session *session, const char *URI, const char *post_data);

    /* close and release wenclient session */
    int webclient_close(struct webclient_session *session);

    int webclient_set_timeout(struct webclient_session *session, int millisecond);

    /* send or receive data from server */
    int webclient_read(struct webclient_session *session, unsigned char *buffer, size_t size);
    int webclient_write(struct webclient_session *session, const unsigned char *buffer, size_t size);

    /* webclient GET/POST header buffer operate by the header fields */
    int webclient_header_fields_add(struct webclient_session *session, const char *fmt, ...);
    const char *webclient_header_fields_get(struct webclient_session *session, const char *fields);

    /* send HTTP POST/GET request, and get response data */
    int webclient_response(struct webclient_session *session, unsigned char **response);
    int webclient_request(const char *URI, const char *header, const char *post_data, unsigned char **response);
    int webclient_resp_status_get(struct webclient_session *session);
    int webclient_content_length_get(struct webclient_session *session);

#ifdef RT_USING_DFS
    /* file related operations */
    int webclient_get_file(const char *URI, const char *filename);
    int webclient_post_file(const char *URI, const char *filename, const char *form_data);
#endif

    /**
 * connect to http server.
 *
 * @param session webclient session
 * @param URI the input server URI address
 *
 * @return <0: connect failed or other error
 *         =0: connect success
 */
    int webclient_connect(struct webclient_session *session, const char *URI);

    /**
 * @brief  send http(s) header 
 * @param  [in] session
 * @param  [in] method : enum WEBCLIENT_METHOD
 * @return WEBCLIENT_OK if ok
 */
    int webclient_send_header(struct webclient_session *session, int method);

    /**
 * @brief  resolve server response data.
 * @param  [in] session : webclient session
 * @return 200 if ok(HTTP status code) 
 */
    int webclient_handle_response(struct webclient_session *session);

    void *http_wget(const char *url, size_t *size);

    const char *get_longitude();
    const char *get_latitude();

#ifdef __cplusplus
}
#endif

#endif

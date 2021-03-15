/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-26     chenyong     modify log information
 */

#include <stdint.h>
#include <stdlib.h>

#include "webclient.h"

#ifdef RT_USING_DFS
#include <dfs_posix.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "web.file"
#ifdef WEBCLIENT_DEBUG
#define DBG_LEVEL DBG_LOG
#else
#define DBG_LEVEL DBG_INFO
#endif /* WEBCLIENT_DEBUG */
#define DBG_COLOR
#include <rtdbg.h>

#define TAG "WEBCLIENT"

/**
 * send GET request and store response data into the file.
 *
 * @param URI input server address
 * @param filename store response date to filename
 *
 * @return <0: GET request failed
 *         =0: success
 */
int webclient_get_file(const char *URI, const char *filename)
{
    int fd = -1, rc = WEBCLIENT_OK;
    size_t offset;
    size_t length, total_length = 0;
    unsigned char *ptr = NULL;
    struct webclient_session *session = NULL;
    int resp_status = 0;

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == NULL)
    {
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }

    if ((resp_status = webclient_get(session, URI)) != 200)
    {
        LOGE(TAG, "get file failed, wrong response: %d.", resp_status);
        rc = -WEBCLIENT_ERROR;
        goto __exit;
    }

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        LOGE(TAG, "get file failed, open file(%s) error.", filename);
        rc = -WEBCLIENT_ERROR;
        goto __exit;
    }

    ptr = (unsigned char *)web_malloc(WEBCLIENT_RESPONSE_BUFSZ);
    if (ptr == NULL)
    {
        LOGE(TAG, "get file failed, no memory for response buffer.");
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }

    if (session->content_length < 0)
    {
        while (1)
        {
            length = webclient_read(session, ptr, WEBCLIENT_RESPONSE_BUFSZ);
            if (length > 0)
            {
                write(fd, ptr, length);
                total_length += length;
                LOG_RAW(">");
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for (offset = 0; offset < (size_t)session->content_length;)
        {
            length = webclient_read(session, ptr,
                                    session->content_length - offset > WEBCLIENT_RESPONSE_BUFSZ ? WEBCLIENT_RESPONSE_BUFSZ : session->content_length - offset);

            if (length > 0)
            {
                write(fd, ptr, length);
                total_length += length;
                LOG_RAW(">");
            }
            else
            {
                break;
            }

            offset += length;
        }
    }

    if (total_length)
    {
        LOGI(TAG, "save %d bytes.", total_length);
    }

__exit:
    if (fd >= 0)
    {
        close(fd);
    }

    if (session != NULL)
    {
        webclient_close(session);
    }

    if (ptr != NULL)
    {
        web_free(ptr);
    }

    return rc;
}

/**
 * post file to http server.
 *
 * @param URI input server address
 * @param filename post data filename
 * @param form_data  form data
 *
 * @return <0: POST request failed
 *         =0: success
 */
int webclient_post_file(const char *URI, const char *filename,
                        const char *form_data)
{
    size_t length;
    char boundary[60];
    int fd = -1, rc = WEBCLIENT_OK;
    char *header = NULL, *header_ptr;
    unsigned char *buffer = NULL, *buffer_ptr;
    struct webclient_session *session = NULL;

    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        LOGI(TAG, "post file failed, open file(%s) error.", filename);
        rc = -WEBCLIENT_FILE_ERROR;
        goto __exit;
    }

    /* get the size of file */
    length = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    buffer = (unsigned char *)web_malloc(WEBCLIENT_RESPONSE_BUFSZ);
    if (buffer == NULL)
    {
        LOGI(TAG, "post file failed, no memory for response buffer.");
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }

    header = (char *)web_malloc(WEBCLIENT_HEADER_BUFSZ);
    if (header == NULL)
    {
        LOGI(TAG, "post file failed, no memory for header buffer.");
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }
    header_ptr = header;

    /* build boundary */
    snprintf(boundary, sizeof(boundary), "----------------------------%012d", rt_tick_get());

    /* build encapsulated mime_multipart information*/
    buffer_ptr = buffer;
    /* first boundary */
    buffer_ptr += snprintf((char *)buffer_ptr,
                           WEBCLIENT_RESPONSE_BUFSZ - (buffer_ptr - buffer), "--%s\r\n", boundary);
    buffer_ptr += snprintf((char *)buffer_ptr,
                           WEBCLIENT_RESPONSE_BUFSZ - (buffer_ptr - buffer),
                           "Content-Disposition: form-data; %s\r\n", form_data);
    buffer_ptr += snprintf((char *)buffer_ptr,
                           WEBCLIENT_RESPONSE_BUFSZ - (buffer_ptr - buffer),
                           "Content-Type: application/octet-stream\r\n\r\n");
    /* calculate content-length */
    length += buffer_ptr - buffer;
    length += strlen(boundary) + 6; /* add the last boundary */

    /* build header for upload */
    header_ptr += snprintf(header_ptr,
                           WEBCLIENT_HEADER_BUFSZ - (header_ptr - header),
                           "Content-Length: %d\r\n", length);
    header_ptr += snprintf(header_ptr,
                           WEBCLIENT_HEADER_BUFSZ - (header_ptr - header),
                           "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == NULL)
    {
        rc = -WEBCLIENT_NOMEM;
        goto __exit;
    }

    session->header->buffer = web_strdup(header);

    rc = webclient_post(session, URI, NULL);
    if (rc < 0)
    {
        goto __exit;
    }

    /* send mime_multipart */
    webclient_write(session, buffer, buffer_ptr - buffer);

    /* send file data */
    while (1)
    {
        length = read(fd, buffer, WEBCLIENT_RESPONSE_BUFSZ);
        if (length <= 0)
        {
            break;
        }

        webclient_write(session, buffer, length);
    }

    /* send last boundary */
    snprintf((char *)buffer, WEBCLIENT_RESPONSE_BUFSZ, "\r\n--%s--\r\n", boundary);
    webclient_write(session, buffer, strlen(boundary) + 6);

__exit:
    if (fd >= 0)
    {
        close(fd);
    }

    if (session != NULL)
    {
        webclient_close(session);
    }

    if (buffer != NULL)
    {
        web_free(buffer);
    }

    if (header != NULL)
    {
        web_free(header);
    }

    return 0;
}

int wget(int argc, char **argv)
{
    if (argc != 3)
    {
        rt_kprintf("Please using: wget <URI> <filename>");
        return -1;
    }

    webclient_get_file(argv[1], argv[2]);
    return 0;
}

#endif /* RT_USING_DFS */

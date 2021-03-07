
#include <stdint.h>
#include <stdlib.h>

#include "webclient.h"

#define TAG "HTTP"
#define HTTP_HEADER_SIZE (1024)
#define HTTP_RECV_MAX_SIZE (10 * 1024)

/**
 * @param  [in] url : http url
 * @param  [out] content : recv buffer for http response body
 * @param  [in] size : size of the content
 * @return total read bytes
 */
void *http_wget(const char *url, size_t *size)
{
    int content_length = 0;
    int total_read = 0;
    int resp_status = 0;

    uint8_t *out_content = NULL;

    struct webclient_session *session = NULL;

    if (!url || !size)
    {
        return;
    }

    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session)
    {
        LOGE(TAG, "session create fail\n");
        goto err;
    }

    webclient_set_timeout(session, 500); //set timeout to 500ms and retry

    /* send GET request by default header */
    if ((resp_status = webclient_get(session, url)) != 200)
    {
        LOGD(TAG, "webclient GET request failed, response(%d) error.\n", resp_status);
        goto err;
    }

    content_length = webclient_content_length_get(session);
    LOGD(TAG, "%s Conten-Length=%d", __FUNCTION__, content_length);
    if (content_length <= 0)
    {
        LOGD(TAG, "HTTP Chuncked Mode");

        int cur_read = 0;
        size_t buf_size = 0;
        while (total_read < HTTP_RECV_MAX_SIZE)
        {
            /* read result */
            buf_size += 512;
            out_content = aos_realloc(out_content, buf_size);
            cur_read = webclient_read(session, out_content + total_read, buf_size);
            if (cur_read > 0)
                total_read += cur_read;
            if (cur_read <= buf_size)
                break;
        }

        if (total_read >= HTTP_RECV_MAX_SIZE)
        {
            LOGE(TAG, "total_read is too large: %d\n", total_read);
            goto err;
        }
    }
    else
    {
        if (content_length >= HTTP_RECV_MAX_SIZE)
        {
            LOGE(TAG, "content length is too large: %d\n", content_length);
            goto err;
        }

        out_content = web_malloc(content_length);

        total_read = webclient_read(session, out_content, content_length);
        if (total_read != content_length)
        {
            LOGE(TAG, "not equal, need read = %d, bytes_read = %d\n", content_length, total_read);
            goto err;
        }
    }

    *size = total_read;
    webclient_close(session);
    return out_content;

err:
    if (out_content)
        free(out_content);

    webclient_close(session);

    return NULL;
}

#ifdef CONFIG_EVM_MODULE_HTTP
#include "evm_module.h"
#include "webclient.h"
#include <pthread.h>

#define HTTP_HEADER_SIZE (1024)
#define HTTP_RECV_MAX_SIZE (10 * 1024)

typedef struct _http_client_t
{
    struct webclient_session *session;
    int obj_id;
    // char *buffer;
    // int method;
    pthread_t pid;
} _http_client_t;

static void _http_response_thread(_http_client_t *client)
{
    if (client == NULL)
        return;
    int total_read = 0;
    int resp_status = 0;
    int content_length = webclient_content_length_get(client->session);
    evm_val_t *obj = evm_module_registry_get(evm_runtime, client->obj_id);
    if (evm_is_undefined(obj) || evm_is_null(obj) || obj == NULL)
        return;

    printf("%s Conten-Length=%d\n", __FUNCTION__, content_length);
    uint8_t *out_content = NULL;
    printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
    if (content_length <= 0)
    {
        printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
        int cur_read = 0;
        size_t buf_size = 0;
        uint8_t *old_content = NULL;
        printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
        evm_val_t *args;
        while (total_read < HTTP_RECV_MAX_SIZE)
        {
            printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
            buf_size += 512;
            old_content = out_content;
            printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
            out_content = evm_malloc(buf_size);
            memcpy(out_content, old_content, buf_size);
            evm_free(old_content);
            cur_read = webclient_read(client->session, out_content + total_read, buf_size);
            if (cur_read > 0)
            {
                total_read += cur_read;
                args = evm_buffer_create(evm_runtime, buf_size);
                if (args)
                {
                    memcpy(evm_buffer_addr(args), out_content + total_read, buf_size);
                    evm_module_event_emit(evm_runtime, obj, "data", 1, args);
                    evm_pop(evm_runtime);
                }
            }
            if (cur_read <= buf_size)
                break;
        }
        printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);

        if (total_read >= HTTP_RECV_MAX_SIZE)
        {
            printf("total_read is too large: %d\n", total_read);
            goto err;
        }
    }
    else
    {
        printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);
        if (content_length >= HTTP_RECV_MAX_SIZE)
        {
            printf("content length is too large: %d\n", content_length);
            goto err;
        }

        out_content = evm_malloc(content_length);
        total_read = webclient_read(client->session, out_content, content_length);
        if (total_read != content_length)
        {
            printf("not equal, need read = %d, bytes_read = %d\n", content_length, total_read);
            goto err;
        }
        evm_val_t *args = evm_buffer_create(evm_runtime, total_read);
        if (args)
        {
            memcpy(evm_buffer_addr(args), out_content, total_read);
            evm_module_event_emit(evm_runtime, obj, "data", 1, args);
            evm_pop(evm_runtime);
        }
    }
    printf("Bug is here: %s, line: %s\n", __FUNCTION__, __LINE__);

err:
    if (out_content)
        evm_free(out_content);

    webclient_close(client->session);
    if (client)
        evm_free(client);
}

//http.createServer([options][, requestListener])
static evm_val_t evm_module_http_createServer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // http server，由于是mcu场景，暂不考虑实现
    return EVM_VAL_UNDEFINED;
}

//request.abort()
static evm_val_t evm_module_http_request_abort(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//request.end([data][, callback])
//完成发送请求。 如果部分请求主体还未发送，则将它们刷新到流中。 如果请求被分块，则发送终止符 '0\r\n\r\n'。
static evm_val_t evm_module_http_request_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//request.setTimeout(ms[, callback])
static evm_val_t evm_module_http_request_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//request.on(event, callback)
static evm_val_t evm_module_http_request_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_string(v) || !evm_is_script(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    _http_client_t *client = (_http_client_t *)evm_object_get_ext_data(p);
    if (!client)
        return EVM_VAL_UNDEFINED;

    if (client->obj_id < 1)
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
    return EVM_VAL_UNDEFINED;
}

//request.write(data[, callback])
static evm_val_t evm_module_http_request_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v) || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;
    webclient_write(session, evm_buffer_addr(v), evm_buffer_len(v));
    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, NULL, 0);
}

static evm_val_t evm_module_http_client_new_request(evm_t *e)
{
    evm_val_t *obj = evm_object_create(e, GC_DICT, 5, 0);
    if (obj)
    {
        evm_prop_append(e, obj, "abort", evm_mk_native((intptr_t)evm_module_http_request_abort));
        evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_http_request_end));
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_http_request_on));
        evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_http_request_setTimeout));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_http_request_write));

        _http_client_t *client = evm_malloc(sizeof(_http_client_t));
        if (client)
        {
            evm_object_set_ext_data(obj, (intptr_t)client);
        }
        client->obj_id = evm_module_registry_add(e, obj);
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

//response.abort()
static evm_val_t evm_module_http_response_abort(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//response.on(event, callback)
static evm_val_t evm_module_http_response_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_string(v) || !evm_is_script(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    _http_client_t *client = (_http_client_t *)evm_object_get_ext_data(p);
    if (!client)
        return EVM_VAL_UNDEFINED;

    if (client->obj_id < 1)
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
    return EVM_VAL_UNDEFINED;
}

//response.end([data][, callback])
static evm_val_t evm_module_http_response_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // 此方法向服务器发出信号，表明已发送所有响应头和主体
    return EVM_VAL_UNDEFINED;
}

//response.getHeader(name)
static evm_val_t evm_module_http_response_getHeader(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    return evm_mk_foreign_string(webclient_header_fields_get(session, evm_2_string(v)));
}

//response.removeHeader(name)
static evm_val_t evm_module_http_response_removeHeader(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//response.setHeader(name, value)
static evm_val_t evm_module_http_response_setHeader(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v) || !evm_is_string(v + 1))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    webclient_header_fields_add(session, evm_2_string(v), evm_2_string(v + 1));
    return EVM_VAL_UNDEFINED;
}

//response.setTimeout(ms, cb)
static evm_val_t evm_module_http_response_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//response.write(data[, callback])
static evm_val_t evm_module_http_response_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v) || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;
    webclient_write(session, evm_buffer_addr(v), evm_buffer_len(v));
    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, NULL, 0);
}

//response.writeHead(statusCode[, statusMessage][, headers])
static evm_val_t evm_module_http_response_writeHead(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_http_client_new_response(evm_t *e)
{
    evm_val_t *obj = evm_object_create(e, GC_DICT, 6, 0);
    if (obj)
    {
        evm_prop_append(e, obj, "abort", evm_mk_native((intptr_t)evm_module_http_response_abort));
        evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_http_response_end));
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_http_response_on));
        evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_http_response_setTimeout));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_http_response_write));
        evm_prop_append(e, obj, "writeHead", evm_mk_native((intptr_t)evm_module_http_response_writeHead));

        _http_client_t *client = evm_malloc(sizeof(_http_client_t));
        if (client)
        {
            evm_object_set_ext_data(obj, (intptr_t)client);
        }
        client->obj_id = evm_module_registry_add(e, obj);
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

//http.request(options[, callback])
static evm_val_t evm_module_http_request(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t *host = evm_prop_get(e, v, "host", 0);
    if (evm_is_undefined(host) || !evm_is_string(host))
        host = evm_prop_get(e, v, "hostname", 0);
    if (evm_is_undefined(host))
        return EVM_VAL_UNDEFINED;

    evm_val_t *port = evm_prop_get(e, v, "port", 0);
    if (evm_is_undefined(port) || !evm_is_number(port))
        return EVM_VAL_UNDEFINED;

    evm_val_t *method = evm_prop_get(e, v, "method", 0);
    if (evm_is_undefined(method) || !evm_is_string(method))
        return EVM_VAL_UNDEFINED;

    evm_val_t *path = evm_prop_get(e, v, "path", 0);
    if (evm_is_undefined(path) || !evm_is_string(path))
        return EVM_VAL_UNDEFINED;

    evm_val_t *headers = evm_prop_get(e, v, "headers", 0);
    if (evm_is_undefined(headers) || !evm_is_object(headers))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == NULL)
    {
        return EVM_VAL_UNDEFINED;
    }

    uint32_t length = evm_prop_len(headers);
    for (uint32_t index = 0; index < length; index++)
    {
        evm_hash_t key = evm_prop_get_key_by_index(e, headers, index);
        if (key == EVM_INVALID_HASH)
        {
            break;
        }
        evm_val_t *value = evm_prop_get_by_key(e, headers, key, index);
        const char *name = evm_string_get(e, key);
        if (evm_is_integer(value))
            webclient_header_fields_add(session, "%s: %d\r\n", name, value);
        else
            webclient_header_fields_add(session, "%s: %s\r\n", name, value);
    }

    uint32_t len = evm_string_len(host) + 5 + evm_string_len(path);
    evm_val_t *url = evm_heap_string_create(e, "", len);

    sprintf(evm_heap_string_addr(url), "%s:%d%s", evm_2_string(host), evm_2_integer(port), evm_2_string(path));

    evm_val_t *args;
    evm_val_t *result = evm_module_http_client_new_request(e);
    int status_code = -1;
    if (strcmp(evm_2_string(method), "get") == 0 || strcmp(evm_2_string(method), "GET") == 0)
    {
        status_code = webclient_get(session, evm_2_string(url));
    }
    else
    {
        status_code = webclient_post(session, evm_2_string(url), NULL);
    }

    if (status_code != 200)
    {
        args = evm_mk_null();
    }
    else
    {
        args = evm_module_http_client_new_response(e);
        _http_client_t *client = (_http_client_t *)evm_object_get_ext_data(args);
        client->session = session;

        pthread_create(&client->pid, NULL, (void *)_http_response_thread, client);
    }

    if (evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, args, 1);

    return *result;
}

//http.get(options[, callback])
static evm_val_t evm_module_http_get(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    char *host;
    evm_val_t *h = evm_prop_get(e, v, "host", 0);
    if (evm_is_string(h))
        host = evm_2_string(h);
    if (evm_is_undefined(h) || evm_is_null(h))
    {
        h = evm_prop_get(e, v, "hostname", 0);
        if (evm_is_string(h))
            host = evm_2_string(h);
    }

    uint16_t port = 80;
    evm_val_t *pt = evm_prop_get(e, v, "port", 0);
    if (evm_is_number(pt))
        port = evm_2_integer(pt);

    char *path;
    evm_val_t *pa = evm_prop_get(e, v, "path", 0);
    if (evm_is_string(pa))
        path = evm_2_string(pa);

    evm_val_t *headers = evm_prop_get(e, v, "headers", 0);
    if (evm_is_undefined(headers) || !evm_is_object(headers))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == NULL)
    {
        return EVM_VAL_UNDEFINED;
    }

    uint32_t length = evm_prop_len(headers);
    for (uint32_t index = 0; index < length; index++)
    {
        evm_hash_t key = evm_prop_get_key_by_index(e, headers, index);
        if (key == EVM_INVALID_HASH)
        {
            break;
        }

        evm_val_t *value = evm_prop_get_by_key(e, headers, key, index);
        const char *name = evm_string_get(e, key);
        if (evm_is_number(value))
        {
            webclient_header_fields_add(session, "%s: %d\r\n", name, evm_2_integer(value));
        }
        else
        {
            webclient_header_fields_add(session, "%s: %s\r\n", name, evm_2_string(value));
        }
    }

    uint32_t len = evm_string_len(h) + 5 + evm_string_len(pa);
    evm_val_t *url = evm_heap_string_create(e, "", len);
    sprintf(evm_heap_string_addr(url), "%s:%d%s", host, port, path);

    evm_val_t *args;
    evm_val_t *result = evm_module_http_client_new_request(e);
    _http_client_t *request = (_http_client_t *)evm_object_get_ext_data(result);

    int status_code = webclient_get(session, evm_2_string(url));
    if (status_code != 200)
    {
        args = evm_mk_null();
    }
    else
    {
        evm_val_t *obj = evm_object_create(e, GC_DICT, 6, 0);
        if (obj == NULL)
            return EVM_VAL_UNDEFINED;

        evm_prop_append(e, obj, "abort", evm_mk_native((intptr_t)evm_module_http_response_abort));
        evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_http_response_end));
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_http_response_on));
        evm_prop_append(e, obj, "setTimeout", evm_mk_native((intptr_t)evm_module_http_response_setTimeout));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_http_response_write));
        evm_prop_append(e, obj, "writeHead", evm_mk_native((intptr_t)evm_module_http_response_writeHead));

        _http_client_t *client = evm_malloc(sizeof(_http_client_t));
        if (client == NULL)
            return EVM_VAL_UNDEFINED;

        client->obj_id = evm_module_registry_add(e, obj);
        client->session = session;

        pthread_create(&client->pid, NULL, (void *)_http_response_thread, client);
        printf("Bug is here: %s, line is: %d\n", __func__, __LINE__);
        evm_object_set_ext_data(obj, (intptr_t)client);
        args = obj;
    }

    if (evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, args, 1);

    request->session = session;
    return *result;
}

//server.on(event, callback)
static evm_val_t evm_module_http_server_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // 配合createServer使用
    return EVM_VAL_UNDEFINED;
}

//server.listen(port[, hostname][, backlog][, callback])
static evm_val_t evm_module_http_server_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // 配合createServer使用，createServer未实现，这里也就不实现
    return EVM_VAL_UNDEFINED;
}

//server.close([callback])
static evm_val_t evm_module_http_server_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;
    webclient_close(session);
    if (argc > 0 && evm_is_script(v))
        evm_run_callback(e, v, &e->scope, NULL, 0);
}

//server.setTimeout(ms[, callback])
static evm_val_t evm_module_http_server_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 0 || !evm_is_number(v))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = (struct webclient_session *)evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    webclient_set_timeout(session, evm_2_integer(v));

    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, NULL, 0);

    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_http(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"createServer", evm_mk_native((intptr_t)evm_module_http_createServer)},
        {"request", evm_mk_native((intptr_t)evm_module_http_request)},
        {"get", evm_mk_native((intptr_t)evm_module_http_get)},
        {NULL, NULL}};
    evm_module_create(e, "http", builtin);
    return e->err;
}
#endif

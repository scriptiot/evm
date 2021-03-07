#ifdef CONFIG_EVM_MODULE_HTTP
#include "evm_module.h"
#include <FreeRTOS.h>
#include <timers.h>
#include "webclient.h"

#define HTTP_HEADER_SIZE (1024)
#define HTTP_RECV_MAX_SIZE (10 * 1024)

typedef struct _http_client_t
{
    struct webclient_session *session;
    int obj_id;
} _http_client_t;

static void _http_client_thread(_http_client_t *client)
{
    int total_read = 0;
    int resp_status = 0;
    int content_length = webclient_content_length_get(client->session);
    // printf("%s Conten-Length=%d\n", __FUNCTION__, content_length);
    uint8_t *out_content = NULL;
    if (content_length <= 0)
    {
        printf("HTTP Chuncked Mode\n");

        int cur_read = 0;
        size_t buf_size = 0;
        uint8_t *old_content = NULL;
        while (total_read < HTTP_RECV_MAX_SIZE)
        {
            /* read result */
            buf_size += 512;
            old_content = out_content;
            out_content = evm_malloc(buf_size);
            memcpy(out_content, old_content, buf_size);
            evm_free(old_content);
            cur_read = webclient_read(client->session, out_content + total_read, buf_size);
            if (cur_read > 0)
                total_read += cur_read;
            if (cur_read <= buf_size)
                break;
        }

        if (total_read >= HTTP_RECV_MAX_SIZE)
        {
            printf("total_read is too large: %d\n", total_read);
            goto err;
        }
    }
    else
    {
        if (content_length >= HTTP_RECV_MAX_SIZE)
        {
            printf("content length is too large: %d\n", content_length);
            goto err;
        }

        out_content = web_malloc(content_length);

        total_read = webclient_read(client->session, out_content, content_length);
        if (total_read != content_length)
        {
            printf("not equal, need read = %d, bytes_read = %d\n", content_length, total_read);
            goto err;
        }
    }

    evm_val_t *obj = evm_module_registry_get(evm_runtime, client->obj_id);
    if (obj)
    {
        evm_val_t *args = evm_buffer_create(evm_runtime, total_read);
        if (args)
        {
            memcpy(evm_buffer_addr(args), out_content, total_read);
            evm_module_event_emit(evm_runtime, obj, "data", 1, args);
            evm_pop(evm_runtime);
        }
    }

err:
    if (out_content)
        evm_free(out_content);
    if (client)
        evm_free(client);
    webclient_close(client->session);
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
static evm_val_t evm_module_http_request_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _http_client_t *client = (_http_client_t *)evm_object_get_ext_data(p);
    if (!client)
        return EVM_VAL_UNDEFINED;
    taskENTER_CRITICAL();
    xTaskCreate(_http_client_thread, "http-client-task", 512, client, 13, NULL);
    taskEXIT_CRITICAL();
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

static evm_val_t evm_module_http_client_new(e)
{
    evm_val_t *obj = evm_object_create(e, GC_DICT, 9, 0);
    if (obj)
    {
        evm_prop_append(e, obj, "abort", evm_mk_native((intptr_t)evm_module_http_request_abort));
        evm_prop_append(e, obj, "end", evm_mk_native((intptr_t)evm_module_http_request_end));
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

    uint32_t length = evm_prop_len(v);
    for (uint32_t index = 0; index < length; index++)
    {
        evm_hash_t key = evm_prop_get_key_by_index(e, v, index);
        if (key == EVM_INVALID_HASH)
        {
            break;
        }
        evm_val_t *v = evm_prop_get_by_index(e, key, index);
        const char *name = evm_string_get(e, key);
        if (evm_is_string(v))
            webclient_header_fields_add(session, "%s: %s\r\n", name);
        else
            webclient_header_fields_add(session, "%s: %d\r\n", name);
    }

    uint32_t len = evm_string_len(host) + 5 + evm_string_len(path);
    evm_val_t *url = evm_heap_string_create(e, len, NULL);
    sprintf(evm_heap_string_addr(url), "%s:%d%s", host, port, path);

    evm_val_t *args;
    if (strcmp(evm_2_string(method), "get") == 0 || strcmp(evm_2_string(method), "GET") == 0)
    {
        if (webclient_get(session, evm_2_string(url)) != 200)
        {
            args = evm_mk_foreign_string("http status code not 200");
            if (evm_is_script(v + 1))
                evm_run_callback(e, v + 1, &e->scope, args, 1);
            return EVM_VAL_UNDEFINED;
        }
        args = evm_module_http_client_new(e);
        if (args && evm_is_script(v + 1))
            evm_run_callback(e, v + 1, &e->scope, args, 1);
        return EVM_VAL_UNDEFINED;
    }
    else
    {
        if (webclient_post(session, evm_2_string(url), NULL) != 200)
        {
            args = evm_mk_foreign_string("http status code not 200");
            if (evm_is_script(v + 1))
                evm_run_callback(e, v + 1, &e->scope, args, 1);
            return EVM_VAL_UNDEFINED;
        }
        args = evm_module_http_client_new(e);
        if (args && evm_is_script(v + 1))
            evm_run_callback(e, v + 1, &e->scope, args, 1);
        return EVM_VAL_UNDEFINED;
    }
}

//http.get(options[, callback])
static evm_val_t evm_module_http_get(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
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

    uint32_t length = evm_prop_len(v);
    for (uint32_t index = 0; index < length; index++)
    {
        evm_hash_t key = evm_prop_get_key_by_index(e, v, index);
        if (key == EVM_INVALID_HASH)
        {
            break;
        }
        evm_val_t *v = evm_prop_get_by_index(e, key, index);
        const char *name = evm_string_get(e, key);
        if (evm_is_string(v))
            webclient_header_fields_add(session, "%s: %s\r\n", name);
        else
            webclient_header_fields_add(session, "%s: %d\r\n", name);
    }

    uint32_t len = evm_string_len(host) + 5 + evm_string_len(path);
    evm_val_t *url = evm_heap_string_create(e, "", len);
    sprintf(evm_heap_string_addr(url), "%s:%d%s", host, port, path);

    evm_object_set_ext_data(e, (intptr_t)session);

    evm_val_t *args;
    int status_code = webclient_get(session, evm_2_string(url));
    if (status_code != 200)
    {
        args = evm_mk_foreign_string("http status code not success.");
        if (evm_is_script(v + 1))
            evm_run_callback(e, v + 1, &e->scope, args, 1);
        return EVM_VAL_UNDEFINED;
    }
    args = evm_module_http_client_new(e);
    _http_client_t *client = (_http_client_t *)evm_object_get_ext_data(args);
    client->session = session;
    if (args && evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, args, 1);
    return EVM_VAL_UNDEFINED;
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

//response.end([data][, callback])
static evm_val_t evm_module_http_response_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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

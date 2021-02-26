#ifdef CONFIG_EVM_MODULE_HTTP
#include "evm_module.h"
#include <rtthread.h>
#include "webclient.h"

#define HTTP_HEADER_SIZE (1024)
#define HTTP_RECV_MAX_SIZE (10 * 1024)

//request.abort()
static evm_val_t evm_module_http_request_abort(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct webclient_session *session = evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    webclient_close(session);
}

//request.end([data][, callback])
static evm_val_t evm_module_http_request_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_http_request_write(e, p, argc, v);

    if (webclient_get(session, url) != 200)
    {
        return EVM_VAL_UNDEFINED
    }

    int flag = 0;
    if (argc > 1 && evm_is_script(v + 1))
        flag = 1;

    int bytes_read = 0, content_pos = 0;
    int content_length = webclient_content_length_get(session);

    do
    {
        bytes_read = webclient_read(session, buffer, 1024);
        if (bytes_read <= 0)
        {
            break;
        }

        if (flag)
            evm_run_callback(e, v + 1, &e->scope, NULL, 0);

        content_pos += bytes_read;
    } while (content_pos < content_length);

    return EVM_VAL_UNDEFINED;
}

//request.setTimeout(ms[, callback])
static evm_val_t evm_module_http_request_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    //ms秒之后，销毁socket，并执行回调函数
    if (argc < 0 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t timeout = evm_2_integer(v);
    if (timeout < 0)
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    webclient_set_timeout(session, timeout);
    return EVM_VAL_NULL;
}

//request.write(data[, callback])
static evm_val_t evm_module_http_request_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v) || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    struct webclient_session *session = evm_object_get_ext_data(p);
    if (!session)
        return EVM_VAL_UNDEFINED;

    evm_val_t *buf = v;
    int result = 0;
    if (evm_is_string(v))
    {
        result = webclient_write(session, evm_2_string(v), evm_string_len(v));
    }
    else
    {
        result = webclient_write(session, evm_buffer_addr(buf), evm_buffer_len(buf))
    }

    if (argc > 1 && evm_is_script(v + 1))
    {
        evm_run_callback(e, v + 1, &e->scope, NULL, 0);
    }

    if (result > 0)
        return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
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

        struct webclient_session *session = evm_malloc(sizeof(struct webclient_session));
        if (session)
        {
            evm_object_set_ext_data(obj, (intptr_t)session);
        }
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

//http.request(options[, callback])
static evm_val_t evm_module_http_request(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *host = evm_prop_get(e, v, "host", 0);
    if (host)
        return EVM_VAL_UNDEFINED;

    uint8_t p = 80;
    evm_val_t *port = evm_prop_get(e, v, "port", 0);
    if (port)
        p = evm_2_integer(port);

    char *m = "GET";
    evm_val_t *method = evm_prop_get(e, v, "method", 0);
    if (method)
        p = evm_2_string(method);

    char *pth = "/";
    evm_val_t *path = evm_prop_get(e, v, "path", 0);
    if (path)
        pth = evm_2_string(path);

    evm_val_t *result = evm_module_http_client_new(e);
    if (!result)
        return EVM_VAL_UNDEFINED;
    struct webclient_session *session = evm_object_get_ext_data(result);

    const char *url;
    sprintf(url, "%s:%d%s", host, p, pth);

    evm_val_t *opts = evm_prop_get(e, v, "headers", 0);

    if (evm_is_object(opts))
    {
        uint32_t count = evm_object_get_count(opts);
        uint32_t len = evm_prop_len(opts);
        while (count < len)
        {
            evm_hash_t key = evm_prop_get_key_by_index(e, opts, count);
            if (key == EVM_INVALID_HASH)
            {
                continue;
            }
            const char *name = evm_string_get(e, key);
            webclient_header_fields_add(session, "%s: %d\r\n", name);
            count++;
        }
    }

    return *result;
}

//http.get(options[, callback])
static evm_val_t evm_module_http_request_get(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    // evm_val_t *method = evm_prop_get(e, v, "method", 0);
    // if (!method)
    //     return EVM_VAL_UNDEFINED;

    evm_prop_set_value(e, p, "method", evm_mk_heap_string((intptr_t) "GET"));

    evm_val_t *client = evm_module_http_request(e, p, argc, v);
    return evm_module_http_request_end(e, client, argc, v);
}

evm_err_t evm_module_http(evm_t *e) {
    evm_builtin_t builtin[] = {
        {"request", evm_mk_native((intptr_t)evm_module_http_request)},
        {"get", evm_mk_native((intptr_t)evm_module_http_request_get)},
        {NULL, NULL}};
    evm_module_create(e, "http", builtin);
	return e->err;
}
#endif

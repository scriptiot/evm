#ifdef CONFIG_EVM_MODULE_HTTP
#include "evm_module.h"
#include <rtthread.h>
#include "webclient.h"

//http.createServer([options][, requestListener])
static evm_val_t evm_module_http_createServer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//http.request(options[, callback])
static evm_val_t evm_module_http_request(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//http.get(options[, callback])
static evm_val_t evm_module_http_get(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//http.server.on(event, callback)
static evm_val_t evm_module_http_server_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//server.listen(port[, hostname][, backlog][, callback])
static evm_val_t evm_module_http_server_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//server.close([callback])
static evm_val_t evm_module_http_server_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//server.setTimeout(ms[, callback])
static evm_val_t evm_module_http_server_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//http.request.on(event, callback)
static evm_val_t evm_module_http_request_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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
	return EVM_VAL_UNDEFINED;
}

//request.setTimeout(ms[, callback])
static evm_val_t evm_module_http_request_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//request.write(data[, callback])
static evm_val_t evm_module_http_request_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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
	return EVM_VAL_UNDEFINED;
}

//response.removeHeader(name)
static evm_val_t evm_module_http_response_removeHeader(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//response.setHeader(name, value)
static evm_val_t evm_module_http_response_setHeader(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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
	return EVM_VAL_UNDEFINED;
}

//response.writeHead(statusCode[, statusMessage][, headers])
static evm_val_t evm_module_http_response_writeHead(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_http(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"createServer", evm_mk_native((intptr_t)evm_module_http_createServer)},
        {"request", evm_mk_native((intptr_t)evm_module_http_request)},
        {"get", evm_mk_native((intptr_t)evm_module_http_get)},
		{NULL, NULL}
	};
	evm_module_create(e, "http", builtin);
	return e->err;
}
#endif

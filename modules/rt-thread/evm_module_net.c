#ifdef CONFIG_EVM_MODULE_NET
#include "evm_module.h"
#include <rtthread.h>

//server.close([closeListener])
static evm_val_t evm_module_net_server_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//server.listen(port[, host][, backlog][, listenListener])
//server.listen(options[, listenListener])
static evm_val_t evm_module_net_server_listen(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//new net.Socket([options])
static evm_val_t evm_module_net_socket_new(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.connect(options[, connectListener])
//socket.connect(port[, host][, connectListener])
static evm_val_t evm_module_net_socket_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.destroy()
static evm_val_t evm_module_net_socket_destroy(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.end([data][, callback])
static evm_val_t evm_module_net_socket_end(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.pause()
static evm_val_t evm_module_net_socket_pause(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.resume()
static evm_val_t evm_module_net_socket_resume(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.setKeepAlive([enable][, initialDelay])
static evm_val_t evm_module_net_socket_setKeepAlive(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.setTimeout(timeout[, callback])
static evm_val_t evm_module_net_socket_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.write(data[, callback])
static evm_val_t evm_module_net_socket_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//socket.on(event, callback)
static evm_val_t evm_module_net_socket_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//net.connect(options[, connectListener])
//net.connect(port[, host][, connectListener])
static evm_val_t evm_module_net_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//net.createConnection(options[, connectListener])
//net.createConnection(port[, host][, connectListener])
static evm_val_t evm_module_net_createConnection(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//net.createServer([options][, connectionListener])
static evm_val_t evm_module_net_createServer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_net(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"connect", evm_mk_native((intptr_t)evm_module_net_connect)},
        {"createConnection", evm_mk_native((intptr_t)evm_module_net_createConnection)},
        {"createServer", evm_mk_native((intptr_t)evm_module_net_createServer)},
		{NULL, NULL}
	};
	evm_module_create(e, "net", builtin);
	return e->err;
}
#endif
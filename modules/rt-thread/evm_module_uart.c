#include "evm_module.h"

evm_val_t *evm_module_uart_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//uart.open(configuration, callback)
static evm_val_t evm_module_uart_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//uart.openSync(configuration)
static evm_val_t evm_module_uart_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//uartport.write(data, callback)
static evm_val_t evm_module_uart_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//uartport.writeSync(data)
static evm_val_t evm_module_uart_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//uartport.close([callback])
static evm_val_t evm_module_uart_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//uartport.closeSync()
static evm_val_t evm_module_uart_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_uart_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 4, 0);
	if( obj ) {
		evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_uart_class_write));
		evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_uart_class_writeSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_uart_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_uart_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_uart(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_uart_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_uart_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "uart", builtin);
	return e->err;
}
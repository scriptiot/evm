#ifdef CONFIG_EVM_UART
#include "evm_module.h"

//UART(name)
static evm_val_t evm_module_uart(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.init(baudrate, bits=8, parity=None, stop=1, *, timeout=0, flow=0, timeout_char=0, read_buf_len=64)
static evm_val_t evm_module_uart_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.deinit()
static evm_val_t evm_module_uart_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.any()
static evm_val_t evm_module_uart_any(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.read([nbytes])
static evm_val_t evm_module_uart_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.readchar()
static evm_val_t evm_module_uart_readchar(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.readinto(buf[, nbytes])
static evm_val_t evm_module_uart_readinto(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.readline()
static evm_val_t evm_module_uart_readline(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.write(buf)
static evm_val_t evm_module_uart_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.writechar(char)
static evm_val_t evm_module_uart_writechar(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//UART.sendbreak()
static evm_val_t evm_module_uart_sendbreak(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_uart(evm_t * e){
	evm_builtin_t class_uart[] = {
		{"init", evm_mk_native( (intptr_t)evm_module_uart_init )},
		{"deinit", evm_mk_native( (intptr_t)evm_module_uart_deinit )},
		{"any", evm_mk_native( (intptr_t)evm_module_uart_any )},
		{"read", evm_mk_native( (intptr_t)evm_module_uart_read )},
		{"readchar", evm_mk_native( (intptr_t)evm_module_uart_readchar )},
		{"readinto", evm_mk_native( (intptr_t)evm_module_uart_readinto )},
		{"readline", evm_mk_native( (intptr_t)evm_module_uart_readline )},
		{"write", evm_mk_native( (intptr_t)evm_module_uart_write )},
		{"writechar", evm_mk_native( (intptr_t)evm_module_uart_writechar )},
		{"sendbreak", evm_mk_native( (intptr_t)evm_module_uart_sendbreak )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_uart, class_uart, NULL);
}
#endif

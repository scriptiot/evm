#ifdef CONFIG_EVM_UART
#include "evm_module.h"
#include <drivers/uart.h>

//UART(name, baudrate, databits, parity, stopbits, rxBufSize)
static evm_val_t evm_module_uart(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc == 1 ){
		evm_val_t params[6];
		params[0] = *v;
		params[1] = evm_mk_number(115200);
		params[2] = evm_mk_number(CONFIG_EVM_UART_DATA_BITS_8);
		params[3] = evm_mk_number(CONFIG_EVM_UART_PARITY_NONE);
		params[4] = evm_mk_number(CONFIG_EVM_UART_STOP_BITS_1);
		params[5] = evm_mk_number(256);
		evm_module_construct(nevm_runtime, p, 6, params, EXPORT_main_serialCreate, EXPORT_SerialDevice_open);
	} else {
		evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_serialCreate, EXPORT_SerialDevice_open);
	}
	return EVM_VAL_UNDEFINED;
}

//UART.any()
static evm_val_t evm_module_uart_any(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_count, 0, NULL);
}

//UART.read(buf, offset, size)
static evm_val_t evm_module_uart_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>2){
		evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
		return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_read, 3, v);
	}
	return EVM_VAL_UNDEFINED;
}


//UART.write(buf, offset, size) | UART.write(str)
static evm_val_t evm_module_uart_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	if(argc>2){
		if( evm_is_buffer(v) )
			return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_write, argc, v);
	} else if( argc > 0 ){
		if( evm_is_string(v) )
			return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_writeString, argc, v);
	}
	return EVM_VAL_UNDEFINED;
}


evm_val_t evm_class_uart(evm_t * e){
	evm_builtin_t class_uart[] = {
		{"any", evm_mk_native( (intptr_t)evm_module_uart_any )},
		{"read", evm_mk_native( (intptr_t)evm_module_uart_read )},
		{"write", evm_mk_native( (intptr_t)evm_module_uart_write )},

		{"NONE",evm_mk_number(CONFIG_EVM_UART_PARITY_NONE)},
		{"ODD",evm_mk_number(CONFIG_EVM_UART_PARITY_ODD)},
		{"EVEN",evm_mk_number(CONFIG_EVM_UART_PARITY_EVEN)},

		{"STOP05",evm_mk_number(CONFIG_EVM_UART_STOP_BITS_0_5)},
		{"STOP1",evm_mk_number(CONFIG_EVM_UART_STOP_BITS_1)},
		{"STOP15",evm_mk_number(CONFIG_EVM_UART_STOP_BITS_1_5)},
		{"STOP2",evm_mk_number(CONFIG_EVM_UART_STOP_BITS_2)},

		{"DATA5",evm_mk_number(CONFIG_EVM_UART_DATA_BITS_5)},
		{"DATA6",evm_mk_number(CONFIG_EVM_UART_DATA_BITS_6)},
		{"DATA7",evm_mk_number(CONFIG_EVM_UART_DATA_BITS_7)},
		{"DATA8",evm_mk_number(CONFIG_EVM_UART_DATA_BITS_8)},
		{"DATA9",evm_mk_number(CONFIG_EVM_UART_DATA_BITS_9)},

		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_uart, class_uart, NULL);
}
#endif

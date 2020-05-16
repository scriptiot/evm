/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
**  Licence: Apache-2.0
****************************************************************************/

#ifdef CONFIG_EVM_UART
#include "evm_module.h"
#include <drivers/uart.h>

//UART(name, baudrate, databits, parity, stopbits, rxBufSize)
/**
 * @brief UART constructor. if only device name is given, the rest parameters are set to default
 * 
 * @param name	serial port name 
 * @param baudrate	baudrate of serial port
 * @param databits	databits
 * @param parity	parity
 * @param stopbits	stopbits
 * @param rxBufSize	receiving buffer size
 * 
 * @usage new UART('UART_2') 
 */
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
/**
 * @brief get the number of bytes remained in buffer
 * 
 * @usage UART.any() 
 */
static evm_val_t evm_module_uart_any(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_count, 0, NULL);
}

//UART.read(buf, offset, size)
/**
 * @brief read data into a given buffer
 * 
 * @param buf		a buffer object
 * @param offset	buffer offset	
 * @param size		the size of data
 * @usage UART(buf, 0, 10) 
 */
static evm_val_t evm_module_uart_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if(argc>2){
		evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
		return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_SerialDevice_read, 3, v);
	}
	return EVM_VAL_UNDEFINED;
}


//UART.write(buf, offset, size) | UART.write(str)
/**
 * @brief write the content of buffer into serial port
 * 
 * @param str		String object
 * 
 * @param buf		buffer object 
 * @param offset	buffer offset
 * @param size		size of data to be written 
 * @usage UART.write('evm'), UART.write(buf, 0, 10) 
 */
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

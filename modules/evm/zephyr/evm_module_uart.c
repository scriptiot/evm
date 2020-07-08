/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/


#include "evm_module.h"
#include <drivers/uart.h>

#define UART_BUFFER_INITIAL_SIZE 16

typedef struct uart_handle_t {
	struct device * dev;
	uint8_t * buf;
	int size;
	int obj;
	int read_callback;
} uart_handle_t;

static void uart_irq_handler(uart_handle_t * handle)
{
    uart_irq_update(handle->dev);

    if (!uart_irq_is_pending(handle->dev)) {
        return;
    }

    if (uart_irq_tx_ready(handle->dev)) {

    }

    if (uart_irq_rx_ready(handle->dev)) {
        u32_t len = uart_fifo_read(handle->dev, handle->buf, UART_BUFFER_INITIAL_SIZE);
        handle->buf[len] = '\0';
        handle->size = len;
    }
}

//UART(name, baudrate, databits, parity, stopbits)
/**
 * @brief UART constructor. if only device name is given, the rest parameters are set to default
 * 
 * @param name	serial port name 
 * @param baudrate	baudrate of serial port
 * @param databits	databits
 * @param parity	parity
 * @param stopbits	stopbits
 * 
 * @usage new UART('UART_2') 
 */
static evm_val_t evm_module_uart(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = NULL;
	if( argc > 1 && evm_is_string(v) ) {
		if( argc > 4 ) {
			for(int i = 1; i < 4; i++){
				if( !evm_is_number(v + i) ) return EVM_VAL_UNDEFINED;
			}
		}

		dev = device_get_binding(evm_2_string(v));
		if( !dev ) {
			evm_set_err(e, ec_type, "Can't find uart device");
			return EVM_VAL_UNDEFINED;
		}

		if( argc > 4){
			struct uart_config uart_cfg = {
				.baudrate = evm_2_integer(v + 1),
				.data_bits = evm_2_integer(v + 2),
				.parity = evm_2_integer(v + 3),
				.stop_bits = evm_2_integer(v + 4),
				.flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
			};
			uart_configure(dev, &uart_cfg);
		} else if(argc > 1){
			struct uart_config uart_cfg = {
				.baudrate = 115200,
				.data_bits = UART_CFG_DATA_BITS_8,
				.parity = UART_CFG_PARITY_NONE,
				.stop_bits = UART_CFG_STOP_BITS_1,
				.flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
			};
			uart_configure(dev, &uart_cfg);
		} 

		uart_handle_t * uart_handle = evm_malloc(sizeof(uart_handle_t));
		uart_handle->dev = dev;
		uart_handle->buf = evm_malloc(UART_BUFFER_INITIAL_SIZE);
		if( !uart_handle->buf ) {
			evm_free(uart_handle);
			evm_set_err(e, ec_type, "Failed to create buffer");
			return EVM_VAL_UNDEFINED;
		}

		
		uart_irq_callback_user_data_set(dev, (uart_irq_callback_user_data_t)uart_irq_handler, uart_handle);
    	uart_irq_rx_enable(dev);
		evm_object_set_ext_data(p, (intptr_t)uart_handle);

		uart_handle->obj = -1;
		uart_handle->read_callback = -1;
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
	uart_handle_t * uart_handle = (uart_handle_t*)evm_object_get_ext_data(p);
	if( uart_handle ){
		return evm_mk_number(uart_handle->size);
	}
	return EVM_VAL_UNDEFINED;
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
	uart_handle_t * uart_handle = (uart_handle_t*)evm_object_get_ext_data(p);
	if(uart_handle && argc > 2){
		struct device * dev = uart_handle->dev;
		char recv_data;
		uint8_t *buf = evm_buffer_addr(v);
		int offset = evm_2_integer(v + 1);
		int len = evm_2_integer(v + 2);
		if( len >= evm_buffer_len(v) - offset ) len = evm_buffer_len(v) - offset;

		int index = 0;
		while (index < len) 
		{
			if(uart_poll_in(dev, &recv_data) >= 0)
			{
				buf[index + offset] = recv_data;
				index++;
				if(index == len)
				{
					break;
				}
			}
		}

		return evm_mk_number(index);
	}
	return evm_mk_number(0);
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
	uart_handle_t * uart_handle = (uart_handle_t*)evm_object_get_ext_data(p);
	if(uart_handle && argc > 0){
		struct device * dev = uart_handle->dev;
		uint8_t *buf = NULL;
		int size = 0;
		if( evm_is_buffer(v) ){
			buf = evm_buffer_addr(v);
			size = evm_buffer_len(v);
		} else if( evm_is_foreign_string(v) ){
			buf = (char *)evm_2_intptr(v);
			size = strlen(buf);
		} else if( evm_is_heap_string(v) ){
			buf = evm_heap_string_addr(v);
			size = strlen(buf);
		} else {
			return EVM_VAL_UNDEFINED;
		}
		
		if( argc > 2){
			int offset = evm_2_integer(v + 1);
			int len = evm_2_integer(v + 2);

			if( evm_is_buffer(v)  ){
				if( len >= evm_buffer_len(v) - offset ) len = size - offset;
			}

			for (int i = 0; i <len; i++) 
				uart_poll_out(dev, buf[i + offset]);
		} else {
			for (int i = 0; i < size; i++) 
				uart_poll_out(dev, buf[i]);
		}
	}
	return EVM_VAL_UNDEFINED;
}

//callback(function)
static evm_val_t evm_module_uart_callback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 0 && evm_is_script(v) ){
		uart_handle_t * uart_handle = (uart_handle_t*)evm_object_get_ext_data(p);
		if( uart_handle ){
			uart_handle->obj = evm_add_reference(*p);
			uart_handle->read_callback = evm_add_reference(*v);
		}
	}
	return EVM_VAL_UNDEFINED;
}


evm_val_t evm_class_uart(evm_t * e){
	evm_builtin_t class_uart[] = {
		{"any", evm_mk_native( (intptr_t)evm_module_uart_any )},
		{"read", evm_mk_native( (intptr_t)evm_module_uart_read )},
		{"write", evm_mk_native( (intptr_t)evm_module_uart_write )},
		{"callback", evm_mk_native( (intptr_t)evm_module_uart_callback )},

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

		{NULL, EVM_VAL_UNDEFINED}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_uart, class_uart, NULL);
}


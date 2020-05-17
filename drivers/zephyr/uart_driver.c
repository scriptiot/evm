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

#include "nevm.h"

#ifdef CONFIG_EVM_UART
#include "evm_module.h"
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>

typedef struct uart_handle_t{
    struct device * dev;
    evm_val_t obj;
}uart_handle_t;

static void uart_irq_handler(uart_handle_t * handle)
{
    uart_irq_update(handle->dev);

    if (!uart_irq_is_pending(handle->dev)) {
        return;
    }

    if (uart_irq_tx_ready(handle->dev)) {

    }

    if (uart_irq_rx_ready(handle->dev)) {
        char recvData;
		uart_fifo_read(handle->dev, &recvData, 1);
        evm_val_t param = evm_mk_number(recvData);
        nevm_object_function_invoke(nevm_runtime, &handle->obj, EXPORT_SerialDevice_writeToBuffer, 1, &param);
    }
}

#endif

//uart_config(int baudrate, char databits, char parity, char stopbits, char flow)
evm_val_t nevm_driver_uart_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    struct uart_config uart_cfg = {
        .baudrate = evm_2_integer(v),
        .data_bits = evm_2_integer(v+1),
        .parity = evm_2_integer(v+2),
        .stop_bits = evm_2_integer(v+3),
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };
    uart_configure(dev, &uart_cfg);
    uart_handle_t * uart_handle = evm_malloc(sizeof(uart_handle_t));
    uart_handle->dev = dev;
    uart_handle->obj = *p;
    uart_irq_callback_user_data_set(dev, (uart_irq_callback_user_data_t)uart_irq_handler, uart_handle);
    uart_irq_rx_enable(dev);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_uart_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_UART
    struct device * dev = device_get_binding(evm_2_string(v));
    nevm_object_set_ext_data(p, (intptr_t)dev);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_uart_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_FALSE;
}

//uart_read_bytes(Int8Array buffer, int offset, int len)
evm_val_t nevm_driver_uart_read_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
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
#endif
    return evm_mk_number(0);
}
//uart_write_bytes(Int8Array buffer|String s, int offset, int len)
evm_val_t nevm_driver_uart_write_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    uint8_t *buf = NULL;
    if( evm_is_buffer(v) ){
        buf = evm_buffer_addr(v);
    } else if( evm_is_foreign_string(v) ){
        buf = (char *)evm_2_intptr(v);
    } else if( evm_is_heap_string(v) ){
        buf = evm_heap_string_addr(v);
    }
    int offset = evm_2_integer(v + 1);
    int len = evm_2_integer(v + 2);
    if( evm_is_buffer(v)  ){
        if( len >= evm_buffer_len(v) - offset ) len = evm_buffer_len(v) - offset;
    }

    for (int i = 0; i <len; i++) 
        uart_poll_out(dev, buf[i + offset]);
    return evm_mk_number(len);
#endif
    return evm_mk_number(0);
}

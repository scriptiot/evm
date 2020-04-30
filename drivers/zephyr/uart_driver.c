#include "nevm.h"

#ifdef EVM_DRIVER_UART
#include <device.h>
#include <uart.h>
#include <zephyr.h>

static volatile bool tx = false;
static volatile bool rx = false;

static void uart_irq_handler(struct device *dev)
{
    uart_irq_update(dev);

    if (!uart_irq_is_pending(dev)) {
        return;
    }

    if (uart_irq_tx_ready(dev)) {

    }

    if (uart_irq_rx_ready(dev)) {

    }
}

#endif

//uart_config(int baudrate, char databits, char parity, char stopbits, Object callback)
evm_val_t nevm_driver_uart_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
#ifdef CONFIG_UART_LINE_CTRL
    int ret = uart_line_ctrl_set(dev, LINE_CTRL_BAUD_RATE, evm_2_integer(v));
    if (ret) {
        return NEVM_FALSE;
    }
#endif
    uart_irq_callback_set(dev, uart_irq_handler);
    uart_irq_rx_enable(dev);
    return NEVM_TRUE;
#endif
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_uart_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = device_get_binding(evm_2_string(v));
    nevm_object_set_ext_data(p, (intptr_t)dev);
    
#endif
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_uart_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_uart_read_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

//uart_write_byte(char data)
evm_val_t nevm_driver_uart_write_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    uart_irq_tx_enable(dev);
    char data = evm_2_integer(v);
    int sent = uart_fifo_fill(dev, (const u8_t *)&data, 1);
    return evm_mk_number(sent);
#endif
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_uart_read_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}
//uart_write_bytes(buf, offset, len)
evm_val_t nevm_driver_uart_write_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    uart_irq_tx_enable(dev);
    int sent = 0;
    int sent_total = 0;
    char * buf = evm_buffer_addr(v);
    int offset = evm_2_integer(v + 1);
    int len = evm_2_integer(v + 2);

    while (len > 0) {
        tx = false;
        sent = uart_fifo_fill(dev, (const u8_t *)buf + offset, len);
        if (!sent) {
            return evm_mk_number(sent_total);
        }

        while (tx == false) {
            ;
        }

        len -= sent;
        buf += sent;
        sent_total += sent;
    }

    uart_irq_tx_disable(dev);
    return evm_mk_number(sent_total);
#endif
    return EVM_VAL_UNDEFINED;
}

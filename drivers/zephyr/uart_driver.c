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
		uart_fifo_read(dev, &recvData, 1);

    }
}

#endif

//uart_config(int baudrate, char databits, char parity, char stopbits, Object callback)
evm_val_t nevm_driver_uart_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    struct uart_config uart_cfg = {
        .baudrate = evm_2_integer(V),
        .parity = evm_2_integer(V+2),
        .stop_bits = evm_2_integer(V+3),
        .data_bits = evm_2_integer(V+1),
        .flow_ctrl = evm_2_integer(V+4)
    };
    uart_configure(dev, &uart_cfg);

    // uart_irq_callback_set(dev, uart_irq_handler);
    // uart_irq_rx_enable(dev);
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

//uart_read_byte(char rec_data,int timeout=100)
evm_val_t nevm_driver_uart_read_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    char recv_char ;
    int timeout = evm_2_integer(v+1); 
    int start = k_cycle_get_32();
    while ((k_cycle_get_32()- start)/(1000*1000)>timeout) 
    {
        if(uart_poll_in(dev, &recv_char) >= 0)
        {
            *v = evm_mk_number(recv_char) ;
            return NEVM_TRUE;
        }
    }
    return NEVM_FALSE;

#endif
    return EVM_VAL_UNDEFINED;
}

//uart_write_byte(char data)
evm_val_t nevm_driver_uart_write_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    uart_poll_out(dev, evm_2_integer(v));

    return NEVM_TRUE;
#endif
    return EVM_VAL_UNDEFINED;
}

//uart_read_bytes(len, timeout)
evm_val_t nevm_driver_uart_read_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    int len = evm_2_integer(v);
    int timeout = evm_2_integer(v + 1);

    char *buff = (char *)malloc(sizeof(char)*len);

    int index = 0;
    int start = k_cycle_get_32();
    while ((k_cycle_get_32()- start)/(1000*1000)>timeout) 
    {
        if(uart_poll_in(dev, &recv_char) >= 0)
        {
            buf[index++] = recv_char;
            if(index == len)
            {
                evm_val_t *ebuff = evm_buffer_create(e,len);
		        evm_buffer_set(ebuff,buff,0,len);
                free(buff);
                return ebuff;
            }
        }
    }
    free(buff);
    return evm_mk_null();
#endif
    return EVM_VAL_UNDEFINED;
}
//uart_write_bytes(buf, offset, len)
evm_val_t nevm_driver_uart_write_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_UART
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    char * buf = evm_buffer_addr(v);
    int offset = evm_2_integer(v + 1);
    int len = evm_2_integer(v + 2);

    for (i = 0; i <len; i++) {
        uart_poll_out(dev, buf[i+offset]);

    return evm_mk_number(len);
#endif
    return EVM_VAL_UNDEFINED;
}

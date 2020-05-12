#include "evm_main.h"

void main(void)
{
    // evm_main();

    struct device * dev = device_get_binding("USART_1");
    struct uart_config uart_cfg = {
        .baudrate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_configure(dev, &uart_cfg);

    while(1)
    {
        uart_poll_out(dev, 0x5E);
        k_sleep(1000);
    }
}

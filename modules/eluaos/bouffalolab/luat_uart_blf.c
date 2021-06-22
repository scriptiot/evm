#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_uart.h"
#include "luat_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <vfs.h>
#include <bl602_uart.h>
#include <bl_uart.h>
#include <hal_uart.h>
#include <device/vfs_uart.h>

static int fd_console;
//串口数量，编号从0开始
#define MAX_DEVICE_COUNT 10
static int serials[MAX_DEVICE_COUNT];

char *luat_find_port(int port) {
    switch(port) {
        case 0: return "/dev/ttyS0";
        case 1: return "/dev/ttyS1";
        case 2: return "/dev/ttyS2";
        case 3: return "/dev/ttyS3";
    }
    return "/dev/ttyS0";
}

char evm_repl_tty_read(evm_t *e)
{
    EVM_UNUSED(e);
    char buffer[16];
    aos_read(fd_console, buffer, 1);
    return buffer[0];
}

int luat_uart_exist(int uartid) {
    return uartid >= 0;
}

int luat_console_setup(int port) {
    fd_console = aos_open(luat_find_port(port), 0);
    return fd_console;
}

static void uart_task(void *arg)
{
    int length = 0, fd;
    luat_uart_t *uart = (luat_uart_t*)arg;
    fd = serials[uart->id];
    while (1) {
        length = UART_GetRxFifoCount(fd);
        if( length > 0 ) {
            rtos_msg_t msg;
            msg.handler = l_uart_handler;
            msg.ptr = NULL;
            msg.arg1 = uart->id;
            msg.arg2 = length;
            luat_msgbus_put(&msg, 1);
        }
        vTaskDelay(10);
    }
    aos_close(fd);
}

int luat_uart_setup(luat_uart_t* uart)
{
    uart_ioc_config_t config;
    int fd = aos_open(luat_find_port(uart->id), 0);
    if (fd < 0) {
        return 0;
    }
    serials[uart->id] = fd;
    config.baud = uart->baud_rate;
    config.parity = uart->parity;
    aos_ioctl(fd, IOCTL_UART_IOC_CONFIG_MODE, &config);
    aos_task_new("uart_thread", uart_task, uart, 2048);
    return 0;
}

int luat_uart_write(int uartid, void* data, size_t length)
{
    return aos_write(uartid, data, length);
}

int luat_uart_read(int uartid, void* buffer, size_t length)
{
    return aos_read(uartid, buffer, length);
}

int luat_uart_close(int uartid)
{
    return aos_close(uartid);
}

int luat_setup_cb(int uartid, int received, int sent) {
    return 0;
}

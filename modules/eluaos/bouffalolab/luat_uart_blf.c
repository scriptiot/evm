#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_uart.h"
#include "luat_log.h"

static int fd_console;

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

int luat_console_setup(int port) {
    fd_console = aos_open(luat_find_port(port), 0);
    return fd_console;
}

int luat_uart_setup(luat_uart_t* uart)
{
    
}

int luat_uart_write(int uartid, void* data, size_t length)
{
    return aos_write(uartid, data, length);
}

int luat_uart_read(int uartid, void* buffer, size_t length)
{
    return 0;
}

int luat_uart_close(int uartid)
{
    return 0;
}

int luat_setup_cb(int uartid, int received, int sent) {
    return 0;
}

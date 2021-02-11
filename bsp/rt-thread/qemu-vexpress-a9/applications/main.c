#include <rtthread.h>
#include <rtdevice.h>

#include "evm_module.h"

static void _main(void* param) {
    evm_main();
    while (1)
        rt_thread_delay(10000000);
}

int main(void)
{
    rt_thread_t t = rt_thread_create("evm", _main, RT_NULL, 32*1024, 15, 20);
    rt_thread_startup(t);
    return 0;
}



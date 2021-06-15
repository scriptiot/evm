#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <string.h>
#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>
#include <netutils/netutils.h>

#include <bl_wifi.h>
#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_sec.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <hal_uart.h>
#include <hal_wifi.h>
#include <hal_sys.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <bl_sys_time.h>
#include <bl_romfs.h>
#include <easyflash.h>
#include <bl60x_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <fdt.h>
#include <libfdt.h>
#include <blog.h>
#include "luat_log.h"
#include "luat_base.h"

extern void ble_stack_start(void);
volatile uint32_t uxTopUsedPriority __attribute__((used)) = configMAX_PRIORITIES - 1;

static wifi_interface_t wifi_interface;

extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap_wifi_start;
extern uint8_t _heap_wifi_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegions[] = {
    {&_heap_start, (unsigned int)&_heap_size}, //set on runtime
    {&_heap_wifi_start, (unsigned int)&_heap_wifi_size},
    {NULL, 0}, /* Terminates the array. */
    {NULL, 0}  /* Terminates the array. */
};

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /*empty*/
    puts("Stack Overflow checked\r\n");
}

void vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize());
    /*empty*/
}

void vApplicationIdleHook(void)
{
#if 0
    __asm volatile(
            "   wfi     "
    );
    /*empty*/
#else
    (void)uxTopUsedPriority;
#endif
}

void vAssertCalled(void)
{
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    taskDISABLE_INTERRUPTS();
    while (ulSetTo1ToExitFunction != 1)
    {
        __asm volatile("NOP");
    }
}

static int get_dts_addr(const char *name, uint32_t *start, uint32_t *off)
{
    uint32_t addr = hal_board_get_factory_addr();
    const void *fdt = (const void *)addr;
    uint32_t offset;

    if (!name || !start || !off)
    {
        return -1;
    }

    offset = fdt_subnode_offset(fdt, 0, name);
    if (offset <= 0)
    {
        log_error("%s NULL.\r\n", name);
        return -1;
    }

    *start = (uint32_t)fdt;
    *off = offset;

    return 0;
}

static void evm_task_proc(void *pvParameters)
{
    uint32_t fdt = 0, offset = 0;
    /* uart */
    if (0 == get_dts_addr("uart", &fdt, &offset))
    {
        vfs_uart_init(fdt, offset);
    }

    printf("=================================================================evm main starts\r\n");
    luat_log_set_uart_port(0);
    luat_main();

    while (1)
    {
        vTaskDelay(100);
    }
}

static void aos_loop_proc(void *pvParameters)
{
    easyflash_init();
    vfs_init();
    vfs_device_init();

#ifdef CONF_USER_ENABLE_VFS_ROMFS
    romfs_register();
#endif

    aos_loop_init();

    aos_loop_run();

    puts("------------------------------------------\r\n");
    puts("+++++++++Critical Exit From Loop++++++++++\r\n");
    puts("******************************************\r\n");
    vTaskDelete(NULL);
}

static void system_init(void)
{
    blog_init();
    bl_irq_init();
    bl_sec_init();
    bl_sec_test();
    bl_dma_init();
    hal_boot2_init();

    /* board config is set after system is init*/
    hal_board_cfg(0);
}

void bfl_main(void)
{
    static StackType_t aos_loop_proc_stack[1024];
    static StaticTask_t aos_loop_proc_task;

    static StackType_t evm_stack[4*1024];
    static StaticTask_t evm_task;

    /*
     * Init UART using pins 16+7 (TX+RX)
     * and baudrate of 2M
     */
    bl_uart_init(0, 16, 7, 255, 255, 2 * 1000 * 1000);
    puts("Starting bl602 now....\r\n");

    vPortDefineHeapRegions(xHeapRegions);
    printf("Heap %u@%p, %u@%p\r\n",
           (unsigned int)&_heap_size, &_heap_start,
           (unsigned int)&_heap_wifi_size, &_heap_wifi_start);

    system_init();

    puts("[OS] Starting aos_loop_proc task...\r\n");
    xTaskCreateStatic(aos_loop_proc, (char *)"event_loop", 1024, NULL, 15, aos_loop_proc_stack, &aos_loop_proc_task);
    xTaskCreateStatic(evm_task_proc, (char *)"evm", 4 * 1024, NULL, 12, evm_stack, &evm_task);
    tcpip_init(NULL, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
}
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
#include <bl_timer.h>
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
#include "evm.h"

#define WIFI_AP_PSM_INFO_SSID "bytecode"
#define WIFI_AP_PSM_INFO_PASSWORD "bytecode888"
#define WIFI_AP_PSM_INFO_PMK "conf_ap_pmk"
#define WIFI_AP_PSM_INFO_BSSID "conf_ap_bssid"
#define WIFI_AP_PSM_INFO_CHANNEL "conf_ap_channel"

extern int evm_main(void);
extern void ble_stack_start(void);
volatile uint32_t uxTopUsedPriority __attribute__((used)) = configMAX_PRIORITIES - 1;

static uint32_t time_main;
static int fd_console;

static wifi_interface_t wifi_interface;

static wifi_conf_t conf = {
    .country_code = "CN",
};

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

static unsigned char char_to_hex(char asccode)
{
    unsigned char ret;

    if ('0' <= asccode && asccode <= '9')
        ret = asccode - '0';
    else if ('a' <= asccode && asccode <= 'f')
        ret = asccode - 'a' + 10;
    else if ('A' <= asccode && asccode <= 'F')
        ret = asccode - 'A' + 10;
    else
        ret = 0;

    return ret;
}

static void _chan_str_to_hex(uint8_t *chan_band, uint16_t *chan_freq, char *chan)
{
    int i, freq_len, base = 1;
    uint8_t band;
    uint16_t freq = 0;
    char *p, *q;

    /*should have the following format
     * 2412|0
     * */
    p = strchr(chan, '|') + 1;
    if (NULL == p)
    {
        return;
    }
    band = char_to_hex(p[0]);
    (*chan_band) = band;

    freq_len = strlen(chan) - strlen(p) - 1;
    q = chan;
    q[freq_len] = '\0';
    for (i = 0; i < freq_len; i++)
    {
        freq = freq + char_to_hex(q[freq_len - 1 - i]) * base;
        base = base * 10;
    }
    (*chan_freq) = freq;
}

static void bssid_str_to_mac(uint8_t *hex, char *bssid, int len)
{
    unsigned char l4, h4;
    int i, lenstr;
    lenstr = len;

    if (lenstr % 2)
    {
        lenstr -= (lenstr % 2);
    }

    if (lenstr == 0)
    {
        return;
    }

    for (i = 0; i < lenstr; i += 2)
    {
        h4 = char_to_hex(bssid[i]);
        l4 = char_to_hex(bssid[i + 1]);
        hex[i / 2] = (h4 << 4) + l4;
    }
}

int check_dts_config(char ssid[33], char password[64])
{
    bl_wifi_ap_info_t sta_info;

    if (bl_wifi_sta_info_get(&sta_info))
    {
        /*no valid sta info is got*/
        return -1;
    }

    strncpy(ssid, (const char *)sta_info.ssid, 32);
    ssid[31] = '\0';
    strncpy(password, (const char *)sta_info.psk, 64);
    password[63] = '\0';

    return 0;
}

static void _connect_wifi()
{
    /*XXX caution for BIG STACK*/
    char pmk[66], bssid[32], chan[10];
    char ssid[33], password[66];
    char val_buf[66];
    char val_len = sizeof(val_buf) - 1;
    uint8_t mac[6];
    uint8_t band = 0;
    uint16_t freq = 0;

    wifi_interface = wifi_mgmr_sta_enable();
    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]   Get STA %p from Wi-Fi Mgmr, pmk ptr %p, ssid ptr %p, password %p\r\n",
           aos_now_ms(),
           wifi_interface,
           pmk,
           ssid,
           password);
    memset(pmk, 0, sizeof(pmk));
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    memset(bssid, 0, sizeof(bssid));
    memset(mac, 0, sizeof(mac));
    memset(chan, 0, sizeof(chan));

    memset(val_buf, 0, sizeof(val_buf));
    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_SSID, val_buf, val_len, NULL);
    if (val_buf[0])
    {
        /*We believe that when ssid is set, wifi_confi is OK*/
        strncpy(ssid, val_buf, sizeof(ssid) - 1);

        /*setup password ans PMK stuff from ENV*/
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PASSWORD, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(password, val_buf, sizeof(password) - 1);
        }

        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PMK, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(pmk, val_buf, sizeof(pmk) - 1);
        }
        if (0 == pmk[0])
        {
            printf("[APP] [WIFI] [T] %lld\r\n",
                   aos_now_ms());
            puts("[APP]    Re-cal pmk\r\n");
            /*At lease pmk is not illegal, we re-cal now*/
            //XXX time consuming API, so consider lower-prirotiy for cal PSK to avoid sound glitch
            wifi_mgmr_psk_cal(
                password,
                ssid,
                strlen(ssid),
                pmk);
            ef_set_env(WIFI_AP_PSM_INFO_PMK, pmk);
            ef_save_env();
        }
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_CHANNEL, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(chan, val_buf, sizeof(chan) - 1);
            printf("connect wifi channel = %s\r\n", chan);
            _chan_str_to_hex(&band, &freq, chan);
        }
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_BSSID, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(bssid, val_buf, sizeof(bssid) - 1);
            printf("connect wifi bssid = %s\r\n", bssid);
            bssid_str_to_mac(mac, bssid, strlen(bssid));
            printf("mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                   mac[0],
                   mac[1],
                   mac[2],
                   mac[3],
                   mac[4],
                   mac[5]);
        }

        wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
    }
    else if (0 == check_dts_config(ssid, password))
    {
        /*nothing here*/
    }
    else
    {
        /*Won't connect, since ssid config is empty*/
        puts("[APP]    Empty Config\r\n");
        puts("[APP]    Try to set the following ENV with psm_set command, then reboot\r\n");
        puts("[APP]    NOTE: " WIFI_AP_PSM_INFO_PMK " MUST be psm_unset when conf is changed\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_SSID "\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_PASSWORD "\r\n");
        puts("[APP]    env(optinal): " WIFI_AP_PSM_INFO_PASSWORD "\r\n");
        wifi_mgmr_sta_connect(wifi_interface,
                              WIFI_AP_PSM_INFO_SSID,
                              WIFI_AP_PSM_INFO_PASSWORD,
                              NULL, NULL, 0, 0);
        return;
    }

    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]    SSID %s\r\n"
           "[APP]    SSID len %d\r\n"
           "[APP]    password %s\r\n"
           "[APP]    password len %d\r\n"
           "[APP]    pmk %s\r\n"
           "[APP]    bssid %s\r\n"
           "[APP]    channel band %d\r\n"
           "[APP]    channel freq %d\r\n",
           aos_now_ms(),
           ssid,
           strlen(ssid),
           password,
           strlen(password),
           pmk,
           bssid,
           band,
           freq);
    //wifi_mgmr_sta_connect(wifi_interface, ssid, pmk, NULL);
}

static void wifi_sta_connect(char *ssid, char *password)
{
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}

static void event_cb_wifi_event(input_event_t *event, void *private_data)
{
    static char *ssid;
    static char *password;

    switch (event->code)
    {
    case CODE_WIFI_ON_INIT_DONE:
    {
        printf("[APP] [EVT] INIT DONE %lld\r\n", aos_now_ms());
        wifi_mgmr_start_background(&conf);
    }
    break;
    case CODE_WIFI_ON_MGMR_DONE:
    {
        printf("[APP] [EVT] MGMR DONE %lld, now %lums\r\n", aos_now_ms(), bl_timer_now_us() / 1000);
        _connect_wifi();
    }
    break;
    case CODE_WIFI_ON_MGMR_DENOISE:
    {
        printf("[APP] [EVT] Microwave Denoise is ON %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE:
    {
        printf("[APP] [EVT] SCAN Done %lld\r\n", aos_now_ms());
        wifi_mgmr_cli_scanlist();
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE_ONJOIN:
    {
        printf("[APP] [EVT] SCAN On Join %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_DISCONNECT:
    {
        printf("[APP] [EVT] disconnect %lld, Reason: %s\r\n",
               aos_now_ms(),
               wifi_mgmr_status_code_str(event->value));
    }
    break;
    case CODE_WIFI_ON_CONNECTING:
    {
        printf("[APP] [EVT] Connecting %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_CMD_RECONNECT:
    {
        printf("[APP] [EVT] Reconnect %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_CONNECTED:
    {
        printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_PRE_GOT_IP:
    {
        printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_GOT_IP:
    {
        printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
        printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
    }
    break;
    case CODE_WIFI_ON_EMERGENCY_MAC:
    {
        printf("[APP] [EVT] EMERGENCY MAC %lld\r\n", aos_now_ms());
        hal_reboot(); //one way of handling emergency is reboot. Maybe we should also consider solutions
    }
    break;
    case CODE_WIFI_ON_PROV_SSID:
    {
        printf("[APP] [EVT] [PROV] [SSID] %lld: %s\r\n",
               aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (ssid)
        {
            vPortFree(ssid);
            ssid = NULL;
        }
        ssid = (char *)event->value;
    }
    break;
    case CODE_WIFI_ON_PROV_BSSID:
    {
        printf("[APP] [EVT] [PROV] [BSSID] %lld: %s\r\n",
               aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (event->value)
        {
            vPortFree((void *)event->value);
        }
    }
    break;
    case CODE_WIFI_ON_PROV_PASSWD:
    {
        printf("[APP] [EVT] [PROV] [PASSWD] %lld: %s\r\n", aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (password)
        {
            vPortFree(password);
            password = NULL;
        }
        password = (char *)event->value;
    }
    break;
    case CODE_WIFI_ON_PROV_CONNECT:
    {
        printf("[APP] [EVT] [PROV] [CONNECT] %lld\r\n", aos_now_ms());
        printf("connecting to %s:%s...\r\n", ssid, password);
        wifi_sta_connect(ssid, password);
    }
    break;
    case CODE_WIFI_ON_PROV_DISCONNECT:
    {
        printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_AP_STA_ADD:
    {
        printf("[APP] [EVT] [AP] [ADD] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
    }
    break;
    case CODE_WIFI_ON_AP_STA_DEL:
    {
        printf("[APP] [EVT] [AP] [DEL] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
    }
    break;
    default:
    {
        printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
        /*nothing*/
    }
    }
}

static void _dump_boot_info(void)
{
    char chip_feature[40];
    const char *banner;

    puts("Booting BL602 Chip...\r\n");

    /*Display Banner*/
    if (0 == bl_chip_banner(&banner))
    {
        puts(banner);
    }
    puts("\r\n");
    /*Chip Feature list*/
    puts("\r\n");
    puts("------------------------------------------------------------\r\n");
    puts("RISC-V Core Feature:");
    bl_chip_info(chip_feature);
    puts(chip_feature);
    puts("\r\n");

    puts("Build Version: ");
    puts(BL_SDK_VER); // @suppress("Symbol is not resolved")
    puts("\r\n");
    puts("Build Date: ");
    puts(__DATE__);
    puts("\r\n");
    puts("Build Time: ");
    puts(__TIME__);
    puts("\r\n");
    puts("------------------------------------------------------------\r\n");
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

char evm_repl_tty_read(evm_t *e)
{
    EVM_UNUSED(e);

    char buffer[16];
    aos_read(fd_console, buffer, 1);
    return buffer[0];
}

static void cmd_stack_wifi(char *buf, int len, int argc, char **argv)
{
    /*wifi fw stack and thread stuff*/
    static uint8_t stack_wifi_init = 0;

    if (1 == stack_wifi_init)
    {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;

    printf("Start Wi-Fi fw @%lums\r\n", bl_timer_now_us() / 1000);
    hal_wifi_start_firmware_task();
    /*Trigger to start Wi-Fi*/
    printf("Start Wi-Fi fw is Done @%lums\r\n", bl_timer_now_us() / 1000);
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
}

static void evm_task_proc(void *pvParameters)
{
    uint32_t fdt = 0, offset = 0;
    /* uart */
    if (0 == get_dts_addr("uart", &fdt, &offset))
    {
        vfs_uart_init(fdt, offset);
    }

    fd_console = aos_open("/dev/ttyS0", 0);
    if (fd_console >= 0)
    {
        printf("Init CLI with event Driven\r\n");
        evm_main();
    }

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

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);
    cmd_stack_wifi(NULL, 0, 0, NULL);

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

    static StackType_t evm_stack[1024];
    static StaticTask_t evm_task;

    time_main = bl_timer_now_us();
    /*
     * Init UART using pins 16+7 (TX+RX)
     * and baudrate of 2M
     */
    bl_uart_init(0, 16, 7, 255, 255, 2 * 1000 * 1000);
    puts("Starting bl602 now....\r\n");

    _dump_boot_info();

    vPortDefineHeapRegions(xHeapRegions);
    printf("Heap %u@%p, %u@%p\r\n",
           (unsigned int)&_heap_size, &_heap_start,
           (unsigned int)&_heap_wifi_size, &_heap_wifi_start);
    printf("Boot2 consumed %lums\r\n", time_main / 1000);

    system_init();

    puts("[OS] Starting aos_loop_proc task...\r\n");
    xTaskCreateStatic(aos_loop_proc, (char *)"event_loop", 1024, NULL, 15, aos_loop_proc_stack, &aos_loop_proc_task);
    xTaskCreateStatic(evm_task_proc, (char *)"evm", 1024, NULL, 14, evm_stack, &evm_task);
    tcpip_init(NULL, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
}
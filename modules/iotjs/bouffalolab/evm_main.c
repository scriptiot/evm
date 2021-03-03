#include "evm_module.h"
#include "ecma.h"

#include <FreeRTOS.h>
#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>
#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_sec.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <hal_uart.h>
#include <hal_sys.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <bl_sys_time.h>
#include <fdt.h>
#include <libfdt.h>
#include <blog.h>


/******************文件操作API******************/
enum FS_MODE
{
    FS_READ = 1,
    FS_WRITE = 2,
    FS_APPEND = 4,
    FS_CREATE = 8,
    FS_OPEN = 16,
    FS_TEXT = 32,
    FS_BIN = 64,
};

int fs_open(char *name, int mode)
{
    return aos_open(name, mode);
}

void fs_close(int fd)
{
    aos_close(fd);
}

int fs_size(int fd)
{
    int length = aos_lseek(fd, 0, SEEK_END);
    aos_lseek(fd, 0, SEEK_SET);
    return length;
}

int fs_read(int fd, char *buf, int len)
{
    return aos_read(fd, buf, len); /**********************************************/
}

int fs_write(int fd, char *buf, int len)
{
    return aos_write(fd, buf, len); /**********************************************/
}

char *evm_open(evm_t *e, char *filename)
{
    int fd;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    fd = fs_open(filename, FS_READ | FS_TEXT);
    if (fd == 0)
        return NULL;
    lSize = fs_size(fd);
    evm_val_t *b = evm_buffer_create(e, sizeof(uint8_t) * lSize + 1);
    buffer = (char *)evm_buffer_addr(b);
    memset(buffer, 0, lSize + 1);
    result = fs_read(fd, buffer, lSize);
    if (!result)
    {
        fs_close(fd);
        return NULL;
    }
    buffer[lSize] = 0;
    fs_close(fd);
    return buffer;
}
/*****************evm文件加载接口*******************/
static int modules_paths_count = 1;
static char *modules_paths[] = {"/romfs"};

const char *vm_load(evm_t *e, char *path, int type)
{
    int file_name_len = strlen(path) + 1;
    char *buffer = NULL;
    if (type == EVM_LOAD_MAIN)
    {
        char *module_name = evm_malloc(file_name_len);
        if (!module_name)
            return NULL;
        sprintf(module_name, "%s", path);
        sprintf(e->file_name, "%s", path);
        buffer = evm_open(e, module_name);
        evm_free(module_name);
    }
    else
    {
        for (int i = 0; i < modules_paths_count; i++)
        {
            int len = strlen(modules_paths[i]) + 1 + file_name_len;
            char *modules_path = evm_malloc(len);
            sprintf(modules_path, "%s/%s", modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = evm_open(e, modules_path);
            evm_free(modules_path);
            if (buffer)
            {
                break;
            }
        }

        if (!buffer)
        {
            const char *module_path = "../../evm/test/eJS/%s";
            int file_name_len = strlen(module_path) + strlen(path) + 1;
            char *module_name = evm_malloc(file_name_len);
            sprintf(module_name, module_path, path);
            sprintf(e->file_name, "%s", path);
            buffer = evm_open(e, module_name);
            evm_free(module_name);
        }
    }
    return buffer;
}

void *vm_malloc(int size)
{
    void *m = pvPortMalloc(size);
    if (m)
        memset(m, 0, size);
    return m;
}

void vm_free(void *mem)
{
    if (mem)
        vPortFree(mem);
}

evm_err_t evm_module_init(evm_t *env)
{
    evm_err_t err;
#ifdef CONFIG_EVM_MODULE_ADC
    err = evm_module_adc(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create adc module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_UART
    err = evm_module_uart(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create uart module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_GPIO
    err = evm_module_gpio(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create gpio module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_FS
    err = evm_module_fs(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create fs module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_NET
    err = evm_module_net(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create net module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_HTTP
    err = evm_module_http(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create http module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_PROCESS
    err = evm_module_process(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create process module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_EVENTS
    err = evm_module_events(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create events module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_DNS
    err = evm_module_dns(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create dns module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_TIMERS
    err = evm_module_timers(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create timers module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_BUFFER
    err = evm_module_buffer(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create buffer module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_ASSERT
    err = evm_module_assert(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create assert module\r\n");
        return err;
    }
#endif
    return ec_ok;
}

void evm_event_thread(void *pvParameters)
{
    evm_t *e = (evm_t *)pvParameters;
    while (1)
    {
#ifdef CONFIG_EVM_MODULE_PROCESS
        evm_module_process_poll(e);
#endif
        vTaskDelay(1);
    }
}

int evm_main()
{
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    evm_t *env = (evm_t *)evm_malloc(sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

    err = evm_module_init(env);
    if (err != ec_ok)
    {
        return err;
    }

    err = ecma_module(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create ecma module\r\n");
        return err;
    }

    evm_module_registry_init(env, EVM_MODULE_REGISTRY_SIZE);

    static StackType_t event_stack[1024];
    static StaticTask_t event_task;

    xTaskCreateStatic(evm_event_thread, "evm-main-task", 1024, NULL, 13, event_stack, &event_task);

#ifdef EVM_LANG_ENABLE_REPL
    evm_repl_run(env, 100, EVM_LANG_JS);
#endif

    // err = evm_boot(env, "hello.js");

    // if (err == ec_no_file)
    // {
    //     evm_print("can't open file\r\n");
    //     return err;
    // }

    err = evm_start(env);
    return err;
}

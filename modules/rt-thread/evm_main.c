#include "evm_module.h"
#include <rtthread.h>

#define SAMPLE_UART_NAME       "uart0"    /* 串口设备名称 */
static rt_device_t serial;                /* 串口设备句柄 */    

/*****************evm交互式终端接口*******************/
char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    char ch;
    while (rt_device_read(serial, -1, &ch, 1) != 1){

    }
    return ch;
}
/******************文件操作API******************/
enum FS_MODE{
    FS_READ = 1,
    FS_WRITE = 2,
    FS_APPEND = 4,
    FS_CREATE = 8,
    FS_OPEN = 16,
    FS_TEXT = 32,
    FS_BIN = 64,
};

void * fs_open(char * name, int mode)
{
    char m[5];
    memset(m, 0, 5);
    if( mode & FS_READ)
        sprintf(m, "%sr", m);

    if( mode & FS_WRITE)
        sprintf(m, "%sw", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    if( mode & FS_BIN)
        sprintf(m, "%sb", m);

    if( mode & FS_APPEND)
        sprintf(m, "%sa", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    return fopen(name, m);
}

void fs_close(void * handle)
{
    fclose((FILE*)handle);
}

int fs_size(void * handle)
{
    FILE *file = (void*)handle;
    fseek (file , 0 , SEEK_END);
    int lSize = ftell (file);
    rewind (file);
    return lSize;
}

int fs_read(void * handle, char * buf, int len)
{
    return fread (buf, 1, len, (FILE*)handle);
}

int fs_write(void * handle, char * buf, int len)
{
    return fwrite(buf, 1, len, (FILE*)handle);
}

char * evm_open(evm_t * e, char *filename){
    FILE *file;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) return NULL;
    lSize = fs_size(file);
    evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*lSize + 1);
    buffer = (char*)evm_buffer_addr(b);
    memset(buffer, 0, lSize + 1);
    result = fs_read(file, buffer, lSize);
    if (!result){
        fclose(file);
        return NULL;
    }
    buffer[lSize] = 0;
    fclose(file);
    return buffer;
}
/*****************evm文件加载接口*******************/
static int modules_paths_count = 2;
static char *modules_paths[] = {
    ".",
    "./evm_modules"
};

const char * vm_load(evm_t * e, char * path, int type)
{
    int file_name_len = strlen(path) + 1;
    char* buffer = NULL;
    if(type == EVM_LOAD_MAIN){
        char * module_name = evm_malloc(file_name_len);
        if( !module_name ) return NULL;
        sprintf(module_name, "%s", path);
        sprintf(e->file_name, "%s", path);
        buffer = evm_open(e, module_name);
        evm_free(module_name);
    } else {
        for(int i=0; i< modules_paths_count; i++){
            int len = strlen(modules_paths[i]) + 1 + file_name_len;
            char* modules_path = evm_malloc(len);
            sprintf(modules_path,  "%s/%s", modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = evm_open(e, modules_path);
            evm_free(modules_path);
            if (buffer){
                break;
            }
        }

        if (!buffer){
            const char * module_path = "../../evm/test/eJS/%s";
            int file_name_len = strlen(module_path) + strlen(path) + 1;
            char * module_name = evm_malloc(file_name_len);
            sprintf(module_name,  module_path, path);
            sprintf(e->file_name, "%s", path);
            buffer = evm_open(e, module_name);
            evm_free(module_name);
        }
    }
    return buffer;
}

void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}

void vm_free(void * mem)
{
    if(mem) free(mem);
}

int evm_main (void) {
    serial = rt_device_find(SAMPLE_UART_NAME);
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    evm_t * env = (evm_t*)evm_malloc(sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

#ifdef CONFIG_EVM_MODULE_ADC
    err = evm_module_adc(env);
    if( err != ec_ok ) {
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_UART
    err = evm_module_uart(env);
    if( err != ec_ok ) {
        return err;
    }
#endif

#ifdef EVM_LANG_ENABLE_REPL
      evm_repl_run(env, 1000, EVM_LANG_JS);
#endif

    err = evm_boot(env, "main.js");

    if (err == ec_no_file){
        evm_print("can't open file\n");
        return err;
    }

    err = evm_start(env);
    return err;
}

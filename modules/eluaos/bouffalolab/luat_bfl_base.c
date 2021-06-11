#include <errno.h>
#include <vfs.h>
#include <aos/kernel.h>
#include <bl_romfs.h>
#include "lua.h"
#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"

#define O_RDONLY 0
#define O_RDWR 2
#define O_CREAT 0x0200
#define O_WRONLY 1
#define O_APPEND 02000

__attribute__((weak)) void luat_timer_us_delay(size_t us) {
    
}

// 文件系统初始化函数, 做个虚拟的
__attribute__((weak)) int luat_fs_init(void) {return 0;}

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    {"rtos", luaopen_rtos},
    {"log", luaopen_log},
    {NULL, NULL}
};

// 按不同的rtconfig加载不同的库函数
void luat_openlibs(lua_State *L) {
    // 初始化队列服务
    luat_msgbus_init();
    //print_list_mem("done>luat_msgbus_init");
    // 加载系统库
    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
        //extern void print_list_mem(const char* name);
        //print_list_mem(lib->name);
    }
}

void luat_os_reboot(int code) {
    
}

void sys_start_standby(int ms);

__attribute__((weak)) void luat_os_standy(int timeout) {
    #ifdef BSP_USING_WM_LIBRARIES
        #ifdef BSP_USING_STANDBY
            sys_start_standby(timeout);
        #endif
    #endif
}

__attribute__((weak)) const char* luat_os_bsp(void) {
    #ifdef BSP_USING_WM_LIBRARIES
        return "w60x";
    #else
        #ifdef STM32L1
            return "stm32";
        #else
            return "_";
        #endif
    #endif
}


__attribute__((weak)) void rt_hw_us_delay(uint32_t us)
{
    ; // nop
}

void luat_os_entry_cri(void) {

}

void luat_os_exit_cri(void) {

}

static const char * _vm_load(evm_t * e, char * path, int type)
{
    static char _path[128];
    int file_name_len = strlen(path) + 1;
    char* buffer = NULL;
    sprintf(_path, "/%s", path);

    struct stat st;
    int ret = stat(_path, &st);
    if (stat(_path, &st) < 0) {
        return NULL;
    }

    int fd = aos_open(_path, O_RDONLY);
    if (fd>= 0) {
        evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*st.st_size + 1);
        buffer = (char*)evm_buffer_addr(b);
        memset(buffer, 0, st.st_size + 1);
        aos_read(fd, buffer, st.st_size);
        aos_close(fd);
    }
    return buffer;
}

static evm_t * env = NULL;

extern evm_t * we_get_runtime(void)
{
    return env;
}

extern int blf_mem_init();

LUA_API lua_State *lua_newstate (lua_Alloc f, void *args) {
    blf_mem_init();
    
    evm_lua_set_allocf(f, args);
    evm_register_print(printf);
    evm_register_file_load((intptr_t)_vm_load);
    env = (evm_t*)evm_malloc(sizeof(evm_t));
    if( env == NULL ) {
        printf("Failed to allocate evm_t\r\n");
        return NULL;
    }
      
    memset(env, 0, sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_LUA_HEAP_SIZE, EVM_LUA_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if( err != ec_ok ) {
        printf("Failed to init evm\r\n");
        return NULL;
    }
    err = ecma_module(env);
    if (err != ec_ok) {
        printf("Failed to init ecma module\r\n");
        return NULL;
    }
    return evm_lua_new_state(env, EVM_LUA_SIZE_OF_GLOBALS);
}

#include "luat_base.h"
#include "luat_malloc.h"

#include "bget.h"

#ifdef BSP_USING_WM_LIBRARIES
    #include "wm_ram_config.h"
    #define LUAT_HEAP_SIZE 64*1024
    #define W600_HEAP_ADDR 0x20028000
    #ifdef RT_USING_WIFI

    #else
    #define W600_MUC_HEAP_SIZE (64*1024)
    ALIGN(RT_ALIGN_SIZE) static char w600_mcu_heap[W600_MUC_HEAP_SIZE]; // MCU模式下, rtt起码剩余140kb内存, 用64kb不过分吧

    #endif
#else
    #ifndef LUAT_HEAP_SIZE
        #ifdef SOC_FAMILY_STM32
            #define LUAT_HEAP_SIZE (400*1024)
        #else
            #define LUAT_HEAP_SIZE 20*1024
        #endif
    #endif
    static char luavm_buff[LUAT_HEAP_SIZE];
#endif

int blf_mem_init() {
    #ifdef BSP_USING_WM_LIBRARIES
    #ifdef RT_USING_WIFI
        // nothing
    #else
        // MUC heap , 占用一部分rtt heap
        bpool((void*)w600_mcu_heap, W600_MUC_HEAP_SIZE);
        // 把wifi的内存全部吃掉
        bpool((void*)WIFI_MEM_START_ADDR, (64 - 16)*1024);
    #endif
    void *ptr = W600_HEAP_ADDR;
    #else
    char *ptr = (char*)luavm_buff;
    memset(ptr, 0, LUAT_HEAP_SIZE);
    #endif
	bpool(ptr, LUAT_HEAP_SIZE);
    return 0;
}


void luat_meminfo_sys(size_t* total, size_t* used, size_t* max_used) {
    
}

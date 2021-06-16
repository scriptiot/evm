#include <stdio.h>

#include "luat_base.h"
#include "luat_malloc.h"

#include "bget.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ecma.h"

#define LUAT_HEAP_SIZE (200*1024)
uint8_t luavm_heap[LUAT_HEAP_SIZE] = {0};

static void _luat_main(void* args) {
    luat_main();
    evm_t *env = luat_get_state()->e;
    while(1) {
        if( env ) {
            ecma_timeout_poll(env);
            ecma_timeout_poll_inc(env, 1);
        }
        vTaskDelay(1);
    }
}

int main(int argc, char** argv) {
    bpool(luavm_heap, LUAT_HEAP_SIZE);
    xTaskCreate( _luat_main, "luatos", 1024*8, NULL, 12, NULL );
    vTaskStartScheduler();
    return 0;
}


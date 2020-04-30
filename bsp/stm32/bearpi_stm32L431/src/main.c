/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr.h>
#include <sys/printk.h>
#include <evm.h>

char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    char c = console_getchar();
    return c;
}

const char * vm_load(evm_t * e, char * path, int type){


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


void main(void)
{
	console_init();

    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printk);
    evm_register_file_load((intptr_t)vm_load);

    // 初始化虚拟机
    int32_t head_size = 20 * 1024;
    int32_t stack_size = 20 * 1024;
    int32_t module_size = 5;
    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, head_size, stack_size, module_size, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);


    evm_repl_run(env, 1000, EVM_LANG_JS);
}

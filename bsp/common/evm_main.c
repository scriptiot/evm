/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#include "evm_main.h"

#if CONFIG_EVM_ECMA
#include "ecma.h"
#endif

#if CONFIG_EVM_LANG_QML
#include "ecma.h"
#include "qml_lvgl_module.h"
#endif

#if CONFIG_EVM_LANG_PYTHON
#include "python_builtins.h"
#endif

#if CONFIG_EVM_HEATSHRINK
#include "wrap_heatshrink.h"
#endif

char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    char c = console_getchar();
    return c;
}

const char * vm_load(evm_t * e, char * path, int type){
    return NULL;
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

const struct uart_config uart_cfg = {
    .baudrate = 115200,
    .parity = UART_CFG_PARITY_NONE,
    .stop_bits = UART_CFG_STOP_BITS_1, 
    .data_bits = UART_CFG_DATA_BITS_8,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};

void console_setup(){
    struct device *uart_dev = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);
    uart_configure(uart_dev, &uart_cfg);
    console_init();	
}

int evm_main(void){
    int lang_type = EVM_LANG_JS;
    console_setup();

    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printk);
    evm_register_file_load((intptr_t)vm_load);

    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if( err ) {
        evm_print("Failed to initialize evm\r\n");
        return err;
    }
    err = evm_module(env);
    if( err ) {
        evm_print("Failed to add evm module\r\n");
        return err;
    }
#if CONFIG_EVM_HEATSHRINK
    err = heatshrink_module(env);
    if( err ) {
        evm_print("Failed to add evm module\r\n");
        return err;
    }
#endif

#if CONFIG_EVM_LANG_QML
    err = qml_lvgl_module(env);
    if( err ) {
        evm_print("Failed to add qml module\r\n");
    }

    evm_boot(env, "main.qml");
    
    evm_start(env);

    while(1){
        lv_task_handler();
        k_msleep(10);
    }
#endif
    
#if CONFIG_EVM_LANG_PYTHON
    python_builtins(env);
    if( err ) {
        evm_print("Failed to add python builtins module\r\n");
    }
    lang_type = EVM_LANG_PY;
#endif

#if CONFIG_EVM_ECMA
    err = ecma_module(env);
    if( err ) {
        evm_print("Failed to add ecma module\r\n");
    }
#endif
    err = evm_repl_run(env, 20, lang_type);
    while( err == ec_exit ){
        evm_poll_callbacks(env);
    }
    return err;
}
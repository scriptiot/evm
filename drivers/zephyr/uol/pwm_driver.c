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

#include "nevm.h"

#if CONFIG_EVM_PWM
#include <pwm.h>
#include <zephyr.h>
#endif

//pwm_config(devname, pin, period, pulse)
evm_val_t nevm_driver_pwm_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
#if CONFIG_EVM_PWM
    const char * name = evm_2_string(v);
    struct device *dev = device_get_binding(name);
    if( !dev ) return NEVM_FALSE;
    if( pwm_pin_set_nsec(dev, evm_2_integer(v + 1), evm_2_integer(v + 2), evm_2_integer(v + 3)) == 0 )
        return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_pwm_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
#if CONFIG_EVM_PWM
    if( argc > 0){
        const char * name = evm_2_string(v);
        struct device *dev = device_get_binding(name);
        if( !dev ) return NEVM_FALSE;
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_pwm_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

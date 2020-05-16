/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
**  Licence: Apache-2.0
****************************************************************************/

#include "nevm.h"

#ifdef CONFIG_EVM_DAC
#include <device.h>
#include <drivers/dac.h>
#endif

//dac_config(char channel)
evm_val_t nevm_driver_dac_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_DAC
    if( argc > 0 && evm_is_number(v) ){
        struct device * dev = (struct device *)nevm_object_get_ext_data(p);
        if( !dev ) return NEVM_FALSE;

        struct dac_channel_cfg cfg = {
            .channel_id = evm_2_integer(v),
            .resolution = 12,
        };
        dac_channel_setup(dev, &cfg);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

//dac_init(String dev)
evm_val_t nevm_driver_dac_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_DAC
    if( argc > 0 && evm_is_string(v) ){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        nevm_object_set_ext_data(p, (intptr_t)dev);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_dac_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_TRUE;
}
//dac_set_value(char channel, int value)
evm_val_t nevm_driver_dac_set_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_DAC
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    dac_write_value(dev, evm_2_integer(v), evm_2_integer(v + 1));
#endif
    return EVM_VAL_UNDEFINED;
}

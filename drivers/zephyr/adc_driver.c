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

#ifdef CONFIG_EVM_ADC

#define ADC_BUFFER_SIZE 2

#include <device.h>
#include <drivers/adc.h>
#endif

//adc_config(char channel)
evm_val_t nevm_driver_adc_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_ADC
    struct device * adc_dev = (struct device *)nevm_object_get_ext_data(p);
    if( !adc_dev ) return NEVM_FALSE;

    struct adc_channel_cfg m_channel_cfg = {
        .channel_id = evm_2_integer(v),
		.differential = 0,
		// .input_positive = 0,
		// .input_negative = 1,
		.reference = ADC_REF_INTERNAL,
		.gain = ADC_GAIN_1,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT,
    };

    int ret = adc_channel_setup(adc_dev, &m_channel_cfg);
    if(ret != 0) return NEVM_FALSE;

    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

//adc_init(String dev)
evm_val_t nevm_driver_adc_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_ADC
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    nevm_object_set_ext_data(p, (intptr_t)dev);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_adc_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

//adc_get_value(char channel)
evm_val_t nevm_driver_adc_get_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_ADC
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    char channel = evm_2_integer(v);
    uint8_t seq_buffer[ADC_BUFFER_SIZE];

    struct adc_sequence seq = {
		.options = NULL,
		.channels = BIT(channel),
		.buffer = &seq_buffer,
		.buffer_size = sizeof(seq_buffer),
		.resolution = 12,
		.oversampling = 0,
		.calibrate = 0
	};

    if (adc_read(dev, &seq) != 0) {
        return NEVM_FALSE;
    }

    return evm_mk_number( seq_buffer[0] | seq_buffer[1] << 8 );
#endif
    return EVM_VAL_UNDEFINED;
}

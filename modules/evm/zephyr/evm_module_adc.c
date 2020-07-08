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


#include "evm_module.h"

#define ADC_BUFFER_SIZE 2

#include <device.h>
#include <drivers/adc.h>
/**
 * @brief ADC class constructor
 * 
 * @param name device name
 * 
 * @uasge new ADC('ADC_1') 
 */
//ADC(devName, channel)
static evm_val_t evm_module_adc(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 1 && evm_is_string(v) && evm_is_number(v + 1) ){
		struct device * dev = device_get_binding(evm_2_string(v));
		if( !dev ) {
			evm_set_err(e, ec_type, "Can't find ADC device");
			return EVM_VAL_UNDEFINED;
		}
		evm_object_set_ext_data(p, (intptr_t)dev);

		struct adc_channel_cfg m_channel_cfg = {
			.channel_id = evm_2_integer(v),
			.differential = 0,
			.reference = ADC_REF_INTERNAL,
			.gain = ADC_GAIN_1,
			.acquisition_time = ADC_ACQ_TIME_DEFAULT,
		};

		adc_channel_setup(dev, &m_channel_cfg);
		evm_prop_set_value(e, p, "channel", *v);
	}
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief read adc value
 * 
 * @uasge ADC.read()
 */
static evm_val_t evm_module_adc_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
    char channel = evm_2_integer( evm_prop_get(e, p, "channel", 0) );
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
        return EVM_VAL_FALSE;
    }

    return evm_mk_number( seq_buffer[0] | seq_buffer[1] << 8 );
}

evm_val_t evm_class_adc(evm_t *e)
{
	evm_builtin_t class_adc[] = {
		{"read", evm_mk_native((intptr_t)evm_module_adc_read)},
		{"channel", evm_mk_number(0)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_module_adc, class_adc, NULL);
}


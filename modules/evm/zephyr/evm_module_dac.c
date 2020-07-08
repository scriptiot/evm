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

#include <device.h>
#include <drivers/dac.h>
/**
 * @brief DAC class constructor
 * 
 * @param name device name
 * 
 * @uasge new DAC('DAC_1') 
 */
//DAC(name, channel)
static evm_val_t evm_module_dac(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 0 && evm_is_string(v) && evm_is_number(v) ){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) {
			evm_set_err(e, ec_type, "Can't find DAC device");
			return EVM_VAL_UNDEFINED;
		}

        struct dac_channel_cfg cfg = {
            .channel_id = evm_2_integer(v + 1),
            .resolution = 12,
        };

		evm_prop_set_value(e, p, "channel", *(v + 1) );
        dac_channel_setup(dev, &cfg);
        evm_object_set_ext_data(p, (intptr_t)dev);
    }
	return EVM_VAL_UNDEFINED;
}

/**
 * @brief dac write value
 * 
 * @param value 
 * 
 * @uasge DAC.write(1024) 
 */
static evm_val_t evm_module_dac_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = (struct device *)evm_object_get_ext_data(p);

	if( dev && argc > 0 ){
		int channel = evm_2_integer( evm_prop_get(e, p, "channel", 0) );
		dac_write_value(dev, channel, evm_2_integer(v));
	}
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_dac(evm_t * e){
	evm_builtin_t class_dac[] = {
		{"write", evm_mk_native( (intptr_t)evm_module_dac_write )},
		{"channel", evm_mk_number(0)},
		{NULL, EVM_VAL_UNDEFINED}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_dac, class_dac, NULL);
}

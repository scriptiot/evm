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

//LED(id)
static evm_val_t evm_module_led(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(argc);

	int id = evm_2_integer(v);
	if( id >= evm_board_get_pin_group_size(ledgroup) ) {
		evm_set_err(e, ec_index, "Out of led index");
		return EVM_VAL_UNDEFINED;
	}

	struct device * dev = device_get_binding(ledgroup[id].port);
	if( !dev ) {
		evm_set_err(e, ec_type, "Can't find Pin device");
		return EVM_VAL_UNDEFINED;
	}

	gpio_pin_configure(dev, (gpio_pin_t)ledgroup[id].pin, (gpio_flags_t)ledgroup[id].mode);
	evm_prop_set_value(e, p, "id", *v);
	evm_object_set_ext_data(p, (intptr_t)dev);
	return EVM_VAL_UNDEFINED;
}
//off()
static evm_val_t evm_module_led_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
	if( dev ){
		int id = evm_2_integer( evm_prop_get(e, p, "id", 0) );
		if( id == -1 ) return EVM_VAL_UNDEFINED;
		gpio_pin_set(dev, (gpio_pin_t)ledgroup[id].pin, 0 );
	}
	return EVM_VAL_UNDEFINED;
}
//on()
static evm_val_t evm_module_led_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
	if( dev ){
		int id = evm_2_integer( evm_prop_get(e, p, "id", 0) );
		if( id == -1 ) return EVM_VAL_UNDEFINED;
		gpio_pin_set(dev, (gpio_pin_t)ledgroup[id].pin, 1 );
	}
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_led(evm_t * e){
	evm_builtin_t class_led[] = {
		{"id", evm_mk_number( -1 )},
		{"off", evm_mk_native( (intptr_t)evm_module_led_off )},
		{"on", evm_mk_native( (intptr_t)evm_module_led_on )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_led, class_led, NULL);
}


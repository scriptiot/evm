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

#ifdef CONFIG_EVM_LED
#include "evm_module.h"

//LED(id)
static evm_val_t evm_module_led(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	int id = evm_2_integer(v);
	if( id >= evm_board_get_pin_group_size(ledgroup) ) {
		evm_set_err(e, ec_index, "Out of led index");
		return EVM_VAL_UNDEFINED;
	}
	evm_val_t params[3];
	params[0] = evm_mk_foreign_string(ledgroup[id].port);
	params[1] = evm_mk_number(ledgroup[id].pin);
	params[2] = evm_mk_number(ledgroup[id].mode);
	evm_module_construct(nevm_runtime, p, 3, &params, EXPORT_main_pinCreate, EXPORT_PinDevice_open);
	return EVM_VAL_UNDEFINED;
}
//off()
static evm_val_t evm_module_led_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	evm_val_t val = evm_mk_number(0);
	nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_write, 1, &val);
	return EVM_VAL_UNDEFINED;
}
//on()
static evm_val_t evm_module_led_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	evm_val_t val = evm_mk_number(1);
	nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_write, 1, &val);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_led(evm_t * e){
	evm_builtin_t class_led[] = {
		{"off", evm_mk_native( (intptr_t)evm_module_led_off )},
		{"on", evm_mk_native( (intptr_t)evm_module_led_on )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_led, class_led, NULL);
}
#endif

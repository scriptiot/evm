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

#ifdef CONFIG_EVM_SWITCH
#include "evm_module.h"

//Switch(id)
/**
 * @brief Switch class constructor
 * 
 * @param id It's an integer which tells the index of your defined switches.
 * @usage new Switch(0)
 */
static evm_val_t evm_module_switch(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	int id = evm_2_integer(v);
	if( id >= evm_board_get_pin_group_size(keygroup) ) {
		evm_set_err(e, ec_index, "Out of key index");
		return EVM_VAL_UNDEFINED;
	}
	evm_val_t params[3];
	params[0] = evm_mk_foreign_string(keygroup[id].port);
	params[1] = evm_mk_number(keygroup[id].pin);
	params[2] = evm_mk_number(keygroup[id].mode);
	evm_module_construct(nevm_runtime, p, 3, &params, EXPORT_main_pinCreate, EXPORT_PinDevice_open);
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief return the logic level of the pin
 * 
 * @usage Switch.value() 
 */
static evm_val_t evm_module_switch_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_read, 0, NULL);
}
/**
 * @brief set a callback function which will be triggered if switch is pressed. 
 * 
 * @param callback function
 * @usage Switch.callback(function)
 */
static evm_val_t evm_module_switch_callback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_setCallback, 1, v);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_switch(evm_t * e){
	evm_builtin_t class_switch[] = {
		{"value", evm_mk_native( (intptr_t)evm_module_switch_value )},
		{"callback", evm_mk_native( (intptr_t)evm_module_switch_callback )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_switch, class_switch, NULL);
}
#endif

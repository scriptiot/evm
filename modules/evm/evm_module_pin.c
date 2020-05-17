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

#ifdef CONFIG_EVM_GPIO

#include "evm_module.h"

//Pin(name, pin, flags)
static evm_val_t evm_module_pin(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_pinCreate, EXPORT_PinDevice_open);
	return EVM_VAL_UNDEFINED;
}
//Pin.value(value)
static evm_val_t evm_module_pin_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	if( argc == 0 ){
		return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_read, 0, NULL);
	} else if( argc == 1 ) {
		nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_PinDevice_write, 1, v);
	}
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_pin(evm_t * e){
	evm_builtin_t cls[] = {
		{"value", evm_mk_native( (intptr_t)evm_module_pin_value )},	
		{"IN", evm_mk_number(CONFIG_EVM_GPIO_INPUT)},	
		{"OUT", evm_mk_number(CONFIG_EVM_GPIO_OUTPUT)},
		{"DISCONNECTED", evm_mk_number(CONFIG_EVM_GPIO_DISCONNECTED)},
		{"INT_ENABLE", evm_mk_number(CONFIG_EVM_GPIO_INT_ENABLE)},
		{"INT_DISABLE", evm_mk_number(CONFIG_EVM_GPIO_INT_DISABLE)},
		{"INT_EDGE_RISING", evm_mk_number(CONFIG_EVM_GPIO_INT_EDGE_RISING)},
		{"INT_EDGE_FALLING", evm_mk_number(CONFIG_EVM_GPIO_INT_EDGE_FALLING)},
		{"INT_EDGE_BOTH", evm_mk_number(CONFIG_EVM_GPIO_INT_EDGE_BOTH)},
		{"PULL_UP", evm_mk_number(CONFIG_EVM_GPIO_PUD_PULL_UP)},
		{"PULL_DOWN", evm_mk_number(CONFIG_EVM_GPIO_PUD_PULL_DOWN)},	
		{"OPEN_SOURCE", evm_mk_number(CONFIG_EVM_GPIO_OPEN_SOURCE)},
		{"OPEN_DRAIN", evm_mk_number(CONFIG_EVM_GPIO_OPEN_DRAIN)},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_pin, cls, NULL);
}

#endif

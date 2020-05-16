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

#ifdef CONFIG_EVM_TIMER
#include "evm_module.h"

//Timer(String name)
static evm_val_t evm_module_timer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_timerCreate, EXPORT_TimerDevice_open);
	return EVM_VAL_UNDEFINED;
}

//Timer.setInterval(callback, period)
static evm_val_t evm_module_timer_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_TimerDevice_start, 2, v);
}

//Timer.clearInterval(id)
static evm_val_t evm_module_timer_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_timer(evm_t * e){
	evm_builtin_t class_timer[] = {
		{"setInterval", evm_mk_native( (intptr_t)evm_module_timer_setInterval )},
		{"clearInterval", evm_mk_native( (intptr_t)evm_module_timer_clearInterval )},
		{"id", evm_mk_number(-1)},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_timer, class_timer, NULL);
}
#endif

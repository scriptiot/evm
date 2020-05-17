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

#ifdef CONFIG_EVM_I2C
#include "evm_module.h"

//I2C(bus, ...)
static evm_val_t evm_module_i2c(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_i2c(evm_t * e){
	evm_builtin_t class_i2c[] = {
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_class_i2c, class_i2c, NULL);
}
#endif

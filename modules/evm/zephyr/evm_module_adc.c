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

/**
 * @brief ADC class constructor
 * 
 * @param name device name
 * 
 * @uasge new ADC('ADC_1') 
 */
static evm_val_t evm_module_adc(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);

	evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_adcCreate, EXPORT_ADCDeivce_open);
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief read adc value
 * 
 * @uasge ADC.read()
 */
static evm_val_t evm_module_adc_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);

	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_ADCDeivce_read, 0, NULL);
}

evm_val_t evm_class_adc(evm_t *e)
{
	evm_builtin_t class_adc[] = {
		{"read", evm_mk_native((intptr_t)evm_module_adc_read)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_module_adc, class_adc, NULL);
}


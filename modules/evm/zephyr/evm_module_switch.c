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
#include <drivers/gpio.h>

typedef struct gpio_handle {
	struct device * dev;
	struct gpio_callback callback;
    int obj;
	int cb;
    int id;  
} gpio_handle_t;

void evm_switch_c_callback(gpio_handle_t * handle){
	if( handle->cb != -1 ){
		evm_val_t * fn = evm_get_reference(handle->cb);
    	evm_run_callback(evm_runtime, fn, NULL, NULL, 0);
	}
}

static void evm_driver_gpio_callback(struct device *gpio, struct gpio_callback *cb, u32_t pins)
{
	gpio_handle_t *handle = CONTAINER_OF(cb, gpio_handle_t, callback);
    evm_add_callback(evm_switch_c_callback, handle);
}

//Switch(id)
/**
 * @brief Switch class constructor
 * 
 * @param id It's an integer which tells the index of your defined switches.
 * @usage new Switch(0)
 */
static evm_val_t evm_module_switch(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 0 ){
		int id = evm_2_integer(v);
		if( id >= evm_board_get_pin_group_size(keygroup) ) {
			evm_set_err(e, ec_index, "Out of led index");
			return EVM_VAL_UNDEFINED;
		}

		struct device * dev = device_get_binding(keygroup[id].port);
		if( !dev ) {
			evm_set_err(e, ec_type, "Can't find switch device");
			return EVM_VAL_UNDEFINED;
		}

		gpio_handle_t * handle = evm_malloc(sizeof(gpio_handle_t));
		if( !handle ) {
			evm_set_err(e, ec_type, "Out of memory");
			return EVM_VAL_UNDEFINED;
		}

		handle->cb = -1;
		handle->id = id;
		handle->dev = dev;
		handle->obj = evm_add_reference(*p);

		gpio_pin_configure(dev, (gpio_pin_t)keygroup[id].pin, (gpio_flags_t)keygroup[id].mode);
		evm_object_set_ext_data(p, (intptr_t)handle);

		gpio_init_callback(&handle->callback, evm_driver_gpio_callback, BIT(keygroup[id].pin));

		gpio_add_callback(dev, &handle->callback);
		gpio_pin_enable_callback(dev, keygroup[id].pin);
	}
	
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief return the logic level of the pin
 * 
 * @usage Switch.value() 
 */
static evm_val_t evm_module_switch_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	gpio_handle_t * handle = (gpio_handle_t *)evm_object_get_ext_data(p);
	if( handle ) {
		int id = handle->id;
		return evm_mk_number( gpio_pin_get(handle->dev, (gpio_pin_t)keygroup[id].pin ) );
	}
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief set a callback function which will be triggered if switch is pressed. 
 * 
 * @param callback function
 * @usage Switch.callback(function)
 */
static evm_val_t evm_module_switch_callback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	gpio_handle_t * handle = (gpio_handle_t *)evm_object_get_ext_data(p);
	if( handle && argc > 0 && evm_is_script(v) ) {
		evm_remove_reference(handle->cb);
		handle->cb = evm_add_reference(*v);
	}
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


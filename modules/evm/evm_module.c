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

#ifdef CONFIG_EVM_GPIO
	extern evm_val_t evm_class_pin(evm_t * e);
#endif
#ifdef CONFIG_EVM_LCD
	extern evm_val_t evm_class_lcd(evm_t * e);
#endif
#ifdef CONFIG_EVM_LED
	extern evm_val_t evm_class_led(evm_t * e);
#endif
#ifdef CONFIG_EVM_FLASH
	extern evm_val_t evm_class_flash(evm_t * e);
#endif
#ifdef CONFIG_EVM_ADC
	extern evm_val_t evm_class_adc(evm_t * e);
#endif
#ifdef CONFIG_EVM_DAC
	extern evm_val_t evm_class_dac(evm_t * e);
#endif
#ifdef CONFIG_EVM_RTC
	extern evm_val_t evm_class_rtc(evm_t * e);
#endif
#ifdef CONFIG_EVM_SWITCH
	extern evm_val_t evm_class_switch(evm_t * e);
#endif
#ifdef CONFIG_EVM_TIMER
	extern evm_val_t evm_class_timer(evm_t * e);
#endif
#ifdef CONFIG_EVM_LED
	extern evm_val_t evm_class_led(evm_t * e);
#endif
#ifdef CONFIG_EVM_UART
	extern evm_val_t evm_class_uart(evm_t * e);
#endif
#ifdef CONFIG_EVM_WATCHDOG
	extern evm_val_t evm_class_watchdog(evm_t * e);
#endif
#ifdef CONFIG_EVM_WIFI
	extern evm_val_t evm_class_wifi(evm_t * e);
#endif
#ifdef CONFIG_EVM_SOCKET
	extern evm_val_t evm_class_socket(evm_t * e);
#endif
#ifdef CONFIG_EVM_NETWORK
	extern evm_val_t evm_class_network(evm_t * e);
#endif

static evm_val_t * callback_list;
static evm_val_t * args_list;
static uint32_t number_of_callbacks;
evm_t * evm_runtime;

void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, uint16_t constructor_api, uint16_t open_api){
    evm_val_t dev = nevm_function_invoke(e, constructor_api, argc, v);
	if( dev == EVM_VAL_NULL ){
		evm_set_err(evm_runtime, ec_type, "Driver is not found");
		return;
	}
    nevm_object_function_invoke(e, &dev, open_api, 0, NULL);
    nevm_object_set_ext_data(p, evm_2_intptr(&dev) );
}

int evm_module_add_callback(evm_val_t callback, evm_val_t args){
	if( !evm_is_script(&callback) ) return -1;
	if( !evm_is_list(&args) && !evm_is_null(&args) && !evm_is_undefined(&args) ) return -1;
	
	for(uint32_t i = 0; i < number_of_callbacks; i++){
		evm_val_t * v = evm_list_get(evm_runtime, callback_list, i);
		if( evm_is_undefined(v) ){
			evm_list_set(evm_runtime, callback_list, i, callback);
			evm_list_set(evm_runtime, args_list, i, args);
			return i;
		}
	}
	return -1;
}

evm_val_t * evm_module_get_callback(int id){
	return evm_list_get(evm_runtime, callback_list, id);
}

int evm_module_remove_callback(int id){
	if( id >= number_of_callbacks ) return ec_index;
	evm_list_set(evm_runtime, callback_list, id, EVM_VAL_UNDEFINED);
	evm_list_set(evm_runtime, args_list, id, EVM_VAL_UNDEFINED);
	return ec_ok;
}
/**
 * @brief 硬件延时
 * 
 * @param ms 延时毫秒
 * @usage m.delay_ms(100) 
 */
static evm_val_t evm_module_delay_ms(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	nevm_function_invoke(nevm_runtime, EXPORT_main_sysDelayMs, argc, v);
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief 硬件延时
 * 
 * @param ms 延时微秒
 * @usage m.delay_us(100) 
 */
static evm_val_t evm_module_delay_us(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	nevm_function_invoke(nevm_runtime, EXPORT_main_sysDelayUs, argc, v);
	return EVM_VAL_UNDEFINED;
}

int evm_module(evm_t * e){
	evm_runtime = e;
	number_of_callbacks = CONFIG_EVM_CALLBACK_SIZE;
	callback_list = evm_list_create(e, GC_LIST, number_of_callbacks);
	if( !callback_list ) return e->err;

	for(uint32_t i = 0; i < number_of_callbacks; i++){
		evm_list_set(e, callback_list, i, EVM_VAL_UNDEFINED);
	}

	
	args_list = evm_list_create(e, GC_LIST, number_of_callbacks);
	if( !args_list ) return e->err;

	evm_builtin_t module[] = {
		{"delay_ms", evm_mk_native((intptr_t)evm_module_delay_ms)},
		{"delay_us", evm_mk_native((intptr_t)evm_module_delay_us)},
#ifdef CONFIG_EVM_GPIO
		{"Pin", evm_class_pin(e)},
#endif
#ifdef CONFIG_EVM_LCD
		{"LCD", evm_class_lcd(e)},
#endif
#ifdef CONFIG_EVM_LED
		{"LED", evm_class_led(e)},
#endif
#ifdef CONFIG_EVM_FLASH
		{"Flash", evm_class_flash(e)},
#endif
#ifdef CONFIG_EVM_ADC
		{"ADC", evm_class_adc(e)},
#endif
#ifdef CONFIG_EVM_DAC
		{"DAC", evm_class_dac(e)},
#endif
#ifdef CONFIG_EVM_RTC
		{"RTC", evm_class_rtc(e)},
#endif
#ifdef CONFIG_EVM_SWITCH
		{"Switch", evm_class_switch(e)},
#endif
#ifdef CONFIG_EVM_WATCHDOG
		{"Watchdog", evm_class_watchdog(e)},
#endif
#ifdef CONFIG_EVM_TIMER
		{"Timer", evm_class_timer(e)},
#endif
#ifdef CONFIG_EVM_UART
		{"UART", evm_class_uart(e)},
#endif
#ifdef CONFIG_EVM_WIFI
		{"WIFI", evm_class_wifi(e)},
#endif
#ifdef CONFIG_EVM_SOCKET
        {"SOCKET", evm_class_socket(e)},
#endif
#ifdef CONFIG_EVM_NETWORK
        {"SOCKET", evm_class_network(e)},
#endif
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "evm", module);
	return e->err;
}


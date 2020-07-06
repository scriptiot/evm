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
#include <zephyr.h>

#if CONFIG_EVM_MODULE_PIN
	extern evm_val_t evm_class_pin(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_LCD
	extern evm_val_t evm_class_lcd(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_LED
	extern evm_val_t evm_class_led(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_FLASH
	extern evm_val_t evm_class_flash(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_ADC
	extern evm_val_t evm_class_adc(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_DAC
	extern evm_val_t evm_class_dac(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_RTC
	extern evm_val_t evm_class_rtc(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_SWITCH
	extern evm_val_t evm_class_switch(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_TIMER
	extern evm_val_t evm_class_timer(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_LED
	extern evm_val_t evm_class_led(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_UART
	extern evm_val_t evm_class_uart(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_WATCHDOG
	extern evm_val_t evm_class_watchdog(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_WIFI
	extern evm_val_t evm_class_wifi(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_SOCKET
	extern evm_val_t evm_class_socket(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_NETWORK
	extern evm_val_t evm_class_network(evm_t * e);
#endif
#if CONFIG_EVM_MODULE_MODBUS_RTU_MASTER
	extern evm_val_t evm_class_modbus_rtu_master(evm_t * e);
#endif

#ifndef CONFIG_EVM_MODULE_CALLBACK_SIZE
	#define CONFIG_EVM_MODULE_CALLBACK_SIZE	16
#endif

static struct k_sem evm_callback_sem;
static intptr_t cb_map[CONFIG_EVM_MODULE_CALLBACK_SIZE];
static intptr_t cb_handles[CONFIG_EVM_MODULE_CALLBACK_SIZE];

static evm_val_t * reference_list;
static uint32_t number_of_references;
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

int evm_add_reference(evm_val_t ref){
	for(uint32_t i = 0; i < number_of_references; i++){
		evm_val_t * v = evm_list_get(evm_runtime, reference_list, i);
		if( evm_is_undefined(v) ){
			evm_list_set(evm_runtime, reference_list, i, ref);
			return i;
		}
	}
	return -1;
}

evm_val_t * evm_get_reference(int id){
	return evm_list_get(evm_runtime, reference_list, id);
}

int evm_remove_reference(int id){
	if( id >= number_of_references ) return ec_index;
	evm_list_set(evm_runtime, reference_list, id, EVM_VAL_UNDEFINED);
	return ec_ok;
}

int evm_add_callback(evm_callback_func fn, void * handle){
	for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++){
		if( cb_map[i] == 0) {
			cb_map[i] = (intptr_t)fn;
			cb_handles[i] = (intptr_t)handle;
			k_sem_give(&evm_callback_sem);
			return i;
		}
	}
	return -1;
}

void evm_poll_callbacks(evm_t *e){
	while(1){
		k_sem_take(&evm_callback_sem, K_FOREVER);
		for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++) {
			evm_callback_func fn = (evm_callback_func)cb_map[i];
			if( fn == NULL) break;
			void * handle = (void*)cb_handles[i];
			fn(handle);
		}
	}
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
	k_sem_init(&evm_callback_sem, 0, UINT_MAX);
	evm_runtime = e;
	number_of_references = CONFIG_EVM_MODULE_CALLBACK_SIZE;
	reference_list = evm_list_create(e, GC_LIST, number_of_references);
	if( !reference_list ) return e->err;

	for(uint32_t i = 0; i < number_of_references; i++){
		evm_list_set(e, reference_list, i, EVM_VAL_UNDEFINED);
	}

	for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++){
		cb_map[i] = NULL;
		cb_handles[i] = NULL;
	}

	evm_builtin_t module[] = {
		{"delay_ms", evm_mk_native((intptr_t)evm_module_delay_ms)},
		{"delay_us", evm_mk_native((intptr_t)evm_module_delay_us)},
#if CONFIG_EVM_MODULE_MODBUS_RTU_MASTER
        {"ModbusRTUMaster", evm_class_modbus_rtu_master(e)},
#endif
#if CONFIG_EVM_MODULE_PIN
		{"Pin", evm_class_pin(e)},
#endif
#if CONFIG_EVM_MODULE_LCD
		{"LCD", evm_class_lcd(e)},
#endif
#if CONFIG_EVM_MODULE_LED
		{"LED", evm_class_led(e)},
#endif
#if CONFIG_EVM_MODULE_FLASH
		{"Flash", evm_class_flash(e)},
#endif
#if CONFIG_EVM_MODULE_ADC
		{"ADC", evm_class_adc(e)},
#endif
#if CONFIG_EVM_MODULE_DAC
		{"DAC", evm_class_dac(e)},
#endif
#if CONFIG_EVM_MODULE_RTC
		{"RTC", evm_class_rtc(e)},
#endif
#if CONFIG_EVM_MODULE_SWITCH
		{"Switch", evm_class_switch(e)},
#endif
#if CONFIG_EVM_MODULE_WATCHDOG
		{"Watchdog", evm_class_watchdog(e)},
#endif
#if CONFIG_EVM_MODULE_TIMER
		{"Timer", evm_class_timer(e)},
#endif
#if CONFIG_EVM_MODULE_UART
		{"UART", evm_class_uart(e)},
#endif
#if CONFIG_EVM_MODULE_WIFI
		{"WIFI", evm_class_wifi(e)},
#endif
#if CONFIG_EVM_MODULE_SOCKET
        {"Socket", evm_class_socket(e)},
#endif
#if CONFIG_EVM_MODULE_NETWORK
        {"Network", evm_class_network(e)},
#endif

        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "evm", module);
	return e->err;
}


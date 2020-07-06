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

typedef struct evm_timer_t {
	int obj;
	int cb;
} evm_timer_t;

static void timer_callback(struct k_timer *handle)
{
    evm_timer_t * timer = (evm_timer_t*)handle->user_data;
	evm_val_t * obj = evm_get_reference(timer->obj);
	evm_val_t * cb = evm_get_reference(timer->cb);
    evm_run_callback(evm_runtime, cb, NULL, NULL, 0);
}

//Timer(String name)
static evm_val_t evm_module_timer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);

	struct k_timer * dev = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
    if( !dev ) {
		evm_set_err(e, ec_type, "Can't find timer device");
		return EVM_VAL_UNDEFINED;
	}

	k_timer_init(dev, timer_callback, NULL);

	evm_timer_t * timer = evm_malloc(sizeof(evm_timer_t));

    evm_object_set_ext_data(p, (intptr_t)dev);
    dev->user_data = (void *)timer;

	timer->obj = evm_add_reference(*p);
	timer->cb = -1;

	return EVM_VAL_UNDEFINED;
}

//Timer.setInterval(callback, period)
static evm_val_t evm_module_timer_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 1 ){
		struct k_timer * dev = (struct k_timer *)evm_object_get_ext_data(p);
		if( !dev ) {
			evm_set_err(e, ec_type, "Can't find timer device");
			return EVM_VAL_UNDEFINED;
		}
		evm_timer_t * timer = (evm_timer_t*)dev->user_data;
		if( timer->cb != -1 ) {
			evm_remove_reference(timer->cb);
		}
		timer->cb = evm_add_reference(*v);

		int period = evm_2_double(v + 1);
		k_timer_start(dev, K_MSEC(period), K_MSEC(period));
		return evm_mk_number(timer->cb);
	}
	return EVM_VAL_UNDEFINED;
}

//Timer.clearInterval(id)
static evm_val_t evm_module_timer_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_timer(evm_t * e){
	evm_builtin_t class_timer[] = {
		{"setInterval", evm_mk_native( (intptr_t)evm_module_timer_setInterval )},
		{"clearInterval", evm_mk_native( (intptr_t)evm_module_timer_clearInterval )},
		{NULL, EVM_VAL_UNDEFINED}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_timer, class_timer, NULL);
}


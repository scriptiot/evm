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

#ifdef CONFIG_EVM_WATCHDOG
#include "evm_module.h"

/**
 *  @brief  create watchdog device
 *
 *  @param  name            : Watchdog device name define in dts
 *
 *  @return  0 on success, negative errno code on fail.

	@usage: new Watchdog(name)
	@demo: new Watchdog('watchdog0')
 */
static evm_val_t evm_module_watchdog(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{	
	evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_watchdogCreate, EXPORT_WatchDogDevice_open);
	return EVM_VAL_UNDEFINED;
}

/**
 * @brief Set up watchdog instance.
 *
 * This function is used for configuring global watchdog settings that
 * affect all timeouts. It should be called after installing timeouts.
 * After successful return, all installed timeouts are valid and must be
 * serviced periodically by calling wdt_feed().
 *
 * @param options Configuration options as defined by the WDT_OPT_* constants
 *        0   Pause watchdog timer when CPU is in sleep state.
 *        1   Pause watchdog timer when CPU is halted by the debugger.
 *
 * @retval 0 If successful.
 * @retval -ENOTSUP If any of the set options is not supported.
 * @retval -EBUSY If watchdog instance has been already setup.
 * @usage: Watchdog.setup(options)
 * @demo: Watchdog.setup(0)
 */
static evm_val_t evm_module_watchdog_setup(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_WatchDogDevice_setup, 1, v);
}

/**
 * @brief Install new timeout.
 *
 * This function must be used before wdt_setup(). Changes applied here
 * have no effects until wdt_setup() is called.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cfg Pointer to timeout configuration structure.
 *
 * @retval channel_id If successful, a non-negative value indicating the index
 *                    of the channel to which the timeout was assigned. This
 *                    value is supposed to be used as the parameter in calls to
 *                    wdt_feed().
 * @retval -EBUSY If timeout can not be installed while watchdog has already
 *		  been setup.
 * @retval -ENOMEM If no more timeouts can be installed.
 * @retval -ENOTSUP If any of the set flags is not supported.
 * @retval -EINVAL If any of the window timeout value is out of possible range.
 *		   This value is also returned if watchdog supports only one
 *		   timeout value for all timeouts and the supplied timeout
 *		   window differs from windows for alarms installed so far.
* @usage: Watchdog.install_timeout(min, max)
 * @demo: Watchdog.install_timeout(0, 1000)
 */
static evm_val_t evm_module_watchdog_install_timeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_WatchDogDevice_intall_timeout, 2, v);
}
/**
 * @brief Feed specified watchdog timeout.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param channel_id Index of the fed channel.
 *
 * @retval 0 If successful.
 * @retval -EINVAL If there is no installed timeout for supplied channel.
 * @usage: Watchdog.feed(channel_id)
 * @demo: Watchdog.feed(0)
 */
static evm_val_t evm_module_watchdog_feed(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_WatchDogDevice_feed, 1, v);
}

/**
 * @brief Disable watchdog instance.
 *
 * This function disables the watchdog instance and automatically uninstalls all
 * timeouts. To set up a new watchdog, install timeouts and call wdt_setup()
 * again. Not all watchdogs can be restarted after they are disabled.

 *
 * @retval 0 If successful.
 * @retval -EFAULT If watchdog instance is not enabled.
 * @retval -EPERM If watchdog can not be disabled directly by application code.
 * @usage: Watchdog.disable()
 * @demo: Watchdog.disable()
 */
static evm_val_t evm_module_watch_disable(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, "disable", 0, v);
}

evm_val_t evm_class_watchdog(evm_t * e){
	evm_builtin_t class_watchdog[] = {
        {"install_timeout", evm_mk_native( (intptr_t)evm_module_watchdog_install_timeout )},
		{"setup", evm_mk_native( (intptr_t)evm_module_watchdog_setup )},
		{"feed", evm_mk_native( (intptr_t)evm_module_watchdog_feed )},
        {"disable", evm_mk_native( (intptr_t)evm_module_watch_disable )},
		{NULL, NULL}
	};
	return  *evm_class_create(e, (evm_native_fn)evm_module_watchdog, class_watchdog, NULL);
}
#endif

#ifdef CONFIG_EVM_MODULE_TIMERS
#include "evm_module.h"
#include <rtthread.h>

static evm_t *timer_e;

static void *callback_handler(void *parameter)
{
    int id = *((int *)parameter);
    evm_val_t *callback = evm_module_registry_get(timer_e, id);
    if (callback == NULL)
        return;
    evm_val_t args = *callback;
    evm_module_next_tick(timer_e, 1, &args);
}

//setTimeout(callback, delay[, args..])
static evm_val_t evm_module_timers_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
        return EVM_VAL_UNDEFINED;

    int tick = evm_2_integer(v + 1);

    int id = evm_module_registry_add(e, v);
    if (id < 0)
        return EVM_VAL_UNDEFINED;

    rt_timer_t timer = rt_timer_create(evm_string_get(evm_object_get_hash(p)),
                                       callback_handler,
                                       (void *)&id,
                                       tick,
                                       RT_TIMER_FLAG_ONE_SHOT);

    rt_err_t rt_timer_start(timer);
    evm_object_set_ext_data(v, (intptr_t)timer);
    return evm_mk_number(id);
}

//clearTimeout(timeout)
static evm_val_t evm_module_timers_clearTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
    {
        return EVM_VAL_UNDEFINED;
    }
    evm_val_t *callback = evm_module_registry_get(e, evm_2_integer(v));
    if (callback == NULL)
        return EVM_VAL_UNDEFINED;
    rt_timer_t timer = evm_object_get_ext_data(callback);
    rt_timer_stop(timer);
    rt_timer_delete(timer);
    evm_module_registry_remove(e, evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}

//setInterval(callback, delay[, args..])
static evm_val_t evm_module_timers_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
        return EVM_VAL_UNDEFINED;

    int tick = evm_2_integer(v + 1);

    int id = evm_module_registry_add(e, v);
    if (id < 0)
        return EVM_VAL_UNDEFINED;

    rt_timer_t timer = rt_timer_create(evm_string_get(evm_object_get_hash(p)),
                                       callback_handler,
                                       (void *)&id,
                                       tick,
                                       RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    rt_err_t rt_timer_start(timer);
    evm_object_set_ext_data(v, (intptr_t)timer);
    return evm_mk_number(id);
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_timers_clearTimeout(e, p, argc, v);
}

evm_err_t evm_module_timers(evm_t *e) {
    timer_e = e;
    evm_builtin_t builtin[] = {
		{"setTimeout", evm_mk_native((intptr_t)evm_module_timers_setTimeout)},
		{"clearTimeout", evm_mk_native((intptr_t)evm_module_timers_clearTimeout)},
        {"setInterval", evm_mk_native((intptr_t)evm_module_timers_setInterval)},
		{"clearInterval", evm_mk_native((intptr_t)evm_module_timers_clearInterval)},
		{NULL, NULL}
	};
	return evm_native_add(e, builtin);
}
#endif

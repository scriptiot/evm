#ifdef CONFIG_EVM_MODULE_TIMERS
#include "evm_module.h"
#include <hal_hwtimer.h>

static evm_t *timer_e;

typedef struct _bl_timer_t
{
    hw_timer_t *handle;
    int id;
} _bl_timer_t;

static void callback_handler(void)
{
    evm_val_t *callback = evm_module_registry_get(timer_e, 0); // id
    if (callback == NULL)
        return;
    evm_val_t args = *callback;
    evm_module_next_tick(timer_e, 1, &args);
}

//setTimeout(callback, delay[, args..])
static evm_val_t evm_module_timers_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    int id = evm_module_registry_add(e, v);
    if (id < 0)
        return EVM_VAL_UNDEFINED;

    _bl_timer_t *timer = evm_malloc(sizeof(_bl_timer_t));

    timer->handle = hal_hwtimer_create(evm_2_integer(v + 1), callback_handler, 0);
    timer->id = id;

    if (timer->handle == NULL)
        return EVM_VAL_UNDEFINED;

    evm_object_set_ext_data(v, (intptr_t)timer);
    return EVM_VAL_UNDEFINED;
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

    _bl_timer_t *timer = (_bl_timer_t *)evm_object_get_ext_data(callback);
    if (timer->id < 0)
        return EVM_VAL_UNDEFINED;

    int ret = hal_hwtimer_delete(timer->handle);

    evm_module_registry_remove(e, evm_2_integer(v));
    if (ret == 0)
    {
        return EVM_VAL_TRUE;
    }
    else
    {
        return EVM_VAL_FALSE;
    }
}

//setInterval(callback, delay[, args..])
static evm_val_t evm_module_timers_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    int id = evm_module_registry_add(e, v);
    if (id < 0)
        return EVM_VAL_UNDEFINED;

    _bl_timer_t *timer = evm_malloc(sizeof(_bl_timer_t));
    timer->handle = hal_hwtimer_create(evm_2_integer(v + 1), callback_handler, 1);
    timer->id = id;

    if (timer->handle == NULL)
        return EVM_VAL_UNDEFINED;

    evm_object_set_ext_data(v, (intptr_t)timer);
    return EVM_VAL_UNDEFINED;
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *callback = evm_module_registry_get(e, evm_2_integer(v));
    if (callback == NULL)
        return EVM_VAL_UNDEFINED;

    _bl_timer_t *timer = (_bl_timer_t *)evm_object_get_ext_data(callback);
    if (timer->id < 0)
        return EVM_VAL_UNDEFINED;

    int ret = hal_hwtimer_delete(timer->handle);

    evm_module_registry_remove(e, evm_2_integer(v));
    if (ret == 0)
    {
        return EVM_VAL_TRUE;
    }
    else
    {
        return EVM_VAL_FALSE;
    }
}

evm_err_t evm_module_timers(evm_t *e)
{
    timer_e = e;
    evm_builtin_t builtin[] = {
        {"setTimeout", evm_mk_native((intptr_t)evm_module_timers_setTimeout)},
        {"clearTimeout", evm_mk_native((intptr_t)evm_module_timers_clearTimeout)},
        {"setInterval", evm_mk_native((intptr_t)evm_module_timers_setInterval)},
        {"clearInterval", evm_mk_native((intptr_t)evm_module_timers_clearInterval)},
        {NULL, NULL}};
    return evm_native_add(e, builtin);
}
#endif

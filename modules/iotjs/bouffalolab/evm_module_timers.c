#ifdef CONFIG_EVM_MODULE_TIMERS
#include "evm_module.h"
#include <FreeRTOS.h>
#include <timers.h>

static evm_t *timer_e;

static evm_val_t *timer_list;

typedef struct _evm_timer_t
{
    int period;
    int count;
} _evm_timer_t;

static void callback_handler()
{
    uint32_t index = 0;
    for (; index < evm_list_len(timer_list); index++)
    {
        evm_val_t *element = evm_list_get(timer_e, timer_list, index);
        if (evm_is_script(element))
        {
            _evm_timer_t *timer = (_evm_timer_t *)evm_object_get_ext_data(element);
            if (timer->count == 0)
            {
                evm_module_next_tick(timer_e, 1, element);
                if (timer->period < 0)
                    evm_list_set(timer_e, timer_list, index, EVM_VAL_UNDEFINED);
                else
                    timer->count = timer->period;
            }
            else
            {
                timer->count--;
            }
        }
    }
}

//setTimeout(callback, delay[, args..])
static evm_val_t evm_module_timers_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    printf("*********setTimeout*********\n");
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    uint32_t length = evm_list_len(timer_list);
    uint32_t index = 0;
    for (; index < length; index++)
    {
        evm_val_t *val = evm_list_get(e, timer_list, index);
        if (evm_is_undefined(val))
        {
            evm_list_set(e, timer_list, index, *v);
            _evm_timer_t *timer = (_evm_timer_t *)evm_malloc(sizeof(_evm_timer_t));
            timer->period = -1;
            timer->count = evm_2_integer(v + 1);
            evm_object_set_ext_data(v, (intptr_t)timer);
            return evm_mk_number(index);
        }
    }

    return EVM_VAL_UNDEFINED;
}

//clearTimeout(timeout)
static evm_val_t evm_module_timers_clearTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_list_set(e, timer_list, evm_2_integer(v), EVM_VAL_UNDEFINED);

    return EVM_VAL_UNDEFINED;
}

//setInterval(callback, delay[, args..])
static evm_val_t evm_module_timers_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_script(v) || !evm_is_integer(v + 1))
    {
        return EVM_VAL_UNDEFINED;
    }

    uint32_t length = evm_list_len(timer_list);
    uint32_t index = 0;
    for (; index < length; index++)
    {
        evm_val_t *val = evm_list_get(e, timer_list, index);
        if (evm_is_undefined(val))
        {
            evm_list_set(e, timer_list, index, *v);
            _evm_timer_t *timer = (_evm_timer_t *)evm_malloc(sizeof(_evm_timer_t));
            timer->period = evm_2_integer(v + 1);
            timer->count = timer->period;
            evm_object_set_ext_data(v, (intptr_t)timer);
            return evm_mk_number(index);
        }
    }
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_timers_clearTimeout(e, p, argc, v);
}

evm_err_t evm_module_timers(evm_t *e)
{
    timer_e = e;
    timer_list = evm_list_create(e, GC_LIST, 10);

    for (int i = 0; i < 10; i++)
    {
        evm_list_set(e, timer_list, i, EVM_VAL_UNDEFINED);
    }

    evm_builtin_t builtin[] = {
        {"setTimeout", evm_mk_native((intptr_t)evm_module_timers_setTimeout)},
        {"clearTimeout", evm_mk_native((intptr_t)evm_module_timers_clearTimeout)},
        {"setInterval", evm_mk_native((intptr_t)evm_module_timers_setInterval)},
        {"clearInterval", evm_mk_native((intptr_t)evm_module_timers_clearInterval)},
        {NULL, NULL}};

    TimerHandle_t handle = xTimerCreate(
        NULL,
        pdMS_TO_TICKS(1),
        pdTRUE,
        0,
        callback_handler);
    xTimerStart(handle, portMAX_DELAY);

    return evm_native_add(e, builtin);
}
#endif

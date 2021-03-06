#ifdef CONFIG_EVM_MODULE_TIMERS
#include "evm_module.h"
#include <time.h>
#include <signal.h>
#include <errno.h>

static evm_t *timer_e;

static void callback_handler(union sigval v)
{
    evm_val_t *callback = evm_module_registry_get(timer_e, v.sival_int);
    if (callback == NULL)
        return;
    evm_val_t args = *callback;
    evm_module_next_tick(timer_e, 1, &args);
}

static timer_t timer(int id, int delay, int once)
{
    timer_t timerid;
    int ret;

    struct sigevent sev;

    // handle in thread when timeout
    memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_value.sival_ptr = &timerid;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = callback_handler;
    sev.sigev_value.sival_int = id;

    struct itimerspec its; // duration settings

    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    if (ret == -1)
    {
        return NULL;
    }

    // set timeout, only once
    // it_value the first timeout duration
    // it_interval the next timeout duration
    if (delay >= 1000)
    {
        its.it_value.tv_sec = delay / 1000;
        its.it_value.tv_nsec = (delay % 1000) * 1000000;
    }
    else
    {
        its.it_value.tv_sec = 0;
        its.it_value.tv_nsec = delay * 1000000;
    }
    if( !once ) {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }

    ret = timer_settime(timerid, TIMER_ABSTIME, &its, NULL);
    if (ret == -1)
    {
        return NULL;
    }

    return timerid;
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
    timer_t timerid = timer(id, evm_2_integer(v + 1), 1);
    if (timerid == NULL)
        return EVM_VAL_UNDEFINED;
    evm_object_set_ext_data(v, (intptr_t)timerid);
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
    timer_t timerid = evm_object_get_ext_data(callback);
    if (timerid == NULL)
        return EVM_VAL_UNDEFINED;
    timer_delete(timerid);
    evm_module_registry_remove(e, evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
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
    timer_t timerid = timer(id, evm_2_integer(v + 1), 0);
    if (timerid == NULL)
        return EVM_VAL_UNDEFINED;
    evm_object_set_ext_data(v, (intptr_t)timerid);
    return evm_mk_number(id);
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
    {
        return EVM_VAL_UNDEFINED;
    }
    evm_module_timers_clearTimeout(e, p, argc, v);
    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_timers(evm_t *e) {
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

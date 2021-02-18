#ifdef CONFIG_EVM_MODULE_TIMERS
#include "evm_module.h"
#include <time.h>
#include <signal.h>
#include <errno.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGALRM

static evm_t *timer_e;

static void callback_handler(union sigval v)
{
    evm_val_t *callback = evm_module_registry_get(timer_e, v.sival_int);
    evm_val_t args = *callback;
    evm_module_next_tick(timer_e, 1, &args);
}

static int timer(int id, int delay, int once)
{
    timer_t timerid;
    int ret;

    struct sigevent sev;

    // handle in thread when timeout
    memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_value.sival_ptr = &timer;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = callback_handler;
    sev.sigev_value.sival_int = id;

    struct itimerspec its; // duration settings

    ret = timer_create(CLOCKID, &sev, &timerid);
    if (ret == -1)
    {
        evm_print("Failed to create timer: %s\n", strerror(errno));
        return -1;
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
        its.it_value.tv_nsec = delay * 1000000;
    }
    if( !once ) {
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }

    ret = timer_settime(timerid, TIMER_ABSTIME, &its, NULL);
    if (ret == -1)
    {
        evm_print("Failed to set timeout: %s\n", strerror(errno));
        return -1;
    }

    return timerid;
}

//setTimeout(callback, delay[, args..])
static evm_val_t evm_module_timers_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int id = evm_module_registry_add(e, v);
    timer_t timerid = timer(id, evm_2_integer(v + 1), 1);
    evm_object_set_ext_data(v, (intptr_t)timerid);
    return EVM_VAL_UNDEFINED;
}

//clearTimeout(timeout)
static evm_val_t evm_module_timers_clearTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t *callback = evm_module_registry_get(e, evm_2_integer(v));
    timer_t timerid = evm_object_get_ext_data(callback);
    timer_delete(timerid);
    evm_module_registry_remove(e, evm_2_integer(v));
	return EVM_VAL_UNDEFINED;
}

//setInterval(callback, delay[, args..])
static evm_val_t evm_module_timers_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int id = evm_module_registry_add(e, v);
    timer_t timerid = timer(id, evm_2_integer(v + 1), 0);
    evm_object_set_ext_data(v, (intptr_t)timerid);
    return evm_mk_number(id);
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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

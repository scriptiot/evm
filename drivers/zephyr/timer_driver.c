#include "nevm.h"
#include "evm_module.h"

#ifdef CONFIG_EVM_TIMER
#include <zephyr.h>
static void timer_callback(struct k_timer *handle)
{
    evm_val_t obj = evm_mk_object(handle->user_data);
    evm_val_t val_id = nevm_object_function_invoke(nevm_runtime, &obj, EXPORT_TimerDevice_getId, 0, NULL);
    int id = evm_2_integer(&val_id);
    evm_val_t * fn = evm_module_get_callback(id);
    evm_run_callback(evm_runtime, fn, NULL, NULL, 0);
}
#endif

//timer_config(callback)
evm_val_t nevm_driver_timer_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_TIMER
    struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    k_timer_init(dev, timer_callback, NULL);
    int id = evm_module_add_callback(*v, EVM_VAL_UNDEFINED);
    return evm_mk_number(id);
#endif
    return NEVM_FALSE;
}
//timer_init(String name)
evm_val_t nevm_driver_timer_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_TIMER
    struct k_timer * dev = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
    if( !dev ) return NEVM_FALSE;
    nevm_object_set_ext_data(p, (intptr_t)dev);
    dev->user_data = evm_2_intptr(p);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_timer_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_FALSE;
}
//timer_start(period)
evm_val_t nevm_driver_timer_start(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_TIMER
    if(argc > 0 && evm_is_number(v) ){
        struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
        int period = evm_2_integer(v);
        k_timer_start(dev, period, period);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_timer_stop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_TIMER
    struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
    k_timer_stop(dev);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}


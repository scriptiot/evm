#include "nevm.h"

#ifdef EVM_DRIVER_TIMER
#include <zephyr.h>
static void timer_callback(struct k_timer *handle)
{

}
#endif

//timer_config(int period, Object callback)
evm_val_t nevm_driver_timer_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_TIMER
    struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    dev->interval = evm_2_integer(v)；
    dev->user_data = evm_2_intptr(v + 1);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}
//timer_init(String name)
evm_val_t nevm_driver_timer_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_TIMER
    struct k_timer * dev = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
    if( !dev ) return NEVM_FALSE;
    nevm_object_set_ext_data(p, (intptr_t)dev);
    k_timer_init(dev, timer_callback, NULL);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_timer_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_FALSE;
}

evm_val_t nevm_driver_timer_start(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_TIMER
    struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
    k_timer_start(dev, dev->interval, dev->interval);
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_timer_stop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_TIMER
    struct k_timer * dev = (struct k_timer*)nevm_object_get_ext_data(p);
    k_timer_stop(dev);
#endif
    return NEVM_FALSE;
}


#include "nevm.h"

#ifdef CONFIG_EVM_PWM
#include <pwm.h>
#include <zephyr.h>
#endif

//pwm_config(devname, pin, period, pulse)
evm_val_t nevm_driver_pwm_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
#ifdef CONFIG_EVM_PWM
    const char * name = evm_2_string(v);
    struct device *dev = device_get_binding(name);
    if( !dev ) return NEVM_FALSE;
    if( pwm_pin_set_nsec(dev, evm_2_integer(v + 1), evm_2_integer(v + 2), evm_2_integer(v + 3)) == 0 )
        return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_pwm_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
#ifdef CONFIG_EVM_PWM
    if( argc > 0){
        const char * name = evm_2_string(v);
        struct device *dev = device_get_binding(name);
        if( !dev ) return NEVM_FALSE;
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_pwm_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

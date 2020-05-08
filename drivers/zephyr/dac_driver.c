#include "nevm.h"

#ifdef EVM_DRIVER_DAC
#include <device.h>
#include <drivers/dac.h>
#endif

//dac_config(char channel)
evm_val_t nevm_driver_dac_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_DAC
    if( argc > 0 && evm_is_number(v) ){
        struct device * dev = (struct device *)nevm_object_get_ext_data(p);
        if( !dev ) return NEVM_FALSE;

        struct dac_channel_cfg cfg = {
            .channel_id = evm_2_integer(v),
            .resolution = 12,
        };
        dac_channel_setup(dev, &cfg);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

//dac_init(String dev)
evm_val_t nevm_driver_dac_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_DAC
    if( argc > 0 && evm_is_string(v) ){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        nevm_object_set_ext_data(p, (intptr_t)dev);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_dac_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_TRUE;
}
//dac_set_value(char channel, int value)
evm_val_t nevm_driver_dac_set_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_DAC
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    dac_write_value(dev, evm_2_integer(v), evm_2_integer(v + 1));
#endif
    return EVM_VAL_UNDEFINED;
}

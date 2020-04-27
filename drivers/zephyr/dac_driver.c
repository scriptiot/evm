#include "nevm.h"
#include <device.h>

//dac_init(String dev, char channel, char resolution)
evm_val_t nevm_driver_dac_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
    if( argc > 2 ){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        dac_channel_cfg cfg;
        cfg.channel_id = evm_2_integer(v + 1);
        cfg.resolution = evm_2_integer(v + 2);
        dac_channel_setup(dev, &cfg);
        return NEVM_TRUE;
    }
    return NEVM_FALSE;
}

evm_val_t nevm_driver_dac_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return NEVM_TRUE;
}
//dac_set_value(String dev, char channel, int value)
evm_val_t nevm_driver_dac_set_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    dac_write_value(dev, evm_2_integer(v + 1), evm_2_integer(v + 2));
    return EVM_VAL_UNDEFINED;
}

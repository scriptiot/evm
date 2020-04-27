#include "nevm.h"
#include <device.h>

//adc_config(String dev, char gain, char reference, short acquisition_time, char differential):
evm_val_t nevm_driver_adc_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
    if( argc > 4 ){

    }
    return EVM_VAL_UNDEFINED;
}

//adc_init(String dev, char channel)
evm_val_t nevm_driver_adc_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
    if( argc > 0 ){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        adc_channel_cfg cfg;
        cfg.gain = ADC_GAIN_1_6;
        cfg.reference = ADC_REF_VDD_1;
        cfg.acquisition_time = ADC_ACQ_TIME_DEFAULT;
        cfg.channel_id = evm_2_integer(v + 1);
        adc_channel_setup(dev, &cfg);
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_adc_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

//adc_get_value(String dev)
evm_val_t nevm_driver_adc_get_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

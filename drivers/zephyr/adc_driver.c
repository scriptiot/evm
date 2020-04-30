#include "nevm.h"

#ifdef EVM_DRIVER_ADC

#define ADC_BUFFER_SIZE 2

#include <device.h>
#include <drivers/adc.h>
#endif

//adc_config()
evm_val_t nevm_driver_adc_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_ADC
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

//adc_init(String dev)
evm_val_t nevm_driver_adc_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_ADC
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    nevm_object_set_ext_data(p, (intptr_t)dev);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_adc_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

//adc_get_value(char channel)
evm_val_t nevm_driver_adc_get_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef EVM_DRIVER_ADC
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    char channel = evm_2_integer(v + 1);
    uint8_t seq_buffer[ADC_BUFFER_SIZE];
    struct adc_seq_entry entry = {
        .sampling_delay = 30,
        .channel_id = channel,
        .buffer = seq_buffer,
        .buffer_length = ADC_BUFFER_SIZE,
    };

    struct adc_seq_table entry_table = {
        .entries = &entry,
        .num_entries = 1,
    };

    if (adc_read(dev, &entry_table) != 0) {
        return NEVM_FALSE;
    }

    return evm_mk_number( seq_buffer[0] | seq_buffer[1] << 8 );
#endif
    return EVM_VAL_UNDEFINED;
}

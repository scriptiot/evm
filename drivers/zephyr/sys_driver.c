#include "nevm.h"

#include <zephyr.h>
#include <device.h>

//system_delay_ms(int millisecond)
evm_val_t nevm_driver_system_delay_ms(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    k_msleep(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}
//system_delay_us(int mircosecond)
evm_val_t nevm_driver_system_delay_us(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    k_usleep(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}
//system_is_device_valid(String name);
evm_val_t nevm_driver_system_is_device_valid(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);EVM_UNUSED(v);
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    return NEVM_TRUE;
}

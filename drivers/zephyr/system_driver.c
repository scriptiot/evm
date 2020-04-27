#include "nevm.h"

//system_set_irq_handler(String name, short id, function callback)
evm_val_t nevm_driver_system_set_irq_handler(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}
//system_is_device_valid(String name, int id);
evm_val_t nevm_driver_system_is_device_valid(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

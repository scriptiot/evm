#include "nevm.h"

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "invalid argument length");return evm_mk_undefined(); }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "invalid argument type");return evm_mk_undefined(); }

//config(int line, int trigger)
evm_val_t nevm_driver_exti_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
    if( argc > 0 && evm_is_number(v) && evm_is_number(v + 1)){

    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_exti_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_exti_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

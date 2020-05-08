#ifdef EVM_DRIVER_I2C
#include "evm_module.h"

//I2C(bus, ...)
static evm_val_t evm_module_i2c(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_i2c(evm_t * e){
	evm_builtin_t class_i2c[] = {
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_class_i2c, class_i2c, NULL);
}
#endif

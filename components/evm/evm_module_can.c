
#ifdef EVM_DRIVER_CAN
#include "evm_module.h"

//CAN(name)
static evm_val_t evm_module_can(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_can(evm_t * e){
	evm_builtin_t class_can[] = {
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_can, class_can, NULL);
}
#endif

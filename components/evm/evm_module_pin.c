#include "evm_module.h"

//Pin(port, number, speed, mode)
static evm_val_t evm_module_pin(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(e, p, v, "pinCreate");
	return EVM_VAL_UNDEFINED;
}
//Pin.value(value)
static evm_val_t evm_module_pin_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)evm_object_get_ext_data(p));
	if( argc == 0 ){
		return nevm_object_function_invoke(nevm_runtime, &dev, "read", 0, NULL);
	} else if( argc == 1 ) {
		nevm_object_function_invoke(nevm_runtime, &dev, "write", 1, v);
	}
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_pin(evm_t * e){
	evm_builtin_t cls[] = {
		{"value", evm_mk_native( (intptr_t)evm_module_pin_value )},		
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_pin, cls, NULL);
}

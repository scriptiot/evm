#include "evm_module.h" 

//DAC(name)
static evm_val_t evm_module_dac(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(e, p, v, "dacCreate");
	return EVM_VAL_UNDEFINED;
}

//DAC.write(value)
static evm_val_t evm_module_dac_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 0 ){
		evm_val_t dev = evm_mk_object((void*)evm_object_get_ext_data(p));
		nevm_object_function_invoke(nevm_runtime, &dev, "write", 1, v);
	}
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_dac(evm_t * e){
	evm_builtin_t class_dac[] = {
		{"write", evm_mk_native( (intptr_t)evm_module_dac_write )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_dac, class_dac, NULL);
}

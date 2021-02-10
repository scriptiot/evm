#include "evm_module.h"

evm_val_t *evm_module_adc_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//adc.open(configuration, callback)
static evm_val_t evm_module_adc_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//adc.openSync(configuration)
static evm_val_t evm_module_adc_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//adcpin.read(callback)
static evm_val_t evm_module_adc_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//adcpin.readSync()
static evm_val_t evm_module_adc_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//adcpin.close([callback])
static evm_val_t evm_module_adc_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//adcpin.closeSync()
static evm_val_t evm_module_adc_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_adc_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 4, 0);
	if( obj ) {
		evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_adc_class_read));
		evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_adc_class_readSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_adc_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_adc_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_adc(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_adc_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_adc_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "adc", builtin);
	return e->err;
}
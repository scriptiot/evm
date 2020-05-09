#ifdef CONFIG_EVM_ADC
#include "evm_module.h"

/**
 * @brief ADC class constructor
 * 
 * @param name device name
 * 
 * @uasge new ADC('ADC_1') 
 */
static evm_val_t evm_module_adc(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(nevm_runtime, p, argc, v, "adcCreate");
	return EVM_VAL_UNDEFINED;
}
/**
 * @brief read adc value
 * 
 * @uasge ADC.read()
 */
static evm_val_t evm_module_adc_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, "read", 0, NULL);
}

evm_val_t evm_class_adc(evm_t *e)
{
	evm_builtin_t class_adc[] = {
		{"read", evm_mk_native((intptr_t)evm_module_adc_read)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_module_adc, class_adc, NULL);
}
#endif

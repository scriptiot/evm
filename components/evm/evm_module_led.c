#include "evm_module.h"


//LED(name)
static evm_val_t evm_module_led(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//intensity([value])
static evm_val_t evm_module_led_intensity(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//off()
static evm_val_t evm_module_led_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//on()
static evm_val_t evm_module_led_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//toggle()
static evm_val_t evm_module_led_toggle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{	
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_led(evm_t * e){
	evm_builtin_t class_led[] = {
		{"intensity", evm_mk_native( (intptr_t)evm_module_led_intensity )},
		{"off", evm_mk_native( (intptr_t)evm_module_led_off )},
		{"on", evm_mk_native( (intptr_t)evm_module_led_on )},
		{"toggle", evm_mk_native( (intptr_t)evm_module_led_toggle )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_led, class_led, NULL);
}

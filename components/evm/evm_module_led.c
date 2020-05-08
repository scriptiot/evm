#ifdef EVM_DRIVER_LED
#include "evm_module.h"
#include "evm_board.h"

//LED(id)
static evm_val_t evm_module_led(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	int id = evm_2_integer(v);
	if( id >= sizeof(ledgroup) ) {
		evm_set_err(e, ec_index, "Out of led index");
		return EVM_VAL_UNDEFINED;
	}
	evm_val_t params[3];
	params[0] = evm_mk_foreign_string(ledgroup[id].port);
	params[1] = evm_mk_number(ledgroup[id].pin);
	params[2] = evm_mk_number(ledgroup[id].mode);
	evm_module_construct(nevm_runtime, p, 3, &params, "pinCreate");
	return EVM_VAL_UNDEFINED;
}

//off()
static evm_val_t evm_module_led_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	evm_val_t val = evm_mk_number(0);
	nevm_object_function_invoke(nevm_runtime, &dev, "write", 1, &val);
	return EVM_VAL_UNDEFINED;
}

//on()
static evm_val_t evm_module_led_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	evm_val_t val = evm_mk_number(1);
	nevm_object_function_invoke(nevm_runtime, &dev, "write", 1, &val);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_led(evm_t * e){
	evm_builtin_t class_led[] = {
		{"off", evm_mk_native( (intptr_t)evm_module_led_off )},
		{"on", evm_mk_native( (intptr_t)evm_module_led_on )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_led, class_led, NULL);
}
#endif

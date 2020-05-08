#ifdef EVM_DRIVER_GPIO

#include "evm_module.h"
#include <drivers/gpio.h>

//Pin(name, pin, flags)
static evm_val_t evm_module_pin(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(nevm_runtime, p, argc, v, "pinCreate");
	return EVM_VAL_UNDEFINED;
}
//Pin.value(value)
static evm_val_t evm_module_pin_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
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
		{"IN", evm_mk_number(GPIO_INPUT)},	
		{"OUT", evm_mk_number(GPIO_OUTPUT)},
		{"DISCONNECTED", evm_mk_number(GPIO_DISCONNECTED)},
		{"INT_ENABLE", evm_mk_number(GPIO_INT_ENABLE)},
		{"INT_DISABLE", evm_mk_number(GPIO_INT_DISABLE)},
		{"INT_EDGE_RISING", evm_mk_number(GPIO_INT_EDGE_RISING)},
		{"INT_EDGE_FALLING", evm_mk_number(GPIO_INT_EDGE_FALLING)},
		{"INT_EDGE_BOTH", evm_mk_number(GPIO_INT_EDGE_BOTH)},
		{"PULL_UP", evm_mk_number(GPIO_PUD_PULL_UP)},
		{"PULL_DOWN", evm_mk_number(GPIO_PUD_PULL_DOWN)},	
		{"OPEN_SOURCE", evm_mk_number(GPIO_OPEN_SOURCE)},
		{"OPEN_DRAIN", evm_mk_number(GPIO_OPEN_DRAIN)},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_pin, cls, NULL);
}

#endif

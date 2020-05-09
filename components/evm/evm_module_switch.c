#ifdef EVM_DRIVER_SWITCH
#include "evm_module.h"
#include "evm_board.h"

//Switch(id)
static evm_val_t evm_module_switch(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	int id = evm_2_integer(v);
	if( id >= evm_board_get_pin_group_size(keygroup) ) {
		evm_set_err(e, ec_index, "Out of key index");
		return EVM_VAL_UNDEFINED;
	}
	evm_val_t params[3];
	params[0] = evm_mk_foreign_string(keygroup[id].port);
	params[1] = evm_mk_number(keygroup[id].pin);
	params[2] = evm_mk_number(keygroup[id].mode);
	evm_module_construct(nevm_runtime, p, 3, &params, "pinCreate");
	return EVM_VAL_UNDEFINED;
}
//Switch.value()
static evm_val_t evm_module_switch_value(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, "read", 0, NULL);
}

//Switch.callback(function)
static evm_val_t evm_module_switch_callback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	nevm_object_function_invoke(nevm_runtime, &dev, "setCallback", 1, v);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_switch(evm_t * e){
	evm_builtin_t class_switch[] = {
		{"value", evm_mk_native( (intptr_t)evm_module_switch_value )},
		{"callback", evm_mk_native( (intptr_t)evm_module_switch_callback )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_switch, class_switch, NULL);
}
#endif

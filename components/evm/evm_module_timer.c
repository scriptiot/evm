#ifdef CONFIG_EVM_TIMER
#include "evm_module.h"

//Timer(String name)
static evm_val_t evm_module_timer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_module_construct(nevm_runtime, p, argc, v, "timerCreate");
	return EVM_VAL_UNDEFINED;
}

//Timer.setInterval(callback, period)
static evm_val_t evm_module_timer_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, "start", 2, v);
}

//Timer.clearInterval(id)
static evm_val_t evm_module_timer_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_timer(evm_t * e){
	evm_builtin_t class_timer[] = {
		{"setInterval", evm_mk_native( (intptr_t)evm_module_timer_setInterval )},
		{"clearInterval", evm_mk_native( (intptr_t)evm_module_timer_clearInterval )},
		{"id", evm_mk_number(-1)},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_timer, class_timer, NULL);
}
#endif

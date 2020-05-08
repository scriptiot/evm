#ifdef EVM_DRIVER_TIMER
#include "evm_module.h"

//Timer(id, ...)
static evm_val_t evm_module_timer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.init(*, freq, prescaler, period)
static evm_val_t evm_module_timer_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.deinit()
static evm_val_t evm_module_timer_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.callback(fun)
static evm_val_t evm_module_timer_callback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.channel(channel, mode, ...)
static evm_val_t evm_module_timer_channel(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.counter([value])
static evm_val_t evm_module_timer_counter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.freq([value])
static evm_val_t evm_module_timer_freq(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.period([value])
static evm_val_t evm_module_timer_period(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.prescaler([value])
static evm_val_t evm_module_timer_prescaler(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//Timer.source_freq()
static evm_val_t evm_module_timer_source_freq(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}


evm_val_t evm_class_timer(evm_t * e){
	evm_builtin_t class_timer[] = {
		{"init", evm_mk_native( (intptr_t)evm_module_timer_init )},
		{"deinit", evm_mk_native( (intptr_t)evm_module_timer_deinit )},
		{"callback", evm_mk_native( (intptr_t)evm_module_timer_callback )},
		{"channel", evm_mk_native( (intptr_t)evm_module_timer_channel )},
		{"counter", evm_mk_native( (intptr_t)evm_module_timer_counter )},
		{"freq", evm_mk_native( (intptr_t)evm_module_timer_freq )},
		{"period", evm_mk_native( (intptr_t)evm_module_timer_period )},
		{"prescaler", evm_mk_native( (intptr_t)evm_module_timer_prescaler )},
		{"source_freq", evm_mk_native( (intptr_t)evm_module_timer_source_freq )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_timer, class_timer, NULL);
}
#endif

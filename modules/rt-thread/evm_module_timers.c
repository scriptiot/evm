#include "evm_module.h"

//setTimeout(callback, delay[, args..])
static evm_val_t evm_module_timers_setTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//clearTimeout(timeout)
static evm_val_t evm_module_timers_clearTimeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//setInterval(callback, delay[, args..])
static evm_val_t evm_module_timers_setInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//clearInterval(timeout)
static evm_val_t evm_module_timers_clearInterval(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_timers(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"setTimeout", evm_mk_native((intptr_t)evm_module_timers_setTimeout)},
		{"clearTimeout", evm_mk_native((intptr_t)evm_module_timers_clearTimeout)},
        {"setInterval", evm_mk_native((intptr_t)evm_module_timers_setInterval)},
		{"clearInterval", evm_mk_native((intptr_t)evm_module_timers_clearInterval)},
		{NULL, NULL}
	};
	return evm_native_add(e, builtin);
}
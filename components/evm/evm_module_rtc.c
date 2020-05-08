#ifdef EVM_DRIVER_RTC
#include "evm_module.h"

//RTC
static evm_val_t evm_module_rtc(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//RTC.datetime()
static evm_val_t evm_module_rtc_datetime(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_rtc(evm_t * e){
	evm_builtin_t class_rtc[] = {
		{"datetime", evm_mk_native( (intptr_t)evm_module_rtc_datetime )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_rtc, class_rtc, NULL);
}
#endif

#ifdef CONFIG_EVM_MODULE_PWM
#include "evm_module.h"

evm_val_t *evm_module_pwm_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//pwm.open(configuration, callback)
static evm_val_t evm_module_pwm_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwm.openSync(configuration)
static evm_val_t evm_module_pwm_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setPeriod(period[, callback])
static evm_val_t evm_module_pwm_class_setPeriod(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setPeriodSync(period)
static evm_val_t evm_module_pwm_class_setPeriodSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setFrequency(frequency[, callback])
static evm_val_t evm_module_pwm_class_setFrequency(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setFrequencySync(frequency)
static evm_val_t evm_module_pwm_class_setFrequencySync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setDutyCycle(dutyCycle[, callback])
static evm_val_t evm_module_pwm_class_setDutyCycle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setDutyCycleSync(dutyCycle)
static evm_val_t evm_module_pwm_class_setDutyCycleSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setEnable(enable[, callback])
static evm_val_t evm_module_pwm_class_setEnable(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.setEnableSync(enable)
static evm_val_t evm_module_pwm_class_setEnableSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.close([callback])
static evm_val_t evm_module_pwm_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//pwmpin.closeSync()
static evm_val_t evm_module_pwm_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_pwm_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 10, 0);
	if( obj ) {
		evm_prop_append(e, obj, "setPeriod", evm_mk_native((intptr_t)evm_module_pwm_class_setPeriod));
		evm_prop_append(e, obj, "setPeriodSync", evm_mk_native((intptr_t)evm_module_pwm_class_setPeriodSync));
        evm_prop_append(e, obj, "setFrequency", evm_mk_native((intptr_t)evm_module_pwm_class_setFrequency));
		evm_prop_append(e, obj, "setFrequencySync", evm_mk_native((intptr_t)evm_module_pwm_class_setFrequencySync));
		evm_prop_append(e, obj, "setDutyCycle", evm_mk_native((intptr_t)evm_module_pwm_class_setDutyCycle));
		evm_prop_append(e, obj, "setDutyCycleSync", evm_mk_native((intptr_t)evm_module_pwm_class_setDutyCycleSync));
        evm_prop_append(e, obj, "setEnable", evm_mk_native((intptr_t)evm_module_pwm_class_setEnable));
		evm_prop_append(e, obj, "setEnableSync", evm_mk_native((intptr_t)evm_module_pwm_class_setEnableSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_pwm_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_pwm_class_close));
	}
	return obj;
}

evm_err_t evm_module_pwm(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_pwm_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_pwm_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "pwm", builtin);
	return e->err;
}
#endif

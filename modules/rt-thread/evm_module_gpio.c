#ifdef CONFIG_EVM_MODULE_GPIO
#include "evm_module.h"

evm_val_t *evm_module_gpio_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//gpio.open(configuration, callback)
static evm_val_t evm_module_gpio_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpio.openSync(configuration)
static evm_val_t evm_module_gpio_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.setDirectionSync(direction)
static evm_val_t evm_module_gpio_class_setDirectionSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.write(value[, callback])
static evm_val_t evm_module_gpio_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.writeSync(value)
static evm_val_t evm_module_gpio_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.read([callback])
static evm_val_t evm_module_gpio_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.readSync()
static evm_val_t evm_module_gpio_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.close([callback])
static evm_val_t evm_module_gpio_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//gpiopin.closeSync()
static evm_val_t evm_module_gpio_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_gpio_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 7, 0);
	if( obj ) {
		evm_prop_append(e, obj, "setDirectionSync", evm_mk_native((intptr_t)evm_module_gpio_class_setDirectionSync));
		evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_gpio_class_write));
		evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_gpio_class_writeSync));
		evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_gpio_class_read));
        evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_gpio_class_readSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_gpio_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_gpio_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_gpio(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_gpio_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_gpio_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "gpio", builtin);
	return e->err;
}
#endif

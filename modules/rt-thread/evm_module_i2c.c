#ifdef CONFIG_EVM_MODULE_I2C
#include "evm_module.h"

evm_val_t *evm_module_i2c_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//i2c.open(configuration, callback)
static evm_val_t evm_module_i2c_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2c.openSync(configuration)
static evm_val_t evm_module_i2c_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.read(length[, callback])
static evm_val_t evm_module_i2c_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.readSync(length)
static evm_val_t evm_module_i2c_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.write(bytes[, callback])
static evm_val_t evm_module_i2c_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.writeSync(bytes)
static evm_val_t evm_module_i2c_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.close([callback])
static evm_val_t evm_module_i2c_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//i2cbus.closeSync()
static evm_val_t evm_module_i2c_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_i2c_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 6, 0);
	if( obj ) {
		evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_i2c_class_read));
		evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_i2c_class_readSync));
        evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_i2c_class_write));
		evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_i2c_class_writeSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_i2c_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_i2c_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_i2c(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_i2c_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_i2c_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "i2c", builtin);
	return e->err;
}
#endif

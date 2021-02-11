#ifdef CONFIG_EVM_MODULE_SPI
#include "evm_module.h"

evm_val_t *evm_module_spi_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

//spi.open(configuration, callback)
static evm_val_t evm_module_spi_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//spi.openSync(configuration)
static evm_val_t evm_module_spi_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//spibus.transfer(txBuffer, [, callback])
static evm_val_t evm_module_spi_class_transfer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//spibus.transferSync(txBuffer)
static evm_val_t evm_module_spi_class_transferSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//spipin.close([callback])
static evm_val_t evm_module_spi_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//spipin.closeSync()
static evm_val_t evm_module_spi_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_spi_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 4, 0);
	if( obj ) {
		evm_prop_append(e, obj, "transfer", evm_mk_native((intptr_t)evm_module_spi_class_transfer));
		evm_prop_append(e, obj, "transferSync", evm_mk_native((intptr_t)evm_module_spi_class_transferSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_spi_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_spi_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_spi(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_spi_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_spi_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "spi", builtin);
	return e->err;
}
#endif

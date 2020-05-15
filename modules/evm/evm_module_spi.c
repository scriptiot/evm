#ifdef CONFIG_EVM_SPI
#include "evm_module.h"

//SPI(name)
static evm_val_t evm_module_spi(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//SPI.deinit()
static evm_val_t evm_module_spi_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_spi(evm_t * e){
	evm_builtin_t class_spi[] = {
		{"deinit", evm_mk_native( (intptr_t)evm_module_spi_deinit )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_spi, class_spi, NULL);
}
#endif

#include "evm_module.h"

//Flash()
static evm_val_t evm_module_flash(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{	
	return EVM_VAL_UNDEFINED;
}
//Flash.readblocks(address, buf, size)
static evm_val_t evm_module_flash_readblocks(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//Flash.writeblocks(address, buf, size)
static evm_val_t evm_module_flash_writeblocks(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_flash(evm_t * e){
	evm_builtin_t class_flash[] = {
		{"readblocks", evm_mk_native( (intptr_t)evm_module_flash_readblocks )},
		{"writeblocks", evm_mk_native( (intptr_t)evm_module_flash_writeblocks )},
		{NULL, NULL}
	};
	return  *evm_class_create(e, (evm_native_fn)evm_module_flash, class_flash, NULL);
}

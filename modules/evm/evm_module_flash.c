#ifdef CONFIG_EVM_FLASH
#include "evm_module.h"

/**
 *  @brief  create flash device
 *
 *  @param  name            : flash device name define in dts
 *
 *  @return  0 on success, negative errno code on fail.

	@usage: new Flash(name)
	@demo: new Flash('FLASH_CTRL')
 */
static evm_val_t evm_module_flash(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{	
	evm_module_construct(nevm_runtime, p, argc, v, EXPORT_main_flashCreate, EXPORT_FlashDevice_open);
	return EVM_VAL_UNDEFINED;
}

/**
 *  @brief  Read data from flash
 *  
 *  Most of flash drivers support unaligned flash access, but some have
 *  restrictions on the read offset or/and the read size. Please refer to
 *  the driver implementation to get details on the read alignment requirement.
 *
 *  @param  offset          : Offset (byte aligned) to read
 *  @param  data            : Buffer to store read data
 *  @param  len             : Number of bytes to read.
 *
 *  @return  0 on success, negative errno code on fail.

	@usage: Flash.read(offset, data, len)
 */
static evm_val_t evm_module_flash_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_FlashDevice_read, 3, v);
}

/**
 *  @brief  Write buffer into flash memory.
 *
 *  Prior to the invocation of this API, the flash_write_protection_set needs
 *  to be called first to disable the write protection.
 *  
 *  @param  dev             : flash device
 *  @param  offset          : starting offset for the write
 *  @param  data            : data to write
 *  @param  len             : Number of bytes to write
 *
 *  @return  0 on success, negative errno code on fail.
 *  @usage: Flash.write(offset, data, len)
 */
static evm_val_t evm_module_flash_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_FlashDevice_write, 3, v);
}


/**
 *  @brief  Erase part or all of a flash memory
 *
 *  Acceptable values of erase size and offset are subject to
 *  hardware-specific multiples of page size and offset. Please check
 *  the API implemented by the underlying sub driver, for example by
 *  using flash_get_page_info_by_offs() if that is supported by your
 *  flash driver.
 *
 *  Prior to the invocation of this API, the flash_write_protection_set needs
 *  to be called first to disable the write protection.
 *
 *  @param  offset          : erase area starting offset
 *  @param  size            : size of area to be erased
 *
 *  @return  0 on success, negative errno code on fail.
 *
 *  @see flash_get_page_info_by_offs()
 *  @see flash_get_page_info_by_idx()
 *  @usage: Flash.erase(offset, size)
 */
static evm_val_t evm_module_flash_erase(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_FlashDevice_erase, 2, v);
}

/**
 *  @brief  Get the minimum write block size supported by the driver
 *
 *  The write block size supported by the driver might differ from the write
 *  block size of memory used because the driver might implements write-modify
 *  algorithm.
 *
 *  @param  dev flash device
 *
 *  @return  write block size in bytes.
 *  @usage: Flash.write_block_size()
 */
static evm_val_t evm_module_flash_write_block_size(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	evm_val_t dev = evm_mk_object((void*)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_FlashDevice_write_block_size, 0, NULL);
}

evm_val_t evm_class_flash(evm_t * e){
	evm_builtin_t class_flash[] = {
		{"read", evm_mk_native( (intptr_t)evm_module_flash_read )},
		{"write", evm_mk_native( (intptr_t)evm_module_flash_write )},
		{"erase", evm_mk_native( (intptr_t)evm_module_flash_erase )},
		{"write_block_size", evm_mk_native( (intptr_t)evm_module_flash_write_block_size )},
		{NULL, NULL}
	};
	return  *evm_class_create(e, (evm_native_fn)evm_module_flash, class_flash, NULL);
}
#endif

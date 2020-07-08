/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/


#include "evm_module.h"

#include <device.h>
#include <drivers/flash.h>
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
	if( argc > 0 && evm_is_string(v) ){
		struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) {
			evm_set_err(e, ec_type, "Can't find Flash device");
			return EVM_VAL_UNDEFINED;
		}
        evm_object_set_ext_data(p, (intptr_t)dev);
    }
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
	if( argc > 2 && evm_is_number(v) && evm_is_buffer(v + 1) && evm_is_number(v + 2)){
        struct device * dev = (struct device *)evm_object_get_ext_data(p);
        if( !dev ) return EVM_VAL_FALSE;
        int data_len = evm_buffer_len(v + 1);
        int len = evm_2_integer(v + 2);
        len = len < data_len ? len : data_len;
        int err = flash_read(dev, evm_2_integer(v), evm_buffer_addr(v + 1), len);
        if (err){
            return EVM_VAL_FALSE;
        }
        return EVM_VAL_TRUE;
    }
	return EVM_VAL_UNDEFINED;
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
	if( argc > 2 && evm_is_number(v) && evm_is_buffer(v + 1) && evm_is_number(v + 2)){
        struct device * dev = (struct device *)evm_object_get_ext_data(p);
        if( !dev ) return EVM_VAL_FALSE;
        int err = flash_write_protection_set(dev, false);
        if (err){
            return EVM_VAL_FALSE;
        }
        int data_len = evm_buffer_len(v + 1);
        int len = evm_2_integer(v + 2);
        len = len < data_len ? len : data_len;
        
        err = flash_write(dev, evm_2_integer(v), evm_buffer_addr(v + 1) , len);
        if (err){
            return EVM_VAL_FALSE;
        }
        return EVM_VAL_TRUE;
    }
	return EVM_VAL_UNDEFINED;
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
	if( argc > 1 && evm_is_number(v) && evm_is_number(v + 1) ){
        struct device * dev = (struct device *)evm_object_get_ext_data(p);
        if( !dev ) return EVM_VAL_FALSE;
        int err = flash_write_protection_set(dev, false);
        if (err){
            return EVM_VAL_FALSE;
        }
        err = flash_erase(dev, evm_2_integer(v), evm_2_integer(v+1));
        if (err){
            return EVM_VAL_FALSE;
        }
        return EVM_VAL_TRUE;
    }
	return EVM_VAL_UNDEFINED;
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
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
    if( !dev ) return EVM_VAL_FALSE;
    return evm_mk_number(flash_get_write_block_size(dev));
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


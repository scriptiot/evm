#include "nevm.h"

#ifdef EVM_DRIVER_FLASH
#include <device.h>
#include <drivers/flash.h>
#endif

//flash_config(char pin, char flags)
evm_val_t nevm_driver_flash_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return NEVM_TRUE;
}

//flash_init(String name)
evm_val_t nevm_driver_flash_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_FLASH
    if( argc > 0 && evm_is_string(v) ){
		struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        nevm_object_set_ext_data(p, (intptr_t)dev);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

//flash_deinit(char pin, char flags)
evm_val_t nevm_driver_flash_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return EVM_VAL_UNDEFINED;
}

//flash_read(offset, data, len)
evm_val_t nevm_driver_flash_read(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_FLASH
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    int err = flash_read(dev, evm_2_integer(v), evm_2_intptr(v) ,evm_2_integer(v+2))
    if (err){
        return NEVM_FALSE;
    }
#endif
    return NEVM_TRUE;
}
//flash_write(offset, data, len)
evm_val_t nevm_driver_flash_write(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_FLASH
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    int err = flash_write(dev, evm_2_integer(v), evm_2_intptr(v) ,evm_2_integer(v+2));
    if (err){
        return NEVM_FALSE;
    }
#endif
    return NEVM_TRUE;
}
//flash_erase(offset, size)
evm_val_t nevm_driver_flash_erase(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_FLASH
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    int err = flash_erase(dev, evm_2_integer(v), evm_2_integer(v+1));
    if (err){
        return NEVM_FALSE;
    }
#endif
    return NEVM_TRUE;
}

//flash_write_block_size()
evm_val_t nevm_driver_flash_write_block_size(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_FLASH
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    return evm_mk_number(flash_get_write_block_size(dev));
#endif
    return EVM_VAL_UNDEFINED;
}
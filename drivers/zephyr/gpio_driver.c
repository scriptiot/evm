#include "nevm.h"

#ifdef EVM_DRIVER_GPIO
#include <device.h>
#include <drivers/gpio.h>
#endif

//gpio_init(String port, char pin, char flags)
evm_val_t nevm_driver_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_GPIO
    if( argc > 2 && evm_is_string(v) && evm_is_number(v + 1) ){
		struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        if( !gpio_pin_configure(dev, (gpio_pin_t)evm_2_integer(v + 1), (gpio_flags_t)evm_2_integer(v + 2)) )
		    return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}
//gpio_deinit(String port)
evm_val_t nevm_driver_gpio_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return EVM_VAL_UNDEFINED;
}
//gpio_write_pin(String port, char pin, char value)
evm_val_t nevm_driver_gpio_write_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_GPIO
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    gpio_pin_set(dev, (gpio_pin_t)evm_2_integer(v + 1), evm_2_integer(v + 2) );
#endif
    return NEVM_TRUE;
}
//gpio_read_pin(int id)
evm_val_t nevm_driver_gpio_read_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef EVM_DRIVER_GPIO
    struct device * dev = device_get_binding(evm_2_string(v));
    if( !dev ) return NEVM_FALSE;
    return evm_mk_number( gpio_pin_get(dev, (gpio_pin_t)evm_2_integer(v + 1) ) );
#endif
return EVM_VAL_UNDEFINED;
}

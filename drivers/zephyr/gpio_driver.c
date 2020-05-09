#include "nevm.h"
#include "evm_module.h"

#ifdef CONFIG_EVM_GPIO
#include <device.h>
#include <drivers/gpio.h>

typedef struct gpio_handle {
    struct gpio_callback callback;
    int id;  
} gpio_handle_t;

static void evm_driver_gpio_callback(struct device *gpio, struct gpio_callback *cb, u32_t pins)
{
    gpio_handle_t *handle = CONTAINER_OF(cb, gpio_handle_t, callback);
    evm_val_t * fn = evm_module_get_callback(handle->id);
    evm_run_callback(evm_runtime, fn, NULL, NULL, 0);
}
#endif


//gpio_config(char pin, char flags)
evm_val_t nevm_driver_gpio_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_GPIO
    if( argc > 1 ){
		struct device * dev = (struct device *)nevm_object_get_ext_data(p);
        if( !dev ) return NEVM_FALSE;
        if( !gpio_pin_configure(dev, (gpio_pin_t)evm_2_integer(v), (gpio_flags_t)evm_2_integer(v + 1)) )
		    return NEVM_TRUE;
    }
#endif
    return NEVM_TRUE;
}

//gpio_init(String name)
evm_val_t nevm_driver_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_GPIO
    if( argc > 0 && evm_is_string(v) ){
		struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        nevm_object_set_ext_data(p, (intptr_t)dev);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}
//gpio_deinit()
evm_val_t nevm_driver_gpio_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return EVM_VAL_UNDEFINED;
}
//gpio_write_pin(char pin, char value)
evm_val_t nevm_driver_gpio_write_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_GPIO
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    gpio_pin_set(dev, (gpio_pin_t)evm_2_integer(v), evm_2_integer(v + 1) );
#endif
    return NEVM_TRUE;
}
//gpio_read_pin(char pin)
evm_val_t nevm_driver_gpio_read_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_GPIO
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    return evm_mk_number( gpio_pin_get(dev, (gpio_pin_t)evm_2_integer(v) ) );
#endif
    return EVM_VAL_UNDEFINED;
}
//gpio_callback(Object callback, char pin)
evm_val_t nevm_driver_gpio_callback(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;
#ifdef CONFIG_EVM_GPIO
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    char pin = evm_2_integer(v + 1); 
    gpio_handle_t *handle = evm_malloc(sizeof(gpio_handle_t));
    if (!handle) {
        evm_set_err(e, ec_memory, "Out of memory");
        return NEVM_FALSE;
    }
    memset(handle, 0, sizeof(gpio_handle_t));
    gpio_init_callback(&handle->callback, evm_driver_gpio_callback, BIT(pin));

	gpio_add_callback(dev, &handle->callback);
    gpio_pin_enable_callback(dev, pin);

    handle->id = evm_module_add_callback(*v, EVM_VAL_UNDEFINED);
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}



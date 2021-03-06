#ifdef CONFIG_EVM_MODULE_GPIO
#include "evm_module.h"
#include "bl_gpio.h"
#include <bl602_gpio.h>

evm_val_t *evm_module_gpio_class_instantiate(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

typedef struct _gpio_dev_t
{
    int pin;
    int direction;
    int mode;
    int edge;
} _gpio_dev_t;

static evm_val_t _gpio_open_device(evm_t *e, evm_val_t *p, int argc, evm_val_t *v, int is_sync)
{
    evm_val_t *val = NULL;
    evm_val_t *ret_obj;
    evm_val_t *cb = NULL;
    evm_val_t args;
    _gpio_dev_t *dev;

    if (argc < 1)
        return EVM_VAL_UNDEFINED;

    if (argc > 1 && evm_is_script(v + 1) && !is_sync)
    {
        cb = v + 1;
    }

    dev = evm_malloc(sizeof(_gpio_dev_t));
    if (!dev)
    {
        args = evm_mk_foreign_string("Insufficient external memory");
        if (cb)
            evm_run_callback(e, cb, NULL, &args, 1);
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
    }

    val = evm_prop_get(e, v, "pin", 0);
    if (val == NULL || !evm_is_integer(val))
    {
        args = evm_mk_foreign_string("Configuration has no 'pin' member");
        if (cb)
            evm_run_callback(e, cb, NULL, &args, 1);
        evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'pin' member");
        return EVM_VAL_UNDEFINED;
    }
    dev->pin = evm_2_integer(val);

    val = evm_prop_get(e, v, "direction", 0);
    if (val == NULL || !evm_is_integer(val))
    {
        args = evm_mk_foreign_string("Configuration has no 'direction' member");
        if (cb)
            evm_run_callback(e, cb, NULL, &args, 1);
        evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'direction' member");
        return EVM_VAL_UNDEFINED;
    }
    dev->direction = evm_2_integer(val);

    val = evm_prop_get(e, v, "mode", 0);
    if (val == NULL || !evm_is_integer(val))
    {
        args = evm_mk_foreign_string("Configuration has no 'mode' member");
        if (cb)
            evm_run_callback(e, cb, NULL, &args, 1);
        evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'mode' member");
        return EVM_VAL_UNDEFINED;
    }
    dev->mode = evm_2_integer(val);

    if (dev->mode)
    {
        bl_gpio_enable_input(dev->pin, dev->direction ? 1 : 0, dev->direction ? 0 : 1);
    }
    else
    {
        bl_gpio_enable_output(dev->pin, dev->direction ? 1 : 0, dev->direction ? 0 : 1);
    }

    ret_obj = evm_module_gpio_class_instantiate(e, p, argc, v);
    if (ret_obj == NULL)
    {
        args = evm_mk_foreign_string("Failed to instantiate");
        if (cb)
            evm_run_callback(e, cb, NULL, &args, 1);
        evm_free(dev);
        return EVM_VAL_UNDEFINED;
    }

    evm_object_set_ext_data(ret_obj, (intptr_t)dev);
    return *ret_obj;
}

//gpio.open(configuration, callback)
static evm_val_t evm_module_gpio_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return _gpio_open_device(e, p, argc, v, 0);
}

//gpio.openSync(configuration)
static evm_val_t evm_module_gpio_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return _gpio_open_device(e, p, argc, v, 1);
}

//gpiopin.setDirectionSync(direction)
static evm_val_t evm_module_gpio_class_setDirectionSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _gpio_dev_t *dev = (_gpio_dev_t *)evm_object_get_ext_data(p);
    evm_val_t args[2];
    if (!dev)
        return EVM_VAL_UNDEFINED;

    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    dev->direction = evm_2_integer(v);
    return EVM_VAL_UNDEFINED;
}

//gpiopin.write(value[, callback])
static evm_val_t evm_module_gpio_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _gpio_dev_t *dev = (_gpio_dev_t *)evm_object_get_ext_data(p);

    if (!dev)
        return EVM_VAL_UNDEFINED;
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    bl_gpio_output_set(dev->pin, evm_2_integer(v) ? 1 : 0);

    evm_val_t args;
    if (argc > 1 && evm_is_script(v + 1))
    {
        args = evm_mk_null();
        evm_run_callback(e, v + 1, NULL, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//gpiopin.writeSync(value)
static evm_val_t evm_module_gpio_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _gpio_dev_t *dev = (_gpio_dev_t *)evm_object_get_ext_data(p);

    if (!dev)
        return EVM_VAL_UNDEFINED;
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    bl_gpio_output_set(dev->pin, evm_2_integer(v) ? 1 : 0);

    return EVM_VAL_UNDEFINED;
}

//gpiopin.read([callback])
static evm_val_t evm_module_gpio_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _gpio_dev_t *dev = (_gpio_dev_t *)evm_object_get_ext_data(p);
    if (!dev)
        return EVM_VAL_UNDEFINED;

    uint8_t status;
    int ret = bl_gpio_input_get(dev->pin, &status);
    if (ret != 0)
        return EVM_VAL_UNDEFINED;

    evm_val_t args[2];
    if (argc > 0 && evm_is_script(v))
    {
        args[0] = evm_mk_null();
        if (status)
        {
            args[1] = EVM_VAL_TRUE;
        }
        else
        {
            args[1] = EVM_VAL_FALSE;
        }
        evm_run_callback(e, v, NULL, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//gpiopin.readSync()
static evm_val_t evm_module_gpio_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _gpio_dev_t *dev = (_gpio_dev_t *)evm_object_get_ext_data(p);
    int status;
    if (!dev)
        return EVM_VAL_UNDEFINED;

    status = bl_gpio_input_get_value(dev->pin);
    if (status)
    {
        return EVM_VAL_TRUE;
    }
    return EVM_VAL_FALSE;
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
    evm_val_t *prop = NULL;
    if (obj)
    {
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

evm_err_t evm_module_gpio(evm_t *e)
{
    evm_val_t *dir_prop = evm_object_create(e, GC_OBJECT, 2, 0);
    if (dir_prop)
    {
        evm_prop_append(e, dir_prop, "IN", evm_mk_number(EVM_GPIO_DIRECTION_IN));
        evm_prop_append(e, dir_prop, "OUT", evm_mk_number(EVM_GPIO_DIRECTION_OUT));
    }
    else
    {
        return e->err;
    }

    evm_val_t *mode_prop = evm_object_create(e, GC_OBJECT, 6, 0);
    if (mode_prop)
    {
        evm_prop_append(e, mode_prop, "NONE", evm_mk_number(EVM_GPIO_MODE_NONE));
        evm_prop_append(e, mode_prop, "PULLUP", evm_mk_number(EVM_GPIO_MODE_PULLUP));
        evm_prop_append(e, mode_prop, "PULLDOWN", evm_mk_number(EVM_GPIO_MODE_PULLDOWN));
        evm_prop_append(e, mode_prop, "FLOAT", evm_mk_number(EVM_GPIO_MODE_FLOAT));
        evm_prop_append(e, mode_prop, "PUSHPULL", evm_mk_number(EVM_GPIO_MODE_PUSHPULL));
        evm_prop_append(e, mode_prop, "OPENDRAIN", evm_mk_number(EVM_GPIO_MODE_OPENDRAIN));
    }
    else
    {
        return e->err;
    }

    evm_val_t *edge_prop = evm_object_create(e, GC_OBJECT, 4, 0);
    if (edge_prop)
    {
        evm_prop_append(e, edge_prop, "NONE", evm_mk_number(EVM_GPIO_EDGE_NONE));
        evm_prop_append(e, edge_prop, "RISING", evm_mk_number(EVM_GPIO_EDGE_RISING));
        evm_prop_append(e, edge_prop, "FALLING", evm_mk_number(EVM_GPIO_EDGE_FALLING));
        evm_prop_append(e, edge_prop, "BOTH", evm_mk_number(EVM_GPIO_EDGE_BOTH));
    }
    else
    {
        return e->err;
    }

    evm_builtin_t builtin[] = {
        {"open", evm_mk_native((intptr_t)evm_module_gpio_open)},
        {"openSync", evm_mk_native((intptr_t)evm_module_gpio_openSync)},
        {"DIRECTION", *dir_prop},
        {"MODE", *mode_prop},
        {"EDGE", *edge_prop},
        {NULL, NULL}};
    evm_module_create(e, "gpio", builtin);
    evm_pop(e);
    evm_pop(e);
    evm_pop(e);
    return e->err;
}
#endif

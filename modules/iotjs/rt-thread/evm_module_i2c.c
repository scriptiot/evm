#ifdef CONFIG_EVM_MODULE_I2C
#include "evm_module.h"
#include <rtthread.h>
#include <rtdevice.h>

typedef struct rt_i2c_bus_device *rt_i2c_bus_device_t;

typedef struct _i2c_bus
{
    rt_uint16_t addr;
    rt_i2c_bus_device_t bus;
} _i2c_bus;

//i2c.open(configuration, callback)
static evm_val_t evm_module_i2c_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t result = evm_module_i2c_openSync();
    if (result == EVM_VAL_UNDEFINED)
        return result;

    evm_val_t args[2];
    args[0] = evm_mk_number(errno);
    args[1] = result;

    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, v + 1, &e->scope, args, 2);

    return result;
}

//i2c.openSync(configuration)
static evm_val_t evm_module_i2c_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t device = evm_prop_get(e, v, "device", 0);
    rt_i2c_bus_device_t i2c_bus_name = (rt_i2c_bus_device_t)rt_device_find(evm_2_string(device));
    if (!i2c_bus_name)
        return EVM_VAL_UNDEFINED;

    evm_val_t address = evm_prop_get(e, v, "address", 0);
    rt_i2c_bus_device_t i2c_bus_addr = (rt_i2c_bus_device_t)rt_device_find(evm_2_integer(address));
    if (!i2c_bus_addr)
        return EVM_VAL_UNDEFINED;

    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 6, 0);
    if (!obj)
        return EVM_VAL_UNDEFINED;

    _i2c_bus *bus = evm_malloc(sizeof(_i2c_bus));
    evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_i2c_class_read));
    evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_i2c_class_readSync));
    evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_i2c_class_write));
    evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_i2c_class_writeSync));
    evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_i2c_class_close));
    evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_i2c_class_closeSync));

    bus->bus = i2c_bus;
    bus->addr = i2c_bus_addr;

    evm_object_set_ext_data(obj, (intptr_t)bus);

    return *obj;
}

//i2cbus.read(length[, callback])
static evm_val_t evm_module_i2c_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t result = evm_module_i2c_class_readSync(e, p, argc, v);
    if (result == EVM_VAL_UNDEFINED)
        return result;
    if (argc < 1 || !evm_is_script(v + 1))
        return EVM_VAL_UNDEFINED;

    evm_val_t args[2];
    args[0] = evm_mk_number(errno);
    args[1] = result;
    evm_run_callback(e, v + 1, &e->scope, args, 2);
}

//i2cbus.readSync(length)
static evm_val_t evm_module_i2c_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;
    rt_size_t size = evm_2_integer(v);
    if (!size)
        return EVM_VAL_UNDEFINED;
    evm_val_t *buffer = evm_buffer_create(e, evm_2_integer(v));
    if (!buffer)
        return EVM_VAL_UNDEFINED;

    _i2c_bus *bus = evm_object_get_ext_data(p);
    if (!bus)
        return EVM_VAL_UNDEFINED;

    rt_device_read(bus->bus, 0, evm_buffer_addr(buffer), size);
    return *buffer;
}

//i2cbus.write(bytes[, callback])
static evm_val_t evm_module_i2c_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t result = evm_module_i2c_class_writeSync(e, p, argc, v);
    if (result == EVM_VAL_UNDEFINED)
        return EVM_VAL_UNDEFINED;

    if (argc < 1 || !evm_is_script(v + 1))
        return EVM_VAL_UNDEFINED;

    evm_val_t args[1];
    args[0] = evm_mk_number(errno);

    evm_run_callback(e, v + 1, &e->scope, args, 1);
}

//i2cbus.writeSync(bytes)
static evm_val_t evm_module_i2c_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    _i2c_bus *bus = evm_object_get_ext_data(p);
    if (!bus)
        return EVM_VAL_UNDEFINED;

    rt_i2c_transfer(bus->bus, evm_buffer_addr(v), evm_buffer_len(v));

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

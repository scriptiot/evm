#ifdef CONFIG_EVM_MODULE_UART
#include "evm_module.h"

#include <rtthread.h>
#include <rtdevice.h>

typedef struct _uart_dev_t {
    rt_device_t dev;
    int baudrate;
    int databits;
} _uart_dev_t;

evm_val_t *evm_module_uart_class_instantiate(evm_t *e);

static evm_val_t _uart_open_device(evm_t *e, evm_val_t *p, int argc, evm_val_t *v, int is_sync) {
	evm_val_t *val = NULL;
	evm_val_t *ret_obj;
	evm_val_t *cb = NULL;
	evm_val_t args;
	_uart_dev_t *dev;

	if( argc < 1)
		return EVM_VAL_UNDEFINED;
	
	if( argc > 1 && evm_is_script(v + 1) && !is_sync ) {
		cb = v + 1;
	} 

	dev = evm_malloc(sizeof(_uart_dev_t));
	if( !dev ) {
		args = evm_mk_foreign_string("Insufficient external memory");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
        evm_set_err(e, ec_memory, "Insufficient external memory");
        return EVM_VAL_UNDEFINED;
	}

	val = evm_prop_get(e, v, "device", 0);
	if( val == NULL || !evm_is_string(val) ) {
		args = evm_mk_foreign_string("Configuration has no 'device' member");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'device' member");
		return EVM_VAL_UNDEFINED;
	}
		
	dev->dev = rt_device_find(evm_2_string(val));
    if ( dev->dev == RT_NULL ) {
        args = evm_mk_foreign_string("Can't find uart device");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
        evm_set_err(e, ec_type, "Can't find uart device");
        return EVM_VAL_UNDEFINED;
    }

	val = evm_prop_get(e, v, "baudRate", 0);
	if( val == NULL || !evm_is_integer(val) ) {
		args = evm_mk_foreign_string("Configuration has no 'baudRate' member");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'baudRate' member");
		return EVM_VAL_UNDEFINED;
	}
	dev->baudrate = evm_2_integer(val);

    val = evm_prop_get(e, v, "dataBits", 0);
	if( val == NULL || !evm_is_integer(val) ) {
		args = evm_mk_foreign_string("Configuration has no 'dataBits' member");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
        evm_set_err(e, ec_type, "Configuration has no 'dataBits' member");
		return EVM_VAL_UNDEFINED;
	}
	dev->databits = evm_2_integer(val);

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = dev->baudrate;
    config.data_bits = dev->databits;
    config.bufsz     = 512;
    rt_device_control(dev->dev, RT_DEVICE_CTRL_CONFIG, &config);
    rt_err_t re = rt_device_open(dev->dev, RT_DEVICE_FLAG_INT_RX);
    if(re != RT_EOK) {
        evm_free(dev);
        evm_set_err(e, ec_type, "Failed to open uart");
		return EVM_VAL_UNDEFINED;
    }

    ret_obj = evm_module_uart_class_instantiate(e);
	if( ret_obj == NULL ) {
		args = evm_mk_foreign_string("Failed to instantiate");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
		return EVM_VAL_UNDEFINED;
	}

	evm_object_set_ext_data(ret_obj, (intptr_t)dev);
}

//uart.open(configuration, callback)
static evm_val_t evm_module_uart_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return _uart_open_device(e, p, argc, v, 0);
}

//uart.openSync(configuration)
static evm_val_t evm_module_uart_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return _uart_open_device(e, p, argc, v, 1);
}

static rt_err_t _uart_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    _uart_dev_t *dev = (_uart_dev_t*)evm_object_get_ext_data(p);
    if( !dev ) 
        return EVM_VAL_UNDEFINED;

    if( argc < 1 || !( evm_is_buffer(v) || evm_is_string(v) ) ) {
        return EVM_VAL_UNDEFINED;
    }

    void *buffer;
    rt_size_t size;

    if( evm_is_string(v) ) {
        buffer = evm_2_string(v);
        size = evm_string_len(v);
    } else {
        buffer = evm_buffer_addr(v);
        size = evm_buffer_len(v);
    }
    return rt_device_write(dev->dev, 0, buffer, size);
}

//uartport.write(data, callback)
static evm_val_t evm_module_uart_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    rt_err_t re = _uart_class_write(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args = evm_mk_number(re);
        evm_run_callback(e, v + 1, NULL, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//uartport.writeSync(data)
static evm_val_t evm_module_uart_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_uart_class_write(e, p, argc, v);
    return EVM_VAL_UNDEFINED;
}

static rt_err_t _uart_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    _uart_dev_t *dev = (_uart_dev_t*)evm_object_get_ext_data(p);
    if( !dev ) 
        return EVM_VAL_UNDEFINED;
    return rt_device_close(dev->dev);
}

//uartport.close([callback])
static evm_val_t evm_module_uart_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	rt_err_t re = _uart_class_close(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args = evm_mk_number(re);
        evm_run_callback(e, v + 1, NULL, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//uartport.closeSync()
static evm_val_t evm_module_uart_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_uart_class_close(e, p, argc, v);
    return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_uart_class_instantiate(evm_t *e)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 4, 0);
	if( obj ) {
		evm_prop_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_uart_class_write));
		evm_prop_append(e, obj, "writeSync", evm_mk_native((intptr_t)evm_module_uart_class_writeSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_uart_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_uart_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_uart(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_uart_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_uart_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "uart", builtin);
	return e->err;
}
#endif

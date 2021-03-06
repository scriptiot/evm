#ifdef CONFIG_EVM_MODULE_UART
#include "evm_module.h"
#include <bl_uart.h>
#include <hal_board.h>
#include <vfs.h>
#include <fdt.h>
#include <libfdt.h>

#define _UART_READ_BUF_SIZE 512

typedef struct _uart_dev_t {
    char *dev;
    int baudrate;
    int databits;
    int fd;
    int obj_id;
    uint8_t buffer[_UART_READ_BUF_SIZE];
} _uart_dev_t;

evm_val_t *evm_module_uart_class_instantiate(evm_t *e);

static int get_dts_addr(const char *name, uint32_t *start, uint32_t *off)
{
    uint32_t addr = hal_board_get_factory_addr();
    const void *fdt = (const void *)addr;
    uint32_t offset;

    if (!name || !start || !off)
    {
        return -1;
    }

    offset = fdt_subnode_offset(fdt, 0, name);
    if (offset <= 0)
    {
        return -1;
    }

    *start = (uint32_t)fdt;
    *off = offset;

    return 0;
}

static evm_val_t evm_module_uart_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _uart_dev_t *uart = (_uart_dev_t *)evm_object_get_ext_data(p);
    if (!uart)
        return EVM_VAL_UNDEFINED;

    evm_module_event_add_listener(e, p, evm_2_string(v), v + 1);
    return EVM_VAL_UNDEFINED;
}

static void _uart_thread(_uart_dev_t *uart)
{
    int bytes_read;
    while (1)
    {
        bytes_read = aos_read(uart->fd, uart->buffer, _UART_READ_BUF_SIZE);
        if (bytes_read > 0 && bytes_read < _UART_READ_BUF_SIZE)
        {
            evm_val_t *obj = evm_module_registry_get(evm_runtime, uart->obj_id);
            if (obj)
            {
                evm_val_t *args = evm_buffer_create(evm_runtime, bytes_read);
                if (args)
                {
                    memcpy(evm_buffer_addr(args), uart->buffer, bytes_read);
                    evm_module_event_emit(evm_runtime, obj, "data", 1, args);
                    evm_pop(evm_runtime);
                }
            }
        }
        else if (bytes_read < 0)
        {
            close(uart->fd);
            break;
        }
        vTaskDelay(10);
    }
    evm_free(uart);
}

static evm_val_t _uart_open_device(evm_t *e, evm_val_t *p, int argc, evm_val_t *v, int is_sync) {
    evm_val_t *val = NULL;
	evm_val_t *ret_obj;
	evm_val_t *cb = NULL;
	evm_val_t args;
	_uart_dev_t *dev;

    if (argc < 1)
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
    dev->dev = evm_2_string(val);

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

    // id tx_pin rx_pin cts_pin rts_pin baudrate
    bl_uart_init(1, 4, 3, 255, 255, dev->baudrate);

    uint32_t fdt = 0, offset = 0;
    /* uart */
    if (0 == get_dts_addr("uart", &fdt, &offset))
    {
        vfs_uart_init(fdt, offset);
    }

    dev->fd = aos_open(dev->dev, 0);
    if (dev->fd < 0)
    {
        evm_free(dev);
        evm_set_err(e, ec_type, "Failed to open uart");
		return EVM_VAL_UNDEFINED;
    }

    xTaskCreate(_uart_thread, "uart-task", 100, dev, 13, NULL);

    ret_obj = evm_module_uart_class_instantiate(e);
    if (ret_obj == NULL)
    {
        args = evm_mk_foreign_string("Failed to instantiate");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(dev);
		return EVM_VAL_UNDEFINED;
    }

    evm_object_set_ext_data(ret_obj, (intptr_t)dev);
    return *ret_obj;
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

static ssize_t _uart_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _uart_dev_t *dev = (_uart_dev_t*)evm_object_get_ext_data(p);
    if( !dev ) 
        return EVM_VAL_UNDEFINED;

    if( argc < 1 || !( evm_is_buffer(v) || evm_is_string(v) ) ) {
        return EVM_VAL_UNDEFINED;
    }

    void *buffer;
    uint32_t size;

    if( evm_is_string(v) ) {
        buffer = evm_2_string(v);
        size = evm_string_len(v);
    } else {
        buffer = evm_buffer_addr(v);
        size = evm_buffer_len(v);
    }

    return aos_write(dev->fd, buffer, size);
}

//uartport.write(data, callback)
static evm_val_t evm_module_uart_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    ssize_t ret = _uart_class_write(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args = evm_mk_number(ret);
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

static int _uart_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    _uart_dev_t *dev = (_uart_dev_t*)evm_object_get_ext_data(p);
    if( !dev )
        return -1;

    int ret = aos_close(dev->fd);
    evm_free(dev);
    return ret;
}

//uartport.close([callback])
static evm_val_t evm_module_uart_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int ret = _uart_class_close(e, p, argc, v);
    if (argc > 0 && evm_is_script(v))
    {
        evm_val_t args = evm_mk_number(ret);
        evm_run_callback(e, v, NULL, &args, 1);
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
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_uart_on));
    }
	return obj;
}

evm_err_t evm_module_uart(evm_t *e) {
    evm_builtin_t builtin[] = {
        {"open", evm_mk_native((intptr_t)evm_module_uart_open)},
        {"openSync", evm_mk_native((intptr_t)evm_module_uart_openSync)},
        {NULL, NULL}};
    evm_module_create(e, "uart", builtin);
	return e->err;
}
#endif

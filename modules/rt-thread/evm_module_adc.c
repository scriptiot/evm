#include "evm_module.h"
#include <rtthread.h>
#include <rtdevice.h>

typedef struct _adc_dev_t {
	rt_adc_device_t dev;
	int channel;
} _adc_dev_t;

evm_val_t *evm_module_adc_class_instantiate(evm_t *e);

static evm_val_t _adc_open_device(evm_t *e, evm_val_t *p, int argc, evm_val_t *v, int is_sync) {
	evm_val_t *val = NULL;
	evm_val_t *ret_obj;
	evm_val_t *cb = NULL;
	evm_val_t args;
	_adc_dev_t *adc_dev;

	if( argc < 1)
		return EVM_VAL_UNDEFINED;
	
	if( argc > 1 && evm_is_script(v + 1) && !is_sync ) {
		cb = v + 1;
	} 

	adc_dev = evm_malloc(sizeof(_adc_dev_t));
	if( !adc_dev ) {
		args = evm_mk_foreign_string("Insufficient external memory");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
        return EVM_VAL_UNDEFINED;
	}

	val = evm_prop_get(e, v, "device", 0);
	if( val == NULL || !evm_is_string(val) ) {
		args = evm_mk_foreign_string("Configuration has no 'device' member");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(adc_dev);
		return EVM_VAL_UNDEFINED;
	}
		
	adc_dev->dev = (rt_adc_device_t)rt_device_find(evm_2_string(val));
    if ( adc_dev->dev == RT_NULL ) {
        args = evm_mk_foreign_string("Can't find adc device");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(adc_dev);
        return EVM_VAL_UNDEFINED;
    }

	val = evm_prop_get(e, v, "channel", 0);
	if( val == NULL || !evm_is_integer(val) ) {
		args = evm_mk_foreign_string("Configuration has no 'channel' member");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(adc_dev);
		return EVM_VAL_UNDEFINED;
	}

	adc_dev->channel = evm_2_integer(val);

	ret_obj = evm_module_adc_class_instantiate(e);
	if( ret_obj == NULL ) {
		args = evm_mk_foreign_string("Failed to instantiate");
		if( cb )
			evm_run_callback(e, cb, NULL, &args, 1);
		evm_free(adc_dev);
		return EVM_VAL_UNDEFINED
	}
	rt_adc_enable(adc_dev->dev, adc_dev->channel);
	evm_object_set_ext_data(ret_obj, (intptr_t)adc_dev);
}

//adc.open(configuration, callback)
static evm_val_t evm_module_adc_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return _adc_open_device(e, p, argc, v, 0);
}

//adc.openSync(configuration)
static evm_val_t evm_module_adc_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return _adc_open_device(e, p, argc, v, 1);
}

//adcpin.read(callback)
static evm_val_t evm_module_adc_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_adc_dev_t *adc_dev = (_adc_dev_t *)evm_object_get_ext_data(p);
	evm_val_t args[2];
	if( !adc_dev )
		return EVM_VAL_UNDEFINED;
	
	if( argc > 0 && evm_is_script(v) ) {
		args[0] = evm_mk_null();
		args[1] = rt_adc_read(adc_dev->dev, adc_dev->channel);
		evm_run_callback(e, v, NULL, args, 2);
	}
	return EVM_VAL_UNDEFINED;
}

//adcpin.readSync()
static evm_val_t evm_module_adc_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_adc_dev_t *adc_dev = (_adc_dev_t *)evm_object_get_ext_data(p);
	if( !adc_dev )
		return EVM_VAL_UNDEFINED;
	
	return evm_mk_number( rt_adc_read(adc_dev->dev, adc_dev->channel) );
}

//adcpin.close([callback])
static evm_val_t evm_module_adc_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_adc_dev_t *adc_dev = (_adc_dev_t *)evm_object_get_ext_data(p);
	rt_err_t err;
	evm_val_t args;
	if( !adc_dev )
		return EVM_VAL_UNDEFINED;
	err = rt_adc_disable(adc_dev->dev, adc_dev->channel);

	if( argc > 0 && evm_is_script(v) ) {
		args = evm_mk_number(err);
		evm_run_callback(e, v, NULL, &args, 1);
	}
	return EVM_VAL_UNDEFINED;
}

//adcpin.closeSync()
static evm_val_t evm_module_adc_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	_adc_dev_t *adc_dev = (_adc_dev_t *)evm_object_get_ext_data(p);
	if( !adc_dev )
		return EVM_VAL_UNDEFINED;
	rt_adc_disable(adc_dev->dev, adc_dev->channel) ;
	return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_adc_class_instantiate(evm_t *e)
{
	evm_val_t *obj = evm_object_create(e, GC_OBJECT, 4, 0);
	if( obj ) {
		evm_prop_append(e, obj, "read", evm_mk_native((intptr_t)evm_module_adc_class_read));
		evm_prop_append(e, obj, "readSync", evm_mk_native((intptr_t)evm_module_adc_class_readSync));
		evm_prop_append(e, obj, "close", evm_mk_native((intptr_t)evm_module_adc_class_close));
		evm_prop_append(e, obj, "closeSync", evm_mk_native((intptr_t)evm_module_adc_class_closeSync));
	}
	return obj;
}

evm_err_t evm_module_adc(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"open", evm_mk_native((intptr_t)evm_module_adc_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_adc_openSync)},
		{NULL, NULL}
	};
	evm_module_create(e, "adc", builtin);
	return e->err;
}
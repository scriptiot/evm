#include "evm_module.h"

evm_t *evm_runtime;

static evm_hash_t _hashname_events;

struct _module_registry_t {
    evm_val_t *start;
    int size;
} _module_registry;

void evm_module_registry_init(evm_t *e, int size) {
    evm_runtime = (evm_val_t *)e;
    e->sp++;
    _module_registry.start = e->sp;
    e->sp += size;
    _module_registry.size = size;

    for(uint32_t i = 0; i < _module_registry.size; i++) {
        _module_registry.start[i] = EVM_VAL_UNDEFINED;
    }

    _hashname_events = evm_str_insert(e, "_events", 0);
}

int evm_module_registry_add(evm_t *e, evm_val_t *v) {
    for(int i = 0; i < _module_registry.size; i++) {
        if( _module_registry.start[i] == EVM_VAL_UNDEFINED ) {
            _module_registry.start[i] = *v;
            return i;
        }
    }
    return -1;
}

evm_val_t *evm_module_registry_get(evm_t *e, int id) {
    if( id < 0 || id >= _module_registry.size )
        return NULL;
    return _module_registry.start + id;
}

void evm_module_registry_remove(evm_t *e, int id) {
    if( id < 0 || id >= _module_registry.size )
        return;
    _module_registry.start[id] = EVM_VAL_UNDEFINED;
}

void evm_module_next_tick(evm_t *e, int argc, evm_val_t *v) {
#ifdef CONFIG_EVM_MODULE_PROCESS
    evm_module_process_nextTick(e, NULL, argc, v);
#endif
}

evm_err_t evm_module_event_add_listener(evm_t *e, evm_val_t *pthis, const char *type, evm_val_t *listener) {
    if( !evm_is_script(listener) ) {
        return evm_set_err(e, ec_type, "Listener must be a function");
    }
    evm_val_t *prop = evm_prop_get_by_key(e, pthis, _hashname_events, 0);
    if( !prop ) {
        prop = evm_object_create(e, GC_OBJECT, 0, 0);
        if( prop )
            evm_prop_push_with_key(e, pthis, _hashname_events, prop);
    }

    evm_prop_append(e, prop, type, *listener);
    return ec_ok;
}

void evm_module_event_remove_listener(evm_t *e, evm_val_t *pthis, const char *type) {
    evm_val_t *prop = evm_prop_get_by_key(e, pthis, _hashname_events, 0);
    if( !prop ) {
        return;
    }
    evm_val_t *listener = evm_prop_get(e, prop, type, 0);
    if( listener ) {
        evm_set_undefined(listener);
    }
}

void evm_module_event_emit (evm_t *e, evm_val_t *pthis, const char *type, int argc, evm_val_t *v) {
    evm_val_t *prop = evm_prop_get_by_key(e, pthis, _hashname_events, 0);
    if( !prop ) {
        return;
    }

    evm_val_t *listener = evm_prop_get(e, prop, type, 0);
    if( listener && evm_is_script(listener)) {
        evm_val_t args[argc + 1];
        args[0] = *listener;
        for(int i = 1; i < argc + 1; i++) {
            args[i] = *(v + i - 1);
        }
        evm_module_next_tick(e, argc + 1, args);
    }
}

evm_err_t evm_module_init(evm_t *env)
{
    evm_err_t err;
#ifdef CONFIG_EVM_MODULE_ADC
    err = evm_module_adc(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create adc module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_UART
    err = evm_module_uart(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create uart module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_GPIO
    err = evm_module_gpio(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create gpio module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_FS
    err = evm_module_fs(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create fs module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_NET
    err = evm_module_net(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create net module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_HTTP
    err = evm_module_http(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create http module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_PROCESS
    err = evm_module_process(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create process module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_EVENTS
    err = evm_module_events(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create events module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_DNS
    err = evm_module_dns(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create dns module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_TIMERS
    err = evm_module_timers(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create timers module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_BUFFER
    err = evm_module_buffer(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create buffer module\r\n");
        return err;
    }
#endif

#ifdef CONFIG_EVM_MODULE_ASSERT
    err = evm_module_assert(env);
    if (err != ec_ok)
    {
        evm_print("Failed to create assert module\r\n");
        return err;
    }
#endif
    return ec_ok;
}

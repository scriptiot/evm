#include "evm_module.h"

static evm_hash_t _hashname_events;

struct _module_registry_t {
    evm_val_t *start;
    int size;
} _module_registry;

void evm_module_registry_init(evm_t *e, int size) {
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
    if( listener ) {
        evm_val_t args[argc + 1];
        args[0] = *listener;
        for(int i = 1; i < argc; i++) {
            args[i] = *(v + i - 1);
        }
        evm_module_next_tick(e, argc + 1, args);
    }
}



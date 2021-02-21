#ifdef CONFIG_EVM_MODULE_EVENTS
#include "evm_module.h"

static evm_hash_t _hashname_events;

//emitter.addListener(event, listener)
static evm_val_t evm_module_events_emitter_addListener(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    if( argc < 2 || !evm_is_string(v) || !evm_is_script(v + 1) )
        return EVM_VAL_UNDEFINED;
    evm_val_t *events = evm_prop_get_by_key(e, p, _hashname_events, 0);
    if( events ) {
        evm_prop_append(e, events, evm_2_string(v), *(v + 1));
    }
    return EVM_VAL_UNDEFINED;
}

//emitter.on(event, listener)
static evm_val_t evm_module_events_emitter_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    return evm_module_events_emitter_addListener(e, p, argc, v);
}

//emitter.emit(event[, args..])
static evm_val_t evm_module_events_emitter_emit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    if( argc < 1 || !evm_is_string(v) )
        return EVM_VAL_UNDEFINED;
    evm_module_event_emit(e, p, evm_2_string(v), argc - 1, v + 1);
    return EVM_VAL_UNDEFINED;
}

//emitter.once(event, listener)
static evm_val_t evm_module_events_emitter_once(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    return evm_module_events_emitter_addListener(e, p, argc, v);
}

//emitter.removeListener(event, listener)
static evm_val_t evm_module_events_emitter_removeListener(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    evm_val_t *events = evm_prop_get_by_key(e, p, _hashname_events, 0);
    if( events ) {
        evm_prop_append(e, events, evm_2_string(v), EVM_VAL_UNDEFINED);
    }
    return EVM_VAL_UNDEFINED;
}

//emitter.removeAllListeners([event])
static evm_val_t evm_module_events_emitter_removeAllListeners(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    evm_val_t *events = evm_object_create(e, GC_OBJECT, 0, 0);
    if( events )
        evm_prop_push_with_key(e, p, _hashname_events, events);
    return EVM_VAL_UNDEFINED;
}

//emitter.EventEmitter
static evm_val_t evm_module_events_emitter_EventEmitter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 7, 0);
    if( obj ) {
        evm_prop_append(e, obj, "addListener", evm_mk_native((intptr_t)evm_module_events_emitter_addListener));
        evm_prop_append(e, obj, "on", evm_mk_native((intptr_t)evm_module_events_emitter_on));
        evm_prop_append(e, obj, "emit", evm_mk_native((intptr_t)evm_module_events_emitter_emit));
        evm_prop_append(e, obj, "once", evm_mk_native((intptr_t)evm_module_events_emitter_once));
        evm_prop_append(e, obj, "removeListener", evm_mk_native((intptr_t)evm_module_events_emitter_removeListener));
        evm_prop_append(e, obj, "removeAllListeners", evm_mk_native((intptr_t)evm_module_events_emitter_removeAllListeners));
        evm_val_t *events = evm_object_create(e, GC_OBJECT, 0, 0);
        if( events )
            evm_prop_push_with_key(e, obj, _hashname_events, events);
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_events(evm_t *e) {
    _hashname_events = evm_str_insert(e, "_events", 0);
    evm_builtin_t builtin[] = {
        {"EventEmitter", evm_mk_native((intptr_t)evm_module_events_emitter_EventEmitter)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "events", builtin);
    return e->err;
}

#endif

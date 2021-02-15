#include "evm_module.h"

static evm_hash_t _name_arguments;

static evm_val_t *evm_process_queue;
//process.nextTick(callback, [...args]);
evm_val_t evm_module_process_nextTick(evm_t *e, evm_val_t *p, int argc, evm_val_t *v) {
    if( argc > 0 && evm_is_script(v) ) {
        if( argc > 1 ) {
            evm_val_t *arguments = evm_list_create(e, GC_LIST, argc - 1);
            if( arguments ) {
                for(uint32_t i = 0; i < argc - 1; i++){
                    evm_list_set(e, arguments, i, *(v + 1));
                }
                evm_attr_append_with_key(e, v, _name_arguments, *arguments);
            }
        }
        evm_list_append(e, evm_process_queue, *v);
    }
    return EVM_VAL_UNDEFINED;
}

void evm_module_process_poll(evm_t *e) {
    uint32_t len = evm_list_len(evm_process_queue);
    for(uint32_t index = 0; index < len; index++) {
        evm_val_t *callback = evm_list_pop(e, evm_process_queue);
        evm_val_t *arguments = evm_attr_get_by_key(e, callback, _name_arguments);
        if( arguments && evm_is_list(arguments) && evm_list_len(arguments) > 0 ) {
            uint32_t argc = evm_list_len(arguments);
            evm_val_t args[ argc ];
            for(uint32_t i = 0; i < argc; i++){
                args[i] = *evm_list_get(e, arguments, i);
            }
            evm_run_callback(e, callback, &e->scope, args, argc);
        } else {
            evm_run_callback(e, callback, &e->scope, NULL, 0);
        }
        len = evm_list_len(evm_process_queue);
    }
}

evm_err_t evm_module_process(evm_t *e) {
    _name_arguments = evm_str_insert(e, "arguments", 0);
    evm_process_queue = evm_list_create(e, GC_LIST, 0);
    evm_push_value(e, EVM_VAL_UNDEFINED);
    evm_builtin_t builtin[] = {
        {"nextTick", evm_mk_native((intptr_t)evm_module_process_nextTick)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "process", builtin);
    return e->err;
}

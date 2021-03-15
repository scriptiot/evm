#ifdef CONFIG_EVM_MODULE_MPY
#include "evm_module.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/builtin.h"
#include "evm.h"

static char *_compat_mp_stack_top;
evm_val_t _compat_mp_convert_basic_type_to_evm_type(evm_t * e, mp_obj_t from);

static void _compact_mp_do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

static void _compat_mp_convert_from_evm_type(evm_t * e, int argc, evm_val_t * from, mp_obj_t * to){
    for(int i = 0; i < argc; i++){
        switch (evm_type(from + i)) {
            case TYPE_FOREIGN_STRING:
            case TYPE_HEAP_STRING: {
                to[i] = mp_obj_new_str(evm_2_string(from + i), (size_t)evm_string_len(from + i));
            }break;
            case TYPE_NUMBER: {
                to[i] = mp_obj_new_int( evm_2_integer(from + i) );
            }break;
            case TYPE_UNDEFINED: {
                to[i] = mp_const_none;
            }break;
            case TYPE_BOOLEAN:
                to[i] = mp_obj_new_bool(evm_2_boolean(from + i));
            break;
            case TYPE_BUFFER: {
                mp_obj_new_bytes(evm_buffer_addr(from + i), evm_buffer_len(from + i));
            }break;
            case TYPE_LIST: {
                int size = evm_list_len(from + i);
                mp_obj_t items[size];
                for (int j = 0; j < size; j++) {
                    _compat_mp_convert_from_evm_type(e, 1, evm_list_get(e, from + i, j), items + j);
                }
                to[i] = mp_obj_new_list(size, items);
            }break;
            case TYPE_OBJECT:{
                if( evm_get_gc_type(e, from + i) == GC_DICT ) {
                    int size = evm_prop_len(from + i);
                    mp_obj_t dict_obj = mp_obj_new_dict(size);
                    mp_obj_t item;
                    for (int j = 0; j < size; j++) {
                        _compat_mp_convert_from_evm_type(e, 1, evm_prop_get_by_index(e, from + i, j), &item);
                        uint32_t key_hash = evm_prop_get_key_by_index(e, from + i, j);
                        const char * str = evm_string_get(e, key_hash);
                        mp_obj_dict_store(dict_obj, mp_obj_new_str(str, strlen(str)), item);
                    }
                    to[i] = dict_obj;
                } else {
                    mp_obj_t local_obj = (mp_obj_t)evm_object_get_ext_data(from + i);
                    if( local_obj != NULL ){
                        to[i] = local_obj;
                    } else {
                        to[i] = MP_ROM_NONE;
                    }
                }
            }break;
        }
    }
}

static evm_val_t _compat_mp_eval(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( argc >0 && evm_is_string(v) ) {
        _compact_mp_do_str(evm_2_string(v), MP_PARSE_SINGLE_INPUT);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t _compat_mp_getter(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok){
    EVM_UNUSED(argc);
    *ok = ec_ok;
    mp_obj_t dest[2];
    qstr attr_name = qstr_from_str( evm_string_get(e, name) );
    mp_obj_t local_obj = (mp_obj_t)evm_object_get_ext_data(p);
    if( local_obj == NULL ) {
        local_obj = mp_load_name(attr_name);
        if( local_obj == MP_OBJ_NULL ) {
            *ok = ec_err;
            return EVM_VAL_UNDEFINED;
        }
        else {
            *(v + 1) = *p;
            return _compat_mp_convert_basic_type_to_evm_type(e, local_obj);
        }
    }
    mp_load_method_protected(local_obj, attr_name, dest, false);
    if (dest[0] == MP_OBJ_NULL) {
        *ok = ec_err;
        return EVM_VAL_UNDEFINED;
    }
    *(v + 1) = *p;
    return _compat_mp_convert_basic_type_to_evm_type(e, dest[0]);
}

static evm_val_t _compat_mp_setter(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok){
    *ok = ec_ok;
    mp_obj_t args[argc];
    _compat_mp_convert_from_evm_type(e, argc, v, args);
    qstr attr_name = qstr_from_str( evm_string_get(e, name) );
    mp_obj_t local_obj = (mp_obj_t)evm_object_get_ext_data(p);
    if( local_obj == NULL ) {
        mp_store_name(attr_name, args[0]);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t _compat_mp_caller(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( !evm_is_invoke(p) )
        return EVM_VAL_UNDEFINED;
    mp_obj_t args[argc];
    _compat_mp_convert_from_evm_type(e, argc, v, args);
    mp_obj_t fun = (mp_obj_t)evm_2_intptr(p);
    return _compat_mp_convert_basic_type_to_evm_type(e, mp_call_function_n_kw(fun, argc, 0, args));
}

static struct evm_object_native_t compat_mp_native = {
    .get_prop = _compat_mp_getter,
    .set_prop = _compat_mp_setter,
    .caller = _compat_mp_caller,
    .creator = NULL,
    .destroyer = NULL,
};

evm_val_t _compat_mp_convert_basic_type_to_evm_type(evm_t * e, mp_obj_t from){
    const mp_obj_type_t *type = mp_obj_get_type(from);
    if( mp_obj_is_int(from) ) {
        return evm_mk_number(mp_obj_get_int(from));
    } else if( mp_obj_is_float(from) ) {
#if MICROPY_PY_BUILTINS_FLOAT
        return evm_mk_number(mp_obj_get_float(from));
#else
        return EVM_VAL_UNDEFINED;
#endif
    } else if( mp_obj_is_bool(from) ) {
        if( mp_obj_is_true(from)) {
            return EVM_VAL_TRUE;
        } else {
            return EVM_VAL_FALSE;
        }
    } else if( mp_obj_is_str(from) ) {
        return evm_mk_foreign_string((intptr_t)mp_obj_str_get_str(from));
    } else if( mp_obj_is_fun(from) ) {
        return evm_mk_invoke(from);
    }  else if( type == &mp_type_bytes ) {
        const char * buf = mp_obj_str_get_str(from);
        int len = mp_obj_get_int(mp_obj_len(from));
        evm_val_t * res = evm_buffer_create(e, len);
        memcpy( evm_buffer_addr(res), buf, len);
        return *res;
    } else if( mp_obj_is_obj(from) ) {
        evm_val_t * obj = evm_object_create(e, GC_OBJECT, 0, 0);
        evm_object_set_native(obj, &compat_mp_native);
        evm_object_set_ext_data(obj, (intptr_t)from);
        return *obj;
    } else {
        return EVM_VAL_UNDEFINED;
    }
}

evm_err_t compat_mp_module(evm_t * e) {
    evm_builtin_t natives[] = {
        {"eval", evm_mk_native( (intptr_t)_compat_mp_eval )},
        {NULL, EVM_VAL_UNDEFINED},
    };
    evm_val_t mpy_module = evm_module_create(e, "mpy", natives);
    evm_object_set_native(&mpy_module, &compat_mp_native);
    evm_object_set_ext_data(&mpy_module, NULL);

    int stack_dummy;
    _compat_mp_stack_top = (char *)&stack_dummy;

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    return e->err;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    EVM_UNUSED(filename);
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    EVM_UNUSED(path);
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    EVM_UNUSED(n_args);EVM_UNUSED(args);EVM_UNUSED(kwargs);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

#endif


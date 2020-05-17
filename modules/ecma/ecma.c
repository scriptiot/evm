/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#include "ecma.h"

evm_val_t * ecma_object_prototype;
evm_val_t * ecma_function_prototype;

static evm_val_t ecma_typeof(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);
    if( argc > 0){
        switch (evm_type(v)) {
            case TYPE_BOOLEAN: return evm_mk_foreign_string((intptr_t)"boolean");
            case TYPE_NUMBER: return evm_mk_foreign_string((intptr_t)"number");
            case TYPE_HEAP_STRING:
            case TYPE_FOREIGN_STRING: return evm_mk_foreign_string((intptr_t)"string");
            case TYPE_FUNCTION: return evm_mk_foreign_string((intptr_t)"function");
            case TYPE_UNDEFINED: return evm_mk_foreign_string((intptr_t)"undefined");
        default:
            return evm_mk_foreign_string((intptr_t)"object");
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t ecma_new(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);
    if(argc > 0){
        if( evm_is_script(v) ) {
            evm_val_t * fn = v;
            if( evm_script_get_native(v) != NULL){
                evm_native_fn fn = evm_script_get_native(v);
                return fn(e, v, argc - 1, v + 1);
            } else {
                evm_val_t * o = evm_object_create(e, GC_DICT, 0, 0);
            #ifdef CONFIG_EVM_ECMA_OBJECT
                evm_val_t * prototype = evm_attr_get(e, fn, "prototype");
                ecma_object_attrs_apply(e, o, prototype);
                evm_attr_set(e, o, 1, "constructor", *fn);
            #endif
                evm_run_callback(e, fn, o, v + 1, argc - 1);
                return *o;
            }
        } else if( evm_is_class(v) ){
            evm_val_t * o = evm_object_create_by_class(e, GC_DICT, v);
            if( evm_class_get_native(v) != NULL){
                evm_native_fn fn = evm_class_get_native(v);
                fn(e, o, argc - 1, v + 1);
            } else {
                evm_run_callback(e, v, o, v + 1, argc - 1);
            }
            return *o;
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t ecma_function_call(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( argc >= 1 && p && evm_is_script(p) && evm_is_object(v) ){
        evm_val_t ret_val = evm_run_callback(e, p, v, v + 1, argc - 1);
        return ret_val;
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t * ecma_prototype_create(evm_t *e ){
    evm_val_t * o = evm_object_create(e, GC_DICT, 0, 1);
    evm_attr_set(e, o, 0, "constructor", evm_mk_native( (intptr_t)ecma_new ));
    return o;
}

static evm_val_t ecma_set_prototype(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(p);
    if( argc > 0 ){
        if( evm_is_script(v) ){
            if( evm_attr_create(e, v, 4) == ec_ok ){
                evm_attr_set(e, v, 0, "constructor", *v);
                evm_attr_set(e, v, 1, "prototype", * ecma_prototype_create(e));
                evm_attr_set(e, v, 2, "call", evm_mk_native((intptr_t)ecma_function_call));
                evm_attr_set(e, v, 3, "__proto__", *ecma_object_prototype);
            }
            evm_set_parent(v, ecma_object_prototype);
        } else if( evm_is_object(v)){
        #ifdef CONFIG_EVM_ECMA_OBJECT
            ecma_object_attrs_apply(e, v, ecma_object_prototype);
        #endif
            evm_set_parent(v, ecma_object_prototype);
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t ecma_isNaN(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(p);
    if( argc == 0 ) return EVM_VAL_TRUE;
    if( argc > 0 ) if( evm_is_nan(v) ) return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
}

#ifdef CONFIG_EVM_ECMA_OBJECT
extern evm_val_t * ecma_object_init(evm_t * e);
#endif
#ifdef CONFIG_EVM_ECMA_ARRAY
extern evm_val_t * ecma_array_init(evm_t * e);
#endif
#ifdef CONFIG_EVM_ECMA_NUMBER
extern evm_val_t * ecma_number_init(evm_t * e);
#endif
#ifdef CONFIG_EVM_ECMA_MATH
extern evm_val_t * ecma_math_init(evm_t * e);
#endif
#ifdef CONFIG_EVM_ECMA_STRING
extern evm_val_t * ecma_string_init(evm_t * e);
#endif
#ifdef CONFIG_EVM_ECMA_REGEXP
extern evm_val_t * ecma_regex_init(evm_t * e);
#endif

int ecma_module(evm_t * e){
    ecma_function_prototype = evm_object_create(e, GC_DICT, 0, 0);
    ecma_object_prototype = evm_object_create(e, GC_DICT, 0, 0);

#ifdef CONFIG_EVM_ECMA_OBJECT
    ecma_object_attrs_apply(e, ecma_object_prototype, ecma_function_prototype);
    ecma_Object = ecma_object_init(e);
#endif
#ifdef CONFIG_EVM_ECMA_ARRAY
    ecma_Array = ecma_array_init(e);
#endif
#ifdef CONFIG_EVM_ECMA_NUMBER
    ecma_Number = ecma_number_init(e);
#endif
#ifdef CONFIG_EVM_ECMA_MATH
    ecma_Math = ecma_math_init(e);
#endif
#ifdef CONFIG_EVM_ECMA_STRING
    ecma_String = ecma_string_init(e);
#endif
#ifdef CONFIG_EVM_ECMA_REGEXP
    ecma_RegExp = ecma_regex_init(e);
#endif
    evm_builtin_t natives[] = {
        {".set_prototype", evm_mk_native( (intptr_t)ecma_set_prototype )},
        {".new", evm_mk_native( (intptr_t)ecma_new )},
        {"isNaN", evm_mk_native( (intptr_t)ecma_isNaN )},
        {"typeof", evm_mk_native( (intptr_t)ecma_typeof )},
    #ifdef CONFIG_EVM_ECMA_REGEXP
        {"RegExp", *ecma_RegExp},
    #endif
    #ifdef CONFIG_EVM_ECMA_MATH
        {"Math", *ecma_Math},
    #endif
    #ifdef CONFIG_EVM_ECMA_OBJECT
        {"Object", *ecma_Object},
    #endif
    #ifdef CONFIG_EVM_ECMA_ARRAY
        {"Array", *ecma_Array},
    #endif
    #ifdef CONFIG_EVM_ECMA_NUMBER
        {"Number", *ecma_number_init(e)},
    #endif
    #ifdef CONFIG_EVM_ECMA_STRING
        {"String", *ecma_string_init(e)},
    #endif
        {NULL, EVM_VAL_UNDEFINED}
    };
    return evm_native_add(e, natives);
}

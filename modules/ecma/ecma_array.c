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

#ifdef CONFIG_EVM_ECMA_ARRAY
#include "ecma.h"

evm_val_t * ecma_Array;

#ifdef CONFIG_EVM_ECMA_OBJECT
void ecma_object_attrs_set(evm_t * e, evm_val_t * o, evm_val_t * object, evm_val_t * prototype);
#endif

evm_val_t ecma_array_concat(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_fill(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_indexOf(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_push(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_pop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_shift(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_slice(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_copyWithin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_unshift(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_filter(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_map(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_find(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
evm_val_t ecma_array_join(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

void ecma_array_set_length(evm_t * e, evm_val_t * o, int len){
#ifdef CONFIG_EVM_ECMA_OBJECT
    evm_attr_set_by_index(e, o, ECMA_OBJECT_ATTR_SIZE, evm_mk_number(len));
#else
    evm_attr_set_by_index(e, o, 0, evm_mk_number(len));
#endif
}

int ecma_array_get_length(evm_t * e, evm_val_t * o){
#ifdef CONFIG_EVM_ECMA_OBJECT
    return evm_2_integer( evm_attr_get_by_index(e, o, ECMA_OBJECT_ATTR_SIZE) );
#else
    return evm_2_integer( evm_attr_get_by_index(e, o, 0) );
#endif
}

void ecma_array_attrs_apply(evm_t * e, evm_val_t * o){
    int index = 0;
    #ifdef CONFIG_EVM_ECMA_OBJECT
    if( evm_attr_create(e, o, ECMA_OBJECT_ATTR_SIZE + ECMA_ARRAY_ATTR_SIZE) == ec_ok ){
        ecma_object_attrs_set(e, o, ecma_Array, ecma_object_prototype);
        index = ECMA_OBJECT_ATTR_SIZE;
    #else
    if( evm_attr_create(e, o, ECMA_ARRAY_ATTR_SIZE) == ec_ok ){
    #endif
        evm_attr_set(e, o, index++, "length", evm_mk_number( evm_list_len(o) ));
        evm_attr_set(e, o, index++, "push", evm_mk_native((intptr_t)ecma_array_push));
        evm_attr_set(e, o, index++, "pop", evm_mk_native((intptr_t)ecma_array_pop));
        evm_attr_set(e, o, index++, "concat", evm_mk_native((intptr_t)ecma_array_concat));
        evm_attr_set(e, o, index++, "fill", evm_mk_native((intptr_t)ecma_array_fill));
        evm_attr_set(e, o, index++, "indexOf", evm_mk_native((intptr_t)ecma_array_indexOf));
        evm_attr_set(e, o, index++, "shift", evm_mk_native((intptr_t)ecma_array_shift));
        evm_attr_set(e, o, index++, "slice", evm_mk_native((intptr_t)ecma_array_slice));
        evm_attr_set(e, o, index++, "copyWithin", evm_mk_native((intptr_t)ecma_array_copyWithin));
        evm_attr_set(e, o, index++, "unshift", evm_mk_native((intptr_t)ecma_array_unshift));
        evm_attr_set(e, o, index++, "filter", evm_mk_native((intptr_t)ecma_array_filter));
        evm_attr_set(e, o, index++, "map", evm_mk_native((intptr_t)ecma_array_map));
        evm_attr_set(e, o, index++, "find", evm_mk_native((intptr_t)ecma_array_find));
        evm_attr_set(e, o, index++, "join", evm_mk_native((intptr_t)ecma_array_join));
        evm_set_parent(o, ecma_object_prototype);
    }
}

evm_val_t ecma_array_push(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) && argc >= 1 ){
        int len = ecma_array_get_length(e, p);
        int real_len = evm_list_len(p);
        if( real_len > 0 && len < real_len )
            evm_list_set(e, p, len, *v);
        else
            evm_list_push(e, p, argc, v);
        ecma_array_set_length(e, p, len + argc);
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_pop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)argc;
    (void)v;

    if( evm_list_len(p) ){
        int len = ecma_array_get_length(e, p);
        evm_val_t val = *evm_list_get(e, p, len - 1);
        evm_list_set(e, p, len - 1, EVM_VAL_UNDEFINED);
        ecma_array_set_length(e, p, len - 1);
        return val;
    }

    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_concat(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( p && evm_is_list(p) && argc >= 1 ){
        int len = evm_list_len(p);

        evm_val_t * o = evm_list_create(e, GC_LIST, len + argc);
        for(int i = 0; i < len; i++){
            evm_list_set(e, o, i, *evm_list_get(e, p, i));
        }
        for(int i = 0; i < argc; i++){
            evm_list_set(e, o, i + len, *(v + i));
        }
        ecma_array_attrs_apply(e, o);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_fill(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) && argc >= 1 ){
        int len = evm_list_len(p);
        int start = 0;
        int end = len;
        if (argc == 2 && evm_is_integer(v + 1)) start = evm_2_integer(v + 1);
        if (argc == 3 && evm_is_integer(v + 2)) end = evm_2_integer(v + 2);
        for(int i = start; i < end; i++){
            evm_list_set(e, p, i, *v);
        }
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_shift(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)argc;
    (void)v;
    if( evm_is_list(p) && evm_list_len(p) ){
        int len = ecma_array_get_length(e, p);
        evm_val_t val = * evm_list_get(e, p, 0);
        for(int i = 0; i < len - 1; i++){
            evm_list_set(e, p, i, *evm_list_get(e, p, i + 1));
        }
        ecma_array_set_length(e, p, len - 1);
        evm_list_set(e, p, len - 1, EVM_VAL_UNDEFINED);
        return val;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_slice(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) ){
        int len = evm_list_len(p);
        int begin = 0;
        int end = len;
        evm_val_t *o = NULL;

        if (argc >= 1 && evm_is_integer(v)) begin = evm_2_integer(v);
        if (begin < 0) begin = begin + len;
        if (argc == 2 && evm_is_integer(v + 1)) end = evm_2_integer(v + 1);
        if (end < 0) end = end + len;

        if (end < begin) len = 0;
        else len = end - begin;

        o = evm_list_create(e, GC_LIST, len);
        for(int i = 0; i < len; i++){
            evm_list_set(e, o, i, *evm_list_get(e, p, i + begin));
        }

        ecma_array_attrs_apply(e, o);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_copyWithin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) && argc > 0 ){
        int len = evm_list_len(p);
        int target = 0;
        int begin = 0;
        int end = len;

        if (argc >= 1 && evm_is_integer(v)) {
            target = evm_2_integer(v);

            if (target >= len) return EVM_VAL_UNDEFINED;
            if (argc >= 2 && evm_is_integer(v+1)) begin = evm_2_integer(v+1);
            if (begin < 0) begin = begin + len;
            if (argc == 3 && evm_is_integer(v+2)) end = evm_2_integer(v+2);
            if (end < 0) end = end + len;
            if (end < begin) return EVM_VAL_UNDEFINED;
            
            for (int i = begin; i < end; i++) {
                evm_list_set(e, p, target + (i - begin), *evm_list_get(e, p, i));
            }

            return *p;
        }
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_unshift(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) ){
        int len = ecma_array_get_length(e, p);
        int real_len = evm_list_len(p);
        if( real_len > 0 && len < real_len )
            evm_list_set(e, p, len, *v);
        else
            evm_list_push(e, p, argc, v);

        for(int i = len - 1; i > -1; i--){
            evm_list_set(e, p, i + argc, *evm_list_get(e, p, i));
        }
        for(int i = 0; i < argc; i++){
            evm_list_set(e, p, i, *(v + i));
        }

        ecma_array_set_length(e, p, len + argc);
        return evm_mk_number(len + argc);
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_join(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( evm_is_list(p) && evm_list_len(p) ){
        int c_len = 1;
        int s_len = 0;
        char * ch = ",";
        if (argc == 1 && evm_is_string(v)) {
            ch = (char*)evm_2_string(v);
            c_len = strlen(ch);
        }
        
        int len = evm_list_len(p);
        if (len == 1) return *evm_list_get(e, p, 0);

        evm_val_t *t = NULL;
        for (int i = 0; i < len; i++) {
            t = evm_list_get(e, p, i);
            if (evm_is_string(t)) {
                s_len = s_len + strlen(evm_2_string(t));
            } else {
                evm_val_t * toString_val = evm_attr_get(e, t, "toString");
                if( toString_val ) {
                    evm_val_t local_str_val = evm_run_callback(e, toString_val, t, NULL, 0);
                    if( evm_is_string(&local_str_val) ) {
                        char * local_addr = (char *)evm_2_string(&local_str_val);
                        int local_len = strlen(local_addr);
                        s_len += local_len;
                    }
                }
            }
        }

        evm_val_t *s = evm_heap_string_create(e, "", s_len + (c_len * (len - 1)));

        int current_idx = 0;
        
        for (int i = 0; i < len; i++) {
            t =  evm_list_get(e, p, i);
            if( evm_is_string(t) ){
                char * local_addr = (char *)evm_2_string(t);
                int local_len = strlen(local_addr);
                evm_heap_string_set(s, local_addr, current_idx, local_len);
                current_idx += local_len;
            } else {
                evm_val_t * toString_val = evm_attr_get(e, t, "toString");
                if( toString_val ) {
                    evm_val_t local_str_val = evm_run_callback(e, toString_val, t, NULL, 0);
                    if( evm_is_string(&local_str_val) ) {
                        char * local_addr = (char *)evm_2_string(&local_str_val);
                        int local_len = strlen(local_addr);
                        evm_heap_string_set(s, local_addr, current_idx, local_len);
                        current_idx += local_len;
                    }
                }
            }

            if (i < len - 1) {
                evm_heap_string_set(s, ch, current_idx, c_len);
                current_idx += c_len;
            }
        }
        return *s;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_filter(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if (evm_is_list(p) && argc > 0 && evm_is_script(v)) {
        evm_val_t *o = evm_list_create(e, GC_LIST, 0);
        int len = evm_list_len(p);
        int arr_len = 0;
        for (int i = 0; i < len; i++) {
            evm_val_t * arg = evm_list_get(e, p, i);
            if (evm_run_callback(e, v, p, arg, 1) == EVM_VAL_TRUE) {
                arr_len++;
                evm_list_push(e, o, 1, arg);
            }
        }
        ecma_array_attrs_apply(e, o);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_map(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if (evm_is_list(p) && argc > 0 && evm_is_script(v)) {
        int len = evm_list_len(p);
        evm_val_t *o = evm_list_create(e, GC_LIST, len);
        for (int i = 0; i < len; i++) {
            evm_val_t * arg = evm_list_get(e, p, i);
            evm_val_t val = evm_run_callback(e, v, p, arg, 1);
            evm_list_set(e, o, i, val);
        }
        ecma_array_attrs_apply(e, o);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_find(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if (evm_is_list(p) && argc > 0 && evm_is_script(v)) {
        int len = evm_list_len(p);
        evm_val_t * o = NULL;
        for (int i = 0; i < len; i++) {
            o = evm_list_get(e, p, i);
            if (evm_run_callback(e, v, p, o, 1) == EVM_VAL_TRUE) break;
        }
        if (evm_is_null(o)) return EVM_VAL_UNDEFINED;
        else return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_array_indexOf(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if(argc == 0) ARG_LENGTH_ERR;
    int len = evm_list_len(p);
    int fromIndex = 0;

    if(argc == 2 && evm_is_integer(v+1)){
        fromIndex = evm_2_integer(v+1);
    }

    if (fromIndex >= len){
        return evm_mk_number(-1);
    }

    for(int i = fromIndex; i < len; i++){
        evm_val_t * local_val = evm_list_get(e, p, i);
        if (evm_is_string(local_val) && strcmp(evm_2_string(local_val), evm_2_string(v)) == 0){
            return evm_mk_number(i);
        } else if (*evm_list_get(e, p, i) == *v){
            return evm_mk_number(i);
        }
    }
    return evm_mk_number(-1);
}

evm_val_t ecma_array(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    evm_val_t *o = NULL;
    if(argc == 0) {
        o = evm_list_create(e, GC_LIST, 0);
    } else if( argc == 1){
        if( evm_is_number(v) ) {
            o = evm_list_create(e, GC_LIST, evm_2_integer(v));
        } else {
            o = evm_list_create(e, GC_LIST, 1);
            evm_list_set(e, o, 0, *v);
        }
    } else {
        o = evm_list_create(e, GC_LIST, argc);
        for(int i = 0; i < argc; i++){
            evm_list_set(e, o, i, *(v + i));
        }
    }

    if( o != NULL){
        ecma_array_attrs_apply(e, o);
        return *o;
    } else {
        return EVM_VAL_UNDEFINED;
    }
}

evm_val_t * ecma_array_init(evm_t * e){
    evm_val_t * o = evm_native_function_create(e, (evm_native_fn)ecma_array, 1);
    evm_attr_set(e, o, 0, "prototype", *ecma_object_prototype);
    return o;
}
#endif
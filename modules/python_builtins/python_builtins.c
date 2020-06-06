/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version : 1.0
**  Email   : scriptiot@aliyun.com
**  Website : https://github.com/scriptiot
**  Licence: MIT Licence
****************************************************************************/
#include "python_builtins.h"

evm_val_t python_list(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    (void)argc;
    if( argc != 0 && argc != 1) ARG_LENGTH_ERR;
    if( argc == 0){
        return *evm_list_create(e, GC_LIST, 0);
    } else if( argc == 1){
        if( !evm_is_number(v) ) ARG_TYPE_ERR;
        return *evm_list_create(e, GC_LIST, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_bytearray(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    if( argc != 1 ) ARG_LENGTH_ERR;
    if( !evm_is_number(v) ) ARG_TYPE_ERR;
    int size = evm_2_integer(v);
    return *evm_buffer_create(e, size);
}

evm_val_t python_len(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR;
    if( evm_is_string(v) ) {
        return evm_mk_number(strlen(evm_2_string(v)));
    } else if( evm_is_buffer(v) ){
        return evm_mk_number(evm_buffer_len(v));
    } else if( evm_is_list(v) ){
        return evm_mk_number( evm_list_len(v) );
    } else {
        ARG_TYPE_ERR;
    }
}

evm_val_t python_int(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    int val = evm_2_integer(v);
    return evm_mk_number(val);
}

evm_val_t python_inner_tuple(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    evm_val_t * t = evm_list_create(e, GC_TUPLE, argc);
    for(int i = 0; i < argc; i++) evm_list_set(e, t, i, *(v + i));
    return *t;
}

evm_val_t python_inner_class(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    EVM_UNUSED(p);EVM_UNUSED(e);EVM_UNUSED(argc);
    evm_script_set_native(v, (evm_native_fn)evm_2_intptr(v + 1));
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_inner_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if( evm_is_class(p) ){
        evm_val_t * obj = evm_object_create_by_class(e, GC_OBJECT, p);
        evm_val_t * parent = evm_get_parent(e, *p);

        evm_val_t * c_obj = obj;
        while(parent){
            evm_val_t * local_obj = evm_object_create_by_class(e, GC_OBJECT, parent);
            evm_set_parent(c_obj, local_obj);
            c_obj = local_obj;
            parent = evm_get_parent(e, *parent);
        }
        evm_run_callback(e, p, obj, NULL, 0);
        evm_val_t * init_fn = evm_prop_get(e, obj, "__init__", 0);
        if( init_fn ){
            *(++e->sp) = *obj;
            evm_val_t * argv = e->sp;
            for(int i = 0; i < argc; i++) {
                *(++e->sp) = *(v + i);
            }
            evm_run_callback(e, init_fn, obj, argv, argc + 1);
        }
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_super(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    if( argc != 2) ARG_LENGTH_ERR;
    if( !evm_is_class(v) || !evm_is_object(v + 1)) ARG_TYPE_ERR;
    return *evm_get_parent(e, *(v + 1));
}

int python_builtins(evm_t * e){
    evm_builtin_t natives[] = {
        {".tuple", evm_mk_native( (intptr_t)python_inner_tuple )},
        {".class", evm_mk_native( (intptr_t)python_inner_class )},
        {".init", evm_mk_native( (intptr_t)python_inner_init )},
        {"list", evm_mk_native( (intptr_t)python_list )},
        {"bytearray", evm_mk_native( (intptr_t)python_bytearray )},
        {"len", evm_mk_native( (intptr_t)python_len )},
        {"int", evm_mk_native( (intptr_t)python_int )},
        {"super", evm_mk_native( (intptr_t)python_super )},
        {NULL, NULL}
    };
    return evm_native_add(e, natives);
}

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

#ifdef CONFIG_EVM_ECMA_STRING
#include "ecma.h"

evm_val_t * ecma_String;

evm_val_t ecma_string_charAt(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

extern void ecma_object_attrs_set(evm_t * e, evm_val_t * o, evm_val_t * prototype);
void ecma_string_attrs_apply(evm_t * e, evm_val_t * o, evm_val_t * prototype){

    if( evm_attr_create(e, o, 9) == ec_ok ){
        ecma_object_attrs_set(e, o, prototype);
        evm_attr_set(e, o, 1, "constructor", *ecma_String);
        evm_attr_set(e, o, 8, "charAt", evm_mk_native((intptr_t)ecma_string_charAt));
    }
}

evm_val_t ecma_string_charAt(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    if( argc == 1 && evm_is_integer(v) && evm_is_string(p) ){
        const char * s = evm_2_string(p);
        int index = evm_2_integer(v);
        int len = strlen(s);
        if( index >= len ) return EVM_VAL_UNDEFINED;
        evm_val_t *o = evm_heap_string_create(e, (char*)s, 1);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t ecma_string(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    evm_val_t *o = NULL;
    if(argc == 1 && evm_is_string(v) ) {
        const char * s = evm_2_string(v);
        o = evm_heap_string_create(e, (char*)s, strlen(s) + 1);
    }
    if( o != NULL ){
        ecma_string_attrs_apply(e, o, ecma_object_prototype);
        return *o;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t * ecma_string_init(evm_t * e){
    evm_val_t *o = evm_native_function_create(e, (evm_native_fn)ecma_string, ECMA_STRING_PROP_SIZE);
    evm_attr_set(e, o, 0, "prototype", *ecma_object_prototype);
    evm_attr_set(e, o, 1, "charAt", evm_mk_native((intptr_t)ecma_string_charAt));
    return o;
}
#endif

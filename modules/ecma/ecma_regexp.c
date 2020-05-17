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

#ifdef CONFIG_EVM_ECMA_REGEXP
#include "ecma.h"
#include "re.h"

evm_val_t * ecma_RegExp;

evm_val_t ecma_regex_test(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    if( argc != 1) ARG_LENGTH_ERR;
    if( !evm_2_string(v )) ARG_TYPE_ERR;
    re_t re = (re_t)evm_2_intptr( evm_attr_get_by_index(e, p, 0) );
    if( re == NULL) return evm_mk_undefined();
    if( re_matchp(re, evm_2_string(v)) == -1) return evm_mk_false();
    return evm_mk_true();
}

evm_val_t ecma_regex_exec(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    if( argc != 1) ARG_LENGTH_ERR;
    if( !evm_2_string(v )) ARG_TYPE_ERR;
    re_t re = (re_t)evm_2_intptr( evm_attr_get_by_index(e, p, 0) );
    if( re == NULL) return evm_mk_undefined();
    int index = re_matchp(re, evm_2_string(v));
    if( index == -1) return evm_mk_null();
    return evm_mk_number(index);
}

evm_val_t ecma_regex(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    if( argc != 1) ARG_LENGTH_ERR;
    if( !evm_2_string(v )) ARG_TYPE_ERR;
    evm_val_t * o = evm_object_create(e, GC_OBJECT, 2, 1);
    evm_prop_set(e, o, 0, "test", evm_mk_native((intptr_t)ecma_regex_test));
    evm_prop_set(e, o, 1, "exec", evm_mk_native((intptr_t)ecma_regex_exec));
    evm_attr_set(e, o, 0, ".re",  evm_mk_foreign(re_compile(evm_2_string(v))));

    return *o;
}

evm_val_t *ecma_regex_init(evm_t * e){
    evm_val_t * o = evm_native_function_create(e, (evm_native_fn)ecma_regex, 1);
    evm_attr_set(e, o, 0, "prototype", *ecma_object_prototype);
    return o;
}
#endif

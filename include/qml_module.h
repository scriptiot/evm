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
#ifndef QML_MOUDLE_H
#define QML_MOUDLE_H

#include "evm.h"

enum EVM_QML_TYPE{
    EVM_QML_INT = 1,
    EVM_QML_DOUBLE = 2,
    EVM_QML_BOOLEAN = 4,
    EVM_QML_STRING = 8,
    EVM_QML_CALLBACK = 16,
    EVM_QML_VALUE = 32,
    EVM_QML_ANY = 64,
    EVM_QML_GROUP = 128,
};

typedef struct evm_qml_value_reg_t{
    int type;
    char * name;
    void* api;
}evm_qml_value_reg_t;

typedef struct evm_qml_object_reg_t{
    char * name;
    char * parent_name;
    evm_native_fn api;
    evm_qml_value_reg_t * values;
}evm_qml_object_reg_t;

void evm_qml_object_set_pointer(evm_val_t * o, void * pointer);
void * evm_qml_object_get_pointer(evm_val_t * o);
char * evm_qml_object_get_name(evm_t* e, evm_val_t * o);
int evm_qml_register(evm_t * e, evm_qml_object_reg_t * regs);
int qml_module(evm_t * e, evm_native_fn init);
void qml_object_gc_init(evm_t * e, evm_val_t * old_self, evm_val_t *new_self);
void evm_qml_write_value(evm_t * e, evm_val_t * src_obj, char * name, evm_val_t v);

#endif

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

#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"
#include "nevm.h"
#include "evm_board.h"
#include "uol_output.h"

#define EVM_ARG_LENGTH_ERR {evm_set_err(e, ec_type, "Bad argument length"); return EVM_VAL_UNDEFINED;}
#define EVM_ARG_TYPE_ERR {evm_set_err(e, ec_type, "Bad argument type"); return EVM_VAL_UNDEFINED;}

int evm_module(evm_t * e);
void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, uint16_t constructor_api, uint16_t open_api);
int evm_module_add_callback(evm_val_t callback, evm_val_t args);
int evm_module_remove_callback(int id);
evm_val_t * evm_module_get_callback(int id);
extern evm_t * evm_runtime;

#endif

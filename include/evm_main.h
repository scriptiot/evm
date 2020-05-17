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

#ifndef EVM_MAIN_H
#define EVM_MAIN_H

#include "evm_module.h"
#include <zephyr.h>
#include <console/console.h>
#include <sys/printk.h>
#include <drivers/uart.h>

#define NEVM_HEAP_SIZE         (3 * 1024)
#define NEVM_STACK_SIZE        (1 * 1024)
#define NEVM_MODULE_SIZE       3    

#define EVM_HEAP_SIZE          (10 * 1024)
#define EVM_STACK_SIZE         (2 * 1024)
#define EVM_MODULE_SIZE        5 

int evm_main(void);

#endif

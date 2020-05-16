/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
**  Licence: Apache-2.0
****************************************************************************/

#include "evm_board.h"

const struct pin_group ledgroup[] = {
    {"GPIOC", 13, GPIO_OUTPUT},
    {NULL, 0, 0},
};

const struct pin_group keygroup[] = {
    {"GPIOB", 2, GPIO_INPUT | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW | GPIO_PUD_PULL_UP},
    {"GPIOB", 3, GPIO_INPUT | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW | GPIO_PUD_PULL_UP},
    {NULL, 0, 0},
};

int evm_board_get_pin_group_size(pin_group * group){
    int i = 0;
    while( group[i].port != NULL ){
        i++;
    }
    return i;
}
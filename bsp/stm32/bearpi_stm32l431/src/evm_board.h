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

#ifndef EVM_BOARD_H
#define EVM_BOARD_H

#include <drivers/gpio.h>
#include <drivers/uart.h>

typedef struct pin_group {
	char* port;
    u8_t  pin;
	u32_t mode;
}pin_group;

extern const pin_group ledgroup[];
extern const pin_group keygroup[];

int evm_board_get_pin_group_size(pin_group * group);

#define CONFIG_EVM_CALLBACK_SIZE              10

#define CONFIG_EVM_GPIO_INPUT                 GPIO_INPUT
#define CONFIG_EVM_GPIO_OUTPUT                GPIO_OUTPUT
#define CONFIG_EVM_GPIO_DISCONNECTED          GPIO_DISCONNECTED
#define CONFIG_EVM_GPIO_INT_ENABLE            GPIO_INT_ENABLE
#define CONFIG_EVM_GPIO_INT_DISABLE           GPIO_INT_DISABLE
#define CONFIG_EVM_GPIO_INT_EDGE_RISING       GPIO_INT_EDGE_RISING
#define CONFIG_EVM_GPIO_INT_EDGE_FALLING      GPIO_INT_EDGE_FALLING
#define CONFIG_EVM_GPIO_INT_EDGE_BOTH         GPIO_INT_EDGE_BOTH
#define CONFIG_EVM_GPIO_PUD_PULL_UP           GPIO_PUD_PULL_UP
#define CONFIG_EVM_GPIO_PUD_PULL_DOWN         GPIO_PUD_PULL_DOWN
#define CONFIG_EVM_GPIO_OPEN_SOURCE           GPIO_OPEN_SOURCE
#define CONFIG_EVM_GPIO_OPEN_DRAIN            GPIO_OPEN_DRAIN


#define CONFIG_EVM_UART_PARITY_NONE           UART_CFG_PARITY_NONE
#define CONFIG_EVM_UART_PARITY_ODD            UART_CFG_PARITY_ODD
#define CONFIG_EVM_UART_PARITY_EVEN           UART_CFG_PARITY_EVEN
#define CONFIG_EVM_UART_STOP_BITS_0_5     UART_CFG_STOP_BITS_0_5
#define CONFIG_EVM_UART_STOP_BITS_1       UART_CFG_STOP_BITS_1
#define CONFIG_EVM_UART_STOP_BITS_1_5     UART_CFG_STOP_BITS_1_5
#define CONFIG_EVM_UART_STOP_BITS_2       UART_CFG_STOP_BITS_2

#define CONFIG_EVM_UART_DATA_BITS_5       UART_CFG_DATA_BITS_5
#define CONFIG_EVM_UART_DATA_BITS_6       UART_CFG_DATA_BITS_6
#define CONFIG_EVM_UART_DATA_BITS_7       UART_CFG_DATA_BITS_7
#define CONFIG_EVM_UART_DATA_BITS_8       UART_CFG_DATA_BITS_8
#define CONFIG_EVM_UART_DATA_BITS_9       UART_CFG_DATA_BITS_9

#endif

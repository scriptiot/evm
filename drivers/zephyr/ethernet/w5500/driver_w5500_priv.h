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

#include <kernel.h>
#include <drivers/gpio.h>

#ifndef _DRIVER_W5500_H
#define _DRIVER_W5500_H

struct w5500_config {
	const char *gpio_port;
	u8_t gpio_pin;
	gpio_dt_flags_t gpio_flags;
	const char *spi_port;
	u8_t spi_cs_pin;
	const char *spi_cs_port;
	u32_t spi_freq;
	u8_t spi_slave;
	u8_t full_duplex;
	s32_t timeout;
};

struct w5500_runtime {
	struct net_if *iface;

	K_THREAD_STACK_MEMBER(thread_stack,
			      CONFIG_ETH_W5500_RX_THREAD_STACK_SIZE);

	struct k_thread thread;
	u8_t mac_address[6];
	struct device *gpio;
	struct device *spi;
	struct spi_cs_control spi_cs;
	struct spi_config spi_cfg;
	struct gpio_callback gpio_cb;
	struct k_sem tx_rx_sem;
	struct k_sem int_sem;
	bool suspended : 1;
	bool iface_initialized : 1;
};

#endif 

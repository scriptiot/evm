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

/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2016 BayLibre, SAS
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <device.h>
#include <init.h>
#include <drivers/pinmux.h>
#include <sys/sys_io.h>

#include <pinmux/stm32/pinmux_stm32.h>

/* pin assignments for NUCLEO-L432KC board */
static const struct pin_config pinconf[] = {
#if DT_NODE_HAS_STATUS(DT_NODELABEL(lpuart1), okay)
	{STM32_PIN_PC0,  STM32_PINMUX_ALT_FUNC_8 | STM32_PUPDR_NO_PULL},
	{STM32_PIN_PC1,  STM32_PINMUX_ALT_FUNC_8 | STM32_PUSHPULL_PULLUP},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(usart1), okay)
	{STM32_PIN_PA9,  STM32L4X_PINMUX_FUNC_PA9_USART1_TX},
	{STM32_PIN_PA10, STM32L4X_PINMUX_FUNC_PA10_USART1_RX},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(usart2), okay)
	{STM32_PIN_PA2, STM32L4X_PINMUX_FUNC_PA2_USART2_TX},
	{STM32_PIN_PA3, STM32L4X_PINMUX_FUNC_PA3_USART2_RX},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(usart3), okay)
	{STM32_PIN_PC4, STM32L4X_PINMUX_FUNC_PC4_USART3_TX},
	{STM32_PIN_PC5, STM32L4X_PINMUX_FUNC_PC5_USART3_TX},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(i2c1), okay)
	{STM32_PIN_PB6, STM32L4X_PINMUX_FUNC_PB6_I2C1_SCL},
	{STM32_PIN_PB7, STM32L4X_PINMUX_FUNC_PB7_I2C1_SDA},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(spi2), okay)
	{STM32_PIN_PB13, STM32_PINMUX_ALT_FUNC_5 |STM32_OSPEEDR_VERY_HIGH_SPEED},
	{STM32_PIN_PC2, STM32L4X_PINMUX_FUNC_PB4_SPI1_MISO},
	{STM32_PIN_PC3, STM32L4X_PINMUX_FUNC_PB5_SPI1_MOSI},
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(adc1), okay)
	{ STM32_PIN_PC2, STM32L4X_PINMUX_FUNC_PC2_ADC123_IN3 },
#endif	/* CONFIG_ADC_1 */
#if DT_NODE_HAS_STATUS(DT_NODELABEL(dac1), okay)
	{ STM32_PIN_PA5, STM32_MODER_ANALOG_MODE | STM32_PUPDR_NO_PULL },
#endif	/* CONFIG_ADC_1 */
#if DT_NODE_HAS_STATUS(DT_NODELABEL(sdmmc1), okay) && \
	CONFIG_DISK_ACCESS_STM32_SDMMC
	{STM32_PIN_PC8, STM32L4X_PINMUX_FUNC_PC8_SDMMC1_D0},
	// {STM32_PIN_PC9, STM32L4X_PINMUX_FUNC_PC9_SDMMC1_D1},
	// {STM32_PIN_PC10, STM32L4X_PINMUX_FUNC_PC10_SDMMC1_D2},
	// {STM32_PIN_PC11, STM32L4X_PINMUX_FUNC_PC11_SDMMC1_D3},
	{STM32_PIN_PC12, STM32L4X_PINMUX_FUNC_PC12_SDMMC1_CK},
	{STM32_PIN_PD2, STM32L4X_PINMUX_FUNC_PD2_SDMMC1_CMD},
#endif
};

static int pinmux_stm32_init(struct device *port)
{
	ARG_UNUSED(port);

	stm32_setup_pins(pinconf, ARRAY_SIZE(pinconf));

	return 0;
}

SYS_INIT(pinmux_stm32_init, PRE_KERNEL_1,
	 CONFIG_PINMUX_STM32_DEVICE_INITIALIZATION_PRIORITY);

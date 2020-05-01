/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// #include <zephyr.h>
// #include <sys/printk.h>
// #include <evm.h>
// #include <drivers/gpio.h>

// char evm_repl_tty_read(evm_t * e)
// {
//     EVM_UNUSED(e);
//     char c = console_getchar();
//     return c;
// }

// const char * vm_load(evm_t * e, char * path, int type){


// }


// void * vm_malloc(int size)
// {
//     void * m = malloc(size);
//     if(m) memset(m, 0 ,size);
//     return m;
// }

// void vm_free(void * mem)
// {
//     if(mem) free(mem);
// }


#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_HAS_NODE(LED0_NODE)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#if DT_PHA_HAS_CELL(LED0_NODE, gpios, flags)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#endif
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#endif

#ifndef FLAGS
#define FLAGS	0
#endif

const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

static int test_configure(void)
{
	struct device *uart_dev = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);

	if (!uart_dev) {
		return 0;
	}

	/* Verify configure() - set device configuration using data in cfg */
	int ret = uart_configure(uart_dev, &uart_cfg);

	/* 0 if successful, - error code otherwise */
	return 0;

}


void main(void)
{
    test_configure();
	struct device *dev;
	bool led_is_on = true;
	int ret;
    printk("start");
	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	while (1) {
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
	}
}



// void main(void)
// {
// 	//console_init();
//     printf("bearpi start\n");
//     struct device *dev;
// 	bool led_is_on = true;
// 	int ret;

// 	dev = device_get_binding("LED0");
// 	if (dev == NULL) {
//         printk("led0 not found");
// 		return;
// 	}

// 	ret = gpio_pin_configure(dev, 0, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		return;
// 	}

// 	while (1) {
// 		gpio_pin_set(dev, 0, (int)led_is_on);
// 		led_is_on = !led_is_on;
// 		k_msleep(1000);
// 	}

    

//     evm_register_free((intptr_t)vm_free);
//     evm_register_malloc((intptr_t)vm_malloc);
//     evm_register_print((intptr_t)printk);
//     evm_register_file_load((intptr_t)vm_load);

//     // 初始化虚拟机
//     int32_t head_size = 20 * 1024;
//     int32_t stack_size = 20 * 1024;
//     int32_t module_size = 5;
//     evm_t * env = (evm_t*)malloc(sizeof(evm_t));
//     memset(env, 0, sizeof(evm_t));
//     int err = evm_init(env, head_size, stack_size, module_size, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);


//     evm_repl_run(env, 1000, EVM_LANG_JS);
// }

#ifndef EVM_BOARD_H
#define EVM_BOARD_H

#include <drivers/gpio.h>

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

#endif

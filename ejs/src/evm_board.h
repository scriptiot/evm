#ifndef EVM_BOARD_H
#define EVM_BOARD_H

#include <drivers/gpio.h>

typedef struct led_group {
	char* port;
    u8_t  pin;
	u32_t mode;
}led_group;

const struct led_group ledgroup[] = {
    {"GPIOC", 13, GPIO_OUTPUT},
};

#endif

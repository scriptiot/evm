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

#endif

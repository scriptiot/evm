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
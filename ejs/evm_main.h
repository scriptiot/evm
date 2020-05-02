#ifndef EVM_MAIN_H
#define EVM_MAIN_H

#include "evm_module.h"
#include <zephyr.h>
#include <console/console.h>
#include <sys/printk.h>
#include <drivers/uart.h>

#define NEVM_HEAP_SIZE          (10 * 1000)
#define NEVM_STACK_SIZE         (2 * 1000)
#define NEVM_MODULE_SIZE        3    

#define EVM_HEAP_SIZE          (20 * 1000)
#define EVM_STACK_SIZE         (2 * 1000)
#define EVM_MODULE_SIZE        5 

int evm_main(void);

#endif
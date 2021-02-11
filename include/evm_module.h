#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"

#define EVM_HEAP_SIZE    (100 * 1024)
#define EVM_STACK_SIZE   (10 * 1024)

#ifdef CONFIG_EVM_MODULE_ADC
extern evm_err_t evm_module_adc(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_GPIO
extern evm_err_t evm_module_gpio(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_PWM
extern evm_err_t evm_module_pwm(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_SPI
extern evm_err_t evm_module_spi(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_TIMERS
extern evm_err_t evm_module_timers(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_I2C
extern evm_err_t evm_module_i2c(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_UART
extern evm_err_t evm_module_uart(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_FS
extern evm_err_t evm_module_fs(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_HTTP
extern evm_err_t evm_module_http(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_UDP
extern evm_err_t evm_module_udp(evm_t *e);
#endif

extern int evm_main(void);

#endif

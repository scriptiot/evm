#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"

#define EVM_GPIO_DIRECTION_IN   0
#define EVM_GPIO_DIRECTION_OUT  1
#define EVM_GPIO_MODE_NONE      0
#define EVM_GPIO_MODE_PULLUP    1
#define EVM_GPIO_MODE_PULLDOWN  2
#define EVM_GPIO_MODE_FLOAT     3
#define EVM_GPIO_MODE_PUSHPULL  4
#define EVM_GPIO_MODE_OPENDRAIN 5
#define EVM_GPIO_EDGE_NONE      0
#define EVM_GPIO_EDGE_RISING    1
#define EVM_GPIO_EDGE_FALLING   2
#define EVM_GPIO_EDGE_BOTH      3

#define EVM_HEAP_SIZE    (100 * 1024)
#define EVM_STACK_SIZE   (10 * 1024)
#define EVM_MODULE_REGISTRY_SIZE       100

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

#ifdef CONFIG_EVM_MODULE_NET
extern evm_err_t evm_module_net(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_PROCESS
extern evm_val_t evm_module_process_nextTick(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
extern void evm_module_process_poll(evm_t *e);
extern evm_err_t evm_module_process(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_EVENTS
extern evm_err_t evm_module_events(evm_t *e);
#endif


#ifdef CONFIG_EVM_MODULE_DNS
extern evm_err_t evm_module_dns(evm_t *e);
#endif

#ifdef CONFIG_EVM_MODULE_TIMERS
extern evm_err_t evm_module_timers(evm_t *e);
#endif

extern void evm_module_registry_init(evm_t *e, int size);
extern int evm_module_registry_add(evm_t *e, evm_val_t *v);
extern evm_val_t *evm_module_registry_get(evm_t *e, int id);
extern void evm_module_registry_remove(evm_t *e, int id);
extern void evm_module_next_tick(evm_t *e, int argc, evm_val_t *v);
extern evm_err_t evm_module_event_add_listener(evm_t *e, evm_val_t *pthis, const char *type, evm_val_t *listener);
extern void evm_module_event_remove_listener(evm_t *e, evm_val_t *pthis, const char *type);
extern void evm_module_event_emit (evm_t *e, evm_val_t *pthis, const char *type, int argc, evm_val_t *v);
extern int evm_main(void);

#endif

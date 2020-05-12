#ifndef NEVM_H
#define NEVM_H

#include "evm.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

typedef struct nevm_builtin_t{
    const char * name;
    evm_native_fn func;
}nevm_builtin_t;

void nevm_object_set_ext_data(evm_val_t * o, intptr_t ptr);
intptr_t nevm_object_get_ext_data(evm_val_t *o);

evm_val_t *nevm_object_create_by_class(evm_t *e, evm_val_t * cls);
void nevm_builtin_init(nevm_builtin_t * builtins);

extern evm_t * nevm_runtime;

/**** invokable interface for evm *****/
evm_val_t nevm_function_invoke(evm_t * ne, uint16_t export_addr, int argc, evm_val_t *args);
evm_val_t nevm_object_function_invoke(evm_t * ne, evm_val_t * obj, uint16_t export_addr, int argc, evm_val_t * args);
/**************************************/

#define NEVM_ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");return EVM_VAL_UNDEFINED; }
#define NEVM_ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");return EVM_VAL_UNDEFINED; }

#define NEVM_TRUE	evm_mk_number(1)
#define NEVM_FALSE	evm_mk_number(0)


/****** adc native api interface *****/
extern evm_val_t nevm_driver_adc_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_adc_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_adc_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_adc_get_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** can native api interface *****/
extern evm_val_t nevm_driver_can_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_start(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_stop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_request_sleep(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_wakeup(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_add_tx_message(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_can_get_rx_message(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** dac native api interface *****/
extern evm_val_t nevm_driver_dac_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_dac_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_dac_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_dac_set_value(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** flash native api interface *****/
extern evm_val_t nevm_driver_flash_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_read(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_write(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_erase(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_flash_write_block_size(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** gpio native api interface *****/
extern evm_val_t nevm_driver_gpio_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_gpio_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_gpio_write_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_gpio_read_pin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_gpio_callback(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** i2c native api interface *****/
extern evm_val_t nevm_driver_i2c_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_master_transmit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_slave_receive(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_mem_transmit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_mem_receive(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_seq_transmit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_i2c_seq_receive(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** pwm native api interface *****/
extern evm_val_t nevm_driver_pwm_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_pwm_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_pwm_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** rtc native api interface *****/
extern evm_val_t nevm_driver_rtc_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_get_time(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_get_date(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_set_time(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_set_date(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_set_alarm(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_get_alarm(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_rtc_deactivate_alarm(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** spi native api interface *****/
extern evm_val_t nevm_driver_spi_gpio_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_transmit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_receive(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_transmit_receive(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_spi_abort(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** timer native api interface *****/
extern evm_val_t nevm_driver_timer_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_timer_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_timer_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_timer_start(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_timer_stop(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** uart native api interface *****/
extern evm_val_t nevm_driver_uart_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_read_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_write_byte(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_read_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_uart_write_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
/****** watchdog native api interface *****/
extern evm_val_t nevm_driver_watchdog_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_install_timeout(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_setup(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_feed(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
extern evm_val_t nevm_driver_watchdog_disable(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
#endif


#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"
#include "nevm.h"

#define EVM_ARG_LENGTH_ERR {evm_set_err(e, ec_type, "Bad argument length"); return EVM_VAL_UNDEFINED;}
#define EVM_ARG_TYPE_ERR {evm_set_err(e, ec_type, "Bad argument type"); return EVM_VAL_UNDEFINED;}

evm_val_t evm_class_accel(evm_t * e);
evm_val_t evm_class_adc(evm_t * e);
evm_val_t evm_class_can(evm_t * e);
evm_val_t evm_class_dac(evm_t * e);
evm_val_t evm_class_extint(evm_t * e);
evm_val_t evm_class_flash(evm_t * e);
evm_val_t evm_class_i2c(evm_t * e);
evm_val_t evm_class_lcd(evm_t * e);
evm_val_t evm_class_led(evm_t * e);
evm_val_t evm_class_pin(evm_t * e);
evm_val_t evm_class_pinaf(evm_t * e);
evm_val_t evm_class_rtc(evm_t * e);
evm_val_t evm_class_servo(evm_t * e);
evm_val_t evm_class_spi(evm_t * e);
evm_val_t evm_class_switch(evm_t * e);
evm_val_t evm_class_timer(evm_t * e);
evm_val_t evm_class_timerchannel(evm_t * e);
evm_val_t evm_class_uart(evm_t * e);
evm_val_t evm_class_usbhid(evm_t * e);
evm_val_t evm_class_usbvcp(evm_t * e);


int evm_module(evm_t * e);

void evm_module_construct(evm_t* e, evm_val_t * p, evm_val_t * v, const char * api_name);

#endif


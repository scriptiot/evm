#include "evm_module.h"

void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, const char * api_name){
    evm_val_t dev = nevm_function_invoke(e, api_name, argc, v);
    nevm_object_function_invoke(e, &dev, "open", 0, NULL);
    nevm_object_set_ext_data(p, evm_2_intptr(&dev) );
}

int evm_module(evm_t * e){
	evm_builtin_t module[] = {
		{"Pin", evm_class_pin(e)},
		{"LCD", evm_class_lcd(e)},
		{"LED", evm_class_led(e)},
		{"Flash", evm_class_flash(e)},
		{"ADC", evm_class_adc(e)},
		{"DAC", evm_class_dac(e)},
		{"RTC", evm_class_rtc(e)},
		{"Switch", evm_class_switch(e)},
		{"Watchdog", evm_class_watchdog(e)},
        {NULL, NULL}
    };
    evm_module_create(e, "evm", module);
	return e->err;
}


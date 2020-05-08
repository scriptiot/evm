#include "evm_module.h"

#ifdef EVM_DRIVER_GPIO
	extern evm_val_t evm_class_pin(evm_t * e);
#endif
#ifdef EVM_DRIVER_LCD
	extern evm_val_t evm_class_lcd(evm_t * e);
#endif
#ifdef EVM_DRIVER_LED
	extern evm_val_t evm_class_led(evm_t * e);
#endif
#ifdef EVM_DRIVER_FLASH
	extern evm_val_t evm_class_flash(evm_t * e);
#endif
#ifdef EVM_DRIVER_ADC
	extern evm_val_t evm_class_adc(evm_t * e);
#endif
#ifdef EVM_DRIVER_DAC
	extern evm_val_t evm_class_dac(evm_t * e);
#endif
#ifdef EVM_DRIVER_RTC
	extern evm_val_t evm_class_rtc(evm_t * e);
#endif
#ifdef EVM_DRIVER_SWITCH
	extern evm_val_t evm_class_switch(evm_t * e);
#endif
#ifdef EVM_DRIVER_WATCHDOG

#endif

void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, const char * api_name){
    evm_val_t dev = nevm_function_invoke(e, api_name, argc, v);
    nevm_object_function_invoke(e, &dev, "open", 0, NULL);
    nevm_object_set_ext_data(p, evm_2_intptr(&dev) );
}

int evm_module(evm_t * e){
	evm_builtin_t module[] = {
#ifdef EVM_DRIVER_GPIO
		{"Pin", evm_class_pin(e)},
#endif
#ifdef EVM_DRIVER_LCD
		{"LCD", evm_class_lcd(e)},
#endif
#ifdef EVM_DRIVER_LED
		{"LED", evm_class_led(e)},
#endif
#ifdef EVM_DRIVER_FLASH
		{"Flash", evm_class_flash(e)},
#endif
#ifdef EVM_DRIVER_ADC
		{"ADC", evm_class_adc(e)},
#endif
#ifdef EVM_DRIVER_DAC
		{"DAC", evm_class_dac(e)},
#endif
#ifdef EVM_DRIVER_RTC
		{"RTC", evm_class_rtc(e)},
#endif
#ifdef EVM_DRIVER_SWITCH
		{"Switch", evm_class_switch(e)},
#endif
#ifdef EVM_DRIVER_WATCHDOG
		{"Watchdog", evm_class_watchdog(e)},
#endif
        {NULL, NULL}
    };
    evm_module_create(e, "evm", module);
	return e->err;
}


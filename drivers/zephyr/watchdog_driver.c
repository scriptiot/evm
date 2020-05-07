#include "nevm.h"

#ifdef EVM_DRIVER_WATCHDOG
#include <device.h>
#include <drivers/watchdog.h>
#endif

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "invalid argument length");return evm_mk_undefined(); }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "invalid argument type");return evm_mk_undefined(); }

evm_val_t nevm_driver_watchdog_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_watchdog_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
   (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_WATCHDOG
    if( argc > 0 && evm_is_string(v) ){
		struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ) return NEVM_FALSE;
        nevm_object_set_ext_data(p, (intptr_t)dev);
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_watchdog_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_watchdog_install_timeout(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
   (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_WATCHDOG
    if( argc >= 2 && evm_is_number(v) && evm_is_number(v+1) ){
        int min = evm_2_integer(v);
        int max = evm_2_integer(v);
        if (min < max){
		    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
            if( !dev ) return NEVM_FALSE;

            struct wdt_timeout_cfg wdt_config;
            /* Reset SoC when watchdog timer expires. */
            wdt_config.flags = WDT_FLAG_RESET_SOC;

            /* Expire watchdog after 1000 milliseconds. */
            wdt_config.window.min = min;
            wdt_config.window.max = max;

            /* Set up watchdog callback. Jump into it when watchdog expired. */
            wdt_config.callback = NULL;

            int wdt_channel_id = wdt_install_timeout(dev, &wdt_config);
            if (wdt_channel_id < 0) {
                return NEVM_FALSE;
            }

            return evm_mk_number(wdt_channel_id);
        }else{
            // evm_set_err(e, ec_type, "min < max");
            return NEVM_FALSE;
        }
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_watchdog_setup(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_WATCHDOG
    if( argc >= 1 && evm_is_number(v)){
        struct device * dev = (struct device *)nevm_object_get_ext_data(p);
        if( !dev ) return NEVM_FALSE;
        int err = wdt_setup(dev, evm_2_integer(v));
        if (err){
            return NEVM_FALSE;
        }
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_watchdog_feed(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_WATCHDOG
    if( argc >= 1 && evm_is_number(v)){
        struct device * dev = (struct device *)nevm_object_get_ext_data(p);
        if( !dev ) return NEVM_FALSE;
        int err = wdt_feed(dev, evm_2_integer(v));
        if (err){
            return NEVM_FALSE;
        }
        return NEVM_TRUE;
    }
#endif
    return NEVM_FALSE;
}

evm_val_t nevm_driver_watchdog_disable(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;(void)p;(void)argc;(void)v;
#ifdef EVM_DRIVER_WATCHDOG
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( !dev ) return NEVM_FALSE;
    int err = wdt_disable(dev);
    if (err){
        return NEVM_FALSE;
    }
    return NEVM_TRUE;
#endif
    return NEVM_FALSE;
}

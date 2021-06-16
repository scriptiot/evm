#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_timer.h"
#include "luat_gpio.h"

#define DBG_TAG           "luat.gpio"
#define DBG_LVL           DBG_INFO
// void luat_gpio_mode(int pin, int mode) {
//     rt_pin_mode(pin, mode);
// }

static void luat_gpio_irq_callback(void* ptr) {
    //LOG_D("IRQ Callback");
    int pin = (int)ptr;
    int value = 1;
    rtos_msg_t msg;
    msg.handler = l_gpio_handler;
    msg.ptr = NULL;
    msg.arg1 = pin;
    msg.arg2 = value;
    luat_msgbus_put(&msg, 1);
}

int luat_gpio_setup(luat_gpio_t* gpio) {
    int pin = 21;
    luat_gpio_irq_callback(pin);
    return 0;
}

int luat_gpio_set(int pin, int level) {
    evm_print("Pin set pin=%d level=%d\n", pin, level);
    return 0;
}

int luat_gpio_get(int pin) {
    evm_print("Pin get pin=%d value=%d\n", pin, 1);
    return 1;
}

void luat_gpio_close(int pin) {
    evm_print("Pin Close, pin=%d\n", pin);
}

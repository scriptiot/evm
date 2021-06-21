#include "luat_base.h"
#include "luat_malloc.h"
#include "luat_msgbus.h"
#include "luat_timer.h"
#include "luat_gpio.h"
#include "bl_gpio.h"

#define DBG_TAG           "luat.gpio"
#define DBG_LVL           DBG_INFO

static void luat_gpio_irq_callback(void* ptr) {

}

int luat_gpio_setup(luat_gpio_t* gpio) {
    int ionum = -1, inputmode = -1, pullup = 0, pulldown = 0;
    switch (gpio->mode)
    {
    case Luat_GPIO_OUTPUT:
        inputmode = 0;
        break;
    case Luat_GPIO_INPUT:
    case Luat_GPIO_IRQ:
        {
            switch (gpio->pull)
            {
            case Luat_GPIO_PULLUP:
                inputmode = 1;
                pullup = 1;
                break;
            case Luat_GPIO_PULLDOWN:
                inputmode = 1;
                pulldown = 1;
                break;
            
            case Luat_GPIO_DEFAULT:
            default:
                inputmode = 1;
                break;
            }
        }
        break;
    default:
        inputmode = 1;
        break;
    }
    if (inputmode) {
        bl_gpio_enable_input(gpio->pin, pullup ? 1 : 0, pulldown ? 1 : 0);
    } else {
        bl_gpio_enable_output(gpio->pin, pullup ? 1 : 0, pulldown ? 1 : 0);
    }
    return 0;
}

int luat_gpio_set(int pin, int level) {
    bl_gpio_output_set(pin, level ? 1 : 0);
    return 0;
}

int luat_gpio_get(int pin) {
    int ret;
    uint8_t val;
    ret = bl_gpio_input_get(pin, &val);
    return val;
}

void luat_gpio_close(int pin) {

}

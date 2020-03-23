#ifndef LVGL_MAIN_H
#define LVGL_MAIN_H

#include "evm.h"
#include "lvgl/lvgl.h"
#include "lv_ex_conf.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./FATFS/ff.h"
#include "delay/delay.h"

void lvgl_main(void);
void lvgl_loop(evm_t * env);

#endif


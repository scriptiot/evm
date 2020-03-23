#ifndef LVGL_MAIN_H
#define LVGL_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
#include "evm.h"
void evm_touchpad(int x, int y, int touch);
void * evm_get_fb();
void evm_lvgl_tick_inc(int x);
void lvgl_main();
void lvgl_loop(evm_t * env);
#ifdef __cplusplus
}
#endif
#endif // LVGL_MAIN_H

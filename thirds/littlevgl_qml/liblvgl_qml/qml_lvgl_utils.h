#ifndef QML_LVGL_UTILS_H
#define QML_LVGL_UTILS_H

#include "lvgl.h"
#include "qml_module.h"
void lvgl_qml_obj_add_style(lv_obj_t * obj);
lv_color_t lvgl_qml_style_get_color(evm_t * e, evm_val_t * v);
lv_layout_t lvgl_qml_get_layout(evm_t * e, evm_val_t * v);
lv_fit_t lvgl_qml_get_fit(evm_t * e, evm_val_t * v);
lv_label_align_t lvgl_qml_ta_get_text_align(evm_t * e, evm_val_t * v);
char * lvgl_qml_get_symbol(evm_t * e, evm_val_t * v);
lv_chart_type_t lvgl_qml_get_ChartType(evm_t * e, evm_val_t * v);

#endif

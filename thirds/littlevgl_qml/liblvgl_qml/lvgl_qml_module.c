/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version : 1.0
**  Email   : scriptiot@aliyun.com
**  Website : https://github.com/scriptiot
**  Licence: MIT Licence
****************************************************************************/
#include "lvgl.h"
#include "evm.h"
#include "qml_module.h"

evm_t * global_e;

lv_color_t lvgl_qml_style_get_color(evm_t * e, evm_val_t * v){
    if( !v ) return LV_COLOR_BLACK;
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "white") ) return LV_COLOR_WHITE;
        if( !strcmp(s, "silver") ) return LV_COLOR_SILVER;
        if( !strcmp(s, "gray") ) return LV_COLOR_GRAY;
        if( !strcmp(s, "black") ) return LV_COLOR_BLACK;
        if( !strcmp(s, "red") ) return LV_COLOR_RED;
        if( !strcmp(s, "maroon") ) return LV_COLOR_MAROON;
        if( !strcmp(s, "yellow") ) return LV_COLOR_YELLOW;
        if( !strcmp(s, "olive") ) return LV_COLOR_OLIVE;
        if( !strcmp(s, "lime") ) return LV_COLOR_LIME;
        if( !strcmp(s, "green") ) return LV_COLOR_GREEN;
        if( !strcmp(s, "cyan") ) return LV_COLOR_CYAN;
        if( !strcmp(s, "aqua") ) return LV_COLOR_AQUA;
        if( !strcmp(s, "teal") ) return LV_COLOR_TEAL;
        if( !strcmp(s, "blue") ) return LV_COLOR_BLUE;
        if( !strcmp(s, "green") ) return LV_COLOR_GREEN;
        if( !strcmp(s, "navy") ) return LV_COLOR_NAVY;
        if( !strcmp(s, "magenta") ) return LV_COLOR_MAGENTA;
        if( !strcmp(s, "purple") ) return LV_COLOR_PURPLE;
        if( !strcmp(s, "orange") ) return LV_COLOR_ORANGE;
        if( !strcmp(s, "darkblue") ) return LV_COLOR_MAKE(0, 51, 102);
        if( !strcmp(s, "lightblue") ) return LV_COLOR_MAKE(46, 203, 203);
    } else if( evm_is_number(v) ){
        int i = evm_2_integer(v);
        return LV_COLOR_MAKE((i >>16) & 0xFF, (i >>8) & 0xFF, i & 0xFF);
    }
    return LV_COLOR_BLACK;
}

lv_layout_t lvgl_qml_get_layout(evm_t * e, evm_val_t * v){
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "off") ) return LV_LAYOUT_OFF;
        if( !strcmp(s, "center") ) return LV_LAYOUT_CENTER;
        if( !strcmp(s, "hLeft") ) return LV_LAYOUT_COL_L;
        if( !strcmp(s, "vCenter") ) return LV_LAYOUT_COL_M;
        if( !strcmp(s, "hRight") ) return LV_LAYOUT_COL_R;
        if( !strcmp(s, "top") ) return LV_LAYOUT_ROW_T;
        if( !strcmp(s, "hCenter") ) return LV_LAYOUT_ROW_M;
        if( !strcmp(s, "bottom") ) return LV_LAYOUT_ROW_B;
        if( !strcmp(s, "pretty") ) return LV_LAYOUT_PRETTY;
        if( !strcmp(s, "grid") ) return LV_LAYOUT_GRID;
    }
    return LV_LAYOUT_OFF;
}

lv_fit_t lvgl_qml_get_fit(evm_t * e, evm_val_t * v){
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "none") ) return LV_FIT_NONE;
        if( !strcmp(s, "tight") ) return LV_FIT_TIGHT;
        if( !strcmp(s, "flood") ) return LV_FIT_FLOOD;
        if( !strcmp(s, "fill") ) return LV_FIT_FILL;
    }
    return LV_FIT_NONE;
}

lv_label_align_t lvgl_qml_ta_get_text_align(evm_t * e, evm_val_t * v){
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "left") ) return LV_LABEL_ALIGN_LEFT;
        if( !strcmp(s, "right") ) return LV_LABEL_ALIGN_RIGHT;
        if( !strcmp(s, "center") ) return LV_LABEL_ALIGN_CENTER;
        if( !strcmp(s, "auto") ) return LV_LABEL_ALIGN_AUTO;
    }
    return LV_LABEL_ALIGN_LEFT;
}

char * lvgl_qml_get_symbol(evm_t * e, evm_val_t * v){
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "audio") ) return LV_SYMBOL_AUDIO;
        if( !strcmp(s, "video") ) return LV_SYMBOL_VIDEO;
        if( !strcmp(s, "list") ) return LV_SYMBOL_LIST;
        if( !strcmp(s, "ok") ) return LV_SYMBOL_OK;
        if( !strcmp(s, "close") ) return LV_SYMBOL_CLOSE;
        if( !strcmp(s, "power") ) return LV_SYMBOL_POWER;
        if( !strcmp(s, "settings") ) return LV_SYMBOL_SETTINGS;
        if( !strcmp(s, "home") ) return LV_SYMBOL_HOME;
        if( !strcmp(s, "download") ) return LV_SYMBOL_DOWNLOAD;
        if( !strcmp(s, "drive") ) return LV_SYMBOL_DRIVE;
        if( !strcmp(s, "refresh") ) return LV_SYMBOL_REFRESH;
        if( !strcmp(s, "mute") ) return LV_SYMBOL_MUTE;
        if( !strcmp(s, "volume_mid") ) return LV_SYMBOL_VOLUME_MID;
        if( !strcmp(s, "volume_max") ) return LV_SYMBOL_VOLUME_MAX;
        if( !strcmp(s, "image") ) return LV_SYMBOL_IMAGE;
        if( !strcmp(s, "edit") ) return LV_SYMBOL_EDIT;
        if( !strcmp(s, "prev") ) return LV_SYMBOL_PREV;
        if( !strcmp(s, "play") ) return LV_SYMBOL_PLAY;
        if( !strcmp(s, "pause") ) return LV_SYMBOL_PAUSE;
        if( !strcmp(s, "stop") ) return LV_SYMBOL_STOP;
        if( !strcmp(s, "next") ) return LV_SYMBOL_NEXT;
        if( !strcmp(s, "eject") ) return LV_SYMBOL_EJECT;
        if( !strcmp(s, "left") ) return LV_SYMBOL_LEFT;
        if( !strcmp(s, "right") ) return LV_SYMBOL_RIGHT;
        if( !strcmp(s, "plus") ) return LV_SYMBOL_PLUS;
        if( !strcmp(s, "minus") ) return LV_SYMBOL_MINUS;
        if( !strcmp(s, "eye_open") ) return LV_SYMBOL_EYE_OPEN;
        if( !strcmp(s, "eye_close") ) return LV_SYMBOL_EYE_CLOSE;
        if( !strcmp(s, "warning") ) return LV_SYMBOL_WARNING;
        if( !strcmp(s, "shuffle") ) return LV_SYMBOL_SHUFFLE;
        if( !strcmp(s, "up") ) return LV_SYMBOL_UP;
        if( !strcmp(s, "down") ) return LV_SYMBOL_DOWN;
        if( !strcmp(s, "loop") ) return LV_SYMBOL_LOOP;
        if( !strcmp(s, "directory") ) return LV_SYMBOL_DIRECTORY;
        if( !strcmp(s, "upload") ) return LV_SYMBOL_UPLOAD;
        if( !strcmp(s, "call") ) return LV_SYMBOL_CALL;
        if( !strcmp(s, "cut") ) return LV_SYMBOL_CUT;
        if( !strcmp(s, "copy") ) return LV_SYMBOL_COPY;
        if( !strcmp(s, "save") ) return LV_SYMBOL_SAVE;
        if( !strcmp(s, "charge") ) return LV_SYMBOL_CHARGE;
        if( !strcmp(s, "paste") ) return LV_SYMBOL_PASTE;
        if( !strcmp(s, "bell") ) return LV_SYMBOL_BELL;
        if( !strcmp(s, "keyboard") ) return LV_SYMBOL_KEYBOARD;
        if( !strcmp(s, "gps") ) return LV_SYMBOL_GPS;
        if( !strcmp(s, "file") ) return LV_SYMBOL_FILE;
        if( !strcmp(s, "wifi") ) return LV_SYMBOL_WIFI;
        if( !strcmp(s, "battery_full") ) return LV_SYMBOL_BATTERY_FULL;
        if( !strcmp(s, "battery_3") ) return LV_SYMBOL_BATTERY_3;
        if( !strcmp(s, "battery_2") ) return LV_SYMBOL_BATTERY_2;
        if( !strcmp(s, "battery_1") ) return LV_SYMBOL_BATTERY_1;
        if( !strcmp(s, "battery_empty") ) return LV_SYMBOL_BATTERY_EMPTY;
        if( !strcmp(s, "usb") ) return LV_SYMBOL_USB;
        if( !strcmp(s, "bluetooth") ) return LV_SYMBOL_BLUETOOTH;
        if( !strcmp(s, "trash") ) return LV_SYMBOL_TRASH;
        if( !strcmp(s, "backspace") ) return LV_SYMBOL_BACKSPACE;
        if( !strcmp(s, "sd_card") ) return LV_SYMBOL_SD_CARD;
        if( !strcmp(s, "new_line") ) return LV_SYMBOL_NEW_LINE;
    }
    return  LV_SYMBOL_DUMMY;
}

lv_chart_type_t lvgl_qml_get_ChartType(evm_t * e, evm_val_t * v){
    if( evm_is_string(v)) {
        char * s = evm_2_string(v);
        if( !strcmp(s, "None") ) return LV_CHART_TYPE_NONE;
        if( !strcmp(s, "Line") ) return LV_CHART_TYPE_LINE;
        if( !strcmp(s, "Bar") ) return LV_CHART_TYPE_COLUMN;
        if( !strcmp(s, "Point") ) return LV_CHART_TYPE_POINT;
        if( !strcmp(s, "VLine") ) return LV_CHART_TYPE_VERTICAL_LINE;
        if( !strcmp(s, "Area") ) return LV_CHART_TYPE_AREA;
    }
    return LV_CHART_TYPE_LINE;
}

lv_chart_series_t * lv_chart_addseries(lv_obj_t * chart)
{
    lv_chart_ext_t * ext    = lv_obj_get_ext_attr(chart);
    lv_chart_series_t * ser = lv_ll_ins_head(&ext->series_ll);
    LV_ASSERT_MEM(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_CHART_POINT_DEF;

    if(ser == NULL) return NULL;

    ser->points = lv_mem_alloc(sizeof(lv_coord_t) * ext->point_cnt);
    LV_ASSERT_MEM(ser->points);
    if(ser->points == NULL) {
        lv_ll_rem(&ext->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;

    uint16_t i;
    lv_coord_t * p_tmp = ser->points;
    for(i = 0; i < ext->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    ext->series.num++;

    return ser;
}


static lvgl_qml_run_callback(evm_t * e, evm_val_t * obj, char * name, int argc, evm_val_t * argv){
    evm_val_t * m_clicked_fn = evm_prop_get(e, obj, name, 0);
    if( m_clicked_fn != NULL)
        evm_run_callback(e, m_clicked_fn, obj, argv, argc);
}

static void lvgl_qml_Button_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onClicked", 0, NULL);
    }else if(event == LV_EVENT_PRESSED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onPressed", 0, NULL);
    }else if(event == LV_EVENT_RELEASED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onReleased", 0, NULL);
    }
}

static void lvgl_qml_Slider_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        if (global_e){
            evm_qml_write_value(global_e, &v, "value", evm_mk_number(lv_slider_get_value(obj)));
            lvgl_qml_run_callback(global_e, &v, "onValueChanged", 0, NULL);
        }
    }
}

static void lvgl_qml_TextArea_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        evm_qml_write_value(global_e, &v, "text", evm_mk_foreign_string(lv_ta_get_text(obj)));
        lvgl_qml_run_callback(global_e, &v, "onValueChanged", 0, NULL);
    } else if(event == LV_EVENT_FOCUSED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onFocused", 0, NULL);
    } else if(event == LV_EVENT_DEFOCUSED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onDefocused", 0, NULL);
    } else if(event == LV_EVENT_CLICKED) {
        evm_val_t v = (evm_val_t)lv_obj_get_user_data(obj);
        lvgl_qml_run_callback(global_e, &v, "onClicked", 0, NULL);
    }
}

void lvgl_qml_gc_init(evm_t * e, evm_val_t *old_self, evm_val_t * new_self){
    lv_obj_t * o = (lv_obj_t*)evm_qml_object_get_pointer(old_self);
    lv_obj_set_user_data(o, *new_self);
    qml_object_gc_init(e, old_self, new_self);
}

static void lvgl_qml_kb_event_cb(lv_obj_t * event_kb, lv_event_t event)
{
    lv_kb_def_event_cb(event_kb, event);
}

static void lvgl_qml_object_add_style(lv_obj_t *obj){
    lv_style_t * style = evm_malloc(sizeof(lv_style_t));
    lv_style_copy(style, lv_obj_get_style(obj));
    lv_obj_set_style(obj, style);
}

static void lvgl_qml_Button_add_style(lv_obj_t *obj, int type, lv_style_t * param_style){
    lv_style_t * style = evm_malloc(sizeof(lv_style_t));
    lv_style_copy(style, param_style);
    lv_btn_set_style(obj, type, style);
}

static evm_val_t lvgl_qml_object_create(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    lv_obj_t * obj = NULL;
    lv_obj_t * parent = lv_scr_act();
    if(argc == 1){
        parent = evm_qml_object_get_pointer(v);
    }
    char * obj_name = evm_qml_object_get_name(e, p);
    if (!strcmp(obj_name, "Rectangle")){
        obj = (lv_obj_t*)lv_obj_create(parent, NULL);
        lv_obj_set_event_cb(obj, lvgl_qml_Button_event_handler);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_obj_set_user_data(obj, *p);
    }else if (!strcmp(obj_name, "Circle")){
        obj = (lv_obj_t*)lv_arc_create(parent, NULL);
    }else if (!strcmp(obj_name, "ProgressBar")){
        obj = (lv_obj_t*)lv_bar_create(parent, NULL);
    } else if (!strcmp(obj_name, "Chart")){
        obj = (lv_obj_t*)lv_chart_create(parent, NULL);
    } else if (!strcmp(obj_name, "ChartSeries")){
        if (!strcmp(evm_qml_object_get_name(e, v), "Chart")){
            obj = lv_chart_addseries(parent);
        }
    }else if (!strcmp(obj_name, "Button")){
        obj = (lv_obj_t*)lv_btn_create(parent, NULL);
        lv_obj_set_event_cb(obj, lvgl_qml_Button_event_handler);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_obj_set_user_data(obj, *p);
    } else if (!strcmp(obj_name, "ImageButton")){
        obj = (lv_obj_t*)lv_imgbtn_create(parent, NULL);
        lv_imgbtn_set_toggle(obj, false);
        lv_obj_set_event_cb(obj, lvgl_qml_Button_event_handler);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_obj_set_user_data(obj, *p);
    } else if (!strcmp(obj_name, "Text")){
        obj = (lv_obj_t*)lv_label_create(parent, NULL);
    } else if (!strcmp(obj_name, "Gauge")){
        obj = (lv_obj_t*)lv_gauge_create(parent, NULL);
    } else if (!strcmp(obj_name, "GaugeValue")){
        evm_qml_object_set_pointer(p, parent);
    } else if (!strcmp(obj_name, "ListView")){
        obj = (lv_obj_t*)lv_list_create(parent, NULL);
    } else if (!strcmp(obj_name, "Roller")){
        obj = (lv_obj_t*)lv_roller_create(parent, NULL);
    } else if (!strcmp(obj_name, "TileView")){
        obj = lv_tileview_create(parent, NULL);
        lv_obj_set_size(obj, lv_obj_get_height(parent), lv_obj_get_height(parent));
        lv_tileview_set_edge_flash(obj, true);
    } else if (!strcmp(obj_name, "Tile")){
        obj = lv_obj_create(parent, NULL);
        lv_obj_set_size(obj, lv_obj_get_height(parent), lv_obj_get_height(parent));
        lv_tileview_add_element(parent, obj);
    }  else if (!strcmp(obj_name, "TabView")){
        obj = lv_tabview_create(parent, NULL);
        lv_obj_set_size(obj, lv_obj_get_height(parent), lv_obj_get_height(parent));
    } else if (!strcmp(obj_name, "Tab")){
        if (!strcmp(evm_qml_object_get_name(e, v), "TabView")){
            obj = lv_tabview_add_tab(parent, "123");
            lv_obj_set_user_data(obj, parent);
        }
    } else if (!strcmp(obj_name, "Slider")){
        obj = (lv_obj_t*)lv_slider_create(parent, NULL);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_obj_set_event_cb(obj, lvgl_qml_Slider_event_handler);
    } else if (!strcmp(obj_name, "ScrollArea")){
        obj = (lv_obj_t*)lv_page_create(parent, NULL);
        lv_page_set_scrl_layout(obj, LV_LAYOUT_PRETTY);
        lv_page_set_style(obj, LV_PAGE_STYLE_BG, &lv_style_transp_tight);
        lv_page_set_style(obj, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
    } else if (!strcmp(obj_name, "MessageBox")){
        obj = (lv_obj_t*)lv_mbox_create(parent, NULL);
    } else if (!strcmp(obj_name, "TextArea")){
        obj = (lv_obj_t*)lv_ta_create(parent, NULL);
        lv_obj_set_user_data(obj, *p);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_ta_set_cursor_type(obj, LV_CURSOR_LINE | LV_CURSOR_HIDDEN);
        lv_obj_set_event_cb(obj, lvgl_qml_TextArea_event_handler);
    } else if(!strcmp(obj_name, "Keyboard")){
        obj = lv_kb_create(lv_scr_act(), NULL);
        lv_obj_set_event_cb(obj, lvgl_qml_kb_event_cb);
        lv_kb_set_cursor_manage(obj, true);
    } else if(!strcmp(obj_name, "Keyboard")){
        obj = lv_img_create(parent, NULL);
    } else if (!strcmp(obj_name, "Image")){
        obj = (lv_obj_t*)lv_img_create(parent, NULL);
    } else if (!strcmp(obj_name, "CheckBox")){
        obj = (lv_obj_t*)lv_cb_create(parent, NULL);
    } else if (!strcmp(obj_name, "Style")){
        lv_style_t * style = evm_malloc(sizeof(lv_style_t));
        lv_style_copy(style, lv_obj_get_style(parent));
        evm_qml_object_set_pointer(p, style);
    } else if (!strcmp(obj_name, "Container")){
        obj = (lv_obj_t*)lv_cont_create(parent, NULL);
        lv_obj_set_event_cb(obj, lvgl_qml_Button_event_handler);
        evm_object_set_init(p, (evm_init_fn)lvgl_qml_gc_init);
        lv_obj_set_user_data(obj, *p);
    } else if (!strcmp(obj_name, "StylePlain")){
        evm_qml_object_set_pointer(p, &lv_style_plain);
    } else if (!strcmp(obj_name, "StylePretty")){
        evm_qml_object_set_pointer(p, &lv_style_pretty);
    } else if (!strcmp(obj_name, "StyleBtnRelease")){
        evm_qml_object_set_pointer(p, &lv_style_btn_rel);
    } else if (!strcmp(obj_name, "StyleBtnPress")){
        evm_qml_object_set_pointer(p, &lv_style_btn_pr);
    }
    if( !obj ) return EVM_VAL_UNDEFINED;
    evm_qml_object_set_pointer(p, obj);
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  lvgl_qml_object_set_x(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_set_x(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  lvgl_qml_object_set_y(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_set_y(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  lvgl_qml_object_set_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_set_width(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  lvgl_qml_object_set_height(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_set_height(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t  lvgl_qml_object_set_style(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_class(v) || evm_is_object(v)) ) {
        lv_style_t * style = (lv_style_t*)evm_qml_object_get_pointer(p);
        lv_obj_t * parent = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_obj_set_style(parent, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_object_set_visible(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_boolean(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_set_hidden(obj, evm_2_intptr(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Text_set_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_label_set_text(obj, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Text_set_symbol(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_label_set_text(obj, lvgl_qml_get_symbol(e, v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Text_set_align(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        if (!strcmp(evm_2_string(v), "left"))
            lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
        if (!strcmp(evm_2_string(v), "right"))
            lv_label_set_align(obj, LV_LABEL_ALIGN_RIGHT);
        if (!strcmp(evm_2_string(v), "center"))
            lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
        if (!strcmp(evm_2_string(v), "auto"))
            lv_label_set_align(obj, LV_LABEL_ALIGN_AUTO);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ListView_set_Items(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        if (lv_list_get_size(obj) != 0) return EVM_VAL_UNDEFINED;
        int len = evm_list_len(v);
        int i;
        for(i = 0; i < len; i++){
            evm_val_t * item  = evm_list_get(e, v, i);
            if (evm_is_object(item)){
                char * icon = evm_2_string(evm_prop_get(e, item, "icon", 1));
                char * text = evm_2_string(evm_prop_get(e, item, "text", 1));
                if (text){
                    lv_list_add_btn(obj, NULL, text);
                }
            }else if (evm_is_string(item)){
                lv_list_add_btn(obj, NULL, evm_2_string(item));
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Roller_set_Items(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        if (lv_list_get_size(obj) != 0) return EVM_VAL_UNDEFINED;
        int len = evm_list_len(v);
        int i;
        int count = 0;
        for(i = 0; i < len; i++){
            evm_val_t * item  = evm_list_get(e, v, i);
            if (evm_is_string(item)){
                count += evm_string_len(item);
                count += 1;
            }
        }
        if (count > 0){
            char options[count];
            memset(options, 0, count);
            for(i = 0; i < len; i++){
                evm_val_t * item  = evm_list_get(e, v, i);
                if (evm_is_string(item)){
                    strcat(options, evm_2_string(item));
                    if (i < len -1){
                        strcat(options, "\n");
                    }
                }
            }
            lv_roller_set_options(obj, options, true);
        }

    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Roller_set_currentIndex(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_number(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_roller_set_selected(obj, evm_2_integer(v), 1);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Roller_set_visibleItemsCount(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_number(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_roller_set_visible_row_count(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Slider_set_value(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_number(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_slider_set_value(obj, evm_2_integer(v), LV_ANIM_OFF);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Slider_set_min(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_number(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_slider_set_range(obj, evm_2_integer(v), lv_slider_get_max_value(obj));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Slider_set_max(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_number(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_slider_set_range(obj, lv_slider_get_min_value(obj), evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_MessageBox_set_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_mbox_set_text(obj, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TileView_set_pageIds(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_list(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        int len = evm_list_len(v);
        lv_point_t vp[len];
        for(int i=0; i< len; i++){
            evm_val_t * id = evm_list_get(e, v, i);
            lv_point_t p;
            p.x = evm_2_integer(evm_list_get(e, id, 0));
            p.y = evm_2_integer(evm_list_get(e, id, 1));
            vp[i] = p;
        }
        lv_tileview_set_valid_positions(obj,  vp, len);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TileView_set_currentId(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_list(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_tileview_set_tile_act(obj, evm_2_integer(evm_list_get(e, v, 0)), evm_2_integer(evm_list_get(e, v, 1)), true);
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t lvgl_qml_Tab_set_title(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_obj_t * parent = (lv_obj_t*)lv_obj_get_user_data(obj);
        lv_tabview_set_tab_name(parent, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_MessageBox_set_buttons(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_list(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        int len = evm_list_len(v);
        char ** btns = evm_malloc(sizeof(char*)*(len));
        int i;
        for(i = 0; i < len; i++){
            char * str = evm_2_string( evm_list_get(e, v, i) );
            int str_len = strlen(str);
            btns[i] = evm_malloc( str_len + 1 );
            memcpy(btns[i], str, str_len);
            btns[i][ str_len ] = 0;
        }
        lv_mbox_add_btns(obj, btns);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_text(obj, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_one_line(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_boolean(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_one_line(obj, evm_2_intptr(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_placeholder_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_placeholder_text(obj, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_pwd_mode(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_boolean(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_pwd_mode(obj, evm_2_intptr(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_max_length(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_integer(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_max_length(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_TextArea_set_text_align(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_ta_set_text_align(obj, lvgl_qml_ta_get_text_align(e, v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Keyboard_set_ta(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && ( (evm_is_object(v) || evm_is_class(v) )) ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_kb_set_ta(obj, (lv_obj_t*)evm_qml_object_get_pointer(v));
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t lvgl_qml_Image_set_source(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        char img[evm_string_len(v) + 2];
        sprintf(img, "P:%s", evm_2_string(v));
        lv_img_set_src(obj, img);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ImageButton_set_pressedSource(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        char img[evm_string_len(v) + 2];
        sprintf(img, "P:%s", evm_2_string(v));
        lv_imgbtn_set_src(obj, LV_BTN_STATE_PR, img);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ImageButton_set_releasedSource(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        char img[evm_string_len(v) + 2];
        sprintf(img, "P:%s", evm_2_string(v));
        lv_imgbtn_set_src(obj, LV_BTN_STATE_REL, img);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ImageButton_set_DisabledSource(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        char img[evm_string_len(v) + 2];
        sprintf(img, "P:%s", evm_2_string(v));
        lv_imgbtn_set_src(obj, LV_BTN_STATE_INA, img);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_CheckBox_set_text(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_cb_set_text(obj, evm_2_string(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Container_set_layout(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_cont_set_layout(obj, lvgl_qml_get_layout(e, v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Container_set_fit(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && (evm_is_string(v))){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer(p);
        lv_cont_set_fit(obj, lvgl_qml_get_fit(e, v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_main_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.main_color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_grad_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.grad_color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_radius(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.radius = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.opa = evm_2_double(v) * 255;
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_border_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.border.color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_border_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.border.opa = evm_2_double(v) * 255;
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_border_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.border.width = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_padding_bottom(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.padding.bottom = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_padding_top(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.padding.top = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_padding_inner(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.padding.inner = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_padding_left(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.padding.left = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_padding_right(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->body.padding.right = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_text_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->text.color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_text_sel_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->text.sel_color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_text_letter_space(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->text.letter_space = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_text_line_space(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->text.line_space = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_text_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->text.opa = evm_2_double(v) * 255;
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_line_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->line.color = lvgl_qml_style_get_color(e, v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_line_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->line.width = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_line_opa(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->line.opa = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Style_set_line_rounded(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_style_t * style = lv_obj_get_style(obj);
        style->line.rounded = evm_2_integer(v);
        lv_obj_set_style(obj, style);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Circle_set_angles(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        if (evm_list_len(v) == 1){
            lv_arc_set_angles(obj, 0, evm_2_integer(evm_list_get(e, v, 0)));
        }else{
            lv_arc_set_angles(obj, evm_2_integer(evm_list_get(e, v, 0)), evm_2_integer(evm_list_get(e, v, 1)));
        }
    }else if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_arc_set_angles(obj, 0, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Bar_set_min(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_bar_set_range(obj, evm_2_integer(v) , lv_bar_get_max_value(obj));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Bar_set_max(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_bar_set_range(obj, lv_bar_get_min_value(obj),  evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Bar_set_value(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_bar_set_value(obj, evm_2_integer(v), false);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Chart_set_type(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_string(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_chart_set_type(obj, lvgl_qml_get_ChartType(e, v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Chart_set_lineWidth(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_integer(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_chart_set_series_width(obj, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Chart_set_lineCounts(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_chart_set_div_line_count(obj, evm_2_integer(evm_list_get(e, v, 0)), evm_2_integer(evm_list_get(e, v, 1)));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Chart_set_yrange(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        lv_chart_set_range(obj, evm_2_integer(evm_list_get(e, v, 0)), evm_2_integer(evm_list_get(e, v, 1)));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ChartSeries_set_data(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * chart = (lv_obj_t *)evm_qml_object_get_pointer( evm_get_parent(e, *p) );
        lv_chart_series_t * obj = (lv_chart_series_t*)evm_qml_object_get_pointer( p );
        int len = evm_list_len(v);
        lv_coord_t y_array[len];
        for(int i=0; i< len; i++){
            y_array[i] = evm_2_integer(evm_list_get(e, v, i));
        }
        lv_chart_set_points(chart, obj, y_array);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_ChartSeries_set_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_string(v)){
        lv_chart_series_t * obj = (lv_chart_series_t *)evm_qml_object_get_pointer( p );
        obj->color =  lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

/***********Gauge*************/
static evm_val_t lvgl_qml_Gauge_set_needle(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        int count = evm_list_len(v);
        lv_color_t * colors = lv_mem_alloc( count * sizeof(lv_color_t));
        for(int i = 0; i < count; i++){
            colors[i] = lvgl_qml_style_get_color(e, evm_list_get(e, v, i) );
        }
        lv_gauge_set_needle_count(obj, count, colors);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Gauge_set_value(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        evm_val_t * id_v = evm_prop_get(e, p, "index", 0);
        if( id_v && evm_is_number(id_v) ){
            lv_gauge_set_value(obj, evm_2_integer(id_v), evm_2_integer(v));
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Gauge_set_min(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        int max = lv_gauge_get_max_value(obj);
        lv_gauge_set_range(obj, evm_2_integer(v), max);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Gauge_set_max(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_number(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        int min = lv_gauge_get_min_value(obj);
        lv_gauge_set_range(obj, min, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_Gauge_set_scale(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 && evm_is_list(v)){
        lv_obj_t * obj = (lv_obj_t*)evm_qml_object_get_pointer( p );
        uint16_t angle = evm_2_integer( evm_list_get(e, v, 0) );
        uint8_t line_cnt = evm_2_integer( evm_list_get(e, v, 1) );
        uint8_t label_cnt = evm_2_integer( evm_list_get(e, v, 2) );
        lv_gauge_set_scale(obj, angle, line_cnt, label_cnt);
    }
    return EVM_VAL_UNDEFINED;
}

/***********全局样式*************/
static evm_val_t lvgl_qml_GlobalStyle_set_main_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.main_color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_grad_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.grad_color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_radius(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.radius = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.opa = evm_2_double(v) * 255;
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_border_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.border.color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_border_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.border.opa = evm_2_double(v) * 255;
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_border_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.border.width = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_padding_bottom(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.padding.bottom = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_padding_top(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.padding.top = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_padding_inner(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.padding.inner = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_padding_left(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.padding.left = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_padding_right(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->body.padding.right = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_text_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->text.color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_text_sel_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->text.sel_color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_text_letter_space(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->text.letter_space = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_text_line_space(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->text.line_space = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_text_opacity(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->text.opa = evm_2_double(v) * 255;
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_line_color(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->line.color = lvgl_qml_style_get_color(e, v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_line_width(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->line.width = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_line_opa(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->line.opa = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t lvgl_qml_GlobalStyle_set_line_rounded(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    if( argc >= 1 ){
        lv_style_t * style = (lv_obj_t*)evm_qml_object_get_pointer( p );
        style->line.rounded = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}


static evm_val_t lvgl_qml_init(evm_t * e, evm_val_t *p, int argc, evm_val_t * v){
    EVM_UNUSED(e);EVM_UNUSED(argc);EVM_UNUSED(v);
    int len = evm_prop_len(p);
    for(int i = 0; i < len; i++){
        evm_val_t * obj = evm_prop_get_by_index(e, p, i);
        if( evm_is_class(obj) || evm_is_object(obj) ){
            evm_val_t * fn = evm_prop_get(e, obj, "onCompleted", 0);
            if(fn){
                evm_run_callback(e, fn, obj, NULL, 0);
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

#define BASE_VALUES {EVM_QML_INT, "x", (evm_native_fn)lvgl_qml_object_set_x},\
                    {EVM_QML_INT, "y", (evm_native_fn)lvgl_qml_object_set_y},\
                    {EVM_QML_INT, "width", (evm_native_fn)lvgl_qml_object_set_width},\
                    {EVM_QML_INT, "height", (evm_native_fn)lvgl_qml_object_set_height},\
                    {EVM_QML_ANY, "style", (evm_native_fn)lvgl_qml_object_set_style},\
                    {EVM_QML_CALLBACK, "onCompleted", NULL},\
                    {EVM_QML_BOOLEAN, "visible", (evm_native_fn)lvgl_qml_object_set_visible},


#define GLOBALSTYLE_VALUES  {EVM_QML_INT | EVM_QML_STRING, "main_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_main_color},\
                            {EVM_QML_INT | EVM_QML_STRING, "grad_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_grad_color},\
                            {EVM_QML_INT, "radius", (evm_native_fn)lvgl_qml_GlobalStyle_set_radius},\
                            {EVM_QML_DOUBLE, "opacity", (evm_native_fn)lvgl_qml_GlobalStyle_set_opacity},\
                            {EVM_QML_INT | EVM_QML_STRING, "border_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_border_color},\
                            {EVM_QML_DOUBLE, "border_opacity", (evm_native_fn)lvgl_qml_GlobalStyle_set_border_opacity},\
                            {EVM_QML_INT, "border_width", (evm_native_fn)lvgl_qml_GlobalStyle_set_border_width},\
                            {EVM_QML_INT, "padding_bottom", (evm_native_fn)lvgl_qml_GlobalStyle_set_padding_bottom},\
                            {EVM_QML_INT, "padding_top", (evm_native_fn)lvgl_qml_GlobalStyle_set_padding_top},\
                            {EVM_QML_INT, "padding_left", (evm_native_fn)lvgl_qml_GlobalStyle_set_padding_left},\
                            {EVM_QML_INT, "padding_right", (evm_native_fn)lvgl_qml_GlobalStyle_set_padding_right},\
                            {EVM_QML_INT, "padding_inner", (evm_native_fn)lvgl_qml_GlobalStyle_set_padding_inner},\
                            {EVM_QML_INT | EVM_QML_STRING, "text_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_text_color},\
                            {EVM_QML_INT | EVM_QML_STRING, "sel_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_text_sel_color},\
                            {EVM_QML_INT, "letter_space", (evm_native_fn)lvgl_qml_GlobalStyle_set_text_letter_space},\
                            {EVM_QML_INT, "line_space", (evm_native_fn)lvgl_qml_GlobalStyle_set_text_line_space},\
                            {EVM_QML_DOUBLE, "text_opacity", (evm_native_fn)lvgl_qml_GlobalStyle_set_text_opacity},\
                            {EVM_QML_INT | EVM_QML_STRING, "line_color", (evm_native_fn)lvgl_qml_GlobalStyle_set_line_color},\
                            {EVM_QML_INT, "line_width", (evm_native_fn)lvgl_qml_GlobalStyle_set_line_width},\
                            {EVM_QML_INT, "line_opa", (evm_native_fn)lvgl_qml_GlobalStyle_set_line_opa},\
                            {EVM_QML_INT, "line_rounded", (evm_native_fn)lvgl_qml_GlobalStyle_set_line_rounded},\
							
static evm_qml_value_reg_t qml_rectangle_values[] = {
	BASE_VALUES
	{EVM_QML_CALLBACK, "onClicked", NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Style_values[] = {
	{EVM_QML_INT | EVM_QML_STRING, "main_color", (evm_native_fn)lvgl_qml_Style_set_main_color},
	{EVM_QML_INT | EVM_QML_STRING, "grad_color", (evm_native_fn)lvgl_qml_Style_set_grad_color},
	{EVM_QML_INT, "radius", (evm_native_fn)lvgl_qml_Style_set_radius},
	{EVM_QML_DOUBLE, "opacity", (evm_native_fn)lvgl_qml_Style_set_opacity},
	{EVM_QML_INT | EVM_QML_STRING, "border_color", (evm_native_fn)lvgl_qml_Style_set_border_color},
	{EVM_QML_DOUBLE, "border_opacity", (evm_native_fn)lvgl_qml_Style_set_border_opacity},
	{EVM_QML_INT, "border_width", (evm_native_fn)lvgl_qml_Style_set_border_width},
	{EVM_QML_INT, "padding_bottom", (evm_native_fn)lvgl_qml_Style_set_padding_bottom},
	{EVM_QML_INT, "padding_top", (evm_native_fn)lvgl_qml_Style_set_padding_top},
	{EVM_QML_INT, "padding_left", (evm_native_fn)lvgl_qml_Style_set_padding_left},
	{EVM_QML_INT, "padding_right", (evm_native_fn)lvgl_qml_Style_set_padding_right},
	{EVM_QML_INT, "padding_inner", (evm_native_fn)lvgl_qml_Style_set_padding_inner},
	{EVM_QML_INT | EVM_QML_STRING, "text_color", (evm_native_fn)lvgl_qml_Style_set_text_color},
	{EVM_QML_INT | EVM_QML_STRING, "sel_color", (evm_native_fn)lvgl_qml_Style_set_text_sel_color},
	{EVM_QML_INT, "letter_space", (evm_native_fn)lvgl_qml_Style_set_text_letter_space},
	{EVM_QML_INT, "line_space", (evm_native_fn)lvgl_qml_Style_set_text_line_space},
	{EVM_QML_DOUBLE, "text_opacity", (evm_native_fn)lvgl_qml_Style_set_text_opacity},
	{EVM_QML_INT | EVM_QML_STRING, "line_color", (evm_native_fn)lvgl_qml_Style_set_line_color},
	{EVM_QML_INT, "line_width", (evm_native_fn)lvgl_qml_Style_set_line_width},
	{EVM_QML_INT, "line_opa", (evm_native_fn)lvgl_qml_Style_set_line_opa},
	{EVM_QML_INT, "line_rounded", (evm_native_fn)lvgl_qml_Style_set_line_rounded},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Circle_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "angles", (evm_native_fn)lvgl_qml_Circle_set_angles},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Bar_values[] = {
	BASE_VALUES
	{EVM_QML_INT, "min", (evm_native_fn)lvgl_qml_Bar_set_min},
	{EVM_QML_INT, "max", (evm_native_fn)lvgl_qml_Bar_set_max},
	{EVM_QML_INT, "value", (evm_native_fn)lvgl_qml_Bar_set_value},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Chart_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "type", (evm_native_fn)lvgl_qml_Chart_set_type},
	{EVM_QML_INT, "lineWidth", (evm_native_fn)lvgl_qml_Chart_set_lineWidth},
    {EVM_QML_ANY, "lineCounts", (evm_native_fn)lvgl_qml_Chart_set_lineCounts},
    {EVM_QML_ANY, "yRange", (evm_native_fn)lvgl_qml_Chart_set_yrange},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_ChartSeries_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "data", (evm_native_fn)lvgl_qml_ChartSeries_set_data},
	{EVM_QML_STRING, "color", (evm_native_fn)lvgl_qml_ChartSeries_set_color},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_button_values[] = {
	BASE_VALUES
	{EVM_QML_CALLBACK, "onClicked", NULL},
	{EVM_QML_CALLBACK, "onPressed", NULL},
	{EVM_QML_CALLBACK, "onReleased", NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Imagebutton_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "pressed", (evm_native_fn)lvgl_qml_ImageButton_set_pressedSource},
	{EVM_QML_STRING, "released", (evm_native_fn)lvgl_qml_ImageButton_set_releasedSource},
	{EVM_QML_STRING, "disbaled", (evm_native_fn)lvgl_qml_ImageButton_set_DisabledSource},
	{EVM_QML_CALLBACK, "onClicked", NULL},
	{EVM_QML_CALLBACK, "onPressed", NULL},
	{EVM_QML_CALLBACK, "onReleased", NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_TileView_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "pageIds", (evm_native_fn)lvgl_qml_TileView_set_pageIds},
    {EVM_QML_ANY, "currentId", (evm_native_fn)lvgl_qml_TileView_set_currentId},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Tile_values[] = {
	BASE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_TabView_values[] = {
	BASE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Tab_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "title", (evm_native_fn)lvgl_qml_Tab_set_title},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_text_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "text", (evm_native_fn)lvgl_qml_Text_set_text},
	{EVM_QML_STRING, "symbol", (evm_native_fn)lvgl_qml_Text_set_symbol},
	{EVM_QML_STRING, "align", (evm_native_fn)lvgl_qml_Text_set_align},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_ListView_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "items", (evm_native_fn)lvgl_qml_ListView_set_Items},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Roller_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "items", (evm_native_fn)lvgl_qml_Roller_set_Items},
	{EVM_QML_INT, "currentIndex", (evm_native_fn)lvgl_qml_Roller_set_currentIndex},
	{EVM_QML_INT, "visibleItemsCount", (evm_native_fn)lvgl_qml_Roller_set_visibleItemsCount},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Slider_values[] = {
	BASE_VALUES
	{EVM_QML_INT, "value", (evm_native_fn)lvgl_qml_Slider_set_value},
	{EVM_QML_INT, "min", (evm_native_fn)lvgl_qml_Slider_set_min},
	{EVM_QML_INT, "max", (evm_native_fn)lvgl_qml_Slider_set_max},
	{EVM_QML_CALLBACK, "onValueChanged", (evm_native_fn)NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_ScrollArea_values[] = {
	BASE_VALUES
	{NULL, NULL, NULL}
};


static evm_qml_value_reg_t qml_MessageBox_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "text", (evm_native_fn)lvgl_qml_MessageBox_set_text},
	{EVM_QML_STRING, "buttons", (evm_native_fn)lvgl_qml_MessageBox_set_buttons},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_TextArea_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "textAlign", (evm_native_fn)lvgl_qml_TextArea_set_text_align},
	{EVM_QML_STRING, "text", (evm_native_fn)lvgl_qml_TextArea_set_text},
	{EVM_QML_STRING, "placeholder", (evm_native_fn)lvgl_qml_TextArea_set_placeholder_text},
	{EVM_QML_BOOLEAN, "oneLine", (evm_native_fn)lvgl_qml_TextArea_set_one_line},
	{EVM_QML_INT, "maxLength", (evm_native_fn)lvgl_qml_TextArea_set_max_length},
	{EVM_QML_BOOLEAN, "pwdMode", (evm_native_fn)lvgl_qml_TextArea_set_pwd_mode},
	{EVM_QML_CALLBACK, "onValueChanged", NULL},
	{EVM_QML_CALLBACK, "onFocused", NULL},
	{EVM_QML_CALLBACK, "onDefocused", NULL},
	{EVM_QML_CALLBACK, "onClicked", NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Keyboard_values[] = {
	BASE_VALUES
    {EVM_QML_ANY, "ta", (evm_native_fn)lvgl_qml_Keyboard_set_ta},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Image_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "source", (evm_native_fn)lvgl_qml_Image_set_source},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_CheckBox_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "text", (evm_native_fn)lvgl_qml_CheckBox_set_text},
	{EVM_QML_CALLBACK, "onClicked", NULL},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Container_values[] = {
	BASE_VALUES
	{EVM_QML_STRING, "layout", (evm_native_fn)lvgl_qml_Container_set_layout},
	{EVM_QML_BOOLEAN, "fit", (evm_native_fn)lvgl_qml_Container_set_fit},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_StylePretty_values[] = {
	GLOBALSTYLE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_StylePlain_values[] = {
	GLOBALSTYLE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_StyleBtnRelease_values[] = {
	GLOBALSTYLE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_StyleBtnPress_values[] = {
	GLOBALSTYLE_VALUES
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_Gauge_values[] = {
	BASE_VALUES
	{EVM_QML_INT, "max", (evm_native_fn)lvgl_qml_Gauge_set_max},
	{EVM_QML_INT, "min", (evm_native_fn)lvgl_qml_Gauge_set_min},
    {EVM_QML_ANY, "scale", (evm_native_fn)lvgl_qml_Gauge_set_scale},
    {EVM_QML_ANY, "needle", (evm_native_fn)lvgl_qml_Gauge_set_needle},
	{NULL, NULL, NULL}
};

static evm_qml_value_reg_t qml_GaugeValue_values[] = {
	{EVM_QML_INT, "value", (evm_native_fn)lvgl_qml_Gauge_set_value},
	{NULL, NULL, NULL}
};

static evm_qml_object_reg_t qml_objects[] = {
    {"Rectangle", NULL, (evm_native_fn)lvgl_qml_object_create, qml_rectangle_values},
    {"Circle", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Circle_values},
    {"ProgressBar", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Bar_values},
    {"Chart", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Chart_values},
    {"ChartSeries", NULL, (evm_native_fn)lvgl_qml_object_create, qml_ChartSeries_values},
    {"Button", NULL, (evm_native_fn)lvgl_qml_object_create, qml_button_values},
    {"ImageButton", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Imagebutton_values},
    {"Text", NULL, (evm_native_fn)lvgl_qml_object_create, qml_text_values},
    {"TileView", NULL, (evm_native_fn)lvgl_qml_object_create, qml_TileView_values},
    {"Tile", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Tile_values},
    {"TabView", NULL, (evm_native_fn)lvgl_qml_object_create, qml_TabView_values},
    {"Tab", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Tab_values},
    {"ListView", NULL, (evm_native_fn)lvgl_qml_object_create, qml_ListView_values},
    {"Roller", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Roller_values},
    {"Slider", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Slider_values},
    {"ScrollArea", NULL, (evm_native_fn)lvgl_qml_object_create, qml_ScrollArea_values},
    {"MessageBox", NULL, (evm_native_fn)lvgl_qml_object_create, qml_MessageBox_values},
    {"TextArea", NULL, (evm_native_fn)lvgl_qml_object_create, qml_TextArea_values},
    {"Keyboard", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Keyboard_values},
    {"Image", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Image_values},
    {"CheckBox", NULL, (evm_native_fn)lvgl_qml_object_create, qml_CheckBox_values},
    {"Container", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Container_values},
    {"Style", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Style_values},
    {"StylePretty", NULL, (evm_native_fn)lvgl_qml_object_create, qml_StylePretty_values},
    {"StylePlain", NULL, (evm_native_fn)lvgl_qml_object_create, qml_StylePlain_values},
    {"StyleBtnRelease", NULL, (evm_native_fn)lvgl_qml_object_create, qml_StyleBtnRelease_values},
    {"StyleBtnPress", NULL, (evm_native_fn)lvgl_qml_object_create, qml_StyleBtnPress_values},
    {"Gauge", NULL, (evm_native_fn)lvgl_qml_object_create, qml_Gauge_values},
    {"GaugeValue", NULL, (evm_native_fn)lvgl_qml_object_create, qml_GaugeValue_values},
    {NULL, NULL, NULL, NULL}
};

int lvgl_qml_module(evm_t * e){
    global_e = e;
    evm_qml_register(e, qml_objects);
    int err = qml_module(e, (evm_native_fn)lvgl_qml_init);
    return err;
}

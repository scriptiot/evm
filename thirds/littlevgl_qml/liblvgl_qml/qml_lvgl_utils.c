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
#include "qml_lvgl_utils.h"

void lvgl_qml_obj_add_style(lv_obj_t * obj){
    lv_style_t * style = lv_mem_alloc(sizeof(lv_style_t));
    memcpy(style, &lv_style_plain, sizeof(lv_style_t));
    lv_obj_set_style(obj, style);
}

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

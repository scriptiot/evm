/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#ifdef CONFIG_EVM_LCD

#include "evm_module.h"
#include <drivers/display.h>


uint16_t evm_lcd_color = 0;

//LCD()
static evm_val_t evm_module_lcd(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	const char * name = evm_2_string(v);
	struct device * dev = device_get_binding(name);
	if (dev == NULL) {
		evm_set_err(evm_runtime, ec_type, "Driver is not found");
	}
	evm_object_set_ext_data(p, (intptr_t)dev);
	return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_lcd_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
	display_blanking_off(dev);
	return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_lcd_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
	display_blanking_on(dev);
	return EVM_VAL_UNDEFINED;
}

//LCD.set_pixel(x, y)
static evm_val_t evm_module_lcd_set_pixel(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	struct device * dev = (struct device *)evm_object_get_ext_data(p);
	struct display_buffer_descriptor buf_desc;
	buf_desc.buf_size = 2;
	buf_desc.pitch = 1;
	buf_desc.width = 1;
	buf_desc.height = 1;

	int x = evm_2_integer(v);
	int y = evm_2_integer(v + 1);
	display_write(dev, x, y, &buf_desc, &evm_lcd_color);
	return EVM_VAL_UNDEFINED;
}
//color = LCD.get_pixel(x, y)
static evm_val_t evm_module_lcd_get_pixel(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_line(x1, y1, x2, y2)
static evm_val_t evm_module_lcd_draw_line(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_rect(x, y, w, h)
static evm_val_t evm_module_lcd_draw_rect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.fill_rect(x, y, w, h)
static evm_val_t evm_module_lcd_fill_rect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_circle(x, y, radius)
static evm_val_t evm_module_lcd_draw_circle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.fill_circle(x, y, colour)
static evm_val_t evm_module_lcd_fill_circle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_text(x, y, text)
static evm_val_t evm_module_lcd_draw_text(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.set_paint_color(rgb)
static evm_val_t evm_module_lcd_set_text_color(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

//LCD.set_background_color(rgb)
static evm_val_t evm_module_lcd_set_background_color(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(p);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_lcd(evm_t * e){
	evm_builtin_t class_lcd[] = {
		{"on", evm_mk_native( (intptr_t)evm_module_lcd_on )},
		{"off", evm_mk_native( (intptr_t)evm_module_lcd_off )},
		{"set_pixel", evm_mk_native( (intptr_t)evm_module_lcd_set_pixel )},
		{"get_pixel", evm_mk_native( (intptr_t)evm_module_lcd_get_pixel )},
		{"draw_line", evm_mk_native( (intptr_t)evm_module_lcd_draw_line )},
		{"draw_rect", evm_mk_native( (intptr_t)evm_module_lcd_draw_rect )},
		{"fill_rect", evm_mk_native( (intptr_t)evm_module_lcd_fill_rect )},
		{"draw_circle", evm_mk_native( (intptr_t)evm_module_lcd_draw_circle )},
		{"fill_circle", evm_mk_native( (intptr_t)evm_module_lcd_fill_circle )},
		{"draw_text", evm_mk_native( (intptr_t)evm_module_lcd_draw_text )},
		{"set_paint_color", evm_mk_native( (intptr_t)evm_module_lcd_set_text_color )},
		{"set_background_color", evm_mk_native( (intptr_t)evm_module_lcd_set_background_color )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_lcd, class_lcd, NULL);
}

#endif

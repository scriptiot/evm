#ifdef CONFIG_EVM_LCD

#include "evm_module.h"

//LCD()
static evm_val_t evm_module_lcd(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_lcd_on(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_lcd_off(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//LCD.set_pixel(x, y)
static evm_val_t evm_module_lcd_set_pixel(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc < 2 ) EVM_ARG_LENGTH_ERR
	if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
	if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
	int x = evm_2_integer(v);
	int y = evm_2_integer(v + 1);

	return EVM_VAL_UNDEFINED;
}
//color = LCD.get_pixel(x, y)
static evm_val_t evm_module_lcd_get_pixel(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_line(x1, y1, x2, y2)
static evm_val_t evm_module_lcd_draw_line(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 4 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 2) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 3) ) EVM_ARG_TYPE_ERR;
		int x1 = evm_2_integer(v);
		int y1 = evm_2_integer(v + 1);
		int x2 = evm_2_integer(v + 2);
		int y2 = evm_2_integer(v + 3);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_rect(x, y, w, h)
static evm_val_t evm_module_lcd_draw_rect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 4 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 2) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 3) ) EVM_ARG_TYPE_ERR;
		int x1 = evm_2_integer(v);
		int y1 = evm_2_integer(v + 1);
		int w = evm_2_integer(v + 2);
		int h = evm_2_integer(v + 3);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.fill_rect(x, y, w, h)
static evm_val_t evm_module_lcd_fill_rect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 4 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 2) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 3) ) EVM_ARG_TYPE_ERR;
		int x1 = evm_2_integer(v);
		int y1 = evm_2_integer(v + 1);
		int w = evm_2_integer(v + 2);
		int h = evm_2_integer(v + 3);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_circle(x, y, radius)
static evm_val_t evm_module_lcd_draw_circle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 3 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 2) ) EVM_ARG_TYPE_ERR;
		int x1 = evm_2_integer(v);
		int y1 = evm_2_integer(v + 1);
		int r = evm_2_integer(v + 2);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.fill_circle(x, y, colour)
static evm_val_t evm_module_lcd_fill_circle(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//LCD.draw_text(x, y, text)
static evm_val_t evm_module_lcd_draw_text(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 3 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_number(v + 1) ) EVM_ARG_TYPE_ERR;
		if( !evm_is_string(v + 2) ) EVM_ARG_TYPE_ERR;
		int x1 = evm_2_integer(v);
		int y1 = evm_2_integer(v + 1);
		char * text = (char*)evm_2_string(v + 2);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.set_paint_color(rgb)
static evm_val_t evm_module_lcd_set_text_color(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 1 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		int color = evm_2_integer(v);
	}
	return EVM_VAL_UNDEFINED;
}

//LCD.set_background_color(rgb)
static evm_val_t evm_module_lcd_set_background_color(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc >= 1 ){
		if( !evm_is_number(v) ) EVM_ARG_TYPE_ERR;
		int color = evm_2_integer(v);
	}
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

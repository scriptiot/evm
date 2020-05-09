#include "nevm.h"

#ifdef CONFIG_EVM_LCD
#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>

#if DT_HAS_NODE(DT_INST(0, ilitek_ili9340))
#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, ilitek_ili9340))
#endif

#if DT_HAS_NODE(DT_INST(0, solomon_ssd1306fb))
#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, solomon_ssd1306fb))
#endif

#if DT_HAS_NODE(DT_INST(0, gooddisplay_gdeh0213b1))
#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, gooddisplay_gdeh0213b1))
#endif

#if DT_HAS_NODE(DT_INST(0, sitronix_st7789v))
#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, sitronix_st7789v))
#endif

#if DT_HAS_NODE(DT_INST(0, fsl_imx6sx_lcdif))
#define DISPLAY_DEV_NAME DT_LABEL(DT_INST(0, fsl_imx6sx_lcdif))
#endif

enum corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT
};

typedef void (*fill_buffer)(enum corner corner, u8_t grey, u8_t *buf,
			    size_t buf_size);

static void fill_buffer_argb8888(enum corner corner, u8_t grey, u8_t *buf,
			       size_t buf_size)
{
	u32_t color = 0;

	switch (corner) {
	case TOP_LEFT:
		color = 0x00FF0000u;
		break;
	case TOP_RIGHT:
		color = 0x0000FF00u;
		break;
	case BOTTOM_RIGHT:
		color = 0x000000FFu;
		break;
	case BOTTOM_LEFT:
		color = grey << 16 | grey << 8 | grey;
		break;
	}

	for (size_t idx = 0; idx < buf_size; idx += 4) {
		*((u32_t *)(buf + idx)) = color;
	}
}

static void fill_buffer_rgb888(enum corner corner, u8_t grey, u8_t *buf,
			       size_t buf_size)
{
	u32_t color = 0;

	switch (corner) {
	case TOP_LEFT:
		color = 0x00FF0000u;
		break;
	case TOP_RIGHT:
		color = 0x0000FF00u;
		break;
	case BOTTOM_RIGHT:
		color = 0x000000FFu;
		break;
	case BOTTOM_LEFT:
		color = grey << 16 | grey << 8 | grey;
		break;
	}

	for (size_t idx = 0; idx < buf_size; idx += 3) {
		*(buf + idx + 0) = color >> 16;
		*(buf + idx + 1) = color >> 8;
		*(buf + idx + 2) = color >> 0;
	}
}

static u16_t get_rgb565_color(enum corner corner, u8_t grey)
{
	u16_t color = 0;
	u16_t grey_5bit;

	switch (corner) {
	case TOP_LEFT:
		color = 0xF800u;
		break;
	case TOP_RIGHT:
		color = 0x07E0u;
		break;
	case BOTTOM_RIGHT:
		color = 0x001Fu;
		break;
	case BOTTOM_LEFT:
		grey_5bit = grey & 0x1Fu;
		/* shift the green an extra bit, it has 6 bits */
		color = grey_5bit << 11 | grey_5bit << (5 + 1) | grey_5bit;
		break;
	}
	return color;
}

static void fill_buffer_rgb565(enum corner corner, u8_t grey, u8_t *buf,
			       size_t buf_size)
{
	u16_t color = get_rgb565_color(corner, grey);

	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(buf + idx + 0) = (color >> 8) & 0xFFu;
		*(buf + idx + 1) = (color >> 0) & 0xFFu;
	}
}

static void fill_buffer_bgr565(enum corner corner, u8_t grey, u8_t *buf,
			       size_t buf_size)
{
	u16_t color = get_rgb565_color(corner, grey);

	for (size_t idx = 0; idx < buf_size; idx += 2) {
		*(u16_t *)(buf + idx) = color;
	}
}

static void fill_buffer_mono(enum corner corner, u8_t grey, u8_t *buf,
			     size_t buf_size)
{
	u16_t color;

	switch (corner) {
	case BOTTOM_LEFT:
		color = (grey & 0x01u) ? 0xFFu : 0x00u;
		break;
	default:
		color = 0;
		break;
	}

	memset(buf, color, buf_size);
}

#endif

evm_val_t nevm_driver_lcd_config(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_LCD
    struct device * dev = (struct device *)nevm_object_get_ext_data(p);
    if( dev == NULL ){
        evm_set_err(e, ec_type, "LCD Device not found");
        return NEVM_FALSE;
    }
#endif
    return NEVM_TRUE;
}

evm_val_t nevm_driver_lcd_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
#ifdef CONFIG_EVM_LCD
    struct device * display_dev = device_get_binding(DISPLAY_DEV_NAME);
    if (display_dev == NULL) {
        evm_set_err(e, ec_type, "LCD Device not found");
        return NEVM_FALSE;
	}
    nevm_object_set_ext_data(p, (intptr_t)dev);
#endif
    return NEVM_TRUE;
}

evm_val_t nevm_driver_lcd_deinit(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_set_point(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_get_point(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_draw_line(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_draw_circle(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_fill_rect(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

evm_val_t nevm_driver_lcd_get_rx_message(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)p;
    return EVM_VAL_UNDEFINED;
}

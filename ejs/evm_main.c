#include "evm_main.h"
#include "uol_output.h"
#include <drivers/gpio.h>
#include <drivers/display.h>

evm_t * nevm_runtime;

char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    char c = console_getchar();
    return c;
}

const char * vm_load(evm_t * e, char * path, int type){
    if( !strcmp(path, "uol_output.ubc") ){
        return uol_binary_buf;
    }
    return NULL;
}

void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}

void vm_free(void * mem)
{
    if(mem) free(mem);
}

int nevm_runtime_setup(void){
    nevm_runtime = (evm_t*)malloc(sizeof(evm_t));
    memset(nevm_runtime, 0, sizeof(evm_t));
    int err = evm_init(nevm_runtime, NEVM_HEAP_SIZE, NEVM_STACK_SIZE, NEVM_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

    err = evm_boot(nevm_runtime, "uol_output.ubc");
    if (err != ec_ok )  {
        printk("Unable to boot uol runtime");
        return err;
    }
    err = nevm_start(nevm_runtime);
    if(err) {return err;}
    return ec_ok;
}

const struct uart_config uart_cfg = {
    .baudrate = 115200,
    .parity = UART_CFG_PARITY_NONE,
    .stop_bits = UART_CFG_STOP_BITS_1,
    .data_bits = UART_CFG_DATA_BITS_8,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};

void console_setup(){
    struct device *uart_dev = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);
    uart_configure(uart_dev, &uart_cfg);
    console_init();	
}

extern int ecma_module(evm_t * e, int num_of_timers);

#if defined(CONFIG_ST7789V)
#define DISPLAY_DRIVER		"ST7789V"
#endif


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

#ifdef CONFIG_SDL_DISPLAY_DEV_NAME
#define DISPLAY_DEV_NAME CONFIG_SDL_DISPLAY_DEV_NAME
#endif

#ifdef CONFIG_DUMMY_DISPLAY_DEV_NAME
#define DISPLAY_DEV_NAME CONFIG_DUMMY_DISPLAY_DEV_NAME
#endif
#ifdef CONFIG_ARCH_POSIX
#include "posix_board_if.h"
#endif

#ifdef CONFIG_ARCH_POSIX
#define RETURN_FROM_MAIN(exit_code) posix_exit_main(exit_code)
#else
#define RETURN_FROM_MAIN(exit_code) return
#endif

enum corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT
};

typedef void (*fill_buffer)(enum corner corner, u8_t grey, u8_t *buf,
			    size_t buf_size);


#ifdef CONFIG_ARCH_POSIX
static void posix_exit_main(int exit_code)
{
#if CONFIG_TEST
	if (exit_code == 0) {
		LOG_INF("PROJECT EXECUTION SUCCESSFUL");
	} else {
		LOG_INF("PROJECT EXECUTION FAILED");
	}
#endif
	posix_exit(exit_code);
}
#endif

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

int evm_main(void){
    console_setup();
    printk("\nstart");

    struct device * pin_dev = device_get_binding("GPIOB");
    if( !pin_dev ){
        printk("pin not found");
        return 0;
    } else {
        printk("pin found");
    }
    gpio_pin_configure(pin_dev, 15, GPIO_OUTPUT| GPIO_PULL_UP);
    gpio_pin_write(pin_dev, 15, 1);

    size_t x;
	size_t y;
	size_t rect_w;
	size_t rect_h;
	size_t h_step;
	size_t scale;
	size_t grey_count;
	u8_t *buf;
	s32_t grey_scale_sleep;
	struct device *display_dev;
	struct display_capabilities capabilities;
	struct display_buffer_descriptor buf_desc;
	size_t buf_size = 0;
	fill_buffer fill_buffer_fnc = NULL;

	printk("Display sample for %s", DISPLAY_DEV_NAME);

	display_dev = device_get_binding(DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		printk("Device %s not found. Aborting sample.",
			DISPLAY_DEV_NAME);
		RETURN_FROM_MAIN(1);
	} else {
        printk("Display found.");
    }

	display_get_capabilities(display_dev, &capabilities);

	if (capabilities.screen_info & SCREEN_INFO_MONO_VTILED)  {
		rect_w = 16;
		rect_h = 8;
	} else {
		rect_w = 2;
		rect_h = 1;
	}

	h_step = rect_h;
	scale = (capabilities.x_resolution / 8) / rect_h;

	rect_w *= scale;
	rect_h *= scale;

	if (capabilities.screen_info & SCREEN_INFO_EPD) {
		grey_scale_sleep = 100;
	} else {
		grey_scale_sleep = 100;
	}

	buf_size = rect_w * rect_h;

	if (buf_size < (capabilities.x_resolution * h_step)) {
		buf_size = capabilities.x_resolution * h_step;
	}

	switch (capabilities.current_pixel_format) {
	case PIXEL_FORMAT_ARGB_8888:
		fill_buffer_fnc = fill_buffer_argb8888;
		buf_size *= 4;
		break;
	case PIXEL_FORMAT_RGB_888:
		fill_buffer_fnc = fill_buffer_rgb888;
		buf_size *= 3;
		break;
	case PIXEL_FORMAT_RGB_565:
		fill_buffer_fnc = fill_buffer_rgb565;
		buf_size *= 2;
		break;
	case PIXEL_FORMAT_BGR_565:
		fill_buffer_fnc = fill_buffer_bgr565;
		buf_size *= 2;
		break;
	case PIXEL_FORMAT_MONO01:
	case PIXEL_FORMAT_MONO10:
		fill_buffer_fnc = fill_buffer_mono;
		buf_size /= 8;
		break;
	default:
		printk("Unsupported pixel format. Aborting sample.");
		RETURN_FROM_MAIN(1);
	}

	buf = malloc(buf_size);

	if (buf == NULL) {
		printk("Could not allocate memory. Aborting sample.");
		RETURN_FROM_MAIN(1);
	}

	(void)memset(buf, 0xFFu, 7200);

    display_blanking_off(display_dev);

	buf_desc.buf_size = 7200;
	buf_desc.pitch = 60;
	buf_desc.width = 60;
	buf_desc.height = 60;

    display_write(display_dev, 0, 0, &buf_desc, buf);


    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printk);
    evm_register_file_load((intptr_t)vm_load);

    if( nevm_runtime_setup() != ec_ok ){
        printk("Failed to setup uol runtime\r\n");
        return ec_err;
    }

    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    err = ecma_module(env, 5);
    err = evm_module(env);
    err = evm_repl_run(env, 100, EVM_LANG_JS);
    return err;
}
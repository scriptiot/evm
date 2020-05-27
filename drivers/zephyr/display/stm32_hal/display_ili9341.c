#define DT_DRV_COMPAT ilitek_ili9341

#include "display_ili9341.h"
#include <drivers/display.h>

#define LOG_LEVEL CONFIG_DISPLAY_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(display_ili9341);

#include <drivers/gpio.h>
#include <sys/byteorder.h>
#include <drivers/spi.h>
#include <string.h>

struct ili9341_data {
#if DT_INST_NODE_HAS_PROP(0, reset_gpios)
	struct device *reset_gpio;
#endif
};

#define ILI9341_RGB_SIZE 2U

static SRAM_HandleTypeDef  SRAM_Handler;
static FSMC_NORSRAM_TimingTypeDef Timing;

void ili9341_write_cmd( uint16_t usCmd )
{
    * ( __IO uint16_t * ) ( ILI9341_FSMC_Addr_CMD ) = usCmd;
}

void ili9341_write_data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( ILI9341_FSMC_Addr_DATA ) = usData;
	
}

uint16_t ili9341_read_data ( void )
{
	return ( * ( __IO uint16_t * ) ( ILI9341_FSMC_Addr_DATA ) );
	
}

void  ili9341_backled_control ( int  onoff)
{
	if ( !onoff )
	{
		digitalL( GPIOG, GPIO_PIN_6);	
	}
	else
	{
		digitalH( GPIOG, GPIO_PIN_6);
	}		
}

void ili9341_reset( void )
{			
	digitalL( GPIOG,GPIO_PIN_11);

	k_sleep(K_MSEC(120));						   

	digitalH( GPIOG,GPIO_PIN_11);		 	 

	k_sleep(K_MSEC(120));	 	
	
}


static void ili9341_exit_sleep(struct ili9341_data *data)
{
	k_sleep(K_MSEC(120));
}

static int ili9341_init(struct device *dev)
{
	struct ili9341_data *data = (struct ili9341_data *)dev->driver_data;

	LOG_DBG("Initializing display driver");

    SRAM_Handler.Instance = FSMC_NORSRAM_DEVICE;
    SRAM_Handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  
    Timing.AddressSetupTime      = 0x00;
    Timing.AddressHoldTime       = 0x00;
    Timing.DataSetupTime         = 0x08;
    Timing.BusTurnAroundDuration = 0x00;
    Timing.CLKDivision           = 0x00;
    Timing.DataLatency           = 0x00;
    Timing.AccessMode            = FSMC_ACCESS_MODE_B;
 
    SRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK4;
	SRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;
	SRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_NOR;
	SRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;
	SRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE;
	SRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;
	SRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   	
	SRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;
	SRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;
	SRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;
	SRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;
	SRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;
  
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitTypeDef GPIO_Initure;
  
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_FSMC_CLK_ENABLE();
  
    GPIO_Initure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull      = GPIO_PULLUP;
    GPIO_Initure.Speed     = GPIO_SPEED_FREQ_HIGH;

    GPIO_Initure.Pin=GPIO_PIN_12;
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
  
	GPIO_Initure.Pin=GPIO_PIN_11;
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
  
    GPIO_Initure.Mode=GPIO_MODE_AF_PP; 
  
	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|\
					         GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15; 
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);
  
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_6;
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);

    HAL_SRAM_Init(& SRAM_Handler, &Timing, &Timing);


	ili9341_backled_control(1);
	ili9341_reset();

	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xCF  );
	ili9341_write_data ( 0x00  );
	ili9341_write_data ( 0x81  );
	ili9341_write_data ( 0x30  );
	
	/*  Power on sequence control (EDh) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xED );
	ili9341_write_data ( 0x64 );
	ili9341_write_data ( 0x03 );
	ili9341_write_data ( 0x12 );
	ili9341_write_data ( 0x81 );
	
	/*  Driver timing control A (E8h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xE8 );
	ili9341_write_data ( 0x85 );
	ili9341_write_data ( 0x10 );
	ili9341_write_data ( 0x78 );
	
	/*  Power control A (CBh) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xCB );
	ili9341_write_data ( 0x39 );
	ili9341_write_data ( 0x2C );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x34 );
	ili9341_write_data ( 0x02 );
	
	/* Pump ratio control (F7h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xF7 );
	ili9341_write_data ( 0x20 );
	
	/* Driver timing control B */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xEA );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x00 );
	
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xB1 );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x1B );
	
	/*  Display Function Control (B6h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xB6 );
	ili9341_write_data ( 0x0A );
	ili9341_write_data ( 0xA2 );
	
	/* Power Control 1 (C0h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xC0 );
	ili9341_write_data ( 0x35 );
	
	/* Power Control 2 (C1h) */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0xC1 );
	ili9341_write_data ( 0x11 );
	
	/* VCOM Control 1 (C5h) */
	ili9341_write_cmd ( 0xC5 );
	ili9341_write_data ( 0x45 );
	ili9341_write_data ( 0x45 );
	
	/*  VCOM Control 2 (C7h)  */
	ili9341_write_cmd ( 0xC7 );
	ili9341_write_data ( 0xA2 );
	
	/* Enable 3G (F2h) */
	ili9341_write_cmd ( 0xF2 );
	ili9341_write_data ( 0x00 );
	
	/* Gamma Set (26h) */
	ili9341_write_cmd ( 0x26 );
	ili9341_write_data ( 0x01 );
	k_sleep(K_MSEC(1));
	
	/* Positive Gamma Correction */
	ili9341_write_cmd ( 0xE0 ); //Set Gamma
	ili9341_write_data ( 0x0F );
	ili9341_write_data ( 0x26 );
	ili9341_write_data ( 0x24 );
	ili9341_write_data ( 0x0B );
	ili9341_write_data ( 0x0E );
	ili9341_write_data ( 0x09 );
	ili9341_write_data ( 0x54 );
	ili9341_write_data ( 0xA8 );
	ili9341_write_data ( 0x46 );
	ili9341_write_data ( 0x0C );
	ili9341_write_data ( 0x17 );
	ili9341_write_data ( 0x09 );
	ili9341_write_data ( 0x0F );
	ili9341_write_data ( 0x07 );
	ili9341_write_data ( 0x00 );
	
	/* Negative Gamma Correction (E1h) */
	ili9341_write_cmd ( 0XE1 ); //Set Gamma
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x19 );
	ili9341_write_data ( 0x1B );
	ili9341_write_data ( 0x04 );
	ili9341_write_data ( 0x10 );
	ili9341_write_data ( 0x07 );
	ili9341_write_data ( 0x2A );
	ili9341_write_data ( 0x47 );
	ili9341_write_data ( 0x39 );
	ili9341_write_data ( 0x03 );
	ili9341_write_data ( 0x06 );
	ili9341_write_data ( 0x06 );
	ili9341_write_data ( 0x30 );
	ili9341_write_data ( 0x38 );
	ili9341_write_data ( 0x0F );
	
	/* memory access control set */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0x36 ); 	
	ili9341_write_data ( 0xC8 );
	k_sleep(K_MSEC(1));
	
	/* column address control set */
	ili9341_write_cmd ( ILI9341_CMD_COLUMN_ADDR ); 
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0xEF );
	
	/* page address control set */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( ILI9341_CMD_PAGE_ADDR ); 
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x00 );
	ili9341_write_data ( 0x01 );
	ili9341_write_data ( 0x3F );
	
	/*  Pixel Format Set (3Ah)  */
	k_sleep(K_MSEC(1));
	ili9341_write_cmd ( 0x3a ); 
	ili9341_write_data ( 0x55 );
	
	/* Sleep Out (11h)  */
	ili9341_write_cmd ( 0x11 );	
	k_sleep(K_MSEC(120));
	
	/* Display ON (29h) */
	ili9341_write_cmd ( 0x29 ); 

	return 0;
}

static void ili9341_set_mem_area(struct ili9341_data *data, const u16_t x,
				 const u16_t y, const u16_t w, const u16_t h)
{
    ili9341_write_cmd ( ILI9341_CMD_COLUMN_ADDR );
	ili9341_write_data ( x >> 8  );
	ili9341_write_data ( x & 0xff  );
	ili9341_write_data ( ( x + w - 1 ) >> 8  );
	ili9341_write_data ( ( x + w - 1 ) & 0xff  );

	ili9341_write_cmd ( ILI9341_CMD_PAGE_ADDR );
	ili9341_write_data ( y >> 8  );
	ili9341_write_data ( y & 0xff  );
	ili9341_write_data ( ( y + h - 1 ) >> 8 );
	ili9341_write_data ( ( y + h - 1) & 0xff );
}

static int ili9341_write(const struct device *dev, const u16_t x,
			 const u16_t y,
			 const struct display_buffer_descriptor *desc,
			 const void *buf)
{
	struct ili9341_data *data = (struct ili9341_data *)dev->driver_data;
	const u8_t *write_data_start = (u8_t *) buf;

	__ASSERT(desc->width <= desc->pitch, "Pitch is smaller then width");
	__ASSERT((desc->pitch * ILI9341_RGB_SIZE * desc->height) <= desc->bu_size,
			"Input buffer too small");

	LOG_DBG("Writing %dx%d (w,h) @ %dx%d (x,y)", desc->width, desc->height,
			x, y);

	ili9341_set_mem_area(data, x, y, desc->width, desc->height);

    ili9341_write_cmd ( ILI9341_CMD_MEM_WRITE );	
	uint32_t nbr_of_points = desc->width * desc->height;	
    uint32_t pixel_index = 0;
	for (uint32_t i = 0; i < nbr_of_points; i ++ ) {
        ili9341_write_data ( write_data_start[pixel_index++] | write_data_start[pixel_index++] >> 8 );
    }
	return 0;
}

static int ili9341_read(const struct device *dev, const u16_t x,
			const u16_t y,
			const struct display_buffer_descriptor *desc,
			void *buf)
{
	LOG_ERR("Reading not supported");
	return -ENOTSUP;
}

static void *ili9341_get_framebuffer(const struct device *dev)
{
	LOG_ERR("Direct framebuffer access not supported");
	return NULL;
}

static int ili9341_display_blanking_off(const struct device *dev)
{
	struct ili9341_data *data = (struct ili9341_data *)dev->driver_data;

	LOG_DBG("Turning display blanking off");
	ili9341_write_cmd(ILI9341_CMD_DISPLAY_ON);
	return 0;
}

static int ili9341_display_blanking_on(const struct device *dev)
{
	struct ili9341_data *data = (struct ili9341_data *)dev->driver_data;

	LOG_DBG("Turning display blanking on");
	ili9341_write_cmd(ILI9341_CMD_DISPLAY_OFF);
	return 0;
}

static int ili9341_set_brightness(const struct device *dev,
				  const u8_t brightness)
{
	LOG_WRN("Set brightness not implemented");
	return -ENOTSUP;
}

static int ili9341_set_contrast(const struct device *dev, const u8_t contrast)
{
	LOG_ERR("Set contrast not supported");
	return -ENOTSUP;
}

static int ili9341_set_pixel_format(const struct device *dev,
				    const enum display_pixel_format
				    pixel_format)
{
#ifdef CONFIG_ILI9341_RGB565
	if (pixel_format == PIXEL_FORMAT_RGB_565) {
#else
	if (pixel_format == PIXEL_FORMAT_RGB_888) {
#endif
		return 0;
	}
	LOG_ERR("Pixel format change not implemented");
	return -ENOTSUP;
}

static int ili9341_set_orientation(const struct device *dev,
				   const enum display_orientation orientation)
{
	if (orientation == DISPLAY_ORIENTATION_NORMAL) {
		return 0;
	}
	LOG_ERR("Changing display orientation not implemented");
	return -ENOTSUP;
}

static void ili9341_get_capabilities(const struct device *dev,
				     struct display_capabilities *capabilities)
{
	memset(capabilities, 0, sizeof(struct display_capabilities));
	capabilities->x_resolution = 320U;
	capabilities->y_resolution = 240U;

    capabilities->supported_pixel_formats = PIXEL_FORMAT_RGB_565;
	capabilities->current_pixel_format = PIXEL_FORMAT_RGB_565;

	capabilities->current_orientation = DISPLAY_ORIENTATION_NORMAL;
}

static const struct display_driver_api ili9341_api = {
	.blanking_on = ili9341_display_blanking_on,
	.blanking_off = ili9341_display_blanking_off,
	.write = ili9341_write,
	.read = ili9341_read,
	.get_framebuffer = ili9341_get_framebuffer,
	.set_brightness = ili9341_set_brightness,
	.set_contrast = ili9341_set_contrast,
	.get_capabilities = ili9341_get_capabilities,
	.set_pixel_format = ili9341_set_pixel_format,
	.set_orientation = ili9341_set_orientation,
};

static struct ili9341_data ili9341_data;

DEVICE_AND_API_INIT(ili9341, "ilitek_ili9341", &ili9341_init,
		    &ili9341_data, NULL, APPLICATION,
		    CONFIG_APPLICATION_INIT_PRIORITY, &ili9341_api);

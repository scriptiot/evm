#include "stm32f10x.h"
#include "./sdio/bsp_sdio_sdcard.h"
#include "./usart/bsp_usart.h"
#include "./Key/bsp_key.h"
#include "./led/bsp_led.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "delay/delay.h"
#include "rtc/rtc.h"
#include "./sram/sram.h"

#include <time.h>
#include "evm.h"
#include "lvgl_main.h"

//实现time头文件
time_t time_dat;
time_t time (time_t *_timer)
{
    *_timer = time_dat;
	return time_dat;
}
//设置时间（可以通过RTC获取时间）
void set_time(){
	struct tm set_time;
	set_time.tm_sec = 10;
	set_time.tm_min = 0;
	set_time.tm_hour = 8;
	set_time.tm_mday = 13;
	set_time.tm_mon = 10-1;
	set_time.tm_year = 2020-1900;
	set_time.tm_isdst = -1;
	time_dat = mktime(&set_time);
}

//实现虚拟机文件加载
FATFS fs;
char * vm_load_file(evm_t * e, char *filename){
    FIL file;
	int size;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    FRESULT res = f_open(&file, filename, FA_READ | FA_OPEN_EXISTING);
    if( res == FR_OK ) {
		size = f_size(&file);
		UINT br;
		evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*size + 1);
		buffer = (char*)evm_buffer_addr(b);
		res = f_read(&file, buffer, size, &br);
		if( res != FR_OK || br != size){
			return NULL;
		}
		f_close(&file);
		return buffer;
	}
	return NULL;
}

//实现虚拟机模块加载
const char * vm_load(evm_t * e, char * path, int type)
{
    int file_name_len = strlen(path) + 1;
    char* buffer = NULL;
	char temp_name[128];
	sprintf(temp_name, "0:/%s", path);
    if(type == EVM_LOAD_MAIN){ //main文件加载路径
        sprintf(e->file_name, "%s", path);
        buffer = vm_load_file(e, temp_name);
    } else { //模块加载路径
        sprintf(e->file_name, "%s", path);
        buffer = vm_load_file(e, temp_name);
    }
    return buffer;
}

//实现虚拟机内存分配malloc接口
void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}
//实现虚拟机内存分配free接口
void vm_free(void * mem)
{
    if(mem) free(mem);
}

#define VM_HEAP_SIZE	(80 * 1024)
#define VM_STACK_SIZE	(10 * 1024)
#define VM_MODULE_SIZE	5

extern int qml_lvgl_module(evm_t * e);

int main(void)
{
	FSMC_SRAM_Init();
	NVIC_Configuration();
	delay_init();
	Key_GPIO_Config();
	LED_GPIO_Config();
	
	
	
	DEBUG_USART_Config();
	
	
	
	Set_System();
	
	if( Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON ){
		LED1_ON;
		Set_USBClock();
		USB_Interrupts_Config();
		USB_Init();
		
		while (bDeviceState != CONFIGURED);
	   
		while (1)
		{
			
		}
	}
	
	ILI9341_Init();
	XPT2046_Init();
	
	RTC_Init();
	
	set_time();
	lvgl_main();
	
	evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);
	
	
	FRESULT res_sd = f_mount(&fs,"0:",1);
	
	//创建虚拟机
	evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
	//虚拟机初始化
    int err = evm_init(env, VM_HEAP_SIZE, VM_STACK_SIZE, VM_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
	if( err != ec_ok ) {evm_errcode_print(env);return err;}
    //加载ecma模块
	ecma_module(env, 5);
    if(env->err) {evm_errcode_print(env);return err;}
	//加载littlevgl qml模块
    err = qml_lvgl_module(env);
    if(err) {evm_errcode_print(env);return err;}
    //加载main.qml文件
	err = evm_boot(env, "main.qml");
	if(err) {evm_errcode_print(env);return err;}
	
 	
	//虚拟机启动
	err = evm_start(env);
	
	//进入lvgl事件循环
	lvgl_loop(env);
 
 	while (bDeviceState != CONFIGURED);
	   
	while (1)
	{
		
	}
}

/* -------------------------------------end of file -------------------------------------------- */

/**
******************************************************************************
* @file    			w5500_conf.c                                                                                              
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				用3.5.0版本库建的工程模板						  	
* @attention		配置MCU，移植w5500程序需要修改的文件，配置w5500的MAC和IP地址 	
*				
* @company  		深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格绝对优势！
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/

#include <stdio.h> 
#include <string.h>
#include "w5500_conf.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"


CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*MAC地址首字节必须为偶数*/
/*如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*定义默认IP信息*/
uint8 local_ip[4]={192,168,1,25};				/*定义W5500默认IP地址*/
uint8 subnet[4]={255,255,255,0};				/*定义W5500默认子网掩码*/
uint8 gateway[4]={192,168,1,1};					/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};			/*定义W5500默认DNS*/

uint16 local_port=8080;	                    	/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,1,160};			/*远端IP地址*/
uint16 remote_port=8080;						/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8  ip_from=IP_FROM_DEFINE;				

uint8  dhcp_ok=0;								/*dhcp成功获取IP*/
uint32 ms=0;									/*毫秒计数*/
uint32 dhcp_time= 0;							/*DHCP运行计数*/
volatile unsigned char    ntptimer = 0;							/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{	
		
  /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	// if(ip_from==IP_FROM_DEFINE)	
	// 	printf(" 使用定义的IP信息配置W5500:\r\n");

	// /*使用DHCP获取IP参数，需调用DHCP子函数*/		
	// if(ip_from==IP_FROM_DHCP)								
	// {
	// 	/*复制DHCP获取的配置信息到配置结构体*/
	// 	if(dhcp_ok==1)
	// 	{
	// 		// printf(" IP from DHCP\r\n");		 
	// 		memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
	// 		memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
	// 		memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
	// 		memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
	// 	}
	// 	else
	// 	{
	// 		// printf(" DHCP子程序未运行,或者不成功\r\n");
	// 		// printf(" 使用定义的IP信息配置W5500\r\n");
	// 	}
	// }
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	// printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	// printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	// printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
	// GPIO_InitTypeDef GPIO_InitStructure;
	// SPI_FLASH_Init();																		 /*初始化STM32 SPI接口*/
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG, ENABLE);
		
	// /*定义RESET引脚*/	
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;						 /*选择要控制的GPIO引脚*/		 
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz*/		
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*设置引脚模式为通用推挽输出*/		
	// GPIO_Init(GPIOB, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
	// GPIO_SetBits(GPIOB, GPIO_Pin_1);		
	// /*定义INT引脚*/
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;						 /*选择要控制的GPIO引脚*/		 
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz */		
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*设置引脚模式为通用推挽模拟上拉输入*/	
	// GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	#ifdef  STM32_SPI1
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4); 
		}
	#endif
	#ifdef STM32_SPI2
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_12); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_12); 
		}
	#endif
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
// void reset_w5500(void)
// {
	// GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	// w5500_delay_us(50);  
	// GPIO_SetBits(GPIOB, GPIO_Pin_1);
	// w5500_delay_us(50);
// }


/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer2_init(void)
{
	TIM2_Configuration();																		/* TIM2 定时配置 */
	TIM2_NVIC_Configuration();															/* 定时器的中断优先级 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 重新开时钟，开始计时 */
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
	timer2_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
	timer2_init();																	
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
	ms++;	
	if(ms>=1000)
	{  
		ms=0;
		dhcp_time++;			/*DHCP定时加1S*/
		#ifndef	__NTP_H__
		ntptimer++;				/*NTP重试时间加1S*/
		#endif
	}

}
/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	printf(" 系统重启中……\r\n");
	JumpAddress = *(vu32*) (0x00000004);
	Jump_To_Application = (pFunction) JumpAddress;
	Jump_To_Application();
}




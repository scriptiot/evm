/**
******************************************************************************
* @file    			w5500.c                                               
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				读写W5500寄存器底层函数           
*
* @company  		深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格绝对优势！
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/
#define DT_DRV_COMPAT wiznet_w5500

#include <stdio.h>
#include <string.h>
#include "w5500_conf.h"
#include "w5500.h"
// #include "socket.h"
#include "utility.h"
#ifdef   __DEF_IINCHIP_PPP__
#include "md5.h"
#endif

#include <net/net_pkt.h>
#include <net/net_if.h>
#include <net/ethernet.h>
// #include <ethernet/eth_stats.h>



uint16 SSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0}; // Max Tx buffer
uint16 RSIZE[MAX_SOCK_NUM] = {0,0,0,0,0,0,0,0}; // Max Rx buffer
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2}; // tx buffer set K bytes
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2}; // rx buffet set K bytes

#define	digitalH(p,i)			{p->BSRR=i;}					
#define digitalL(p,i)			{p->BSRR=(uint32_t)i << 16;}
/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
		digitalH(GPIOA, GPIO_PIN_4); 
	}
	else if (val == HIGH)
	{
		digitalL(GPIOA, GPIO_PIN_4); 
	}
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
void reset_w5500(void)
{
	digitalL(GPIOB, GPIO_PIN_1);
	w5500_delay_us(50);  
	digitalH(GPIOB, GPIO_PIN_1);
	w5500_delay_us(50);
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(struct device *dev,uint8 dat)
{
  	struct w5500_runtime *context = dev->driver_data;
	  
  	u8_t buf[1] = {dat};
	const struct spi_buf tx_buf = {
		.buf = buf,
		.len = sizeof(buf)
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};

	return(spi_write(context->spi, &context->spi_cfg, &tx));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE(struct device *dev, uint32 addrbsb,  uint8 data)
{
	iinchip_csoff();                              		
	IINCHIP_SpiSendData(dev, (addrbsb & 0x00FF0000)>>16);	
	IINCHIP_SpiSendData(dev, (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x000000F8) + 4);  
	IINCHIP_SpiSendData(dev, data);                   
	iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(struct device *dev,uint32 addrbsb)
{
	uint8 data = 0;
	iinchip_csoff();                            
	IINCHIP_SpiSendData(dev, (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x000000F8))    ;
	data = IINCHIP_SpiSendData(dev, 0x00);            
	iinchip_cson();                               
	return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   	buf：写入字符串
*@param   	len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(struct device *dev, uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	// if(len == 0) printf(" Unexpected2 length 0\r\n");
	iinchip_csoff();                               
	IINCHIP_SpiSendData(dev, (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
		IINCHIP_SpiSendData(dev, buf[idx]);
	}
	iinchip_cson();                           
	return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(struct device *dev,uint32 addrbsb, uint8* buf,uint16 len)
{
	uint16 idx = 0;
	// if(len == 0)
	// {
	// 	printf(" Unexpected2 length 0\r\n");
	// }
	iinchip_csoff();                                
	IINCHIP_SpiSendData(dev, (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData(dev, (addrbsb & 0x000000F8));    
	for(idx = 0; idx < len; idx++)                   
	{
		buf[idx] = IINCHIP_SpiSendData(dev, 0x00);
	}
	iinchip_cson();                                  
	return len;
}

/**
*@brief	  This function is to get the Max size to receive.
*@param	  s: socket number
*@return	This function return the RX buffer size has been setted
*/
void iinchip_init(struct device *dev)
{
  setMR(dev, MR_RST);
  #ifdef __DEF_IINCHIP_DBG__
    printf(" MR value is %02x \r\n",IINCHIP_READ_COMMON(MR));
  #endif
}

/**
*@brief	  This function is to get the Max size to receive.
*@param	  s: socket number
*@return	This function return the RX buffer size has been setted
*/
uint16 getIINCHIP_RxMAX(SOCKET s)
{
	return RSIZE[s];
}	

/**
*@brief	  This function is to get the Max size to receive.
*@param	  s: socket number
*@return	This function return the RX buffer size has been setted
*/
uint16 getIINCHIP_TxMAX(SOCKET s)
{
	return SSIZE[s];
}

/**
*@brief	  This function is to set up gateway IP address.
*@param	  addr: a pointer to a 4 -byte array responsible to set the Gateway IP address 
*@return	None
*/
void setGAR(struct device *dev, uint8 * addr)
{
	wiz_write_buf(dev,GAR0, addr, 4);
}

/**
*@brief  	This function is to get gateway IP address.
*@param	  addr: a pointer to a 4 -byte array responsible to get the Gateway IP address  
*@return	None
*/
void getGAR(struct device *dev,uint8 * addr)
{
	wiz_read_buf(dev, GAR0, addr, 4);
}

/**
*@brief 	This function is to set up SubnetMask address
*@param	  addr: a pointer to a 4 -byte array responsible to set the subway IP address.  
*@return	None
*/
void setSUBR(struct device *dev,uint8 * addr)
{   
	wiz_write_buf(dev, SUBR0, addr, 4);
}
/**
*@brief	  This function is to set up MAC address.
*@param	  addr: a pointer to a 6 -byte array responsible to set the MAC address.  
*@return	None
*/
void setSHAR(struct device *dev,uint8 * addr)
{
	wiz_write_buf(dev,SHAR0, addr, 6);  
}

/**
*@brief	  This function is to set up Source IP address.
*@param 	addr:a pointer to a 4 -byte array responsible to set the Source IP addres.  
*@return	None
*/
void setSIPR(struct device *dev,uint8 * addr)
{
	wiz_write_buf(dev,SIPR0, addr, 4);  
}

/**
*@brief	  This function is to get Subnet mask.
*@param	  addr:a pointer to a 4 -byte array responsible to set the Subnet mask.  
*@return	None
*/
void getSUBR(struct device *dev,uint8 * addr)
{
	wiz_read_buf(dev,SUBR0, addr, 4);
}

/**
*@brief		This function is to get up Source MAC .
*@param		addr: a pointer to a 6 -byte array responsible to get the MAC  
*@return	None
*/
void getSHAR(struct device *dev,uint8 * addr)
{
	wiz_read_buf(dev, SHAR0, addr, 6);
}

/**
*@brief		This function is to get up Source IP .
*@param		addr: a pointer to a 4 -byte array responsible to get the Source IP  
*@return	None
*/
void getSIPR(struct device *dev, uint8 * addr)
{
	wiz_read_buf(dev, SIPR0, addr, 4);
}
/**
*@brief		This function is to set the MR register.
*@param		val: the value to set to MR  
*@return	None
*/
void setMR(struct device *dev,uint8 val)
{
	IINCHIP_WRITE(dev,MR,val);
}

/**
*@brief		This function is to get Interrupt register in common register.
*@param		None  
*@return	The value read from the IR register
*/
uint8 getIR(struct device *dev )
{
	return IINCHIP_READ(dev,IR);
}

/**
@brief		This function is to set up Retransmission time.
*					If there is no response from the peer or delay in response then retransmission
*					will be there as per RTR (Retry Time-value Register)setting
*@param		timeout: The value write to  the RTR0 register 
*@return	None
*/
void setRTR(struct device *dev,uint16 timeout)
{
	IINCHIP_WRITE(dev,RTR0,(uint8)((timeout & 0xff00) >> 8));
  	IINCHIP_WRITE(dev,RTR1,(uint8)(timeout & 0x00ff));
}

/**
@brief		This function is to set the number of Retransmission.
*					If there is no response from the peer or delay in response then recorded time
*					as per RTR & RCR register seeting then time out will occur.
*@param		retry: Times to  retry 
*@return	None
*/
void setRCR(struct device *dev,uint8 retry)
{
  IINCHIP_WRITE(dev,WIZ_RCR,retry);
}

/**
*@brief		This function is to the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)
*					If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
*					set in IR register.
*@param		mask: the bits to clear
*@return	None
*/
void clearIR(struct device *dev,uint8 mask)
{
  IINCHIP_WRITE(dev, IR, ~mask | getIR(dev) ); 
}

/**
*@brief  	This function is to set the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*@param		s: socket number
*@param		Sn_MSSR: the maximum segment size
*@return	None
*/
void setSn_MSS(struct device *dev,SOCKET s, uint16 Sn_MSSR)
{
  IINCHIP_WRITE(dev, Sn_MSSR0(s), (uint8)((Sn_MSSR & 0xff00) >> 8));
  IINCHIP_WRITE(dev, Sn_MSSR1(s), (uint8)(Sn_MSSR & 0x00ff));
}

/**
*@brief  	This function is to set the IP Time to live(TTL) Register
*@param		s: socket number
*@param		Sn_MSSR: the IP Time to live
*@return	None
*/
void setSn_TTL(struct device *dev,SOCKET s, uint8 ttl)
{    
	IINCHIP_WRITE(dev, Sn_TTL(s) , ttl);
}

/**
*@brief		This function is to read the Interrupt & Soket Status registe
*@param		s: socket number
*@return	socket interrupt status
*/
uint8 getSn_IR(struct device *dev,SOCKET s)
{
	return IINCHIP_READ(dev,Sn_IR(s));
}

/**
*@brief 	This function is to write the Interrupt & Soket Status register to clear the interrupt
*@param		s: socket number
*@return  socket interrupt status
*/
void setSn_IR(struct device *dev,uint8 s, uint8 val)
{
	IINCHIP_WRITE(dev, Sn_IR(s), val);
}

/**
*@brief 	This function is to get socket status
*@param		s: socket number
*@return  socket status
*/
uint8 getSn_SR(struct device *dev,SOCKET s)
{
	return IINCHIP_READ(dev,Sn_SR(s));
}

/**
*@brief		This fuction is to get socket TX free buf size
*					This gives free buffer size of transmit buffer. This is the data size that user can transmit.
*					User shuold check this value first and control the size of transmitting data
*@param		s: socket number
*@return  socket TX free buf size
*/
uint16 getSn_TX_FSR(struct device *dev,SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(dev,Sn_TX_FSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(dev,Sn_TX_FSR1(s));
    if (val1 != 0)
    {
      val = IINCHIP_READ(dev,Sn_TX_FSR0(s));
      val = (val << 8) + IINCHIP_READ(dev,Sn_TX_FSR1(s));
    }
  } while (val != val1);
   return val;
}

/**
*@brief		This fuction is to give size of received data in receive buffer.
*@param		s: socket number
*@return  socket TX free buf size
*/
uint16 getSn_RX_RSR(struct device *dev,SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(dev,Sn_RX_RSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(dev,Sn_RX_RSR1(s));
    if(val1 != 0)
    {
			val = IINCHIP_READ(dev,Sn_RX_RSR0(s));
			val = (val << 8) + IINCHIP_READ(dev,Sn_RX_RSR1(s));
    }
  } while (val != val1);
   return val;
}

/**
*@brief   This function is being called by send() and sendto() function also.
*					This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
*				  register. User should read upper byte first and lower byte later to get proper value.
*@param		s: socket number
*@param		data: data buffer to send
*@param		len: data length
*@return  socket TX free buf size
*/
void send_data_processing(struct device *dev,SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr =0;
  uint32 addrbsb =0;
  if(len == 0)
  {
    // printf(" CH: %d Unexpected1 length 0\r\n", s);
    return;
  }
   
  ptr = IINCHIP_READ(dev, Sn_TX_WR0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(dev,Sn_TX_WR1(s));

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
  wiz_write_buf(dev,addrbsb, data, len);
  
  ptr += len;
  IINCHIP_WRITE(dev, Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE(dev, Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));
}

/**
*@brief  	This function is being called by recv() also.
*					This function read the Rx read pointer register
*					and after copy the data from receive buffer update the Rx write pointer register.
*					User should read upper byte first and lower byte later to get proper value.
*@param		s: socket number
*@param		data: data buffer to receive
*@param		len: data length
*@return  None
*/
void recv_data_processing(struct device *dev,SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr = 0;
  uint32 addrbsb = 0;
  
  if(len == 0)
  {
    // printf(" CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ(dev, Sn_RX_RD0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(dev, Sn_RX_RD1(s) );

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
  wiz_read_buf(dev,addrbsb, data, len);
  ptr += len;

  IINCHIP_WRITE(dev, Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE(dev, Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
}

/**
*@brief		This function set the transmit & receive buffer size as per the channels is used
*@Note: 	TMSR and RMSR bits are as follows\n
*					Maximum memory size for Tx, Rx in the W5500 is 16K Bytes,\n
*					In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
*					Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
*					and to data transmission and receiption from non-allocated channel may cause some problems.\n
*					If the 16KBytes memory is already  assigned to centain channel, \n
*					other 3 channels couldn't be used, for there's no available memory.\n
* 				If two 4KBytes memory are assigned to two each channels, \n
*					other 2 channels couldn't be used, for there's no available memory.\n
*@param		tx_size: tx buffer size to set=tx_size[s]*(1024)
*@param		rx_size: rx buffer size to set=rx_size[s]*(1024)
*@return	None
*/
void socket_buf_init(struct device *dev, uint8 * tx_size, uint8 * rx_size  )
{
	int16 i;
	int16 ssum=0,rsum=0;

	for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		IINCHIP_WRITE(dev, (Sn_TXMEM_SIZE(i)), tx_size[i]);
		IINCHIP_WRITE(dev, (Sn_RXMEM_SIZE(i)), rx_size[i]);

		#ifdef __DEF_IINCHIP_DBG__
			printf(" tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
			printf(" rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
		#endif
		SSIZE[i] = (int16)(0);
		RSIZE[i] = (int16)(0);

		if (ssum <= 16384)
		{
			SSIZE[i] = (int16)tx_size[i]*(1024);
		}

		if (rsum <= 16384)
		{
			RSIZE[i]=(int16)rx_size[i]*(1024);
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];
	}
}

/**
*@brief		检测物理层连接
*@param		无
*@return	无
*/
void PHY_check(struct device *dev)
{
	uint8 PHY_connect=0;  
	PHY_connect=0x01&getPHYStatus(dev);
	if(PHY_connect==0)
	{
		// printf(" \r\n 请检查网线是否连接?\r\n");
		while(PHY_connect == 0) { 
			PHY_connect=0x01 & getPHYStatus(dev);	
			// printf(" .");
			w5500_delay_ms(500);
		}
		// printf(" \r\n");	
	}
}

uint8 getPHYStatus(struct device *dev)
{
	return IINCHIP_READ(dev,PHYCFGR);
}
static int w5500_init(struct device *dev)
{
	const struct w5500_config *config = dev->config_info;
	struct w5500_runtime *context = dev->driver_data;
	u8_t retries = 3;
	u16_t tmp;

	/* SPI config */
	context->spi_cfg.operation = SPI_WORD_SET(8);
	context->spi_cfg.frequency = config->spi_freq;
	context->spi_cfg.slave = config->spi_slave;

	context->spi = device_get_binding((char *)config->spi_port);
	if (!context->spi) {
		return -EINVAL;
	}

#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	context->spi_cs.gpio_dev =
		device_get_binding((char *)config->spi_cs_port);
	if (!context->spi_cs.gpio_dev) {
		return -EINVAL;
	}

	context->spi_cs.gpio_pin = config->spi_cs_pin;
	context->spi_cfg.cs = &context->spi_cs;
#endif
	/* Start interruption-poll thread */
	k_thread_create(&context->thread, context->thread_stack,
			800,
			(k_thread_entry_t)2,
			(void *)dev, NULL, NULL,
			K_PRIO_COOP(2),
			0, K_NO_WAIT);

	context->suspended = false;

	return 0;
}


static struct w5500_runtime w5500_0_runtime = {
	.tx_rx_sem = Z_SEM_INITIALIZER(w5500_0_runtime.tx_rx_sem,
				       1,  UINT_MAX),
	.int_sem  = Z_SEM_INITIALIZER(w5500_0_runtime.int_sem,
				      0, UINT_MAX),
};

static const struct w5500_config w5500_0_config = {
	.spi_port = DT_INST_BUS_LABEL(0),
	.spi_freq  = DT_INST_PROP(0, spi_max_frequency),
	.spi_slave = DT_INST_REG_ADDR(0),
#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	.spi_cs_port = DT_INST_SPI_DEV_CS_GPIOS_LABEL(0),
	.spi_cs_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(0),
#endif
	.timeout = 100,
};

ETH_NET_DEVICE_INIT(w5500_0, DT_INST_LABEL(0),
		    w5500_init, device_pm_control_nop,
		    &w5500_0_runtime, &w5500_0_config,
		    CONFIG_ETH_INIT_PRIORITY, &api_funcs, NET_ETH_MTU);



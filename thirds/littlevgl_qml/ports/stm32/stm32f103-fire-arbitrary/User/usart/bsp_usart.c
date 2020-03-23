#include "./usart/bsp_usart.h"

#define  DEBUG_USARTx                   USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

#define  DEBUG_USART_IRQ                USART1_IRQn
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler

void DEBUG_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);	
	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	/* 发送一个字节数据到串口 */
	USART_SendData(DEBUG_USARTx, (uint8_t) ch);
	
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

	return (ch);
}

int fgetc(FILE *stream){
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(DEBUG_USARTx);
}

_ttywrch(int ch)
{
ch = ch;
}
#endif  

uart_buffer_t uart1_buffer;
uart_buffer_t uart2_buffer;
uart_buffer_t uart3_buffer;
uart_buffer_t uart4_buffer;
uart_buffer_t uart5_buffer;

void DEBUG_USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USART1,USART_IT_RXNE)!=RESET){		
		ucTemp = USART_ReceiveData(DEBUG_USART1);
		uart_write_buffer(&uart1_buffer, ucTemp);
	}	 
}

void DEBUG_USART2_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USART2,USART_IT_RXNE)!=RESET){		
		ucTemp = USART_ReceiveData(DEBUG_USART2);
		uart_write_buffer(&uart2_buffer, ucTemp);
	}	 
}


void DEBUG_UART4_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USART4,USART_IT_RXNE)!=RESET){		
		ucTemp = USART_ReceiveData(DEBUG_USART4);
		uart_write_buffer(&uart4_buffer, ucTemp);
	}	 
}

void DEBUG_USART5_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USART5,USART_IT_RXNE)!=RESET){		
		ucTemp = USART_ReceiveData(DEBUG_USART5);
		uart_write_buffer(&uart5_buffer, ucTemp);
	}	 
}

int uart_write_buffer(uart_buffer_t * buffer, uint8_t data){
	if( buffer->len >= UART_BUFFER_LEN){
		return 0;
	}
	buffer->buf[buffer->tail] = data;
	buffer->tail = (buffer->tail + 1) % UART_BUFFER_LEN;
	buffer->len++;
	return 1;
}

int uart_read_buffer(uart_buffer_t * buffer, uint8_t * data){
	if( buffer->len == 0){
		return 0;
	}
	*data = buffer->buf[buffer->head];
	buffer->head = (buffer->head + 1) % UART_BUFFER_LEN;
	buffer->len--;
	return 1;
}

int uart_read(int port, uint8_t * buf, int len){
	int count = 0;
	uart_buffer_t * buffer;
	switch(port){
		case 1:buffer = &uart1_buffer;break;
		case 2:buffer = &uart2_buffer;break;
		case 3:buffer = &uart3_buffer;break;
		case 4:buffer = &uart4_buffer;break;
		case 5:buffer = &uart5_buffer;break;
		default:return count;
	}
	uint8_t ch;
	int i;
	for(i = 0; i < len; i++){
		if( uart_read_buffer(buffer, &ch) ){
			count++;
			buf[i] = ch;
		}
	}
	return count;
}

void uart_write(int port, uint8_t * buf, int len){
	USART_TypeDef * usart_port;
	switch(port){
		case 1:usart_port = DEBUG_USART1;break;
		case 2:usart_port = DEBUG_USART2;break;
		case 3:usart_port = DEBUG_USART3;;break;
		case 4:usart_port = DEBUG_USART4;break;
		case 5:usart_port = DEBUG_USART5;break;
		default:return;
	}
	int i;
	for(i = 0; i < len; i++){
		USART_SendData(usart_port, buf[i]);
		while (USART_GetFlagStatus(usart_port, USART_FLAG_TXE) == RESET);	
	}
}

int uart_any(int port){
	switch(port){
		case 1:return uart1_buffer.len;
		case 2:return uart2_buffer.len;
		case 3:return uart3_buffer.len;
		case 4:return uart4_buffer.len;
		case 5:return uart5_buffer.len;
		default:return 0;
	}
}

void uart_deinit(int port){
	USART_TypeDef * usart_port;
	switch(port){
		case 1:usart_port = DEBUG_USART1;break;
		case 2:usart_port = DEBUG_USART2;break;
		case 3:usart_port = DEBUG_USART3;;break;
		case 4:usart_port = DEBUG_USART4;break;
		case 5:usart_port = DEBUG_USART5;break;
		default:return;
	}
	USART_DeInit(usart_port);
}

int uart_init(int port, int baudrate, int databits, float stopbits, char * parity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = baudrate;
	// 配置 针数据字长
	switch(databits){
		case 8:{
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		}break;
		case 9:{
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		}break;
		default:return 0;
	}
	// 配置停止位
	if(stopbits == 1) USART_InitStructure.USART_StopBits = USART_StopBits_1;
	else if(stopbits == 0.5) USART_InitStructure.USART_StopBits = USART_StopBits_0_5;
	else if(stopbits == 2) USART_InitStructure.USART_StopBits = USART_StopBits_2;
	else if(stopbits == 1.5) USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
	else return 0;
	// 配置校验位
	if( !strcmp(parity, "none") ) USART_InitStructure.USART_Parity = USART_Parity_No;
	else if( !strcmp(parity, "even") ) USART_InitStructure.USART_Parity = USART_Parity_Even; 
	else if( !strcmp(parity, "odd") ) USART_InitStructure.USART_Parity = USART_Parity_Odd; 
	else return 0;
	
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	switch(port){
		case 1:{
			DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);
			DEBUG_USART1_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);
			
			//将USART Tx的GPIO配置为推挽复用模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

			//将USART Rx的GPIO配置为浮空输入模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);
			
			// 完成串口的初始化配置
			USART_Init(DEBUG_USART1, &USART_InitStructure);
			// 使能串口
			USART_Cmd(DEBUG_USART1, ENABLE);
		}break;
		case 2:{
			DEBUG_USART2_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);
			DEBUG_USART2_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);
			
			//将USART Tx的GPIO配置为推挽复用模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

			//将USART Rx的GPIO配置为浮空输入模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
			
			// 完成串口的初始化配置
			USART_Init(DEBUG_USART2, &USART_InitStructure);
			// 使能串口
			USART_Cmd(DEBUG_USART2, ENABLE);
		}break;
		case 3:{
			DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);
			DEBUG_USART1_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);
			
			//将USART Tx的GPIO配置为推挽复用模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

			//将USART Rx的GPIO配置为浮空输入模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);
			
			// 完成串口的初始化配置
			USART_Init(DEBUG_USART3, &USART_InitStructure);
			// 使能串口
			USART_Cmd(DEBUG_USART3, ENABLE);
		}break;
		case 4:{
			DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART4_GPIO_CLK, ENABLE);
			DEBUG_USART1_APBxClkCmd(DEBUG_USART4_CLK, ENABLE);
			
			//将USART Tx的GPIO配置为推挽复用模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART4_TX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(DEBUG_USART4_TX_GPIO_PORT, &GPIO_InitStructure);

			//将USART Rx的GPIO配置为浮空输入模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART4_RX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(DEBUG_USART4_RX_GPIO_PORT, &GPIO_InitStructure);
			
			// 完成串口的初始化配置
			USART_Init(DEBUG_USART4, &USART_InitStructure);
			// 使能串口
			USART_Cmd(DEBUG_USART4, ENABLE);
		}break;
		case 5:{
			DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART5_GPIO_CLK, ENABLE);
			DEBUG_USART1_APBxClkCmd(DEBUG_USART5_CLK, ENABLE);
			
			//将USART Tx的GPIO配置为推挽复用模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART5_TX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(DEBUG_USART5_TX_GPIO_PORT, &GPIO_InitStructure);

			//将USART Rx的GPIO配置为浮空输入模式
			GPIO_InitStructure.GPIO_Pin = DEBUG_USART5_RX_GPIO_PIN;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(DEBUG_USART5_RX_GPIO_PORT, &GPIO_InitStructure);
			
			// 完成串口的初始化配置
			USART_Init(DEBUG_USART5, &USART_InitStructure);
			// 使能串口
			USART_Cmd(DEBUG_USART5, ENABLE);
		}break;
		default:return 0;
	}
	return 1;	    
}


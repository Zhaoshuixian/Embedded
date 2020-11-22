
#include "usart.h"
#include <stdio.h>

unsigned char UART1_Frame_Flag=0;
unsigned short int UART1_RLen;
unsigned char UART1_RBuf[BUFF1_MAX_LEN];

unsigned char UART2_Frame_Flag=0;
unsigned short int UART2_RLen;
unsigned char UART2_RBuf[BUFF2_MAX_LEN];


//重定义fputc函数 
int fputc(int ch, FILE *f)
{
/* 将Printf内容发往串口 */
  USART_SendData(USART3, (unsigned char) ch);
  while (USART_GetFlagStatus(USART3, USART_FLAG_TDE) == RESET); 
  return (ch);
}

void uart_init(unsigned char Port,unsigned long Baud)
{
//GPIO端口设置
 	GPIO_InitType GPIO_InitStructure;
	USART_InitType USART_InitStructure;
	NVIC_InitType NVIC_InitStructure;
	if(1==Port)	//串口1初始化
	{
	  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1,ENABLE);

		//USART1_TX   PA.9  
		GPIO_InitStructure.GPIO_Pins 	= GPIO_Pins_9;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP ;   
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);			
		//USART1_RX	  PA.10
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
		//USART1_TX  ENABLE   PA.8
		GPIO_InitStructure.GPIO_Pins = MODBUS_USART_EN_PIN;				 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_InitStructure);  
		MODBUS_TX_EN(0);
		Rs485RxEnable;
		//Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//选择USART1中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;	//主优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//从优先级为3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

		//USART 初始化设置
		USART_InitStructure.USART_BaudRate = Baud;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //数据位8位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //停止位1位
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //无校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //发送或者接收
		USART_Init(USART1, &USART_InitStructure);	
		
		USART_INTConfig(USART1, USART_INT_IDLEF, ENABLE);//开启接收中断
		USART_INTConfig(USART1, USART_INT_RDNE, ENABLE);//开启接收中断
 
		USART_Cmd(USART1, ENABLE);                    //使能串口
    USART_ClearFlag(USART1, USART_FLAG_TRAC);//清发送完成标志位			
	 //RS485等待接收数据					 
	}
	else if(2==Port)
	{
	  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2,ENABLE);
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				 //推挽复用输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);  

		//Usart1 NVIC 配置	
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//选择USART1中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;	//主优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//从优先级为3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1  
		//USART 初始化设置

		USART_InitStructure.USART_BaudRate = Baud;//一般设置为9600;
//		USART_InitStructure.USART_BaudRate = 9600;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //数据位8位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //停止位1位
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //无校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //发送或者接收
		USART_Init(USART2, &USART_InitStructure);
		
	  USART_INTConfig(USART2, USART_INT_IDLEF, ENABLE);//开启接收中断
		USART_INTConfig(USART2, USART_INT_RDNE, ENABLE);//开启接收中断
		USART_Cmd(USART2, ENABLE);                    //使能串口	
		
    USART_ClearFlag(USART2, USART_FLAG_TRAC);//清发送完成标志位			
	}
	else if(3==Port)
	{
	  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3,ENABLE);
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				 //推挽复用输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);  

		//Usart1 NVIC 配置	
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//选择USART1中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;	//主优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//从优先级为3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1  
		//USART 初始化设置

		USART_InitStructure.USART_BaudRate = 9600;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //数据位8位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //停止位1位
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //无校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
		USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;	  //发送或者接收
		USART_Init(USART3, &USART_InitStructure);
		
//	  USART_INTConfig(USART3, USART_INT_IDLEF, ENABLE);//开启接收中断
//		USART_INTConfig(USART3, USART_INT_RDNE, ENABLE);//开启接收中断
		USART_Cmd(USART3, ENABLE);                    //使能串口	
		
    USART_ClearFlag(USART3, USART_FLAG_TRAC);//清发送完成标志位			
	}	
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
 // 捕获溢出错误中断.
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORERR) != RESET)//注意！不能使用if(USART_GetITStatus(USART1, USART_FLAG_RDNE) != RESET)来判断
	{
		USART_ClearFlag(USART1,USART_FLAG_ORERR);        //读SR
		USART_ReceiveData(USART1); // 读取接收到的数据
	}
		//  正常接收串口中断.
	if(USART_GetITStatus(USART1, USART_INT_RDNE) != RESET)//接收中断
	{
		USART_ClearITPendingBit(USART1, USART_FLAG_RDNE);// 清除中断标示.
		if(BUFF1_MAX_LEN>UART1_RLen)
		{
		  UART1_RBuf[UART1_RLen++] = USART_ReceiveData(USART1);// 读取接收到的数据				
		}
	}
	if(USART_GetITStatus(USART1, USART_INT_IDLEF) != RESET)//接受中断
	{
		//STM32F1系列清除IDLE标志必须先读SR DR寄存器
		UART1_Frame_Flag=1;
		USART_ReceiveData(USART1); // 读取接收到的数据	
		USART_ClearFlag(USART1, USART_FLAG_IDLEF);//清中断标志
	}		
}
void USART2_IRQHandler(void)                	//串口2中断服务程序
{

 // 捕获溢出错误中断.
	if (USART_GetFlagStatus(USART2, USART_FLAG_ORERR) != RESET)//注意！不能使用if(USART_GetITStatus(USART1, USART_FLAG_RDNE) != RESET)来判断
	{
		USART_ClearFlag(USART2,USART_FLAG_ORERR);        //读SR
		USART_ReceiveData(USART2); // 读取接收到的数据
	}
	//  正常接收串口中断.
	if(USART_GetITStatus(USART2, USART_INT_RDNE) != RESET)//接收中断
	{
		USART_ClearITPendingBit(USART2, USART_FLAG_RDNE);// 清除中断标示.
		if(BUFF2_MAX_LEN>UART2_RLen)
		{
		  UART2_RBuf[UART2_RLen++] = USART_ReceiveData(USART2);// 读取接收到的数据	
		}		
	}
	if(USART_GetITStatus(USART2,USART_INT_IDLEF) != RESET)//接受中断
	{
		//STM32F1系列清除IDLE标志必须先读SR DR寄存器
		UART1_Frame_Flag=2;
    USART_ReceiveData(USART2); // 读取接收到的数据	
		USART_ClearFlag(USART2, USART_FLAG_IDLEF);//清中断标志
	}		
}

void enable_uart1_port(unsigned char x)
{
	(x)?MODBUS_TX_EN(1):(MODBUS_TX_EN(0));
}

void Uart1_SendByte(uint8_t byte)
{
	USART_SendData(USART1,byte);	
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == SET)); //查询发送FIFO是否发送完成
	USART_ClearFlag(USART1, USART_FLAG_TRAC);//清除传输完成标志位
}

void Uart2_SendByte(uint8_t byte)
{
	USART_SendData(USART2,byte);	
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TRAC) == SET)); //查询发送FIFO是否发送完成
	USART_ClearFlag(USART2, USART_FLAG_TRAC);//清除传输完成标志位
}

void Uart3_SendByte(uint8_t byte)
{
	USART_SendData(USART3,byte);	
	while(!(USART_GetFlagStatus(USART3, USART_FLAG_TRAC) == SET)); //查询发送FIFO是否发送完成
	USART_ClearFlag(USART3, USART_FLAG_TRAC);//清除传输完成标志位
}

void Uart1_SendData(uint8_t *data ,unsigned short int len)
{
	for(unsigned short int i=0;i<len;i++)
	{
		Uart1_SendByte(*data);
		data++;	
	}
}
void Uart2_SendData(uint8_t *data ,unsigned short int len)
{
	for(unsigned short int i=0;i<len;i++)
	{
		Uart2_SendByte(*data);
		data++;
	}
}

void Uart3_SendData(uint8_t *data ,unsigned short int len)
{
	for(unsigned short int i=0;i<len;i++)
	{
		Uart3_SendByte(*data);
		data++;
	}
}

void usart1_transimit(uint8_t *data ,unsigned short int len,void (*en_port)(unsigned char))
{
	  if(NULL!=en_port) en_port(1);//使能发送
	
		for(unsigned short int i = 0;i < len;i++)
		{
			USART_SendData(USART1,data[i]);
			while(!(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == SET)){}; //查询发送FIFO是否发送完成
		}	
	  if(NULL!=en_port) en_port(0);//使能接收
}

void usart2_transimit(uint8_t *data ,unsigned short int len,void (*en_port)(unsigned char))
{
	  if(NULL!=en_port) en_port(1);//使能发送
	
		for(unsigned short int i = 0;i < len;i++)
		{
			USART_SendData(USART2,data[i]);
			while(!(USART_GetFlagStatus(USART2, USART_FLAG_TRAC) == SET)){}; //查询发送FIFO是否发送完成
		}	
		
	  if(NULL!=en_port) en_port(0);//使能接收
}


void UartCheckAndSet(unsigned char bound)//波特率参数检测并设置//波特率：18表示2400,30表示4800，60表示9600
{	
	unsigned long TempBound = 0;

 	if((bound > 0)&&(bound < 10))
	{
		if(bound == 1)
		{
			TempBound  = 2400;
		}else if(bound == 2)
		{
		 	TempBound  = 4800;
		}else if(bound == 3)
		{
			TempBound  = 9600;
		}else if(bound == 4)
		{
			TempBound  = 14400;
		}else if(bound == 5)
		{
			TempBound  = 19200;
		}else if(bound == 6)
		{
			TempBound  = 38400;
		}else if(bound == 7)
		{
			TempBound  = 56000;
		}else if(bound == 8)
		{
			TempBound  = 57600;
		}else if(bound == 9)
		{
			TempBound  = 115200;
		}
		uart_init(1,TempBound);//重设串口波特率	
	}
}



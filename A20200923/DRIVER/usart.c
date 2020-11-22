
#include "usart.h"
#include <stdio.h>

unsigned char UART1_Frame_Flag=0;
unsigned short int UART1_RLen;
unsigned char UART1_RBuf[BUFF1_MAX_LEN];

unsigned char UART2_Frame_Flag=0;
unsigned short int UART2_RLen;
unsigned char UART2_RBuf[BUFF2_MAX_LEN];


//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
/* ��Printf���ݷ������� */
  USART_SendData(USART3, (unsigned char) ch);
  while (USART_GetFlagStatus(USART3, USART_FLAG_TDE) == RESET); 
  return (ch);
}

void uart_init(unsigned char Port,unsigned long Baud)
{
//GPIO�˿�����
 	GPIO_InitType GPIO_InitStructure;
	USART_InitType USART_InitStructure;
	NVIC_InitType NVIC_InitStructure;
	if(1==Port)	//����1��ʼ��
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
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
		//USART1_TX  ENABLE   PA.8
		GPIO_InitStructure.GPIO_Pins = MODBUS_USART_EN_PIN;				 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_InitStructure);  
		MODBUS_TX_EN(0);
		Rs485RxEnable;
		//Usart1 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//ѡ��USART1�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;	//�����ȼ�Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�Ϊ3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

		//USART ��ʼ������
		USART_InitStructure.USART_BaudRate = Baud;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //����λ8λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //ֹͣλ1λ
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //��У��
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //���ͻ��߽���
		USART_Init(USART1, &USART_InitStructure);	
		
		USART_INTConfig(USART1, USART_INT_IDLEF, ENABLE);//���������ж�
		USART_INTConfig(USART1, USART_INT_RDNE, ENABLE);//���������ж�
 
		USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
    USART_ClearFlag(USART1, USART_FLAG_TRAC);//�巢����ɱ�־λ			
	 //RS485�ȴ���������					 
	}
	else if(2==Port)
	{
	  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2,ENABLE);
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				 //���츴�����
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);  

		//Usart1 NVIC ����	
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//ѡ��USART1�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;	//�����ȼ�Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�Ϊ3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1  
		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = Baud;//һ������Ϊ9600;
//		USART_InitStructure.USART_BaudRate = 9600;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //����λ8λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //ֹͣλ1λ
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //��У��
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  //���ͻ��߽���
		USART_Init(USART2, &USART_InitStructure);
		
	  USART_INTConfig(USART2, USART_INT_IDLEF, ENABLE);//���������ж�
		USART_INTConfig(USART2, USART_INT_RDNE, ENABLE);//���������ж�
		USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���	
		
    USART_ClearFlag(USART2, USART_FLAG_TRAC);//�巢����ɱ�־λ			
	}
	else if(3==Port)
	{
	  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3,ENABLE);
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				 //���츴�����
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		 //��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);  

		//Usart1 NVIC ����	
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//ѡ��USART1�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;	//�����ȼ�Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�Ϊ3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1  
		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = 9600;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 //����λ8λ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;		 //ֹͣλ1λ
		USART_InitStructure.USART_Parity = USART_Parity_No;			 //��У��
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
		USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;	  //���ͻ��߽���
		USART_Init(USART3, &USART_InitStructure);
		
//	  USART_INTConfig(USART3, USART_INT_IDLEF, ENABLE);//���������ж�
//		USART_INTConfig(USART3, USART_INT_RDNE, ENABLE);//���������ж�
		USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���	
		
    USART_ClearFlag(USART3, USART_FLAG_TRAC);//�巢����ɱ�־λ			
	}	
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
 // ������������ж�.
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORERR) != RESET)//ע�⣡����ʹ��if(USART_GetITStatus(USART1, USART_FLAG_RDNE) != RESET)���ж�
	{
		USART_ClearFlag(USART1,USART_FLAG_ORERR);        //��SR
		USART_ReceiveData(USART1); // ��ȡ���յ�������
	}
		//  �������մ����ж�.
	if(USART_GetITStatus(USART1, USART_INT_RDNE) != RESET)//�����ж�
	{
		USART_ClearITPendingBit(USART1, USART_FLAG_RDNE);// ����жϱ�ʾ.
		if(BUFF1_MAX_LEN>UART1_RLen)
		{
		  UART1_RBuf[UART1_RLen++] = USART_ReceiveData(USART1);// ��ȡ���յ�������				
		}
	}
	if(USART_GetITStatus(USART1, USART_INT_IDLEF) != RESET)//�����ж�
	{
		//STM32F1ϵ�����IDLE��־�����ȶ�SR DR�Ĵ���
		UART1_Frame_Flag=1;
		USART_ReceiveData(USART1); // ��ȡ���յ�������	
		USART_ClearFlag(USART1, USART_FLAG_IDLEF);//���жϱ�־
	}		
}
void USART2_IRQHandler(void)                	//����2�жϷ������
{

 // ������������ж�.
	if (USART_GetFlagStatus(USART2, USART_FLAG_ORERR) != RESET)//ע�⣡����ʹ��if(USART_GetITStatus(USART1, USART_FLAG_RDNE) != RESET)���ж�
	{
		USART_ClearFlag(USART2,USART_FLAG_ORERR);        //��SR
		USART_ReceiveData(USART2); // ��ȡ���յ�������
	}
	//  �������մ����ж�.
	if(USART_GetITStatus(USART2, USART_INT_RDNE) != RESET)//�����ж�
	{
		USART_ClearITPendingBit(USART2, USART_FLAG_RDNE);// ����жϱ�ʾ.
		if(BUFF2_MAX_LEN>UART2_RLen)
		{
		  UART2_RBuf[UART2_RLen++] = USART_ReceiveData(USART2);// ��ȡ���յ�������	
		}		
	}
	if(USART_GetITStatus(USART2,USART_INT_IDLEF) != RESET)//�����ж�
	{
		//STM32F1ϵ�����IDLE��־�����ȶ�SR DR�Ĵ���
		UART1_Frame_Flag=2;
    USART_ReceiveData(USART2); // ��ȡ���յ�������	
		USART_ClearFlag(USART2, USART_FLAG_IDLEF);//���жϱ�־
	}		
}

void enable_uart1_port(unsigned char x)
{
	(x)?MODBUS_TX_EN(1):(MODBUS_TX_EN(0));
}

void Uart1_SendByte(uint8_t byte)
{
	USART_SendData(USART1,byte);	
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == SET)); //��ѯ����FIFO�Ƿ������
	USART_ClearFlag(USART1, USART_FLAG_TRAC);//���������ɱ�־λ
}

void Uart2_SendByte(uint8_t byte)
{
	USART_SendData(USART2,byte);	
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TRAC) == SET)); //��ѯ����FIFO�Ƿ������
	USART_ClearFlag(USART2, USART_FLAG_TRAC);//���������ɱ�־λ
}

void Uart3_SendByte(uint8_t byte)
{
	USART_SendData(USART3,byte);	
	while(!(USART_GetFlagStatus(USART3, USART_FLAG_TRAC) == SET)); //��ѯ����FIFO�Ƿ������
	USART_ClearFlag(USART3, USART_FLAG_TRAC);//���������ɱ�־λ
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
	  if(NULL!=en_port) en_port(1);//ʹ�ܷ���
	
		for(unsigned short int i = 0;i < len;i++)
		{
			USART_SendData(USART1,data[i]);
			while(!(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) == SET)){}; //��ѯ����FIFO�Ƿ������
		}	
	  if(NULL!=en_port) en_port(0);//ʹ�ܽ���
}

void usart2_transimit(uint8_t *data ,unsigned short int len,void (*en_port)(unsigned char))
{
	  if(NULL!=en_port) en_port(1);//ʹ�ܷ���
	
		for(unsigned short int i = 0;i < len;i++)
		{
			USART_SendData(USART2,data[i]);
			while(!(USART_GetFlagStatus(USART2, USART_FLAG_TRAC) == SET)){}; //��ѯ����FIFO�Ƿ������
		}	
		
	  if(NULL!=en_port) en_port(0);//ʹ�ܽ���
}


void UartCheckAndSet(unsigned char bound)//�����ʲ�����Ⲣ����//�����ʣ�18��ʾ2400,30��ʾ4800��60��ʾ9600
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
		uart_init(1,TempBound);//���贮�ڲ�����	
	}
}



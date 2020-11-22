/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "stm32f10x_conf.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
//------------------------------------------------------------	
//STM32���� �����ж�ʹ��  
	if(xRxEnable==TRUE)   
	{   
	//ʹ�ܽ��պͽ����ж�
    MODBUS_TX_EN(0);		
		USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);  
	}   
	else if(xRxEnable == FALSE)  
	{   
	//��ֹ���պͽ����ж�   
    MODBUS_TX_EN(1);				
		USART_ITConfig(MODBUS_USART, USART_IT_RXNE, DISABLE);  
	}  
	//STM32���� �����ж�ʹ��  
	if(xTxEnable==TRUE)   
	{  
	//ʹ�ܷ�������ж� 
   MODBUS_TX_EN(1);		
		USART_ITConfig(MODBUS_USART, USART_IT_TXE, ENABLE);  
	}   
	else if(xTxEnable == FALSE)   
	{  
	//��ֹ��������ж� 
    MODBUS_TX_EN(0);		
		USART_ITConfig(MODBUS_USART, USART_IT_TXE, DISABLE);  
	}   
	else if(xTxEnable == FALSE)   
	{  
		 MODBUS_TX_EN(0);
		USART_ITConfig(MODBUS_USART, USART_IT_TC, DISABLE);  
	}  
//------------------------------------------------------------	
}

BOOL 
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	//------------------------------------------------------------	
	GPIO_InitTypeDef  GPIO_InitStructure;  
	USART_InitTypeDef USART_InitStructure;  
	NVIC_InitTypeDef  NVIC_InitStructure; 
	
	(void)ucPORT; //���޸Ĵ���  
	(void)ucDataBits; //���޸����ݳ���  
	(void)eParity; //�����Ч���ʽ  
	
	/***���ų�ʼ��*************************************/  

	//ʱ��ʹ��  
	RCC_APB2PeriphClockCmd(MODBUS_USART_GPIO_CLK,ENABLE);  
	RCC_APB2PeriphClockCmd(MODBUS_USART_CLK,ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	
	//TX  
	GPIO_InitStructure.GPIO_Pin = MODBUS_USART_TX_PIN;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������  
	GPIO_Init(MODBUS_USART_TX_PORT, &GPIO_InitStructure);  
	//RX  
	GPIO_InitStructure.GPIO_Pin = MODBUS_USART_RX_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������  
	GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = MODBUS_USART_EN_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_InitStructure);  

	/***************���ڳ�ʼ��********************/  
	USART_InitStructure.USART_BaudRate    = ulBaudRate;  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	USART_InitStructure.USART_Mode        = USART_Mode_Rx|USART_Mode_Tx;  
	USART_InitStructure.USART_Parity      = USART_Parity_No;  
	USART_InitStructure.USART_StopBits    = USART_StopBits_1;  
	USART_InitStructure.USART_WordLength  = USART_WordLength_8b;  
	USART_Init(MODBUS_USART, &USART_InitStructure);  
	USART_Cmd(MODBUS_USART, ENABLE);  

	/*****************************�жϳ�ʼ��*************************************/  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_USART_IRQ ;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQͨ��ʹ��  
	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���  

	return TRUE; 
	//------------------------------------------------------------	
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
//------------------------------------------------------------	
  USART_SendData(MODBUS_USART, ucByte);  //??????  
  while (USART_GetFlagStatus(MODBUS_USART, USART_FLAG_TC) == RESET){};   
  return TRUE; 
//------------------------------------------------------------	
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
//------------------------------------------------------------	
	 *pucByte=USART_ReceiveData(MODBUS_USART);
//------------------------------------------------------------	
    return TRUE;
}

//------------------------------------------------------------
//�����жϴ�����(ע�������ʱ��FinSH���ڳ�ͻ,����������)  
void MODBUS_USART_IRQHandler(void)  
{  
   if(USART_GetITStatus(MODBUS_USART, USART_IT_TXE) == SET)   
 {  
     prvvUARTTxReadyISR();  
     USART_ClearITPendingBit(MODBUS_USART, USART_IT_TXE);  
   }  
   
   if(USART_GetITStatus(MODBUS_USART, USART_IT_RXNE) == SET)   
 {  
     prvvUARTRxISR();  
     USART_ClearITPendingBit(MODBUS_USART, USART_IT_RXNE);  
   }  
}  
//-----------------------END-------------------------------------
/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

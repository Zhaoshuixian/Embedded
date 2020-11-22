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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	NVIC_InitTypeDef NVIC_InitStructure;  
	uint16_t PrescalerValue = 0;  
	//ʹ�ܶ�ʱ��4��ʱ��  
	RCC_APB1PeriphClockCmd(MODBUS_TIM_CLK, ENABLE); //ʱ��ʹ��  
	//��ʱ��4ʱ������˵��  
	//HCLKΪ72MHz,APB1��2��ƵΪ36MHz  
	//TIM4ʱ�ӱ�Ƶ��Ϊ72MHz(Ӳ���Զ���Ƶ,�ﵽ���)  
	//TIM4�ķ�Ƶϵ��Ϊ3599,ʱ���Ƶ��Ϊ��72 / (1 + Prescaler) = 20KHz,��׼Ϊ50us  
	//TIM������ֵΪ��usTim1Timerout50u  
	PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;   
	//��ʱ��TIM4��ʼ��  
	TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us;   
	TIM_TimeBaseStructure.TIM_Prescaler =PrescalerValue;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
	TIM_TimeBaseInit(MODBUS_TIM, &TIM_TimeBaseStructure);   
	//ʹ��Ԥװ��  
	TIM_ARRPreloadConfig(MODBUS_TIM, ENABLE);  

	//�ж����ȼ�NVIC����  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_TIM_IRQ ;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
	NVIC_Init(&NVIC_InitStructure);   
	//�������жϱ�־λ  
	TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
	//��ʱ������жϹر�  
	TIM_ITConfig(MODBUS_TIM,TIM_IT_Update,DISABLE);  
	//ʧ�ܶ�ʱ��  
	TIM_Cmd(MODBUS_TIM, DISABLE);  
	
  return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
   TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
   TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, ENABLE);  
   //���ö�ʱ���ĳ�ֵ  
    TIM_SetCounter(MODBUS_TIM,0x0000);  
    TIM_Cmd(MODBUS_TIM, ENABLE);  	
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
  TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
  TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, DISABLE);  
  TIM_SetCounter(MODBUS_TIM,0x0000);   
  //�رն�ʱ��  
  TIM_Cmd(MODBUS_TIM, DISABLE); 	
}

//Modbus ��ʱ���жϺ���  
void MODBUS_TIM_IRQHandler( void )  
{  
  if(TIM_GetITStatus(MODBUS_TIM, TIM_IT_Update) == SET)   
{  
     TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
     prvvTIMERExpiredISR();  
   }  
}  

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}


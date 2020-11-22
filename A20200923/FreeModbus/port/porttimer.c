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
	//使能定时器4的时钟  
	RCC_APB1PeriphClockCmd(MODBUS_TIM_CLK, ENABLE); //时钟使能  
	//定时器4时间配置说明  
	//HCLK为72MHz,APB1经2分频为36MHz  
	//TIM4时钟倍频后为72MHz(硬件自动倍频,达到最大)  
	//TIM4的分频系数为3599,时间基频率为：72 / (1 + Prescaler) = 20KHz,基准为50us  
	//TIM最大计数值为：usTim1Timerout50u  
	PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;   
	//定时器TIM4初始化  
	TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us;   
	TIM_TimeBaseStructure.TIM_Prescaler =PrescalerValue;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
	TIM_TimeBaseInit(MODBUS_TIM, &TIM_TimeBaseStructure);   
	//使能预装载  
	TIM_ARRPreloadConfig(MODBUS_TIM, ENABLE);  

	//中断优先级NVIC设置  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
	NVIC_InitStructure.NVIC_IRQChannel = MODBUS_TIM_IRQ ;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
	NVIC_Init(&NVIC_InitStructure);   
	//清除溢出中断标志位  
	TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
	//定时器溢出中断关闭  
	TIM_ITConfig(MODBUS_TIM,TIM_IT_Update,DISABLE);  
	//失能定时器  
	TIM_Cmd(MODBUS_TIM, DISABLE);  
	
  return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
   TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);  
   TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, ENABLE);  
   //设置定时器的初值  
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
  //关闭定时器  
  TIM_Cmd(MODBUS_TIM, DISABLE); 	
}

//Modbus 定时器中断函数  
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


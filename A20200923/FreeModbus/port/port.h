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

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>
#include <inttypes.h>
#include "stm32f10x_conf.h"

#include "pindef.h"


#define ENTER_CRITICAL_SECTION( )    __set_PRIMASK(1); //关闭中断  
#define EXIT_CRITICAL_SECTION( )     __set_PRIMASK(0); //开启中断  
//Modbus串口   
#define  MODBUS_USART            USART1  
#define  MODBUS_USART_CLK        RCC_APB2Periph_USART1  
#define  MODBUS_USART_GPIO_CLK   RCC_APB2Periph_GPIOA  
#define  MODBUS_USART_TX_PORT    GPIOA  
#define  MODBUS_USART_RX_PORT    GPIOA  
#define  MODBUS_USART_TX_PIN     GPIO_Pin_9  
#define  MODBUS_USART_RX_PIN     GPIO_Pin_10 
#define  MODBUS_USART_EN_PIN     GPIO_Pin_11
#define  MODBUS_USART_IRQ           USART1_IRQn
#define  MODBUS_USART_IRQHandler    USART1_IRQHandler 

#define  MODBUS_TX_EN(x)            USER_USART_EN(x)           

//Modbus定时器  
#define  MODBUS_TIM              TIM4        
#define  MODBUS_TIM_CLK          RCC_APB1Periph_TIM4 
#define  MODBUS_TIM_IRQ          TIM4_IRQn  
#define  MODBUS_TIM_IRQHandler   TIM4_IRQHandler  

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }


typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#endif

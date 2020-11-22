

#include "timer.h"   

void timer1_init(unsigned short int arr,unsigned short int psc)
{
  TMR_TimerBaseInitType TMR_TimerBaseStructure;
	NVIC_InitType         NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1,ENABLE);	
	
	TMR_TimerBaseStructure.TMR_Period = arr-1;
  TMR_TimerBaseStructure.TMR_DIV    =	psc-1;
	TMR_TimerBaseStructure.TMR_ClockDivision = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter=0;	
	TMR_TimerBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;  
	TMR_TimerBaseStructure.TMR_ClockDivision =TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR1,&TMR_TimerBaseStructure); 
	
	TMR_ClearFlag(TMR1,TMR_FLAG_Update); 
  TMR_ClearITPendingBit(TMR1,TMR_FLAG_Update); 
	
	TMR_INTConfig(TMR1,TMR_INT_Overflow,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TMR1_OV_TMR10_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	TMR_Cmd(TMR1,ENABLE); 					 
}


void timer2_init(unsigned short int arr,unsigned short int psc)//for rrs
{
  TMR_TimerBaseInitType TMR_TimerBaseStructure;
	NVIC_InitType         NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2,ENABLE);	
	
	TMR_TimerBaseStructure.TMR_Period = arr-1;
  TMR_TimerBaseStructure.TMR_DIV    =	psc-1;
	TMR_TimerBaseStructure.TMR_ClockDivision = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter=0;	
	TMR_TimerBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;  
	TMR_TimerBaseStructure.TMR_ClockDivision =TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR2,&TMR_TimerBaseStructure); 
	
	TMR_ClearFlag(TMR2,TMR_FLAG_Update); 
  TMR_ClearITPendingBit(TMR2,TMR_FLAG_Update); 
	
	TMR_INTConfig(TMR2,TMR_INT_Overflow,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TMR2_GLOBAL_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	TMR_Cmd(TMR2,ENABLE); 					 
}

void timer3_init(unsigned short int arr,unsigned short int psc)//for buzzer
{
  TMR_TimerBaseInitType TMR_TimerBaseStructure;
	NVIC_InitType         NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3,ENABLE);	
	
	TMR_TimerBaseStructure.TMR_Period = arr-1;
  TMR_TimerBaseStructure.TMR_DIV    =	psc-1;
	TMR_TimerBaseStructure.TMR_ClockDivision = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter=0;	
	TMR_TimerBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;  
	TMR_TimerBaseStructure.TMR_ClockDivision =TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR3,&TMR_TimerBaseStructure); 
	
	TMR_ClearFlag(TMR3,TMR_FLAG_Update); 
  TMR_ClearITPendingBit(TMR3,TMR_FLAG_Update); 
	
	TMR_INTConfig(TMR3,TMR_INT_Overflow,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TMR3_GLOBAL_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	TMR_Cmd(TMR3,ENABLE);				
}

void timer4_init(unsigned short int arr,unsigned short int psc)//for infrared rx
{
  TMR_TimerBaseInitType TMR_TimerBaseStructure;
	NVIC_InitType         NVIC_InitStructure;
	TMR_ICInitType        TMR_ICInitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4,ENABLE);	
	
	TMR_TimerBaseStructure.TMR_Period = arr-1;
  TMR_TimerBaseStructure.TMR_DIV    =	psc-1;
	TMR_TimerBaseStructure.TMR_ClockDivision = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter=0;	
	TMR_TimerBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Up;  
	TMR_TimerBaseStructure.TMR_ClockDivision =TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR4,&TMR_TimerBaseStructure); 

  //输入捕获比较模式配置  
  TMR_ICInitStructure.TMR_Channel     = TMR_Channel_2;
  TMR_ICInitStructure.TMR_ICPolarity  = TMR_ICPolarity_Rising;	//下降沿捕获
  TMR_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
  TMR_ICInitStructure.TMR_ICDIV       = TMR_ICDIV_DIV1;	 //配置输入分频,不分频 
  TMR_ICInitStructure.TMR_ICFilter    = 0x03;
  TMR_ICInit(TMR4,&TMR_ICInitStructure);//初始化定时器输入捕获通道	
	
	TMR_INTConfig(TMR4,TMR_INT_Overflow,ENABLE);
  TMR_INTConfig(TMR4,TMR_INT_CC2,ENABLE);          //允许更新和捕获中断
	
	TMR_ClearFlag(TMR4,TMR_FLAG_Update); 
	TMR_ClearFlag(TMR4,TMR_FLAG_CC2); 	
  TMR_ClearITPendingBit(TMR4,TMR_FLAG_Update); 	
  TMR_ClearITPendingBit(TMR4,TMR_FLAG_CC2); 	
	
	NVIC_InitStructure.NVIC_IRQChannel = TMR4_GLOBAL_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	TMR_Cmd(TMR4,ENABLE); 					 
}

void timer10_init(unsigned short int arr,unsigned short int psc)//for infrared tx wave timer
{
  TMR_TimerBaseInitType TMR_TimerBaseStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR10,ENABLE);	
	
	TMR_TimerBaseStructure.TMR_Period = arr-1;
  TMR_TimerBaseStructure.TMR_DIV    =	psc-1;
	TMR_TimerBaseStructure.TMR_ClockDivision = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter=0;	
	TMR_TimerBaseStructure.TMR_CounterMode   = TMR_CounterDIR_Down;  //衰减模式
	TMR_TimerBaseStructure.TMR_ClockDivision =TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR10,&TMR_TimerBaseStructure); 

	TMR_INTConfig(TMR10,TMR_INT_Overflow,ENABLE);
	
	TMR_ClearFlag(TMR10,TMR_FLAG_Update); 
  TMR_ClearITPendingBit(TMR10,TMR_FLAG_Update); 	

	TMR_Cmd(TMR10,DISABLE); 					 
}





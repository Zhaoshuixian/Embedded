


#include "buzzer.h" 
#include "delay.h"
#include "timer.h"
//void buzzer_loudly(unsigned char x)
//{
//  xdelay xdelay_us=delay_us;
//	if(0==x)
//	{
//		BUZZER(0);
//	}
//	else
//	{
//		BUZZER(1);
//		xdelay_us(300);
//		BUZZER(0);
//		xdelay_us(300);		
//	}
//}

buzzer_st buzzer=BUZZER_DEFAULT_CFG;


void	buzzer_init(void *arg)//蜂鸣器初始化
{
	GPIO_InitType  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(BEEP_PCC,ENABLE);	
	
	GPIO_InitStructure.GPIO_Pins = BEEP_PIN;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;  //推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
	BEEP_SW(0);
	
	timer3_init(200,144);//200uS /*For BUZZER*/	
}

void buzzer_start(void *arg,unsigned char bnum,unsigned int btime)
{
	buzzer_st *const me=(buzzer_st*)arg;
  
	me->num=bnum;
  me->rt=btime;
 	me->conter=0;
	me->en=1;	
}

void buzzer_speak(void *arg)
{
	buzzer_st *const me=(buzzer_st*)arg;
	
	if(0<me->num)
	{
		if(200<=me->conter++)//蜂鸣间隔时长
		{
			me->conter=0;
			--me->num;
			me->rt=500;//重载
		}	
	}	
	if(0<me->rt)
	{
		me->rt--;
		me->en=~me->en;
  }
	else
	{
		me->en=0;
	}
	BEEP_SW(me->en);	
}

//////////////////////////////////////////////////////////////////////////////////////////
void buzzer_thread_entry(void *parameter)
{
  buzzer_speak(&buzzer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void TMR3_GLOBAL_IRQHandler(void)
{
	if(TMR_GetINTStatus(TMR3,TMR_INT_Overflow) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{	
    buzzer_thread_entry(0);
		TMR_ClearITPendingBit(TMR3,TMR_INT_Overflow);  //清除TIMx的中断待处理位:TIM 中断源		
	}	
}





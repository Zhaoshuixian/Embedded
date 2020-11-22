

#include "infrade_t.h"
#include "infrade_r.h"
#include "timer.h"   
/*红外发射驱动

--用定时器复现记录的波形数据---

*/

unsigned char wave_recurrent_flag=0;

//产生26.31us=38KHz
void infrared_carrfreq_init(void)//TIM4_CH4
{
	GPIO_InitType          GPIO_InitStructure;
	TMR_OCInitType         TMR_OCInitStructure;
  TMR_TimerBaseInitType  TMR_TimerBaseStructure;

  RCC_APB2PeriphClockCmd(IR_TX_PCC, ENABLE);// 
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
 	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4,ENABLE);	
	
  GPIO_InitStructure.GPIO_Pins     = IR_TX_PIN ;   
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_PP; //
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //
  GPIO_Init(IR_TX_PORT,&GPIO_InitStructure); //	
	
	TMR_TimerBaseStructure.TMR_Period            = 27-1;
  TMR_TimerBaseStructure.TMR_DIV               = 144-1;
	TMR_TimerBaseStructure.TMR_ClockDivision     = 0;
	TMR_TimerBaseStructure.TMR_RepetitionCounter = 0;	
	TMR_TimerBaseStructure.TMR_CounterMode       = TMR_CounterDIR_Up;  
	TMR_TimerBaseStructure.TMR_ClockDivision     = TMR_CKD_DIV1;
	TMR_TimeBaseInit(TMR4,&TMR_TimerBaseStructure); 
	TMR_ARPreloadConfig(TMR4,ENABLE);
	
	TMR_OCInitStructure.TMR_Pulse       = 13;//6
  TMR_OCInitStructure.TMR_OCMode      = TMR_OCMode_PWM1;
	TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;//比较输出使能
//	TMR_OCInitStructure.TMR_OutputNState= TMR_OutputNState_Enable;
	TMR_OCInitStructure.TMR_OCPolarity  = TMR_OCPolarity_High;//输出极性:TIM输出比较极性高
//	TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_High;
	TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
	TMR_OCInitStructure.TMR_OCNIdleState= TMR_OCIdleState_Reset;

	TMR_OC4Init(TMR4,&TMR_OCInitStructure);  
	TMR_OC4PreloadConfig(TMR4,TMR_OCPreload_Enable); //使能TIM4在CCR4上的预装载寄存器 

	TMR_Cmd(TMR4,DISABLE);	
	TMR_CtrlPWMOutputs(TMR4,DISABLE);	
}


void infrared_carrfreq_start(FunctionalState NewState)
{
	TMR_Cmd(TMR4,NewState);	
	TMR_CtrlPWMOutputs(TMR4,NewState);		
}

void infrared_wtimer_start(FunctionalState NewState)
{
	 TMR_Cmd(TMR10,NewState); //启动定时器	
}

void infrared_wtimer_reload(unsigned short int counter)
{
	TMR_SetCounter(TMR10,counter);//设置状态计数值
}

unsigned short int infrared_wtimer_read(void)
{
	return TMR_GetCounter(TMR10);//读取计数值
}

void infrared_wtimer_init(void)
{
	timer10_init(1,144);//1us
}


void infrared_tx_init(void)
{
  infrared_carrfreq_init();//载波定时器
	infrared_wtimer_init();  //数据定时器
}
	
void infrared_data_recurrent(void *wave_data)/*波形复现*/
{
	study_st *const study_data=(study_st *)wave_data;
	
	infrared_carrfreq_start(ENABLE);//启动载波定时器
	infrared_wtimer_start(ENABLE); //启动数据波定时器
	
	for(unsigned char i=0;i<study_data->edge_cnt/2;i++)
	{	
		infrared_wtimer_reload(study_data->wave_buff[i]);//设置状态计数值
		while(0!=infrared_wtimer_read())//LOW LEVEL ->OUTPUT
		{
			IR_TX_SW(0);
		}
		infrared_wtimer_reload(study_data->wave_buff[++i]);//设置状态计数值
		while(0!=infrared_wtimer_read()) //HIGH LEVEL ->OUTPUT
		{
			IR_TX_SW(1);			
		}			
	}
	infrared_wtimer_start(DISABLE); //关闭定时器
	infrared_carrfreq_start(DISABLE);//启动载波定时器
}


void infrared_tx_thread_entry(void)
{
	if(wave_recurrent_flag)//波形复现标志
	{
		wave_recurrent_flag=0;
		infrared_data_recurrent(0);
	}
	
}






#include "infrade_t.h"
#include "infrade_r.h"
#include "timer.h"   
/*���ⷢ������

--�ö�ʱ�����ּ�¼�Ĳ�������---

*/

unsigned char wave_recurrent_flag=0;

//����26.31us=38KHz
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
	TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;//�Ƚ����ʹ��
//	TMR_OCInitStructure.TMR_OutputNState= TMR_OutputNState_Enable;
	TMR_OCInitStructure.TMR_OCPolarity  = TMR_OCPolarity_High;//�������:TIM����Ƚϼ��Ը�
//	TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_High;
	TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Set;
	TMR_OCInitStructure.TMR_OCNIdleState= TMR_OCIdleState_Reset;

	TMR_OC4Init(TMR4,&TMR_OCInitStructure);  
	TMR_OC4PreloadConfig(TMR4,TMR_OCPreload_Enable); //ʹ��TIM4��CCR4�ϵ�Ԥװ�ؼĴ��� 

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
	 TMR_Cmd(TMR10,NewState); //������ʱ��	
}

void infrared_wtimer_reload(unsigned short int counter)
{
	TMR_SetCounter(TMR10,counter);//����״̬����ֵ
}

unsigned short int infrared_wtimer_read(void)
{
	return TMR_GetCounter(TMR10);//��ȡ����ֵ
}

void infrared_wtimer_init(void)
{
	timer10_init(1,144);//1us
}


void infrared_tx_init(void)
{
  infrared_carrfreq_init();//�ز���ʱ��
	infrared_wtimer_init();  //���ݶ�ʱ��
}
	
void infrared_data_recurrent(void *wave_data)/*���θ���*/
{
	study_st *const study_data=(study_st *)wave_data;
	
	infrared_carrfreq_start(ENABLE);//�����ز���ʱ��
	infrared_wtimer_start(ENABLE); //�������ݲ���ʱ��
	
	for(unsigned char i=0;i<study_data->edge_cnt/2;i++)
	{	
		infrared_wtimer_reload(study_data->wave_buff[i]);//����״̬����ֵ
		while(0!=infrared_wtimer_read())//LOW LEVEL ->OUTPUT
		{
			IR_TX_SW(0);
		}
		infrared_wtimer_reload(study_data->wave_buff[++i]);//����״̬����ֵ
		while(0!=infrared_wtimer_read()) //HIGH LEVEL ->OUTPUT
		{
			IR_TX_SW(1);			
		}			
	}
	infrared_wtimer_start(DISABLE); //�رն�ʱ��
	infrared_carrfreq_start(DISABLE);//�����ز���ʱ��
}


void infrared_tx_thread_entry(void)
{
	if(wave_recurrent_flag)//���θ��ֱ�־
	{
		wave_recurrent_flag=0;
		infrared_data_recurrent(0);
	}
	
}




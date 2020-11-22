
#include "task.h"

/////////////////////////////////��ʱ�������е�������//////////////////////////////////////////
Task_Struct Tasks[]=//
{
    {0,LED_TASK_TIME,LED_TASK_TIME,led_thread_entry},	//LED
  	{0,KEY_TASK_TIME,KEY_TASK_TIME,key_thread_entry}, //KEY	
//  //	{0,RTC_TASK_TIME,RTC_TASK_TIME,ertc_thread_entry}, //RTC	
////   	{0,ADC_TASK_TIME,ADC_TASK_TIME,adc_thread_entry}, //SENSOR ADC SAMPLE	
  	{0,NRF_TASK_TIME,NRF_TASK_TIME,rf_thread_entry}, //NRF24L01
//  	{0,TSENS_TASK_TIME,TSENS_TASK_TIME,tsensor_thread_entry}, //DS18B20/NTC	
  	{0,IR_TASK_TIME,IR_TASK_TIME,ir_thread_entry}, //IR	
//  	{0,LCD_TASK_TIME,LCD_TASK_TIME,lcd_thread_entry}, //OLED	
 		
};

unsigned char TASK_NUM = sizeof(Tasks)/sizeof(Task_Struct);

/////////////////////////////////////��ʱ�������//////////////////////////////////////
void task_handler(void)//������
{
  static unsigned char i;
	for (i=0; i<TASK_NUM ; i++) //
	{
		if(Tasks[i].IsRun) //����ִ�б�־
		{
      Tasks[i].TaskPointer(0);//ִ�е�ǰ����
			Tasks[i].IsRun = 0;//�����־
		}
	}
}

void task_scheduler(void)
{
 static unsigned char i;
	for(i=0;i<TASK_NUM;i++) //��������
	{
		if(Tasks[i].TaskTimer)
		{
			Tasks[i].TaskTimer--;
			if(0==Tasks[i].TaskTimer) 
			{
				Tasks[i].IsRun = 0x01;
				Tasks[i].TaskTimer = Tasks[i].TaskTime;
			}
		}
	}	
}

void RRS_Handler(void)
{
	if(TMR_GetINTStatus(TMR2,TMR_INT_Overflow) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{	
		task_scheduler();
		TMR_ClearITPendingBit(TMR2,TMR_INT_Overflow);  //���TIMx���жϴ�����λ:TIM �ж�Դ		
	}
}

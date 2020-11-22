
#include "task.h"

/////////////////////////////////分时任务运行调度配置//////////////////////////////////////////
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

/////////////////////////////////////分时任务调度//////////////////////////////////////
void task_handler(void)//任务处理
{
  static unsigned char i;
	for (i=0; i<TASK_NUM ; i++) //
	{
		if(Tasks[i].IsRun) //任务执行标志
		{
      Tasks[i].TaskPointer(0);//执行当前任务
			Tasks[i].IsRun = 0;//清除标志
		}
	}
}

void task_scheduler(void)
{
 static unsigned char i;
	for(i=0;i<TASK_NUM;i++) //遍历任务
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
	if(TMR_GetINTStatus(TMR2,TMR_INT_Overflow) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{	
		task_scheduler();
		TMR_ClearITPendingBit(TMR2,TMR_INT_Overflow);  //清除TIMx的中断待处理位:TIM 中断源		
	}
}

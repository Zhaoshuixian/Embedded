#ifndef __TASK_H__
#define	__TASK_H__

#include "stdint.h"
//-------------底层驱动头文件------
#include "24cxx.h" 
#include "buzzer.h" 
#include "led.h"
#include "usart.h"
#include "ts06.h"
#include "ertc.h"
#include "oled.h"
#include "nrf24l01.h"
#include "ds18b20.h"
#include "infrade_r.h"
//-----------END-----------------

#define RRS_Handler         TMR2_GLOBAL_IRQHandler
#define scheduler_start()   (TMR_Cmd(TMR2, ENABLE))
#define scheduler_stop()    (TMR_Cmd(TMR2, DISABLE))
//////////////////////////任务运行时间配置///////////////////////////////////////////////
#define KEY_TASK_TIME       (20)
#define LED_TASK_TIME       (500)
#define ADC_TASK_TIME       (500)
#define TSENS_TASK_TIME     (1000)
#define NRF_TASK_TIME       (10)
#define RTC_TASK_TIME       (1000)  /*10s*/
#define IR_TASK_TIME        (10)
#define LCD_TASK_TIME       (10)
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	unsigned char IsRun; 
	unsigned int  TaskTimer; 
	unsigned int  TaskTime;
	void (*TaskPointer)(void *parameter); 
}Task_Struct;


void task_scheduler(void);
void task_handler(void);//任务处理

#endif

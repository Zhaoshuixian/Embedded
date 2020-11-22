
#include "task.h"
#include "timer.h"   
#include "wdg.h" 
#include "delay.h" 
#include "dev_cfg.h"
#include "w25x16.h"
#include "infrade_r.h"
#include "infrade_t.h"
/*
--> Project Platform：Base on MDK + Chip AT32F415RBT7(Cotex-M4)
--> Create Date     ：2020/08/24
--> SoftWare Version：V2.0
--> HardWare Version：V1.0
--> Author By       : ZHAUSHION 

--> The last Modify date：2020/09/29
--> Modify Content：
		1.实现触摸按键
		2.实现蜂鸣器
		3.实现红外NEC解码
		4.实现OLED显示
		5.
*/

int main(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetVectorTable(NVIC_VectTab_FLASH|0X0000,ENABLE);
  timer2_init(1000,144);//1MS  /*For RRS*/	
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
//	uart_init(1,RS485_DEFUALT_BAUD);//485串口1初始化为9600  
// 	uart_init(2,IOT_DEFUALT_BAUD);  //GM串口2初始化为9600 
//	w25q_device_register(W25Q16,&w25q16);
//	w25q_device_check(&w25q16);
	#if DEBUG
	printf("W25Q16 is successfully mounted");
	#endif
//  at24c_device_register(AT24C02,&at24c02);
//	at24c_device_check(&at24c02);
	#if DEBUG
	printf("AT24C02 is successfully mounted");
	#endif	
	ts06_init(&ts06);
	led_init(&led);	
//	lcd_init();
  buzzer_init(&buzzer);	
  //ertc_init(&rtc);
//	tsensor_init();
	nrf24_init(&nrf24,RX_MODE);
  infrared_rx_init();
//	infrared_tx_init();
	#if 0
  IWDG_Init(IWDG_Prescaler_256,1250); //看门狗1S钟计时 
  #endif	
	while(1)
	{
//		uart1_thread_entry(0);
//		uart2_thread_entry(0);
    task_handler();		
		IWDG_Feed();
	}
}



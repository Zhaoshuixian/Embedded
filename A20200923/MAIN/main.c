
#include "task.h"
#include "timer.h"   
#include "wdg.h" 
#include "delay.h" 
#include "dev_cfg.h"
#include "w25x16.h"
#include "infrade_r.h"
#include "infrade_t.h"
/*
--> Project Platform��Base on MDK + Chip AT32F415RBT7(Cotex-M4)
--> Create Date     ��2020/08/24
--> SoftWare Version��V2.0
--> HardWare Version��V1.0
--> Author By       : ZHAUSHION 

--> The last Modify date��2020/09/29
--> Modify Content��
		1.ʵ�ִ�������
		2.ʵ�ַ�����
		3.ʵ�ֺ���NEC����
		4.ʵ��OLED��ʾ
		5.
*/

int main(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetVectorTable(NVIC_VectTab_FLASH|0X0000,ENABLE);
  timer2_init(1000,144);//1MS  /*For RRS*/	
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
//	uart_init(1,RS485_DEFUALT_BAUD);//485����1��ʼ��Ϊ9600  
// 	uart_init(2,IOT_DEFUALT_BAUD);  //GM����2��ʼ��Ϊ9600 
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
  IWDG_Init(IWDG_Prescaler_256,1250); //���Ź�1S�Ӽ�ʱ 
  #endif	
	while(1)
	{
//		uart1_thread_entry(0);
//		uart2_thread_entry(0);
    task_handler();		
		IWDG_Feed();
	}
}






#include "key.h" 
#include "24cxx.h"
#include "autom.h"
#include "task.h"

void key_init(void) 
{

	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(MKEY_PCC,ENABLE); 

	GPIO_InitStructure.GPIO_Pins = MKEY_PIN;
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;  // 
	GPIO_Init(MKEY_PORT, &GPIO_InitStructure); 
}

void key_gpio_init(void)
{
	
	
}


void key_thread_entry(void *parameter)
{
	#define PRESSED           (0) //按下
	#define LONG_PRESSED_TIME (5000/KEY_TASK_TIME) //长按时长	
	static unsigned char keypress_timer;
	static unsigned char last_key,now_key;
	
	if(PRESSED==MKEY())
	{
		now_key=1;
//		if(last_key!=now_key)
//		{
//			mb_host.enport=enable_uart1_port;//指定使能端口函数
//		  mb_host.transmit=usart1_transimit;//指定发送端口函数 
//		  mb_host.check=UartCRCCheck;	
//		  modbus_host_send_create(&mb_host,BORADCAST_ADDR+1,0x06,0x0150,1,0);
//		}
		if(LONG_PRESSED_TIME<keypress_timer++)
		{
			at24c_default(&at24c02);
			keypress_timer =0;
			g_ulAutoControl.warn_active(ON);//报警	
			//UartCheckAndSet(g_ulAutoControl.rs485.device_baud);	
			NVIC_SystemReset();
		}
	}
	else
	{
	  now_key=0;
	}
	last_key=now_key;
}


//*****************************************************************************





#include "led.h"

led_st led=
{
	.gpio_init = led_gpio_init,
	.red.sw    = red_red_drv,
	.blue.sw   = blue_red_drv,	
};

void led_gpio_init(void)
{
 	GPIO_InitType GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(LED_PCC,ENABLE);
	//LED״ָ̬ʾ��
	GPIO_InitStructure.GPIO_Pins 	= RLED_PIN|BLED_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT_PP ;   
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);	
	RLED_SW(1);	
	BLED_SW(1);		
}

void red_red_drv(unsigned char sw_value)
{
	RLED_SW(sw_value);
}

void blue_red_drv(unsigned char sw_value)
{
	BLED_SW(sw_value);
}

void led_func(void *arg,unsigned char sw_value)
{
	drv_st *const me = (drv_st *)arg;
	
	sw_value?(me->sw(0)):(me->sw(1));	
}

void led_init(led_st *const led_me)
{
	led_me->gpio_init();
}

//-------------------------------------------------------
void led_thread_entry(void *parameter)
{
	static unsigned char x;
	x=(x+1)%2;
	led_func(&led.red,x%2);	
	led_func(&led.blue,0);
}

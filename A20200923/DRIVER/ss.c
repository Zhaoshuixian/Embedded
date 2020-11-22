

#include "ss.h"



void ss_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(SS_PCC,ENABLE);
	
  GPIO_InitStructure.GPIO_Pins = NTC_PIN|SS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		  
  GPIO_Init(SS_PORT,&GPIO_InitStructure);	
}

void ss_init(void *arg)
{
	ss_st *const me=(ss_st *)arg;
	me->gpio_init();
	me->adc_init();
  me->adc_data=0;	
}


unsigned char  ss_value(void *arg)
{
	ss_st *const me=(ss_st *)arg;
	unsigned int ss_value=0;
	
	
	
	return ss_value;

}


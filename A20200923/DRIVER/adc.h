#ifndef __ADC_H
#define __ADC_H	

#include "at32f4xx.h"

extern volatile unsigned short int ADC_ConvertedValue[4];
extern unsigned char NTC_Get_Val(void);
void adc_config_init(void);
void adc_thread_entry(void *parameter);

#endif 


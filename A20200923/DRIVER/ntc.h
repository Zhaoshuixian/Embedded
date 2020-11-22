

#ifndef __NTC_H__
#define __NTC_H__


#include "myadc.h"

typedef struct
{
	void (*gpio_init)(void);
	void (*adc_init)(void);
	unsigned int adc_data;
	
}ntc_st;

extern  ntc_st ntc;

#endif
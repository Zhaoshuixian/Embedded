

#ifndef __SS_H__
#define __SS_H__

#include "myadc.h"


/*¹âÃô´«¸ĞÆ÷*/
typedef struct
{
	void (*gpio_init)(void);
	void (*adc_init)(void);
	unsigned int adc_data;
	
}ss_st;



#endif



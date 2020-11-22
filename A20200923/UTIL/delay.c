

#include "delay.h"

//UNDER FREQ=150MHZ
void xdelay_us(unsigned int us)
{
	for(unsigned int i=0;i<3;i++)
	{
		for(unsigned int j=0;j<8*us;j++)
		{
			
		}
	}
}

void xdelay_ms(unsigned int ms)
{
	for(unsigned int i=0;i<3000;i++)//1ms
	{
		for(unsigned int j=0;j<8*ms;j++)
		{
			
		}
	}	
}




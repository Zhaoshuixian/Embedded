
#ifndef __TIMER_H
#define __TIMER_H

#include "at32f4xx.h"

void timer1_init(unsigned short int arr,unsigned short int psc);
void timer2_init(unsigned short int arr,unsigned short int psc);   
void timer3_init(unsigned short int arr,unsigned short int psc);
void timer4_init(unsigned short int arr,unsigned short int psc);
void timer10_init(unsigned short int arr,unsigned short int psc);


#endif

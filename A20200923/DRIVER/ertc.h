

#ifndef __ERTC_H__
#define __ERTC_H__

#include "stdio.h"
#include "at32f4xx.h"
#include "at32f4xx_ertc.h"

typedef struct
{
	unsigned char year; //00-99
	unsigned char month;//1-12
	unsigned char day;//
	unsigned char week;//1-7
	
	unsigned char hour;//
	unsigned char min;
	unsigned char sec;	
	
}time_st;


typedef struct
{
	
  time_st system,alarm;
	
}rtc_st;

extern rtc_st rtc;

void ertc_init(void *arg);
void ertc_thread_entry(void *parameter);

#endif



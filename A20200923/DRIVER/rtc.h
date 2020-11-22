#ifndef __RTC_H__
#define __RTC_H__

#include "at32f4xx.h"

#define RTC_DEFAULT_CFG \
{\
	2020,8,25,2,12,59,0 \
}

//时间结构体
//公历年月日周
typedef struct 
{
  unsigned short int year;	
  unsigned char  month;
  unsigned char  day;
  unsigned char  week;
  unsigned char  hour;	
  unsigned char  min;  
  unsigned char  sec;
}rtc_time_st;  

extern rtc_time_st rtc_time;

void rtc_cfg_init(void);
void rtc_init(void *arg);
unsigned char is_leapyear(unsigned short  pyear);
unsigned char rtc_data_set(void *arg);
void rtc_data_get(void *arg);

void hw_rtc_thread_entry(void *parameter);

#endif

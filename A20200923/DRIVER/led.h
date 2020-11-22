
#ifndef __LED_H__
#define __LED_H__


#include "pindef.h"

typedef struct
{
  void (*sw)(unsigned char);
	
}drv_st;

typedef struct
{
	void (*gpio_init)(void);
  drv_st  red,blue;
}led_st;

extern led_st led;

void led_gpio_init(void);
void red_red_drv(unsigned char sw_value);
void blue_red_drv(unsigned char sw_value);
void led_func(void *arg,unsigned char sw_value);
void led_init(led_st *const led_me);
void led_thread_entry(void *parameter);
#endif



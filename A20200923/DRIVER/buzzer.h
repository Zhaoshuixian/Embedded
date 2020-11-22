#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "pindef.h" 

typedef struct
{
	unsigned short int conter; //蜂鸣间隔累加器
	unsigned char num:7; //蜂鸣次数
	unsigned char en:1;//运行状态
	unsigned int rt;//运行时间
}buzzer_st;

extern buzzer_st buzzer;

#define BUZZER_DEFAULT_CFG \
{\
	.conter=0,\
	.num=2,\
	.en =1,\
	.rt=500 \
}

//IO操作函数	
#define BUZZER(x)    BEEP_SW(x)

void buzzer_init(void *arg); //蜂鸣器初始化
void buzzer_start(void *arg,unsigned char bnum,unsigned int btime);
void buzzer_loudly(unsigned char x);

#endif 



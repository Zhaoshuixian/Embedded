#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "pindef.h" 

typedef struct
{
	unsigned short int conter; //��������ۼ���
	unsigned char num:7; //��������
	unsigned char en:1;//����״̬
	unsigned int rt;//����ʱ��
}buzzer_st;

extern buzzer_st buzzer;

#define BUZZER_DEFAULT_CFG \
{\
	.conter=0,\
	.num=2,\
	.en =1,\
	.rt=500 \
}

//IO��������	
#define BUZZER(x)    BEEP_SW(x)

void buzzer_init(void *arg); //��������ʼ��
void buzzer_start(void *arg,unsigned char bnum,unsigned int btime);
void buzzer_loudly(unsigned char x);

#endif 



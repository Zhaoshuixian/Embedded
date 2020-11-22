

#ifndef __INFRADE_R_H__
#define __INFRADE_R_H__

#include "pindef.h"

/*****遥控键值代码宏****/
#define  PWR_SW   0XA2
#define  MENU     0XE2
#define  TEST     0X22
#define  ADD      0X02
#define  REDUCE   0X98
#define  RECALL   0XC2
#define  PREVIOUS 0XE0
#define  PLAY     0XA8
#define  NEXT     0X90
#define  CANCEL   0XB0
#define  ZERO     0X68
#define  ONE      0X30
#define  TWO      0X18
#define  THREE    0X7A
#define  FOUR     0X10
#define  FIVE     0X38
#define  SIX      0X5A
#define  SEVEN    0X42
#define  EIGHT    0X4A
#define  NINE     0X52

/***************************/

typedef struct
{
	#define MAX_EDGE_NUM (256) /*可记录的最多波形边沿数目*/
	
	unsigned char status;
	unsigned char wave_buff[MAX_EDGE_NUM];
	unsigned int  edge_cnt;
	unsigned int  wave_time;
	unsigned int  timeout;
}study_st;//其他解码

typedef struct
{
  unsigned char cmd;
  unsigned char cmdn;
  unsigned char addr;
  unsigned char addrn;
}data_st;

typedef struct
{
  unsigned char rec_num;
  unsigned char valid_data;
  unsigned char rec_status;
  unsigned int  rec_data;
  unsigned int  rec_dval;
  data_st data;	
}nec_st;//NEC解码

typedef struct
{ 
  nec_st   nec;
	study_st study;
}ir_st;

#define IR_NEC_IRQHandler    TMR4_GLOBAL_IRQHandler
#define IR_STUDY_IRQHandler  EXTI9_5_IRQHandler

extern ir_st ir;

extern void infrared_rx_init(void);
extern void ir_thread_entry(void *parameter);

#endif


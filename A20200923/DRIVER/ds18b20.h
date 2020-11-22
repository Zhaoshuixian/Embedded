

#ifndef __DS18B20_H__
#define	__DS18B20_H__

#include "pindef.h"

typedef enum
{
	POSITIVE=0,//��
	NEGATIVE, //��
}polar_em;

typedef struct
{
  void (*gpio_init)(void);         //GPIO��ʼ��
  void (*gpio_set)(unsigned char);   //����
  unsigned char (*gpio_read)(void); //��ȡ���ŵ�ƽ�ź�
	void (*delay_us)(unsigned int);    //��ʱ	
}gpio_drv_st;


typedef struct
{
  polar_em polar; //�¶ȼ��ԣ�0������1������
  signed int  data;	    //�¶�����
  gpio_drv_st one_wire;
}ds18b20_st;

extern ds18b20_st ds18b20;

#define  SkipROM         0xCC    //����ROM
#define  SearchROM       0xF0    //����ROM
#define  ReadROM         0x33    //��ROM
#define  MatchROM        0x55    //ƥ��ROM
#define  AlarmROM        0xEC    //�澯ROM

#define  StartConvert    0x44    //��ʼ�¶�ת�������¶�ת���ڼ����������0��ת�����������1
#define  ReadScratchpad  0xBE    //���ݴ�����9���ֽ�
#define  WriteScratchpad 0x4E    //д�ݴ������¶ȸ澯TH��TL
#define  CopyScratchpad  0x48    //���ݴ������¶ȸ澯���Ƶ�EEPROM���ڸ����ڼ����������0������������1
#define  RecallEEPROM    0xB8    //��EEPROM���¶ȸ澯���Ƶ��ݴ����У������ڼ����0��������ɺ����1
#define  ReadPower       0xB4    //����Դ�Ĺ��緽ʽ��0Ϊ������Դ���磻1Ϊ�ⲿ��Դ����

#define Precision         0x7F   //�������üĴ��� 1f=9λ; 3f=10λ; 5f=11λ; 7f=12λ;
#define AlarmTH           (AlarmTemp>>8)
#define AlarmTL           (AlarmTemp&0xFF)
#define CONVERT_TICK      1000 //�¶�ת�����ʱ��
#define AlarmTemp         37*Convert_K 
#define Convert_K         16   //(1/0.0625)


#define BUS_CFG {\
	.gpio_init  =ds_gpio_init,\
	.gpio_set   =ds_set,\
	.gpio_read  =ds_read,\
	.delay_us   =xdelay_us,\
}

void ds_gpio_init(void);
void ds_set(unsigned char s);
unsigned char ds_read(void);
void ds18b20_init(void *arg);

//__WEAK 
void tsensor_init(void);
//__WEAK 
void tsensor_thread_entry(void *parameter);

#endif


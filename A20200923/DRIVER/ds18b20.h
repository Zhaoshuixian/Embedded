

#ifndef __DS18B20_H__
#define	__DS18B20_H__

#include "pindef.h"

typedef enum
{
	POSITIVE=0,//正
	NEGATIVE, //负
}polar_em;

typedef struct
{
  void (*gpio_init)(void);         //GPIO初始化
  void (*gpio_set)(unsigned char);   //拉高
  unsigned char (*gpio_read)(void); //获取引脚电平信号
	void (*delay_us)(unsigned int);    //延时	
}gpio_drv_st;


typedef struct
{
  polar_em polar; //温度极性（0：正；1：负）
  signed int  data;	    //温度数据
  gpio_drv_st one_wire;
}ds18b20_st;

extern ds18b20_st ds18b20;

#define  SkipROM         0xCC    //跳过ROM
#define  SearchROM       0xF0    //搜索ROM
#define  ReadROM         0x33    //读ROM
#define  MatchROM        0x55    //匹配ROM
#define  AlarmROM        0xEC    //告警ROM

#define  StartConvert    0x44    //开始温度转换，在温度转换期间总线上输出0，转换结束后输出1
#define  ReadScratchpad  0xBE    //读暂存器的9个字节
#define  WriteScratchpad 0x4E    //写暂存器的温度告警TH和TL
#define  CopyScratchpad  0x48    //将暂存器的温度告警复制到EEPROM，在复制期间总线上输出0，复制完后输出1
#define  RecallEEPROM    0xB8    //将EEPROM的温度告警复制到暂存器中，复制期间输出0，复制完成后输出1
#define  ReadPower       0xB4    //读电源的供电方式：0为寄生电源供电；1为外部电源供电

#define Precision         0x7F   //精度配置寄存器 1f=9位; 3f=10位; 5f=11位; 7f=12位;
#define AlarmTH           (AlarmTemp>>8)
#define AlarmTL           (AlarmTemp&0xFF)
#define CONVERT_TICK      1000 //温度转换间隔时间
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


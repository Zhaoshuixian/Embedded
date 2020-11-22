#ifndef __PCF8563_H
#define __PCF8563_H	 


#include "myiic.h"
#include "pindef.h"

#ifndef NULL
#define NULL	((void *)0)
#endif

#define COMPILE_STR_DATE  (__DATE__)
#define COMPILE_STR_TIME  (__TIME__)

//编译时间
#define SM(o,s,m)               (((o)[0] == (s)[0]) && ((o)[1] == (s)[1]) && (o)[2] == (s)[2]) ? m
#define STRM_TO_DEC(s)          SM("Jan",s,0x01) : SM("Feb",s,0x02) : SM("Mar",s,0x03) : SM("Apr",s,0x04) :\
                                SM("May",s,0x05) : SM("Jun",s,0x06) : SM("Jul",s,0x07) : SM("Aug",s,0x08) :\
                                SM("Sep",s,0x09) : SM("Oct",s,0x0A) : SM("Nov",s,0x0B) : SM("Dec",s,0x0C) : '?'


// PCF8563寄存器定义
#define			CTRL_STU1				0X00						//控制状态寄存器1
#define			CTRL_STU2				0X01						//控制状态寄存器2
#define			SEC_REG					0X02						//秒寄存器
#define			MIN_REG					0X03						//分寄存器
#define			HOUR_REG					0X04						//小时寄存器
#define			DAY_REG					0X05						//日寄存器
#define			WEEK_REG					0X06						//周寄存器
#define			MON_REG					0X07						//月寄存器		
#define			YEAR_REG					0X08						//年寄存器
#define			ALARM_MIN_REG			0X09						//分钟报警寄存器	
#define			ALARM_HOUR_REG			0X0A						//小时报警寄存器
#define			ALARM_DAY_REG			0X0B						//日报警寄存器
#define			ALARM_WEEK_REG			0X0C						//周报警寄存器
#define			CLKOUT_REG				0X0D						//时钟输出寄存器
#define			TIMER_REG				0X0E						//定时器控制寄存器
#define			COUNTER_REG				0X0F						//倒数计数器控制寄存器
//地址定义
#define			READ_ADDR				  0XA3						//8563读操作地址
#define			WRITE_ADDR				0XA2						//8563写操作地址
//命令定义
#define			START_CLOCK				0x00						//启动时钟
#define AF (control2&0x1f)

#define PCF8563_RD_ADDR	(0xA3)
#define PCF8563_WR_ADDR	(0xA2) 

//时间读取结构体
typedef struct 
{
	u16 year;
	u8  mon;
	u8  day;
	u8  week;	
	u8  hour;
	u8  min;
	u8  sec;			
	//公历日月年周	
}rtc_st;

//for pcf8563
#define IIC2_SCL(x)      (x==0?GPIO_ResetBits(PCF8563_PORT,PCF8563_SCL_PIN):GPIO_SetBits(PCF8563_PORT,PCF8563_SCL_PIN)) //SCL
#define IIC2_SDA(x)      (x==0?GPIO_ResetBits(PCF8563_PORT,PCF8563_SDA_PIN):GPIO_SetBits(PCF8563_PORT,PCF8563_SDA_PIN))  //SDA	 
#define IIC2_READ_SDA()  GPIO_ReadInputDataBit(PCF8563_PORT,PCF8563_SDA_PIN)
#define SDA_IN2()  	     {PCF8563_PORT->CTRLH &=0xFFFF0FFF;PCF8563_PORT->CTRLH|=((unsigned int)8<<3*4);}//PC11-SDA //PC10-SCL
#define SDA_OUT2() 	     {PCF8563_PORT->CTRLH &=0xFFFF0FFF;PCF8563_PORT->CTRLH|=(unsigned int)(3<<3*4);}
//IO操作函数	 
#define IIC_SCL2(x)    IIC2_SCL(x) //SCL
#define IIC_SDA2(x)    IIC2_SDA(x)  //SDA	 
#define READ_SDA2      IIC2_READ_SDA()  //输入SDA 

//-------------实现接口--------------
void pcf8563_gpio_init(void);
void pcf8563_sda_mode(unsigned char s);
void pcf8563_sda_out(unsigned char s);
void pcf8563_sck_out(unsigned char s);
unsigned char pcf8563_sda_read(void);
//-----------------END-------------

unsigned char pcf8563_rtc_init(void);
unsigned char pcf8563_rtc_set(rtc_st *const rtc_me);
unsigned char pcf8563_rtc_get(rtc_st *const rtc_me);
unsigned char pcf8563_rtc_week_get(rtc_st *const rtc_me);
void rtc_thread_entry(void *parameter);

 
#endif


#ifndef __PCF8563_H
#define __PCF8563_H	 


#include "myiic.h"
#include "pindef.h"

#ifndef NULL
#define NULL	((void *)0)
#endif

#define COMPILE_STR_DATE  (__DATE__)
#define COMPILE_STR_TIME  (__TIME__)

//����ʱ��
#define SM(o,s,m)               (((o)[0] == (s)[0]) && ((o)[1] == (s)[1]) && (o)[2] == (s)[2]) ? m
#define STRM_TO_DEC(s)          SM("Jan",s,0x01) : SM("Feb",s,0x02) : SM("Mar",s,0x03) : SM("Apr",s,0x04) :\
                                SM("May",s,0x05) : SM("Jun",s,0x06) : SM("Jul",s,0x07) : SM("Aug",s,0x08) :\
                                SM("Sep",s,0x09) : SM("Oct",s,0x0A) : SM("Nov",s,0x0B) : SM("Dec",s,0x0C) : '?'


// PCF8563�Ĵ�������
#define			CTRL_STU1				0X00						//����״̬�Ĵ���1
#define			CTRL_STU2				0X01						//����״̬�Ĵ���2
#define			SEC_REG					0X02						//��Ĵ���
#define			MIN_REG					0X03						//�ּĴ���
#define			HOUR_REG					0X04						//Сʱ�Ĵ���
#define			DAY_REG					0X05						//�ռĴ���
#define			WEEK_REG					0X06						//�ܼĴ���
#define			MON_REG					0X07						//�¼Ĵ���		
#define			YEAR_REG					0X08						//��Ĵ���
#define			ALARM_MIN_REG			0X09						//���ӱ����Ĵ���	
#define			ALARM_HOUR_REG			0X0A						//Сʱ�����Ĵ���
#define			ALARM_DAY_REG			0X0B						//�ձ����Ĵ���
#define			ALARM_WEEK_REG			0X0C						//�ܱ����Ĵ���
#define			CLKOUT_REG				0X0D						//ʱ������Ĵ���
#define			TIMER_REG				0X0E						//��ʱ�����ƼĴ���
#define			COUNTER_REG				0X0F						//�������������ƼĴ���
//��ַ����
#define			READ_ADDR				  0XA3						//8563��������ַ
#define			WRITE_ADDR				0XA2						//8563д������ַ
//�����
#define			START_CLOCK				0x00						//����ʱ��
#define AF (control2&0x1f)

#define PCF8563_RD_ADDR	(0xA3)
#define PCF8563_WR_ADDR	(0xA2) 

//ʱ���ȡ�ṹ��
typedef struct 
{
	u16 year;
	u8  mon;
	u8  day;
	u8  week;	
	u8  hour;
	u8  min;
	u8  sec;			
	//������������	
}rtc_st;

//for pcf8563
#define IIC2_SCL(x)      (x==0?GPIO_ResetBits(PCF8563_PORT,PCF8563_SCL_PIN):GPIO_SetBits(PCF8563_PORT,PCF8563_SCL_PIN)) //SCL
#define IIC2_SDA(x)      (x==0?GPIO_ResetBits(PCF8563_PORT,PCF8563_SDA_PIN):GPIO_SetBits(PCF8563_PORT,PCF8563_SDA_PIN))  //SDA	 
#define IIC2_READ_SDA()  GPIO_ReadInputDataBit(PCF8563_PORT,PCF8563_SDA_PIN)
#define SDA_IN2()  	     {PCF8563_PORT->CTRLH &=0xFFFF0FFF;PCF8563_PORT->CTRLH|=((unsigned int)8<<3*4);}//PC11-SDA //PC10-SCL
#define SDA_OUT2() 	     {PCF8563_PORT->CTRLH &=0xFFFF0FFF;PCF8563_PORT->CTRLH|=(unsigned int)(3<<3*4);}
//IO��������	 
#define IIC_SCL2(x)    IIC2_SCL(x) //SCL
#define IIC_SDA2(x)    IIC2_SDA(x)  //SDA	 
#define READ_SDA2      IIC2_READ_SDA()  //����SDA 

//-------------ʵ�ֽӿ�--------------
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


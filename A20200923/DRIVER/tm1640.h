#ifndef __TM1640_H__
#define __TM1640_H__	 


#include "at32f4xx.h"
#include "myiic.h"


//----------------------TM1640-----------------------

//�Դ��ַ
#define GRAD1 (0x00)
#define GRAD2 (0x01)
#define GRAD3 (0x02)
#define GRAD4 (0x03)
#define GRAD5 (0x04)
#define GRAD6 (0x05)
#define GRAD7 (0x06)
#define GRAD8 (0x07)
#define GRAD9 (0x08)
#define GRAD10 (0x09)
#define GRAD11 (0x0A)
#define GRAD12 (0x0B)
//.......
#define GRAD16 (0x0F)
//����ģʽ
#define DATA_CMD (0x40) /*������������*/
#define DIS_CMD  (0x80) /*��ʾ������������*/
#define ADDR_CMD (0xC0) /*��ַ��������*/


//---------------------------------------------

#define IIC3_SCL(x)    (x==0?GPIO_ResetBits(TM_PORT,TM_SCL_PIN):GPIO_SetBits(TM_PORT,TM_SCL_PIN)) //SCL
#define IIC3_SDA(x)    (x==0?GPIO_ResetBits(TM_PORT,TM_SDA_PIN):GPIO_SetBits(TM_PORT,TM_SDA_PIN))  //SDA	 
#define IIC3_READ_SDA()  GPIO_ReadInputDataBit(TM_PORT,TM_SDA_PIN)

#define SDA3_IN     {TM_PORT->CTRLH &=0xF0FFFFFF;TM_PORT->CTRLH|=((unsigned int)8<<6*4);}//PC14--SDA 
#define SDA3_OUT    {TM_PORT->CTRLH &=0xF0FFFFFF;TM_PORT->CTRLH|=(unsigned int)(3<<6*4);}

#define READ_SDA3   IIC3_READ_SDA() 
#define IIC_SDA3(n) IIC3_SDA(n)
#define IIC_SCL3(n) IIC3_SCL(n) 

//------------��1��LED--------
#define LED1_RED		(1<<0)  //�ȱÿ���
#define LED1_GREEN	(1<<1) 
#define LED2_RED		(1<<2) 	//�յ�1����
#define LED2_GREEN	(1<<3)
#define LED3_RED		(1<<4)	//�յ�1�ػ�
#define LED3_GREEN	(1<<5)
#define LED7				(1<<6)	//��Դ
//------------��2��LED--------
#define LED4_RED		(1<<0)	//
#define LED4_GREEN	(1<<1)	//�ǿ�
#define LED5_RED		(1<<2)	//
#define LED5_GREEN	(1<<3)	//����
#define LED6_RED		(1<<4)	//
#define LED6_GREEN	(1<<5)	//��ͣ
#define LED8				(1<<6)	//ͨѶ


#define HOTPUMP_STATUS_LED (LED1_RED)
#define MAIN_BOARD_PWR_LED (LED7) /*�����Դָʾ��*/
#define MAIN_BOARD_COM_LED (LED8)/*����IOTͨѶָʾ��*/
#define WATTANK_STATUS_LED (LED5_RED)

//-------------ʵ�ֽӿ�--------------
void tm1640_gpio_init(void);
void tm1640_sda_mode(unsigned char s);
void tm1640_sda_out(unsigned char s);
void tm1640_sck_out(unsigned char s);
unsigned char tm1640_sda_read(void);
//-----------------END-------------


void tm1640_init(void);
void disboard_thread_entry(void *parameter);

#endif


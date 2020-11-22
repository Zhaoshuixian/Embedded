
#ifndef __MYIIC_H
#define __MYIIC_H

//----------i2c�Խӽӿں���ָ��-------
typedef struct
{
	unsigned char trans_mode;//����ģʽ��0:Ĭ���Ǵ�MSB��ʼ���䣨ͨ���ͣ���1����LSB��ʼ���䣨�ر�
	void (*gpio_init)(void);
	void (*sda_mode)(unsigned char );	
	unsigned char  (*sda_in)(void);
	void (*sda_out)(unsigned char );
	void (*sck_out)(unsigned char );
	void (*delay_us)(unsigned int);
	void (*delay_ms)(unsigned int);	
}i2c_st;

//-----------i2c��׼�ӿ�---------
void iic_start(i2c_st *const i2c_me);
void iic_stop(i2c_st *const i2c_me);
unsigned char iic_wait_ack(i2c_st *const i2c_me);
void iic_ack(i2c_st *const i2c_me);
void iic_nack(i2c_st *const i2c_me);
void iic_delayms(i2c_st *const i2c_me,unsigned short int s);
void iic_sendbyte(i2c_st *const i2c_me,unsigned char tbyte);
unsigned char iic_readbyte(i2c_st *const i2c_me,unsigned char ack);
void iic_init(i2c_st *const i2c_me);
//-----------------END-----------------
#endif


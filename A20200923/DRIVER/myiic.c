
#include "myiic.h"


//TM1640�Ǵ�LSB���䣬��ͬ������IIC�豸��MSB���� ��������Ҫת������λ����

unsigned char data_bitreverse(unsigned char data)
{
	static unsigned char temp_data=0;
	for(unsigned char i=0;i<8;i++)//MSB
	{
    temp_data<<=1;
		temp_data^=data&0x01;
		data>>=1;
	}
	return temp_data;
}	


//--------------��׼I2CЭ����----------------------------

void iic_start(i2c_st *const i2c_me)
{
	i2c_me->sda_mode(1);
	i2c_me->sda_out(1);
	i2c_me->sck_out(1);		
	i2c_me->delay_us(4);
	i2c_me->sda_out(0);	
	i2c_me->delay_us(4);
	i2c_me->sck_out(0);	
}

void iic_stop(i2c_st *const i2c_me)
{
	i2c_me->sda_mode(1);//sda�����
	i2c_me->sck_out(0);		
	i2c_me->sda_out(0);//STOP:when CLK is high DATA change form low to high
	i2c_me->delay_us(4);
	i2c_me->sck_out(1);		
	i2c_me->sda_out(1);//����I2C���߽����ź�
	i2c_me->delay_us(4);
}

unsigned char iic_wait_ack(i2c_st *const i2c_me)
{
	unsigned char ucErrTime=0;
	
	i2c_me->sda_mode(0);      //SDA����Ϊ����  

	i2c_me->sda_out(1);
	i2c_me->delay_us(1);		   
	i2c_me->sck_out(1);
	i2c_me->delay_us(1);		 
	while(i2c_me->sda_in())
	{
		if(ucErrTime++>250)
		{
			iic_stop(i2c_me);
			return 1;
		}
	}
	i2c_me->sck_out(0);//ʱ�����0 	   
	return 0;  	
}

void iic_ack(i2c_st *const i2c_me)
{
	i2c_me->sck_out(0);
	i2c_me->sda_mode(1);
	i2c_me->sda_out(0);
	i2c_me->delay_us(2);	
	i2c_me->sck_out(1);
	i2c_me->delay_us(2);	
	i2c_me->sck_out(0);
}

void iic_nack(i2c_st *const i2c_me)
{
	i2c_me->sck_out(0);
	i2c_me->sda_mode(1);
	i2c_me->sda_out(1);
	i2c_me->delay_us(2);	
	i2c_me->sck_out(1);
	i2c_me->delay_us(2);	
	i2c_me->sck_out(0);	
}

void iic_delayms(i2c_st *const i2c_me,unsigned short int s)
{
	i2c_me->delay_ms(s);
}


void iic_sendbyte(i2c_st *const i2c_me,unsigned char tbyte)
{		
	unsigned char t,temp_tbyte=tbyte;
	if(i2c_me->trans_mode) //LSBģʽ
	temp_tbyte=data_bitreverse(tbyte);//λ��ߵ�
	//Ĭ����MSBλ��ʼ����
	i2c_me->sda_mode(1); 	 
	i2c_me->sck_out(0);	//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{     			
		i2c_me->sda_out((temp_tbyte&0x80)>>7);
		temp_tbyte<<=1; 
		i2c_me->delay_us(2);	  
		i2c_me->sck_out(1);
		i2c_me->delay_us(2);	
		i2c_me->sck_out(0);		
		i2c_me->delay_us(2);	
	}	 	
}

unsigned char iic_readbyte(i2c_st *const i2c_me,unsigned char ack)
{
	unsigned char i,receive=0;
  i2c_me->sda_mode(0); 	//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
		i2c_me->sck_out(0); 
		i2c_me->delay_us(2);	
		i2c_me->sck_out(1);
		receive<<=1;
		if(i2c_me->sda_in())
		{
			receive++; 
		}					
		i2c_me->delay_us(2);	
  }					 
    if (!ack) iic_nack(i2c_me);//����nACK
    else      iic_ack(i2c_me); //����ACK   
    return receive;	
}

void iic_init(i2c_st *const i2c_me)
{
	i2c_me->gpio_init();
	i2c_me->sck_out(1);
	i2c_me->sda_out(1);	
}


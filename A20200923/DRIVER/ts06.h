

#ifndef __TS06_H__
#define __TS06_H__

#include "pindef.h"
#include "myiic.h"
#include "delay.h"

#define TS06_SLAVE_ADDR_WR   0XD2
#define TS06_SLAVE_ADDR_RD   (TS06_SLAVE_ADDR_WR+1)

/*寄存器地址*/
#define SEN1_ADDR         0X00
#define SEN2_ADDR         0X01
#define SEN3_ADDR         0X02
#define CTRL1_ADDR        0X03
#define CTRL2_ADDR        0X04
#define REF_RST_ADDR      0X05
#define CH_HOLD_ADDR      0X06
#define CAL_HOLD_ADDR     0X07/*校准寄存器 */
#define DEMO_EN_ADDR      0X08
#define OUTPUT_REG_ADDR   0X25 /*键值输出寄存器 */

/*寄存器配置值*/
#define  SEN1_VAL        0X11 /*CH1&CH2 Touch Sensitivity set*/
#define  SEN2_VAL        0X11 /*CH3&CH4 Touch Sensitivity set*/
#define  SEN3_VAL        0X11 /*CH3&CH4 Touch Sensitivity set*/
#define  CTRL1_VAL       0x0A /* */
#define  CTRL2_VAL       0x12 /* */
#define  REF_RST_VAL     0x00 /* */
#define  CH_HOLD_VAL     0x00  /*使能按键通道CH1~CH4*/
#define  CAL_HOLD_VAL    0x00 /*Enable Refer SensTouch and Cal*/
#define  CAL_CTRL        0xFD /* */
#define  DEMO_EN         0x00 /* */


#define TS06_SCL(x)      (x==0?GPIO_ResetBits(TS06_SCL_PORT,TS06_SCL_PIN):GPIO_SetBits(TS06_SCL_PORT,TS06_SCL_PIN)) //SCL
#define TS06_SDA(x)      (x==0?GPIO_ResetBits(TS06_SDA_PORT,TS06_SDA_PIN):GPIO_SetBits(TS06_SDA_PORT,TS06_SDA_PIN))  //SDA	 
#define TS06_READ_SDA()  GPIO_ReadInputDataBit(TS06_SDA_PORT,TS06_SDA_PIN)

#define TS06_SDA_IN()    {TS06_SDA_PORT->CTRLH&=0xFFFFFFF0;TS06_SDA_PORT->CTRLH|=((unsigned int)8<<0*4);}//PB8-SDA//PC0-SCL
#define TS06_SDA_OUT()   {TS06_SDA_PORT->CTRLH&=0xFFFFFFF0;TS06_SDA_PORT->CTRLH|=(unsigned int)(3<<0*4);}


typedef enum
{
	NONE_TK=0,
  TK1,	
  TK2,	
  TK3,	
  TK4,		
}key_em;

typedef struct 
{
	unsigned char last:1;	
	unsigned char now:1;
	unsigned char :6;
}status_st;

typedef struct 
{
	i2c_st i2c;
	key_em read_key,hold_key,trig_key;//键值：当前读取值/保持键值/触发键值
	status_st status;//按键状态
	unsigned int hold_time;//按键按下保持时长
}tkey_st;
 
extern tkey_st ts06;

#define TS06_I2C_BUS_CFG \
{\
	.trans_mode = 0,\
	.gpio_init  = ts06_gpio_init,\
	.sda_mode   = ts06_sda_mode,\
	.sda_in     = ts06_sda_read,\
	.sda_out    = ts06_sda_out,\
	.sck_out    = ts06_sck_out,\
	.delay_us   = xdelay_us,\
	.delay_ms   = xdelay_ms	\
}


void ts06_gpio_init(void);
void ts06_sda_mode(unsigned char s);
void ts06_sda_out(unsigned char s);
void ts06_sck_out(unsigned char s);
unsigned char ts06_sda_read(void);
void ts06_init(void *arg);

void key_thread_entry(void *parameter);
#endif



#ifndef __24CXX_H
#define __24CXX_H

#include "myiic.h"
#include "pindef.h"
#include "delay.h"

//24CXX��������(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)

typedef enum
{
	AT_NULL=0,
	AT24C01,  /*127*/
	AT24C02,  /*255*/
	AT24C04,  /*511*/
	AT24C08,  /*1023*/
	AT24C16	,	/*2047*/
	AT24C32	,	/*4095*/
	AT24C64	, /*8191*/
	AT24C128,	/*16383*/
	AT24C256,	/*32767*/  	
}at24c_type;

typedef struct
{
	at24c_type type;
	i2c_st i2c;
}at24c_device_st;//���豸

//MSB��ʼ����
#define EE_I2C_BUS_CFG {\
.trans_mode =0,\
.gpio_init  = at24c_gpio_init,\
.sda_mode   = at24c_sda_mode,\
.sda_in     = at24c_sda_read,\
.sda_out    = at24c_sda_out,\
.sck_out    = at24c_sck_out,\
.delay_us   = xdelay_us,\
.delay_ms   = xdelay_ms,\
}

#define NV_EE_SIZE  (0xFF+1)

extern unsigned char AT24X_FLASH_BUF[NV_EE_SIZE];

//------NV_DATA_ADDR TAB(����ʧ�����ݴ洢��ַ����)-----------------
#define NV_EE_START_ADDR        (0x00) /*��ʼ��ַ*/
#define NV_EE_END_ADDR          (0xFF) /*������ַ*/
/*RS485���ݵ�*/
#define NV_RS485_ADDR_ADDR      (0x09)/*RS485��ַ��Ϣ*/
#define NV_RS485_BAUD_ADDR      (0x0A)/*RS485��������Ϣ*/
/*ˮ�����ݵ�*/
#define NV_TANK_TEMPER_TH_ADDR    (0x0B)/*ˮ�������¶���ֵ*/
#define NV_TANK_UPWATLEV_TH_ADDR  (0x0C)/*ˮ������ˮλ��ֵ*/
#define NV_TANK_DWWATLEV_TH_ADDR  (0x0D)/*ˮ������ˮλ��ֵ*/
#define NV_TANK_FULLSCALE_HB_ADDR (0x0E)/*ˮ��ˮλ�����̸��ֽ�*/
#define NV_TANK_FULLSCALE_LB_ADDR (0x0F)/*ˮ��ˮλ�����̵��ֽ�*/
/*�ȱ����ݵ�*/
#define NV_HOTPUMP_TEMPER_TH_ADDR    (0x10)/*�ȱ��¶���ֵ*/
#define NV_HOTPUMP_WATPRESS_TH_ADDR  (0x11)/*�ȱ�ˮѹ��ֵ��Ϣ*/
#define NV_HOTPUMP_WORKCURR_TH_ADDR  (0x12)/*�ȱ���ͣ��ֵ����*/
/*�߿�������ʱ������ݵ�*/
#define NV_WCB_MON_HOUR_ON_ADDR      (0x17)/*�߿�����һ����Сʱ*/
#define NV_WCB_MON_MIN_ON_ADDR       (0x18)/*�߿�����һ��������*/

#define NV_WCB_MON_HOUR_OFF_ADDR     (0x19)/*�߿�����һ�ر�Сʱ*/
#define NV_WCB_MON_MIN_OFF_ADDR      (0x1A)/*�߿�����һ�رշ���*/

#define NV_WCB_TUS_HOUR_ON_ADDR      (0x1B)/*�߿����ܶ�����Сʱ*/
#define NV_WCB_TUS_MIN_ON_ADDR       (0x1C)/*�߿����ܶ���������*/

#define NV_WCB_TUS_HOUR_OFF_ADDR     (0x1D)/*�߿����ܶ��ر�Сʱ*/
#define NV_WCB_TUS_MIN_OFF_ADDR      (0x1E)/*�߿����ܶ��رշ���*/

#define NV_WCB_WED_HOUR_ON_ADDR      (0x1F)/*�߿�����������Сʱ*/
#define NV_WCB_WED_MIN_ON_ADDR       (0x20)/*�߿���������������*/

#define NV_WCB_WED_HOUR_OFF_ADDR     (0x21)/*�߿��������ر�Сʱ*/
#define NV_WCB_WED_MIN_OFF_ADDR      (0x22)/*�߿��������رշ���*/

#define NV_WCB_THR_HOUR_ON_ADDR      (0x23)/*�߿������Ŀ���Сʱ*/
#define NV_WCB_THR_MIN_ON_ADDR       (0x24)/*�߿������Ŀ�������*/

#define NV_WCB_THR_HOUR_OFF_ADDR     (0x25)/*�߿������Ĺر�Сʱ*/
#define NV_WCB_THR_MIN_OFF_ADDR      (0x26)/*�߿������Ĺرշ���*/

#define NV_WCB_FRI_HOUR_ON_ADDR      (0x27)/*�߿������忪��Сʱ*/
#define NV_WCB_FRI_MIN_ON_ADDR       (0x28)/*�߿������忪������*/

#define NV_WCB_FRI_HOUR_OFF_ADDR     (0x29)/*�߿�������ر�Сʱ*/
#define NV_WCB_FRI_MIN_OFF_ADDR      (0x2A)/*�߿�������رշ���*/

#define NV_WCB_SAT_HOUR_ON_ADDR      (0x2B)/*�߿�����������Сʱ*/
#define NV_WCB_SAT_MIN_ON_ADDR       (0x2C)/*�߿���������������*/

#define NV_WCB_STA_HOUR_OFF_ADDR     (0x2D)/*�߿��������ر�Сʱ*/
#define NV_WCB_STA_MIN_OFF_ADDR      (0x2E)/*�߿��������رշ���*/

#define NV_WCB_SUN_HOUR_ON_ADDR      (0x2F)/*�߿������տ���Сʱ*/
#define NV_WCB_SUN_MIN_ON_ADDR       (0x30)/*�߿������տ�������*/

#define NV_WCB_SUN_HOUR_OFF_ADDR     (0x31)/*�߿������չر�Сʱ*/
#define NV_WCB_SUN_MIN_OFF_ADDR      (0x32)/*�߿������չرշ���*/

#define NV_METER_CURR_BIAS_CALIB_ADDR (0x33)/*����оƬ����ƫ��У׼*/
#define NV_METER_CURR_BIAS_CALIB_LEN  (3) 
#define NV_METER_VOLT_BIAS_CALIB_ADDR (0x36)/*����оƬ��ѹƫ��У׼*/
#define NV_METER_VOLT_BIAS_CALIB_LEN  (3) 
#define NV_METER_CURR_GAIN_CALIB_ADDR (0x39)/*����оƬ��������У׼*/
#define NV_METER_CURR_GAIN_CALIB_LEN  (3) 
#define NV_METER_VOLT_GAIN_CALIB_ADDR (0x3c)/*����оƬ��ѹ����У׼*/
#define NV_METER_VOLT_GAIN_CALIB_LEN  (3) 
#define NV_METER_PWR_BIAS_CALIB_ADDR  (0x3F)/*����оƬ����ƫ��У׼*/
#define NV_METER_PWR_BIAS_CALIB_LEN   (3)
#define NV_METER_DATA_ERROR_ADDR      (0x42)/*����оƬ��������*/
#define NV_METER_DATA_ERROR_LEN       (3)
#define NV_METER_COMPENSATION_ADDR    (0x45)/*����оƬ��λ����*/
#define NV_METER_COMPENSATION_LEN     (1)

#define TOTAL_CFG_LEN                (NV_WCB_SUN_MIN_OFF_ADDR-NV_RS485_ADDR_ADDR+1)/*�����������ֽ�����*/

#define NV_EEPROM_HAS_CFG_ADDR       (NV_EE_END_ADDR)/*EEPROM����������(���״�ʹ��)*/
//-----------------------------------END--------------------------

//for 24cxx
#define AT24C_SCL(x)      (x==0?GPIO_ResetBits(AT24CXX_PORT,AT24CXX_SCL_PIN):GPIO_SetBits(AT24CXX_PORT,AT24CXX_SCL_PIN)) //SCL
#define AT24C_SDA(x)      (x==0?GPIO_ResetBits(AT24CXX_PORT,AT24CXX_SDA_PIN):GPIO_SetBits(AT24CXX_PORT,AT24CXX_SDA_PIN))  //SDA	 

#define AT24C_READ_SDA()  GPIO_ReadInputDataBit(AT24CXX_PORT,AT24CXX_SDA_PIN)
#define AT24C_SDA_IN()    {AT24CXX_PORT->CTRLH&=0xFFFFFF0F;AT24CXX_PORT->CTRLH|=((unsigned int)8<<1*4);}//PC9-SDA//PC8-SCL
#define AT24C_SDA_OUT()   {AT24CXX_PORT->CTRLH&=0xFFFFFF0F;AT24CXX_PORT->CTRLH|=(unsigned int)(3<<1*4);}


//-------------ʵ�ֽӿ�--------------
void at24c_gpio_init(void);
void at24c_sda_mode(unsigned char s);
void at24c_sda_out(unsigned char s);
void at24c_sck_out(unsigned char s);
unsigned char at24c_sda_read(void);
//-----------------END-------------		

extern at24c_device_st at24c02;


void at24c_device_register(at24c_type dev_type,at24c_device_st *const at24c_device);
unsigned char at24c_device_check(at24c_device_st *const at24c_device);
	

unsigned char at24c_readbyte(at24c_device_st *const at24c_device,unsigned short int reg_addr);
void at24c_writebyte(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char wdata);
void at24c_writenbytes(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char *wdata,unsigned char wdata_len);
unsigned int at24c_readnbytes(at24c_device_st *const at24c_device,unsigned short int reg_addr,unsigned char rdata_len);
void at24c_read_byaddr(at24c_device_st *const at24c_device,unsigned short int reg_addr,unsigned char *rdata,unsigned short int rdata_len);
void at24c_write_byaddr(at24c_device_st *const at24c_device,unsigned char waddr,unsigned char *wdata,unsigned short int wdata_len);
unsigned char at24c_hascfg(at24c_device_st *const at24c_device,unsigned char cfg_addr);
unsigned char at24cx_check(at24c_device_st *const at24c_device);
void at24c_default(at24c_device_st *const at24c_device);
unsigned char at24cx_databuff_get(at24c_device_st *const at24c_device);


#endif

















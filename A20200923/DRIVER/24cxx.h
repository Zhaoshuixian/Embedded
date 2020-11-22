#ifndef __24CXX_H
#define __24CXX_H

#include "myiic.h"
#include "pindef.h"
#include "delay.h"

//24CXX驱动函数(适合24C01~24C16,24C32~256未经过测试!有待验证!)

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
}at24c_device_st;//多设备

//MSB开始传输
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

//------NV_DATA_ADDR TAB(非易失性数据存储地址分配)-----------------
#define NV_EE_START_ADDR        (0x00) /*起始地址*/
#define NV_EE_END_ADDR          (0xFF) /*结束地址*/
/*RS485数据点*/
#define NV_RS485_ADDR_ADDR      (0x09)/*RS485地址信息*/
#define NV_RS485_BAUD_ADDR      (0x0A)/*RS485波特率信息*/
/*水箱数据点*/
#define NV_TANK_TEMPER_TH_ADDR    (0x0B)/*水箱下限温度阈值*/
#define NV_TANK_UPWATLEV_TH_ADDR  (0x0C)/*水箱上限水位阈值*/
#define NV_TANK_DWWATLEV_TH_ADDR  (0x0D)/*水箱下限水位阈值*/
#define NV_TANK_FULLSCALE_HB_ADDR (0x0E)/*水箱水位满量程高字节*/
#define NV_TANK_FULLSCALE_LB_ADDR (0x0F)/*水箱水位满量程低字节*/
/*热泵数据点*/
#define NV_HOTPUMP_TEMPER_TH_ADDR    (0x10)/*热泵温度阈值*/
#define NV_HOTPUMP_WATPRESS_TH_ADDR  (0x11)/*热泵水压阈值信息*/
#define NV_HOTPUMP_WORKCURR_TH_ADDR  (0x12)/*热泵启停阈值电流*/
/*线控器控制时间段数据点*/
#define NV_WCB_MON_HOUR_ON_ADDR      (0x17)/*线控器周一开启小时*/
#define NV_WCB_MON_MIN_ON_ADDR       (0x18)/*线控器周一开启分钟*/

#define NV_WCB_MON_HOUR_OFF_ADDR     (0x19)/*线控器周一关闭小时*/
#define NV_WCB_MON_MIN_OFF_ADDR      (0x1A)/*线控器周一关闭分钟*/

#define NV_WCB_TUS_HOUR_ON_ADDR      (0x1B)/*线控器周二开启小时*/
#define NV_WCB_TUS_MIN_ON_ADDR       (0x1C)/*线控器周二开启分钟*/

#define NV_WCB_TUS_HOUR_OFF_ADDR     (0x1D)/*线控器周二关闭小时*/
#define NV_WCB_TUS_MIN_OFF_ADDR      (0x1E)/*线控器周二关闭分钟*/

#define NV_WCB_WED_HOUR_ON_ADDR      (0x1F)/*线控器周三开启小时*/
#define NV_WCB_WED_MIN_ON_ADDR       (0x20)/*线控器周三开启分钟*/

#define NV_WCB_WED_HOUR_OFF_ADDR     (0x21)/*线控器周三关闭小时*/
#define NV_WCB_WED_MIN_OFF_ADDR      (0x22)/*线控器周三关闭分钟*/

#define NV_WCB_THR_HOUR_ON_ADDR      (0x23)/*线控器周四开启小时*/
#define NV_WCB_THR_MIN_ON_ADDR       (0x24)/*线控器周四开启分钟*/

#define NV_WCB_THR_HOUR_OFF_ADDR     (0x25)/*线控器周四关闭小时*/
#define NV_WCB_THR_MIN_OFF_ADDR      (0x26)/*线控器周四关闭分钟*/

#define NV_WCB_FRI_HOUR_ON_ADDR      (0x27)/*线控器周五开启小时*/
#define NV_WCB_FRI_MIN_ON_ADDR       (0x28)/*线控器周五开启分钟*/

#define NV_WCB_FRI_HOUR_OFF_ADDR     (0x29)/*线控器周五关闭小时*/
#define NV_WCB_FRI_MIN_OFF_ADDR      (0x2A)/*线控器周五关闭分钟*/

#define NV_WCB_SAT_HOUR_ON_ADDR      (0x2B)/*线控器周六开启小时*/
#define NV_WCB_SAT_MIN_ON_ADDR       (0x2C)/*线控器周六开启分钟*/

#define NV_WCB_STA_HOUR_OFF_ADDR     (0x2D)/*线控器周六关闭小时*/
#define NV_WCB_STA_MIN_OFF_ADDR      (0x2E)/*线控器周六关闭分钟*/

#define NV_WCB_SUN_HOUR_ON_ADDR      (0x2F)/*线控器周日开启小时*/
#define NV_WCB_SUN_MIN_ON_ADDR       (0x30)/*线控器周日开启分钟*/

#define NV_WCB_SUN_HOUR_OFF_ADDR     (0x31)/*线控器周日关闭小时*/
#define NV_WCB_SUN_MIN_OFF_ADDR      (0x32)/*线控器周日关闭分钟*/

#define NV_METER_CURR_BIAS_CALIB_ADDR (0x33)/*计量芯片电流偏置校准*/
#define NV_METER_CURR_BIAS_CALIB_LEN  (3) 
#define NV_METER_VOLT_BIAS_CALIB_ADDR (0x36)/*计量芯片电压偏置校准*/
#define NV_METER_VOLT_BIAS_CALIB_LEN  (3) 
#define NV_METER_CURR_GAIN_CALIB_ADDR (0x39)/*计量芯片电流增益校准*/
#define NV_METER_CURR_GAIN_CALIB_LEN  (3) 
#define NV_METER_VOLT_GAIN_CALIB_ADDR (0x3c)/*计量芯片电压增益校准*/
#define NV_METER_VOLT_GAIN_CALIB_LEN  (3) 
#define NV_METER_PWR_BIAS_CALIB_ADDR  (0x3F)/*计量芯片功率偏置校准*/
#define NV_METER_PWR_BIAS_CALIB_LEN   (3)
#define NV_METER_DATA_ERROR_ADDR      (0x42)/*计量芯片错误数据*/
#define NV_METER_DATA_ERROR_LEN       (3)
#define NV_METER_COMPENSATION_ADDR    (0x45)/*计量芯片相位补藏*/
#define NV_METER_COMPENSATION_LEN     (1)

#define TOTAL_CFG_LEN                (NV_WCB_SUN_MIN_OFF_ADDR-NV_RS485_ADDR_ADDR+1)/*计算连续区字节数量*/

#define NV_EEPROM_HAS_CFG_ADDR       (NV_EE_END_ADDR)/*EEPROM已配置数据(非首次使用)*/
//-----------------------------------END--------------------------

//for 24cxx
#define AT24C_SCL(x)      (x==0?GPIO_ResetBits(AT24CXX_PORT,AT24CXX_SCL_PIN):GPIO_SetBits(AT24CXX_PORT,AT24CXX_SCL_PIN)) //SCL
#define AT24C_SDA(x)      (x==0?GPIO_ResetBits(AT24CXX_PORT,AT24CXX_SDA_PIN):GPIO_SetBits(AT24CXX_PORT,AT24CXX_SDA_PIN))  //SDA	 

#define AT24C_READ_SDA()  GPIO_ReadInputDataBit(AT24CXX_PORT,AT24CXX_SDA_PIN)
#define AT24C_SDA_IN()    {AT24CXX_PORT->CTRLH&=0xFFFFFF0F;AT24CXX_PORT->CTRLH|=((unsigned int)8<<1*4);}//PC9-SDA//PC8-SCL
#define AT24C_SDA_OUT()   {AT24CXX_PORT->CTRLH&=0xFFFFFF0F;AT24CXX_PORT->CTRLH|=(unsigned int)(3<<1*4);}


//-------------实现接口--------------
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

















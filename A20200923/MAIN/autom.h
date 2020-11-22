#ifndef __AUTOCONTROL_H__
#define __AUTOCONTROL_H__

#include "pcf8563.h"

#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------非易失性存储接口----------------------
typedef struct
{
	void (*wr_data)(unsigned short int ,unsigned char *,unsigned char );
	void (*rd_data)(unsigned short int ,unsigned char *,unsigned char );
}nv_st;


typedef struct
{
	unsigned short int cur_scale;/*当前量程*/			
	unsigned short int full_scale;/*满量程*/
	unsigned short int percent95_scale;/*0.75量程*/			
	unsigned short int percent85_scale;/*0.75量程*/		
	unsigned short int percent75_scale;/*0.75量程*/	
	unsigned short int percent65_scale;/*0.75量程*/	
	unsigned short int percent55_scale;/*0.50量程*/	
	unsigned short int percent45_scale;/*0.50量程*/
	unsigned short int percent35_scale;/*0.50量程*/	
	unsigned short int percent25_scale;/*0.25量程*/
	unsigned short int percent15_scale;	
	unsigned short int percent5_scale;/*量程*/		
}rang_st;

typedef struct
{
	unsigned char  online;  //传感器在线	
	unsigned char  cur_val;  //当前值值
//	unsigned char  up_th_val;//上限阈值
	unsigned char  dw_th_val;//下限阈值	
//	unsigned char  data_err;//数据错误		
}sensor_st;//传感器

typedef struct
{
	unsigned char  workcurrent_th_val;//启停电流阈值
  unsigned char   run_status;        //启停状态
	rang_st    rang; //量程
	sensor_st  sensor_temper; //温度传感器
  sensor_st  sensor_watpress;//水压传感器	
}hotpump_st;//热泵

typedef struct
{
	rang_st rang;
	unsigned char online;   //传感器在线
	unsigned char cur_val;  //水位值（量化值）	
	unsigned char up_th_val;//上限阈值
	unsigned char dw_th_val;//上限阈值		
	unsigned char calib_cmd;//校准命令
  unsigned char data_err;//数据错误	
}watlevel_st;//水位传感器

typedef struct
{
	sensor_st   sensor_temper;//温度传感器
	watlevel_st sensor_watlevel ;//水位传感器
}watertank_st;//水箱

typedef struct
{
	unsigned char hour;
	unsigned char min;
}ctime_st;//控制时间

typedef struct
{
	ctime_st open_time,close_time;//时间段
}period_st;

typedef struct
{
  period_st  period[7];	
	void (*power)(unsigned char);
}wcb_st;

typedef struct
{
  unsigned char device_addr;	/*本机地址*/
	unsigned char device_baud;	/*本机波特率*/
}rs485_st;

typedef struct
{
  unsigned char seg_show[8];
  unsigned char led_show[2];
}dis_st;



typedef struct 
{
//bit0:水箱温度传感器挂载报警
//bit1:热泵温度传感器挂载报警	
//bit2:水位传感器挂载报警
//bit3:水压传感器挂载报警			
//bit4:水箱水温低于阈值
//bit5:热泵水温低于阈值
//bit6:水箱水位低于下限阈值
//bit7:水箱水位高于上限阈值
//bit8:水箱水位上下限值设反，错误
//bit9:热泵水压低于阈值
//bit10:电流互感器挂载
  unsigned char  updata_flag;    //升级标志
	unsigned char  updata_status;  //升级状态	
	unsigned short int  warning_status;//报警信息
	unsigned short int  updata_timer; //升级计时器	
  wcb_st wcb;        /*线控器*/
	hotpump_st   hotpump;/*热泵*/
	watertank_st watertank;/*水箱*/
	rs485_st     rs485;
  rtc_st       sys_rtc;
	dis_st       dis;
	void (*warn_active)(unsigned char);//报警	
	unsigned char (*rtc_set)(rtc_st *const);//RTC设置	
}automtype;//自控

typedef enum
{
	SM0=0,
	SM1,	
	SM2,	
//	SM3,	
	//SM4,		
}status_em;

typedef struct
{
	status_em status;
	unsigned int rtime;
}fsm_st;

extern automtype g_ulAutoControl;
void autom_thread_entry(void *parameter);

#ifdef __cplusplus
}
#endif

#endif
//

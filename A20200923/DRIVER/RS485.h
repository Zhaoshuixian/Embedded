#ifndef	RS485_H
#define	RS485_H

#include "at32f4xx.h"
#include "stdint.h"


/*
在大家通读Modbus协议时，总是会碰到一些容易混淆的名词，从而导致对于协议的解读出现问题，
在本文中，我们对于Modbus协议中可能会给大家造成困惑的名词作相关的解释。

1.功能码：功能码在modbus协议用于表示信息帧的功能，
常用的功能码有03，04，06，16等，
其中03功能码的作用是读保持寄存器内容，
04功能码的作用是读输入寄存器内容(输入寄存器和保持寄存器的区别看下文），
06功能码的内容是预置单个保持寄存器，
16功能码的内容则是预置多个保持寄存器。

2.输入寄存器和保持寄存器：
04功能码的作用就是读输入寄存器，而03功能码的作用则是读保持寄存器，
很多人在看到这两个功能码的时候总是希望找到这两个功能码的区别，
保持寄存器和输入寄存器到底是什么区别，
modbus协议最开始是用来解决PLC的通信协议问题的，
主要用于输入输出数字量信号以及模拟量信号，
所谓的输入寄存器就是从模拟量信号输入引申出来的，
即输入寄存器只能从模拟量信号输入端改变寄存器，
而主机则不能通过下发指令改变输入寄存器的数据，
而保持寄存器则是用于输出模拟量信号的，
主机是可以改变寄存器数据，也就是说对于主机而言，
输入寄存器是只读的，而保持寄存器是可以读写的，
当主机用06，16功能码的指令去预置输入寄存器的时候，
设备会返回一个代码为0x81的错误代码，即企图写只读寄存器。

3.Modbus中的数据地址格式：
在Modbus协议中，经常会出现类似于3xxxx,4xxxx寄存器，
这个表示的是寄存器支持的数据类型。
我们用列表来说明，还有Modbus数据地址格式是从0开始，
比如以下一个寄存器40009，即表示保持寄存器，寄存器地址为00 08，
类似的数据地址格式经常在组态软件以及PLC系统中用到。

映射地址	使用的功能码	地址类型	  存取方式	           描述
0XXXX	        01,05,15	离散输出	    读写	      每个卷表示单个开关位
1XXXX	              02	离散输入	    只读	      每个卷表示单个开关位
2XXXX	     03,04,06,16	浮点寄存器	  只读/读写	  两个连续16位寄存器表示一个浮点数
3XXXX	              04	输入寄存器	  只读	      每个寄存器表示一个16位无符号整数
4XXXX	        03,06,16	保持寄存器	  读写	      每个寄存器表示一个16位无符号整数
5XXXX	     03,04,06,16	ASCII字符	    读写	      每个寄存器表示两个ASCII码
4.Modbus RTU/ASCII/TCP:Modbus协议最开始是用于可编程逻辑控制器（PLC）之间的通讯，
由于其具有的开放性，大量的用于现场智能仪表。Modbus协议有多个变种，
其中最著名的是Modbus RTU/Modbus ASCII和Modbus TCP通信协议。
其中RTU/ASCII协议是基于串行口通信，而TCP协议则是基于以太网通信。
他们之间的区别和联系请参考本站其他页面内容：
Modbus RTU/TCP协议解析以及Modbus RTU/ASCII通信协议异同点。

5.Modbus错误代码表,modbus有功能码，校验码，
异常功能码和错误代码，其中异常功能码和错误代码非常容易混淆，
一般来说异常功能码指的是某个功能码执行的时候出现的相应异常功能码，
一般都是在功能码的基础上加上0x80，比如03功能码出现的异常码是0x83异常功能码，
16功能码对应出现的异常功能码则是0x90,而错误代码则是表示出现错误的具体情况，
比如寄存器地址不存在，不管是读还是写，如果该寄存器地址不存在的话，错误代码为02。
具体情况请参阅本站其他页面文章。

*/


#define BORADCAST_ADDR (0x00) /*广播地址*/

typedef struct
{
	void (*enport)(unsigned char);
	void (*transmit)(unsigned char *,unsigned short int ,void (*mb_enport)(unsigned char));	
	unsigned short int (*check)(unsigned char *,unsigned short int);
	union
	{
		#define DATA_MAX_SIZE (255)
		unsigned char buff[DATA_MAX_SIZE];
		struct
		{
			unsigned char ulAddr;	//设备地址
			unsigned char ulFunc;//功能码
			unsigned char ulRegH;//寄存器起始地址H
			unsigned char ulRegL;//寄存器起始地址L
			unsigned char ulDataH;
			unsigned char ulDataL;
			unsigned char ulCrcL;
			unsigned char ulCrcH;
			unsigned char ulData[DATA_MAX_SIZE-8];		
		}frame;		
	}data;
}modbus_st;//协议数据帧格式

typedef struct
{
	void (*enport)(unsigned char);
	void (*transmit)(unsigned char *,unsigned short int ,void (*mb_enport)(unsigned char));	
	unsigned short int (*check)(unsigned char *,unsigned short int );
	union
	{	
		#define DATA_MAX_SIZE (255)
		unsigned char buff[DATA_MAX_SIZE];
		struct
		{
			unsigned char ulHeader;//数据协议头		
			unsigned char ulAddr;	//设备地址
			unsigned char ulFunc;//功能码
			unsigned char ulRegH;//寄存器起始地址H
			unsigned char ulRegL;//寄存器起始地址L
			unsigned char ulData[DATA_MAX_SIZE-5-1];
			unsigned char ulEnd;	
		}frame;
	}data;
}ppotocol_st;//私有协议数据帧格式

/*打算进一步规范MODBUS协议操作*/
#ifdef UPDATA 

#define HOLD_REG_NUM (100) /*保持寄存器数目*/
#define INPUT_REG_NUM (50) /*输入寄存器数目*/

extern unsigned short int HOLD_REG_BUFF[HOLD_REG_NUM];
extern unsigned short int INPUT_REG_BUFF[INPUT_REG_NUM];//
#endif
//--------------------REGISTER TAB(数据寄存器分配表)----------------------
//--1~50--只读区
#define OR_REG_SOFT_VER           	(0x0001)/*软件版本信息 --只读*/
#define OR_REG_WARN_INFO          	(0x0002)/*报警信息 --只读*/

#define OR_REG_TANK_TEMPER        	(0x0003)/*水箱温度信息--只读*/
#define OR_REG_TANK_WATLEV         	(0x0004)/*水箱水位信息--只读*/
#define OR_REG_TANK_FULLSCALE      	(0x0005)/*水箱水位满量程--只读*/

#define OR_REG_HOTPUMP_WORKSTA    	(0x0006)/*热泵启停状态--只读*/
#define OR_REG_HOTPUMP_TEMPER     	(0x0007)/*热泵温度信息--只读*/
#define OR_REG_HOTPUMP_WATPRESS     (0x0008)/*热泵水位信息--只读*/

#define OR_METER_RMS_CURR_ADDR    	(0x0009)/*计量芯片读得的有效电流--只读*/
#define OR_METER_RMS_VOLT_ADDR    	(0x000A)/*计量芯片读得的有效电压--只读*/
#define OR_METER_RMS_PWR_ADDR     	(0x000B)/*计量芯片读得的有功功率--只读*/

//--300~499--读写区
#define RW_REG_SYSRTC_YEAR          (0x0130)/*系统RTC年份 --读写*/
#define RW_REG_SYSRTC_MONTH         (0x0131)/*系统RTC月份 --读写*/
#define RW_REG_SYSRTC_DAY           (0x0132)/*系统RTC日份 --读写*/
#define RW_REG_SYSRTC_HOUR          (0x0133)/*系统RTC时份 --读写*/
#define RW_REG_SYSRTC_MIN           (0x0134)/*系统RTC分份 --读写*/
#define RW_REG_SYSRTC_SEC           (0x0135)/*系统RTC秒份 --读写*/

#define RW_REG_TANK_UPWATLEV_TH     (0x0136)/*水箱上限水位阈值--读写*/
#define RW_REG_TANK_DWWATLEV_TH     (0x0137)/*水箱下限水位阈值--读写*/
#define RW_REG_TANK_WATLEV_CALIB  	(0x0138)/*水箱水位校准--读写*/
#define RW_REG_TANK_TEMPER_TH       (0x0139)/*水箱水温下限阈值--读写*/

#define RW_REG_HOTPUMP_WATPRESS_TH  (0x013A)/*热泵水压阈值信息--读写*/
#define RW_REG_HOTPUMP_WORKCURR_TH  (0x013B)/*热泵启停阈值电流--读写*/
#define RW_REG_HOTPUMP_TEMPER_TH    (0x013C)/*热泵温度阈值--读写*/

#define RW_REG_RS485_ADDR      		  (0x013D)/*RS485地址信息--读写*/
#define RW_REG_RS485_BAUD      		  (0x013E)/*RS485波特率信息--读写*/

#define RW_WCB_MON_HOUR_ON_ADDR     (0x013F)/*线控器周一开启小时*/
#define RW_WCB_MON_MIN_ON_ADDR      (0x0140)/*线控器周一开启分钟*/

#define RW_WCB_MON_HOUR_OFF_ADDR    (0x0141)/*线控器周一关闭小时*/
#define RW_WCB_MON_MIN_OFF_ADDR     (0x0142)/*线控器周一关闭分钟*/

#define RW_WCB_TUS_HOUR_ON_ADDR     (0x0143)/*线控器周二开启小时*/
#define RW_WCB_TUS_MIN_ON_ADDR      (0x0144)/*线控器周二开启分钟*/

#define RW_WCB_TUS_HOUR_OFF_ADDR    (0x0145)/*线控器周一关闭小时*/
#define RW_WCB_TUS_MIN_OFF_ADDR     (0x0146)/*线控器周一关闭分钟*/

#define RW_WCB_WED_HOUR_ON_ADDR     (0x0147)/*线控器周三开启小时*/
#define RW_WCB_WED_MIN_ON_ADDR      (0x0148)/*线控器周三开启分钟*/

#define RW_WCB_WED_HOUR_OFF_ADDR    (0x0149)/*线控器周三关闭小时*/
#define RW_WCB_WED_MIN_OFF_ADDR     (0x014A)/*线控器周三关闭分钟*/

#define RW_WCB_THR_HOUR_ON_ADDR     (0x014B)/*线控器周四开启小时*/
#define RW_WCB_THR_MIN_ON_ADDR      (0x014C)/*线控器周四开启分钟*/

#define RW_WCB_THR_HOUR_OFF_ADDR    (0x014D)/*线控器周四关闭小时*/
#define RW_WCB_THR_MIN_OFF_ADDR     (0x014E)/*线控器周四关闭分钟*/

#define RW_WCB_FRI_HOUR_ON_ADDR     (0x014F)/*线控器周五开启小时*/
#define RW_WCB_FRI_MIN_ON_ADDR      (0x0150)/*线控器周五开启分钟*/

#define RW_WCB_FRI_HOUR_OFF_ADDR    (0x0151)/*线控器周五关闭小时*/
#define RW_WCB_FRI_MIN_OFF_ADDR     (0x0152)/*线控器周五关闭分钟*/

#define RW_WCB_SAT_HOUR_ON_ADDR     (0x0153)/*线控器周六开启小时*/
#define RW_WCB_SAT_MIN_ON_ADDR      (0x0154)/*线控器周六开启分钟*/

#define RW_WCB_STA_HOUR_OFF_ADDR    (0x0155)/*线控器周六关闭小时*/
#define RW_WCB_STA_MIN_OFF_ADDR     (0x0156)/*线控器周六关闭分钟*/

#define RW_WCB_SUN_HOUR_ON_ADDR     (0x0157)/*线控器周日开启小时*/
#define RW_WCB_SUN_MIN_ON_ADDR      (0x0158)/*线控器周日开启分钟*/

#define RW_WCB_SUN_HOUR_OFF_ADDR    (0x0159)/*线控器周日关闭小时*/
#define RW_WCB_SUN_MIN_OFF_ADDR     (0x015A)/*线控器周日关闭分钟*/


#define RW_METER_CALIB_ADDR         (0x2055)/*计量校准*/
//-----------------------END----------------------------
extern uint8_t USART_EventFlag;

#endif


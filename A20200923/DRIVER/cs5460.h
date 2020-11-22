#ifndef __CS5460_H__
#define __CS5460_H__

#include "pindef.h"
#include "myspi.h"


//IO操作函数	 
//#define RESET_5460 	PBout(12)
#define EN_5460	    (CS5460A_CSN(0),W25Q_CSN(1))//使能片选CS5460
#define UN_5460	    CS5460A_CSN(1)   //取消s片选CS5460

/* CS5460A命令字 */
#define CS5460A_M_START  0xe8       //以连续计算周期方式启动转换
#define CS5460A_S_START  0xe0       //以  单计算周期方式启动转换
#define SYNC0            0xfe       //同步控制命令字的结尾
#define SYNC1            0xff       //同步控制命令字的开始，连续写入3个或更多的SYNC1，然后再写入SYNC0
#define PAUSE            0xa0       //上电或暂停
#define STAND_BY         0x88       //进入等待模式
#define SLEEP            0x90       //进入睡眠模式
#define CALIB            0x30       //校准控制命令，需要与下述命令共同使用
#define CALIB_I          0x08       //只对电流通道校准
#define CALIB_V          0x10       //只对电压通道校准
#define CALIB_IV         0x18       //对电流、电压通道都校准
#define CALIB_AC         0x04       //AC校准
#define CALIB_DC         0x00       //DC校准
#define CALIB_GAIN       0x02       //增益校准
#define CALIB_OFF        0x01       //偏移量校准
/*CS5460内部寄存器的低六位*/
#define CS5460A_CONFIG   0x00  //配置寄存器
#define I_DC_OFF    0x02       //电流DC偏移量寄存器
#define I_GAIN      0x04       //电流AC增益寄存器
#define V_DC_OFF    0x06       //电压DC偏移量寄存器
#define V_GAIN      0x08       //电压AC增益寄存器
#define CYCLE_COUNT       0x0A  //每个计算周期的A/D转换数 
#define PULSE_RATE   0x0c       //用于设置 EOUT 和 EDIR 上的能量-脉冲速率
#define LAST_CURRENT      0x0E  //瞬时电流寄存器（最近一次电流采样）
#define LAST_VOLT         0x10  //瞬时电压寄存器（最近一次电压采样）
#define LAST_POWER        0x12  //瞬时功率寄存器（最近一次功率采样）
#define LAST_ENERGY       0x14  //电能寄存器（最后一次计算周期的累计值）
#define LAST_CURRENT_RMS  0x16  //电流有效值寄存器（最后一次计算周期的值）
#define LAST_VOLT_RMS     0x18  //电压有效值寄存器（最后一次计算周期的值）
#define TBC          0x1a       //时基校准寄存器
#define P_OFF        0x1c       //功率偏移量寄存器
#define STATUS            0x1E  //状态寄存器
#define I_AC_OFF     0x20       //电流AC偏移量寄存器
#define V_AC_OFF     0x22       //电压AC偏移量寄存器
#define MASK              0x34  //中断屏蔽寄存器
#define CTRL         0x38       //控制寄存器
#define VOLT_DC_OFFSET    0X06  //电压通道 直流 偏置寄存器
#define CURRENT_DC_OFFSET 0X02  //电流通道 直流 偏置寄存器
#define VOLT_AC_OFFSET    0X22  //电压通道 交流 偏置寄存器
#define CURRENT_AC_OFFSET 0X20  //电流通道 交流 偏置寄存器
/*直流偏置(已校准)*/
#define CURRENT_DC      0xCC4ADD //电压偏置   
#define VOLT_DC         0X331396 //电流偏置
/*寄存器的读写*/
#define READ  0x00    //读
#define WRITE 0x40    //写
/*各寄存器复位后的初始值*/
#define CONFIG_INIT      0x000001   //配置寄存器
#define STATUS_INIT      0x000001   //状态寄存器
#define MASK_INIT        0x000000   //中断屏蔽寄存器
#define CYCLE_COUNT_INIT 0x000FA0   //周期计数寄存器
/*定义以上寄存器的初始化后的值*/
#define  CONFIG_DATA      0x0010E1  //软件复位
#define  CALCULATE_DOWN   0x800000  //数据已经就绪，转换结束
#define  LOW_POWER        0x000004  //检测到低电源
#define  WDT              0X000020  //能量超过5秒未被读取，该位置位 
/*定义其他命令*/
#define  POWER_OFF_CTRL  0X88  //掉电控制
#define  STANDERED_CTRL  0XC0  //标准控制
#define  TRANSFORM_START 0Xe8 //开始转换,执行连续计算周期
//软件滤波函数
#define	N	24
#define	CS_VRMS  0
#define  CS_IRMS  1
#define  CS_PRMS  2

#ifdef __cplusplus
extern "C"{
#endif

extern u8  g_ulCS5460UpdataFlag;//BIT0 =1:CS5460存在 0=不存在//BIT1=1:有功功率负号 0=正号


typedef struct
{
	vu32	volt_rms; // 全局变量
	vu32  amp_rms;
	vu32  actpwr_rms;
	vu32  energy_w;//实时功率W	
	
}meter_st;

extern meter_st cs5460a;

void cs5460_meter_read(meter_st *const cs5460a_me);//CS5460事件查找
void cs5460a_check(void);//CS5460自检
void CS5460IRQHandler(void);//错误态寄存器查找




double cs5460a_fliter_get(u8 CS);//CS5460软件滤波
double cs5460a_activepwr_get(void);//CS5460读电能累计值
double cs5460a_rmsvolt_get(void);//CS5460读电压
double cs5460a_rmsamp_get(void);//CS5460读电流

void cs5460a_wait_sr(void) ;//检查DRDY位是否置位
void cs5460a_clear_sr(void);//CS5460清状态寄存器
void cs5460a_rddata(u8 reg,u8 *ptr);//CS5460读多个字节
void cs5460a_wrdata(u8 *ptr,u8 n);//CS5460写多个字节

void cs540a_calibmode(u8 mode);//校准CS5460参数  
void cs5406a_finecalib(u8 *finedata); //对校准参数进行细调
void cs5460a_phasecompens(u8 data); //相位补偿
void cs5460a_reboot(void); //CS5460重启
void cs5460a_config_init(void); //CS5460配置初始化

void cs5460a_cs(unsigned char s);
void cs5460a_init(void);
void cs54060a_cs_gpio_init(void);

void meter_thread_entry(void *parameter);


#ifdef __cplusplus
}
#endif

#endif

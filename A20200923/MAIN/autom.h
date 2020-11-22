#ifndef __AUTOCONTROL_H__
#define __AUTOCONTROL_H__

#include "pcf8563.h"

#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------����ʧ�Դ洢�ӿ�----------------------
typedef struct
{
	void (*wr_data)(unsigned short int ,unsigned char *,unsigned char );
	void (*rd_data)(unsigned short int ,unsigned char *,unsigned char );
}nv_st;


typedef struct
{
	unsigned short int cur_scale;/*��ǰ����*/			
	unsigned short int full_scale;/*������*/
	unsigned short int percent95_scale;/*0.75����*/			
	unsigned short int percent85_scale;/*0.75����*/		
	unsigned short int percent75_scale;/*0.75����*/	
	unsigned short int percent65_scale;/*0.75����*/	
	unsigned short int percent55_scale;/*0.50����*/	
	unsigned short int percent45_scale;/*0.50����*/
	unsigned short int percent35_scale;/*0.50����*/	
	unsigned short int percent25_scale;/*0.25����*/
	unsigned short int percent15_scale;	
	unsigned short int percent5_scale;/*����*/		
}rang_st;

typedef struct
{
	unsigned char  online;  //����������	
	unsigned char  cur_val;  //��ǰֵֵ
//	unsigned char  up_th_val;//������ֵ
	unsigned char  dw_th_val;//������ֵ	
//	unsigned char  data_err;//���ݴ���		
}sensor_st;//������

typedef struct
{
	unsigned char  workcurrent_th_val;//��ͣ������ֵ
  unsigned char   run_status;        //��ͣ״̬
	rang_st    rang; //����
	sensor_st  sensor_temper; //�¶ȴ�����
  sensor_st  sensor_watpress;//ˮѹ������	
}hotpump_st;//�ȱ�

typedef struct
{
	rang_st rang;
	unsigned char online;   //����������
	unsigned char cur_val;  //ˮλֵ������ֵ��	
	unsigned char up_th_val;//������ֵ
	unsigned char dw_th_val;//������ֵ		
	unsigned char calib_cmd;//У׼����
  unsigned char data_err;//���ݴ���	
}watlevel_st;//ˮλ������

typedef struct
{
	sensor_st   sensor_temper;//�¶ȴ�����
	watlevel_st sensor_watlevel ;//ˮλ������
}watertank_st;//ˮ��

typedef struct
{
	unsigned char hour;
	unsigned char min;
}ctime_st;//����ʱ��

typedef struct
{
	ctime_st open_time,close_time;//ʱ���
}period_st;

typedef struct
{
  period_st  period[7];	
	void (*power)(unsigned char);
}wcb_st;

typedef struct
{
  unsigned char device_addr;	/*������ַ*/
	unsigned char device_baud;	/*����������*/
}rs485_st;

typedef struct
{
  unsigned char seg_show[8];
  unsigned char led_show[2];
}dis_st;



typedef struct 
{
//bit0:ˮ���¶ȴ��������ر���
//bit1:�ȱ��¶ȴ��������ر���	
//bit2:ˮλ���������ر���
//bit3:ˮѹ���������ر���			
//bit4:ˮ��ˮ�µ�����ֵ
//bit5:�ȱ�ˮ�µ�����ֵ
//bit6:ˮ��ˮλ����������ֵ
//bit7:ˮ��ˮλ����������ֵ
//bit8:ˮ��ˮλ������ֵ�跴������
//bit9:�ȱ�ˮѹ������ֵ
//bit10:��������������
  unsigned char  updata_flag;    //������־
	unsigned char  updata_status;  //����״̬	
	unsigned short int  warning_status;//������Ϣ
	unsigned short int  updata_timer; //������ʱ��	
  wcb_st wcb;        /*�߿���*/
	hotpump_st   hotpump;/*�ȱ�*/
	watertank_st watertank;/*ˮ��*/
	rs485_st     rs485;
  rtc_st       sys_rtc;
	dis_st       dis;
	void (*warn_active)(unsigned char);//����	
	unsigned char (*rtc_set)(rtc_st *const);//RTC����	
}automtype;//�Կ�

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

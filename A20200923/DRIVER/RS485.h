#ifndef	RS485_H
#define	RS485_H

#include "at32f4xx.h"
#include "stdint.h"


/*
�ڴ��ͨ��ModbusЭ��ʱ�����ǻ�����һЩ���׻��������ʣ��Ӷ����¶���Э��Ľ���������⣬
�ڱ����У����Ƕ���ModbusЭ���п��ܻ���������������������صĽ��͡�

1.�����룺��������modbusЭ�����ڱ�ʾ��Ϣ֡�Ĺ��ܣ�
���õĹ�������03��04��06��16�ȣ�
����03������������Ƕ����ּĴ������ݣ�
04������������Ƕ�����Ĵ�������(����Ĵ����ͱ��ּĴ������������ģ���
06�������������Ԥ�õ������ּĴ�����
16���������������Ԥ�ö�����ּĴ�����

2.����Ĵ����ͱ��ּĴ�����
04����������þ��Ƕ�����Ĵ�������03��������������Ƕ����ּĴ�����
�ܶ����ڿ����������������ʱ������ϣ���ҵ������������������
���ּĴ���������Ĵ���������ʲô����
modbusЭ���ʼ���������PLC��ͨ��Э������ģ�
��Ҫ������������������ź��Լ�ģ�����źţ�
��ν������Ĵ������Ǵ�ģ�����ź�������������ģ�
������Ĵ���ֻ�ܴ�ģ�����ź�����˸ı�Ĵ�����
����������ͨ���·�ָ��ı�����Ĵ��������ݣ�
�����ּĴ��������������ģ�����źŵģ�
�����ǿ��Ըı�Ĵ������ݣ�Ҳ����˵�����������ԣ�
����Ĵ�����ֻ���ģ������ּĴ����ǿ��Զ�д�ģ�
��������06��16�������ָ��ȥԤ������Ĵ�����ʱ��
�豸�᷵��һ������Ϊ0x81�Ĵ�����룬����ͼдֻ���Ĵ�����

3.Modbus�е����ݵ�ַ��ʽ��
��ModbusЭ���У����������������3xxxx,4xxxx�Ĵ�����
�����ʾ���ǼĴ���֧�ֵ��������͡�
�������б���˵��������Modbus���ݵ�ַ��ʽ�Ǵ�0��ʼ��
��������һ���Ĵ���40009������ʾ���ּĴ������Ĵ�����ַΪ00 08��
���Ƶ����ݵ�ַ��ʽ��������̬����Լ�PLCϵͳ���õ���

ӳ���ַ	ʹ�õĹ�����	��ַ����	  ��ȡ��ʽ	           ����
0XXXX	        01,05,15	��ɢ���	    ��д	      ÿ�����ʾ��������λ
1XXXX	              02	��ɢ����	    ֻ��	      ÿ�����ʾ��������λ
2XXXX	     03,04,06,16	����Ĵ���	  ֻ��/��д	  ��������16λ�Ĵ�����ʾһ��������
3XXXX	              04	����Ĵ���	  ֻ��	      ÿ���Ĵ�����ʾһ��16λ�޷�������
4XXXX	        03,06,16	���ּĴ���	  ��д	      ÿ���Ĵ�����ʾһ��16λ�޷�������
5XXXX	     03,04,06,16	ASCII�ַ�	    ��д	      ÿ���Ĵ�����ʾ����ASCII��
4.Modbus RTU/ASCII/TCP:ModbusЭ���ʼ�����ڿɱ���߼���������PLC��֮���ͨѶ��
��������еĿ����ԣ������������ֳ������Ǳ�ModbusЭ���ж�����֣�
��������������Modbus RTU/Modbus ASCII��Modbus TCPͨ��Э�顣
����RTU/ASCIIЭ���ǻ��ڴ��п�ͨ�ţ���TCPЭ�����ǻ�����̫��ͨ�š�
����֮����������ϵ��ο���վ����ҳ�����ݣ�
Modbus RTU/TCPЭ������Լ�Modbus RTU/ASCIIͨ��Э����ͬ�㡣

5.Modbus��������,modbus�й����룬У���룬
�쳣������ʹ�����룬�����쳣������ʹ������ǳ����׻�����
һ����˵�쳣������ָ����ĳ��������ִ�е�ʱ����ֵ���Ӧ�쳣�����룬
һ�㶼���ڹ�����Ļ����ϼ���0x80������03��������ֵ��쳣����0x83�쳣�����룬
16�������Ӧ���ֵ��쳣����������0x90,������������Ǳ�ʾ���ִ���ľ��������
����Ĵ�����ַ�����ڣ������Ƕ�����д������üĴ�����ַ�����ڵĻ����������Ϊ02��
�����������ı�վ����ҳ�����¡�

*/


#define BORADCAST_ADDR (0x00) /*�㲥��ַ*/

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
			unsigned char ulAddr;	//�豸��ַ
			unsigned char ulFunc;//������
			unsigned char ulRegH;//�Ĵ�����ʼ��ַH
			unsigned char ulRegL;//�Ĵ�����ʼ��ַL
			unsigned char ulDataH;
			unsigned char ulDataL;
			unsigned char ulCrcL;
			unsigned char ulCrcH;
			unsigned char ulData[DATA_MAX_SIZE-8];		
		}frame;		
	}data;
}modbus_st;//Э������֡��ʽ

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
			unsigned char ulHeader;//����Э��ͷ		
			unsigned char ulAddr;	//�豸��ַ
			unsigned char ulFunc;//������
			unsigned char ulRegH;//�Ĵ�����ʼ��ַH
			unsigned char ulRegL;//�Ĵ�����ʼ��ַL
			unsigned char ulData[DATA_MAX_SIZE-5-1];
			unsigned char ulEnd;	
		}frame;
	}data;
}ppotocol_st;//˽��Э������֡��ʽ

/*�����һ���淶MODBUSЭ�����*/
#ifdef UPDATA 

#define HOLD_REG_NUM (100) /*���ּĴ�����Ŀ*/
#define INPUT_REG_NUM (50) /*����Ĵ�����Ŀ*/

extern unsigned short int HOLD_REG_BUFF[HOLD_REG_NUM];
extern unsigned short int INPUT_REG_BUFF[INPUT_REG_NUM];//
#endif
//--------------------REGISTER TAB(���ݼĴ��������)----------------------
//--1~50--ֻ����
#define OR_REG_SOFT_VER           	(0x0001)/*����汾��Ϣ --ֻ��*/
#define OR_REG_WARN_INFO          	(0x0002)/*������Ϣ --ֻ��*/

#define OR_REG_TANK_TEMPER        	(0x0003)/*ˮ���¶���Ϣ--ֻ��*/
#define OR_REG_TANK_WATLEV         	(0x0004)/*ˮ��ˮλ��Ϣ--ֻ��*/
#define OR_REG_TANK_FULLSCALE      	(0x0005)/*ˮ��ˮλ������--ֻ��*/

#define OR_REG_HOTPUMP_WORKSTA    	(0x0006)/*�ȱ���ͣ״̬--ֻ��*/
#define OR_REG_HOTPUMP_TEMPER     	(0x0007)/*�ȱ��¶���Ϣ--ֻ��*/
#define OR_REG_HOTPUMP_WATPRESS     (0x0008)/*�ȱ�ˮλ��Ϣ--ֻ��*/

#define OR_METER_RMS_CURR_ADDR    	(0x0009)/*����оƬ���õ���Ч����--ֻ��*/
#define OR_METER_RMS_VOLT_ADDR    	(0x000A)/*����оƬ���õ���Ч��ѹ--ֻ��*/
#define OR_METER_RMS_PWR_ADDR     	(0x000B)/*����оƬ���õ��й�����--ֻ��*/

//--300~499--��д��
#define RW_REG_SYSRTC_YEAR          (0x0130)/*ϵͳRTC��� --��д*/
#define RW_REG_SYSRTC_MONTH         (0x0131)/*ϵͳRTC�·� --��д*/
#define RW_REG_SYSRTC_DAY           (0x0132)/*ϵͳRTC�շ� --��д*/
#define RW_REG_SYSRTC_HOUR          (0x0133)/*ϵͳRTCʱ�� --��д*/
#define RW_REG_SYSRTC_MIN           (0x0134)/*ϵͳRTC�ַ� --��д*/
#define RW_REG_SYSRTC_SEC           (0x0135)/*ϵͳRTC��� --��д*/

#define RW_REG_TANK_UPWATLEV_TH     (0x0136)/*ˮ������ˮλ��ֵ--��д*/
#define RW_REG_TANK_DWWATLEV_TH     (0x0137)/*ˮ������ˮλ��ֵ--��д*/
#define RW_REG_TANK_WATLEV_CALIB  	(0x0138)/*ˮ��ˮλУ׼--��д*/
#define RW_REG_TANK_TEMPER_TH       (0x0139)/*ˮ��ˮ��������ֵ--��д*/

#define RW_REG_HOTPUMP_WATPRESS_TH  (0x013A)/*�ȱ�ˮѹ��ֵ��Ϣ--��д*/
#define RW_REG_HOTPUMP_WORKCURR_TH  (0x013B)/*�ȱ���ͣ��ֵ����--��д*/
#define RW_REG_HOTPUMP_TEMPER_TH    (0x013C)/*�ȱ��¶���ֵ--��д*/

#define RW_REG_RS485_ADDR      		  (0x013D)/*RS485��ַ��Ϣ--��д*/
#define RW_REG_RS485_BAUD      		  (0x013E)/*RS485��������Ϣ--��д*/

#define RW_WCB_MON_HOUR_ON_ADDR     (0x013F)/*�߿�����һ����Сʱ*/
#define RW_WCB_MON_MIN_ON_ADDR      (0x0140)/*�߿�����һ��������*/

#define RW_WCB_MON_HOUR_OFF_ADDR    (0x0141)/*�߿�����һ�ر�Сʱ*/
#define RW_WCB_MON_MIN_OFF_ADDR     (0x0142)/*�߿�����һ�رշ���*/

#define RW_WCB_TUS_HOUR_ON_ADDR     (0x0143)/*�߿����ܶ�����Сʱ*/
#define RW_WCB_TUS_MIN_ON_ADDR      (0x0144)/*�߿����ܶ���������*/

#define RW_WCB_TUS_HOUR_OFF_ADDR    (0x0145)/*�߿�����һ�ر�Сʱ*/
#define RW_WCB_TUS_MIN_OFF_ADDR     (0x0146)/*�߿�����һ�رշ���*/

#define RW_WCB_WED_HOUR_ON_ADDR     (0x0147)/*�߿�����������Сʱ*/
#define RW_WCB_WED_MIN_ON_ADDR      (0x0148)/*�߿���������������*/

#define RW_WCB_WED_HOUR_OFF_ADDR    (0x0149)/*�߿��������ر�Сʱ*/
#define RW_WCB_WED_MIN_OFF_ADDR     (0x014A)/*�߿��������رշ���*/

#define RW_WCB_THR_HOUR_ON_ADDR     (0x014B)/*�߿������Ŀ���Сʱ*/
#define RW_WCB_THR_MIN_ON_ADDR      (0x014C)/*�߿������Ŀ�������*/

#define RW_WCB_THR_HOUR_OFF_ADDR    (0x014D)/*�߿������Ĺر�Сʱ*/
#define RW_WCB_THR_MIN_OFF_ADDR     (0x014E)/*�߿������Ĺرշ���*/

#define RW_WCB_FRI_HOUR_ON_ADDR     (0x014F)/*�߿������忪��Сʱ*/
#define RW_WCB_FRI_MIN_ON_ADDR      (0x0150)/*�߿������忪������*/

#define RW_WCB_FRI_HOUR_OFF_ADDR    (0x0151)/*�߿�������ر�Сʱ*/
#define RW_WCB_FRI_MIN_OFF_ADDR     (0x0152)/*�߿�������رշ���*/

#define RW_WCB_SAT_HOUR_ON_ADDR     (0x0153)/*�߿�����������Сʱ*/
#define RW_WCB_SAT_MIN_ON_ADDR      (0x0154)/*�߿���������������*/

#define RW_WCB_STA_HOUR_OFF_ADDR    (0x0155)/*�߿��������ر�Сʱ*/
#define RW_WCB_STA_MIN_OFF_ADDR     (0x0156)/*�߿��������رշ���*/

#define RW_WCB_SUN_HOUR_ON_ADDR     (0x0157)/*�߿������տ���Сʱ*/
#define RW_WCB_SUN_MIN_ON_ADDR      (0x0158)/*�߿������տ�������*/

#define RW_WCB_SUN_HOUR_OFF_ADDR    (0x0159)/*�߿������չر�Сʱ*/
#define RW_WCB_SUN_MIN_OFF_ADDR     (0x015A)/*�߿������չرշ���*/


#define RW_METER_CALIB_ADDR         (0x2055)/*����У׼*/
//-----------------------END----------------------------
extern uint8_t USART_EventFlag;

#endif


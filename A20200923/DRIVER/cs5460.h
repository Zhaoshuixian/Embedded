#ifndef __CS5460_H__
#define __CS5460_H__

#include "pindef.h"
#include "myspi.h"


//IO��������	 
//#define RESET_5460 	PBout(12)
#define EN_5460	    (CS5460A_CSN(0),W25Q_CSN(1))//ʹ��ƬѡCS5460
#define UN_5460	    CS5460A_CSN(1)   //ȡ��sƬѡCS5460

/* CS5460A������ */
#define CS5460A_M_START  0xe8       //�������������ڷ�ʽ����ת��
#define CS5460A_S_START  0xe0       //��  ���������ڷ�ʽ����ת��
#define SYNC0            0xfe       //ͬ�����������ֵĽ�β
#define SYNC1            0xff       //ͬ�����������ֵĿ�ʼ������д��3��������SYNC1��Ȼ����д��SYNC0
#define PAUSE            0xa0       //�ϵ����ͣ
#define STAND_BY         0x88       //����ȴ�ģʽ
#define SLEEP            0x90       //����˯��ģʽ
#define CALIB            0x30       //У׼���������Ҫ���������ͬʹ��
#define CALIB_I          0x08       //ֻ�Ե���ͨ��У׼
#define CALIB_V          0x10       //ֻ�Ե�ѹͨ��У׼
#define CALIB_IV         0x18       //�Ե�������ѹͨ����У׼
#define CALIB_AC         0x04       //ACУ׼
#define CALIB_DC         0x00       //DCУ׼
#define CALIB_GAIN       0x02       //����У׼
#define CALIB_OFF        0x01       //ƫ����У׼
/*CS5460�ڲ��Ĵ����ĵ���λ*/
#define CS5460A_CONFIG   0x00  //���üĴ���
#define I_DC_OFF    0x02       //����DCƫ�����Ĵ���
#define I_GAIN      0x04       //����AC����Ĵ���
#define V_DC_OFF    0x06       //��ѹDCƫ�����Ĵ���
#define V_GAIN      0x08       //��ѹAC����Ĵ���
#define CYCLE_COUNT       0x0A  //ÿ���������ڵ�A/Dת���� 
#define PULSE_RATE   0x0c       //�������� EOUT �� EDIR �ϵ�����-��������
#define LAST_CURRENT      0x0E  //˲ʱ�����Ĵ��������һ�ε���������
#define LAST_VOLT         0x10  //˲ʱ��ѹ�Ĵ��������һ�ε�ѹ������
#define LAST_POWER        0x12  //˲ʱ���ʼĴ��������һ�ι��ʲ�����
#define LAST_ENERGY       0x14  //���ܼĴ��������һ�μ������ڵ��ۼ�ֵ��
#define LAST_CURRENT_RMS  0x16  //������Чֵ�Ĵ��������һ�μ������ڵ�ֵ��
#define LAST_VOLT_RMS     0x18  //��ѹ��Чֵ�Ĵ��������һ�μ������ڵ�ֵ��
#define TBC          0x1a       //ʱ��У׼�Ĵ���
#define P_OFF        0x1c       //����ƫ�����Ĵ���
#define STATUS            0x1E  //״̬�Ĵ���
#define I_AC_OFF     0x20       //����ACƫ�����Ĵ���
#define V_AC_OFF     0x22       //��ѹACƫ�����Ĵ���
#define MASK              0x34  //�ж����μĴ���
#define CTRL         0x38       //���ƼĴ���
#define VOLT_DC_OFFSET    0X06  //��ѹͨ�� ֱ�� ƫ�üĴ���
#define CURRENT_DC_OFFSET 0X02  //����ͨ�� ֱ�� ƫ�üĴ���
#define VOLT_AC_OFFSET    0X22  //��ѹͨ�� ���� ƫ�üĴ���
#define CURRENT_AC_OFFSET 0X20  //����ͨ�� ���� ƫ�üĴ���
/*ֱ��ƫ��(��У׼)*/
#define CURRENT_DC      0xCC4ADD //��ѹƫ��   
#define VOLT_DC         0X331396 //����ƫ��
/*�Ĵ����Ķ�д*/
#define READ  0x00    //��
#define WRITE 0x40    //д
/*���Ĵ�����λ��ĳ�ʼֵ*/
#define CONFIG_INIT      0x000001   //���üĴ���
#define STATUS_INIT      0x000001   //״̬�Ĵ���
#define MASK_INIT        0x000000   //�ж����μĴ���
#define CYCLE_COUNT_INIT 0x000FA0   //���ڼ����Ĵ���
/*�������ϼĴ����ĳ�ʼ�����ֵ*/
#define  CONFIG_DATA      0x0010E1  //�����λ
#define  CALCULATE_DOWN   0x800000  //�����Ѿ�������ת������
#define  LOW_POWER        0x000004  //��⵽�͵�Դ
#define  WDT              0X000020  //��������5��δ����ȡ����λ��λ 
/*������������*/
#define  POWER_OFF_CTRL  0X88  //�������
#define  STANDERED_CTRL  0XC0  //��׼����
#define  TRANSFORM_START 0Xe8 //��ʼת��,ִ��������������
//����˲�����
#define	N	24
#define	CS_VRMS  0
#define  CS_IRMS  1
#define  CS_PRMS  2

#ifdef __cplusplus
extern "C"{
#endif

extern u8  g_ulCS5460UpdataFlag;//BIT0 =1:CS5460���� 0=������//BIT1=1:�й����ʸ��� 0=����


typedef struct
{
	vu32	volt_rms; // ȫ�ֱ���
	vu32  amp_rms;
	vu32  actpwr_rms;
	vu32  energy_w;//ʵʱ����W	
	
}meter_st;

extern meter_st cs5460a;

void cs5460_meter_read(meter_st *const cs5460a_me);//CS5460�¼�����
void cs5460a_check(void);//CS5460�Լ�
void CS5460IRQHandler(void);//����̬�Ĵ�������




double cs5460a_fliter_get(u8 CS);//CS5460����˲�
double cs5460a_activepwr_get(void);//CS5460�������ۼ�ֵ
double cs5460a_rmsvolt_get(void);//CS5460����ѹ
double cs5460a_rmsamp_get(void);//CS5460������

void cs5460a_wait_sr(void) ;//���DRDYλ�Ƿ���λ
void cs5460a_clear_sr(void);//CS5460��״̬�Ĵ���
void cs5460a_rddata(u8 reg,u8 *ptr);//CS5460������ֽ�
void cs5460a_wrdata(u8 *ptr,u8 n);//CS5460д����ֽ�

void cs540a_calibmode(u8 mode);//У׼CS5460����  
void cs5406a_finecalib(u8 *finedata); //��У׼��������ϸ��
void cs5460a_phasecompens(u8 data); //��λ����
void cs5460a_reboot(void); //CS5460����
void cs5460a_config_init(void); //CS5460���ó�ʼ��

void cs5460a_cs(unsigned char s);
void cs5460a_init(void);
void cs54060a_cs_gpio_init(void);

void meter_thread_entry(void *parameter);


#ifdef __cplusplus
}
#endif

#endif


#ifndef __DEMO_H__
#define __DEMO_H__

#include "mb.h"
#include "mbport.h"
//�������Ĵ��������ּĴ�������Ȧ����ʼ��ַ�Ͷ��������  

//--------------------------------------
//���ּĴ�����ʼ��ַ  
#define REG_HOLDING_START (0x0001)
//���ּĴ�������  
#define REG_HOLDING_NREGS (16) 
//����Ĵ�����ʼ��ַ  

//--------------------------------------
//����Ĵ�����ʼ��ַ 
#define REG_INPUT_START (0x0001) 
//����Ĵ�������  
#define REG_INPUT_NREGS (16)  
//��Ȧ�Ĵ�����ʼ��ַ  
//--------------------------------------

#define REG_COILS_START (0x0001)  
//��Ȧ�Ĵ�������  
#define REG_COILS_SIZE (16)  
//--------------------------------------
//��ɢ�Ĵ�����ʼ��ַ

#define   REG_DISCRETE_START (0x0001)
#define   REG_DISCRETE_SIZE (16)

//--------------------------------------

//���ּĴ�������  
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
//����Ĵ������� 
extern USHORT usRegInputBuf[REG_INPUT_NREGS];  
//��Ȧ�Ĵ�������  
extern UCHAR ucRegCoilsBuf[REG_COILS_SIZE/8];
//��ɢ����Ĵ������� 
extern UCHAR ucRegDiscreteBuf[REG_DISCRETE_SIZE/8];  

extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue);

void modbus_thread_entry(void *parameter);


#endif


#ifndef __DEMO_H__
#define __DEMO_H__

#include "mb.h"
#include "mbport.h"
//添加输入寄存器、保持寄存器、线圈的起始地址和定义的内容  

//--------------------------------------
//保持寄存器起始地址  
#define REG_HOLDING_START (0x0001)
//保持寄存器数量  
#define REG_HOLDING_NREGS (16) 
//输入寄存器起始地址  

//--------------------------------------
//输入寄存器起始地址 
#define REG_INPUT_START (0x0001) 
//输入寄存器数量  
#define REG_INPUT_NREGS (16)  
//线圈寄存器起始地址  
//--------------------------------------

#define REG_COILS_START (0x0001)  
//线圈寄存器数量  
#define REG_COILS_SIZE (16)  
//--------------------------------------
//离散寄存器起始地址

#define   REG_DISCRETE_START (0x0001)
#define   REG_DISCRETE_SIZE (16)

//--------------------------------------

//保持寄存器内容  
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
//输入寄存器内容 
extern USHORT usRegInputBuf[REG_INPUT_NREGS];  
//线圈寄存器内容  
extern UCHAR ucRegCoilsBuf[REG_COILS_SIZE/8];
//离散输入寄存器内容 
extern UCHAR ucRegDiscreteBuf[REG_DISCRETE_SIZE/8];  

extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue);

void modbus_thread_entry(void *parameter);


#endif

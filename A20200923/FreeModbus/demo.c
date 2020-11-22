

/* ----------------------- Modbus includes ----------------------------------*/

#include "demo.h"
/* ----------------------- Defines ------------------------------------------*/  

//保持寄存器内容  
//保持寄存器内容  
USHORT usRegHoldingBuf[REG_HOLDING_NREGS] ={0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};
//输入寄存器内容 
USHORT usRegInputBuf[REG_INPUT_NREGS] ={0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888};  
//线圈寄存器内容  
UCHAR ucRegCoilsBuf[REG_COILS_SIZE/8] = {0x00,0xFF};  
//离散输入寄存器内容 
UCHAR ucRegDiscreteBuf[REG_DISCRETE_SIZE/8] ={0};  

/* ----------------------- Start implementation -----------------------------*/
void modbus_thread_entry(void *parameter)
{
   eMBPoll();
}

//读输入寄存器函数 支持的命令为读 0x04  
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_INPUT_START );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

// 保持寄存器的读写函数 支持的命令为读 0x03 和写0x06 可读可写 
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,eMBRegisterMode eMode )
{
	//错误状态  
	eMBErrorCode eStatus = MB_ENOERR;  
	//偏移量  
	int16_t iRegIndex; 
	
	//判断寄存器是不是在范围内  
	if( ( (int16_t)usAddress >= REG_HOLDING_START )&& ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )  
	{  
		//计算偏移量  
		iRegIndex = ( int16_t )( usAddress - REG_HOLDING_START);  
		switch ( eMode )  
		{  
			//读处理函数  
			case MB_REG_READ:  
				while( usNRegs > 0 )  
				{  
					*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );  
					*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );  
					iRegIndex++;  
					usNRegs--;  
				}  
			break;  

			//写处理函数  
			case MB_REG_WRITE:  
				while( usNRegs > 0 )  
				{  
					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  
					usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;  
					iRegIndex++;  
					usNRegs--;  
				}  
			break;  
		}  
  }  
	else  
	{  
	   eStatus = MB_ENOREG; //返回错误状态   
	}  	
  return eStatus;
}
 
//线圈处理函数 可读可写 支持的命令为读 0x01 0x05  
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )  
{  
	//错误状态  
	eMBErrorCode eStatus = MB_ENOERR;  
	//寄存器个数  
	USHORT iNCoils = ( int16_t )usNCoils;  
	//寄存器偏移量  
	USHORT usBitOffset;  
	//判断寄存器是不是在范围内  
	if(( (USHORT)usAddress >= REG_COILS_START ) &&( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )  
	{  
		//计算寄存器偏移量  
		usBitOffset = ( USHORT )( usAddress - REG_COILS_START );  
		switch ( eMode )  
		{  
		//读操作  
		case MB_REG_READ:  
			while( iNCoils > 0 )  
			{  
				*pucRegBuffer++ = xMBUtilGetBits(&ucRegCoilsBuf[0], usBitOffset,(UCHAR)( iNCoils > 8 ? 8 : iNCoils ));  
				iNCoils -= 8;  
				usBitOffset += 8;  
			}  
		break; 
		//写操作  
		case MB_REG_WRITE:  
			while( iNCoils > 0 )  
			{  
				xMBUtilSetBits(ucRegCoilsBuf, usBitOffset,(UCHAR)( iNCoils > 8 ? 8 : iNCoils ), *pucRegBuffer++ );  
				iNCoils -= 8;  
			}  
		break;  
		}  
	}  
	else  
	{  
	  eStatus = MB_ENOREG;  
	}  
	return eStatus;   
}  

//开关输入寄存器处理函数 可读可写 支持的命令为读 0x02  
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	  //操作寄存器个数
	  USHORT iNDiscrete = usNDiscrete;
	  //寄存器偏移量  
	  USHORT usBitOffset;  	
    if ((usAddress >= REG_DISCRETE_START)&& (usAddress + usNDiscrete  <= REG_DISCRETE_START + REG_DISCRETE_SIZE))
    {
		    //获取偏移量  
		    usBitOffset = (USHORT)( usAddress-REG_DISCRETE_START); 	
			
        while( iNDiscrete > 0 )
        {
					#if 0
					 *pucRegBuffer++ = xMBUtilGetBits(ucRegDiscreteBuf, usBitOffset,(UCHAR)( iNDiscrete > 8 ? 8 : iNDiscrete )); 
            iNDiscrete-=8;
            usBitOffset+=8;
					#endif
            *pucRegBuffer++ = ( unsigned char )( ucRegDiscreteBuf[usBitOffset]);
            iNDiscrete--;
            usBitOffset++;
					
        }			
    }
    else
    {
        eStatus = MB_ENOREG;
    }	
		
    return eStatus;
}



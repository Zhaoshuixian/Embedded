

/* ----------------------- Modbus includes ----------------------------------*/

#include "demo.h"
/* ----------------------- Defines ------------------------------------------*/  

//���ּĴ�������  
//���ּĴ�������  
USHORT usRegHoldingBuf[REG_HOLDING_NREGS] ={0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};
//����Ĵ������� 
USHORT usRegInputBuf[REG_INPUT_NREGS] ={0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888};  
//��Ȧ�Ĵ�������  
UCHAR ucRegCoilsBuf[REG_COILS_SIZE/8] = {0x00,0xFF};  
//��ɢ����Ĵ������� 
UCHAR ucRegDiscreteBuf[REG_DISCRETE_SIZE/8] ={0};  

/* ----------------------- Start implementation -----------------------------*/
void modbus_thread_entry(void *parameter)
{
   eMBPoll();
}

//������Ĵ������� ֧�ֵ�����Ϊ�� 0x04  
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

// ���ּĴ����Ķ�д���� ֧�ֵ�����Ϊ�� 0x03 ��д0x06 �ɶ���д 
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,eMBRegisterMode eMode )
{
	//����״̬  
	eMBErrorCode eStatus = MB_ENOERR;  
	//ƫ����  
	int16_t iRegIndex; 
	
	//�жϼĴ����ǲ����ڷ�Χ��  
	if( ( (int16_t)usAddress >= REG_HOLDING_START )&& ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )  
	{  
		//����ƫ����  
		iRegIndex = ( int16_t )( usAddress - REG_HOLDING_START);  
		switch ( eMode )  
		{  
			//��������  
			case MB_REG_READ:  
				while( usNRegs > 0 )  
				{  
					*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );  
					*pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );  
					iRegIndex++;  
					usNRegs--;  
				}  
			break;  

			//д������  
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
	   eStatus = MB_ENOREG; //���ش���״̬   
	}  	
  return eStatus;
}
 
//��Ȧ������ �ɶ���д ֧�ֵ�����Ϊ�� 0x01 0x05  
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )  
{  
	//����״̬  
	eMBErrorCode eStatus = MB_ENOERR;  
	//�Ĵ�������  
	USHORT iNCoils = ( int16_t )usNCoils;  
	//�Ĵ���ƫ����  
	USHORT usBitOffset;  
	//�жϼĴ����ǲ����ڷ�Χ��  
	if(( (USHORT)usAddress >= REG_COILS_START ) &&( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )  
	{  
		//����Ĵ���ƫ����  
		usBitOffset = ( USHORT )( usAddress - REG_COILS_START );  
		switch ( eMode )  
		{  
		//������  
		case MB_REG_READ:  
			while( iNCoils > 0 )  
			{  
				*pucRegBuffer++ = xMBUtilGetBits(&ucRegCoilsBuf[0], usBitOffset,(UCHAR)( iNCoils > 8 ? 8 : iNCoils ));  
				iNCoils -= 8;  
				usBitOffset += 8;  
			}  
		break; 
		//д����  
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

//��������Ĵ��������� �ɶ���д ֧�ֵ�����Ϊ�� 0x02  
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	  //�����Ĵ�������
	  USHORT iNDiscrete = usNDiscrete;
	  //�Ĵ���ƫ����  
	  USHORT usBitOffset;  	
    if ((usAddress >= REG_DISCRETE_START)&& (usAddress + usNDiscrete  <= REG_DISCRETE_START + REG_DISCRETE_SIZE))
    {
		    //��ȡƫ����  
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



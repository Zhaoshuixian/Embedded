

#include "mb_host.h"


	modbus_st mb_host;
//---主机发送帧创建
void modbus_host_send_create(modbus_st *const mb_host_me,unsigned char dev_addr,unsigned char func,unsigned short int start_reg, unsigned short int reg_num,unsigned char *reg_val)
{
  //F03
	//HOST发送帧：设备地址+功能码+起始地址+寄存器数目+CRC
	//SLAVE响应帧：设备地址+功能码+字节数+寄存器值+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败	
	
	//F06
	//注意：接收和发送数据一致
	//HOST发送帧：设备地址+功能码+起始地址+寄存器值+CRC
	//SLAVE响应帧：设备地址+功能码+起始地址+寄存器值+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败	
	
	//F10
	//HOST发送帧：设备地址+功能码+起始地址+寄存器数量+字节数+寄存器值+CRC
	//SLAVE响应帧：设备地址+功能码+起始地址+寄存器数量+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败	
	
	unsigned short int ulByteSize = 0;
  unsigned short int ulCrc = 0;
//--同一处	
	mb_host_me->data.buff[ulByteSize++] =dev_addr;//设备地址
	mb_host_me->data.buff[ulByteSize++] =func;	//功能码
	mb_host_me->data.buff[ulByteSize++] =start_reg>>8;//寄存器地址H
	mb_host_me->data.buff[ulByteSize++] =start_reg&0xFF;;//寄存器地址L
//--不同处	
	switch(func)//执行功能码
	{
		 case 0x03://HOST发送帧：设备地址+功能码+起始地址+寄存器数目+CRC
				mb_host_me->data.buff[ulByteSize++] =reg_num>>8;//寄存器数目H
				mb_host_me->data.buff[ulByteSize++] =reg_num&0xFF;;//寄存器数目L		 
			 break;
		 case 0x06://HOST发送帧：设备地址+功能码+起始地址+寄存器值+CRC
				mb_host_me->data.buff[ulByteSize++] =*reg_val>>8;//寄存器数目H
				mb_host_me->data.buff[ulByteSize++] =*reg_val&0xFF;;//寄存器数目L			 
			 break;
		 case 0x10://HOST发送帧：设备地址+功能码+起始地址+寄存器数量+字节数+寄存器值+CRC
			 mb_host_me->data.buff[ulByteSize++] =(1<<reg_num);//字节数
			 for(unsigned char i=0;i<(1<<reg_num);i++)
			 {
				 mb_host_me->data.buff[ulByteSize++] =(*reg_val++)>>8;//寄存器值H
				 mb_host_me->data.buff[ulByteSize++] =(*reg_val++)&0xFF;;//寄存器值L			
			 }
			 break;
		 default:
			 break;
	}
//--负载校验位	
	ulCrc =  mb_host_me->check(&mb_host_me->data.buff[0],ulByteSize-2);
	mb_host_me->data.buff[ulByteSize-1] = (unsigned char)(ulCrc);		
	mb_host_me->data.buff[ulByteSize-2] = (unsigned char)(ulCrc >> 8);	
	
	mb_host_me->transmit(&mb_host_me->data.buff[0],ulByteSize,mb_host_me->enport);//执行发送	
}

//---从机响应帧处理
unsigned char modbus_host_recev_handler(modbus_st *const mb_host_me,unsigned char *data_buff,unsigned char data_len)
{
	 //失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	 if((4>data_len))//最小长度错误
	 {
		 data_len=0;
     return 1;//
	 }
	 else if(0!=mb_host_me->check(data_buff,data_len))//校验不一致
	 {
		 data_len=0;
     return 1;
	 }
	 else
	 {
		 switch(data_buff[1])//功能码
		 {
			 case 0x03://SLAVE响应帧：设备地址+功能码+字节数+寄存器值+CRC
				 if(mb_host_me->data.buff[0]==data_buff[0])
				 {
					 unsigned char ulByteSize=data_buff[2];//字节数
					 while(ulByteSize>0)
					 {
		//				 =data_buff[3+(ulByteSize>>1)]//寄存器值H
		//				 =data_buff[4+(ulByteSize>>1)]//寄存器值H	
						 ulByteSize-=2;				 
					 }
           return 0;					 
				 }
				 break;
			 case 0x06://SLAVE响应帧：设备地址+功能码+起始地址+寄存器值+CRC
				 if(mb_host_me->data.buff[0]==data_buff[0])
				 {
					 return 0;
				 }
				 break;	
			 case 0x10://SLAVE响应帧：设备地址+功能码+起始地址+寄存器数量+CRC
				 if(mb_host_me->data.buff[0]==data_buff[0])
				 {
					   return 0;
				 }				 
				 break;	
			 default:
				 break;		 
		 }
   }
	 return 0;
}

void modbus_host_handler(void)
{
	/*
		3.5个字符的时间间隔，只是用在RTU模式下面，因为RTU模式没有开始符和结束符，
		两个数据包之间只能靠时间间隔来区分，Modbus定义在不同的波特率下，间隔时间是不一样的，
		所以就是3.5个字符的时间，波特率高，这个时间间隔就小，波特率低，这个时间间隔相应就大
		4800  = 7.297ms
		9600  = 3.646ms
		19200  = 1.771ms
		38400  = 0.885ms
	*/
	unsigned short int t3_5_timer=0;
	if(100<t3_5_timer++)
	{
		//modbus_host_recev_handler(&mb_host,UART1_RBuf,UART1_RLen);
		t3_5_timer=0;
	}

//	while(100<t3_5_timer++)
//	{
//		if(0==modbus_host_recev_handler(&mb_host,UART1_RBuf,UART1_RLen)) 
//		{
//			t3_5_timer=0;
//			return;//成功返回
//		}
//	}
//	if(100<t3_5_timer)
//	{
//		return;//失败返回
//	}
}

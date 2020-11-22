

#include "mb_host.h"


	modbus_st mb_host;
//---��������֡����
void modbus_host_send_create(modbus_st *const mb_host_me,unsigned char dev_addr,unsigned char func,unsigned short int start_reg, unsigned short int reg_num,unsigned char *reg_val)
{
  //F03
	//HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�����Ŀ+CRC
	//SLAVE��Ӧ֡���豸��ַ+������+�ֽ���+�Ĵ���ֵ+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��	
	
	//F06
	//ע�⣺���պͷ�������һ��
	//HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
	//SLAVE��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��	
	
	//F10
	//HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+�ֽ���+�Ĵ���ֵ+CRC
	//SLAVE��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��	
	
	unsigned short int ulByteSize = 0;
  unsigned short int ulCrc = 0;
//--ͬһ��	
	mb_host_me->data.buff[ulByteSize++] =dev_addr;//�豸��ַ
	mb_host_me->data.buff[ulByteSize++] =func;	//������
	mb_host_me->data.buff[ulByteSize++] =start_reg>>8;//�Ĵ�����ַH
	mb_host_me->data.buff[ulByteSize++] =start_reg&0xFF;;//�Ĵ�����ַL
//--��ͬ��	
	switch(func)//ִ�й�����
	{
		 case 0x03://HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�����Ŀ+CRC
				mb_host_me->data.buff[ulByteSize++] =reg_num>>8;//�Ĵ�����ĿH
				mb_host_me->data.buff[ulByteSize++] =reg_num&0xFF;;//�Ĵ�����ĿL		 
			 break;
		 case 0x06://HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
				mb_host_me->data.buff[ulByteSize++] =*reg_val>>8;//�Ĵ�����ĿH
				mb_host_me->data.buff[ulByteSize++] =*reg_val&0xFF;;//�Ĵ�����ĿL			 
			 break;
		 case 0x10://HOST����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+�ֽ���+�Ĵ���ֵ+CRC
			 mb_host_me->data.buff[ulByteSize++] =(1<<reg_num);//�ֽ���
			 for(unsigned char i=0;i<(1<<reg_num);i++)
			 {
				 mb_host_me->data.buff[ulByteSize++] =(*reg_val++)>>8;//�Ĵ���ֵH
				 mb_host_me->data.buff[ulByteSize++] =(*reg_val++)&0xFF;;//�Ĵ���ֵL			
			 }
			 break;
		 default:
			 break;
	}
//--����У��λ	
	ulCrc =  mb_host_me->check(&mb_host_me->data.buff[0],ulByteSize-2);
	mb_host_me->data.buff[ulByteSize-1] = (unsigned char)(ulCrc);		
	mb_host_me->data.buff[ulByteSize-2] = (unsigned char)(ulCrc >> 8);	
	
	mb_host_me->transmit(&mb_host_me->data.buff[0],ulByteSize,mb_host_me->enport);//ִ�з���	
}

//---�ӻ���Ӧ֡����
unsigned char modbus_host_recev_handler(modbus_st *const mb_host_me,unsigned char *data_buff,unsigned char data_len)
{
	 //ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	 if((4>data_len))//��С���ȴ���
	 {
		 data_len=0;
     return 1;//
	 }
	 else if(0!=mb_host_me->check(data_buff,data_len))//У�鲻һ��
	 {
		 data_len=0;
     return 1;
	 }
	 else
	 {
		 switch(data_buff[1])//������
		 {
			 case 0x03://SLAVE��Ӧ֡���豸��ַ+������+�ֽ���+�Ĵ���ֵ+CRC
				 if(mb_host_me->data.buff[0]==data_buff[0])
				 {
					 unsigned char ulByteSize=data_buff[2];//�ֽ���
					 while(ulByteSize>0)
					 {
		//				 =data_buff[3+(ulByteSize>>1)]//�Ĵ���ֵH
		//				 =data_buff[4+(ulByteSize>>1)]//�Ĵ���ֵH	
						 ulByteSize-=2;				 
					 }
           return 0;					 
				 }
				 break;
			 case 0x06://SLAVE��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
				 if(mb_host_me->data.buff[0]==data_buff[0])
				 {
					 return 0;
				 }
				 break;	
			 case 0x10://SLAVE��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+CRC
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
		3.5���ַ���ʱ������ֻ������RTUģʽ���棬��ΪRTUģʽû�п�ʼ���ͽ�������
		�������ݰ�֮��ֻ�ܿ�ʱ���������֣�Modbus�����ڲ�ͬ�Ĳ������£����ʱ���ǲ�һ���ģ�
		���Ծ���3.5���ַ���ʱ�䣬�����ʸߣ����ʱ������С�������ʵͣ����ʱ������Ӧ�ʹ�
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
//			return;//�ɹ�����
//		}
//	}
//	if(100<t3_5_timer)
//	{
//		return;//ʧ�ܷ���
//	}
}

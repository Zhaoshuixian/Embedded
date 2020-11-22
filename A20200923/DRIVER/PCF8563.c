
#include "pcf8563.h"
#include "autom.h"
#include "delay.h"

i2c_st pcf8563_i2c=
{
	.trans_mode =0,//MSB��ʼ����
	.gpio_init  = pcf8563_gpio_init,	
	.sda_mode   = pcf8563_sda_mode,
	.sda_in     = pcf8563_sda_read,
	.sda_out    = pcf8563_sda_out,
	.sck_out    = pcf8563_sck_out,
  .delay_us   = delay_us,
  .delay_ms   = delay_ms,	
};

u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	 

//-----------------��Ҫʵ�ֵĶԽ�IICЭ��ӿڵĺ���-----------------
void pcf8563_gpio_init(void)
{
 	GPIO_InitType GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(PCF8563_PCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pins = PCF8563_SCL_PIN|PCF8563_SDA_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(PCF8563_PORT, &GPIO_InitStructure); 	
}

void pcf8563_sda_mode(unsigned char s)
{
	if(0!=s)
	{
		SDA_OUT2();
	}
	else
	{
		SDA_IN2();	
	}
}

void pcf8563_sda_out(unsigned char s)
{
	IIC_SDA2(s);
}

void pcf8563_sck_out(unsigned char s)
{
	IIC_SCL2(s);
}

unsigned char pcf8563_sda_read(void)
{
	return READ_SDA2;
}

//--------------------------------------------------------------

//��PCF8563ָ����ַ����һ������
unsigned char  pcf8563_readbyte_byaddr(unsigned short int reg_addr)
{				  
	unsigned char  temp=0;	
	
  iic_start(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,PCF8563_WR_ADDR);   //����������ַPCF8563_RD_ADDR,д���� 	
	iic_wait_ack(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,reg_addr%256);   //���͵͵�ַ
	iic_wait_ack(&pcf8563_i2c);
	iic_start(&pcf8563_i2c);  	 	   
	iic_sendbyte(&pcf8563_i2c,PCF8563_RD_ADDR);           //�������ģʽ		   
	iic_wait_ack(&pcf8563_i2c);	 
	temp=iic_readbyte(&pcf8563_i2c,0);		
	iic_nack(&pcf8563_i2c);//���ͷ�Ӧλ	
	iic_stop(&pcf8563_i2c);//����һ��ֹͣ����	
	
	return temp;
}
//��PCF8563ָ����ַд��һ������
void pcf8563_writebyte_byaddr(unsigned short int waddr,unsigned char wdata)
{				   	  	    																 
  iic_start(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,PCF8563_WR_ADDR);   //����������ַPCF8563_WR_ADDR,д���� 	 
	iic_wait_ack(&pcf8563_i2c);	 
	iic_sendbyte(&pcf8563_i2c,waddr%256);   //���͵͵�ַ
	iic_wait_ack(&pcf8563_i2c);										  		   
	iic_sendbyte(&pcf8563_i2c,wdata);     //�����ֽ�							   
	iic_wait_ack(&pcf8563_i2c);		    	   
	iic_stop(&pcf8563_i2c);//����һ��ֹͣ����	
 	iic_delayms(&pcf8563_i2c,10);
}
//��PCF8563�����ָ����ַ��ʼд�볤��ΪLen������
unsigned char pcf8563_writedata_byaddr(unsigned short int waddr,unsigned char *wdata,unsigned char wdata_len)
{  	

	for(unsigned char t=0;t<wdata_len;t++)
	{
		pcf8563_writebyte_byaddr(waddr,*wdata);
		waddr++;
		wdata++;
	}	 
  return 0;
}
//��PCF8563�����ָ����ַ��ʼ��������ΪLen������
unsigned char pcf8563_readdata_byaddr(unsigned short int raddr,unsigned char *rdata,unsigned char rdata_len)
{  	
	unsigned char  *temp;
	temp = rdata;          
	for(unsigned char t=0;t<rdata_len;t++)
	{
		*rdata=pcf8563_readbyte_byaddr(raddr);
		 raddr++;
		 rdata++;
	}	
	if((temp[5]==0)||(temp[3]==0))
	{
		return 0xff;
	}
	return 0;//��ȡ��ȷʱ����0
}

//PCF8563ʱ������ָ��
unsigned char  pcf8563_start(void)
{
 	unsigned char startcode=0x00;
	if(0==pcf8563_writedata_byaddr(CTRL_STU1,&startcode,1)) return 0;
 	else return 1;	
}

//PCF8563ʱ��ָֹͣ��
unsigned char pcf8563_stop(void)
{
 	unsigned char stopcode=0x20;
 	if(0==pcf8563_writedata_byaddr(CTRL_STU1,&stopcode,1))
		return 0;
 	else return 1;	
}


//�жϲ��������TF��־λ
unsigned char pcf8563_clrint(void)
{
 	unsigned char temp=0x12;
	if(0==pcf8563_writedata_byaddr(CTRL_STU2,&temp,1))return 0;
 	else return 1;
}

//��BCD��ת��Ϊʮ������
unsigned char bcd_converthex(unsigned char cdata)
{
	unsigned char temp;
	
	temp=cdata/16;
	cdata%=16;
	cdata=cdata+temp*10;
	
	return cdata;	
}

//��ʮ����������ת��ΪBCD��
void arr_convertbcd(unsigned char *arr,unsigned char arr_len)
{
	unsigned char temp;
	for(;arr_len>0;arr_len--)
	{
		temp=*arr/10;
		*arr%=10;
		*arr=*arr+temp*16;
		arr++;
	}	
}

//��һλʮ��������ת��ΪBCD�룬������
unsigned char hex_converbcd(unsigned char cdata)
{
	unsigned char temp;
	temp=cdata/10;
	cdata%=10;
	cdata=cdata+temp*16;
	
	return cdata;		
}

//#define DATA_TEST

void maketime_convert(rtc_st *const rtc_me)
{
  /*  
  =======bit0123456789A=======
  __DATE__:"Apr 23 2019"
  __TIME__:"13:57:57"
  */
  unsigned char spkTimeBuf[12];
	
	#if defined (DATA_TEST) /*�̶�ʱ�����ݲ���*/
	
  unsigned char *dateBuf = (unsigned char *)"May 22 2019";
  unsigned char *timeBuf = (unsigned char *)"12:30:15";		
	#else
  unsigned char *dateBuf = (unsigned char *)COMPILE_STR_DATE;
  unsigned char *timeBuf = (unsigned char *)COMPILE_STR_TIME;	
	#endif

  memset(spkTimeBuf, 0x00, sizeof(spkTimeBuf));

  spkTimeBuf[0] = (unsigned char)((dateBuf[7] - '0')); // 2      
  spkTimeBuf[1] = (unsigned char)((dateBuf[8] - '0')); // 0        
  spkTimeBuf[2] = (unsigned char)((dateBuf[9] - '0')); // 1        
  spkTimeBuf[3] = (unsigned char)((dateBuf[10] - '0')); // 9
	
  rtc_me->year=((unsigned short int)(spkTimeBuf[0]*1000));
  rtc_me->year+=((unsigned short int)(spkTimeBuf[1]*100));
  rtc_me->year+=((unsigned short int)(spkTimeBuf[2]*10));	
  rtc_me->year+=(unsigned short int)spkTimeBuf[3];

  rtc_me->mon = STRM_TO_DEC(COMPILE_STR_DATE);

  spkTimeBuf[4] = (unsigned char)((dateBuf[4] - '0'));  // �� ʮλ  2 
  if (dateBuf[4] ==' ')
  {
    spkTimeBuf[4] = 0;
  }
	rtc_me->day =spkTimeBuf[4]*10;
  spkTimeBuf[5] = (unsigned char)((dateBuf[5] - '0'));  // �� ��λ  3	
	rtc_me->day += spkTimeBuf[5];
	
  spkTimeBuf[6]  = (unsigned char)((timeBuf[0] - '0'));  //  1 // hour
	rtc_me->hour=(spkTimeBuf[6]*10);
  spkTimeBuf[7]= (unsigned char)((timeBuf[1] - '0'));  //  6
	rtc_me->hour+=spkTimeBuf[7]; 
  spkTimeBuf[8] = (unsigned char)((timeBuf[3] - '0'));  //  0 // minute
	rtc_me->min=(spkTimeBuf[8]*10);
  spkTimeBuf[9]= (unsigned char)((timeBuf[4] - '0'));  //  7
  rtc_me->min+=spkTimeBuf[9];
  spkTimeBuf[10] = (unsigned char)((timeBuf[6] - '0'));  //  5  // second
  rtc_me->sec=(spkTimeBuf[10]*10);	
  spkTimeBuf[11]= (unsigned char)((timeBuf[7] - '0'));  //  2	
  rtc_me->sec+=spkTimeBuf[11];
}

unsigned char pcf8563_rtc_init(void)
{
	iic_init(&pcf8563_i2c);
	pcf8563_start();
	//��һ������
	if(pcf8563_readbyte_byaddr(ALARM_MIN_REG) != 0x88)		//оƬ�Ƿ��Ѿ����ù���ʹ�÷��ӱ����Ĵ�����Ϊ�жϣ�0x88�رձ����ж�
	{	 			
 		pcf8563_writebyte_byaddr(ALARM_MIN_REG,0X88);//������д��PCF8563��Ӧ�Ĵ���
		#if 0
			g_ulAutoControl.sys_rtc.year=2020;
			g_ulAutoControl.sys_rtc.mon=5;
			g_ulAutoControl.sys_rtc.day=11;
			g_ulAutoControl.sys_rtc.hour=19;
			g_ulAutoControl.sys_rtc.min=0;
			g_ulAutoControl.sys_rtc.sec=0;
		#else
		  maketime_convert(&g_ulAutoControl.sys_rtc);//��ȡ������������ʱ��
		#endif
		#ifdef DEBUG_MODE
			printf("rtc first config!\r\n");	
		#endif
		pcf8563_rtc_set(&g_ulAutoControl.sys_rtc);   //����ʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���ڡ�BCD���ʾ2001-8-08 08:08:00
	}		    				     
	pcf8563_rtc_get(&g_ulAutoControl.sys_rtc);//����ʱ��
	return 0;	
}

//����ʱ��
unsigned char pcf8563_rtc_set(rtc_st *const rtc_me)
{
	 unsigned char hcb_time[7];
	//Ϊ�����¼��ݣ���ʹ��ʮ���������ݽ��д��룬��8563ʹ�õ���BCD�룬������Ҫ��ʮ������ת����BCD��
	hcb_time[6]=hex_converbcd((rtc_me->year-2000));//��
	hcb_time[5]=hex_converbcd(rtc_me->mon);//��
	hcb_time[4]=pcf8563_rtc_week_get(rtc_me);//����
	hcb_time[3]=hex_converbcd(rtc_me->day);//��
	hcb_time[2]=hex_converbcd(rtc_me->hour);//ʱ
	hcb_time[1]=hex_converbcd(rtc_me->min);//��
	hcb_time[0]=hex_converbcd(rtc_me->sec);//��
	pcf8563_stop();

	if(0==pcf8563_writedata_byaddr(2,hcb_time,7))
	{
		pcf8563_start();
		return 0;
	}
	else 
	{
		return 1; 
	}		
}

//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������
unsigned char rtemp[7];
unsigned char pcf8563_rtc_get(rtc_st *const rtc_me)
{

	if(0==pcf8563_readdata_byaddr(2,rtemp,7))
	{
		rtemp[6] = rtemp[6];//��
		rtemp[5] &= 0x1F;//��
		rtemp[4] &= 0x07;//����
		rtemp[3] &= 0x3F;//��
		rtemp[2] &= 0x3F;//ʱ
		rtemp[1] &= 0x7F;//��
		rtemp[0] &= 0x7F;//�� 

		rtc_me->year   = bcd_converthex(rtemp[6])+2000; 
		rtc_me->mon    = bcd_converthex(rtemp[5]); 
		rtc_me->day    = bcd_converthex(rtemp[3]); 
		rtc_me->hour   = bcd_converthex(rtemp[2]); 
		rtc_me->min    = bcd_converthex(rtemp[1]); 
		rtc_me->sec    = bcd_converthex(rtemp[0]); 
		rtc_me->week   = bcd_converthex(rtemp[4]); 
		//������0ʱ������7
		if(rtc_me->week==0)
		{
			rtc_me->week=7;
		}
		return 0;
	}	  
	else return 1;
}

//��������:���빫�����ڵõ����ں�(ֻ����1901-2099��)	
unsigned char pcf8563_rtc_week_get(rtc_st *const rtc_me)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=rtc_me->year/100;	yearL=rtc_me->year%100;   
	if (yearH>19)yearL+=100;// ���Ϊ21����,�������100
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+rtc_me->day+table_week[rtc_me->mon-1];
	if (yearL%4==0&&rtc_me->mon<3)temp2--;
	return(temp2%7);
} 

//u8	RTC_Get_Week(u16 year,u8 month,u8 day)
//{	
//	u16 temp2;
//	u8 yearH,yearL;
//	yearH=year/100;	yearL=year%100;   
//	if (yearH>19)yearL+=100;// ���Ϊ21����,�������100
//	// ����������ֻ��1900��֮���  
//	temp2=yearL+yearL/4;
//	temp2=temp2%7; 
//	temp2=temp2+day+table_week[month-1];
//	if (yearL%4==0&&month<3)temp2--;
//	return(temp2%7);
//} 

void rtc_thread_entry(void *parameter)
{
	pcf8563_rtc_get(&g_ulAutoControl.sys_rtc);
	#if 0
	printf("%04d-%02d-%02d %02d:%02d:%02d ",g_ulAutoControl.sys_rtc.year,\
	g_ulAutoControl.sys_rtc.mon,\
	g_ulAutoControl.sys_rtc.day,\
	g_ulAutoControl.sys_rtc.hour,\
	g_ulAutoControl.sys_rtc.min,\
	g_ulAutoControl.sys_rtc.sec);
	switch(g_ulAutoControl.sys_rtc.week)
	{
		case 1:
			printf("Monday");
			break;
		case 2:
			printf("Tuesday");
			break;	
		case 3:
			printf("Wensday");
			break;
		case 4:
			printf("Thursday");
			break;	
		case 5:
			printf("Friday");
			break;
		case 6:
			printf("Saturday");
			break;		
		case 7:
			printf("Sunday");
			break;	
    default:
			printf("%d",g_ulAutoControl.sys_rtc.week);		
			printf("Week Error!");		
     break;			
	}
	printf("\r\n");
  #endif	
}


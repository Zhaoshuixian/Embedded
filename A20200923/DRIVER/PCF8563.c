
#include "pcf8563.h"
#include "autom.h"
#include "delay.h"

i2c_st pcf8563_i2c=
{
	.trans_mode =0,//MSB开始传输
	.gpio_init  = pcf8563_gpio_init,	
	.sda_mode   = pcf8563_sda_mode,
	.sda_in     = pcf8563_sda_read,
	.sda_out    = pcf8563_sda_out,
	.sck_out    = pcf8563_sck_out,
  .delay_us   = delay_us,
  .delay_ms   = delay_ms,	
};

u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	 

//-----------------需要实现的对接IIC协议接口的函数-----------------
void pcf8563_gpio_init(void)
{
 	GPIO_InitType GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(PCF8563_PCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pins = PCF8563_SCL_PIN|PCF8563_SDA_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
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

//在PCF8563指定地址读出一个数据
unsigned char  pcf8563_readbyte_byaddr(unsigned short int reg_addr)
{				  
	unsigned char  temp=0;	
	
  iic_start(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,PCF8563_WR_ADDR);   //发送器件地址PCF8563_RD_ADDR,写数据 	
	iic_wait_ack(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,reg_addr%256);   //发送低地址
	iic_wait_ack(&pcf8563_i2c);
	iic_start(&pcf8563_i2c);  	 	   
	iic_sendbyte(&pcf8563_i2c,PCF8563_RD_ADDR);           //进入接收模式		   
	iic_wait_ack(&pcf8563_i2c);	 
	temp=iic_readbyte(&pcf8563_i2c,0);		
	iic_nack(&pcf8563_i2c);//发送非应位	
	iic_stop(&pcf8563_i2c);//产生一个停止条件	
	
	return temp;
}
//在PCF8563指定地址写入一个数据
void pcf8563_writebyte_byaddr(unsigned short int waddr,unsigned char wdata)
{				   	  	    																 
  iic_start(&pcf8563_i2c);	
	iic_sendbyte(&pcf8563_i2c,PCF8563_WR_ADDR);   //发送器件地址PCF8563_WR_ADDR,写数据 	 
	iic_wait_ack(&pcf8563_i2c);	 
	iic_sendbyte(&pcf8563_i2c,waddr%256);   //发送低地址
	iic_wait_ack(&pcf8563_i2c);										  		   
	iic_sendbyte(&pcf8563_i2c,wdata);     //发送字节							   
	iic_wait_ack(&pcf8563_i2c);		    	   
	iic_stop(&pcf8563_i2c);//产生一个停止条件	
 	iic_delayms(&pcf8563_i2c,10);
}
//在PCF8563里面的指定地址开始写入长度为Len的数据
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
//在PCF8563里面的指定地址开始读出长度为Len的数据
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
	return 0;//读取正确时返回0
}

//PCF8563时钟启动指令
unsigned char  pcf8563_start(void)
{
 	unsigned char startcode=0x00;
	if(0==pcf8563_writedata_byaddr(CTRL_STU1,&startcode,1)) return 0;
 	else return 1;	
}

//PCF8563时钟停止指令
unsigned char pcf8563_stop(void)
{
 	unsigned char stopcode=0x20;
 	if(0==pcf8563_writedata_byaddr(CTRL_STU1,&stopcode,1))
		return 0;
 	else return 1;	
}


//中断产生后清除TF标志位
unsigned char pcf8563_clrint(void)
{
 	unsigned char temp=0x12;
	if(0==pcf8563_writedata_byaddr(CTRL_STU2,&temp,1))return 0;
 	else return 1;
}

//将BCD码转化为十六进制
unsigned char bcd_converthex(unsigned char cdata)
{
	unsigned char temp;
	
	temp=cdata/16;
	cdata%=16;
	cdata=cdata+temp*10;
	
	return cdata;	
}

//将十六进制数组转化为BCD码
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

//将一位十六进制数转化为BCD码，并返回
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
	
	#if defined (DATA_TEST) /*固定时间数据测试*/
	
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

  spkTimeBuf[4] = (unsigned char)((dateBuf[4] - '0'));  // 日 十位  2 
  if (dateBuf[4] ==' ')
  {
    spkTimeBuf[4] = 0;
  }
	rtc_me->day =spkTimeBuf[4]*10;
  spkTimeBuf[5] = (unsigned char)((dateBuf[5] - '0'));  // 日 个位  3	
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
	//第一次配置
	if(pcf8563_readbyte_byaddr(ALARM_MIN_REG) != 0x88)		//芯片是否已经配置过，使用分钟报警寄存器作为判断，0x88关闭报警中断
	{	 			
 		pcf8563_writebyte_byaddr(ALARM_MIN_REG,0X88);//将数据写入PCF8563相应寄存器
		#if 0
			g_ulAutoControl.sys_rtc.year=2020;
			g_ulAutoControl.sys_rtc.mon=5;
			g_ulAutoControl.sys_rtc.day=11;
			g_ulAutoControl.sys_rtc.hour=19;
			g_ulAutoControl.sys_rtc.min=0;
			g_ulAutoControl.sys_rtc.sec=0;
		#else
		  maketime_convert(&g_ulAutoControl.sys_rtc);//获取编译器最后编译时间
		#endif
		#ifdef DEBUG_MODE
			printf("rtc first config!\r\n");	
		#endif
		pcf8563_rtc_set(&g_ulAutoControl.sys_rtc);   //设置时间，年、月、日、时、分、秒、星期。BCD码表示2001-8-08 08:08:00
	}		    				     
	pcf8563_rtc_get(&g_ulAutoControl.sys_rtc);//更新时间
	return 0;	
}

//设置时钟
unsigned char pcf8563_rtc_set(rtc_st *const rtc_me)
{
	 unsigned char hcb_time[7];
	//为了向下兼容，将使用十六进制数据进行传入，而8563使用的是BCD码，所以需要将十六进制转换成BCD码
	hcb_time[6]=hex_converbcd((rtc_me->year-2000));//年
	hcb_time[5]=hex_converbcd(rtc_me->mon);//月
	hcb_time[4]=pcf8563_rtc_week_get(rtc_me);//星期
	hcb_time[3]=hex_converbcd(rtc_me->day);//日
	hcb_time[2]=hex_converbcd(rtc_me->hour);//时
	hcb_time[1]=hex_converbcd(rtc_me->min);//分
	hcb_time[0]=hex_converbcd(rtc_me->sec);//秒
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

//得到当前的时间
//返回值:0,成功;其他:错误代码
unsigned char rtemp[7];
unsigned char pcf8563_rtc_get(rtc_st *const rtc_me)
{

	if(0==pcf8563_readdata_byaddr(2,rtemp,7))
	{
		rtemp[6] = rtemp[6];//年
		rtemp[5] &= 0x1F;//月
		rtemp[4] &= 0x07;//星期
		rtemp[3] &= 0x3F;//日
		rtemp[2] &= 0x3F;//时
		rtemp[1] &= 0x7F;//分
		rtemp[0] &= 0x7F;//秒 

		rtc_me->year   = bcd_converthex(rtemp[6])+2000; 
		rtc_me->mon    = bcd_converthex(rtemp[5]); 
		rtc_me->day    = bcd_converthex(rtemp[3]); 
		rtc_me->hour   = bcd_converthex(rtemp[2]); 
		rtc_me->min    = bcd_converthex(rtemp[1]); 
		rtc_me->sec    = bcd_converthex(rtemp[0]); 
		rtc_me->week   = bcd_converthex(rtemp[4]); 
		//当星期0时，换成7
		if(rtc_me->week==0)
		{
			rtc_me->week=7;
		}
		return 0;
	}	  
	else return 1;
}

//功能描述:输入公历日期得到星期号(只允许1901-2099年)	
unsigned char pcf8563_rtc_week_get(rtc_st *const rtc_me)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=rtc_me->year/100;	yearL=rtc_me->year%100;   
	if (yearH>19)yearL+=100;// 如果为21世纪,年份数加100
	// 所过闰年数只算1900年之后的  
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
//	if (yearH>19)yearL+=100;// 如果为21世纪,年份数加100
//	// 所过闰年数只算1900年之后的  
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


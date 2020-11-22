
#include "tm1640.h"
#include "autom.h"
#include "ota.h"
#include "stdio.h"
#include "delay.h"

i2c_st tm1640_i2c=
{
	.trans_mode =1,//LSB开始传输
	.gpio_init  = tm1640_gpio_init,	//
	.sda_mode   = tm1640_sda_mode,
	.sda_in     = tm1640_sda_read,
	.sda_out    = tm1640_sda_out,
	.sck_out    = tm1640_sck_out,
  .delay_us   = delay_us,
  .delay_ms   = delay_ms,	
};
                                //----0-----1---2----3----4----5----6----7----8----9
const uint8_t seg_code_ck_unp[27] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
	              //--A---B----C----D----E----F--------NONE--'-'-     -'I'--'L'-'C'--'U'-- 'P'-'D'-'T'-
								  0x77,0x7C,0x39,0x5E,0x79,0x71,0xff,0x00,0x40,0x08,0x30,0x38,0x39,0x1C,0x73,0x5C,0x50};//共阴不带点

const uint8_t seg_code_ck_p[20] = {0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,
								  0x77,0x7C,0x39,0x5E,0x79,0x71,0xff,0x00,0x40,0x08};//共阴带点

	
//-----------------需要实现的对接IIC协议接口的函数-----------------
void tm1640_gpio_init(void)
{					     
 	GPIO_InitType GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(TM_PCC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
	
  PWR_BackupAccessCtrl(ENABLE);/* 允许修改RTC和后备寄存器*/
  RCC_LSEConfig(RCC_LSE_DISABLE); /* 关闭外部低速时钟,PC14+PC15可以用作普通IO*/
	
	GPIO_InitStructure.GPIO_Pins = TM_SCL_PIN|TM_SDA_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(TM_PORT, &GPIO_InitStructure);

  PWR_BackupAccessCtrl(DISABLE);/* 禁止修改RTC和后备寄存器*/	
}

void tm1640_sda_mode(unsigned char s)
{
	if(0!=s)
	{
		SDA3_OUT;
	}
	else
	{
		SDA3_IN;
	}
}

void tm1640_sda_out(unsigned char s)
{
	IIC_SDA3(s);
}

void tm1640_sck_out(unsigned char s)
{
	IIC_SCL3(s);
}

unsigned char tm1640_sda_read(void)
{
	return READ_SDA3;
}
//--------------------------------------------
void tm1640_writeregister(unsigned char reg_data)
{
	iic_start(&tm1640_i2c);
	iic_sendbyte(&tm1640_i2c,reg_data);//写寄存器数据
	iic_stop(&tm1640_i2c);	
}

void tm1640_writedata(unsigned char waddr,unsigned char wdata)
{
		iic_start(&tm1640_i2c);
	  iic_sendbyte(&tm1640_i2c,ADDR_CMD|waddr);
	  iic_sendbyte(&tm1640_i2c,wdata);
		iic_stop(&tm1640_i2c);	
}

void tm1640_writendatas(unsigned char waddr,unsigned char *wdata,unsigned char wdata_len)
{
	for(unsigned char i=0;i<wdata_len;i++)
	{
		iic_start(&tm1640_i2c);
		iic_sendbyte(&tm1640_i2c,ADDR_CMD|(waddr++));
		iic_sendbyte(&tm1640_i2c,seg_code_ck_unp[*wdata++]);
		iic_stop(&tm1640_i2c);
	} 	
}

void tm1640_cfg_init(void)
{
	tm1640_writeregister(DATA_CMD|0x04);//0x44固定地址模式；
	tm1640_writeregister(DIS_CMD|0x00);//关显示
}
void tm1640_init(void)
{
	iic_init(&tm1640_i2c);
	tm1640_cfg_init();
	tm1640_writedata(8,0x00);
	tm1640_writedata(9,0x00);	
	tm1640_writendatas(0,0x00,8);
	tm1640_writeregister(DIS_CMD|0x0F);//开显示，并且亮度最大	
	
}

void disboard_thread_entry(void *parameter)
{
	#define ANIMA_TIME (500/DIS_TASK_TIME)
	
	static dis_st temp_dis;
  static unsigned char anima_rtimer,i;
	
	g_ulAutoControl.dis.led_show[0]|=MAIN_BOARD_PWR_LED;//上电点亮电源指示LED
#if 0	
	static unsigned char reinit_timer;
	if(0!=tm1640_i2c.sda_in())//若SDA口处于高电平，超过预置时间
	{
		if(1000/DIS_TASK_TIME<reinit_timer++)//热重连机制
		{
			tm1640_init();//重新初始化
			reinit_timer=0;
		}
	}
	else
	{
		reinit_timer=0;
	}	
#endif
//------------------UPDATA ANIMATION GUI---------------------
	if(g_ulAutoControl.updata_flag)//升级状态下
	{
		#define UPDATA_RESULT_DIS_TIME  (3000/DIS_TASK_TIME) /*升级结果显示暂留时间 3s*/
		
		g_ulAutoControl.updata_timer++;
		if(UPDATA_ING==g_ulAutoControl.updata_status)//非传输状态下不需要判断
		{
			//主要预防中途信号传输中断/脱落,主动退出升级模式
			if(UPDATA_ERR_TIME<g_ulAutoControl.updata_timer)//计时器开始计时
			{
				printf(">> transmit has broken or timeout,please check cable or software...\r\n");
				g_ulAutoControl.updata_status=UPDATA_FAIL;//升级失败
			}	
	  }
		if(READY_UPDATA==g_ulAutoControl.updata_status)//准备状态
		{
			//----updata 闪烁---
			if(g_ulAutoControl.updata_timer/(500/DIS_TASK_TIME)%2)
			{
				memset(&g_ulAutoControl.dis.seg_show[1],0x11,6);//灭 "updata"
			}
			else
			{			
				g_ulAutoControl.dis.seg_show[0] = 0x12;//'-'	
				g_ulAutoControl.dis.seg_show[1] = 0x17;//'U'
				g_ulAutoControl.dis.seg_show[2] = 0x18;//'P'
				g_ulAutoControl.dis.seg_show[3] = 0x19;//'D'
				g_ulAutoControl.dis.seg_show[4] = 0x0A;//'A'
				g_ulAutoControl.dis.seg_show[5] = 0x1A;//'T'
				g_ulAutoControl.dis.seg_show[6] = 0x0A;//'A'	
				g_ulAutoControl.dis.seg_show[7] = 0x12;//'-'
			}				
		}		
		else if(UPDATA_ING==g_ulAutoControl.updata_status)//升级中
		{
			//升级进度动画 xx:升级进度完成率
			//-     xx
			//--    xx
			//---   xx
			//----  xx
			//----- xx
			//------xx
			 anima_rtimer++;
			 if(5<=i)
			 {
				 i=0;  
				 anima_rtimer=0;
				 memset(&g_ulAutoControl.dis.seg_show[0],0x11,6);
			 }
			 else
			 {
				 g_ulAutoControl.dis.seg_show[0] = 0x12;//'-'			
				 if(0==anima_rtimer%ANIMA_TIME)
				 {
					 g_ulAutoControl.dis.seg_show[i+1] = g_ulAutoControl.dis.seg_show[i];
					 i++;			 
				 }
			 }			 
	  }
		else if(UPDATA_OK==g_ulAutoControl.updata_status)//升级成功
		{
			//----Succ 闪烁---
			if(g_ulAutoControl.updata_timer/(500/DIS_TASK_TIME)%2)
			{
				memset(&g_ulAutoControl.dis.seg_show[2],0x11,4);//灭 "Succ"
			}
			else
			{
				g_ulAutoControl.dis.seg_show[0] = 0x12;//'-'	
				g_ulAutoControl.dis.seg_show[1] = 0x12;//'-'
				g_ulAutoControl.dis.seg_show[2] = 0x05;//'S'
				g_ulAutoControl.dis.seg_show[3] = 0x17;//'u'
				g_ulAutoControl.dis.seg_show[4] = 0x16;//'c'
				g_ulAutoControl.dis.seg_show[5] = 0x16;//'c'
				g_ulAutoControl.dis.seg_show[6] = 0x12;//'-'	
				g_ulAutoControl.dis.seg_show[7] = 0x12;//'-'				
			}
			//---END---
			if(UPDATA_RESULT_DIS_TIME<g_ulAutoControl.updata_timer)//
			{
				g_ulAutoControl.warn_active(OFF);
				g_ulAutoControl.updata_flag=0;
				NVIC_SystemReset();//收完数据进行重启	
			}		
		}
		else if(UPDATA_FAIL==g_ulAutoControl.updata_status)//升级失败
		{
			//----FAIL 闪烁---
			if(g_ulAutoControl.updata_timer/(500/DIS_TASK_TIME)%2)
			{
				memset(&g_ulAutoControl.dis.seg_show[2],0x11,4);//灭 "FAIL"
			}
			else
			{
				g_ulAutoControl.dis.seg_show[0] = 0x12;//'-'	
				g_ulAutoControl.dis.seg_show[1] = 0x12;//'-'
				g_ulAutoControl.dis.seg_show[2] = 0x0F;//'F'
				g_ulAutoControl.dis.seg_show[3] = 0x0A;//'A'
				g_ulAutoControl.dis.seg_show[4] = 0x14;//'I'
				g_ulAutoControl.dis.seg_show[5] = 0x15;//'L'
				g_ulAutoControl.dis.seg_show[6] = 0x12;//'-'	
				g_ulAutoControl.dis.seg_show[7] = 0x12;//'-'			
			}
			//---END---
			if(UPDATA_ERR_TIME<g_ulAutoControl.updata_timer)//因传输中断/超时引起的升级失败
			{
				if(UPDATA_ERR_TIME+UPDATA_RESULT_DIS_TIME<g_ulAutoControl.updata_timer)//
				{
					g_ulAutoControl.warn_active(OFF);
					g_ulAutoControl.updata_flag=0;
				}
			}
			else//非上原因失败
			{
				if(UPDATA_RESULT_DIS_TIME<g_ulAutoControl.updata_timer)
				{
					g_ulAutoControl.warn_active(OFF);
					g_ulAutoControl.updata_flag=0;
				}					
			}
		}		
	}
	else//非升级状态
	{
		g_ulAutoControl.updata_timer=0;
	}
//---------------------END----------------------	
	if(0!=memcmp(&g_ulAutoControl.dis.seg_show[0],&temp_dis.seg_show[0],8))
	{
		memcpy(&temp_dis.seg_show[0],&g_ulAutoControl.dis.seg_show[0],8);
		tm1640_writendatas(GRAD1,temp_dis.seg_show,8);
	}
  else if(g_ulAutoControl.dis.led_show[0]!=temp_dis.led_show[0])
	{
		temp_dis.led_show[0]=g_ulAutoControl.dis.led_show[0];
    tm1640_writedata(GRAD9,temp_dis.led_show[0]);	
	}
	else if(g_ulAutoControl.dis.led_show[1]!=temp_dis.led_show[1])
	{
		temp_dis.led_show[1]=g_ulAutoControl.dis.led_show[1];
		tm1640_writedata(GRAD10,temp_dis.led_show[1]); 		
	}
}



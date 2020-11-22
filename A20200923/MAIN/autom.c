

#include "autom.h"
#include "task.h"

/*
//theme/device/air/525533061467
//测试语句
{
  "product" : "AE360",
  "mac" : "525533061467",
 "payload": "010300020001",
 "oper": "",
 "batch": ""
}
*/
automtype  g_ulAutoControl;
fsm_st autocontrol_fsm;

//-------------------------------------------------------
void watertank_fullscale_read(void)//水箱满量程读取
{
  at24c_read_byaddr(&at24c02,NV_TANK_FULLSCALE_HB_ADDR,AT24X_FLASH_BUF,2);
	
	g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale|=((unsigned short int)AT24X_FLASH_BUF[NV_TANK_FULLSCALE_HB_ADDR]<<8);
	g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale|=AT24X_FLASH_BUF[NV_TANK_FULLSCALE_LB_ADDR];

	g_ulAutoControl.watertank.sensor_watlevel.rang.percent95_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.95;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent85_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.85;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent75_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.75;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent65_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.65;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent55_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.55;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent45_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.45;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent35_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.35;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent25_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.25;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent15_scale=(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.15;
	g_ulAutoControl.watertank.sensor_watlevel.rang.percent5_scale =(float)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale*0.5;	
	return ;
}

//-------------------------------------------------------
void hotpump_workstatus_read(void)//热泵启用状态判断读取
{
	#define OFFLINE_CRMS_VAL  (200) /*互感器没有接入的阈值*/
	unsigned short int checkvalue = 0;		
	checkvalue = g_ulAutoControl.hotpump.workcurrent_th_val;//工作门限电流阈值	    
	if(1==(g_ulCS5460UpdataFlag&0x01))//CS5460存在
	{
		if(cs5460a.amp_rms<OFFLINE_CRMS_VAL)
		{
			g_ulAutoControl.warning_status|=(1<<10);//互感器没有接入
		}
		else
		{
			g_ulAutoControl.warning_status&=~(1<<10);			
		}
		if((cs5460a.amp_rms/100) >= checkvalue)//高于工作门限电流
		{
      g_ulAutoControl.hotpump.run_status=1;
		}
		else if((cs5460a.amp_rms/100) < checkvalue)//低于门限电流
		{
		  g_ulAutoControl.hotpump.run_status=0;
		}
	}
	else
	{
		  g_ulAutoControl.hotpump.run_status=0;
	}
	return ;	
}

//----时间段控制----
void worktimer_execute(rtc_st *const rtc_me,period_st *const work_period)
{
	static unsigned char switch_status;
	#ifdef DEBUG_MODE
		switch(rtc_me->week)
		{
			case 1:
				printf("今日星期一\r\n");
				break;
			case 2:
				printf("今日星期二\r\n");
				break;	
			case 3:
				printf("今日星期三\r\n");
				break;
			case 4:
				printf("今日星期四\r\n");
				break;	
			case 5:
				printf("今日星期五\r\n");
				break;
			case 6:
				printf("今日星期六\r\n");
				break;		
			case 7:
				printf("今日星期日\r\n");
				break;	
			default:	
				printf("Week Error!\r\n");		
			 break;			
		}	
		printf("系统时间：%02d:%02d\r\n",rtc_me->hour,rtc_me->min);	
		printf("预设开启时间：%02d:%02d\r\n",work_period->open_time.hour,work_period->open_time.min);			
		printf("预设关闭时间：%02d:%02d\r\n",work_period->close_time.hour,work_period->close_time.min);					
	#endif		
	if(rtc_me->hour>=work_period->open_time.hour&&rtc_me->hour<=work_period->close_time.hour)
	{
		//系统小时值 在 预设开小时和预设关小时(不包含)内
		if(rtc_me->hour>=work_period->open_time.hour&&rtc_me->hour<work_period->close_time.hour)
		{
			//1.系统小时 大于预设开小时值
			if(rtc_me->hour>work_period->open_time.hour)
			{
				if(0==switch_status)
				{
					switch_status=1;
				  g_ulAutoControl.warn_active(ON);//报警
				  g_ulAutoControl.wcb.power(ON);//开机
				}
				#ifdef DEBUG_MODE
				 printf("1.OPEN WCB !\r\n");	
				#endif									
			}
			//系统小时 等于预设开小时值
			else if(rtc_me->hour==work_period->open_time.hour)
			{
				//系统分钟 大于 预设开分钟
				if(rtc_me->min>work_period->open_time.min)
				{
					if(0==switch_status)
				  {
					  switch_status=1;
					  g_ulAutoControl.warn_active(ON);//报警
					  g_ulAutoControl.wcb.power(ON);//开机
					  #ifdef DEBUG_MODE
					   printf("2.OPEN WCB !\r\n");	
					  #endif
					}						
				}
				else
				{
					switch_status=0;
					g_ulAutoControl.wcb.power(OFF);//关机		
					#ifdef DEBUG_MODE
					printf("2.CLOSE WCB !\r\n");	
					#endif										
				}	
			}
			else
			{
				switch_status=0;
				g_ulAutoControl.wcb.power(OFF);//关机		
				#ifdef DEBUG_MODE
				 printf("1.CLOSE WCB !\r\n");	
				#endif										
			}
		}
		//系统小时值等于预设关小时
		if(rtc_me->hour==work_period->close_time.hour)
		{
			//系统分钟大于预设开分钟值(在预设开和预设关小时值一样的情况下) 且系统分钟小于预设关分钟
			if(rtc_me->min>=work_period->open_time.min&&rtc_me->min<work_period->close_time.min)
			{
				if(0==switch_status)
				{
					switch_status=1;
					g_ulAutoControl.warn_active(ON);//报警
					g_ulAutoControl.wcb.power(ON);//开机
					#ifdef DEBUG_MODE
					printf("3.OPEN WCB !\r\n");	
					#endif
				}
			}
			else
			{
				switch_status=0;
				g_ulAutoControl.wcb.power(OFF);//关机		
				#ifdef DEBUG_MODE
				 printf("3.CLOSE WCB !\r\n");	
				#endif										
			}								
		}
	}
	else
	{
		switch_status=0;
		g_ulAutoControl.wcb.power(OFF);//关机	
		#ifdef DEBUG_MODE
		printf("4.CLOSE WCB !\r\n");	
		#endif								
	}			
}

//--------空气热水泵自动控制-----------------
void AirHotWaterpump_AutoContrl(fsm_st *const Aucontrol_fsm_me)
{
	#define FSM_TIME_INV            (60*60000/AUTOM_TASK_TIME) /*状态机计时器翻转周期 1hour*/
	#define RTC_TIME_READ_INV       (60000/AUTOM_TASK_TIME) /*1min读一次*/
	#define ERR_STATUS_TIME_INV     (500/AUTOM_TASK_TIME) /*500Ms一次*/
	
	extern void wcb_powersupply(unsigned char);
	extern void watertank_fullscale_read(void);
	extern void hotpump_workstatus_read(void);
	
	switch(Aucontrol_fsm_me->status)
	{
		case SM0://初始化
		  g_ulAutoControl.rtc_set=pcf8563_rtc_set;	
			g_ulAutoControl.wcb.power=wcb_powersupply;
		  g_ulAutoControl.warn_active=buzzer_loudly;	
			Aucontrol_fsm_me->status=SM1;
			break;
		case SM1:
			if(0!=g_ulAutoControl.warning_status)
			{
				//定时鸣叫，防止一直处于阻塞状态，以便让出CPU权限,给予其他任务使用
			  if(0==Aucontrol_fsm_me->rtime%(100/AUTOM_TASK_TIME))
				{
					//g_ulAutoControl.warn_active(ON);//报警
				}
        if(g_ulAutoControl.warning_status&(1<<10))//互感器没有挂载，热泵工作状态LED闪烁
				{
					  if(Aucontrol_fsm_me->rtime/ERR_STATUS_TIME_INV%2)//500MS
						{
						  g_ulAutoControl.dis.led_show[0]|= HOTPUMP_STATUS_LED;
						}
						else 
						{
						  g_ulAutoControl.dis.led_show[0]&= ~HOTPUMP_STATUS_LED;
						}
				}
				else//已成功挂载，热泵工作状态LED常亮
				{
					 g_ulAutoControl.dis.led_show[0]|= HOTPUMP_STATUS_LED;					
        }	
			}
			else
			{
				g_ulAutoControl.warn_active(OFF);//解除报警					
			}
			//----------------------水箱水位校准-----------------------
			if(g_ulAutoControl.watertank.sensor_watlevel.calib_cmd)//收到校准命令
			{
			  //获取当前量程值作为满量程值
			  g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale=g_ulAutoControl.watertank.sensor_watlevel.rang.cur_scale;
         //校准后的量程值，要存储在FLASH/EEPROM中
				enter_critical();//进入临界段,数据读写保护
				at24c_writebyte(&at24c02,NV_TANK_FULLSCALE_HB_ADDR,(u8)(g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale>>8));
				at24c_writebyte(&at24c02,NV_TANK_FULLSCALE_LB_ADDR,(u8)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale&0xFF);
				watertank_fullscale_read();//水箱满量程数据初始读取
				exit_critical(); //退出临界段
				g_ulAutoControl.watertank.sensor_watlevel.calib_cmd=0;
			}
			//水箱水温报警判断
			if(g_ulAutoControl.watertank.sensor_temper.online)//水箱温度传感器已挂载
			{
				g_ulAutoControl.warning_status&=~(1<<0);	
				if(g_ulAutoControl.watertank.sensor_temper.dw_th_val>=g_ulAutoControl.watertank.sensor_temper.cur_val)
				{
				    g_ulAutoControl.warning_status|=(1<<4);		
				}
				else
				{
				    g_ulAutoControl.warning_status&=~(1<<4);				
				}
	  	}
			//水箱水位报警判断
			//水箱水位上限阈值低于下限阈值,数据写入有误
			if(g_ulAutoControl.watertank.sensor_watlevel.online)//水位传感器已挂载
			{
				g_ulAutoControl.warning_status&=~(1<<2);	
				if(g_ulAutoControl.watertank.sensor_watlevel.up_th_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
				{
					g_ulAutoControl.watertank.sensor_watlevel.data_err=1;//数据有误
					g_ulAutoControl.warning_status|=(1<<8);
				}
				else
				{
					g_ulAutoControl.watertank.sensor_watlevel.data_err=0;//清除标志
					g_ulAutoControl.warning_status&=~(1<<8);
					//水箱水温高于上限阈值或者低于下限阈值
					if(g_ulAutoControl.watertank.sensor_watlevel.up_th_val<=g_ulAutoControl.watertank.sensor_watlevel.cur_val||\
						g_ulAutoControl.watertank.sensor_watlevel.cur_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
					{
						//1.水箱水位低于下限阈值
						if(g_ulAutoControl.watertank.sensor_watlevel.cur_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
						{
							g_ulAutoControl.warning_status|=(1<<6);
						}
						else
						{
							g_ulAutoControl.warning_status&=~(1<<6);	
						}
						//2.水箱水位高于上限阈值
						if(g_ulAutoControl.watertank.sensor_watlevel.up_th_val<=g_ulAutoControl.watertank.sensor_watlevel.cur_val)
						{
							g_ulAutoControl.warning_status|=(1<<7);							
						}
						else
						{
							g_ulAutoControl.warning_status&=~(1<<7);								
						}										
					}
				}
		  }
      //---------------------热泵状态检测---------------------------
			hotpump_workstatus_read();//热泵工作状态实时读取			
		  if(g_ulAutoControl.hotpump.run_status)//热泵开启的条件下
			{
				//1.热泵水温低于下限阈值
				if(g_ulAutoControl.hotpump.sensor_temper.online)
				{
					g_ulAutoControl.warning_status&=~(1<<1);	
					if(g_ulAutoControl.hotpump.sensor_temper.dw_th_val>g_ulAutoControl.hotpump.sensor_temper.cur_val)
					{						
				    g_ulAutoControl.warning_status|=(1<<5);		
					}
					else
					{
				    g_ulAutoControl.warning_status&=~(1<<5);							
					}
			  }
				//2.热泵水压低于下限阈值 报警	
        if(g_ulAutoControl.hotpump.sensor_watpress.online)//水压传感器已挂载
				{					
				  g_ulAutoControl.warning_status&=~(1<<3);						
				 	if(g_ulAutoControl.hotpump.sensor_watpress.dw_th_val>g_ulAutoControl.hotpump.sensor_watpress.cur_val)
					{						
				    g_ulAutoControl.warning_status|=(1<<9);		
					}
					else
					{
				    g_ulAutoControl.warning_status&=~(1<<9);						
					}
			  }
		  }
			//-----------------------线控器电源定时时间段控制--------------------------
			if(0==Aucontrol_fsm_me->rtime%RTC_TIME_READ_INV)//一分钟扫描一次时间
			{				
				worktimer_execute(&g_ulAutoControl.sys_rtc,&g_ulAutoControl.wcb.period[g_ulAutoControl.sys_rtc.week-1]);					
			}
	    Aucontrol_fsm_me->rtime=(Aucontrol_fsm_me->rtime+1)%FSM_TIME_INV;
      break;			
			case SM2:
				break;
			default:
				break;						
	 }
}
//--------------------------------------------------
void autom_thread_entry(void *parameter)
{
	AirHotWaterpump_AutoContrl(&autocontrol_fsm);
}











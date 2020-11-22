
#include "rs485.h"

modbus_st   ulModbus;
ppotocol_st ulPpotocol;

unsigned char USART_EventFlag =0;		//保存标志

#ifdef UPDATA
unsigned short int HOLD_REG_BUFF[HOLD_REG_NUM];//
unsigned short int INPUT_REG_BUFF[INPUT_REG_NUM];//

/*	
	for(unsigned short int i=0;i<ulRegNum;i++)				//读取输入寄存器数据【04】
	{
		modbus_me->data.buff[3+2*i] =INPUT_REG_BUFF[ulSartReg+i]>>8;
		modbus_me->data.buff[4+2*i] =INPUT_REG_BUFF[ulSartReg+i];
	}
	
	for(unsigned short int i=0;i<ulRegNum;i++)				//读取保持寄存器的数据【03】
	{
		modbus_me->data.buff[3+2*i] =HOLD_REG_BUFF[ulSartReg+i]>>8;
		modbus_me->data.buff[4+2*i] =HOLD_REG_BUFF[ulSartReg+i];
	}	
*/
#endif
//--------------------------------------------------------
unsigned char modbus_read_data(modbus_st *const modbus_me)
{
	unsigned short int ulSartReg = 0;//寄存器起始地址
	unsigned short int ulRegNum = 0;//寄存器数目
	
	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
  ulRegNum   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;
  
	//暂时不做04码功能，统一用03码来读取
	for(unsigned short int i=0;i<ulRegNum;i++)				//读取输入寄存器数据【04】
	{
	//******************************只读*******************************************
		if((ulSartReg+i) == OR_REG_HOTPUMP_TEMPER)				//热泵温度
		{
//			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_temper.cur_val)>>8;//高位为0
//			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_temper.cur_val);	
		}	
		else if((ulSartReg+i) == OR_REG_HOTPUMP_WATPRESS)	//热泵水压
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.cur_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.cur_val);	
		}							
		else if((ulSartReg+i) == OR_REG_HOTPUMP_WORKSTA)//热泵启停状态
		{
			modbus_me->data.buff[3+2*i] = 0;
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.hotpump.run_status;	
		}
		else if((ulSartReg+i) == OR_REG_TANK_TEMPER)		//水箱温度		
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_temper.cur_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_temper.cur_val);	
		}
		else if((ulSartReg+i) == OR_REG_TANK_WATLEV)			//水箱水位	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val);	
		}
		else if(ulSartReg == OR_REG_TANK_FULLSCALE)				//水箱水位满量程
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val);			
		}				
		else if((ulSartReg+i) == OR_METER_RMS_CURR_ADDR)//计量实时测量电流
		{
			modbus_me->data.buff[3+2*i] = ((cs5460a.amp_rms/100)>>8)&0xff;//高位为0
			modbus_me->data.buff[4+2*i] = (cs5460a.amp_rms/100)&0xff;//电流		ma
		}					
		else if((ulSartReg+i) == OR_REG_SOFT_VER)				//软件版本
		{
			modbus_me->data.buff[3+2*i] = (CURR_SOFT_VER>>8)&0xff;
			modbus_me->data.buff[4+2*i] = CURR_SOFT_VER&0xff;
		}	
		else if((ulSartReg+i) == OR_REG_WARN_INFO)				//报警信息
		{
			modbus_me->data.buff[3+2*i] = g_ulAutoControl.warning_status>>8;
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.warning_status;
		}	
		else	//补0	
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = 0;		
		}			
	}			
	//***************************可读写************************************
	for(unsigned short int i=0;i<ulRegNum;i++)	//读取保持寄存器的数据【03】
	{			
		if((ulSartReg+i) == RW_REG_HOTPUMP_WORKCURR_TH)				//热泵启停阈值
		{
			modbus_me->data.buff[3+2*i] = g_ulAutoControl.hotpump.workcurrent_th_val>>8;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.hotpump.workcurrent_th_val;	
		}
		else if((ulSartReg+i) == RW_REG_TANK_UPWATLEV_TH)		//水箱水位上限阈值
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.up_th_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.up_th_val);		
		}		
		else if((ulSartReg+i) == RW_REG_TANK_DWWATLEV_TH)			//水箱水温下限阈值	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.dw_th_val);	
		}		
		else if((ulSartReg+i) == RW_REG_TANK_TEMPER_TH)			//水箱水温下限阈值	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_temper.dw_th_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_temper.dw_th_val);	
		}				
		else if((ulSartReg+i) == RW_REG_HOTPUMP_WATPRESS_TH)			//热泵水压下限阈值
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.dw_th_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.dw_th_val);	
		}	
		else if((ulSartReg+i) == RW_REG_HOTPUMP_TEMPER_TH)			//热泵水温下限阈值	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_temper.dw_th_val)>>8;//高位为0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_temper.dw_th_val);	
		}		
		else if((ulSartReg+i) == RW_REG_RS485_ADDR)				//设备地址	255
		{		 
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.rs485.device_addr;				
		}
		else if((ulSartReg+i) == RW_REG_RS485_BAUD)				//波特率
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.rs485.device_baud;				
		}
		else if((ulSartReg+i) == RW_REG_SYSRTC_YEAR)				//系统时间年
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.sys_rtc.year>>8)&0xFF;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.year&0xFF;		
		}	
		else if((ulSartReg+i) == RW_REG_SYSRTC_MONTH)				//系统时间月
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.mon;				
		}		
		else if((ulSartReg+i) == RW_REG_SYSRTC_DAY)				//系统时间日
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.day;		
		}				
		else if((ulSartReg+i) == RW_REG_SYSRTC_HOUR)				//系统时间时
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.hour;				
		}
		else if((ulSartReg+i) == RW_REG_SYSRTC_MIN)				//系统时间分
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.min;		
		}	
		else if((ulSartReg+i) == RW_REG_SYSRTC_SEC)				//系统时间秒
		{
			modbus_me->data.buff[3+2*i] = 0;//高位为0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.sec;		
		}			
	}
  return 0;	//成功
}


unsigned char modbus_write_data(modbus_st *const modbus_me)
{
	unsigned char ulErrCode=0;//错误码
	unsigned short int ulSartReg = 0;//寄存器起始地址
	unsigned short int ulRegValue = 0;//寄存器值

	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
  ulRegValue   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;	
	
	switch(ulSartReg)//数据寄存器配置点
	{
		case RW_REG_TANK_UPWATLEV_TH://水箱水位上限
			g_ulAutoControl.watertank.sensor_watlevel.up_th_val = ulRegValue;
			at24c_writebyte(&at24c02,NV_TANK_UPWATLEV_TH_ADDR,g_ulAutoControl.watertank.sensor_watlevel.up_th_val);			
			break;
		case RW_REG_TANK_DWWATLEV_TH://水箱水位下限
			g_ulAutoControl.watertank.sensor_watlevel.dw_th_val= ulRegValue;
		  at24c_writebyte(&at24c02,NV_TANK_DWWATLEV_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);
			break;		
		case RW_REG_TANK_WATLEV_CALIB://水箱水位校准
		  g_ulAutoControl.watertank.sensor_watlevel.calib_cmd= ulRegValue&0x01;
//		at24c_writebyte(NV_TANK_DWTEMPER_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);			
			break;
		case RW_REG_TANK_TEMPER_TH://水箱水温下限阈值
		  g_ulAutoControl.watertank.sensor_temper.dw_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WATPRESS_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);			
			break;		
		case RW_REG_HOTPUMP_WATPRESS_TH://热泵水压阈值
			g_ulAutoControl.hotpump.sensor_watpress.dw_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WATPRESS_TH_ADDR,g_ulAutoControl.hotpump.sensor_watpress.dw_th_val);		
			break;
		case RW_REG_HOTPUMP_WORKCURR_TH://热泵启停阈值
		  g_ulAutoControl.hotpump.workcurrent_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WORKCURR_TH_ADDR,g_ulAutoControl.hotpump.workcurrent_th_val);			
			break;		
		case RW_REG_HOTPUMP_TEMPER_TH://热泵水温下限阈值
			g_ulAutoControl.hotpump.sensor_temper.dw_th_val= ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_TEMPER_TH_ADDR,g_ulAutoControl.hotpump.sensor_temper.dw_th_val);		
			break;
		case RW_WCB_MON_HOUR_ON_ADDR://线控器周一开启小时
			if(ulRegValue<24)
			{		
				g_ulAutoControl.wcb.period[0].open_time.hour= ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_MON_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[0].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;
		case RW_WCB_MON_MIN_ON_ADDR://线控器周一开启分钟
			if(ulRegValue<60)
			{		
				g_ulAutoControl.wcb.period[0].open_time.min = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_MON_MIN_ON_ADDR,g_ulAutoControl.wcb.period[0].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_MON_HOUR_OFF_ADDR://线控器周一关闭小时
			if(ulRegValue<24)
			{				
				g_ulAutoControl.wcb.period[0].close_time.hour= ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_MON_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[0].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_MON_MIN_OFF_ADDR://线控器周一关闭分钟
			if(ulRegValue<60)
			{				
				g_ulAutoControl.wcb.period[0].close_time.min = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_MON_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[0].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_TUS_HOUR_ON_ADDR://线控器周二开启小时
			if(ulRegValue<24)
			{					
				g_ulAutoControl.wcb.period[1].open_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_TUS_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[1].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_TUS_MIN_ON_ADDR://线控器周二开启分钟
			if(ulRegValue<60)
			{				
				g_ulAutoControl.wcb.period[1].open_time.min = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_TUS_MIN_ON_ADDR,g_ulAutoControl.wcb.period[1].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_TUS_HOUR_OFF_ADDR://线控器周二关闭小时
			if(ulRegValue<24)
			{				
				g_ulAutoControl.wcb.period[1].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_TUS_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[1].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_TUS_MIN_OFF_ADDR://线控器周二关闭分钟
			if(ulRegValue<60)
			{		
			  g_ulAutoControl.wcb.period[1].close_time.min = ulRegValue;
			  at24c_writebyte(&at24c02,NV_WCB_TUS_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[1].close_time.min);
			}
			else
			{
			  ulErrCode = 0x03;				
			}
			break;
		case RW_WCB_WED_HOUR_ON_ADDR://线控器周三开启小时
			if(ulRegValue<24)
			{			
				g_ulAutoControl.wcb.period[2].open_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_WED_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[2].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;	
		case RW_WCB_WED_MIN_ON_ADDR://线控器周三开启分钟
			if(ulRegValue<60)
			{				
			g_ulAutoControl.wcb.period[2].open_time.min = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_WED_MIN_ON_ADDR,g_ulAutoControl.wcb.period[2].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_WED_HOUR_OFF_ADDR://线控器周三关闭小时
			if(ulRegValue<24)
			{			
				g_ulAutoControl.wcb.period[2].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_WED_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[2].close_time.hour);
			}
			else
			{
				ulErrCode= 0x03;			
			}			
			break;		
		case RW_WCB_WED_MIN_OFF_ADDR://线控器周三关闭分钟
			if(ulRegValue<60)
			{			
			  g_ulAutoControl.wcb.period[1].close_time.min = ulRegValue;
			  at24c_writebyte(&at24c02,NV_WCB_WED_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[1].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_THR_HOUR_ON_ADDR://线控器周四开启小时
			if(ulRegValue<24)
			{		
				g_ulAutoControl.wcb.period[3].open_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_THR_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[3].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}				
			break;		
		case RW_WCB_THR_MIN_ON_ADDR:	//线控器周四开启分钟
			if(ulRegValue<60)
			{			
			g_ulAutoControl.wcb.period[3].open_time.min = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_THR_MIN_ON_ADDR,g_ulAutoControl.wcb.period[3].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_THR_HOUR_OFF_ADDR://线控器周四关闭小时
			if(ulRegValue<24)
			{				
				g_ulAutoControl.wcb.period[3].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_THR_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[3].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_THR_MIN_OFF_ADDR://线控器周四关闭分钟
			if(ulRegValue<60)
			{			
			g_ulAutoControl.wcb.period[3].close_time.min = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_THR_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[3].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_FRI_HOUR_ON_ADDR:	//线控器周五开启小时
			if(ulRegValue<24)
			{				
				g_ulAutoControl.wcb.period[4].open_time.hour= ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_FRI_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[4].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;	
		case RW_WCB_FRI_MIN_ON_ADDR://线控器周五开启分钟
			if(ulRegValue<60)
			{			
			g_ulAutoControl.wcb.period[4].open_time.min = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_FRI_MIN_ON_ADDR,g_ulAutoControl.wcb.period[4].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;					
			}			
			break;
		case RW_WCB_FRI_HOUR_OFF_ADDR:	//线控器周五关闭小时
			if(ulRegValue<24)
			{			
				g_ulAutoControl.wcb.period[4].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_FRI_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[4].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;	
		case RW_WCB_FRI_MIN_OFF_ADDR://线控器周五关闭分钟
			if(ulRegValue<60)
			{		
			  g_ulAutoControl.wcb.period[4].close_time.min = ulRegValue;
			  at24c_writebyte(&at24c02,NV_WCB_FRI_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[4].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_SAT_HOUR_ON_ADDR://线控器周六开启小时
			if(ulRegValue<24)
			{					
				g_ulAutoControl.wcb.period[5].open_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_SAT_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[5].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_SAT_MIN_ON_ADDR://线控器周六开启分钟
			if(ulRegValue<60)
			{			
			  g_ulAutoControl.wcb.period[5].open_time.min = ulRegValue;
			  at24c_writebyte(&at24c02,NV_WCB_SAT_MIN_ON_ADDR,g_ulAutoControl.wcb.period[5].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_WCB_STA_HOUR_OFF_ADDR://线控器周六关闭小时
			if(ulRegValue<24)
			{				
				g_ulAutoControl.wcb.period[5].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_STA_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[5].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_STA_MIN_OFF_ADDR://线控器周六关闭分钟
			if(ulRegValue<60)
			{			
			g_ulAutoControl.wcb.period[5].close_time.min = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_STA_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[5].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;					
			}			
			break;
		case RW_WCB_SUN_HOUR_ON_ADDR:	//线控器周日开启小时
			if(ulRegValue<24)
			{			
				g_ulAutoControl.wcb.period[6].open_time.hour= ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_SUN_HOUR_ON_ADDR,g_ulAutoControl.wcb.period[6].open_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;		
		case RW_WCB_SUN_MIN_ON_ADDR://线控器周日开启分钟
			if(ulRegValue<60)
			{				
			g_ulAutoControl.wcb.period[6].open_time.min  = ulRegValue;
			at24c_writebyte(&at24c02,NV_WCB_SUN_MIN_ON_ADDR,g_ulAutoControl.wcb.period[6].open_time.min);
			}
			else
			{
				ulErrCode = 0x03;					
			}			
			break;
		case RW_WCB_SUN_HOUR_OFF_ADDR:	//线控器周日关闭小时
			if(ulRegValue<24)
			{			
				g_ulAutoControl.wcb.period[6].close_time.hour = ulRegValue;
				at24c_writebyte(&at24c02,NV_WCB_SUN_HOUR_OFF_ADDR,g_ulAutoControl.wcb.period[6].close_time.hour);
			}
			else
			{
				ulErrCode = 0x03;			
			}			
			break;
		case RW_WCB_SUN_MIN_OFF_ADDR://线控器周日关闭分钟
			if(ulRegValue<60)
			{				
			  g_ulAutoControl.wcb.period[6].close_time.min = ulRegValue;
			  at24c_writebyte(&at24c02,NV_WCB_SUN_MIN_OFF_ADDR,g_ulAutoControl.wcb.period[6].close_time.min);
			}
			else
			{
				ulErrCode = 0x03;				
			}			
			break;
		case RW_REG_RS485_ADDR://设备地址
			if((ulRegValue>=1)&&(ulRegValue<=255))
			{
				g_ulAutoControl.rs485.device_addr  = ulRegValue;
				at24c_writebyte(&at24c02,NV_RS485_ADDR_ADDR,g_ulAutoControl.rs485.device_addr);						
			}
			else
			{
				ulErrCode = 0x03;				
			}				
			break;	
		case RW_REG_RS485_BAUD://波特率
			if(((ulRegValue > 0)&&(ulRegValue<10)))
			{
				g_ulAutoControl.rs485.device_baud  = ulRegValue;	
				at24c_writebyte(&at24c02,NV_RS485_BAUD_ADDR,g_ulAutoControl.rs485.device_baud);		
				UartCheckAndSet(g_ulAutoControl.rs485.device_baud);		//重启串口
			}
			else
			{
				ulErrCode= 0x03;
			}			
			break;
		case RW_REG_SYSRTC_YEAR:	//系统时间年
			if(ulRegValue>1990&&ulRegValue<2099)//只允许从19年开始设置
			{
				g_ulAutoControl.sys_rtc.year =ulRegValue;
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);				
			}
			else
			{
				ulErrCode = 0x03;
			}				
			break;		
		case RW_REG_SYSRTC_MONTH:	//系统时间月
			if((ulRegValue>=1)&&(ulRegValue<=12))
			{
				g_ulAutoControl.sys_rtc.mon = ulRegValue;					
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);							
			}else
			{
				ulErrCode = 0x03;
			}			
			break;
		case RW_REG_SYSRTC_DAY://系统时间日
			if((ulRegValue>=1)&&(ulRegValue<=31))
			{
				g_ulAutoControl.sys_rtc.day= ulRegValue;					
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);					
			}else
			{
				ulErrCode = 0x03;
			}			
			break;		
		case RW_REG_SYSRTC_HOUR://系统时间时
			if(ulRegValue<24)
			{
				g_ulAutoControl.sys_rtc.hour = ulRegValue;						
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);				
			}else
			{
				ulErrCode = 0x03;
			}					
			break;
		case RW_REG_SYSRTC_MIN://系统时间分
			if(ulRegValue<60)
			{
				g_ulAutoControl.sys_rtc.min = ulRegValue;						
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);						
			}else
			{
				ulErrCode= 0x03;
			}				
			break;		
		case RW_REG_SYSRTC_SEC://系统时间秒
			if(ulRegValue<60)
			{
				g_ulAutoControl.sys_rtc.sec= ulRegValue;					
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);					
			}else
			{
				ulErrCode = 0x03;
			}			
			break;
//    ..........			
//		case xxx:
//			break;				
		default:
			break;	
	}
	return ulErrCode;
}

//读保持寄存器
void modbus_f03_handler(modbus_st *const modbus_me)
{
	//发送帧：设备地址+功能码+起始地址+寄存器数目+CRC
	//响应帧：设备地址+功能码+字节数+寄存器值+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败
	unsigned short int ulByteSize = 0;
	unsigned char ulErrCode=0;
	unsigned short int ulCrc = 0;
	unsigned short int ulSartReg = 0;
	unsigned short int ulRegNum = 0;

	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;//读取的寄存器起始地址(0~0xFFFF)
  ulRegNum   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;//读取的寄存器数目(1~0x007D)
	
	ulByteSize=(1<<ulRegNum);
	
	if(0x01>ulRegNum||ulRegNum>0x7D)//只能连续读取125个寄存器
	{
		ulErrCode=3;//寄存器数目超限，失败/错去 返回异常码03
	}
	else
	{
		if((((ulSartReg+ulRegNum-1)>0) && ((ulSartReg+ulRegNum-1)<51))||(((ulSartReg+ulRegNum-1)>300) && ((ulSartReg+ulRegNum-1)<500)))//检查寄存器有效范围
		{
			ulErrCode=modbus_read_data(modbus_me);
		}
		else
		{
			ulErrCode=2;
		}
  }
//------接收响应---	
	enter_critical();//临界区 读写保护	
	if(0!=ulErrCode)//错误响应发送帧
	{
		modbus_me->data.buff[1] |= 0x80;
		modbus_me->data.buff[2]  = ulErrCode;
		ulCrc =  UartCRCCheck(&modbus_me->data.buff[0],3);
		modbus_me->data.buff[3] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[4] = (unsigned char)(ulCrc >> 8);
		modbus_me->transmit(&modbus_me->data.buff[0],5,modbus_me->enport);
	  ulErrCode=0;
	}
	else  //正常响应发送帧
	{		
		ulByteSize = 5+ulByteSize;	//数据长度
		modbus_me->data.buff[2] = ulRegNum*2;	
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],(ulByteSize-2));//计算校验位值		
		modbus_me->data.buff[ulByteSize-2] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[ulByteSize-1] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],ulByteSize,modbus_me->enport);
	}		
	exit_critical();				
}

//单写保持寄存器
void modbus_f06_handler(modbus_st *const modbus_me)
{
	//注意：接收和发送数据一致
	//发送帧：设备地址+功能码+起始地址+寄存器值+CRC
	//响应帧：设备地址+功能码+起始地址+寄存器值+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败
	
	unsigned char ulErrCode=0;//错误码
	unsigned short int ulSartReg = 0;//寄存器起始地址
  unsigned short int ulCrc = 0;	
	
	//------发送读取----	
	//要写的寄存器的起始地址
	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;

	if(((ulSartReg-1)>300) && ((ulSartReg-1)<500))//检查寄存器有效范围
	{
		ulErrCode=modbus_write_data(modbus_me);
	}
	else
	{
		ulErrCode=0x02;
	}
  //------接收响应---		
	enter_critical();//临界区 读写保护	
	if(0!=ulErrCode)
	{
		modbus_me->data.buff[1] |= 0x80;
		modbus_me->data.buff[2] = ulErrCode;
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],3);
		modbus_me->data.buff[3] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[4] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],5,modbus_me->enport);
		ulErrCode= 0;
	}
	else
	{
		modbus_me->transmit(&modbus_me->data.buff[0],8,modbus_me->enport);
	}	
	exit_critical();	
}

//多写保持寄存器
void modbus_f16_handler(modbus_st *const modbus_me)
{
	//发送帧：设备地址+功能码+起始地址+寄存器数量+字节数+寄存器值+CRC
	//响应帧：设备地址+功能码+起始地址+寄存器数量+CRC
	//失败响应帧：设备地址+功能码|0x80 + 异常码(01/02/03/04)+CRC 
	//01：功能码错误或不支持
	//02：寄存器地址范围无效
	//03：数量超限
	//04：读取失败
	
	unsigned char ulErrCode=0;
	unsigned short int ulCrc = 0;
	unsigned short int ulSartReg = 0;
	unsigned short int ulRegNum = 0;
	
	//要写的寄存器起始地址
	ulSartReg  = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
	//要写的寄存器数目
	ulRegNum   = ((unsigned short int)modbus_me->data.buff[4]<<8)|modbus_me->data.buff[5];
	//要写的字节数

	if(((ulSartReg+ulRegNum-1)>300) && ((ulSartReg+ulRegNum-1)<500))//检查寄存器范围
	{
		for(unsigned char i=0;i<ulRegNum;i++)//寄存器数目
		{
			modbus_me->data.frame.ulDataH=modbus_me->data.buff[7+(1<<i)];
	    modbus_me->data.frame.ulDataL=modbus_me->data.buff[8+(1<<i)];//要写得寄存器值
			ulErrCode=modbus_write_data(modbus_me);	//data[7+2*uli] data[8+2*uli]
		}		
	}
	else
	{		
		ulErrCode=0x02;
	}	
	enter_critical();//临界区 读写保护	
	if(0!=ulErrCode)
	{
		ulErrCode = 0;
		modbus_me->data.buff[1] |= 0x80;
		modbus_me->data.buff[2] = ulErrCode;
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],3);
		modbus_me->data.buff[3] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[4] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],5,modbus_me->enport);
	}
	else
	{
	  modbus_me->data.frame.ulDataH=modbus_me->data.buff[4];
	  modbus_me->data.frame.ulDataL=modbus_me->data.buff[5];//寄存器数目
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],6);
		modbus_me->data.buff[6] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[7] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],8,modbus_me->enport);
	}	
	exit_critical();
}	


//---MODBUS485协议数据处理
void modbus_handler(modbus_st *const modbus_me,unsigned char *data_buff,unsigned char data_len,unsigned char dev_addr)
{

	unsigned short int ulCrcTemp=0 ;
	unsigned short int ulCrc=0 ;
	
	memset(&modbus_me->data.buff[0],0x00,DATA_MAX_SIZE);
	memcpy(&modbus_me->data.buff[0],data_buff,data_len);
  //检测有效地址
	if(dev_addr==modbus_me->data.frame.ulAddr||BORADCAST_ADDR==modbus_me->data.frame.ulAddr)
	{
		ulCrcTemp =((unsigned short int)modbus_me->data.buff[data_len-1]<<8);//接收的校验值
	  ulCrcTemp|=modbus_me->data.buff[data_len-2];
		
	  ulCrc =modbus_me->check(&modbus_me->data.buff[0],data_len-2);//计算校验位值
		
		if(ulCrcTemp==ulCrc)//校验通过
		{
			switch(modbus_me->data.frame.ulFunc)
			{
				case 0x03://读保持寄存器
					modbus_f03_handler(modbus_me);
					break;
				case 0x06://单写保持寄存器
					modbus_f06_handler(modbus_me);
					break;	
				case 0x10://多写保持寄存器
					modbus_f16_handler(modbus_me);
					break;	
				default:
					break;		
			}			
		}
		else	
		{
			return;//校验不通过
		}
	}	
	else
	{
		return;
	}
}

//---以太网协处理MCU通信MAC配置写入
void mqtt_module_maccfg_handler(unsigned char *data_buff,unsigned char data_len)
{
	unsigned short int ulCrc = 0;
	unsigned short int ulCrcTemp = 0;	
	//协议帧：AA BB +(MAC字符串转HEX值)+CRC校验
	
	ulCrcTemp  = ((unsigned short int)data_buff[data_len-1]<<8)|data_buff[data_len-2];
	ulCrc =UartCRCCheck(&data_buff[0],data_len -2);//计算校验位值		
	
	if(ulCrcTemp==ulCrc)
	{
		if(0xAA==data_buff[0]&&0xBB==data_buff[1])
		{
			enter_critical();
			usart2_transimit(data_buff,data_len,NULL);
			g_ulAutoControl.warn_active(ON);						
			exit_critical();
			return;			
		}		
	}
	else
	{
		return;
	}
}
//---私有协议写数据
void privatepotocol_write_data(ppotocol_st *const ulPpotocol_me,unsigned short int data_len)
{
	unsigned char ulErrCode=0;
	unsigned short int ulSartReg = 0;
	unsigned short int ulCrc=0;
	
	ulSartReg = ((unsigned short int)ulPpotocol_me->data.frame.ulRegH<<8)|ulPpotocol_me->data.frame.ulRegL;
	
	switch(ulSartReg)
	{
		case RW_METER_CALIB_ADDR:
					memcpy(&AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR],&ulPpotocol_me->data.buff[5],15);
					enter_critical();
					at24c_write_byaddr(&at24c02,NV_METER_CURR_BIAS_CALIB_ADDR,(unsigned char*)&AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR],15);
					exit_critical();
					cs5460a_reboot();//电表采集初始化			
			break;
		default:
			ulErrCode=1;
			break;
	}
	if(1==ulErrCode)//错误帧
	{
		ulErrCode = 0;
		ulPpotocol_me->data.buff[1]|= 0x80;
		ulCrc =  ulPpotocol_me->check(&ulPpotocol_me->data.buff[1],(data_len-4));
		ulPpotocol_me->data.buff[data_len-3] = (unsigned char)(ulCrc);		
		ulPpotocol_me->data.buff[data_len-2] = (unsigned char)(ulCrc >> 8);										
	}
	ulPpotocol_me->transmit(&ulPpotocol_me->data.buff[0],data_len,ulPpotocol_me->enport);								
}

//---私有协议处理
void privatepotocol_handler(ppotocol_st *const ulPpotocol_me,unsigned char *data_buff,unsigned char data_len,unsigned char dev_addr)
{
	unsigned short int ulCrcTemp=0 ;
	unsigned short int ulCrc=0 ;
	
	memset(&ulPpotocol_me->data.buff[0],0x00,DATA_MAX_SIZE);
	memcpy(&ulPpotocol_me->data.buff[0],data_buff,data_len);

	if(0x22==ulPpotocol_me->data.frame.ulHeader)		//起始位
	{
		if((dev_addr==ulPpotocol_me->data.frame.ulAddr)||(BORADCAST_ADDR==ulPpotocol_me->data.frame.ulAddr))		//addr
		{
			if(0x03==ulPpotocol_me->data.buff[data_len-1])//结束位
			{
				ulCrc =  ulPpotocol_me->check(&data_buff[1],(data_len-4));
				ulCrcTemp  = ulPpotocol_me->data.buff[data_len-2];
				ulCrcTemp  <<= 8;
				ulCrcTemp  |= ulPpotocol_me->data.buff[data_len-3];				
				if(ulCrc == ulCrcTemp)
				{
					privatepotocol_write_data(ulPpotocol_me,data_len);
					return;
				}
			}
		}
	}		
}

//---串口1处理
void uart1_thread_entry(void *parameter)//RS485
{
	if(((g_ulUserEventFlag&UASRT_EVENT_Flag_Usart1) == UASRT_EVENT_Flag_Usart1))
	{
			g_ulUserEventFlag &= ~UASRT_EVENT_Flag_Usart1;
			//1.
			mqtt_module_maccfg_handler(UART1_RBuf,UART1_RLen);//通过RS485接口接收，然后转发另一个串口
			//2.
			//3.
			ota_handler(&ota,UART1_RBuf,UART1_RLen);
			//4.
			ulModbus.enport=enable_uart1_port;//指定使能端口函数
			ulModbus.transmit=usart1_transimit;//指定发送端口函数 
			ulModbus.check=UartCRCCheck;
			modbus_handler(&ulModbus,UART1_RBuf,UART1_RLen,g_ulAutoControl.rs485.device_addr);		
//	modbus_host_recev_handler(&mb_host,UART1_RBuf,UART1_RLen);
		UART1_RLen = 0;
	}
}

//---串口2处理
void uart2_thread_entry(void *parameter)//ETH/GPRS
{
	if(((g_ulUserEventFlag&UASRT_EVENT_Flag_Usart2) == UASRT_EVENT_Flag_Usart2))
	{
		//g_ulAutoControl.warn_active(ON);
		g_ulUserEventFlag &= ~UASRT_EVENT_Flag_Usart2;		
    //1.
		ota_handler(&ota,UART2_RBuf,UART2_RLen);
		//2.
		ulModbus.enport=NULL;//指定使能端口函数
		ulModbus.transmit=usart2_transimit;//指定发送端口函数 
		ulModbus.check=UartCRCCheck;
		modbus_handler(&ulModbus,UART2_RBuf,UART2_RLen,g_ulAutoControl.rs485.device_addr);							
		//3.		
		ulPpotocol.enport=NULL;
		ulPpotocol.transmit=usart2_transimit;
		privatepotocol_handler(&ulPpotocol,UART2_RBuf,UART2_RLen,g_ulAutoControl.rs485.device_addr);			
		UART2_RLen = 0;	
	}
}


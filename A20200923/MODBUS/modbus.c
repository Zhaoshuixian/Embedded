

#include "modbus.h"


/*

//--------------------------------------------------------
unsigned char modbus_read_data(modbus_st *const modbus_me)
{
	unsigned short int ulSartReg = 0;//�Ĵ�����ʼ��ַ
	unsigned short int ulRegNum = 0;//�Ĵ�����Ŀ
	
	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
  ulRegNum   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;
  
	//��ʱ����04�빦�ܣ�ͳһ��03������ȡ
	for(unsigned short int i=0;i<ulRegNum;i++)				//��ȡ����Ĵ������ݡ�04��
	{
	//******************************ֻ��*******************************************
		if((ulSartReg+i) == OR_REG_HOTPUMP_TEMPER)				//�ȱ��¶�
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_temper.cur_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_temper.cur_val);	
		}	
		else if((ulSartReg+i) == OR_REG_HOTPUMP_WATPRESS)	//�ȱ�ˮѹ
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.cur_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.cur_val);	
		}							
		else if((ulSartReg+i) == OR_REG_HOTPUMP_WORKSTA)//�ȱ���ͣ״̬
		{
			modbus_me->data.buff[3+2*i] = 0;
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.hotpump.run_status;	
		}
		else if((ulSartReg+i) == OR_REG_TANK_TEMPER)		//ˮ���¶�		
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_temper.cur_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_temper.cur_val);	
		}
		else if((ulSartReg+i) == OR_REG_TANK_WATLEV)			//ˮ��ˮλ	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val);	
		}
		else if(ulSartReg == OR_REG_TANK_FULLSCALE)				//ˮ��ˮλ������
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.cur_val);			
		}				
		else if((ulSartReg+i) == OR_METER_RMS_CURR_ADDR)//����ʵʱ��������
		{
			modbus_me->data.buff[3+2*i] = ((cs5460a.amp_rms/100)>>8)&0xff;//��λΪ0
			modbus_me->data.buff[4+2*i] = (cs5460a.amp_rms/100)&0xff;//����		ma
		}					
		else if((ulSartReg+i) == OR_REG_SOFT_VER)				//����汾
		{
			modbus_me->data.buff[3+2*i] = (CURR_SOFT_VER>>8)&0xff;
			modbus_me->data.buff[4+2*i] = CURR_SOFT_VER&0xff;
		}	
		else if((ulSartReg+i) == OR_REG_WARN_INFO)				//������Ϣ
		{
			modbus_me->data.buff[3+2*i] = g_ulAutoControl.warning_status>>8;
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.warning_status;
		}	
		else	//��0	
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = 0;		
		}			
	}			
	//***************************�ɶ�д************************************
	for(unsigned short int i=0;i<ulRegNum;i++)	//��ȡ���ּĴ��������ݡ�03��
	{			
		if((ulSartReg+i) == RW_REG_HOTPUMP_WORKCURR_TH)				//�ȱ���ͣ��ֵ
		{
			modbus_me->data.buff[3+2*i] = g_ulAutoControl.hotpump.workcurrent_th_val>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.hotpump.workcurrent_th_val;	
		}
		else if((ulSartReg+i) == RW_REG_TANK_UPWATLEV_TH)		//ˮ��ˮλ������ֵ
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.up_th_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.up_th_val);		
		}		
		else if((ulSartReg+i) == RW_REG_TANK_DWWATLEV_TH)			//ˮ��ˮ��������ֵ	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_watlevel.dw_th_val);	
		}		
		else if((ulSartReg+i) == RW_REG_TANK_TEMPER_TH)			//ˮ��ˮ��������ֵ	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.watertank.sensor_temper.dw_th_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.watertank.sensor_temper.dw_th_val);	
		}				
		else if((ulSartReg+i) == RW_REG_HOTPUMP_WATPRESS_TH)			//�ȱ�ˮѹ������ֵ
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.dw_th_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_watpress.dw_th_val);	
		}	
		else if((ulSartReg+i) == RW_REG_HOTPUMP_TEMPER_TH)			//�ȱ�ˮ��������ֵ	
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.hotpump.sensor_temper.dw_th_val)>>8;//��λΪ0
			modbus_me->data.buff[4+2*i] = (g_ulAutoControl.hotpump.sensor_temper.dw_th_val);	
		}		
		else if((ulSartReg+i) == RW_REG_RS485_ADDR)				//�豸��ַ	255
		{		 
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.rs485.device_addr;				
		}
		else if((ulSartReg+i) == RW_REG_RS485_BAUD)				//������
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.rs485.device_baud;				
		}
		else if((ulSartReg+i) == RW_REG_SYSRTC_YEAR)				//ϵͳʱ����
		{
			modbus_me->data.buff[3+2*i] = (g_ulAutoControl.sys_rtc.year>>8)&0xFF;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.year&0xFF;		
		}	
		else if((ulSartReg+i) == RW_REG_SYSRTC_MONTH)				//ϵͳʱ����
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.mon;				
		}		
		else if((ulSartReg+i) == RW_REG_SYSRTC_DAY)				//ϵͳʱ����
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.day;		
		}				
		else if((ulSartReg+i) == RW_REG_SYSRTC_HOUR)				//ϵͳʱ��ʱ
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.hour;				
		}
		else if((ulSartReg+i) == RW_REG_SYSRTC_MIN)				//ϵͳʱ���
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.min;		
		}	
		else if((ulSartReg+i) == RW_REG_SYSRTC_SEC)				//ϵͳʱ����
		{
			modbus_me->data.buff[3+2*i] = 0;//��λΪ0
			modbus_me->data.buff[4+2*i] = g_ulAutoControl.sys_rtc.sec;		
		}			
	}
  return 0;	//�ɹ�
}


unsigned char modbus_write_data(modbus_st *const modbus_me)
{
	unsigned char ulErrCode=0;//������
	unsigned short int ulSartReg = 0;//�Ĵ�����ʼ��ַ
	unsigned short int ulRegValue = 0;//�Ĵ���ֵ

	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
  ulRegValue   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;	
	
	switch(ulSartReg)//���ݼĴ������õ�
	{
		case RW_REG_TANK_UPWATLEV_TH://ˮ��ˮλ����
			g_ulAutoControl.watertank.sensor_watlevel.up_th_val = ulRegValue;
			at24c_writebyte(&at24c02,NV_TANK_UPWATLEV_TH_ADDR,g_ulAutoControl.watertank.sensor_watlevel.up_th_val);			
			break;
		case RW_REG_TANK_DWWATLEV_TH://ˮ��ˮλ����
			g_ulAutoControl.watertank.sensor_watlevel.dw_th_val= ulRegValue;
		  at24c_writebyte(&at24c02,NV_TANK_DWWATLEV_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);
			break;		
		case RW_REG_TANK_WATLEV_CALIB://ˮ��ˮλУ׼
		  g_ulAutoControl.watertank.sensor_watlevel.calib_cmd= ulRegValue&0x01;
//		at24c_writebyte(NV_TANK_DWTEMPER_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);			
			break;
		case RW_REG_TANK_TEMPER_TH://ˮ��ˮ��������ֵ
		  g_ulAutoControl.watertank.sensor_temper.dw_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WATPRESS_TH_ADDR,g_ulAutoControl.watertank.sensor_temper.dw_th_val);			
			break;		
		case RW_REG_HOTPUMP_WATPRESS_TH://�ȱ�ˮѹ��ֵ
			g_ulAutoControl.hotpump.sensor_watpress.dw_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WATPRESS_TH_ADDR,g_ulAutoControl.hotpump.sensor_watpress.dw_th_val);		
			break;
		case RW_REG_HOTPUMP_WORKCURR_TH://�ȱ���ͣ��ֵ
		  g_ulAutoControl.hotpump.workcurrent_th_val = ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_WORKCURR_TH_ADDR,g_ulAutoControl.hotpump.workcurrent_th_val);			
			break;		
		case RW_REG_HOTPUMP_TEMPER_TH://�ȱ�ˮ��������ֵ
			g_ulAutoControl.hotpump.sensor_temper.dw_th_val= ulRegValue;
		  at24c_writebyte(&at24c02,NV_HOTPUMP_TEMPER_TH_ADDR,g_ulAutoControl.hotpump.sensor_temper.dw_th_val);		
			break;
		case RW_WCB_MON_HOUR_ON_ADDR://�߿�����һ����Сʱ
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
		case RW_WCB_MON_MIN_ON_ADDR://�߿�����һ��������
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
		case RW_WCB_MON_HOUR_OFF_ADDR://�߿�����һ�ر�Сʱ
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
		case RW_WCB_MON_MIN_OFF_ADDR://�߿�����һ�رշ���
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
		case RW_WCB_TUS_HOUR_ON_ADDR://�߿����ܶ�����Сʱ
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
		case RW_WCB_TUS_MIN_ON_ADDR://�߿����ܶ���������
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
		case RW_WCB_TUS_HOUR_OFF_ADDR://�߿����ܶ��ر�Сʱ
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
		case RW_WCB_TUS_MIN_OFF_ADDR://�߿����ܶ��رշ���
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
		case RW_WCB_WED_HOUR_ON_ADDR://�߿�����������Сʱ
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
		case RW_WCB_WED_MIN_ON_ADDR://�߿���������������
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
		case RW_WCB_WED_HOUR_OFF_ADDR://�߿��������ر�Сʱ
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
		case RW_WCB_WED_MIN_OFF_ADDR://�߿��������رշ���
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
		case RW_WCB_THR_HOUR_ON_ADDR://�߿������Ŀ���Сʱ
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
		case RW_WCB_THR_MIN_ON_ADDR:	//�߿������Ŀ�������
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
		case RW_WCB_THR_HOUR_OFF_ADDR://�߿������Ĺر�Сʱ
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
		case RW_WCB_THR_MIN_OFF_ADDR://�߿������Ĺرշ���
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
		case RW_WCB_FRI_HOUR_ON_ADDR:	//�߿������忪��Сʱ
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
		case RW_WCB_FRI_MIN_ON_ADDR://�߿������忪������
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
		case RW_WCB_FRI_HOUR_OFF_ADDR:	//�߿�������ر�Сʱ
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
		case RW_WCB_FRI_MIN_OFF_ADDR://�߿�������رշ���
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
		case RW_WCB_SAT_HOUR_ON_ADDR://�߿�����������Сʱ
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
		case RW_WCB_SAT_MIN_ON_ADDR://�߿���������������
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
		case RW_WCB_STA_HOUR_OFF_ADDR://�߿��������ر�Сʱ
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
		case RW_WCB_STA_MIN_OFF_ADDR://�߿��������رշ���
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
		case RW_WCB_SUN_HOUR_ON_ADDR:	//�߿������տ���Сʱ
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
		case RW_WCB_SUN_MIN_ON_ADDR://�߿������տ�������
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
		case RW_WCB_SUN_HOUR_OFF_ADDR:	//�߿������չر�Сʱ
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
		case RW_WCB_SUN_MIN_OFF_ADDR://�߿������չرշ���
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
		case RW_REG_RS485_ADDR://�豸��ַ
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
		case RW_REG_RS485_BAUD://������
			if(((ulRegValue > 0)&&(ulRegValue<10)))
			{
				g_ulAutoControl.rs485.device_baud  = ulRegValue;	
				at24c_writebyte(&at24c02,NV_RS485_BAUD_ADDR,g_ulAutoControl.rs485.device_baud);		
				UartCheckAndSet(g_ulAutoControl.rs485.device_baud);		//��������
			}
			else
			{
				ulErrCode= 0x03;
			}			
			break;
		case RW_REG_SYSRTC_YEAR:	//ϵͳʱ����
			if(ulRegValue>1990&&ulRegValue<2099)//ֻ�����19�꿪ʼ����
			{
				g_ulAutoControl.sys_rtc.year =ulRegValue;
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);				
			}
			else
			{
				ulErrCode = 0x03;
			}				
			break;		
		case RW_REG_SYSRTC_MONTH:	//ϵͳʱ����
			if((ulRegValue>=1)&&(ulRegValue<=12))
			{
				g_ulAutoControl.sys_rtc.mon = ulRegValue;					
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);							
			}else
			{
				ulErrCode = 0x03;
			}			
			break;
		case RW_REG_SYSRTC_DAY://ϵͳʱ����
			if((ulRegValue>=1)&&(ulRegValue<=31))
			{
				g_ulAutoControl.sys_rtc.day= ulRegValue;					
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);					
			}else
			{
				ulErrCode = 0x03;
			}			
			break;		
		case RW_REG_SYSRTC_HOUR://ϵͳʱ��ʱ
			if(ulRegValue<24)
			{
				g_ulAutoControl.sys_rtc.hour = ulRegValue;						
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);				
			}else
			{
				ulErrCode = 0x03;
			}					
			break;
		case RW_REG_SYSRTC_MIN://ϵͳʱ���
			if(ulRegValue<60)
			{
				g_ulAutoControl.sys_rtc.min = ulRegValue;						
				g_ulAutoControl.rtc_set(&g_ulAutoControl.sys_rtc);						
			}else
			{
				ulErrCode= 0x03;
			}				
			break;		
		case RW_REG_SYSRTC_SEC://ϵͳʱ����
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

//�����ּĴ���
void modbus_f03_handler(modbus_st *const modbus_me)
{
	//����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�����Ŀ+CRC
	//��Ӧ֡���豸��ַ+������+�ֽ���+�Ĵ���ֵ+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��
	unsigned short int ulByteSize = 0;
	unsigned char ulErrCode=0;
	unsigned short int ulCrc = 0;
	unsigned short int ulSartReg = 0;
	unsigned short int ulRegNum = 0;

	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;//��ȡ�ļĴ�����ʼ��ַ(0~0xFFFF)
  ulRegNum   = ((unsigned short int)modbus_me->data.frame.ulDataH<<8)|modbus_me->data.frame.ulDataL;//��ȡ�ļĴ�����Ŀ(1~0x007D)
	
	ulByteSize=(1<<ulRegNum);
	
	if(0x01>ulRegNum||ulRegNum>0x7D)//ֻ��������ȡ125���Ĵ���
	{
		ulErrCode=3;//�Ĵ�����Ŀ���ޣ�ʧ��/��ȥ �����쳣��03
	}
	else
	{
		if((((ulSartReg+ulRegNum-1)>0) && ((ulSartReg+ulRegNum-1)<51))||(((ulSartReg+ulRegNum-1)>300) && ((ulSartReg+ulRegNum-1)<500)))//���Ĵ�����Ч��Χ
		{
			ulErrCode=modbus_read_data(modbus_me);
		}
		else
		{
			ulErrCode=2;
		}
  }
//------������Ӧ---	
	enter_critical();//�ٽ��� ��д����	
	if(0!=ulErrCode)//������Ӧ����֡
	{
		modbus_me->data.buff[1] |= 0x80;
		modbus_me->data.buff[2]  = ulErrCode;
		ulCrc =  UartCRCCheck(&modbus_me->data.buff[0],3);
		modbus_me->data.buff[3] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[4] = (unsigned char)(ulCrc >> 8);
		modbus_me->transmit(&modbus_me->data.buff[0],5,modbus_me->enport);
	  ulErrCode=0;
	}
	else  //������Ӧ����֡
	{		
		ulByteSize = 5+ulByteSize;	//���ݳ���
		modbus_me->data.buff[2] = ulRegNum*2;	
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],(ulByteSize-2));//����У��λֵ		
		modbus_me->data.buff[ulByteSize-2] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[ulByteSize-1] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],ulByteSize,modbus_me->enport);
	}		
	exit_critical();				
}

//��д���ּĴ���
void modbus_f06_handler(modbus_st *const modbus_me)
{
	//ע�⣺���պͷ�������һ��
	//����֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
	//��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ���ֵ+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��
	
	unsigned char ulErrCode=0;//������
	unsigned short int ulSartReg = 0;//�Ĵ�����ʼ��ַ
  unsigned short int ulCrc = 0;	
	
	//------���Ͷ�ȡ----	
	//Ҫд�ļĴ�������ʼ��ַ
	ulSartReg   = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;

	if(((ulSartReg-1)>300) && ((ulSartReg-1)<500))//���Ĵ�����Ч��Χ
	{
		ulErrCode=modbus_write_data(modbus_me);
	}
	else
	{
		ulErrCode=0x02;
	}
  //------������Ӧ---		
	enter_critical();//�ٽ��� ��д����	
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

//��д���ּĴ���
void modbus_f16_handler(modbus_st *const modbus_me)
{
	//����֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+�ֽ���+�Ĵ���ֵ+CRC
	//��Ӧ֡���豸��ַ+������+��ʼ��ַ+�Ĵ�������+CRC
	//ʧ����Ӧ֡���豸��ַ+������|0x80 + �쳣��(01/02/03/04)+CRC 
	//01������������֧��
	//02���Ĵ�����ַ��Χ��Ч
	//03����������
	//04����ȡʧ��
	
	unsigned char ulErrCode=0;
	unsigned short int ulCrc = 0;
	unsigned short int ulSartReg = 0;
	unsigned short int ulRegNum = 0;
	
	//Ҫд�ļĴ�����ʼ��ַ
	ulSartReg  = ((unsigned short int)modbus_me->data.frame.ulRegH<<8)|modbus_me->data.frame.ulRegL;
	//Ҫд�ļĴ�����Ŀ
	ulRegNum   = ((unsigned short int)modbus_me->data.buff[4]<<8)|modbus_me->data.buff[5];
	//Ҫд���ֽ���

	if(((ulSartReg+ulRegNum-1)>300) && ((ulSartReg+ulRegNum-1)<500))//���Ĵ�����Χ
	{
		for(unsigned char i=0;i<ulRegNum;i++)//�Ĵ�����Ŀ
		{
			modbus_me->data.frame.ulDataH=modbus_me->data.buff[7+(1<<i)];
	    modbus_me->data.frame.ulDataL=modbus_me->data.buff[8+(1<<i)];//Ҫд�üĴ���ֵ
			ulErrCode=modbus_write_data(modbus_me);	//data[7+2*uli] data[8+2*uli]
		}		
	}
	else
	{		
		ulErrCode=0x02;
	}	
	enter_critical();//�ٽ��� ��д����	
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
	  modbus_me->data.frame.ulDataL=modbus_me->data.buff[5];//�Ĵ�����Ŀ
		ulCrc =  modbus_me->check(&modbus_me->data.buff[0],6);
		modbus_me->data.buff[6] = (unsigned char)(ulCrc);		
		modbus_me->data.buff[7] = (unsigned char)(ulCrc >> 8);	
		modbus_me->transmit(&modbus_me->data.buff[0],8,modbus_me->enport);
	}	
	exit_critical();
}	


//---MODBUS485Э�����ݴ���
void modbus_handler(modbus_st *const modbus_me,uart_st *const uart_me,unsigned char dev_addr)
{

	unsigned short int ulCrcTemp=0 ;
	unsigned short int ulCrc=0 ;
	
	memset(&modbus_me->data.buff[0],0x00,DATA_MAX_SIZE);
	memcpy(&modbus_me->data.buff[0],uart_me->frame.data,uart_me->frame.data_len);
  //�����Ч��ַ
	if(dev_addr==modbus_me->data.frame.ulAddr||BORADCAST_ADDR==modbus_me->data.frame.ulAddr)
	{
		ulCrcTemp =((unsigned short int)modbus_me->data.buff[uart_me->frame.data_len-1]<<8);//���յ�У��ֵ
	  ulCrcTemp|=modbus_me->data.buff[uart_me->frame.data_len-2];
		
	  ulCrc =modbus_me->check(&modbus_me->data.buff[0],uart_me->frame.data_len-2);//����У��λֵ
		
		if(ulCrcTemp==ulCrc)//У��ͨ��
		{
			switch(modbus_me->data.frame.ulFunc)
			{
				case 0x03://�����ּĴ���
					modbus_f03_handler(modbus_me);
					break;
				case 0x06://��д���ּĴ���
					modbus_f06_handler(modbus_me);
					break;	
				case 0x10://��д���ּĴ���
					modbus_f16_handler(modbus_me);
					break;	
				default:
					break;		
			}			
		}
		else	
		{
			return;//У�鲻ͨ��
		}
	}	
	else
	{
		return;
	}
}

*/



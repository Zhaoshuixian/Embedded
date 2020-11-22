

#include "autom.h"
#include "task.h"

/*
//theme/device/air/525533061467
//�������
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
void watertank_fullscale_read(void)//ˮ�������̶�ȡ
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
void hotpump_workstatus_read(void)//�ȱ�����״̬�ж϶�ȡ
{
	#define OFFLINE_CRMS_VAL  (200) /*������û�н������ֵ*/
	unsigned short int checkvalue = 0;		
	checkvalue = g_ulAutoControl.hotpump.workcurrent_th_val;//�������޵�����ֵ	    
	if(1==(g_ulCS5460UpdataFlag&0x01))//CS5460����
	{
		if(cs5460a.amp_rms<OFFLINE_CRMS_VAL)
		{
			g_ulAutoControl.warning_status|=(1<<10);//������û�н���
		}
		else
		{
			g_ulAutoControl.warning_status&=~(1<<10);			
		}
		if((cs5460a.amp_rms/100) >= checkvalue)//���ڹ������޵���
		{
      g_ulAutoControl.hotpump.run_status=1;
		}
		else if((cs5460a.amp_rms/100) < checkvalue)//�������޵���
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

//----ʱ��ο���----
void worktimer_execute(rtc_st *const rtc_me,period_st *const work_period)
{
	static unsigned char switch_status;
	#ifdef DEBUG_MODE
		switch(rtc_me->week)
		{
			case 1:
				printf("��������һ\r\n");
				break;
			case 2:
				printf("�������ڶ�\r\n");
				break;	
			case 3:
				printf("����������\r\n");
				break;
			case 4:
				printf("����������\r\n");
				break;	
			case 5:
				printf("����������\r\n");
				break;
			case 6:
				printf("����������\r\n");
				break;		
			case 7:
				printf("����������\r\n");
				break;	
			default:	
				printf("Week Error!\r\n");		
			 break;			
		}	
		printf("ϵͳʱ�䣺%02d:%02d\r\n",rtc_me->hour,rtc_me->min);	
		printf("Ԥ�迪��ʱ�䣺%02d:%02d\r\n",work_period->open_time.hour,work_period->open_time.min);			
		printf("Ԥ��ر�ʱ�䣺%02d:%02d\r\n",work_period->close_time.hour,work_period->close_time.min);					
	#endif		
	if(rtc_me->hour>=work_period->open_time.hour&&rtc_me->hour<=work_period->close_time.hour)
	{
		//ϵͳСʱֵ �� Ԥ�迪Сʱ��Ԥ���Сʱ(������)��
		if(rtc_me->hour>=work_period->open_time.hour&&rtc_me->hour<work_period->close_time.hour)
		{
			//1.ϵͳСʱ ����Ԥ�迪Сʱֵ
			if(rtc_me->hour>work_period->open_time.hour)
			{
				if(0==switch_status)
				{
					switch_status=1;
				  g_ulAutoControl.warn_active(ON);//����
				  g_ulAutoControl.wcb.power(ON);//����
				}
				#ifdef DEBUG_MODE
				 printf("1.OPEN WCB !\r\n");	
				#endif									
			}
			//ϵͳСʱ ����Ԥ�迪Сʱֵ
			else if(rtc_me->hour==work_period->open_time.hour)
			{
				//ϵͳ���� ���� Ԥ�迪����
				if(rtc_me->min>work_period->open_time.min)
				{
					if(0==switch_status)
				  {
					  switch_status=1;
					  g_ulAutoControl.warn_active(ON);//����
					  g_ulAutoControl.wcb.power(ON);//����
					  #ifdef DEBUG_MODE
					   printf("2.OPEN WCB !\r\n");	
					  #endif
					}						
				}
				else
				{
					switch_status=0;
					g_ulAutoControl.wcb.power(OFF);//�ػ�		
					#ifdef DEBUG_MODE
					printf("2.CLOSE WCB !\r\n");	
					#endif										
				}	
			}
			else
			{
				switch_status=0;
				g_ulAutoControl.wcb.power(OFF);//�ػ�		
				#ifdef DEBUG_MODE
				 printf("1.CLOSE WCB !\r\n");	
				#endif										
			}
		}
		//ϵͳСʱֵ����Ԥ���Сʱ
		if(rtc_me->hour==work_period->close_time.hour)
		{
			//ϵͳ���Ӵ���Ԥ�迪����ֵ(��Ԥ�迪��Ԥ���Сʱֵһ���������) ��ϵͳ����С��Ԥ��ط���
			if(rtc_me->min>=work_period->open_time.min&&rtc_me->min<work_period->close_time.min)
			{
				if(0==switch_status)
				{
					switch_status=1;
					g_ulAutoControl.warn_active(ON);//����
					g_ulAutoControl.wcb.power(ON);//����
					#ifdef DEBUG_MODE
					printf("3.OPEN WCB !\r\n");	
					#endif
				}
			}
			else
			{
				switch_status=0;
				g_ulAutoControl.wcb.power(OFF);//�ػ�		
				#ifdef DEBUG_MODE
				 printf("3.CLOSE WCB !\r\n");	
				#endif										
			}								
		}
	}
	else
	{
		switch_status=0;
		g_ulAutoControl.wcb.power(OFF);//�ػ�	
		#ifdef DEBUG_MODE
		printf("4.CLOSE WCB !\r\n");	
		#endif								
	}			
}

//--------������ˮ���Զ�����-----------------
void AirHotWaterpump_AutoContrl(fsm_st *const Aucontrol_fsm_me)
{
	#define FSM_TIME_INV            (60*60000/AUTOM_TASK_TIME) /*״̬����ʱ����ת���� 1hour*/
	#define RTC_TIME_READ_INV       (60000/AUTOM_TASK_TIME) /*1min��һ��*/
	#define ERR_STATUS_TIME_INV     (500/AUTOM_TASK_TIME) /*500Msһ��*/
	
	extern void wcb_powersupply(unsigned char);
	extern void watertank_fullscale_read(void);
	extern void hotpump_workstatus_read(void);
	
	switch(Aucontrol_fsm_me->status)
	{
		case SM0://��ʼ��
		  g_ulAutoControl.rtc_set=pcf8563_rtc_set;	
			g_ulAutoControl.wcb.power=wcb_powersupply;
		  g_ulAutoControl.warn_active=buzzer_loudly;	
			Aucontrol_fsm_me->status=SM1;
			break;
		case SM1:
			if(0!=g_ulAutoControl.warning_status)
			{
				//��ʱ���У���ֹһֱ��������״̬���Ա��ó�CPUȨ��,������������ʹ��
			  if(0==Aucontrol_fsm_me->rtime%(100/AUTOM_TASK_TIME))
				{
					//g_ulAutoControl.warn_active(ON);//����
				}
        if(g_ulAutoControl.warning_status&(1<<10))//������û�й��أ��ȱù���״̬LED��˸
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
				else//�ѳɹ����أ��ȱù���״̬LED����
				{
					 g_ulAutoControl.dis.led_show[0]|= HOTPUMP_STATUS_LED;					
        }	
			}
			else
			{
				g_ulAutoControl.warn_active(OFF);//�������					
			}
			//----------------------ˮ��ˮλУ׼-----------------------
			if(g_ulAutoControl.watertank.sensor_watlevel.calib_cmd)//�յ�У׼����
			{
			  //��ȡ��ǰ����ֵ��Ϊ������ֵ
			  g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale=g_ulAutoControl.watertank.sensor_watlevel.rang.cur_scale;
         //У׼�������ֵ��Ҫ�洢��FLASH/EEPROM��
				enter_critical();//�����ٽ��,���ݶ�д����
				at24c_writebyte(&at24c02,NV_TANK_FULLSCALE_HB_ADDR,(u8)(g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale>>8));
				at24c_writebyte(&at24c02,NV_TANK_FULLSCALE_LB_ADDR,(u8)g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale&0xFF);
				watertank_fullscale_read();//ˮ�����������ݳ�ʼ��ȡ
				exit_critical(); //�˳��ٽ��
				g_ulAutoControl.watertank.sensor_watlevel.calib_cmd=0;
			}
			//ˮ��ˮ�±����ж�
			if(g_ulAutoControl.watertank.sensor_temper.online)//ˮ���¶ȴ������ѹ���
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
			//ˮ��ˮλ�����ж�
			//ˮ��ˮλ������ֵ����������ֵ,����д������
			if(g_ulAutoControl.watertank.sensor_watlevel.online)//ˮλ�������ѹ���
			{
				g_ulAutoControl.warning_status&=~(1<<2);	
				if(g_ulAutoControl.watertank.sensor_watlevel.up_th_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
				{
					g_ulAutoControl.watertank.sensor_watlevel.data_err=1;//��������
					g_ulAutoControl.warning_status|=(1<<8);
				}
				else
				{
					g_ulAutoControl.watertank.sensor_watlevel.data_err=0;//�����־
					g_ulAutoControl.warning_status&=~(1<<8);
					//ˮ��ˮ�¸���������ֵ���ߵ���������ֵ
					if(g_ulAutoControl.watertank.sensor_watlevel.up_th_val<=g_ulAutoControl.watertank.sensor_watlevel.cur_val||\
						g_ulAutoControl.watertank.sensor_watlevel.cur_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
					{
						//1.ˮ��ˮλ����������ֵ
						if(g_ulAutoControl.watertank.sensor_watlevel.cur_val<=g_ulAutoControl.watertank.sensor_watlevel.dw_th_val)
						{
							g_ulAutoControl.warning_status|=(1<<6);
						}
						else
						{
							g_ulAutoControl.warning_status&=~(1<<6);	
						}
						//2.ˮ��ˮλ����������ֵ
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
      //---------------------�ȱ�״̬���---------------------------
			hotpump_workstatus_read();//�ȱù���״̬ʵʱ��ȡ			
		  if(g_ulAutoControl.hotpump.run_status)//�ȱÿ�����������
			{
				//1.�ȱ�ˮ�µ���������ֵ
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
				//2.�ȱ�ˮѹ����������ֵ ����	
        if(g_ulAutoControl.hotpump.sensor_watpress.online)//ˮѹ�������ѹ���
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
			//-----------------------�߿�����Դ��ʱʱ��ο���--------------------------
			if(0==Aucontrol_fsm_me->rtime%RTC_TIME_READ_INV)//һ����ɨ��һ��ʱ��
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











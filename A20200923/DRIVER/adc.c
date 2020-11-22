#include "adc.h"
#include "autom.h"

#define ADC_CH_NUM    (4)            /*ʹ��ADCͨ����ĿΪ4*/
#define TEMPER_CH_NUM (ADC_CH_NUM-2) /*ռ��2ͨ��*/


#if defined (RESISTOR_10K_TAB) /*10K*/
const u16 TempH_Tab[] = 
	{3132,3094,3056,3016,2977,2935,2895,2854,2811,2768,2724,
	                       2681,2636,2592,2547,2502,2456,2411,2366,2315,2275,
	                       2229,2182,2136,2093,2048,2003,1958,1914,1870,1827, 
	                       1782,1742,1701,1658,1613,1577,1536,1497,1458,1421, 
		                     1383,1347,1309,1277,1244,1207,1170,1143,1111,1079, 
		                     1051,1021,993,964,938,910,883,861,835,811, 
		                     787,766,741,722,700,683,660,642,622,604, 
		                     586,568,552,537,522,506,494,471,465,448, 
		                     435,426,413,403,389,379,369,359,348,338, 
	                       327,317,310,300,293,282,275,268,260,253, 
                         };

const u16 TempL_Tab[] ={ 
//	-1	 -2  -3   -4   -5	-6	 -7   -8   -9  -10-----��Ӧ�¶�
	3169,3205,3240,3275,3308,3341,3373,3404,3434,3463,
//	-11 -12 -13 -14 -15 -16 -17 -18 -19 -20-----��Ӧ�¶�
	3492,3519,3545,3570,3590,3620,3643,3664,3686,3706,
//	-21 -22 -23 -24 -25 -26 -27 -28 -29 -30-----��Ӧ�¶�
//	834,843,849,857,865,873,877,887,893,900,
};
#endif

volatile u16 ADC_ConvertedValue[4]={0};

/*******************************************************************************
�������ƣ�NTC�¶Ȳ��, �����������ɼ�ADֵAD_Value���¶ȱ���в��������ֵ����iֵ���Ƕ�Ӧ�¶�
�������  
Ӱ���־λ:��
*******************************************************************************/
u8 Get_Temp(u16 AD_Value)  // ����¶�����
{
	u8 k,flag,f;
	u16  buf;
	u8   temp_Val;
	k=0;
	flag=0;
    if((AD_Value>=4000)||(AD_Value<260))
	   {
	    flag=1;
	   }
    else 
	   {
	   if((AD_Value>=260)&&(AD_Value<3160))
	    {
	     flag=2;
	    }
	   else if((AD_Value>=3160)&&(AD_Value<4000))
	    {
	     flag=3;
	    }	 
	  }
    switch (flag) 
       {
	     case 0:
		        f=0;		  
				break;
		 case 1:
		        f=0;
		        temp_Val=0xff; 
				break;
	     case 2:
		        f=0x01;
				//tmp =0x1f;		  
				break;
		 case 3: 
		        f=0x02;
				//tmp =0x0f;
				break;
		 default: break;
		}
	while(1)
	 {
	 	    if(f==0x00)
	 			 {
	 			   break;
	 			 }
		  if(f==0x01) // �������¶�
			 {
				if(AD_Value>=TempH_Tab[k+1]&&AD_Value<=TempH_Tab[k])	 
				 {
					buf = TempH_Tab[k]-TempH_Tab[k+1];
					if(((TempH_Tab[k]-AD_Value)*2)<=buf)	
					temp_Val=k; 
					else temp_Val=k+1;	
					break;
				 }
				 if(k>(sizeof(TempH_Tab)/sizeof(TempH_Tab[0])))break;
			  }
		if(f==0x02) // �������¶�
		 {
			if(AD_Value>=TempL_Tab[k]&&AD_Value<=TempL_Tab[k+1])	 
			{
   			buf =TempL_Tab[k+1]-TempL_Tab[k];
				if(((AD_Value-TempL_Tab[k])*2)<=buf)	
				temp_Val =-k; 
				else temp_Val=-(k+1);	
				break;
			}
			if(k>(sizeof(TempL_Tab)/sizeof(TempL_Tab[0])))break;
		   }
		   k++;
	}
   return temp_Val;
}

//-------------ˮλ��ȡ------
u8 Get_WatreLevel(u16 AD_Value)
{
	//����
	if(0<AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent5_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=0;		
	}			
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent5_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent15_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=5;		
	}			
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent15_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent25_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=15;		
	}			
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent25_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent35_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=25;		
	}		
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent35_scale<=AD_Value&&AD_Value<=g_ulAutoControl.watertank.sensor_watlevel.rang.percent45_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=35;		
	}	
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent45_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent55_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=45;		
	}
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent55_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent65_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=55;		
	}	
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent65_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent75_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=65;		
	}		
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent75_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent85_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=75;		
	}	
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent85_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.percent95_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=85;		
	}		
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.percent95_scale<=AD_Value&&AD_Value<g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale)
	{
		g_ulAutoControl.watertank.sensor_watlevel.cur_val=95;		
	}		
	if(g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale<=AD_Value)
	{
		if(0==g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale)g_ulAutoControl.watertank.sensor_watlevel.cur_val=0;//��̶�
		else g_ulAutoControl.watertank.sensor_watlevel.cur_val=100;//���̶�
	}
	return 0;
}

//-------------ˮѹ��ȡ------
u8 Get_WatrePress(u16 AD_Value)
{
  //1.�ϵ����Ҫ�� ��FLASH����EEPROM�ж�ȡ������ֵ
}

void adc_thread_entry(void *parameter)
{
    #define TEMPER_OFFLINE_ADC_VAL       (0xFF6)	/*�¶ȴ�����������*/
	  #define WATET_SENSOR_OFFLINE_ADC_VAL (0)	    /*ˮλ/ˮѹ������������*/
	
		static u8 AD_counter0=0,i=0;
		static u16 twink_timer;
	
		u16 Temp_adc_val[TEMPER_CH_NUM]={0};
		u16 WatLevel_adc_val=0;
		u16 WatPress_adc_val=0;		
		u16 sun_temp[ADC_CH_NUM]={0};
		
//---------------------���ֵ-----------------------	
		for(AD_counter0=0;AD_counter0<10;AD_counter0++)
		{
			for(i=0;i<ADC_CH_NUM;i++)
			{
				sun_temp[i]+= ADC_ConvertedValue[i]; 		 
			}
		}	
		for(i=0;i<TEMPER_CH_NUM;i++)
		{
			Temp_adc_val[i] = sun_temp[i]/10;//�¶�ADC��ֵ
		}
		WatLevel_adc_val=sun_temp[2]/10;//ˮλADC��ֵ
		WatPress_adc_val=sun_temp[3]/10;//ˮѹADC��ֵ
		
		g_ulAutoControl.watertank.sensor_temper.online=1;//�ѹ��ش�����
		g_ulAutoControl.watertank.sensor_watlevel.online=1;		
		g_ulAutoControl.hotpump.sensor_watpress.online=1;		
		g_ulAutoControl.hotpump.sensor_temper.online=1;
		
		//0:ˮ���¶ȴ�����
	  //1:�ȱ��¶ȴ�����	
		//2:ˮλ�¶ȴ�����
	  //3:ˮѹ�¶ȴ�����			
//---------�¶ȴ������������ж���ֵ----------------------	
		if(TEMPER_OFFLINE_ADC_VAL<=Temp_adc_val[0])//4086
		{
			g_ulAutoControl.watertank.sensor_temper.online=0;//û�н��봫����
			g_ulAutoControl.warning_status|=(1<<0);
		}
		if(TEMPER_OFFLINE_ADC_VAL<=Temp_adc_val[1])//4086
		{
			g_ulAutoControl.hotpump.sensor_temper.online=0;//û�н��봫����
			g_ulAutoControl.warning_status|=(1<<1);
		}
		if(WATET_SENSOR_OFFLINE_ADC_VAL==WatLevel_adc_val||g_ulAutoControl.watertank.sensor_watlevel.data_err)
		{
			g_ulAutoControl.watertank.sensor_watlevel.online=0;
			if(WATET_SENSOR_OFFLINE_ADC_VAL==WatLevel_adc_val)
			{
				g_ulAutoControl.warning_status|=(1<<2);
			}
		}
		if(WATET_SENSOR_OFFLINE_ADC_VAL==WatPress_adc_val)
		{
			g_ulAutoControl.hotpump.sensor_watpress.online=0;
				g_ulAutoControl.warning_status|=(1<<3);			
		}
//---------------------�������ʾ--------------------
//1.����������	
    if(0==g_ulAutoControl.updata_flag)
		{
			if(g_ulAutoControl.watertank.sensor_temper.online)
			{
				g_ulAutoControl.watertank.sensor_temper.cur_val= Get_Temp(Temp_adc_val[0]);//ˮ���ˮ��
				g_ulAutoControl.dis.seg_show[0] = g_ulAutoControl.watertank.sensor_temper.cur_val/10;
				g_ulAutoControl.dis.seg_show[1] = g_ulAutoControl.watertank.sensor_temper.cur_val%10;
			}	
			if(g_ulAutoControl.hotpump.sensor_temper.online)
			{
				g_ulAutoControl.hotpump.sensor_temper.cur_val = Get_Temp(Temp_adc_val[1]);//�ȱõ�ˮ��
				g_ulAutoControl.dis.seg_show[2] = g_ulAutoControl.hotpump.sensor_temper.cur_val/10;
				g_ulAutoControl.dis.seg_show[3] = g_ulAutoControl.hotpump.sensor_temper.cur_val%10;			
			}
			if(g_ulAutoControl.watertank.sensor_watlevel.online)
			{
				Get_WatreLevel(WatLevel_adc_val);//ˮ���ˮλֵ	
				g_ulAutoControl.dis.seg_show[4] = g_ulAutoControl.watertank.sensor_watlevel.cur_val/10;
				g_ulAutoControl.dis.seg_show[5] = g_ulAutoControl.watertank.sensor_watlevel.cur_val%10;			
			}
			if(g_ulAutoControl.hotpump.sensor_watpress.online)
			{
				g_ulAutoControl.hotpump.sensor_watpress.cur_val=Get_WatreLevel(WatPress_adc_val);//�ȱõ�ˮѹֵ	
				g_ulAutoControl.dis.seg_show[6] = g_ulAutoControl.hotpump.sensor_watpress.cur_val/10;
				g_ulAutoControl.dis.seg_show[7] = g_ulAutoControl.hotpump.sensor_watpress.cur_val%10;					
			}			
		}			
//----------------------��˸��ʾ������-----------------------
//2.������������	
    if(0==g_ulAutoControl.updata_flag)
		{
			twink_timer=(twink_timer+1)%2;		
			if(twink_timer)//������ "E1"
			{
				if(0==g_ulAutoControl.watertank.sensor_temper.online)
				{
					g_ulAutoControl.dis.seg_show[0] = 0x0E;
					g_ulAutoControl.dis.seg_show[1] = 0x01;
					if(g_ulAutoControl.watertank.sensor_watlevel.data_err)
					{
						 g_ulAutoControl.dis.seg_show[1] = 0x02;
					}
				}
				if(0==g_ulAutoControl.hotpump.sensor_temper.online)
				{
					g_ulAutoControl.dis.seg_show[2] = 0x0E;
					g_ulAutoControl.dis.seg_show[3] = 0x01;				
				}	
				if(0==g_ulAutoControl.watertank.sensor_watlevel.online)
				{
					g_ulAutoControl.dis.seg_show[4] = 0x0E;
					g_ulAutoControl.dis.seg_show[5] = 0x01;						
				}
				if(0==g_ulAutoControl.hotpump.sensor_watpress.online)
				{
					g_ulAutoControl.dis.seg_show[6] = 0x0E;
					g_ulAutoControl.dis.seg_show[7] = 0x01;						
				}				
			}
			else //����
			{
				if(0==g_ulAutoControl.watertank.sensor_temper.online)
				{
					g_ulAutoControl.dis.seg_show[0] = 0x11;
					g_ulAutoControl.dis.seg_show[1] = 0x11;
				}
				if(0==g_ulAutoControl.hotpump.sensor_temper.online)
				{
					g_ulAutoControl.dis.seg_show[2] = 0x11;
					g_ulAutoControl.dis.seg_show[3] = 0x11;				
				}
				if(0==g_ulAutoControl.watertank.sensor_watlevel.online)
				{
					g_ulAutoControl.dis.seg_show[4] = 0x11;
					g_ulAutoControl.dis.seg_show[5] = 0x11;						
				}
				if(0==g_ulAutoControl.hotpump.sensor_watpress.online)
				{
					g_ulAutoControl.dis.seg_show[6] = 0x11;
					g_ulAutoControl.dis.seg_show[7] = 0x11;						
				}				
			}			
	 }				
}

void adc_config_init(void)
{
	GPIO_InitType GPIO_InitStructure;
	DMA_InitType  DMA_InitStructure;
	ADC_InitType  ADC_InitStructure;
	
	//ADC ������ʱ�Ӳ��ó��� 28 MHz�������� PCLK2 ����Ƶ������
	RCC_ADCCLKConfig(RCC_APB2CLK_Div8);//16M
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1,ENABLE);	
  RCC_APB2PeriphClockCmd(NTC_PCC,ENABLE);
	
  GPIO_InitStructure.GPIO_Pins = NTC_A_PIN|NTC_B_PIN|WATL_PIN|WATP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		  
  GPIO_Init(NTC_PORT,&GPIO_InitStructure);

	/* DMA channel1 configuration */
  DMA_Reset(DMA1_Channel1);
	//0x4001244C
	//����Ĵ��� ADC1->RDOR
  DMA_InitStructure.DMA_PeripheralBaseAddr = ((u32)0x4001244C);//����DMA�������ַ,��Ϊ���ת������ļĴ���  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//�����ڴ����ַ  
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//DMA_DIR_PeripheralDST;//����AD������Ϊ���ݴ������Դ
  DMA_InitStructure.DMA_BufferSize = 4;//ָ��DMAͨ����DMA����Ĵ�С,����Ҫ���ټ����ڴ�ռ�
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//�趨�Ĵ�����ַ�̶�  
  DMA_InitStructure.DMA_MemoryInc =DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable//�趨�ڴ��ַ�ݼӣ���ÿ��DMA���ǽ�������Ĵ����е�ֵ���������ڴ�ռ���  
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//�趨�������ݿ�� 
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//�趨�ڴ�ĵĿ�� 
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;//�趨DMA������ѭ������ģʽ  
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//�趨DMAѡ����ͨ��������ȼ� 
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA channel1 */
  DMA_ChannelEnable(DMA1_Channel1, ENABLE);
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_InitStructure.ADC_ScanMode =ENABLE;// DISABLE;//ENABLE;//��ͨ���ɼ�
  ADC_InitStructure.ADC_ContinuousMode = ENABLE;//�趨ADת��������ģʽ 
  ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;//��ʹ���ⲿ�ٷ�ת�� 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�ɼ��������ڼĴ������������ķ�ʽ��� 
  ADC_InitStructure.ADC_NumOfChannel = ADC_CH_NUM;//�ɼ�ͨ���� 
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel11 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239_5);//ADC_IN10  //PC0--TEMP1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239_5);//ADC_IN11  //PC1--TEMP1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239_5);//ADC_IN12  //PC2--WATER-LEVEL
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239_5);//ADC_IN13	//PC3--WATER-PRESS	
  /* Enable ADC1 DMA */
  ADC_DMACtrl(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Ctrl(ADC1, ENABLE);
  /* Enable ADC1 reset calibaration register */   
  ADC_RstCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCtrl(ADC1, ENABLE);	
}
//


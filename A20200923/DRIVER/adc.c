#include "adc.h"
#include "autom.h"

#define ADC_CH_NUM    (4)            /*使用ADC通道数目为4*/
#define TEMPER_CH_NUM (ADC_CH_NUM-2) /*占用2通道*/


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
//	-1	 -2  -3   -4   -5	-6	 -7   -8   -9  -10-----对应温度
	3169,3205,3240,3275,3308,3341,3373,3404,3434,3463,
//	-11 -12 -13 -14 -15 -16 -17 -18 -19 -20-----对应温度
	3492,3519,3545,3570,3590,3620,3643,3664,3686,3706,
//	-21 -22 -23 -24 -25 -26 -27 -28 -29 -30-----对应温度
//	834,843,849,857,865,873,877,887,893,900,
};
#endif

volatile u16 ADC_ConvertedValue[4]={0};

/*******************************************************************************
程序名称：NTC温度查表, 功能描述：采集AD值AD_Value与温度表进行查表，如果查得值，则i值就是对应温度
输入参数  
影响标志位:无
*******************************************************************************/
u8 Get_Temp(u16 AD_Value)  // 获得温度数据
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
		  if(f==0x01) // 检索正温度
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
		if(f==0x02) // 检索负温度
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

//-------------水位获取------
u8 Get_WatreLevel(u16 AD_Value)
{
	//量化
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
		if(0==g_ulAutoControl.watertank.sensor_watlevel.rang.full_scale)g_ulAutoControl.watertank.sensor_watlevel.cur_val=0;//零刻度
		else g_ulAutoControl.watertank.sensor_watlevel.cur_val=100;//满刻度
	}
	return 0;
}

//-------------水压获取------
u8 Get_WatrePress(u16 AD_Value)
{
  //1.上电后需要先 从FLASH或者EEPROM中读取满量程值
}

void adc_thread_entry(void *parameter)
{
    #define TEMPER_OFFLINE_ADC_VAL       (0xFF6)	/*温度传感器不在线*/
	  #define WATET_SENSOR_OFFLINE_ADC_VAL (0)	    /*水位/水压传感器不在线*/
	
		static u8 AD_counter0=0,i=0;
		static u16 twink_timer;
	
		u16 Temp_adc_val[TEMPER_CH_NUM]={0};
		u16 WatLevel_adc_val=0;
		u16 WatPress_adc_val=0;		
		u16 sun_temp[ADC_CH_NUM]={0};
		
//---------------------求均值-----------------------	
		for(AD_counter0=0;AD_counter0<10;AD_counter0++)
		{
			for(i=0;i<ADC_CH_NUM;i++)
			{
				sun_temp[i]+= ADC_ConvertedValue[i]; 		 
			}
		}	
		for(i=0;i<TEMPER_CH_NUM;i++)
		{
			Temp_adc_val[i] = sun_temp[i]/10;//温度ADC均值
		}
		WatLevel_adc_val=sun_temp[2]/10;//水位ADC均值
		WatPress_adc_val=sun_temp[3]/10;//水压ADC均值
		
		g_ulAutoControl.watertank.sensor_temper.online=1;//已挂载传感器
		g_ulAutoControl.watertank.sensor_watlevel.online=1;		
		g_ulAutoControl.hotpump.sensor_watpress.online=1;		
		g_ulAutoControl.hotpump.sensor_temper.online=1;
		
		//0:水箱温度传感器
	  //1:热泵温度传感器	
		//2:水位温度传感器
	  //3:水压温度传感器			
//---------温度传感器不在线判断阈值----------------------	
		if(TEMPER_OFFLINE_ADC_VAL<=Temp_adc_val[0])//4086
		{
			g_ulAutoControl.watertank.sensor_temper.online=0;//没有接入传感器
			g_ulAutoControl.warning_status|=(1<<0);
		}
		if(TEMPER_OFFLINE_ADC_VAL<=Temp_adc_val[1])//4086
		{
			g_ulAutoControl.hotpump.sensor_temper.online=0;//没有接入传感器
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
//---------------------数码管显示--------------------
//1.传感器在线	
    if(0==g_ulAutoControl.updata_flag)
		{
			if(g_ulAutoControl.watertank.sensor_temper.online)
			{
				g_ulAutoControl.watertank.sensor_temper.cur_val= Get_Temp(Temp_adc_val[0]);//水箱的水温
				g_ulAutoControl.dis.seg_show[0] = g_ulAutoControl.watertank.sensor_temper.cur_val/10;
				g_ulAutoControl.dis.seg_show[1] = g_ulAutoControl.watertank.sensor_temper.cur_val%10;
			}	
			if(g_ulAutoControl.hotpump.sensor_temper.online)
			{
				g_ulAutoControl.hotpump.sensor_temper.cur_val = Get_Temp(Temp_adc_val[1]);//热泵的水温
				g_ulAutoControl.dis.seg_show[2] = g_ulAutoControl.hotpump.sensor_temper.cur_val/10;
				g_ulAutoControl.dis.seg_show[3] = g_ulAutoControl.hotpump.sensor_temper.cur_val%10;			
			}
			if(g_ulAutoControl.watertank.sensor_watlevel.online)
			{
				Get_WatreLevel(WatLevel_adc_val);//水箱的水位值	
				g_ulAutoControl.dis.seg_show[4] = g_ulAutoControl.watertank.sensor_watlevel.cur_val/10;
				g_ulAutoControl.dis.seg_show[5] = g_ulAutoControl.watertank.sensor_watlevel.cur_val%10;			
			}
			if(g_ulAutoControl.hotpump.sensor_watpress.online)
			{
				g_ulAutoControl.hotpump.sensor_watpress.cur_val=Get_WatreLevel(WatPress_adc_val);//热泵的水压值	
				g_ulAutoControl.dis.seg_show[6] = g_ulAutoControl.hotpump.sensor_watpress.cur_val/10;
				g_ulAutoControl.dis.seg_show[7] = g_ulAutoControl.hotpump.sensor_watpress.cur_val%10;					
			}			
		}			
//----------------------闪烁显示报警码-----------------------
//2.传感器不在线	
    if(0==g_ulAutoControl.updata_flag)
		{
			twink_timer=(twink_timer+1)%2;		
			if(twink_timer)//报警码 "E1"
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
			else //灭屏
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
	
	//ADC 的输入时钟不得超过 28 MHz，它是由 PCLK2 经分频产生，
	RCC_ADCCLKConfig(RCC_APB2CLK_Div8);//16M
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1,ENABLE);	
  RCC_APB2PeriphClockCmd(NTC_PCC,ENABLE);
	
  GPIO_InitStructure.GPIO_Pins = NTC_A_PIN|NTC_B_PIN|WATL_PIN|WATP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		  
  GPIO_Init(NTC_PORT,&GPIO_InitStructure);

	/* DMA channel1 configuration */
  DMA_Reset(DMA1_Channel1);
	//0x4001244C
	//结果寄存器 ADC1->RDOR
  DMA_InitStructure.DMA_PeripheralBaseAddr = ((u32)0x4001244C);//定义DMA外设基地址,即为存放转换结果的寄存器  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//定义内存基地址  
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//DMA_DIR_PeripheralDST;//定义AD外设作为数据传输的来源
  DMA_InitStructure.DMA_BufferSize = 4;//指定DMA通道的DMA缓存的大小,即需要开辟几个内存空间
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//设定寄存器地址固定  
  DMA_InitStructure.DMA_MemoryInc =DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable//设定内存地址递加，即每次DMA都是将该外设寄存器中的值传到三个内存空间中  
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//设定外设数据宽度 
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//设定内存的的宽度 
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;//设定DMA工作再循环缓存模式  
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//设定DMA选定的通道软件优先级 
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA channel1 */
  DMA_ChannelEnable(DMA1_Channel1, ENABLE);
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_InitStructure.ADC_ScanMode =ENABLE;// DISABLE;//ENABLE;//多通道采集
  ADC_InitStructure.ADC_ContinuousMode = ENABLE;//设定AD转化在连续模式 
  ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;//不使用外部促发转换 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//采集的数据在寄存器中以左对齐的方式存放 
  ADC_InitStructure.ADC_NumOfChannel = ADC_CH_NUM;//采集通道数 
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


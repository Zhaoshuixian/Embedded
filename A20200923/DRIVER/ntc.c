

#include "ntc.h"

ntc_st ntc;

const u16 TempH_Tab[] = 
{
	3132,3094,3056,3016,2977,2935,2895,2854,2811,2768,2724,
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


void ntc_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(NTC_PCC,ENABLE);
	
  GPIO_InitStructure.GPIO_Pins = NTC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		  
  GPIO_Init(NTC_PORT,&GPIO_InitStructure);	
}

void ntc_init(void *arg)
{
	ntc_st *const me=(ntc_st *)arg;
	
}

unsigned char ntc_value(void *arg)
{
	ntc_st *const me=(ntc_st *)arg;
	unsigned int ntc_value=0;
	
	
	
	return ntc_value;	
}

//__WEAK void tsensor_init(void)
//{
//	//ntc_init(&ntc);
//}

//__WEAK void tsensor_thread_entry(void *parameter)
//{
//	//ntc_read(&ntc);
//}


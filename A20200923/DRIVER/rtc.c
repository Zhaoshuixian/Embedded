
#include "rtc.h"
#include "stdio.h"
#include "at32f4xx_ertc.h"

rtc_time_st rtc_time=RTC_DEFAULT_CFG;

const unsigned char mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//平均的月份日期表
/////////////////////////////////////////////////////////////////////////////////////////////
void rtc_cfg_init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR,ENABLE);/* 使能PWR和BKP时钟 */  
  PWR_BackupAccessCtrl(ENABLE); /* 使能对后备寄存器的访问 */     
  PWR_Reset();/* 复位BKP寄存器 */    
  RCC_LSEConfig(RCC_LSE_ENABLE);/* 使能LSE */    
  while (RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET) {}/*等待启动完成 */  
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);/* 将 RTC时钟设置为LSE这个32.768KHZ的晶振*/ 
  RCC_ERTCCLKCmd(ENABLE); /* 使能RTC Clock */ 
  ERTC_WaitForSynchro();/* 等待同步 */   
//  RTC_SetDIV(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)*//* 配置了预分频值: 设置RTC时钟周期为1s */     
  //    RTC_ITConfig(RTC_IT_SEC, ENABLE);/* 使能RTC秒中断 */      
}

/////////////////////////////////////////////////////////////////////////////////////////////
void rtc_init(void *arg)
{
	rtc_time_st *const me=arg;
  /*如果是第一次配置时钟，则执行RCC_Configuration()进行配置*/
  if(ERTC_ReadBackupRegister(ERTC_BKP_DT1)!=0x1016)
  {
    rtc_cfg_init();
    rtc_data_set(me);
    ERTC_WriteBackupRegister(ERTC_BKP_DT1,0x1016);//向执行的后备寄存器中写入用户程序数据 
		#if DEBUG_MODE
    printf(" First Config RTC!\r\n");
		#endif
  }
  else
  {
    ERTC_WaitForSynchro();//等待RTC寄存器同步完成
    //RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能RTC秒中断
  }
  
  rtc_data_get(me);//更新时间
}

/////////////////////////////////////////////////////////////////////////////////////////////
unsigned char is_leapyear(unsigned short  pyear)
{
  unsigned char Leap_Flag=0;
  ((((pyear%4)==0)&&(pyear%100)!=0)||(pyear%400==0))?(Leap_Flag=1):(Leap_Flag=0);
  
  return Leap_Flag;
}
/*
*设置时钟
*把输入的时钟转换为秒钟
*以1970年1月1日为基准
*1970~2099年为合法年份
返回值：0，成功；其它：错误
*/
/////////////////////////////////////////////////////////////////////////////////////////////
unsigned char rtc_data_set(void *arg)
{
	rtc_time_st *const me=(rtc_time_st *)arg;
  unsigned short int t;
  unsigned int sec_total_counter=0;
  if(me->year<1970||me->year>2099)  
  {
    return 1;
  }
  for(t=1970;t<me->year;t++)    //把所有年份的秒钟相加
  {
    (is_leapyear(t))?(sec_total_counter+=31622400):(sec_total_counter+=31536000);////闰年的秒钟数
  }
  me->month-=1;//先减掉一个月再算秒数（如现在是5月10日，则只需要算前4个月的天数，再加上10天，然后计算秒数）
  for(t=0;t<me->month;t++)
  {
    sec_total_counter+=(unsigned int)mon_table[t]*86400;//月份秒钟数相加
    (is_leapyear(me->year)&&t==1)?(sec_total_counter+=86400):(0);//闰年，2月份增加一天的秒钟数
  }
  sec_total_counter+=(unsigned int)(me->day-1)*86400;//把前面日期的秒钟数相加（这一天还没过完，所以-1）
  sec_total_counter+=(unsigned int)me->hour*3600;//小时秒钟数
  sec_total_counter+=(unsigned int)me->min*60;//分钟秒钟数
  sec_total_counter+=me->sec;
 
  ERTC_SetWakeUpCounter(sec_total_counter);//设置RTC计数器的值
  ERTC_WakeUpCmd(ENABLE);
  rtc_data_get(me);//更新时间	

  return 0;
}

/*
得到当前的时间
成功返回0，错误返回其它
*/
/////////////////////////////////////////////////////////////////////////////////////////////
void rtc_data_get(void *arg) 
{
	rtc_time_st *const me=(rtc_time_st *)arg;
  static unsigned short int dayCount=0;
  unsigned int sec_total_counter=0;
  unsigned int tmp=0;
  unsigned short int tmp1=0;
  
  sec_total_counter=ERTC_GetWakeUpCounter();
  tmp=sec_total_counter/86400;//得到天数
  
	
  if(dayCount!=tmp)//超过一天
  { 
    dayCount=tmp;
    tmp1=1970;//从1970年开始
    while(tmp>=365)
    {
      if(1==is_leapyear(tmp1))//是闰年
      {
        if(tmp>=366)
        {
          tmp-=366;
        }
        else
        {
          break; //减掉闰年的天数
        }								
      }
      else
      {
        tmp-=365;//平年
      }
      tmp1++;
    }
    me->year=tmp1;//得到年份
    printf("1.year:%d\r\n",me->year);  
    tmp1=0;
    
    while(tmp>=28)//超过一个月
    {
      if(is_leapyear(me->year)&&tmp1==1)//当年是闰年且轮循到2月
      {
        if(tmp>=29)
        {									
          tmp-=29;
        }
        else
        {
          break;
        }
      }
      else
      {
        if(tmp>=mon_table[tmp1])//平年
        {
          tmp-=mon_table[tmp1];
        }
        else
        {
          break;
        }
      }
      tmp1++;
    }
    me->month=tmp1+1;//得到月份，tmp1=0表示1月，所以要加1
    me->day=tmp+1;    //得到日期，因为这一天还没过完，所以tmp只到其前一天，但是显示的时候要显示正常日期
  }
  
  tmp=sec_total_counter%86400;//得到秒钟数
  me->hour=tmp/3600;//小时
  me->min=(tmp%3600)/60;//分钟
  me->sec=(tmp%3600)%60;//秒
  
}

void calc_week(void *arg)
{
	rtc_time_st *const me=(rtc_time_st *)arg;
  unsigned int virt_yeartxt;//创建一个虚拟适用于基姆拉尔森算式的年变量
  unsigned int virt_monthtxt;//创建一个虚拟适用于基姆拉尔森算式的月变量
  virt_yeartxt=me->year;//赋予给 虚拟年
  virt_monthtxt=me->month;//赋予给 虚拟月
  //把一月和二月看成是上一年的十三月和十四月， 例： 如果是 2004-1-10 则换算成： 2003-13-10 来代入公式计算。
  if((1==me->month)||(2==me->month))
  {
    virt_monthtxt+=12; //产生 13/14
    virt_yeartxt-=1;//赋予给 虚拟年
  } 
  //基姆拉尔森算式 此公式是 0~6 表示星期一至星期日
  me->week= (me->day + 2*virt_monthtxt + 3*(virt_monthtxt+1)/5 + virt_yeartxt+virt_yeartxt/4 -\
    virt_yeartxt/100 + virt_yeartxt/400)%7;
  me->week+=1;//转换为 1~7 表示星期一至星期日
}

/*
RTC时钟中断
每秒触发一次
*/
//void RTC_IRQHandler(void)
//{                                
//  if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
//  {
//    RTC_ClearITPendingBit(RTC_IT_ALR);//清闹钟中断        
//  }                                                    
//  RTC_ClearITPendingBit(RTC_IT_OW);//清闹钟中断
//  RTC_WaitForLastTask();                                                   
//}

void hw_rtc_thread_entry(void *parameter)
{ 

  while(1)
  {
    rtc_data_get(&rtc_time);
		#if DEBUG_MODE
    printf("rtc_data:%d-%02d-%02d %02d:%02d:%02d",\
      rtc_time.year,\
      rtc_time.month,\
      rtc_time.day,\
      rtc_time.hour,\
      rtc_time.min,\
      rtc_time.sec\
      ); 
    calc_week(&rtc_time);
    switch(rtc_time.week)
    {
      case 1:
        printf(" Monday\r\n"); 
        break;  
      case 2: 
        printf(" Tuesday\r\n");         
        break;   
      case 3:
        printf(" Wednesday\r\n");         
        break;  
      case 4: 
        printf(" Thursday\r\n");        
        break;   
      case 5:
        printf(" Friday\r\n");         
        break;  
      case 6:
        printf(" Saturday\r\n");         
        break;  
      case 7:
        printf(" Sunday\r\n");         
        break;  
      default:
        printf(" %d",rtc_time.week); 
        printf(" Week Error!\r\n");         
        break;        
    }
		#endif
  }
}


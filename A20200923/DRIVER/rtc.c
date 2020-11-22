
#include "rtc.h"
#include "stdio.h"
#include "at32f4xx_ertc.h"

rtc_time_st rtc_time=RTC_DEFAULT_CFG;

const unsigned char mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//ƽ�����·����ڱ�
/////////////////////////////////////////////////////////////////////////////////////////////
void rtc_cfg_init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR,ENABLE);/* ʹ��PWR��BKPʱ�� */  
  PWR_BackupAccessCtrl(ENABLE); /* ʹ�ܶԺ󱸼Ĵ����ķ��� */     
  PWR_Reset();/* ��λBKP�Ĵ��� */    
  RCC_LSEConfig(RCC_LSE_ENABLE);/* ʹ��LSE */    
  while (RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET) {}/*�ȴ�������� */  
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);/* �� RTCʱ������ΪLSE���32.768KHZ�ľ���*/ 
  RCC_ERTCCLKCmd(ENABLE); /* ʹ��RTC Clock */ 
  ERTC_WaitForSynchro();/* �ȴ�ͬ�� */   
//  RTC_SetDIV(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)*//* ������Ԥ��Ƶֵ: ����RTCʱ������Ϊ1s */     
  //    RTC_ITConfig(RTC_IT_SEC, ENABLE);/* ʹ��RTC���ж� */      
}

/////////////////////////////////////////////////////////////////////////////////////////////
void rtc_init(void *arg)
{
	rtc_time_st *const me=arg;
  /*����ǵ�һ������ʱ�ӣ���ִ��RCC_Configuration()��������*/
  if(ERTC_ReadBackupRegister(ERTC_BKP_DT1)!=0x1016)
  {
    rtc_cfg_init();
    rtc_data_set(me);
    ERTC_WriteBackupRegister(ERTC_BKP_DT1,0x1016);//��ִ�еĺ󱸼Ĵ�����д���û��������� 
		#if DEBUG_MODE
    printf(" First Config RTC!\r\n");
		#endif
  }
  else
  {
    ERTC_WaitForSynchro();//�ȴ�RTC�Ĵ���ͬ�����
    //RTC_ITConfig(RTC_IT_SEC, ENABLE);//ʹ��RTC���ж�
  }
  
  rtc_data_get(me);//����ʱ��
}

/////////////////////////////////////////////////////////////////////////////////////////////
unsigned char is_leapyear(unsigned short  pyear)
{
  unsigned char Leap_Flag=0;
  ((((pyear%4)==0)&&(pyear%100)!=0)||(pyear%400==0))?(Leap_Flag=1):(Leap_Flag=0);
  
  return Leap_Flag;
}
/*
*����ʱ��
*�������ʱ��ת��Ϊ����
*��1970��1��1��Ϊ��׼
*1970~2099��Ϊ�Ϸ����
����ֵ��0���ɹ�������������
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
  for(t=1970;t<me->year;t++)    //��������ݵ��������
  {
    (is_leapyear(t))?(sec_total_counter+=31622400):(sec_total_counter+=31536000);////�����������
  }
  me->month-=1;//�ȼ���һ����������������������5��10�գ���ֻ��Ҫ��ǰ4���µ��������ټ���10�죬Ȼ�����������
  for(t=0;t<me->month;t++)
  {
    sec_total_counter+=(unsigned int)mon_table[t]*86400;//�·����������
    (is_leapyear(me->year)&&t==1)?(sec_total_counter+=86400):(0);//���꣬2�·�����һ���������
  }
  sec_total_counter+=(unsigned int)(me->day-1)*86400;//��ǰ�����ڵ���������ӣ���һ�컹û���꣬����-1��
  sec_total_counter+=(unsigned int)me->hour*3600;//Сʱ������
  sec_total_counter+=(unsigned int)me->min*60;//����������
  sec_total_counter+=me->sec;
 
  ERTC_SetWakeUpCounter(sec_total_counter);//����RTC��������ֵ
  ERTC_WakeUpCmd(ENABLE);
  rtc_data_get(me);//����ʱ��	

  return 0;
}

/*
�õ���ǰ��ʱ��
�ɹ�����0�����󷵻�����
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
  tmp=sec_total_counter/86400;//�õ�����
  
	
  if(dayCount!=tmp)//����һ��
  { 
    dayCount=tmp;
    tmp1=1970;//��1970�꿪ʼ
    while(tmp>=365)
    {
      if(1==is_leapyear(tmp1))//������
      {
        if(tmp>=366)
        {
          tmp-=366;
        }
        else
        {
          break; //�������������
        }								
      }
      else
      {
        tmp-=365;//ƽ��
      }
      tmp1++;
    }
    me->year=tmp1;//�õ����
    printf("1.year:%d\r\n",me->year);  
    tmp1=0;
    
    while(tmp>=28)//����һ����
    {
      if(is_leapyear(me->year)&&tmp1==1)//��������������ѭ��2��
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
        if(tmp>=mon_table[tmp1])//ƽ��
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
    me->month=tmp1+1;//�õ��·ݣ�tmp1=0��ʾ1�£�����Ҫ��1
    me->day=tmp+1;    //�õ����ڣ���Ϊ��һ�컹û���꣬����tmpֻ����ǰһ�죬������ʾ��ʱ��Ҫ��ʾ��������
  }
  
  tmp=sec_total_counter%86400;//�õ�������
  me->hour=tmp/3600;//Сʱ
  me->min=(tmp%3600)/60;//����
  me->sec=(tmp%3600)%60;//��
  
}

void calc_week(void *arg)
{
	rtc_time_st *const me=(rtc_time_st *)arg;
  unsigned int virt_yeartxt;//����һ�����������ڻ�ķ����ɭ��ʽ�������
  unsigned int virt_monthtxt;//����һ�����������ڻ�ķ����ɭ��ʽ���±���
  virt_yeartxt=me->year;//����� ������
  virt_monthtxt=me->month;//����� ������
  //��һ�ºͶ��¿�������һ���ʮ���º�ʮ���£� ���� ����� 2004-1-10 ����ɣ� 2003-13-10 �����빫ʽ���㡣
  if((1==me->month)||(2==me->month))
  {
    virt_monthtxt+=12; //���� 13/14
    virt_yeartxt-=1;//����� ������
  } 
  //��ķ����ɭ��ʽ �˹�ʽ�� 0~6 ��ʾ����һ��������
  me->week= (me->day + 2*virt_monthtxt + 3*(virt_monthtxt+1)/5 + virt_yeartxt+virt_yeartxt/4 -\
    virt_yeartxt/100 + virt_yeartxt/400)%7;
  me->week+=1;//ת��Ϊ 1~7 ��ʾ����һ��������
}

/*
RTCʱ���ж�
ÿ�봥��һ��
*/
//void RTC_IRQHandler(void)
//{                                
//  if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
//  {
//    RTC_ClearITPendingBit(RTC_IT_ALR);//�������ж�        
//  }                                                    
//  RTC_ClearITPendingBit(RTC_IT_OW);//�������ж�
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



#include "ertc.h"

rtc_st rtc;

#define ERTC_CLOCK_SOURCE_LSE          /* LSE used as ERTC source clock */
//#define ERTC_CLOCK_SOURCE_LSI         /* LSI used as ERTC source clock. The ERTC Clock may varies due to LSI frequency dispersion. */ 

/**
  * @brief  Configure the ERTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void ertc_config(void *arg)
{
	
	rtc_st *const me =(rtc_st *)arg;
	
	ERTC_InitType  ERTC_InitStructure;
	ERTC_TimeType  ERTC_TimeStructure; //时刻
  ERTC_DateType  ERTC_DateStructure; //日期
  ERTC_AlarmType ERTC_AlarmStructure;//闹钟	
	
//  NVIC_InitType  NVIC_InitStructure;
  EXTI_InitType  EXTI_InitStructure;	
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);//使能备份电源设备时钟
  /* Allow access to ERTC */
  PWR_BackupAccessCtrl(ENABLE);//允许操作ERTC
  /* Reset ERTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
	
#if defined (ERTC_CLOCK_SOURCE_LSI)  /* LSI used as ERTC source clock*/
  /* The ERTC Clock may varies due to LSI frequency dispersion */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSISTBL) == RESET){};
  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSI);
  
  /* ck_spre(1Hz) = ERTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
#elif defined (ERTC_CLOCK_SOURCE_LSE) /* LSE used as ERTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ENABLE);
  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSESTBL) == RESET){};
  /* Select the ERTC Clock Source */
  RCC_ERTCCLKConfig(RCC_ERTCCLKSelection_LSE);
  /* ck_spre(1Hz) = ERTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  __IO uint32_t  uwSynchPrediv = 0xFF;
  __IO uint32_t  uwAsynchPrediv = 0x7F;
#endif /* ERTC_CLOCK_SOURCE_LSI */
  /* Enable the ERTC Clock */
  RCC_ERTCCLKCmd(ENABLE);
  /* Deinitializes the ERTC registers */  
  ERTC_Reset();  
  /* Wait for ERTC APB registers synchronisation */
  ERTC_WaitForSynchro();
  /* Configure the ERTC data register and ERTC prescaler */
  ERTC_InitStructure.ERTC_AsynchPrediv = uwAsynchPrediv;
  ERTC_InitStructure.ERTC_SynchPrediv  = uwSynchPrediv;
  ERTC_InitStructure.ERTC_HourFormat   = ERTC_HourFormat_24;//24H格式
  ERTC_Init(&ERTC_InitStructure);
	
	//设置闹钟时间
  /* Set the alarm 05h:20min:30s */
//  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_AMPM    = ERTC_H12_AM;//上午的
//  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours   = me->alarm.hour;
//  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes = me->alarm.min;
//  ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds = me->alarm.sec;
//  ERTC_AlarmStructure.ERTC_AlarmDateWeek          = 0x31;
//  ERTC_AlarmStructure.ERTC_AlarmDateWeekSel = ERTC_AlarmDateWeekSel_Date;
//  ERTC_AlarmStructure.ERTC_AlarmMask = ERTC_AlarmMask_DateWeek;
//  /* Configure the ERTC Alarm A register */
//  ERTC_SetAlarmValue(ERTC_Format_BCD, ERTC_AlA, &ERTC_AlarmStructure);
	
//  //配置闹钟中断
//  /* ERTC Alarm A Interrupt Configuration */
//  EXTI_ClearIntPendingBit(EXTI_Line17);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  
//  /* Enable the ERTC Alarm Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = ERTCAlarm_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);	
	
//  /* Enable ERTC Alarm A Interrupt */
//  ERTC_INTConfig(ERTC_INT_ALA, ENABLE);//使能闹钟中断
//  /* Enable the alarm */
//  ERTC_AlarmCmd(ERTC_AlA, ENABLE);//使能闹钟
//  ERTC_ClearFlag(ERTC_FLAG_ALAF);
	
	//设置RTC初始日期
  /* Set the date: Friday January 11th 2013 */
  ERTC_DateStructure.ERTC_Year    = me->system.year;
  ERTC_DateStructure.ERTC_Month   = me->system.month;
  ERTC_DateStructure.ERTC_Date    = me->system.day;
  ERTC_DateStructure.ERTC_WeekDay = me->system.week;
  ERTC_SetDateValue(ERTC_Format_BCD, &ERTC_DateStructure);
  
  /* Set the time to 05h 20mn 00s AM */
	if(12>me->system.hour)
		
  ERTC_TimeStructure.ERTC_AMPM    = ERTC_H12_AM;//上午的
	else
  ERTC_TimeStructure.ERTC_AMPM    = ERTC_H12_PM;//下午的		
  ERTC_TimeStructure.ERTC_Hours   = me->system.hour;
  ERTC_TimeStructure.ERTC_Minutes = me->system.hour;
  ERTC_TimeStructure.ERTC_Seconds = me->system.sec; 
  
  ERTC_SetTimeValue(ERTC_Format_BCD, &ERTC_TimeStructure);   
  /* Indicator for the ERTC configuration */
  ERTC_WriteBackupRegister(ERTC_BKP_DT0, 0x32F1);
}	

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void ERTC_TimeShow(void)
{
	ERTC_TimeType  ERTC_TimeStructure; //时刻	
  /* Get the current Time */
  ERTC_GetTimeValue(ERTC_Format_BIN, &ERTC_TimeStructure);
	#if DEBUG_MODE	
  /* Display time Format : hh:mm:ss */
  printf("%0.2d:%0.2d:%0.2d\r\n",\
	ERTC_TimeStructure.ERTC_Hours, \
	ERTC_TimeStructure.ERTC_Minutes, \
	ERTC_TimeStructure.ERTC_Seconds);
	#endif
	
}

/**
  * @brief  Display the current Alarm.
  * @param  None
  * @retval None
  */
void ERTC_AlarmShow(void)
{
  ERTC_AlarmType ERTC_AlarmStructure;//闹钟		
  /* Get the current Alarm */
  ERTC_GetAlarmValue(ERTC_Format_BIN, ERTC_AlA, &ERTC_AlarmStructure);
	#if DEBUG_MODE
  printf("Alarm %0.2d:%0.2d:%0.2d\r\n", \
	ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Hours,\
	ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Minutes,\
	ERTC_AlarmStructure.ERTC_AlarmTime.ERTC_Seconds);
	#endif
}

void ertc_init(void *arg)
{
	rtc_st *const me =(rtc_st *)arg;
	
  if(ERTC_ReadBackupRegister(ERTC_BKP_DT0) != 0x32F1)//首次上电配置
  {  
    /* ERTC configuration  */
    ertc_config(me);
    /* Display the ERTC Time and Alarm */
    ERTC_TimeShow();
    ERTC_AlarmShow();
  }
  else//已上电配置过
  {
    /* Check if the Power On Reset flag is set */
    if(RCC_GetFlagStatus(RCC_FLAG_PORST) != RESET)//上电复位
    {
      /* Power On Reset occurred     */
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      /* External Reset occurred */
    }
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR,ENABLE);
    /* Allow access to ERTC */
    PWR_BackupAccessCtrl(ENABLE);//允许操作ERTC
    /* Wait for ERTC APB registers synchronisation */
    ERTC_WaitForSynchro();
    /* Clear the ERTC Alarm Flag */
    ERTC_ClearFlag(ERTC_FLAG_ALAF);//清除闹钟标志
    /* Clear the EXTI Line 17 Pending bit (Connected internally to ERTC Alarm) */
    EXTI_ClearIntPendingBit(EXTI_Line17);//闹钟中断线
    /* Display the ERTC Time and Alarm */
    ERTC_TimeShow();
    ERTC_AlarmShow();
  }	
}

void ertc_get(void *arg)
{
	ERTC_TimeType  ERTC_TimeStructure; //时刻
	ERTC_DateType  ERTC_DateStructure; //日期
	
	ERTC_GetTimeValue(ERTC_Format_BIN,&ERTC_TimeStructure);
	ERTC_GetDateValue(ERTC_Format_BIN,&ERTC_DateStructure);
	
	rtc.system.year  = ERTC_DateStructure.ERTC_Year;
	rtc.system.month = ERTC_DateStructure.ERTC_Month;
	rtc.system.day   = ERTC_DateStructure.ERTC_Date;
	rtc.system.hour  = ERTC_TimeStructure.ERTC_Hours;
	rtc.system.min   = ERTC_TimeStructure.ERTC_Minutes;
	rtc.system.sec   = ERTC_TimeStructure.ERTC_Seconds;	
	
#if 0	
	static int temp=0;
	if(temp != ERTC_TimeStructure.ERTC_Seconds)
	{
		temp = ERTC_TimeStructure.ERTC_Seconds;
		/* Display time Format : hh:mm:ss */
		printf("%0.2d:%0.2d:%0.2d\r\n",ERTC_TimeStructure.ERTC_Hours, ERTC_TimeStructure.ERTC_Minutes, ERTC_TimeStructure.ERTC_Seconds);
	}
#endif
	
}

void ertc_thread_entry(void *parameter)
{
	ertc_get(&rtc);
}









#include "infrade_r.h"
#include "timer.h"   
#include "buzzer.h"
#include "task.h"

/*红外接收驱动*/

/*
红外学习思想：
记录波形，然后需要的时候再复现波形

方法：
采用外部中断+定时器

外部中断负责边沿的处理，定时器则记录期间的波形时间长度

*/

ir_st ir;

const unsigned char IR_ID=0;

void infrared_rx_init(void)
{
	GPIO_InitType  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(IR_RX_PCC, ENABLE);// 
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
	
  GPIO_InitStructure.GPIO_Pins     = IR_RX_PIN ;   
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_PU; //
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz; //
  
  GPIO_Init(IR_RX_PORT,&GPIO_InitStructure); //

	timer4_init(10000,144);
}

//引脚外部中断配置
void infrade_rx_interrupt(FunctionalState NewState)
{
	NVIC_InitType NVIC_InitStructure;  
	EXTI_InitType EXTI_InitStructure;  

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  //PB7
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;     
	NVIC_Init(&NVIC_InitStructure);  

	EXTI_ClearIntPendingBit(EXTI_Line7);  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinsSource7);//PB7

	EXTI_InitStructure.EXTI_Line   = EXTI_Line7; 
	EXTI_InitStructure.EXTI_Mode   = EXTI_Mode_Interrupt;   
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising_Falling;//上下沿
	EXTI_InitStructure.EXTI_LineEnable=NewState;  
	EXTI_Init(&EXTI_InitStructure);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ir_status_analyze(void *arg)
{
  nec_st *const me =(nec_st *)arg;
	
  if(me->rec_status&0x80)			//上次有数据被接收到了4.5MS的帧头（me->rec_status^7==1）
  {	
    me->rec_status&=~0X10;                     //(me->rec_status & 1110 1111)	取消上升沿已经被捕获标记（me->rec_status^4=0）
    if((me->rec_status&0X0F)==0X00)  me->rec_status|=1<<6;//标记已经完成一次按键的键值信息采集（me->rec_status^6=1）

    if((me->rec_status&0X0F)<14)     me->rec_status++;
    else
    {
      me->rec_status&=~(1<<7);		         //清空引导标识（me->rec_status^7=0）
      me->rec_status&=0XF0;			//清空计数器	(me->rec_status & 1111 0000)
      //me->rec_status&=(F<<4);	       //清空计数器	(me->rec_status & 1111 0000)
    }								 	   	
  }		
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ir_wave_analyze(void *arg)
{
	nec_st *const me =(nec_st *)arg;
	
  if(IR_RX_DATA())//捕获到上升沿【9ms帧头的捕获】
  { 
    TMR_OC2PolarityConfig(TMR4,TMR_ICPolarity_Falling);	//CC4P=1  设置为下降沿捕获
    TMR_SetCounter(TMR4,0);					//清空定时器值
    me->rec_status|=(1<<4);					//标记上升沿已经被捕获（me->rec_status^4=1）
  }                                                   
  else //若捕捉到下降沿
  {
    me->rec_dval=TMR_GetCapture2(TMR4);				//读取CCR4也可以清CC4IF标志位
    TMR_OC2PolarityConfig(TMR4,TMR_ICPolarity_Rising); //CC4P=0  设置为上升沿捕获 
    if(me->rec_dval>4200&&me->rec_dval<4700) //【4.5ms的帧头捕获】
    {
      me->rec_status|=1<<7;	         //标记成功接收到了引导码
      me->rec_num=0;		//清除按键次数计数器
    }
    else if((me->rec_status&0X10)&&(me->rec_status&0X80))//接收到引导码标志信号电平【9ms + 4.5ms】
    {			
      if(me->rec_dval>300&&me->rec_dval<800)	      //560为标准值,560us
      {
        me->rec_data<<=1;		     //左移一位.
        me->rec_data|=0;	            //接收到0	   
      }
      else if(me->rec_dval>1400&&me->rec_dval<1800)   //1680为标准值,1680us
      {
        me->rec_data<<=1;		    //左移一位.
        me->rec_data|=1;		    //接收到1
      }
      else if(me->rec_dval>2200&&me->rec_dval<2600)  //得到按键键值增加的信息 2500为标准值
      {
        me->rec_num++; 	          //按键次数增加1次
        me->rec_status&=0XF0;	         //清空计时器		
      }
    }					 
    me->rec_status&=~(1<<4);      //me->rec_status^4=0
  }	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ir_code_parse(void *arg)
{	
	nec_st *const me =(nec_st *)arg;
	
	static unsigned char  last_status,now_status;
	
  if(me->rec_status&(1<<6))//得到一个按键的所有信息了（me->rec_status^6==1）
  { 
    me->data.addr  = (me->rec_data>>24)&0xFF;		      //得到地码(Addr)
    me->data.addrn = (me->rec_data>>16)&0xFF;	        //得到地址反码(AddrN)
    me->data.cmd   = (me->rec_data>>8)&0xFF;           //得到命令码(Cmd)
    me->data.cmdn  =  me->rec_data&0xFF;               //得到命令反码(CmdN) 				
    if((me->data.addr==(unsigned char)~me->data.addrn)&&(me->data.addr==IR_ID)&&(me->data.cmd==(unsigned char)~me->data.cmdn))//检验遥控识别码(ID)及地址和命令（纠错） 
    {
			now_status=1;
			if(last_status!=now_status)
			{
			  led_func(&led.blue,1);	
				buzzer_start(&buzzer,1,500);//蜂鸣器鸣叫
				last_status = now_status;
			}
      me->valid_data=me->data.cmd;//键值正确
      me->valid_data=1;
    }   			
    if((0==me->valid_data)||(0==(me->rec_status&0x80)))//按键数据错误/遥控已经没有按下了
    {
      me->rec_status&=~(1<<6);//清除接收到有效按键标识 me->rec_status^6=0
      me->rec_num=0;	 //清除按键次数计数器
    }
  }
	else
	{
    now_status=0;
		last_status = now_status;		
	}
}

static void ir_study_record(void *arg)
{
	study_st *const me =(study_st *)arg;
	
	if(me->status)//处于学习状态
	{
		led_func(&led.blue,1);	
		if(0!=me->edge_cnt)//边沿记录为0，
		{
			infrade_rx_interrupt(ENABLE);//表示刚进入，则开启外部中断使能
		}
		else //有边沿记录了
		{
			me->timeout                    =  0; //
			me->wave_time                  =  TMR_GetCounter(TMR4);//读取边沿前的电平时间长度
			me->wave_buff[me->edge_cnt++]  =  me->wave_time;//并录入缓存区
			TMR_SetCounter(TMR4,0);//重置定时器
		}		
	}
	else
	{
		infrade_rx_interrupt(DISABLE);//关闭外部中断，禁止波形处理
		me->edge_cnt=0;
		memset(me->wave_buff,0x00,MAX_EDGE_NUM);
	}
}

static void ir_study_status(void *arg)
{
	#define IR_STUDY_TIMEOUT  (20/IR_TASK_TIME) /*红外j结束学习判断时间*/
	
	study_st *const me =(study_st *)arg;

	if(me->status)
	{
		if(IR_STUDY_TIMEOUT<me->timeout++)//超时
		{
			me->status=0;//关闭学习状态
		}	
  }
}

//用于NEC解码处理
void IR_NEC_IRQHandler(void)
{
  if(TMR_GetINTStatus(TMR4,TMR_INT_Overflow)!=RESET) //检测是否发生溢出更新中断事件（即捕获的信号周期大于设定的10MS），判断帧头前9ms
  {
	  ir_status_analyze(&ir.nec);//仅NEC解码 
  }
  if(TMR_GetINTStatus(TMR4,TMR_INT_CC2)!=RESET) //检测是否发生捕获中断事件
  {	
   	ir_wave_analyze(&ir.nec);//仅NEC解码 
  }
  TMR_ClearITPendingBit(TMR4,TMR_INT_CC2);	 //清除中断事件标志位 
  TMR_ClearITPendingBit(TMR4,TMR_INT_Overflow);	 //清除中断事件标志位   	
}

//用于红外学习处理
void IR_STUDY_IRQHandler(void)
{
	if(EXTI_GetIntStatus(EXTI_Line7)!=RESET)
	{
	  buzzer_start(&buzzer,1,500);//蜂鸣器鸣叫				
		ir_study_record(&ir.study);
		EXTI_ClearIntPendingBit(EXTI_Line7);		
	}		
}

void ir_thread_entry(void *parameter)
{
	ir_code_parse(&ir.nec);//仅NEC解码
  ir_study_status(&ir.study);//红外学习
}


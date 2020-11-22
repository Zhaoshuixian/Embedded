

#include "infrade_r.h"
#include "timer.h"   
#include "buzzer.h"
#include "task.h"

/*�����������*/

/*
����ѧϰ˼�룺
��¼���Σ�Ȼ����Ҫ��ʱ���ٸ��ֲ���

������
�����ⲿ�ж�+��ʱ��

�ⲿ�жϸ�����صĴ�����ʱ�����¼�ڼ�Ĳ���ʱ�䳤��

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

//�����ⲿ�ж�����
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
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising_Falling;//������
	EXTI_InitStructure.EXTI_LineEnable=NewState;  
	EXTI_Init(&EXTI_InitStructure);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ir_status_analyze(void *arg)
{
  nec_st *const me =(nec_st *)arg;
	
  if(me->rec_status&0x80)			//�ϴ������ݱ����յ���4.5MS��֡ͷ��me->rec_status^7==1��
  {	
    me->rec_status&=~0X10;                     //(me->rec_status & 1110 1111)	ȡ���������Ѿ��������ǣ�me->rec_status^4=0��
    if((me->rec_status&0X0F)==0X00)  me->rec_status|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ���me->rec_status^6=1��

    if((me->rec_status&0X0F)<14)     me->rec_status++;
    else
    {
      me->rec_status&=~(1<<7);		         //���������ʶ��me->rec_status^7=0��
      me->rec_status&=0XF0;			//��ռ�����	(me->rec_status & 1111 0000)
      //me->rec_status&=(F<<4);	       //��ռ�����	(me->rec_status & 1111 0000)
    }								 	   	
  }		
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ir_wave_analyze(void *arg)
{
	nec_st *const me =(nec_st *)arg;
	
  if(IR_RX_DATA())//���������ء�9ms֡ͷ�Ĳ���
  { 
    TMR_OC2PolarityConfig(TMR4,TMR_ICPolarity_Falling);	//CC4P=1  ����Ϊ�½��ز���
    TMR_SetCounter(TMR4,0);					//��ն�ʱ��ֵ
    me->rec_status|=(1<<4);					//����������Ѿ�������me->rec_status^4=1��
  }                                                   
  else //����׽���½���
  {
    me->rec_dval=TMR_GetCapture2(TMR4);				//��ȡCCR4Ҳ������CC4IF��־λ
    TMR_OC2PolarityConfig(TMR4,TMR_ICPolarity_Rising); //CC4P=0  ����Ϊ�����ز��� 
    if(me->rec_dval>4200&&me->rec_dval<4700) //��4.5ms��֡ͷ����
    {
      me->rec_status|=1<<7;	         //��ǳɹ����յ���������
      me->rec_num=0;		//�����������������
    }
    else if((me->rec_status&0X10)&&(me->rec_status&0X80))//���յ��������־�źŵ�ƽ��9ms + 4.5ms��
    {			
      if(me->rec_dval>300&&me->rec_dval<800)	      //560Ϊ��׼ֵ,560us
      {
        me->rec_data<<=1;		     //����һλ.
        me->rec_data|=0;	            //���յ�0	   
      }
      else if(me->rec_dval>1400&&me->rec_dval<1800)   //1680Ϊ��׼ֵ,1680us
      {
        me->rec_data<<=1;		    //����һλ.
        me->rec_data|=1;		    //���յ�1
      }
      else if(me->rec_dval>2200&&me->rec_dval<2600)  //�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ
      {
        me->rec_num++; 	          //������������1��
        me->rec_status&=0XF0;	         //��ռ�ʱ��		
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
	
  if(me->rec_status&(1<<6))//�õ�һ��������������Ϣ�ˣ�me->rec_status^6==1��
  { 
    me->data.addr  = (me->rec_data>>24)&0xFF;		      //�õ�����(Addr)
    me->data.addrn = (me->rec_data>>16)&0xFF;	        //�õ���ַ����(AddrN)
    me->data.cmd   = (me->rec_data>>8)&0xFF;           //�õ�������(Cmd)
    me->data.cmdn  =  me->rec_data&0xFF;               //�õ������(CmdN) 				
    if((me->data.addr==(unsigned char)~me->data.addrn)&&(me->data.addr==IR_ID)&&(me->data.cmd==(unsigned char)~me->data.cmdn))//����ң��ʶ����(ID)����ַ��������� 
    {
			now_status=1;
			if(last_status!=now_status)
			{
			  led_func(&led.blue,1);	
				buzzer_start(&buzzer,1,500);//����������
				last_status = now_status;
			}
      me->valid_data=me->data.cmd;//��ֵ��ȷ
      me->valid_data=1;
    }   			
    if((0==me->valid_data)||(0==(me->rec_status&0x80)))//�������ݴ���/ң���Ѿ�û�а�����
    {
      me->rec_status&=~(1<<6);//������յ���Ч������ʶ me->rec_status^6=0
      me->rec_num=0;	 //�����������������
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
	
	if(me->status)//����ѧϰ״̬
	{
		led_func(&led.blue,1);	
		if(0!=me->edge_cnt)//���ؼ�¼Ϊ0��
		{
			infrade_rx_interrupt(ENABLE);//��ʾ�ս��룬�����ⲿ�ж�ʹ��
		}
		else //�б��ؼ�¼��
		{
			me->timeout                    =  0; //
			me->wave_time                  =  TMR_GetCounter(TMR4);//��ȡ����ǰ�ĵ�ƽʱ�䳤��
			me->wave_buff[me->edge_cnt++]  =  me->wave_time;//��¼�뻺����
			TMR_SetCounter(TMR4,0);//���ö�ʱ��
		}		
	}
	else
	{
		infrade_rx_interrupt(DISABLE);//�ر��ⲿ�жϣ���ֹ���δ���
		me->edge_cnt=0;
		memset(me->wave_buff,0x00,MAX_EDGE_NUM);
	}
}

static void ir_study_status(void *arg)
{
	#define IR_STUDY_TIMEOUT  (20/IR_TASK_TIME) /*����j����ѧϰ�ж�ʱ��*/
	
	study_st *const me =(study_st *)arg;

	if(me->status)
	{
		if(IR_STUDY_TIMEOUT<me->timeout++)//��ʱ
		{
			me->status=0;//�ر�ѧϰ״̬
		}	
  }
}

//����NEC���봦��
void IR_NEC_IRQHandler(void)
{
  if(TMR_GetINTStatus(TMR4,TMR_INT_Overflow)!=RESET) //����Ƿ�����������ж��¼�����������ź����ڴ����趨��10MS�����ж�֡ͷǰ9ms
  {
	  ir_status_analyze(&ir.nec);//��NEC���� 
  }
  if(TMR_GetINTStatus(TMR4,TMR_INT_CC2)!=RESET) //����Ƿ��������ж��¼�
  {	
   	ir_wave_analyze(&ir.nec);//��NEC���� 
  }
  TMR_ClearITPendingBit(TMR4,TMR_INT_CC2);	 //����ж��¼���־λ 
  TMR_ClearITPendingBit(TMR4,TMR_INT_Overflow);	 //����ж��¼���־λ   	
}

//���ں���ѧϰ����
void IR_STUDY_IRQHandler(void)
{
	if(EXTI_GetIntStatus(EXTI_Line7)!=RESET)
	{
	  buzzer_start(&buzzer,1,500);//����������				
		ir_study_record(&ir.study);
		EXTI_ClearIntPendingBit(EXTI_Line7);		
	}		
}

void ir_thread_entry(void *parameter)
{
	ir_code_parse(&ir.nec);//��NEC����
  ir_study_status(&ir.study);//����ѧϰ
}


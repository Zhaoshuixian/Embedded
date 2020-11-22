
#include "ts06.h"
#include "buzzer.h"
#include "task.h"

tkey_st ts06=
{
	.read_key  = NONE_TK,
	.hold_key  = NONE_TK,	
	.trig_key  = NONE_TK,	
  .status    = {0},	
	.hold_time =  0,
  .i2c       = TS06_I2C_BUS_CFG,
};

//-----------------需要实现的对接IIC协议接口的函数-----------------
void ts06_gpio_init(void)
{
 	GPIO_InitType GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(TS06_SDA_PCC,ENABLE);
  RCC_APB2PeriphClockCmd(TS06_SCL_PCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pins = TS06_SDA_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;
	GPIO_Init(TS06_SDA_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pins = TS06_SCL_PIN;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;	
	GPIO_Init(TS06_SCL_PORT, &GPIO_InitStructure); 	
}

void ts06_sda_mode(unsigned char s)
{
	(0!=s)?(TS06_SDA_OUT()):(TS06_SDA_IN());
}

void ts06_sda_out(unsigned char s)
{
	TS06_SDA(s);
}

void ts06_sck_out(unsigned char s)
{
	TS06_SCL(s);
}

unsigned char ts06_sda_read(void)
{
	return TS06_READ_SDA();
}


//////////////////////////////////////////////////////////////////////
void ts06_writebyte(void *arg,unsigned char Reg_Addr,unsigned char Reg_Data)
{
	 i2c_st *const me = (i2c_st *)arg;	
	
   iic_start(me);                    //起始信号
   iic_sendbyte(me,TS06_SLAVE_ADDR_WR);//发送设备地址+写信号
	 iic_wait_ack(me);
   iic_sendbyte(me,Reg_Addr);        //内部寄存器地址
	 iic_wait_ack(me);
   iic_sendbyte(me,Reg_Data);       //内部寄存器数据
	 iic_wait_ack(me);
	 iic_stop(me);                   //发送停止信号		
}

//////////////////////////////////////////////////////////////////////
unsigned char ts06_readbyte(void *arg,unsigned char Reg_Addr)
{  
	 unsigned char Reg_Data;
	 i2c_st *const me = (i2c_st *)arg;
	
	 iic_start(me);                          //起始信号
	 iic_sendbyte(me,TS06_SLAVE_ADDR_WR);     //发送设备地址+写信号
	 iic_wait_ack(me);	
	 iic_sendbyte(me,Reg_Addr);              //发送存储单元地址，从0开始   
	 iic_wait_ack(me);	
	 iic_stop(me);   
	
	 iic_start(me);                           //起始信号
	 iic_sendbyte(me,TS06_SLAVE_ADDR_RD);     //发送设备地址+读信号
	 iic_wait_ack(me);	
	 Reg_Data=iic_readbyte(me,0);            //读出寄存器数据  
	 iic_stop(me);                           //停止信号	 
	
	 return Reg_Data; 
}

//////////////////////////////////////////////////////////////////////
void ts06_init(void *arg)
{
	 tkey_st *const me = (tkey_st *)arg;	
	 iic_init(&me->i2c);
	 iic_delayms(&me->i2c,150);//SCL SDA high pulse remain time for power on(>100ms)
	 ts06_writebyte(&me->i2c,CH_HOLD_ADDR,CH_HOLD_VAL);/*ch_hold set*/
   ts06_writebyte(&me->i2c,REF_RST_ADDR,REF_RST_VAL);
}

///////////////////////////////////////////////////////////////////
unsigned char tkey_value=0;
unsigned char getkey(void *arg)
{
	tkey_st *const me = (tkey_st *)arg;		
	/*获取键值寄存器值*/
	tkey_value=ts06_readbyte(me,OUTPUT_REG_ADDR);
	switch(tkey_value)
	{
		case 8:
			me->read_key=TK4;break;/* */
		case 4:
			me->read_key=TK3;break;/* */
		case 2:
			me->read_key=TK2;break;/* */
		case 1:
			me->read_key=TK1;break;/* */
		default:
		break;			
	}
   
	/*取有效值范围*/
	if(tkey_value&0x0F) me->status.now=1;//有效按键动作
	else  	            me->status.now=0;//清除状态         
	
	if(me->status.now)//按键按下
	{
		me->hold_time++;//计时
	  me->hold_key=me->read_key;//保持键值（即若寄存器不变化，此值恒在）		
		if(me->status.last!=me->status.now)//按键变化
		{
	     buzzer_start(&buzzer,1,500);//蜂鸣器鸣叫		
			 led_func(&led.blue,1);			
			 me->trig_key=me->read_key;  //触发键值（即若寄存器不变化，此值只存在一次）	
		}
	}
	else//释放
	{
		me->hold_time=0;//按下时间计时清0；
	}
	
	me->status.last=me->status.now;//记录末态	
	
	return 0;
}

void holdkey_handle(void *arg)
{
	tkey_st *const me = (tkey_st *)arg;
	
	/*按键KEY长按时间阈值*/
	#define KEY_LONGTIME_HOLD (3000/KEY_TASK_TIME)
	
	switch(me->hold_key)
	{
		case TK1:
			if(KEY_LONGTIME_HOLD<me->hold_time)
			{
				
			}
			break;
		case TK2:
			break;
		case TK3:
			break;
		case TK4:
			break;
    default:break;		
	}	
}

void trigkey_handle(void *arg)
{
	tkey_st *const me = (tkey_st *)arg;
	switch(me->trig_key)
	{
		case TK1:
      OLED_adjust_flag=1;
      OLED_Brightness++;  //OLED亮度1-254
			break;
		case TK2:
      OLED_adjust_flag=1;
      OLED_Brightness--;  //OLED亮度1-254			
			break;
		case TK3:
			break;
		case TK4:
			break;
    default:break;		
	}	
	me->trig_key=NONE_TK;//用完立即销毁
}

void key_thread_entry(void *parameter)
{
	getkey(&ts06);
	trigkey_handle(&ts06);
  holdkey_handle(&ts06);
}








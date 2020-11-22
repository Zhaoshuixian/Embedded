
#include "ds18b20.h"
#include "delay.h"

ds18b20_st ds18b20=
{
	.one_wire =BUS_CFG,
};
  
///////////////////////////////////////////////////////////////////////////////

void ds_gpio_init(void)
{ 
  GPIO_InitType GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(DS_PCC,ENABLE);//开设备时钟
  
  GPIO_InitStruct.GPIO_Pins     = DS_PIN ;   //设端口
  GPIO_InitStruct.GPIO_Mode     = GPIO_Mode_OUT_OD; //设端口信号输出模式	
  GPIO_InitStruct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //设端口输出最高时钟速度
  
  GPIO_Init(DS_PORT, &GPIO_InitStruct); //初始化端口
}

void ds_set(unsigned char s)
{
  s?(GPIO_SetBits(DS_PORT,DS_PIN)):(GPIO_ResetBits(DS_PORT,DS_PIN));
}
///////////////////////////////////////////////////////////////////////////////
unsigned char ds_read(void)
{
  return GPIO_ReadInputDataBit(DS_PORT,DS_PIN);
}

///////////////////////////////////////////////////////////////////////////////
unsigned char ds18b20_reset(void *arg)
{
	gpio_drv_st *const me =(gpio_drv_st *)arg;

  unsigned char res=0;
  me->gpio_set(1);         //初始化置高
  me->delay_us (600);
  /*MCU拉低总线，约480us~960us 然后释放总线*/
  me->gpio_set(0);     

  //拉低总线
  me->delay_us(505);         //约500us 
  me->gpio_set(1);           //释放总线
  me->delay_us(30);          //设置一个释放总线的电平渐变时间（15~60us）
  
  /*此时IC会输出LV以示应答*/
  /*MCU读应答信号，若在此期间读得LV，则表示IC复位成功*/
  res = me->gpio_read();    //读总线电平
  while(res);               //若读得低电平
  me->delay_us(150);        //则延时100us（60~240us）
  me->gpio_set(1);          // IC释放总线
	
  return res;
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_writebyte(void *arg,unsigned char wdat)
{
	gpio_drv_st *const me =(gpio_drv_st *)arg;
	
  unsigned char bit_shift=8;
  
  me->gpio_set(1);          //初始化置高
  me->delay_us(12);           //延迟

  for(bit_shift=8;bit_shift>0;bit_shift--)
  {
    me->gpio_set(0);        //拉低总线，允许时序在10~15us
    me->delay_us(11);          //约12us
    (wdat&0x01)?(me->gpio_set(1)):(me->gpio_set(0));
    me->delay_us(30);          //约30us（20~45us）
    me->gpio_set(1);          //释放总线
    me->delay_us(3);          //延时一段时间，准备写下一位数据
    wdat>>=1;
  }
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_readbyte(void *arg,unsigned char *rdat)
{
	gpio_drv_st *const me =(gpio_drv_st *)arg;
  
  me->gpio_set(1);         //初始化置高
  me->delay_us (6);           //延迟

  for(unsigned char bit_shift=8;bit_shift>0;bit_shift--)
  {
    *rdat>>=1;
    me->gpio_set(0);     //拉低总线电平，约1us
    me->delay_us (1);   
    me->gpio_set(1);       //释放总线
    (me->gpio_read())?(*rdat|=0x80):(*rdat&=~0x80);
    me->delay_us (42);       //允许时序时间为40~45us
    me->gpio_set(1); ;      //释放总线
  }
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_getuid(void *arg,unsigned char *rom_addr)//64bit唯一码
{
	ds18b20_st *const me =(ds18b20_st *)arg;

  ds18b20_writebyte(&me->one_wire,ReadROM);
  
  for(unsigned char i=8;i>0;i--)
  {
    ds18b20_readbyte(&me->one_wire,&rom_addr[8-i]);
  }
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_startconvert(void *arg)
{
	ds18b20_st *const me =(ds18b20_st *)arg;
	
  ds18b20_reset(&me->one_wire);                   //复位
  ds18b20_writebyte(&me->one_wire,SkipROM);       //跳过ROM搜索
  ds18b20_writebyte(&me->one_wire,StartConvert);  //开始转换命令
  ds18b20_reset(&me->one_wire);                   //复位
  ds18b20_writebyte(&me->one_wire,SkipROM);       //跳过ROM搜索
  ds18b20_writebyte(&me->one_wire,ReadScratchpad);//读寄存器值命令  
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_init(void *arg)
{
	ds18b20_st *const me =(ds18b20_st *)arg;
	
  me->one_wire.gpio_init(); //执行配置初始化
  me->one_wire.gpio_set(1);  //总线处于释放
	
  ds18b20_reset(&me->one_wire);            //复位
  ds18b20_writebyte(&me->one_wire,SkipROM); //跳过ROM搜索cmd
  ds18b20_writebyte(&me->one_wire,WriteScratchpad);//
  //	ds18b20_writebyte(&me->one_wire,AlarmTL);
  //	ds18b20_writebyte(&me->one_wire,AlarmTH);//写报警值
  ds18b20_writebyte(&me->one_wire,Precision);//写入转换精度
  ds18b20_reset(&me->one_wire);//复位
  ds18b20_writebyte(&me->one_wire,SkipROM); //跳过ROM搜索cmd
  ds18b20_writebyte(&me->one_wire,CopyScratchpad);//复制寄存器的值存入EEPROM
  while(0==me->one_wire.gpio_read());  //等待复制完成
   
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_read(void *arg)
{
	ds18b20_st *const me =(ds18b20_st *)arg;
	
  unsigned char ds_th=0,ds_tl=0;
  
  ds18b20_startconvert(&me->one_wire);      //温度转换开启
  ds18b20_readbyte(&me->one_wire,&ds_tl);
  ds18b20_readbyte(&me->one_wire,&ds_th);
   
  me->data=(signed int)(((signed int)ds_th<<8)|ds_tl);   //获取最终温度数据
  if(ds_th&0xFC)//检查正负值
  {
    
    me->polar=NEGATIVE;  //负值温度
    me->data=(~(signed int)me->data+1);
    me->data*=62;
    me->data/=1000;
  }
  else
  {
    me->polar=POSITIVE; //正值温度
    me->data*=62;
    me->data/=1000;		
  }
}

//__WEAK 
void tsensor_init(void)
{
	ds18b20_init(&ds18b20);
}

//__WEAK 
void tsensor_thread_entry(void *parameter)
{
  ds18b20_read(&ds18b20); 
}



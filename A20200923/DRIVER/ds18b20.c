
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
  RCC_APB2PeriphClockCmd(DS_PCC,ENABLE);//���豸ʱ��
  
  GPIO_InitStruct.GPIO_Pins     = DS_PIN ;   //��˿�
  GPIO_InitStruct.GPIO_Mode     = GPIO_Mode_OUT_OD; //��˿��ź����ģʽ	
  GPIO_InitStruct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //��˿�������ʱ���ٶ�
  
  GPIO_Init(DS_PORT, &GPIO_InitStruct); //��ʼ���˿�
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
  me->gpio_set(1);         //��ʼ���ø�
  me->delay_us (600);
  /*MCU�������ߣ�Լ480us~960us Ȼ���ͷ�����*/
  me->gpio_set(0);     

  //��������
  me->delay_us(505);         //Լ500us 
  me->gpio_set(1);           //�ͷ�����
  me->delay_us(30);          //����һ���ͷ����ߵĵ�ƽ����ʱ�䣨15~60us��
  
  /*��ʱIC�����LV��ʾӦ��*/
  /*MCU��Ӧ���źţ����ڴ��ڼ����LV�����ʾIC��λ�ɹ�*/
  res = me->gpio_read();    //�����ߵ�ƽ
  while(res);               //�����õ͵�ƽ
  me->delay_us(150);        //����ʱ100us��60~240us��
  me->gpio_set(1);          // IC�ͷ�����
	
  return res;
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_writebyte(void *arg,unsigned char wdat)
{
	gpio_drv_st *const me =(gpio_drv_st *)arg;
	
  unsigned char bit_shift=8;
  
  me->gpio_set(1);          //��ʼ���ø�
  me->delay_us(12);           //�ӳ�

  for(bit_shift=8;bit_shift>0;bit_shift--)
  {
    me->gpio_set(0);        //�������ߣ�����ʱ����10~15us
    me->delay_us(11);          //Լ12us
    (wdat&0x01)?(me->gpio_set(1)):(me->gpio_set(0));
    me->delay_us(30);          //Լ30us��20~45us��
    me->gpio_set(1);          //�ͷ�����
    me->delay_us(3);          //��ʱһ��ʱ�䣬׼��д��һλ����
    wdat>>=1;
  }
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_readbyte(void *arg,unsigned char *rdat)
{
	gpio_drv_st *const me =(gpio_drv_st *)arg;
  
  me->gpio_set(1);         //��ʼ���ø�
  me->delay_us (6);           //�ӳ�

  for(unsigned char bit_shift=8;bit_shift>0;bit_shift--)
  {
    *rdat>>=1;
    me->gpio_set(0);     //�������ߵ�ƽ��Լ1us
    me->delay_us (1);   
    me->gpio_set(1);       //�ͷ�����
    (me->gpio_read())?(*rdat|=0x80):(*rdat&=~0x80);
    me->delay_us (42);       //����ʱ��ʱ��Ϊ40~45us
    me->gpio_set(1); ;      //�ͷ�����
  }
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_getuid(void *arg,unsigned char *rom_addr)//64bitΨһ��
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
	
  ds18b20_reset(&me->one_wire);                   //��λ
  ds18b20_writebyte(&me->one_wire,SkipROM);       //����ROM����
  ds18b20_writebyte(&me->one_wire,StartConvert);  //��ʼת������
  ds18b20_reset(&me->one_wire);                   //��λ
  ds18b20_writebyte(&me->one_wire,SkipROM);       //����ROM����
  ds18b20_writebyte(&me->one_wire,ReadScratchpad);//���Ĵ���ֵ����  
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_init(void *arg)
{
	ds18b20_st *const me =(ds18b20_st *)arg;
	
  me->one_wire.gpio_init(); //ִ�����ó�ʼ��
  me->one_wire.gpio_set(1);  //���ߴ����ͷ�
	
  ds18b20_reset(&me->one_wire);            //��λ
  ds18b20_writebyte(&me->one_wire,SkipROM); //����ROM����cmd
  ds18b20_writebyte(&me->one_wire,WriteScratchpad);//
  //	ds18b20_writebyte(&me->one_wire,AlarmTL);
  //	ds18b20_writebyte(&me->one_wire,AlarmTH);//д����ֵ
  ds18b20_writebyte(&me->one_wire,Precision);//д��ת������
  ds18b20_reset(&me->one_wire);//��λ
  ds18b20_writebyte(&me->one_wire,SkipROM); //����ROM����cmd
  ds18b20_writebyte(&me->one_wire,CopyScratchpad);//���ƼĴ�����ֵ����EEPROM
  while(0==me->one_wire.gpio_read());  //�ȴ��������
   
}

///////////////////////////////////////////////////////////////////////////////
void ds18b20_read(void *arg)
{
	ds18b20_st *const me =(ds18b20_st *)arg;
	
  unsigned char ds_th=0,ds_tl=0;
  
  ds18b20_startconvert(&me->one_wire);      //�¶�ת������
  ds18b20_readbyte(&me->one_wire,&ds_tl);
  ds18b20_readbyte(&me->one_wire,&ds_th);
   
  me->data=(signed int)(((signed int)ds_th<<8)|ds_tl);   //��ȡ�����¶�����
  if(ds_th&0xFC)//�������ֵ
  {
    
    me->polar=NEGATIVE;  //��ֵ�¶�
    me->data=(~(signed int)me->data+1);
    me->data*=62;
    me->data/=1000;
  }
  else
  {
    me->polar=POSITIVE; //��ֵ�¶�
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



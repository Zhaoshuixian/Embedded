
#include "dt11.h"


//DT11初始化
//返回值 ：1 不存在  0存在	 

uint8_t DT11_TEMP = 0;
uint8_t DT11_HUMI = 0;
 

void Dht11Rst(void)	   
{                 
    DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//??DQ
    delay_ms(20);    	//延时18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
    delay_us(30);     	//????20~40us
}


//等待DT11回应
//返回1  DT11 不存在    0存在 
u8 Dht11Check(void) 	   
{   
    u8 retry=0;
    DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11 拉低 40~80us
    {
        retry++;
        delay_us(1);
    };	 
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉高 40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)
			return 1;	    
    return 0;
}


//从DT11 读取一个位
//返回值 0或者1
u8 Dht11ReadBit(void) 			 
{
    u8 retry=0;
    while(DHT11_DQ_IN&&retry<100)//等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//等待变为高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us
    if(DHT11_DQ_IN)return 1;
    else return 0;		   
}

//从DHT11读取一个字节
//返回值：独到的数据
u8 Dht11ReadByte(void)    
{        
    u8 i,dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
        dat<<=1; 
        dat|=Dht11ReadBit();
    }						    
    return dat;
}


//从DT11里读到一次数据 
//temp:温度值返回
//humi:(20%-90%)
//返回值：0 读取正常  1 读取失败
u8 Dht11ReadData(void)    
{        
    u8 buf[5];
    u8 i;
    Dht11Rst();
    if(Dht11Check()==0)
    {
        for(i=0;i<5;i++)//??40???
        {
            buf[i]=Dht11ReadByte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            DT11_HUMI=buf[0];
            DT11_TEMP=buf[2];
        }
    }else 
			return 1;
    return 0;	    
}

u8 Dht11Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE ); //启动IO复用功能

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  Dht11Rst();
  return Dht11Check();
}

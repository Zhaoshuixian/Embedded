
#include "dt11.h"


//DT11��ʼ��
//����ֵ ��1 ������  0����	 

uint8_t DT11_TEMP = 0;
uint8_t DT11_HUMI = 0;
 

void Dht11Rst(void)	   
{                 
    DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//??DQ
    delay_ms(20);    	//��ʱ18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
    delay_us(30);     	//????20~40us
}


//�ȴ�DT11��Ӧ
//����1  DT11 ������    0���� 
u8 Dht11Check(void) 	   
{   
    u8 retry=0;
    DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11 ���� 40~80us
    {
        retry++;
        delay_us(1);
    };	 
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11���� 40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)
			return 1;	    
    return 0;
}


//��DT11 ��ȡһ��λ
//����ֵ 0����1
u8 Dht11ReadBit(void) 			 
{
    u8 retry=0;
    while(DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�ߵ�ƽ
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//�ȴ�40us
    if(DHT11_DQ_IN)return 1;
    else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
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


//��DT11�����һ������ 
//temp:�¶�ֵ����
//humi:(20%-90%)
//����ֵ��0 ��ȡ����  1 ��ȡʧ��
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE ); //����IO���ù���

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  Dht11Rst();
  return Dht11Check();
}

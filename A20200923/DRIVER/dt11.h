#ifndef	DT11_H
#define DT11_H

#include "includes.h"

//IOIO方向
#define DHT11_IO_IN()  {GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=8<<20;}  // PB11
#define DHT11_IO_OUT() {GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=3<<20;}

////IO操作函数										   
#define	DHT11_DQ_OUT PBout(5) //	PG11 
#define	DHT11_DQ_IN  PBin(5)  //	PG11


u8 Dht11Init(void);
void Dht11Rst(void);
u8 Dht11ReadData(void);   
extern uint8_t DT11_TEMP;
extern uint8_t DT11_HUMI;
#endif

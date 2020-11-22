

#ifndef __SPI_H__
#define __SPI_H__

#include "at32f4xx.h"

void spi_gpio_init(void);
unsigned char SPI1_ReadWriteByte(unsigned char TxData);
unsigned char SPI2_ReadWriteByte(unsigned char TxData);

#endif 



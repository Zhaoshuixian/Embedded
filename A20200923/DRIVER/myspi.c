
#include "myspi.h"


void spi_init(spi_st *const spi_me)
{
	spi_me->spi_gpio_init();
	spi_me->cs_gpio_init();	
	spi_me->cs(1);		//SPI FLASH²»Ñ¡ÖÐ	
}



#ifndef __MYSPI_H__
#define __MYSPI_H__

#include "spi.h"

typedef struct
{
	void (*spi_gpio_init)(void);
	void (*cs_gpio_init)(void);
	void (*cs)(unsigned char);//Æ¬Ñ¡¿ØÖÆ
	unsigned char (*wr_data)(unsigned char);
	unsigned char (*rd_data)(unsigned char);
	void (*delay_us)(unsigned int);
	void (*delay_ms)(unsigned int);	
}spi_st;


void spi_init(spi_st *const spi_me);



#endif

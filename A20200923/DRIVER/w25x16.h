#ifndef __FLASH_H
#define __FLASH_H			


#include "pindef.h"
#include "myspi.h"
#include "delay.h"

#define	SPI_FLASH_LOCK   (W25Q_CSN(0))  //选中FLASH		
#define	SPI_FLASH_UNLOCK  W25Q_CSN(1) 


extern u8 SPI_FLASH_BUF[1024];	



#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg	0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

typedef enum
{
	W25Q_NULL=0,
	W25Q08=0xEF13,
	W25Q16=0xEF14,
	W25Q32=0xEF15,
	W25Q64=0xEF16,
	W25Q128=0xEF17,
	W25Q_UNKNOW,
}w25q_type;

typedef struct
{
	w25q_type type;
	spi_st spi;
}w25q_device_st;//多设备

extern w25q_device_st w25q16;
void w25q_device_register(w25q_type type,w25q_device_st *const w25q_device);
unsigned char w25q_device_check(w25q_device_st *const w25q_device);
//---------------------------
void spi_gpio_init(void);
void w25q_cs_gpio_init(void);
void w25q_cs(unsigned char s);
//-------------------------------
unsigned short int w25q_read_id(w25q_device_st *const w25q_device);
unsigned char w25q_read_sr(w25q_device_st *const w25q_device)  ;
void w25q_write_sr(w25q_device_st *const w25q_device,unsigned char sr)  ;
void w25q_write_enable(w25q_device_st *const w25q_device) ;
void w25q_write_disable(w25q_device_st *const w25q_device)   ;
void w25q_write_sr(w25q_device_st *const w25q_device,unsigned char sr)  ;
void w25q_write_enable(w25q_device_st *const w25q_device)   ;
void w25q_write_disable(w25q_device_st *const w25q_device)  ;
void w25q_read_data(w25q_device_st *const w25q_device,unsigned char* rdata,unsigned int raddr,unsigned short int rdata_len)  ;
void w25q_busy(w25q_device_st *const w25q_device)  ;
void w25q_erase_total(w25q_device_st *const w25q_device)   ;
void w25q_erase_sector(w25q_device_st *const w25q_device,unsigned int sector_addr)  ;
void w25q_pd_mode(w25q_device_st *const w25q_device) ;
void w25q_wakeup(w25q_device_st *const w25q_device)   ;
void w25q_write_page(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len) ;
void w25q_write_uncheck(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len)   ;
void w25q_write_data(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len) ;

#endif



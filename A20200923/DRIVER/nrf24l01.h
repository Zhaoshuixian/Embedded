

#ifndef __NRF24L01_H__
#define __NRF24L01_H__


#include "myspi.h"
#include "pindef.h"
#include "delay.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TX_PLOAD_WIDTH 32  				// ����ͨ����Ч���ݿ��

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI(SI24R1) commands
#define READ_IC_REG        0x00  // Define read command to register
#define WRITE_IC_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI(SI24R1) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define RSSI            0x09  // 'Received Signal Strength Indecator' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATUS Register 
#define RX_DR						0x40  /**/
#define TX_DS						0x20
#define MAX_RT					0x10

typedef enum
{
	RX_MODE,
	TX_MODE=1,
	RTX_MODE=2,
}RF_MODE;/*ͨ��ģʽ RX_MODE / TX_MODE*/

typedef struct 
{
	#define MAX_BUFF_LEN (32)
	unsigned char ch;          /*ͨ���ŵ�ֵ*/
	unsigned char sub_addr[5]; /*ͨ�ŵ�ַ���*/
	unsigned char dev_addr;    /*�豸��ַ���*/
	unsigned char data[MAX_BUFF_LEN];
	unsigned char len;
	unsigned char checksum;            
}para_st;

typedef struct
{
	void (*irq_gpio_init)(void);
	void (*ce_gpio_init)(void);
	void (*ce_sel)(unsigned char);
	unsigned char (*irq_read)(void);	
	spi_st  spi;
	para_st para;
}nrf24_device_st;//


#define NRF24_SPI_BUS_CFG \
{ \
	.spi_gpio_init = spi_gpio_init,\
	.cs_gpio_init  = nrf24_cs_gpio_init,\
	.cs            = nrf24_cs,\
	.wr_data       = SPI1_ReadWriteByte,\
	.rd_data       = SPI1_ReadWriteByte,\
	.delay_us      = xdelay_us,\
	.delay_ms      = xdelay_ms\
}

#define NRF24_PARA_CFG \
{\
	.ch       = 124,\
	.sub_addr = {0x11,0x22,0x33,0x44,0x55},\
	.dev_addr = 1,\
}

extern nrf24_device_st nrf24;


void nrf24_cs_gpio_init(void);
void nrf24_cs(unsigned char s);
void nrf24_ce_gpio_init(void);
void nrf24_ce(unsigned char s);
void nrf24_irq_gpio_init(void);
unsigned char nrf24_irq_read(void);
void nrf24_init(void *arg,RF_MODE mode);

void rf_thread_entry(void *parameter);

#endif




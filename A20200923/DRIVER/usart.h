#ifndef __USART_H
#define __USART_H

#include "pindef.h"

//////////////////////////////////////////////////////////////////////////////////	 

#define Rs485TxEnable  MODBUS_TX_EN(1)
#define Rs485RxEnable  MODBUS_TX_EN(0)

#define BUFF1_MAX_LEN (256)
#define BUFF2_MAX_LEN (256)


extern  unsigned  short int UART1_RLen;
extern 	unsigned  char UART1_RBuf[BUFF1_MAX_LEN];
extern	unsigned  short int UART2_RLen;
extern	unsigned  char UART2_RBuf[BUFF2_MAX_LEN];

void uart_init(unsigned char ulPort,unsigned long bound);
void Uart1_SendByte(unsigned  char byte);
void enable_uart1_port(unsigned char x);
void  Uart1_SendData(unsigned  char *data ,unsigned  short int len);
void  Uart2_SendData(unsigned  char *data ,unsigned  short int len);
void  Uart3_SendData(unsigned  char *data ,unsigned  short int len);
void usart3_transimit(unsigned  char *data ,unsigned  short int len,void (*en_port)(unsigned char));
void usart2_transimit(unsigned  char *data ,unsigned  short int len,void (*en_port)(unsigned char));
void usart1_transimit(unsigned  char *data ,unsigned  short int len,void (*en_port)(unsigned char));

extern void UartCheckAndSet(unsigned char bound);

void uart1_thread_entry(void *);
void uart2_thread_entry(void *);

#endif

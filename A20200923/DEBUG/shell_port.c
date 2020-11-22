/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell_port.h"


#define  UART_NUM_3      (3)
#define  SHELL_UART      (UART_NUM_3)


Shell shell;
char shellBuffer[512];

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 */
void userShellWrite(char data)
{
   Uart3_SendData((unsigned char *)&data, 1);
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @return char 状态
 */
signed char userShellRead(char *data)
{
   return 
		 /*(uart_read_bytes(SHELL_UART, (uint8_t *)data, 1, portMAX_DELAY) == 1)?0:-1*/
	 1;
}


/**
 * @brief 用户shell初始化
 * 
 */
void shell_init(void)
{
	  /*For DEBUG*/	
 	  uart_init(SHELL_UART,115200);  //调试串口3初始化为9600 
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);
}


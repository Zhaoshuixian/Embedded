


#ifndef __MB_HOST_H__
#define __MB_HOST_H__

#include "../user/UART/RS485.h"

extern modbus_st mb_host;

void modbus_host_send_create(modbus_st *const mb_host_me,unsigned char dev_addr,unsigned char func,unsigned short int start_reg, unsigned short int reg_num,unsigned char *reg_val);
unsigned char modbus_host_recev_handler(modbus_st *const mb_host_me,unsigned char *data_buff,unsigned char data_len);
void modbus_host_handler(void);

#endif



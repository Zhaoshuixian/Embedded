#include "24cxx.h" 


//EEPROM存储数组  
unsigned char AT24X_FLASH_BUF[NV_EE_SIZE];

/*设备句柄*/
at24c_device_st at24c02=
{
	.type = AT24C02,
  .i2c  = EE_I2C_BUS_CFG,
};
		
void at24c_gpio_init(void)
{
  GPIO_InitType GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(AT24CXX_PCC,ENABLE); 	
  
  GPIO_InitStructure.GPIO_Pins = AT24CXX_SCL_PIN|AT24CXX_SDA_PIN;				 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(AT24CXX_PORT, &GPIO_InitStructure);  	
}

void at24c_sda_mode(unsigned char s)
{
	if(0!=s)
	{
		AT24C_SDA_OUT();
	}
	else
	{
		AT24C_SDA_IN();
	}
}

void at24c_sda_out(unsigned char s)
{
	AT24C_SDA(s);
}

void at24c_sck_out(unsigned char s)
{
	AT24C_SCL(s);
}

unsigned char at24c_sda_read(void)
{
	return AT24C_READ_SDA();
}

void at24c_device_register(at24c_type at_type,at24c_device_st *const at24c_device)
{
	at24c_device->type=at_type;
	iic_init(&at24c_device->i2c);	//挂载I2C
}

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
unsigned char at24c_device_check(at24c_device_st *const at24c_device)
{

	at24cx_databuff_get(at24c_device);//读全域数据并检查标志位
	while(0x55!=at24c_readbyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR))
	{
		return 1;//失败
	}
	at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,AT24X_FLASH_BUF,NV_EE_SIZE-1);//读0x00~0xFF存储域
	return 0;
}


//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
unsigned char at24c_readbyte(at24c_device_st *const at24c_device,unsigned short int reg_addr)
{
	unsigned char temp=0;
	
  iic_start(&at24c_device->i2c);
	if(at24c_device->type>AT24C16)
	{
		iic_sendbyte(&at24c_device->i2c,0XA0); //发送写命令
    iic_wait_ack(&at24c_device->i2c);
		iic_sendbyte(&at24c_device->i2c,reg_addr>>8);//发送高地址
		iic_wait_ack(&at24c_device->i2c);		 
	}
	else 
	{
		iic_sendbyte(&at24c_device->i2c,0XA0+((reg_addr/256)<<1));   //发送器件地址0XA0,写数据
	}		
	iic_wait_ack(&at24c_device->i2c); 
  iic_sendbyte(&at24c_device->i2c,reg_addr%256);   //发送低地址
	iic_wait_ack(&at24c_device->i2c);	    
	iic_start(&at24c_device->i2c);  	 	   
	iic_sendbyte(&at24c_device->i2c,0XA1);           //进入接收模式			   
	iic_wait_ack(&at24c_device->i2c);	 
  temp=iic_readbyte(&at24c_device->i2c,0);		   
  iic_stop(&at24c_device->i2c); 
	
	return temp;	
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void at24c_writebyte(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char wdata)
{				   	  	    																 
  iic_start(&at24c_device->i2c);
	if(at24c_device->type>AT24C16)
	{
		iic_sendbyte(&at24c_device->i2c,0XA0); //发送写命令
	iic_wait_ack(&at24c_device->i2c); 
		iic_sendbyte(&at24c_device->i2c,waddr>>8);//发送高地址
	iic_wait_ack(&at24c_device->i2c);  
	}
	else
	{
		iic_sendbyte(&at24c_device->i2c,0XA0+((waddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	iic_wait_ack(&at24c_device->i2c); 
  iic_sendbyte(&at24c_device->i2c,waddr%256);   //发送低地址
	iic_wait_ack(&at24c_device->i2c);  										  		   
	iic_sendbyte(&at24c_device->i2c,wdata);     //发送字节							   
	iic_wait_ack(&at24c_device->i2c); 	    	   
  iic_stop(&at24c_device->i2c);//产生一个停止条件 
	iic_delayms(&at24c_device->i2c,10);	 
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void at24c_writenbytes(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char *wdata,unsigned char wdata_len)
{  	
	unsigned char t;
	for(t=0;t<wdata_len;t++)
	{
		at24c_writebyte(at24c_device,waddr+t,(*wdata>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
unsigned int at24c_readnbytes(at24c_device_st *const at24c_device,unsigned short int reg_addr,unsigned char rdata_len)
{  	
	unsigned char t;
	unsigned int temp=0;
	for(t=0;t<rdata_len;t++)
	{
		temp<<=8;
		temp+=at24c_readbyte(at24c_device,reg_addr+rdata_len-t-1); 	 				   
	}
	return temp;												    
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个
void at24c_read_byaddr(at24c_device_st *const at24c_device,unsigned short int reg_addr,unsigned char *rdata,unsigned short int rdata_len)
{
	while(rdata_len)
	{
		*rdata++=at24c_readbyte(at24c_device,reg_addr++);	
		rdata_len--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void at24c_write_byaddr(at24c_device_st *const at24c_device,unsigned char waddr,unsigned char *wdata,unsigned short int wdata_len)
{
	while(wdata_len--)
	{
		at24c_writebyte(at24c_device,waddr,*wdata);
		waddr++;
		wdata++;
	}
}

//FLASH恢复出厂默认值
void at24c_default(at24c_device_st *const at24c_device)  
{
    memset(AT24X_FLASH_BUF,0xFF,NV_EE_SIZE);
	
  	at24c_write_byaddr(at24c_device,NV_EE_START_ADDR,(unsigned char*)&AT24X_FLASH_BUF,NV_EE_SIZE);//擦除存储0~255

	
	  enter_critical();//进入临界段,数据读写保护
	  at24c_write_byaddr(at24c_device,NV_RS485_ADDR_ADDR,(u8*)&AT24X_FLASH_BUF[NV_RS485_ADDR_ADDR],TOTAL_CFG_LEN);//再度写入预置配置参数
	  at24c_writebyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR,0X55);
	  exit_critical(); //退出临界段
}

unsigned char at24cx_databuff_get(at24c_device_st *const at24c_device)
{
	unsigned char temp;
  temp=at24c_readbyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR);
	if(0x55==temp)//已配置过参数
	{
		at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,&AT24X_FLASH_BUF[NV_EE_START_ADDR],NV_EE_SIZE);
		return 0;	
	}	   
	else//首次配置
	{
		at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,AT24X_FLASH_BUF,NV_EE_SIZE);//读0x00~0xFF存储域
		
		enter_critical();
	  at24c_write_byaddr(at24c_device,NV_RS485_ADDR_ADDR,(unsigned char*)&AT24X_FLASH_BUF[NV_RS485_ADDR_ADDR],TOTAL_CFG_LEN);
	  at24c_writebyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR,0X55);
		exit_critical();
		return 1;  
	} 
}


 












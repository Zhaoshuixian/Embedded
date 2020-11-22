
#include "nrf24l01.h"
#include "buzzer.h"
#include "led.h"


nrf24_device_st nrf24=
{
  .spi          = NRF24_SPI_BUS_CFG,
	.para         = NRF24_PARA_CFG,
	.ce_gpio_init = nrf24_ce_gpio_init,
	.irq_gpio_init= nrf24_irq_gpio_init,
	.ce_sel       = nrf24_ce,
	.irq_read     = nrf24_irq_read,
};

#define MAX_BUFF_SIZE (128) /*最大接收缓存*/
unsigned char DATA_BUFF[MAX_BUFF_SIZE]={0};

////////////////////////////////////////////////////////////////////////////
//unsigned char spi_wr(unsigned char byte)
//{
//	unsigned char bit_ctr;
//	SPI_SCK(0); 
//	for(bit_ctr=0; bit_ctr<8; bit_ctr++)
//	{
//		((byte&0x80))?(SPI_MOSI(1)):(SPI_MOSI(0));														 
//		byte<<=1;                      
//		SPI_SCK(1); 
//    byte|= SPI_MISO();   	
//		SPI_SCK(0);		
//	}	
//	return(byte);                              
//}

void nrf24_cs_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(NRF_CS_PCC,ENABLE);

	GPIO_InitStructure.GPIO_Pins = NRF_CS_PIN;   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;  
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
 	GPIO_Init(NRF_CS_PORT, &GPIO_InitStructure);	
}

void nrf24_cs(unsigned char s)
{
  NRF_CSN(s);
}

void nrf24_ce_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(NRF_CE_PCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pins = NRF_CE_PIN;   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;  
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
 	GPIO_Init(NRF_CE_PORT, &GPIO_InitStructure);	
}

void nrf24_ce(unsigned char s)
{
  NRF_CE(s);
}

void nrf24_irq_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(NRF_IRQ_PCC,ENABLE);

	GPIO_InitStructure.GPIO_Pins = NRF_IRQ_PIN;   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
 	GPIO_Init(NRF_IRQ_PORT, &GPIO_InitStructure);		
}

unsigned char nrf24_irq_read(void)
{
	return NRF_IRQ_READ();
}

/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：WRITE_IC_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
unsigned char nrf24_write_register(void *arg,unsigned char reg, unsigned char value)
{
	spi_st *const spi =(spi_st *)arg;
	
	unsigned char status;
  
	spi->cs(0);
	status = spi->wr_data(reg);				
	spi->wr_data(value);	
	spi->cs(1);
	return(status);
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：WRITE_IC_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
unsigned char nrf24_write_register_nbytes(void *arg,unsigned char reg, const unsigned char *pBuf, unsigned char bytes)
{
	
	spi_st *const spi =(spi_st *)arg;
	unsigned char status,byte_ctr;
   
	spi->cs(0);		
  status = spi->wr_data(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)  
  {	
    spi->wr_data(*pBuf++);
  }  	
	spi->cs(1);		
  return(status);       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
unsigned char nrf24_read_register(void *arg,unsigned char reg)
{
	
	spi_st *const spi =(spi_st *)arg;
	
 	unsigned char value;

	spi->cs(0);	  
	spi->wr_data(reg);	
	value = spi->wr_data(0);
	spi->cs(1);		
	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
unsigned char nrf24_read_register_nbytes(void *arg,unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	spi_st *const spi =(spi_st *)arg;
	
	unsigned char status,byte_ctr;

	spi->cs(0);		
  status = spi->wr_data(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
	{
    pBuf[byte_ctr] = spi->wr_data(0);                   //读取数据，低字节在前  
	}		
	spi->cs(1);	
	
  return(status);    
}


/********************************************************
函数功能：X24L01接收模式初始化                      
*********************************************************/
void nrf24_rx_mode(void *arg)
{	
	nrf24_device_st *const me=(nrf24_device_st*)arg;	
	
	me->ce_sel(0); 
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + RX_ADDR_P0,me->para.sub_addr,5);	// 接收设备接收通道0使用和发送设备相同的发送地址
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_AA,    0x01);               						// 使能接收通道0自动应答
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_RXADDR,0x01);           						// 使能接收通道0
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_CH,me->para.ch);                 // 选择射频通道
	nrf24_write_register(&me->spi,WRITE_IC_REG + RX_PW_P0,TX_PLOAD_WIDTH);  						// 接收通道0选择和发送通道相同有效数据宽度
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_SETUP,0x0F);            				  // 数据传输率2Mbps，发射功率7dBm
	nrf24_write_register(&me->spi,WRITE_IC_REG + CONFIG,  0x0F);              					// CRC使能，16位CRC校验，上电，接收模式
	nrf24_write_register(&me->spi,WRITE_IC_REG + STATUS,  0xFF);  											//清除所有的中断标志位	
	me->ce_sel(1); // 拉高CE启动接收设备
	me->spi.delay_ms(150);
}						

/********************************************************
函数功能：X24L01发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void nrf24_tx_mode(void *arg)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;	
	
	me->ce_sel(0); 
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + TX_ADDR,me->para.sub_addr   ,5);         //写入发送地址
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + RX_ADDR_P0,me->para.sub_addr,5);   // 为了应答接收设备，接收通道0地址和发送地址相同
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_AA,      0x01);       										          // 使能接收通道0自动应答
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_RXADDR,  0x01);   										             // 使能接收通道0
	nrf24_write_register(&me->spi,WRITE_IC_REG + SETUP_RETR, 0x0A);  											// 自动重发延时等待250us+86us，自动重发10次
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_CH,me->para.ch);         							   	// 选择射频通道
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_SETUP,   0x0F);    											// 数据传输率2Mbps，发射功率0dBm
	nrf24_write_register(&me->spi,WRITE_IC_REG + CONFIG,     0x0E);      											// CRC使能，16位CRC校验，上电
	me->spi.delay_ms(150);
//	me->ce_sel(1); 
}

/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
unsigned char nrf24_rxpacket(void *arg,unsigned char *rxbuf)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;		
	unsigned char state;
	
	state = nrf24_read_register(&me->spi,STATUS);  			                 //读取状态寄存器的值    	  
	nrf24_write_register(&me->spi,WRITE_IC_REG+STATUS,state);               //清除RX_DS中断标志

	if(state&RX_DR)								                           //接收到数据
	{
		nrf24_read_register_nbytes(&me->spi,RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //读取数据
		nrf24_write_register(&me->spi,FLUSH_RX,0xff);					              //清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;                                                   //没收到任何数据
}


/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败 
          0x20:发送成功            
          0xff:发送失败                  
*********************************************************/
unsigned char nrf24_txpacket(void *arg,unsigned char *txbuf)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;		
	unsigned char state;		
	
	me->ce_sel(0); //CE拉低，使能X24L01配置
  nrf24_write_register_nbytes(&me->spi,WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX FIFO,32个字节																			   
	me->ce_sel(1); 				  //CE置高，使能发送	
	while(1==me->irq_read());//等待发送完成
	state = nrf24_read_register(&me->spi,STATUS);  											   //读取状态寄存器的值	   
	nrf24_write_register(&me->spi,WRITE_IC_REG+STATUS, state); 					   //清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)																			                 //达到最大重发次数
	{
		nrf24_write_register(&me->spi,FLUSH_TX,0xff);										     //清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)				return TX_DS;	//发送完成

	return 0XFF;												//发送失败
}

///////////////////////////////////////////////////////////////////////////////////////
unsigned char Checksum(unsigned char sumdata[],unsigned char len)//和校验算法
{
	unsigned char i=0;
	unsigned short int  sum=0;
	for(i=0;i<len;i++)
	{
		sum+=sumdata[i];
	}
	sum&=0xff;
	return (unsigned char)sum;
}

///////////////////////////////////////////////////////////////////////////////////////////
unsigned char nrf24_check(void *arg)//检测模块（测试读写功能）
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;		
	unsigned char rBuf[5]={0};
	
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG+TX_ADDR,me->para.sub_addr,5); //
  nrf24_read_register_nbytes(&me->spi,TX_ADDR,rBuf,5);
	
	if(0==memcmp(rBuf,me->para.sub_addr,sizeof(rBuf))) return 0;

	return 1;	
}

///////////////////////////////////////////////////////////////////////////////////////////

void nrf24_init(void *arg,RF_MODE MODE)
{
	nrf24_device_st *me= (nrf24_device_st *) arg;
	
	me->spi.spi_gpio_init();
	me->spi.cs_gpio_init();	
	me->irq_gpio_init();
	me->ce_gpio_init();
	
	me->spi.cs(1);
	me->ce_sel(0);
	
	if(TX_MODE==MODE)       nrf24_tx_mode(me);
	else if(RX_MODE==MODE)  nrf24_rx_mode(me);
	
	nrf24_write_register(&me->spi,FLUSH_RX,0xFF);										    //清除RX FIFO寄存器 
	_todo :
	if(nrf24_check(me)) 
	{
		goto _todo ;
	}
}

void nrf24_transmit(void *arg,RF_MODE MODE)//射频数据发送
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;	

	if(TX_MODE==MODE)    //发送模式
	{
		DATA_BUFF[0] = 0x07;                  //---协议头:0x07
		DATA_BUFF[1] = me->para.dev_addr;    //---设备编号
		DATA_BUFF[2] = me->para.len;        //---数据有效长度
		memcpy(&DATA_BUFF[3],&me->para.data[0],me->para.len);
		me->para.checksum=Checksum(&DATA_BUFF[0],3+me->para.len);//---总检
		DATA_BUFF[me->para.len+1]=me->para.checksum;
		DATA_BUFF[me->para.len+2]=0xAA;                //---协议尾:0xAA		
		nrf24_txpacket(me,DATA_BUFF);
	}
	else if(RX_MODE==MODE)                      //接收模式
	{
		if(0==nrf24_rxpacket(me,DATA_BUFF))//接收到数据包
		{
			me->para.len        = DATA_BUFF[2];
			if(me->para.dev_addr==DATA_BUFF[1])//核对设备地址
			{
	      buzzer_start(&buzzer,1,500);//蜂鸣器鸣叫
        led_func(&led.blue,1);					
				if(0x07==DATA_BUFF[0]&&0xAA==DATA_BUFF[6])//核对帧头帧尾
				{
					me->para.checksum = Checksum(&DATA_BUFF[0],3+me->para.len);//---总检
					if(me->para.checksum==DATA_BUFF[5])//核对校验位
					{							
						memcpy(&me->para.data[0],&DATA_BUFF[3],me->para.len);
					}
				}
			}			
		}
	}	
	memset(&DATA_BUFF[0],0,sizeof(DATA_BUFF));
}

void rf_thread_entry(void *parameter)
{
	nrf24_transmit(&nrf24,RX_MODE);
}



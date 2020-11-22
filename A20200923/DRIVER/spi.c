
#include "spi.h"

/********************************************************************************************************			                                                           
*  功能: 	 初始化SPI FLASH的IO口
*  输入参数: num：需要初始化的SPI口
*  返回值：  无
*  注：这里由于芯片是通过SPI方式与W25Q128进行通信的，所以这里实际上是对SPIx的初始化，具体根据选用的SPI口进行对应的初始化
********************************************************************************************************/

void spi_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;
	SPI_InitType SPI_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1,ENABLE);	

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5 | GPIO_Pins_6 | GPIO_Pins_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pins_5|GPIO_Pins_6|GPIO_Pins_7);

	SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;	//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;		//选择了串行时钟的稳态:时钟悬空低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1EDGE;		//数据捕获于第一个时钟沿
	SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;			//NSS信号由硬件NSS管脚 还是软件用SSI位控制
	SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_4;//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CPOLY = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Enable(SPI1, ENABLE); //使能SPI外设	
}

unsigned char SPI1_ReadWriteByte(unsigned char TxData)
{		
	u16 retry=0;					 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>400)return 0;
	}			  
	SPI_I2S_TxData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>400)return 0;
	}	  						    
	return SPI_I2S_RxData(SPI1);					    
}
////////////////////////////////////////////////////////////////////////////////

unsigned char SPI2_ReadWriteByte(unsigned char TxData)
{		
	u16 retry=0;					 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>400)return 0;
	}			  
	SPI_I2S_TxData(SPI2, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>400)return 0;
	}	  						    
	return SPI_I2S_RxData(SPI2);					    
}



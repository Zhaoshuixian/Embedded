


#include "w25x16.h"


//64个块区(一个块区有16个扇区)，
//64*16个扇区(一个扇区有4K字节)	
unsigned char SPI_FLASH_BUF[1024];      //写W25Q128时用到的数据储存中间寄存器
unsigned short int W25QXX_TYPE;

/*设备注册句柄*/
w25q_device_st w25q16=
{
	.spi.spi_gpio_init = spi_gpio_init,
	.spi.cs_gpio_init  = w25q_cs_gpio_init,	
	.spi.cs        = w25q_cs,
	.spi.wr_data   = SPI1_ReadWriteByte,
	.spi.rd_data   = SPI1_ReadWriteByte,
	.spi.delay_us  = xdelay_us,
	.spi.delay_ms  = xdelay_ms,
};

void w25q_device_register(w25q_type type,w25q_device_st *const w25q_device)
{
	w25q_device->type=type;
	spi_init(&w25q_device->spi);//挂载SPI
}

unsigned char w25q_device_check(w25q_device_st *const w25q_device)
{
	#define CHECK_TIME  (1000)
	unsigned short int read_timer;
	while(w25q_device->type!=w25q_read_id(w25q_device)&&CHECK_TIME<=read_timer++)
	{
		return 1;//失败
	}
	return 0;//
}

void w25q_cs_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(W25Q_CS_PCC,ENABLE);

	GPIO_InitStructure.GPIO_Pins = W25Q_CS_PIN;   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;  
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
 	GPIO_Init(W25Q_CS_PORT, &GPIO_InitStructure);
}


/*********************W25QXX片选端，低电平有效 这个片选端需要根据具体电路修改***************************/
void w25q_cs(unsigned char s)
{
	(s)?(SPI_FLASH_LOCK):(SPI_FLASH_UNLOCK);
}
				                             							
/********************************************************************************************************			                                                           
*  功能: 	 读取芯片ID
*  输入参数: 无
*  返回值：  芯片ID【W25Q08-0XEF13，W25Q16-0XEF14，W25Q32-0XEF15，W25Q64-0XEF16，W25Q128-0XEF17】
********************************************************************************************************/	  
unsigned short int w25q_read_id(w25q_device_st *const w25q_device)
{
	static unsigned short int w_id;
	
	w25q_device->spi.cs(1);				    
	
	w25q_device->spi.wr_data(0x90);                //发送读取ID命令	    
	w25q_device->spi.wr_data(0x00); 	    
	w25q_device->spi.wr_data(0x00); 	    
	w25q_device->spi.wr_data(0x00); 	 			   
	
	w_id|=w25q_device->spi.rd_data(0xFF)<<8;  
	w_id|=w25q_device->spi.rd_data(0xFF);	 
	
	w25q_device->spi.cs(0);				    
	
	return w_id;
} 

/********************************************************************************************************			                                                           
*  功能: 	 读取W25QXX的状态寄存器
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  W25QXX的状态值【SPR RV TB BP2 BP1 BP0 WEL BUSY】SPR：              默认0,状态寄存器保护位,配合WP使用
															 TB，BP2，BP1，BP0：FLASH区域写保护设置
															 WEL：              写使能锁定
															 BUSY：             忙标记位(1,忙;0,空闲)
********************************************************************************************************/
unsigned char w25q_read_sr(w25q_device_st *const w25q_device)   
{  
	unsigned char byte=0;   
	
	w25q_device->spi.cs(1);		                   //使能器件   
	w25q_device->spi.wr_data(W25X_ReadStatusReg); //发送读取状态寄存器命令    
  byte=w25q_device->spi.rd_data(0Xff);          //读取一个字节  
	w25q_device->spi.cs(0);		                   //取消片选 
	
	return byte;   
} 

/********************************************************************************************************			                                                           
*  功能: 	 写W25QXX状态寄存器
*  输入参数: sr：待写入字节
			 num：与W25QXX相连的SPI口
*  返回值：  无
*  注：只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
********************************************************************************************************/
void w25q_write_sr(w25q_device_st *const w25q_device,unsigned char sr)   
{   
	w25q_device->spi.cs(1);		                   //使能器件    
	w25q_device->spi.wr_data(W25X_WriteStatusReg);//发送写状态寄存器命令    
	w25q_device->spi.wr_data(sr);               	//写入一个字节
	w25q_device->spi.cs(0);		                   //取消片选    	      
}   

/********************************************************************************************************			                                                           
*  功能: 	 W25QXX写使能【将WEL置位】
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_write_enable(w25q_device_st *const w25q_device)   
{
	w25q_device->spi.cs(1);		                   //使能器件   
	w25q_device->spi.wr_data(W25X_WriteEnable); 	//发送写使能
	w25q_device->spi.cs(0);		                   //取消片选    	      
} 

/********************************************************************************************************			                                                           
*  功能: 	 W25QXX写保护【将WEL清零】
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_write_disable(w25q_device_st *const w25q_device)      
{  
	w25q_device->spi.cs(1);		                   //使能器件   
	w25q_device->spi.wr_data(W25X_WriteDisable);  //发送写禁止指令
	w25q_device->spi.cs(0);		                   //取消片选  	      
} 		
  		    
/********************************************************************************************************			                                                           
*  功能: 	 读取SPI FLASH【在指定地址开始读取指定长度的数据】
*  输入参数: rdata      数据存储区
			 raddr     开始读取的地址(24bit)
		   rdata_len(最大65535)
*  返回值：  无
********************************************************************************************************/
void w25q_read_data(w25q_device_st *const w25q_device,unsigned char* rdata,unsigned int raddr,unsigned short int rdata_len)   
{ 
 	unsigned short int i;   										    
	
	w25q_device->spi.cs(1);		                   //使能器件   
	w25q_device->spi.wr_data(W25X_ReadData);         	//发送读取命令   
	w25q_device->spi.wr_data((unsigned char)((raddr)>>16));  	//发送24bit地址    
	w25q_device->spi.wr_data((unsigned char)((raddr)>>8));   
	w25q_device->spi.wr_data((unsigned char)raddr);   
	for(i=0;i<rdata_len;i++)
	{ 
		rdata[i]=w25q_device->spi.rd_data(0XFF);   	//循环读数  
	}
	w25q_device->spi.cs(0);		                   //取消片选  				    	      
}  

/********************************************************************************************************			                                                           
*  功能: 	 等待芯片完成操作
*  输入参数: 无
*  返回值：  无
********************************************************************************************************/
void w25q_busy(w25q_device_st *const w25q_device)   
{   
	while((w25q_read_sr(w25q_device)&0x01)==0x01);  		// 等待BUSY位清空
}

/********************************************************************************************************			                                                           
*  功能: 	 擦除整个芯片【擦除时间会比较长】
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_erase_total(w25q_device_st *const w25q_device)   
{                                   
	w25q_write_enable(w25q_device);                 	 	//SET WEL 
	w25q_busy(w25q_device);   

	w25q_device->spi.cs(1);                            	//使能器件    
	w25q_device->spi.wr_data(W25X_ChipErase);        	//发送片擦除命令 
	w25q_device->spi.cs(0);		                   //取消片选    

	w25q_busy(w25q_device);   				   		//等待芯片擦除结束
}  

/********************************************************************************************************			                                                           
*  功能: 	 擦除一个扇区【擦除时间最少要150ms】
*  输入参数: Dst_Addr：扇区地址【根据实际容量设置】
			 num：     与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_erase_sector(w25q_device_st *const w25q_device,unsigned int sector_addr)   
{  
 
	sector_addr*=4096;
	w25q_write_enable(w25q_device);                  	       //SET WEL 	 
	w25q_busy(w25q_device);   
	w25q_device->spi.cs(1);                            	//使能器件   

	w25q_device->spi.wr_data(W25X_SectorErase);      	    //发送扇区擦除指令 
	w25q_device->spi.wr_data((unsigned char)((sector_addr)>>16));  	   //发送24bit地址    
	w25q_device->spi.wr_data((unsigned char)((sector_addr)>>8));   
	w25q_device->spi.wr_data((unsigned char)sector_addr); 
	
	w25q_device->spi.cs(0);		                   //取消片选 	      
	w25q_busy(w25q_device);   				   		//等待擦除完成
} 

/********************************************************************************************************			                                                           
*  功能: 	 进入掉电模式
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_pd_mode(w25q_device_st *const w25q_device)   
{ 
	w25q_device->spi.cs(1);                       //使能器件    
	w25q_device->spi.wr_data(W25X_PowerDown);     //发送掉电命令
	w25q_device->spi.cs(0);		                   //取消片选 	  	      
	w25q_device->spi.delay_us(3);                 //等待TPD  
}   

/********************************************************************************************************			                                                           
*  功能: 	 唤醒芯片
*  输入参数: num：与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_wakeup(w25q_device_st *const w25q_device)   
{  
	w25q_device->spi.cs(1);                       //使能器件   
	w25q_device->spi.wr_data(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB 
	w25q_device->spi.cs(0);		                   //取消片选 	  		      
  w25q_device->spi.delay_us(3);                //等待TRES1
}   

/********************************************************************************************************			                                                           
*  功能: 	 在一页内写入SPI FLASH【在指定地址开始写入最大256字节的数据】
*  输入参数: pBuffer：      数据存储区
			 WriteAddr：    开始写入的地址(24bit)
		     NumByteToRead：要写入的字节数(最大256)【该数不应该超过该页的剩余字节数】
			 num：          与W25QXX相连的SPI口
*  返回值：  无
********************************************************************************************************/
void w25q_write_page(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len) 
{
	unsigned short int i;  

	w25q_write_enable(w25q_device);           //SET WEL 

	w25q_device->spi.cs(1);		                   //使能器件  

	w25q_device->spi.wr_data(W25X_PageProgram);      	//发送写页命令   
	w25q_device->spi.wr_data((unsigned char)((waddr)>>16)); 	//发送24bit地址    
	w25q_device->spi.wr_data((unsigned char)((waddr)>>8));   
	w25q_device->spi.wr_data((unsigned char)waddr);   

	for(i=0;i<wdata_len;i++) w25q_device->spi.wr_data(wdata[i]);//循环写数 
	w25q_device->spi.cs(0);		                   //取消片选 

	w25q_busy(w25q_device);					   		//等待写入结束
} 

/********************************************************************************************************			                                                           
*  功能: 	 无检验写SPI FLASH【在指定地址开始写入指定长度的数据，必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败！写数据时自动换页】
*  输入参数: pBuffer：       数据存储区
			 WriteAddr：     开始写入的地址(24bit)
		     NumByteToWrite：要写入的字节数(最大256)【该数不应该超过该页的剩余字节数】
*  返回值：  无
********************************************************************************************************/
void w25q_write_uncheck(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len)   
{ 			 		 
	unsigned short int pageremain;	   
	
	pageremain=256-waddr%256; //单页剩余的字节数		 	    
	
	if(wdata_len<=pageremain) pageremain=wdata_len;//不大于256个字节
		
	while(1)
	{	   
		w25q_write_page(w25q_device,wdata,waddr,pageremain);
		if(wdata_len==pageremain)
		{
			break;//写入结束了
		}
	 	else //NumByteToWrite>pageremain
		{
			wdata+=pageremain;
			waddr+=pageremain;	
			wdata_len-=pageremain;			  //减去已经写入了的字节数
			//一次可以写入256个字节////不够256个字节了
			(wdata_len>256)?( pageremain=256):(pageremain=wdata_len); 
		}
	}	    
}

/********************************************************************************************************			                                                           
*  功能: 	 写SPI FLASH【在指定地址开始写入指定长度的数据，函数先检验待写入空间是否为0xff，不是则先擦除再写入】
*  输入参数: pBuffer：       数据存储区
			 WriteAddr：     开始写入的地址(24bit)
		     NumByteToWrite：要写入的字节数(最大65535) 
*  返回值：  无
********************************************************************************************************/
void w25q_write_data(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len)  
{ 
	unsigned int secpos;
	unsigned short int secoff;
	unsigned short int secremain;	   
 	unsigned short int i;      
   
	unsigned char* W25QXX_BUF;

	W25QXX_BUF=SPI_FLASH_BUF;
 	
	secpos=waddr/4096;//扇区地址  
	secoff=waddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	
	if(wdata_len<=secremain) secremain=wdata_len;//不大于4096个字节
		
	while(1) 
	{	
		w25q_read_data(w25q_device,W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)	break;//需要擦除  
		}
		if(i<secremain)//需要擦除
		{
			w25q_erase_sector(w25q_device,secpos);		//擦除这个扇区
			for(i=0;i<secremain;i++)	   		//复制
			{
				W25QXX_BUF[i+secoff]=wdata[i];	  
			}
			w25q_write_uncheck(w25q_device,W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}
		else 
		{
			w25q_write_uncheck(w25q_device,wdata,waddr,secremain);//写已经擦除了的,直接写入扇区剩余区间.
		}			
		
		if(wdata_len==secremain) 
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 
		 	wdata+=secremain;  				//指针偏移
			waddr+=secremain;				//写地址偏移	   
		  wdata_len-=secremain;			//字节数递减
			//下一个扇区还是写不完////下一个扇区可以写完了
			(wdata_len>4096)?(secremain=4096):(secremain=wdata_len);
		}	 
	}	 
}











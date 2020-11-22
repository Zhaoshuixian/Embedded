


#include "w25x16.h"


//64������(һ��������16������)��
//64*16������(һ��������4K�ֽ�)	
unsigned char SPI_FLASH_BUF[1024];      //дW25Q128ʱ�õ������ݴ����м�Ĵ���
unsigned short int W25QXX_TYPE;

/*�豸ע����*/
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
	spi_init(&w25q_device->spi);//����SPI
}

unsigned char w25q_device_check(w25q_device_st *const w25q_device)
{
	#define CHECK_TIME  (1000)
	unsigned short int read_timer;
	while(w25q_device->type!=w25q_read_id(w25q_device)&&CHECK_TIME<=read_timer++)
	{
		return 1;//ʧ��
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


/*********************W25QXXƬѡ�ˣ��͵�ƽ��Ч ���Ƭѡ����Ҫ���ݾ����·�޸�***************************/
void w25q_cs(unsigned char s)
{
	(s)?(SPI_FLASH_LOCK):(SPI_FLASH_UNLOCK);
}
				                             							
/********************************************************************************************************			                                                           
*  ����: 	 ��ȡоƬID
*  �������: ��
*  ����ֵ��  оƬID��W25Q08-0XEF13��W25Q16-0XEF14��W25Q32-0XEF15��W25Q64-0XEF16��W25Q128-0XEF17��
********************************************************************************************************/	  
unsigned short int w25q_read_id(w25q_device_st *const w25q_device)
{
	static unsigned short int w_id;
	
	w25q_device->spi.cs(1);				    
	
	w25q_device->spi.wr_data(0x90);                //���Ͷ�ȡID����	    
	w25q_device->spi.wr_data(0x00); 	    
	w25q_device->spi.wr_data(0x00); 	    
	w25q_device->spi.wr_data(0x00); 	 			   
	
	w_id|=w25q_device->spi.rd_data(0xFF)<<8;  
	w_id|=w25q_device->spi.rd_data(0xFF);	 
	
	w25q_device->spi.cs(0);				    
	
	return w_id;
} 

/********************************************************************************************************			                                                           
*  ����: 	 ��ȡW25QXX��״̬�Ĵ���
*  �������: num����W25QXX������SPI��
*  ����ֵ��  W25QXX��״ֵ̬��SPR RV TB BP2 BP1 BP0 WEL BUSY��SPR��              Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
															 TB��BP2��BP1��BP0��FLASH����д��������
															 WEL��              дʹ������
															 BUSY��             æ���λ(1,æ;0,����)
********************************************************************************************************/
unsigned char w25q_read_sr(w25q_device_st *const w25q_device)   
{  
	unsigned char byte=0;   
	
	w25q_device->spi.cs(1);		                   //ʹ������   
	w25q_device->spi.wr_data(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������    
  byte=w25q_device->spi.rd_data(0Xff);          //��ȡһ���ֽ�  
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ 
	
	return byte;   
} 

/********************************************************************************************************			                                                           
*  ����: 	 дW25QXX״̬�Ĵ���
*  �������: sr����д���ֽ�
			 num����W25QXX������SPI��
*  ����ֵ��  ��
*  ע��ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
********************************************************************************************************/
void w25q_write_sr(w25q_device_st *const w25q_device,unsigned char sr)   
{   
	w25q_device->spi.cs(1);		                   //ʹ������    
	w25q_device->spi.wr_data(W25X_WriteStatusReg);//����д״̬�Ĵ�������    
	w25q_device->spi.wr_data(sr);               	//д��һ���ֽ�
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ    	      
}   

/********************************************************************************************************			                                                           
*  ����: 	 W25QXXдʹ�ܡ���WEL��λ��
*  �������: num����W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_write_enable(w25q_device_st *const w25q_device)   
{
	w25q_device->spi.cs(1);		                   //ʹ������   
	w25q_device->spi.wr_data(W25X_WriteEnable); 	//����дʹ��
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ    	      
} 

/********************************************************************************************************			                                                           
*  ����: 	 W25QXXд��������WEL���㡿
*  �������: num����W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_write_disable(w25q_device_st *const w25q_device)      
{  
	w25q_device->spi.cs(1);		                   //ʹ������   
	w25q_device->spi.wr_data(W25X_WriteDisable);  //����д��ָֹ��
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ  	      
} 		
  		    
/********************************************************************************************************			                                                           
*  ����: 	 ��ȡSPI FLASH����ָ����ַ��ʼ��ȡָ�����ȵ����ݡ�
*  �������: rdata      ���ݴ洢��
			 raddr     ��ʼ��ȡ�ĵ�ַ(24bit)
		   rdata_len(���65535)
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_read_data(w25q_device_st *const w25q_device,unsigned char* rdata,unsigned int raddr,unsigned short int rdata_len)   
{ 
 	unsigned short int i;   										    
	
	w25q_device->spi.cs(1);		                   //ʹ������   
	w25q_device->spi.wr_data(W25X_ReadData);         	//���Ͷ�ȡ����   
	w25q_device->spi.wr_data((unsigned char)((raddr)>>16));  	//����24bit��ַ    
	w25q_device->spi.wr_data((unsigned char)((raddr)>>8));   
	w25q_device->spi.wr_data((unsigned char)raddr);   
	for(i=0;i<rdata_len;i++)
	{ 
		rdata[i]=w25q_device->spi.rd_data(0XFF);   	//ѭ������  
	}
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ  				    	      
}  

/********************************************************************************************************			                                                           
*  ����: 	 �ȴ�оƬ��ɲ���
*  �������: ��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_busy(w25q_device_st *const w25q_device)   
{   
	while((w25q_read_sr(w25q_device)&0x01)==0x01);  		// �ȴ�BUSYλ���
}

/********************************************************************************************************			                                                           
*  ����: 	 ��������оƬ������ʱ���Ƚϳ���
*  �������: num����W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_erase_total(w25q_device_st *const w25q_device)   
{                                   
	w25q_write_enable(w25q_device);                 	 	//SET WEL 
	w25q_busy(w25q_device);   

	w25q_device->spi.cs(1);                            	//ʹ������    
	w25q_device->spi.wr_data(W25X_ChipErase);        	//����Ƭ�������� 
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ    

	w25q_busy(w25q_device);   				   		//�ȴ�оƬ��������
}  

/********************************************************************************************************			                                                           
*  ����: 	 ����һ������������ʱ������Ҫ150ms��
*  �������: Dst_Addr��������ַ������ʵ���������á�
			 num��     ��W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_erase_sector(w25q_device_st *const w25q_device,unsigned int sector_addr)   
{  
 
	sector_addr*=4096;
	w25q_write_enable(w25q_device);                  	       //SET WEL 	 
	w25q_busy(w25q_device);   
	w25q_device->spi.cs(1);                            	//ʹ������   

	w25q_device->spi.wr_data(W25X_SectorErase);      	    //������������ָ�� 
	w25q_device->spi.wr_data((unsigned char)((sector_addr)>>16));  	   //����24bit��ַ    
	w25q_device->spi.wr_data((unsigned char)((sector_addr)>>8));   
	w25q_device->spi.wr_data((unsigned char)sector_addr); 
	
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ 	      
	w25q_busy(w25q_device);   				   		//�ȴ��������
} 

/********************************************************************************************************			                                                           
*  ����: 	 �������ģʽ
*  �������: num����W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_pd_mode(w25q_device_st *const w25q_device)   
{ 
	w25q_device->spi.cs(1);                       //ʹ������    
	w25q_device->spi.wr_data(W25X_PowerDown);     //���͵�������
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ 	  	      
	w25q_device->spi.delay_us(3);                 //�ȴ�TPD  
}   

/********************************************************************************************************			                                                           
*  ����: 	 ����оƬ
*  �������: num����W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_wakeup(w25q_device_st *const w25q_device)   
{  
	w25q_device->spi.cs(1);                       //ʹ������   
	w25q_device->spi.wr_data(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB 
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ 	  		      
  w25q_device->spi.delay_us(3);                //�ȴ�TRES1
}   

/********************************************************************************************************			                                                           
*  ����: 	 ��һҳ��д��SPI FLASH����ָ����ַ��ʼд�����256�ֽڵ����ݡ�
*  �������: pBuffer��      ���ݴ洢��
			 WriteAddr��    ��ʼд��ĵ�ַ(24bit)
		     NumByteToRead��Ҫд����ֽ���(���256)��������Ӧ�ó�����ҳ��ʣ���ֽ�����
			 num��          ��W25QXX������SPI��
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_write_page(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len) 
{
	unsigned short int i;  

	w25q_write_enable(w25q_device);           //SET WEL 

	w25q_device->spi.cs(1);		                   //ʹ������  

	w25q_device->spi.wr_data(W25X_PageProgram);      	//����дҳ����   
	w25q_device->spi.wr_data((unsigned char)((waddr)>>16)); 	//����24bit��ַ    
	w25q_device->spi.wr_data((unsigned char)((waddr)>>8));   
	w25q_device->spi.wr_data((unsigned char)waddr);   

	for(i=0;i<wdata_len;i++) w25q_device->spi.wr_data(wdata[i]);//ѭ��д�� 
	w25q_device->spi.cs(0);		                   //ȡ��Ƭѡ 

	w25q_busy(w25q_device);					   		//�ȴ�д�����
} 

/********************************************************************************************************			                                                           
*  ����: 	 �޼���дSPI FLASH����ָ����ַ��ʼд��ָ�����ȵ����ݣ�����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ�ܣ�д����ʱ�Զ���ҳ��
*  �������: pBuffer��       ���ݴ洢��
			 WriteAddr��     ��ʼд��ĵ�ַ(24bit)
		     NumByteToWrite��Ҫд����ֽ���(���256)��������Ӧ�ó�����ҳ��ʣ���ֽ�����
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_write_uncheck(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len)   
{ 			 		 
	unsigned short int pageremain;	   
	
	pageremain=256-waddr%256; //��ҳʣ����ֽ���		 	    
	
	if(wdata_len<=pageremain) pageremain=wdata_len;//������256���ֽ�
		
	while(1)
	{	   
		w25q_write_page(w25q_device,wdata,waddr,pageremain);
		if(wdata_len==pageremain)
		{
			break;//д�������
		}
	 	else //NumByteToWrite>pageremain
		{
			wdata+=pageremain;
			waddr+=pageremain;	
			wdata_len-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			//һ�ο���д��256���ֽ�////����256���ֽ���
			(wdata_len>256)?( pageremain=256):(pageremain=wdata_len); 
		}
	}	    
}

/********************************************************************************************************			                                                           
*  ����: 	 дSPI FLASH����ָ����ַ��ʼд��ָ�����ȵ����ݣ������ȼ����д��ռ��Ƿ�Ϊ0xff���������Ȳ�����д�롿
*  �������: pBuffer��       ���ݴ洢��
			 WriteAddr��     ��ʼд��ĵ�ַ(24bit)
		     NumByteToWrite��Ҫд����ֽ���(���65535) 
*  ����ֵ��  ��
********************************************************************************************************/
void w25q_write_data(w25q_device_st *const w25q_device,unsigned char* wdata,unsigned int waddr,unsigned short int wdata_len)  
{ 
	unsigned int secpos;
	unsigned short int secoff;
	unsigned short int secremain;	   
 	unsigned short int i;      
   
	unsigned char* W25QXX_BUF;

	W25QXX_BUF=SPI_FLASH_BUF;
 	
	secpos=waddr/4096;//������ַ  
	secoff=waddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	
	if(wdata_len<=secremain) secremain=wdata_len;//������4096���ֽ�
		
	while(1) 
	{	
		w25q_read_data(w25q_device,W25QXX_BUF,secpos*4096,4096);//������������������
		
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)	break;//��Ҫ����  
		}
		if(i<secremain)//��Ҫ����
		{
			w25q_erase_sector(w25q_device,secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				W25QXX_BUF[i+secoff]=wdata[i];	  
			}
			w25q_write_uncheck(w25q_device,W25QXX_BUF,secpos*4096,4096);//д����������  

		}
		else 
		{
			w25q_write_uncheck(w25q_device,wdata,waddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		}			
		
		if(wdata_len==secremain) 
		{
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 
		 	wdata+=secremain;  				//ָ��ƫ��
			waddr+=secremain;				//д��ַƫ��	   
		  wdata_len-=secremain;			//�ֽ����ݼ�
			//��һ����������д����////��һ����������д����
			(wdata_len>4096)?(secremain=4096):(secremain=wdata_len);
		}	 
	}	 
}











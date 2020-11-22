
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

#define MAX_BUFF_SIZE (128) /*�����ջ���*/
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
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_IC_REG��reg��
					value:�Ĵ�����ֵ
����  ֵ��״̬�Ĵ�����ֵ
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
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_IC_REG��reg��
					pBuf:д�����׵�ַ
					bytes:д�����ֽ���
����  ֵ��״̬�Ĵ�����ֵ
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
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��READ_REG��reg��
����  ֵ���Ĵ���ֵ
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
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ��READ_REG��reg��
					pBuf:���ջ��������׵�ַ
					bytes:��ȡ�ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
unsigned char nrf24_read_register_nbytes(void *arg,unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	spi_st *const spi =(spi_st *)arg;
	
	unsigned char status,byte_ctr;

	spi->cs(0);		
  status = spi->wr_data(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
	{
    pBuf[byte_ctr] = spi->wr_data(0);                   //��ȡ���ݣ����ֽ���ǰ  
	}		
	spi->cs(1);	
	
  return(status);    
}


/********************************************************
�������ܣ�X24L01����ģʽ��ʼ��                      
*********************************************************/
void nrf24_rx_mode(void *arg)
{	
	nrf24_device_st *const me=(nrf24_device_st*)arg;	
	
	me->ce_sel(0); 
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + RX_ADDR_P0,me->para.sub_addr,5);	// �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_AA,    0x01);               						// ʹ�ܽ���ͨ��0�Զ�Ӧ��
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_RXADDR,0x01);           						// ʹ�ܽ���ͨ��0
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_CH,me->para.ch);                 // ѡ����Ƶͨ��
	nrf24_write_register(&me->spi,WRITE_IC_REG + RX_PW_P0,TX_PLOAD_WIDTH);  						// ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_SETUP,0x0F);            				  // ���ݴ�����2Mbps�����书��7dBm
	nrf24_write_register(&me->spi,WRITE_IC_REG + CONFIG,  0x0F);              					// CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
	nrf24_write_register(&me->spi,WRITE_IC_REG + STATUS,  0xFF);  											//������е��жϱ�־λ	
	me->ce_sel(1); // ����CE���������豸
	me->spi.delay_ms(150);
}						

/********************************************************
�������ܣ�X24L01����ģʽ��ʼ��                      
��ڲ�������
����  ֵ����
*********************************************************/
void nrf24_tx_mode(void *arg)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;	
	
	me->ce_sel(0); 
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + TX_ADDR,me->para.sub_addr   ,5);         //д�뷢�͵�ַ
	nrf24_write_register_nbytes(&me->spi,WRITE_IC_REG + RX_ADDR_P0,me->para.sub_addr,5);   // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_AA,      0x01);       										          // ʹ�ܽ���ͨ��0�Զ�Ӧ��
	nrf24_write_register(&me->spi,WRITE_IC_REG + EN_RXADDR,  0x01);   										             // ʹ�ܽ���ͨ��0
	nrf24_write_register(&me->spi,WRITE_IC_REG + SETUP_RETR, 0x0A);  											// �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_CH,me->para.ch);         							   	// ѡ����Ƶͨ��
	nrf24_write_register(&me->spi,WRITE_IC_REG + RF_SETUP,   0x0F);    											// ���ݴ�����2Mbps�����书��0dBm
	nrf24_write_register(&me->spi,WRITE_IC_REG + CONFIG,     0x0E);      											// CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	me->spi.delay_ms(150);
//	me->ce_sel(1); 
}

/********************************************************
�������ܣ���ȡ��������                       
��ڲ�����rxbuf:�������ݴ���׵�ַ
����  ֵ��0:���յ�����
          1:û�н��յ�����
*********************************************************/
unsigned char nrf24_rxpacket(void *arg,unsigned char *rxbuf)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;		
	unsigned char state;
	
	state = nrf24_read_register(&me->spi,STATUS);  			                 //��ȡ״̬�Ĵ�����ֵ    	  
	nrf24_write_register(&me->spi,WRITE_IC_REG+STATUS,state);               //���RX_DS�жϱ�־

	if(state&RX_DR)								                           //���յ�����
	{
		nrf24_read_register_nbytes(&me->spi,RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //��ȡ����
		nrf24_write_register(&me->spi,FLUSH_RX,0xff);					              //���RX FIFO�Ĵ���
		return 0; 
	}	   
	return 1;                                                   //û�յ��κ�����
}


/********************************************************
�������ܣ�����һ�����ݰ�                      
��ڲ�����txbuf:Ҫ���͵�����
����  ֵ��0x10:�ﵽ����ط�����������ʧ�� 
          0x20:���ͳɹ�            
          0xff:����ʧ��                  
*********************************************************/
unsigned char nrf24_txpacket(void *arg,unsigned char *txbuf)
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;		
	unsigned char state;		
	
	me->ce_sel(0); //CE���ͣ�ʹ��X24L01����
  nrf24_write_register_nbytes(&me->spi,WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX FIFO,32���ֽ�																			   
	me->ce_sel(1); 				  //CE�øߣ�ʹ�ܷ���	
	while(1==me->irq_read());//�ȴ��������
	state = nrf24_read_register(&me->spi,STATUS);  											   //��ȡ״̬�Ĵ�����ֵ	   
	nrf24_write_register(&me->spi,WRITE_IC_REG+STATUS, state); 					   //���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_RT)																			                 //�ﵽ����ط�����
	{
		nrf24_write_register(&me->spi,FLUSH_TX,0xff);										     //���TX FIFO�Ĵ��� 
		return MAX_RT; 
	}
	if(state&TX_DS)				return TX_DS;	//�������

	return 0XFF;												//����ʧ��
}

///////////////////////////////////////////////////////////////////////////////////////
unsigned char Checksum(unsigned char sumdata[],unsigned char len)//��У���㷨
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
unsigned char nrf24_check(void *arg)//���ģ�飨���Զ�д���ܣ�
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
	
	nrf24_write_register(&me->spi,FLUSH_RX,0xFF);										    //���RX FIFO�Ĵ��� 
	_todo :
	if(nrf24_check(me)) 
	{
		goto _todo ;
	}
}

void nrf24_transmit(void *arg,RF_MODE MODE)//��Ƶ���ݷ���
{
	nrf24_device_st *const me=(nrf24_device_st*)arg;	

	if(TX_MODE==MODE)    //����ģʽ
	{
		DATA_BUFF[0] = 0x07;                  //---Э��ͷ:0x07
		DATA_BUFF[1] = me->para.dev_addr;    //---�豸���
		DATA_BUFF[2] = me->para.len;        //---������Ч����
		memcpy(&DATA_BUFF[3],&me->para.data[0],me->para.len);
		me->para.checksum=Checksum(&DATA_BUFF[0],3+me->para.len);//---�ܼ�
		DATA_BUFF[me->para.len+1]=me->para.checksum;
		DATA_BUFF[me->para.len+2]=0xAA;                //---Э��β:0xAA		
		nrf24_txpacket(me,DATA_BUFF);
	}
	else if(RX_MODE==MODE)                      //����ģʽ
	{
		if(0==nrf24_rxpacket(me,DATA_BUFF))//���յ����ݰ�
		{
			me->para.len        = DATA_BUFF[2];
			if(me->para.dev_addr==DATA_BUFF[1])//�˶��豸��ַ
			{
	      buzzer_start(&buzzer,1,500);//����������
        led_func(&led.blue,1);					
				if(0x07==DATA_BUFF[0]&&0xAA==DATA_BUFF[6])//�˶�֡ͷ֡β
				{
					me->para.checksum = Checksum(&DATA_BUFF[0],3+me->para.len);//---�ܼ�
					if(me->para.checksum==DATA_BUFF[5])//�˶�У��λ
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



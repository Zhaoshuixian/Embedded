#include "24cxx.h" 


//EEPROM�洢����  
unsigned char AT24X_FLASH_BUF[NV_EE_SIZE];

/*�豸���*/
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
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
	iic_init(&at24c_device->i2c);	//����I2C
}

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
unsigned char at24c_device_check(at24c_device_st *const at24c_device)
{

	at24cx_databuff_get(at24c_device);//��ȫ�����ݲ�����־λ
	while(0x55!=at24c_readbyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR))
	{
		return 1;//ʧ��
	}
	at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,AT24X_FLASH_BUF,NV_EE_SIZE-1);//��0x00~0xFF�洢��
	return 0;
}


//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
unsigned char at24c_readbyte(at24c_device_st *const at24c_device,unsigned short int reg_addr)
{
	unsigned char temp=0;
	
  iic_start(&at24c_device->i2c);
	if(at24c_device->type>AT24C16)
	{
		iic_sendbyte(&at24c_device->i2c,0XA0); //����д����
    iic_wait_ack(&at24c_device->i2c);
		iic_sendbyte(&at24c_device->i2c,reg_addr>>8);//���͸ߵ�ַ
		iic_wait_ack(&at24c_device->i2c);		 
	}
	else 
	{
		iic_sendbyte(&at24c_device->i2c,0XA0+((reg_addr/256)<<1));   //����������ַ0XA0,д����
	}		
	iic_wait_ack(&at24c_device->i2c); 
  iic_sendbyte(&at24c_device->i2c,reg_addr%256);   //���͵͵�ַ
	iic_wait_ack(&at24c_device->i2c);	    
	iic_start(&at24c_device->i2c);  	 	   
	iic_sendbyte(&at24c_device->i2c,0XA1);           //�������ģʽ			   
	iic_wait_ack(&at24c_device->i2c);	 
  temp=iic_readbyte(&at24c_device->i2c,0);		   
  iic_stop(&at24c_device->i2c); 
	
	return temp;	
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void at24c_writebyte(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char wdata)
{				   	  	    																 
  iic_start(&at24c_device->i2c);
	if(at24c_device->type>AT24C16)
	{
		iic_sendbyte(&at24c_device->i2c,0XA0); //����д����
	iic_wait_ack(&at24c_device->i2c); 
		iic_sendbyte(&at24c_device->i2c,waddr>>8);//���͸ߵ�ַ
	iic_wait_ack(&at24c_device->i2c);  
	}
	else
	{
		iic_sendbyte(&at24c_device->i2c,0XA0+((waddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	iic_wait_ack(&at24c_device->i2c); 
  iic_sendbyte(&at24c_device->i2c,waddr%256);   //���͵͵�ַ
	iic_wait_ack(&at24c_device->i2c);  										  		   
	iic_sendbyte(&at24c_device->i2c,wdata);     //�����ֽ�							   
	iic_wait_ack(&at24c_device->i2c); 	    	   
  iic_stop(&at24c_device->i2c);//����һ��ֹͣ���� 
	iic_delayms(&at24c_device->i2c,10);	 
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void at24c_writenbytes(at24c_device_st *const at24c_device,unsigned short int waddr,unsigned char *wdata,unsigned char wdata_len)
{  	
	unsigned char t;
	for(t=0;t<wdata_len;t++)
	{
		at24c_writebyte(at24c_device,waddr+t,(*wdata>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ�
void at24c_read_byaddr(at24c_device_st *const at24c_device,unsigned short int reg_addr,unsigned char *rdata,unsigned short int rdata_len)
{
	while(rdata_len)
	{
		*rdata++=at24c_readbyte(at24c_device,reg_addr++);	
		rdata_len--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void at24c_write_byaddr(at24c_device_st *const at24c_device,unsigned char waddr,unsigned char *wdata,unsigned short int wdata_len)
{
	while(wdata_len--)
	{
		at24c_writebyte(at24c_device,waddr,*wdata);
		waddr++;
		wdata++;
	}
}

//FLASH�ָ�����Ĭ��ֵ
void at24c_default(at24c_device_st *const at24c_device)  
{
    memset(AT24X_FLASH_BUF,0xFF,NV_EE_SIZE);
	
  	at24c_write_byaddr(at24c_device,NV_EE_START_ADDR,(unsigned char*)&AT24X_FLASH_BUF,NV_EE_SIZE);//�����洢0~255

	
	  enter_critical();//�����ٽ��,���ݶ�д����
	  at24c_write_byaddr(at24c_device,NV_RS485_ADDR_ADDR,(u8*)&AT24X_FLASH_BUF[NV_RS485_ADDR_ADDR],TOTAL_CFG_LEN);//�ٶ�д��Ԥ�����ò���
	  at24c_writebyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR,0X55);
	  exit_critical(); //�˳��ٽ��
}

unsigned char at24cx_databuff_get(at24c_device_st *const at24c_device)
{
	unsigned char temp;
  temp=at24c_readbyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR);
	if(0x55==temp)//�����ù�����
	{
		at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,&AT24X_FLASH_BUF[NV_EE_START_ADDR],NV_EE_SIZE);
		return 0;	
	}	   
	else//�״�����
	{
		at24c_read_byaddr(at24c_device,NV_EE_START_ADDR,AT24X_FLASH_BUF,NV_EE_SIZE);//��0x00~0xFF�洢��
		
		enter_critical();
	  at24c_write_byaddr(at24c_device,NV_RS485_ADDR_ADDR,(unsigned char*)&AT24X_FLASH_BUF[NV_RS485_ADDR_ADDR],TOTAL_CFG_LEN);
	  at24c_writebyte(at24c_device,NV_EEPROM_HAS_CFG_ADDR,0X55);
		exit_critical();
		return 1;  
	} 
}


 












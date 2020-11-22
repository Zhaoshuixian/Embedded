
#include "spi.h"

/********************************************************************************************************			                                                           
*  ����: 	 ��ʼ��SPI FLASH��IO��
*  �������: num����Ҫ��ʼ����SPI��
*  ����ֵ��  ��
*  ע����������оƬ��ͨ��SPI��ʽ��W25Q128����ͨ�ŵģ���������ʵ�����Ƕ�SPIx�ĳ�ʼ�����������ѡ�õ�SPI�ڽ��ж�Ӧ�ĳ�ʼ��
********************************************************************************************************/

void spi_gpio_init(void)
{
	GPIO_InitType GPIO_InitStructure;
	SPI_InitType SPI_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1,ENABLE);	

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5 | GPIO_Pins_6 | GPIO_Pins_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pins_5|GPIO_Pins_6|GPIO_Pins_7);

	SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;	//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����յ͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1EDGE;		//���ݲ����ڵ�һ��ʱ����
	SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;			//NSS�ź���Ӳ��NSS�ܽ� ���������SSIλ����
	SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_4;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CPOLY = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Enable(SPI1, ENABLE); //ʹ��SPI����	
}

unsigned char SPI1_ReadWriteByte(unsigned char TxData)
{		
	u16 retry=0;					 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>400)return 0;
	}			  
	SPI_I2S_TxData(SPI1, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
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
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>400)return 0;
	}			  
	SPI_I2S_TxData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>400)return 0;
	}	  						    
	return SPI_I2S_RxData(SPI2);					    
}



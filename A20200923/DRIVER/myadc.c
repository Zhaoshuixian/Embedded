

#include "myadc.h"

#if 0


#define  ADC_CH_NUM (3)

typedef struct
{
	unsigned char  ADC_Channel;
	unsigned char  Rank; 
	unsigned char  ADC_SampleTime;
}cfg_st;

typedef struct
{
	unsigned int  convert_buff[2]; 
	cfg_st cfg;
}adc_st;

void adc_init(void *arg)
{
	
	adc_st *const me =(adc_st *)arg;
	
	DMA_InitType  DMA_InitStructure;
	ADC_InitType  ADC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1,ENABLE);	

	/* DMA channel1 configuration */
  DMA_Reset(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ((u32)0x4001244C);//����DMA�������ַ,��Ϊ���ת������ļĴ���  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//�����ڴ����ַ  
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//DMA_DIR_PeripheralDST;//����AD������Ϊ���ݴ������Դ
  DMA_InitStructure.DMA_BufferSize = 4;//ָ��DMAͨ����DMA����Ĵ�С,����Ҫ���ټ����ڴ�ռ�
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//�趨�Ĵ�����ַ�̶�  
  DMA_InitStructure.DMA_MemoryInc =DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable//�趨�ڴ��ַ�ݼӣ���ÿ��DMA���ǽ�������Ĵ����е�ֵ���������ڴ�ռ���  
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//�趨�������ݿ�� 
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//�趨�ڴ�ĵĿ�� 
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;//�趨DMA������ѭ������ģʽ  
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//�趨DMAѡ����ͨ��������ȼ� 
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA channel1 */
  //DMA_Ctrl(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_InitStructure.ADC_ScanMode =ENABLE;// DISABLE;//ENABLE;//��ͨ���ɼ�
  ADC_InitStructure.ADC_ContinuousMode = ENABLE;//�趨ADת��������ģʽ 
  ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;//��ʹ���ⲿ�ٷ�ת�� 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�ɼ��������ڼĴ������������ķ�ʽ��� 
  ADC_InitStructure.ADC_NumOfChannel = ADC_CH_NUM;//�ɼ�ͨ���� 
  ADC_Init(ADC1, &ADC_InitStructure);
	
	for(unsigned char i=0;i<ADC_CH_NUM;i++)
	{
	  ADC_RegularChannelConfig(ADC1,me->cfg.ADC_Channel, me->cfg.Rank, me->cfg.ADC_SampleTime);//ADC_IN10  //PC0--TEMP1		
	}
  /* Enable ADC1 DMA */
  ADC_DMACtrl(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Ctrl(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_RstCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCtrl(ADC1, ENABLE);	
}

#endif




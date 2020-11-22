

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
  DMA_InitStructure.DMA_PeripheralBaseAddr = ((u32)0x4001244C);//定义DMA外设基地址,即为存放转换结果的寄存器  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//定义内存基地址  
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//DMA_DIR_PeripheralDST;//定义AD外设作为数据传输的来源
  DMA_InitStructure.DMA_BufferSize = 4;//指定DMA通道的DMA缓存的大小,即需要开辟几个内存空间
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//设定寄存器地址固定  
  DMA_InitStructure.DMA_MemoryInc =DMA_MEMORYINC_ENABLE;//DMA_MemoryInc_Enable//设定内存地址递加，即每次DMA都是将该外设寄存器中的值传到三个内存空间中  
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//设定外设数据宽度 
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//设定内存的的宽度 
  DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;//设定DMA工作再循环缓存模式  
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//设定DMA选定的通道软件优先级 
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA channel1 */
  //DMA_Ctrl(DMA1_Channel1, ENABLE);
     
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_InitStructure.ADC_ScanMode =ENABLE;// DISABLE;//ENABLE;//多通道采集
  ADC_InitStructure.ADC_ContinuousMode = ENABLE;//设定AD转化在连续模式 
  ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;//不使用外部促发转换 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//采集的数据在寄存器中以左对齐的方式存放 
  ADC_InitStructure.ADC_NumOfChannel = ADC_CH_NUM;//采集通道数 
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




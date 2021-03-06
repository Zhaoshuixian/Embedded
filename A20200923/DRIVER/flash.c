
#include "flash.h"
#include "string.h"

volatile FLASH_Status FLASHStatus;
volatile uint32_t flashWriteOffset = SYS_APP_BAK_SAVE_ADDR_BASE;
volatile uint32_t flashReadOffset = SYS_APP_BAK_SAVE_ADDR_BASE;


/* MCU OTA */
/*擦除指定的Flash页*/
void flash_erase_page(uint8_t flashPage , uint32_t addr_base)
{
	FLASHStatus=FLASH_PRC_DONE;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_PRCDN|FLASH_PGRM_FLR|FLASH_WRPRT_FLR);
	FLASH_ErasePage(addr_base + flashPage*FLASH_PAGE_SIZE);
	FLASH_Lock();
}
void flash_erase(uint32_t size , uint32_t addr_base)
{
    uint32_t flashPageSum;
		uint32_t i;
    /*如果小于1024做处理*/
    if(size < FLASH_PAGE_SIZE)
        size = FLASH_PAGE_SIZE;
    /* 计算需要擦写的Flash页 */
    if((size % FLASH_PAGE_SIZE) == 0)
    {
        flashPageSum = size / FLASH_PAGE_SIZE;
    }
    else
    {
        flashPageSum = (size / FLASH_PAGE_SIZE) + 1;
    }
    for(i = 0;i<flashPageSum;i++)
    {
        flash_erase_page(i,addr_base);
    }
}
void writeFlash(uint16_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
    uint16_t count=0;
		FLASHStatus=FLASH_PRC_DONE;
    if(wFlashAddr >= 0x08020000)
    {
#ifdef DEBUG
        printf("Waring:Flash Write Addr Error\r\n");
#endif
        flashWriteOffset = SYS_APP_BAK_SAVE_ADDR_BASE;
        return;
    }
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_PRCDN|FLASH_PGRM_FLR|FLASH_WRPRT_FLR);/////ȥԽFlashәطքҪ־λ	 
	while(count < len && FLASHStatus == FLASH_PRC_DONE)
	{
		FLASH_ProgramHalfWord((wFlashAddr + count*2),buf_to_save[count]); //вflashһٶַ֘дɫѫؖè16λé	
		count ++;     
	}
	FLASH_Lock();
}

void readFlash(uint16_t * buf_to_get,uint16_t len , uint32_t readFlashAddr)
{
	uint16_t count=0;
	while(count<len)
	{
	 	buf_to_get[count]=*(uint16_t *)(readFlashAddr + count*2);
		count++;
	}
}
/*写Flash,控制写长度,Flash地址偏移*/
void wFlashData(uint8_t * buf_to_save , uint16_t len , uint32_t wFlashAddr)
{
		uint16_t i ;
    uint8_t WriteFlashTempBuf[PIECEMAXLEN];//写Flash临时缓冲区
    uint16_t WriteFlashTempLen = 0;//写Flash长度
    memset(WriteFlashTempBuf,0xEE,sizeof(WriteFlashTempBuf));//写Flash临时缓冲区首先全部填充0xEE
		for(i = 0;i<len;i++)
		{
			WriteFlashTempBuf[i] = buf_to_save[i];
		}
//    memcpy(WriteFlashTempBuf,buf_to_save,len);//临时缓冲区
    WriteFlashTempLen = len;
    if(len%2 != 0)  WriteFlashTempLen += 1;//因为Flash只能写半字
    writeFlash((uint16_t *)&WriteFlashTempBuf ,  WriteFlashTempLen/2 , wFlashAddr);
}
void rFlashData(uint8_t * buf_to_get , uint16_t len , uint32_t rFlashAddr)
{
//    uint8_t ReadFlashTempBuf[PIECEMAXLEN];//写Flash临时缓冲区
    uint16_t ReadFlashTempLen = 0;//写Flash长度
    
    if(len%2 == 0)
    {
        ReadFlashTempLen = len;
//        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
//        memcpy(buf_to_get,ReadFlashTempBuf,len);
				readFlash((uint16_t *)buf_to_get,ReadFlashTempLen/2 , rFlashAddr);
    }
    else
    {
        ReadFlashTempLen = len + 1;//因为Flash只能读半字
//        readFlash((uint16_t *)&ReadFlashTempBuf,ReadFlashTempLen/2 , rFlashAddr);
//        memcpy(buf_to_get,ReadFlashTempBuf,len);
				readFlash((uint16_t *)buf_to_get,ReadFlashTempLen/2 , rFlashAddr);
    }
}
/****IAP*****/
typedef  void (*iapfun)(void);				//֨ӥһٶگ˽`эքӎ˽.
iapfun jump2app; 
uint16_t iapbuf[1024];   

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 


//设置栈顶指针
__asm void MSR_MSP(uint32_t addr) 
{
    MSR MSP, r0 			//set Main    Stack value
    BX r14
}
void iap_load_app(uint32_t appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//ݬөջַ֥֘ˇرۏר.
	{ 
		#if DEBUG
		printf("Stack Success!\r\n");
		#endif
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//ԃۧպëȸ֚׾ٶؖΪԌѲߪʼַ֘(شλַ֘)		
		MSR_MSP(*(vu32*)appxaddr);					//ԵʼۯAPPבջָ֫(ԃۧպëȸք֚һٶؖԃԚզ؅ջַ֥֘)
		jump2app();									//͸תսAPP.
	}
	else
	{
		#if DEBUG
		printf("Stack Failed!\r\n");
		#endif
	}
}


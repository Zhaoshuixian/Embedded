
#include "cs5460.h"
#include "24cxx.h"
#include "task.h"
#include "autom.h"
#include "delay.h"

spi_st cs5460a_spi=
{
	.spi_gpio_init = spi_gpio_init,
	.cs_gpio_init  = cs54060a_cs_gpio_init,	
	.cs        = cs5460a_cs,
	.wr_data   = SPIx_ReadWriteByte,
	.rd_data   = SPIx_ReadWriteByte,
	.delay_us  = delay_us,
	.delay_ms  = delay_ms,
};

u8	  g_ulCS5460UpdataFlag = 0x01;//CS5460�¼�BIT0 = 1=���ڣ�0=������																			 

meter_st cs5460a;

static u8 CS5460A_DATABUFF[5] = {0};


#define  NEED_POWER_FUNC

#if defined NEED_POWER_FUNC /*���ʲ�����Ҫ���й���*/
#else
 #define  NEED_AMP_FUNC  /*������������������*/
#endif

void cs5460a_cs(unsigned char s)
{
	(s)?(EN_5460):(UN_5460);
}

void meter_thread_entry(void *parameter)
{
	if((g_ulCS5460UpdataFlag&&0x01) == 0x01)//CS5460����
	{
		cs5460_meter_read(&cs5460a);
	}
	else
	{
		cs5460a_check();//CS5460�Ƿ����
		memset(&cs5460a,0x00,sizeof(cs5460a));
	}	
}

void cs5460_meter_read(meter_st *const cs5460a_me)
{
	cs5460a_check();//CS5460�Ƿ����
#if defined (NEED_VOLT_FUNC)|| defined (NEED_POWER_FUNC)
	double volt_now=0;	
	volt_now = cs5460a_fliter_get(CS_VRMS);
	if( (volt_now-cs5460a_me->volt_rms>0.03)||(cs5460a_me->volt_rms-volt_now>0.03) )
	{
		cs5460a_me->volt_rms = volt_now*100;		
		if(((cs5460a_me->volt_rms/100)==0)&&((cs5460a_me->volt_rms%100)<0x30)) //�����Ǳ������
		{
			volt_now = 0;
			cs5460a_me->volt_rms = 0;
		}
		else
		{
			cs5460a_me->volt_rms = volt_now*100; 
		}
	}
#endif
#if defined (NEED_AMP_FUNC) || defined (NEED_POWER_FUNC)
	double amp_now=0;
	amp_now = cs5460a_fliter_get(CS_IRMS);
	if( (amp_now-cs5460a_me->amp_rms>0.003)||(cs5460a_me->amp_rms-amp_now>0.003) )
	{
		cs5460a_me->amp_rms=amp_now*100;		
		if(((cs5460a_me->amp_rms/100)==0)&&((cs5460a_me->amp_rms%100)<0x30))//�����Ǳ������ 
		{
			cs5460a_me->amp_rms=amp_now*100;
		}
		else
		{
			cs5460a_me->amp_rms=amp_now*100; 
		}
	}
#endif
#if defined (NEED_POWER_FUNC)	
	double atcpwr_now=0;
	atcpwr_now = cs5460a_fliter_get(CS_PRMS);
	if( (atcpwr_now-cs5460a_me->actpwr_rms>0.003)||(cs5460a_me->actpwr_rms-atcpwr_now>0.003) )
	{   
		if(atcpwr_now>1) //�����Ǳ������ 
		{
			cs5460a_me->actpwr_rms=atcpwr_now*100;
		  cs5460a_me->energy_w = cs5460a_me->actpwr_rms;//�й�����
			cs5460a_me->energy_w = ((cs5460a_me->volt_rms/100)*(cs5460a_me->amp_rms/100))/10;//���ڹ���
		}
		else
		{
			atcpwr_now = 0;
			cs5460a_me->actpwr_rms= 0;
			cs5460a_me->energy_w = 0;
		}
	}
#endif
	CS5460IRQHandler();// ���CS5460�����Ƿ�����
}

//////////////////////////////////////////////////////////////////////////////////////
void  cs5460a_check(void) // CS5460�Լ�
{
	cs5460a_rddata(STATUS,CS5460A_DATABUFF); //��״̬�Ĵ���
	if(((CS5460A_DATABUFF[1] == 0x00)&&(CS5460A_DATABUFF[2] == 0x00)&&(CS5460A_DATABUFF[3] == 0x00))||
	((CS5460A_DATABUFF[1] == 0xff)&&(CS5460A_DATABUFF[2] == 0xff)&&(CS5460A_DATABUFF[3] == 0xff))||
	((CS5460A_DATABUFF[1] == 0xfe)&&(CS5460A_DATABUFF[2] == 0xfe)&&(CS5460A_DATABUFF[3] == 0xfe)))
	{
		g_ulAutoControl.dis.seg_show[4] = 0x0E;
		g_ulAutoControl.dis.seg_show[5] = 0x05;	
		g_ulCS5460UpdataFlag &=~0x01;			
		CS5460IRQHandler();//���CS5460�����Ƿ�����
	}
	else
	{
		//���ܼ�����ϴ������
		g_ulCS5460UpdataFlag |= 0x01;
	}	
}
void CS5460IRQHandler(void)//���CS5460�����Ƿ�����
{
	int databuf = 0;
	u8 s_cBuf[5] = {0};
	cs5460a_rddata(STATUS,CS5460A_DATABUFF); //��״̬�Ĵ���
	databuf |= CS5460A_DATABUFF[1]; 
	databuf = databuf<<16;
	databuf |= CS5460A_DATABUFF[2]; 
	databuf = databuf<<8;
	databuf |= CS5460A_DATABUFF[3]; 
	if((databuf&0x000020) == 0x000020)//CS5460��WDT�����¼�����
	{		
		cs5460a_rddata(READ|LAST_ENERGY,CS5460A_DATABUFF);//�ȶ�ȡ�����Ĵ���
		
		s_cBuf[3] = CS5460A_DATABUFF[3]; //L
		s_cBuf[2] = CS5460A_DATABUFF[2]; //M
		s_cBuf[1] = CS5460A_DATABUFF[1]; //H
		s_cBuf[0] = (WRITE|STATUS);
		cs5460a_wrdata(s_cBuf,4); //д״̬�Ĵ���
		cs5460a_rddata(STATUS,CS5460A_DATABUFF); //��״̬�Ĵ���
	}
}//////////////////////////////////////////////////////////////////
double cs5460a_fliter_get(u8 CS)//CS5460����˲�
{
   u8 count,i,j;
   double value_buf[N],temp;
   double  sum=0;
   for(count=0;count<N;count++)
   {
		if(CS==CS_VRMS)
		{
			value_buf[count] = cs5460a_rmsvolt_get();
		}
		else if(CS==CS_IRMS)
		{
			value_buf[count] = cs5460a_rmsamp_get();
		}
		else
			value_buf[count] = cs5460a_activepwr_get();
   } 
   for (j=0;j<N-1;j++)
   {
      for (i=0;i<N-j-1;i++)
      {
         if( value_buf[i]>value_buf[i+1])
         {
            temp = value_buf[i];
            value_buf[i] = value_buf[i+1];
            value_buf[i+1] = temp;
         }
      }
   }
   for(count=4;count<N-4;count++)
   {
   		sum += value_buf[count];
   }
   return (sum/(N-8));
}//////////////////////////////////////////////////////////////

double cs5460a_activepwr_get(void)//CS5460������
{
	float ENERGY;
	int j=0,i=0;
	float G=1.0/2.0, temp=0.0;
 	cs5460a_rddata(READ|LAST_ENERGY,CS5460A_DATABUFF);
// ��ΪI,V,P,E���з������������Ҫת��
	if(CS5460A_DATABUFF[1]&0x80)  //����Ϊ����
	{
		CS5460A_DATABUFF[1] = ~CS5460A_DATABUFF[1];
		CS5460A_DATABUFF[2] = ~CS5460A_DATABUFF[2];
		CS5460A_DATABUFF[3] = ~CS5460A_DATABUFF[3];			
		g_ulCS5460UpdataFlag |= 0x02;//��λ���ʷ���
	}
	else
	{
		CS5460A_DATABUFF[1] = CS5460A_DATABUFF[1];
		CS5460A_DATABUFF[2] = CS5460A_DATABUFF[2];
		CS5460A_DATABUFF[3] = CS5460A_DATABUFF[3];		
		g_ulCS5460UpdataFlag &= 0xfd;//��λ���ʷ���
	}
	for(i=1;i<=3;i++)
	{
		for(j=7;j>=0;j--)
		{
			if( (CS5460A_DATABUFF[i]&(1<<j)) != 0 )
				temp+=G;
			G = G/2.0;
		}
	}
   ENERGY = ((7500*temp)/0.38)*2; //����*����ֵ/0.6//����ΪI,V,P,E���з������������λռ��һλ�����Գ���2
	return ENERGY;
}
////////////////////////////////////////////////////////////////
double cs5460a_rmsvolt_get(void)//CS5460����ѹ
{
  float VoltRMS;
	int j=0,i=0;
	float G=1.0/2.0,temp=0.0;
	cs5460a_rddata(READ|LAST_VOLT_RMS,CS5460A_DATABUFF);
	for(i=1;i<=3;i++)
	{
		for(j=7;j>=0;j--)
		{
			if( (CS5460A_DATABUFF[i]&(1<<j)) != 0 )
				temp+=G;
			G = G/2.0;
		}
	}
	VoltRMS = (250*temp)/0.75; //����*����ֵ/0.6
	return VoltRMS;
}//////////////////////////////////////////////////////////////////

double cs5460a_rmsamp_get(void)//CS5460������
{
	float CurrentRMS;
	int j=0,i=0;
	float G=1.0/2.0,temp=0.0;
	cs5460a_rddata(READ|LAST_CURRENT_RMS,CS5460A_DATABUFF);
	for(i=1;i<=3;i++)
	{
		for(j=7;j>=0;j--)
		{
			if( (CS5460A_DATABUFF[i]&(1<<j)) != 0 )
				temp+=G;
			G = G/2.0; //���渡����
		}
	}
	CurrentRMS =(30000*temp)/0.6 ;	//����*����ֵ/0.6
	return CurrentRMS;
}/////////////////////////////////////////////////////////////////

void cs5460a_wait_sr(void)//���DRDYλ�Ƿ���λ
{
	u16	i;	
	for( i = 50000; i > 0; i--)//
	{
		cs5460a_rddata(0x1e,CS5460A_DATABUFF); 
		if(( CS5460A_DATABUFF[1]&0x80) != 0 )//�ж�
			i = 1;//�˳�
		else
			delay_us(1);//�ȴ�
	}
}/////////////////////////////////////////////////////////////////
void cs5460a_clear_sr(void)//CS5460��״̬�Ĵ���
{
    CS5460A_DATABUFF[0]=0x5e; 
    CS5460A_DATABUFF[1]=0xff; 
    CS5460A_DATABUFF[2]=0xff; 
    CS5460A_DATABUFF[3]=0xff; 
    cs5460a_wrdata(CS5460A_DATABUFF,4); 
}//////////////////////////////////////////////////////////////////
void cs5460a_rddata(u8 reg,u8 *ptr)//CS5460������ֽ�
{
	s8 i=0; 
	cs5460a_spi.cs(1); 
	cs5460a_spi.wr_data(reg);	  
	cs5460a_spi.cs(0);
	cs5460a_spi.cs(1); //ʹ��ƬѡCS5460
	for(i=3;i>0;i--)
	{
		*ptr=cs5460a_spi.rd_data(SYNC0); 
		ptr++;
	}
	cs5460a_spi.cs(0); 
}///////////////////////////////////////////////////////////////
void cs5460a_wrdata(u8 *ptr,u8 n)//CS5460д����ֽ�
{
	s8 j=0;
	cs5460a_spi.cs(1); 
	for(j=n;j>=1;j--)
	{  
		cs5460a_spi.wr_data(*ptr);	  
		ptr++;
	}
	cs5460a_spi.cs(0); 
}///////////////////////////////////////////////////////////////
void cs540a_calibmode(u8 mode)//У׼CS5460����
{
	scheduler_stop();
	cs5460a_clear_sr() ;
	cs5460a_wait_sr() ;
	cs5460a_clear_sr();
	CS5460A_DATABUFF[0]=0xa0;
	cs5460a_wrdata(CS5460A_DATABUFF,1);//����POWER_HALT ����
	switch(mode) 
	{
	 	case 0: break;
		case 1://����ƫ��У׼
			 //��һ�׶Σ�����ƫ��У׼����������������ѹ����̽ӣ�
			cs5460a_clear_sr();			
			CS5460A_DATABUFF[0]=0xcd ;//��������ƫ��У׼��
			cs5460a_wrdata(CS5460A_DATABUFF,1); //дУ׼����Ĵ���
			cs5460a_wait_sr(); //�ȴ�У׼���
			cs5460a_rddata(0x20,CS5460A_DATABUFF); //��Iacoff �Ĵ���
			at24c_write_byaddr(&at24c02,NV_METER_CURR_BIAS_CALIB_ADDR,(u8*)CS5460A_DATABUFF,NV_METER_CURR_BIAS_CALIB_LEN);//��*�ֽڴ���ʼ,д��SIZE���ȵ����� 
			cs5460a_clear_sr();
			CS5460A_DATABUFF[0]=0xd5 ;//��ѹ����ƫ��У׼
			cs5460a_wrdata(CS5460A_DATABUFF,1); //дУ׼����Ĵ���    
			cs5460a_wait_sr();     //�ȴ�У׼���
			cs5460a_rddata(0x22,CS5460A_DATABUFF); //����ѹƫ�üĴ���
			at24c_write_byaddr(&at24c02,NV_METER_VOLT_BIAS_CALIB_ADDR,(u8*)CS5460A_DATABUFF,NV_METER_VOLT_BIAS_CALIB_LEN);//��*�ֽڴ���ʼ,д��SIZE���ȵ����� 
				break;
		case 2:
				break;
		case 3: //��������Ĵ��� 
			 cs5460a_clear_sr();
			 CS5460A_DATABUFF[0]=0xce;//������������У׼��
			 cs5460a_wrdata(CS5460A_DATABUFF,1); //дУ׼�������
			 cs5460a_wait_sr(); //�ȴ�У׼���
			 cs5460a_rddata(0x04,CS5460A_DATABUFF);
			 at24c_write_byaddr(&at24c02,NV_METER_CURR_GAIN_CALIB_ADDR,(u8*)CS5460A_DATABUFF,NV_METER_CURR_GAIN_CALIB_LEN);//��*�ֽڴ���ʼ,д��SIZE���ȵ����� 
			 cs5460a_clear_sr();
			 CS5460A_DATABUFF[0]=0xd6;//��ѹ��������У׼
			 cs5460a_wrdata(CS5460A_DATABUFF,1); //дУ׼�������
			 cs5460a_wait_sr(); //�ȴ�У׼���
			 cs5460a_rddata(0x08,CS5460A_DATABUFF); 
			 at24c_write_byaddr(&at24c02,NV_METER_VOLT_GAIN_CALIB_ADDR,(u8*)CS5460A_DATABUFF,NV_METER_VOLT_GAIN_CALIB_LEN);//��*�ֽڴ���ʼ,д��SIZE���ȵ����� 
				break;
		case 4:	
				break;
		default:
				break;
	}
  scheduler_start();
}////////////////////////////////////////////////////////////////////
void  cs5406a_finecalib(u8 *finedata)//��У׼��������ϸ��
{
	u32 edata=0,udata=0;
	u8 errflag=0;
	at24c_read_byaddr(&at24c02,NV_EE_START_ADDR,AT24X_FLASH_BUF,NV_EE_SIZE-1);//������������������
	if(finedata[5]==0x00)//����+++++++++++++++++++
	{
		if(finedata[6]==0x01)//����ƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata+udata)>=(edata+0X1FFFF)) 
				errflag=1;//�д���
			else
				edata += udata;					
		}
		else if(finedata[6]==0x02)//��ѹƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata+udata)>=(edata+0X1FFFF)) 
				errflag=1;//�д���
			else
				edata += udata;		
		}else if(finedata[6]==0x03)//��������
		{
			edata = AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata+udata)>=(edata+0X1FFFF)) 
				errflag=1;//�д���
			else
				edata += udata;
		}
		else if(finedata[6]==0x04)//��ѹ����
		{
			edata = AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata+udata)>=(edata+0X1FFFF)) 
				errflag=1;//�д���
			else
				edata += udata;	
		}
		else if(finedata[6]==0x05)//����ƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata+udata)>=(edata+0X1FFFF)) 
				errflag=1;//�д���
			else
				edata += udata;		
		}
		else
			errflag=1;//�д���
	}
	else if(finedata[5]==0x01)//����-----------------------------------
	{
		if(finedata[6]==0x01)//����ƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata-udata)<=(edata-0X1FFFF)) 
				errflag=1;//�д���
			else
				edata -= udata;
		}
		else if(finedata[6]==0x02)//��ѹƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata-udata)<=(edata-0X1FFFF)) 
				errflag=1;//�д���
			else
				edata -= udata;		
		}
		else if(finedata[6]==0x03)//��������
		{
			edata = AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata-udata)<=(edata-0X1FFFF)) 
				errflag=1;//�д���
			else
				edata -= udata;		
		}
		else if(finedata[6]==0x04)//��ѹ����
		{
			edata = AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata-udata)<=(edata-0X1FFFF)) 
				errflag=1;//�д���
			else
				edata -= udata;		
		}
		else if(finedata[6]==0x05)//����ƫ��
		{
			edata = AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+1];
			edata <<= 8;
			edata |= AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+2];
			udata = finedata[7];
			udata <<= 8;
			udata |= finedata[8];
			if((edata-udata)<=(edata-0X1FFFF)) 
				errflag=1;//�д���
			else
				edata -= udata;
		}
		else
			errflag=1;//�д���
	}
	else
	{
		errflag=1;//�д���		
	}
	if(errflag==0)
	{
		AT24X_FLASH_BUF[NV_METER_DATA_ERROR_ADDR] = (u8)(edata>>16);
		AT24X_FLASH_BUF[NV_METER_DATA_ERROR_ADDR+1] = (u8)(edata>>8);  
		AT24X_FLASH_BUF[NV_METER_DATA_ERROR_ADDR+2] = (u8)(edata); 		
		at24c_write_byaddr(&at24c02,NV_METER_DATA_ERROR_ADDR,(u8*)&AT24X_FLASH_BUF[NV_METER_DATA_ERROR_ADDR],NV_METER_DATA_ERROR_LEN);
		cs5460a_reboot();//���ɼ���ʼ��
	}
}////////////////////////////////////////////////////////////
void cs5460a_phasecompens(u8 data)//��λ����
{
	u8 ucharr;	
	ucharr = data;	
	if(ucharr>70) ucharr=70;//�ж�
	AT24X_FLASH_BUF[NV_METER_COMPENSATION_ADDR] = ucharr; 		
	at24c_write_byaddr(&at24c02,NV_METER_COMPENSATION_ADDR,(u8*)&AT24X_FLASH_BUF[NV_METER_COMPENSATION_ADDR],NV_METER_COMPENSATION_LEN);
	cs5460a_reboot();//���ɼ���ʼ��
}////////////////////////////////////////////////////////////
void cs5460a_reboot(void)//CS5460����
{
  scheduler_stop();
	cs5460a_config_init();//���³�ʼ��
  scheduler_start();
}
//------------------------------------------
#define NV_DATA_CALIB 
void cs5460a_config_init(void)//CS5460��ʼ��
{
	u8 Buf[5] = {0};
	
	CS5460A_RESET(0);
	delay_ms(20);
	CS5460A_RESET(1);
	delay_ms(25);
	
	CS5460A_DATABUFF[0]=0xff;            //SYNC1 
	CS5460A_DATABUFF[1]=0xff;            //SYNC1 
	CS5460A_DATABUFF[2]=0xff;            //SYNC1 
	CS5460A_DATABUFF[3]=0xff;            //SYNC1 
	cs5460a_wrdata(CS5460A_DATABUFF,4);   //  
	
	CS5460A_DATABUFF[0]=0xff;            //SYNC1 
	CS5460A_DATABUFF[1]=0xff;            //SYNC1 
	CS5460A_DATABUFF[2]=0xff;            //SYNC1 
	CS5460A_DATABUFF[3]=0xfe;            //SYNC0 
	cs5460a_wrdata(CS5460A_DATABUFF,4);   // 3 1 1 0 
	
	at24c_read_byaddr(&at24c02,0,AT24X_FLASH_BUF,NV_EE_SIZE-1);//������������������

  CS5460A_DATABUFF[0]=WRITE|CURRENT_AC_OFFSET;    
#if defined (NV_DATA_CALIB)	
	CS5460A_DATABUFF[1]=AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR];          
	CS5460A_DATABUFF[2]=AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+1];
	CS5460A_DATABUFF[3]=AT24X_FLASH_BUF[NV_METER_CURR_BIAS_CALIB_ADDR+2];
#else
	CS5460A_DATABUFF[1]=0x00;          
	CS5460A_DATABUFF[2]=0x03;
	CS5460A_DATABUFF[3]=0x90;
#endif
	cs5460a_wrdata(CS5460A_DATABUFF,4);   //���� ����ƫ�� У׼�Ĵ���
	CS5460A_DATABUFF[0]=WRITE|VOLT_AC_OFFSET;
#if defined (NV_DATA_CALIB)		
	CS5460A_DATABUFF[1]=AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR];      
	CS5460A_DATABUFF[2]=AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+1];
	CS5460A_DATABUFF[3]=AT24X_FLASH_BUF[NV_METER_VOLT_BIAS_CALIB_ADDR+2];
#else	
	CS5460A_DATABUFF[1]=0x00;          
	CS5460A_DATABUFF[2]=0xF0;
	CS5460A_DATABUFF[3]=0x00;
#endif	
	cs5460a_wrdata(CS5460A_DATABUFF,4);   //��ѹ ����ƫ�� У׼�Ĵ���
	
	CS5460A_DATABUFF[0]=WRITE|I_GAIN;   
#if defined (NV_DATA_CALIB)		
	CS5460A_DATABUFF[1]=AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR];
	CS5460A_DATABUFF[2]=AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+1];
	CS5460A_DATABUFF[3]=AT24X_FLASH_BUF[NV_METER_CURR_GAIN_CALIB_ADDR+2];
#else	
	CS5460A_DATABUFF[1]=0x92;          
	CS5460A_DATABUFF[2]=0xF0;
	CS5460A_DATABUFF[3]=0x00;
#endif		
	cs5460a_wrdata(CS5460A_DATABUFF,4);     //��������У׼�Ĵ���
	
	CS5460A_DATABUFF[0]=WRITE|V_GAIN; 
#if defined (NV_DATA_CALIB)		
	CS5460A_DATABUFF[1]=AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR];
	CS5460A_DATABUFF[2]=AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+1];
	CS5460A_DATABUFF[3]=AT24X_FLASH_BUF[NV_METER_VOLT_GAIN_CALIB_ADDR+2];
#else	
	CS5460A_DATABUFF[1]=0x4D;          
	CS5460A_DATABUFF[2]=0xD0;
	CS5460A_DATABUFF[3]=0x00;
#endif			
	cs5460a_wrdata(CS5460A_DATABUFF,4);     //��ѹ����У׼�Ĵ���

	//����ƫ��У׼�Ĵ����ǲ����ʾ,����ʱ��Ҫ���в�������,��������Ҫ����	
	CS5460A_DATABUFF[0]=WRITE|P_OFF; 
#if defined (NV_DATA_CALIB)			
	CS5460A_DATABUFF[1]=AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR];
	CS5460A_DATABUFF[2]=AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+1];
	CS5460A_DATABUFF[3]=AT24X_FLASH_BUF[NV_METER_PWR_BIAS_CALIB_ADDR+2];
#else	
	CS5460A_DATABUFF[1]=0x00;          
	CS5460A_DATABUFF[2]=0x00;
	CS5460A_DATABUFF[3]=0x00;
#endif	
	cs5460a_wrdata(CS5460A_DATABUFF,4);      //����ƫ��У׼�Ĵ���

	CS5460A_DATABUFF[0]=(WRITE|CS5460A_CONFIG);;         
	CS5460A_DATABUFF[1]=00;            	//GI=0  ����ͨ������=10(250mv); GI=1  ����ͨ������=50 (50mv)  
	CS5460A_DATABUFF[2]=0x18;				//�ж���ʽ:00-�ߵ�ƽ  08-�͵�ƽ  10-�½���  18-������          
	CS5460A_DATABUFF[3]=0x61;           //DCLK=MCLK/1,��ͨ   
// CS5460A_DATABUFF[3]=0x21;       		//DCLK=MCLK/1,ȫͨ    
	cs5460a_wrdata(CS5460A_DATABUFF,4);  //д���üĴ���
     
	CS5460A_DATABUFF[0]=(WRITE|STATUS);          
	CS5460A_DATABUFF[1]=0xFF; 
	CS5460A_DATABUFF[2]=0xFF; 
	CS5460A_DATABUFF[3]=0xFF; 
	cs5460a_wrdata(CS5460A_DATABUFF,4);  //�����ͣ����״̬�Ĵ���

	CS5460A_DATABUFF[0]=(WRITE|MASK);                          
	CS5460A_DATABUFF[1]=0x00; 	   		//��У׼��������ʹ��DRDY=1 У׼������DRDY=0 Ĭ�ϣ�
	CS5460A_DATABUFF[2]=0x00; 
	CS5460A_DATABUFF[3]=0x00; 
	cs5460a_wrdata(CS5460A_DATABUFF,4);  //д�ж����μĴ���,ȱʡֵ

	CS5460A_DATABUFF[0]=(WRITE|CTRL);                          
	CS5460A_DATABUFF[1]=0x00; 
	CS5460A_DATABUFF[2]=0x00; 
	CS5460A_DATABUFF[3]=0x00; 
	cs5460a_wrdata(CS5460A_DATABUFF,4);  //д���ƼĴ���,ȱʡֵ

	CS5460A_DATABUFF[0]=(WRITE|PULSE_RATE);
	CS5460A_DATABUFF[1]=0x00;
	CS5460A_DATABUFF[2]=0x00;
	CS5460A_DATABUFF[3]=0xBE;//1026�������ʾ1KW.H�ȵ�
	cs5460a_wrdata(CS5460A_DATABUFF,4); //дEOUT ��������Ĵ���

	CS5460A_DATABUFF[0]=(WRITE|CYCLE_COUNT);//д���ڼ����Ĵ���                          
	CS5460A_DATABUFF[1]=0x00; 
	CS5460A_DATABUFF[2]=0x0f; 
	CS5460A_DATABUFF[3]=0xa0; 			//ÿ���Ӽ��� 1�Σ�N=4000
	cs5460a_wrdata(CS5460A_DATABUFF,4);  

	cs5460a_rddata(STATUS,CS5460A_DATABUFF); //��״̬�Ĵ���
	Buf[3] = CS5460A_DATABUFF[3];	   	//L
	Buf[2] = CS5460A_DATABUFF[2];	   	//M
	Buf[1] = CS5460A_DATABUFF[1];	  		//H
	Buf[0] = (WRITE|STATUS);
	cs5460a_wrdata(Buf,4); //д״̬�Ĵ���

	Buf[0]=CS5460A_M_START;
	cs5460a_wrdata(Buf,1); //����CS5460A
}

void cs54060a_cs_gpio_init(void)
{
	GPIO_InitType  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(CS5460A_CS_PCC, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pins = CS5460A_CS_PIN;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		//�������
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_Init(CS5460A_CS_PORT, &GPIO_InitStructure);
}

void cs5460a_gpio_init(void)//�˿ڳ�ʼ��
{
	GPIO_InitType  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(CS5460A_RESET_PCC, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pins = CS5460A_RESET_PIN;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		//�������
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	
	GPIO_Init(CS5460A_RESET_PORT, &GPIO_InitStructure);	
	CS5460A_RESET(1);
}


void cs5460a_init(void)
{
	cs5460a_gpio_init();
	spi_init(&cs5460a_spi);
}




#include "oled.h"
#include <stdlib.h>
#include "font.h" //字符图像文件
#include "ds18b20.h"


/*0.96-OLED	驱动*/
void OLED_GPIO_Init(void)
{
 	GPIO_InitType GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(OLED_IIC_PCC,ENABLE);	
  RCC_APB2PeriphClockCmd(OLED_CTRL_PCC,ENABLE);	
	
	GPIO_InitStructure.GPIO_Pins     =  OLED_SCL_PIN|OLED_SDA_PIN;
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_OUT_PP;
	GPIO_Init(OLED_IIC_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pins     =  OLED_DC_PIN|OLED_RST_PIN;
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_OUT_PP;
	GPIO_Init(OLED_CTRL_PORT, &GPIO_InitStructure);
	
	OLED_SCL(1);
  OLED_SDA(1);
	OLED_RST(1);
	OLED_DC(1);
}


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

unsigned char  OLED_adjust_flag=0;
unsigned char  OLED_Brightness=0;  //OLED亮度1-254
unsigned char  _OLED_Reverse = 0;     
unsigned char  _OLED_Overlap = 1;
		
unsigned char _buf[WIDTH * PAGES]; //全屏缓存,横向WIDTH个像素，纵向PAGES页，页内每8个像素作为一个字节，共WIDTH * PAGES个字节

static char _x, _y;
static unsigned char _Font_Width = 6;

/*  写命令Write Command   */
void OLED_Write_Command(unsigned char dat)
{
	OLED_DC_Clr();
	xdelay_us(50);
	OLED_SCLK_Clr();
	OLED_DC_Clr();
	
	for (unsigned char i = 0; i < 8; i++)
	{
		(dat & 0x80)?(OLED_SDIN_Set()):(OLED_SDIN_Clr());
		OLED_SCLK_Clr();
		xdelay_us(50);
		OLED_SCLK_Set();
		dat <<= 1;
	}
}

/*  写数据
    Write Data  */
void OLED_Write_Data(unsigned char dat)
{
	
	OLED_DC_Set();
	xdelay_us(50);
	OLED_SCLK_Clr();	
	OLED_DC_Set();	
	
	for (unsigned char i = 0; i < 8; i++)
	{
		(dat & 0x80)?(OLED_SDIN_Set()):(OLED_SDIN_Clr());
		OLED_SCLK_Clr();
		xdelay_us(50);
		OLED_SCLK_Set();
		dat <<= 1;
	}
}

/* 1：反显，0：正常
   1: Reverse, 0: Normal*/
void OLED_Reverse(unsigned char  i)
{
    _OLED_Reverse = i&0x1;
}

/* 绘图模式：1：叠加，0：覆盖
   Draw Mode: 1: overlap, 0: Redraw*/
void OLED_Overlap(unsigned char  i)
{
    _OLED_Overlap = i&0x01;
}

/* 设置ASCII字符宽度
   Set ASCII character width*/
void OLED_SetFontWidth(unsigned char w)
{
    _Font_Width = w;
}

/* 设置内存位置x:0-127，y:0-63
   Set Ram address x:0-127, y:0-63*/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    _x = x;
    _y = y;
}

/* 清除缓存内容
   Clear data in buffer*/
void OLED_Clear(void)
{
    memset(_buf, 0x00, WIDTH * PAGES);
}

/*  绘制字节至缓存
    offset: 字节偏移量
    reserve_hl: 保留高位或低位
    比如要写入的字节1111 1111跨了两页0001 1111 1110 0000，
    在覆盖模式下，为避免覆盖相邻的内容，在写入0001 1111时，指定offset=3，reserve_hl=1，写入1110 0000时指定指定offset=5，reserve_hl=0
    Draw byte to buffer
    offset: Offset of Byte
    reserve_hl: Reserve High or Low
    If the byte 1111 1111 we want to write cross two pages 0001 1111 1110 0000
    In redraw mode, to avoid affect adjacent content. Set offset=3, reserve_hl=1, when write 0001 1111. Set offset=5, reserve_hl=0 when write 1110 0000
    */
void OLED_Draw_Byte(unsigned char *pBuf, unsigned char mask, unsigned char offset, unsigned char reserve_hl)
{
    if (_OLED_Overlap)
    {
        if (_OLED_Reverse)
            *pBuf |= ~mask;
        else
            *pBuf |= mask;
    }
    else
    {
        if (_OLED_Reverse)
        {
            /* 保留高位
               Reserve upper */
            if (reserve_hl&0x01) 
            {
                *pBuf &= (~mask) | (0xFF << (8 - offset));
                *pBuf |= (~mask) & (0xFF >> offset);
            }
            /* 保留低位
               Reserve lower */
            else 
            {
                *pBuf &= (~mask) | (0xFF >> (8 - offset));
                *pBuf |= (~mask) & (0xFF << offset);
            }
        }
        else
        {
            /* 保留高位
               Reserve upper */
            if (reserve_hl&0x01) 
            {
                *pBuf &= mask | (0xFF << (8 - offset));
                *pBuf |= mask & (0xFF >> offset);
            }
            /* 保留低位
               Reserve lower */ 
            else 
            {
                *pBuf &= mask | (0xFF >> (8 - offset));
                *pBuf |= mask & (0xFF << offset);
            }
        }
    }
}

/*  绘制字符至缓存
    Draw char to buffer*/
void OLED_DrawChar(unsigned char x, unsigned char y, unsigned char chr)
{
    unsigned char c;
    unsigned char i;
    unsigned char mask;
    unsigned char *pBuf;
    unsigned char offset;
    offset = y & 7;
    c = chr - ' ';

    pBuf = &_buf[(y >> 3) * WIDTH + x];

    for (i = 0; i < _Font_Width; i++)
    {
        mask = F6x8[c][i] << offset;
        OLED_Draw_Byte(pBuf++, mask, offset, 0);
    }

    if (offset && y < HEIGHT - 8)
    {
        pBuf = &_buf[((y >> 3) + 1) * WIDTH + x];
        for (i = 0; i < _Font_Width; i++)
        {
            mask = F6x8[c][i] >> (8 - offset);
            OLED_Draw_Byte(pBuf++, mask, 8 - offset, 1);
        }
    }
}

/*  绘制字符串至缓存
    Draw string to buffer   */
void OLED_DrawString(unsigned char *chr)
{
    while (*chr)
    {
        if (_x > WIDTH - _Font_Width)
        {
            _x = 0;
            _y += 8;
        }
        if (_y > HEIGHT - 8)
        {
            _y = 0;
        }

        OLED_DrawChar(_x, _y, *chr++);
        _x += _Font_Width;
    }
}

/* m^n */
unsigned int oled_pow(unsigned char m, unsigned char n) //reentrant
{
    /* 常规算法 */
    unsigned int result = 1;

    while (n--)
        result *= m;

    return result;

    /* 递归算法 需用reentrant修饰函数*/
    // if (n != 0)
    //     return (m * oled_pow(m, n - 1));
    // else
    //     return 1;
}

/*  绘制数字至缓存
    Draw digit to buffer
    len: length of digit
    digit:  0~4294967295   */
void OLED_DrawNum(unsigned int digit, unsigned char len)
{
    unsigned char t, i, temp;
    unsigned char enshow = 0;
    i = 0;
    for (t = 0; t < len; t++)
    {
        temp = (digit / oled_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                i++;
                continue;
            }
            else
                enshow = 1;
        }

        if (_x > WIDTH - _Font_Width)
        {
            _x = 0;
            _y += 8;
        }
        if (_y > HEIGHT - 8)
        {
            _y = 0;
        }

        OLED_DrawChar(_x + (_Font_Width) * (t - i), _y, temp + '0');
    }
    _x += len;
}

/*  绘制16x16汉字至缓存
    Draw 16x16 Chinese to buffer    */
void OLED_DrwCHS_16x16(unsigned char chs_no)
{
    if (_x > WIDTH - 16)
    {
        _x = 0;
        _y += 16;
    }
    if (_y > HEIGHT - 16)
    {
        _y = 0;
    }
    OLED_DrawBMP(_x, _y, _x + 15, _y + 15, (unsigned char *)CHS_16x16[chs_no], sizeof(CHS_16x16[chs_no]) / sizeof(CHS_16x16[chs_no][0]));
    _x += 16;
}

/*  绘制16x16汉字至缓存
    Draw 16x16 Chinese to buffer    */
void OLED_DrwICON_16x16(const char* icon_name)
{
	unsigned char chs_no=0;
	if(0==strcmp(icon_name," "))
	{
		chs_no=0;
	}
	else if(0==strcmp(icon_name,"MSG"))
	{
		chs_no=1;
	}
	else if(0==strcmp(icon_name,"BT"))
	{
		chs_no=2;
	}
	else if(0==strcmp(icon_name,"WIFI"))
	{
		chs_no=3;
	}	
	else if(0==strcmp(icon_name,"ETH_CON"))
	{
		chs_no=4;
	}		
	else if(0==strcmp(icon_name,"ETH_DISCON"))
	{
		chs_no=5;
	}		
	if (_x > WIDTH - 16)
	{
			_x = 0;
			_y += 16;
	}
	if (_y > HEIGHT - 16)
	{
			_y = 0;
	}
	OLED_DrawBMP(_x, _y, _x + 15, _y + 15, (unsigned char *)ICON_16x16[chs_no], sizeof(ICON_16x16[chs_no]) / sizeof(ICON_16x16[chs_no][0]));
	_x += 16;
}


/*  绘制一个像素至缓存
    Draw one pixel to buffer    */
void OLED_DrawPixel(unsigned char x, unsigned char y)
{
    unsigned char mask;
    unsigned char *pBuf;

    if (_x > WIDTH - 1)
    {
        _x = 0;
        _y += 1;
    }
    if (_y > HEIGHT - 1)
    {
        _y = 0;
    }

    pBuf = &_buf[(y >> 3) * WIDTH + x];
    mask = 1 << (y & 7);
    if (_OLED_Reverse)
    {
        *pBuf++ &= ~mask;
    }
    else
    {
        *pBuf++ |= mask;
    }
}

/*  绘制两点线段至缓存
    Draw line between two points to buffer  */
void OLED_DrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
    int dx, dy, ystep;
    int err;
    unsigned char swapxy = 0;

    if (x0 > WIDTH - 1)
        x0 = WIDTH - 1;

    if (y0 > HEIGHT - 1)
        y0 = HEIGHT - 1;

    if (x1 > WIDTH - 1)
        x1 = WIDTH - 1;

    if (y1 > HEIGHT - 1)
        y1 = HEIGHT - 1;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);

    if (dy > dx)
    {
        swapxy = 1;
        _swap_char(dx, dy);
        _swap_char(x0, y0);
        _swap_char(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_char(x0, x1);
        _swap_char(y0, y1);
    }

    err = dx >> 1;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (swapxy == 0)
            OLED_DrawPixel(x0, y0);
        else
            OLED_DrawPixel(y0, x0);

        err -= dy;

        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

/*  绘制垂直线至缓存
    Draw vertical line to buffer*/
void OLED_DrawVLine(unsigned char x, unsigned char y, unsigned char w)
{
    unsigned char mask;
    unsigned char *pBuf;

    if (x > WIDTH - 1)
        x = WIDTH - 1;

    if (y + w > HEIGHT)
        w = HEIGHT - y;

    while (w--)
    {
        pBuf = &_buf[(y >> 3) * WIDTH + x];
        mask = 1 << (y & 7);
        if (_OLED_Reverse)
        {
            *pBuf++ &= ~mask;
        }
        else
        {
            *pBuf++ |= mask;
        }
        y++;
    }
}

/*  绘制水平线至缓存
    Draw horizontal line to buffer  */
void OLED_DrawHLine(unsigned char x, unsigned char y, unsigned char w)
{
    unsigned char *pBuf;
    unsigned char mask;

    if (x + w > WIDTH)
        w = WIDTH - x;

    if (y > HEIGHT - 1)
        y = HEIGHT - 1;

    pBuf = &_buf[(y >> 3) * WIDTH + x];
    mask = 1 << (y & 7);

    while (w--)
    {
        if (_OLED_Reverse)
        {
            *pBuf++ &= ~mask;
        }
        else
        {
            *pBuf++ |= mask;
        }
    }
}

/*  绘制图片至缓存
    Draw pic to buffer  */
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char *BMP, unsigned short len)
{
    unsigned short j = 0;
    unsigned char x, y;
    unsigned char mask;
    unsigned char *pBuf;
    unsigned char offset;

    if (x0 > x1)
    {
        _swap_char(x0, x1);
    }

    if (y0 > y1)
    {
        _swap_char(y0, y1);
    }

    if (x0 > WIDTH - 1)
        x0 = WIDTH - 1;

    if (y0 > HEIGHT - 1)
        y0 = HEIGHT - 1;

    if (x1 > WIDTH - 1)
        x1 = WIDTH - 1;

    if (y1 > HEIGHT - 1)
        y1 = HEIGHT - 1;

    for (y = y0; y <= y1; y += 8)
    {
        offset = y & 7;

        for (x = x0; x <= x1; x++)
        {
            if (j > len - 1)
                break;

            pBuf = &_buf[(y >> 3) * WIDTH + x];
            mask = *(BMP + j++) << offset;
            OLED_Draw_Byte(pBuf++, mask, offset, 0);

            if (offset && y < HEIGHT - 8)
            {
                j--;
                pBuf = &_buf[((y >> 3) + 1) * WIDTH + x];
                mask = *(BMP + j++) >> (8 - offset);
                OLED_Draw_Byte(pBuf++, mask, 8 - offset, 1);
            }
        }
    }
}

/*  将缓存内容显示到屏幕上
    Send buffer to display  */
void OLED_Display(void)
{
    unsigned char i, j;
    static unsigned char *pBuf;
	  if(pBuf!= _buf)
		{
			 pBuf = _buf;
		}

    for (j = 0; j < PAGES; j++)
    {
        for (i = 0; i < WIDTH; i++)
        {
           OLED_Write_Data(*pBuf++);
        }
    }
}

/*  初始化SSD1306
    Initialize SSD1306  */
void OLED_Init(void)
{
	  OLED_GPIO_Init();
	
    xdelay_ms(50);
    OLED_RST_Clr();
    xdelay_ms(20);
    OLED_RST_Set();

    OLED_Write_Command(0xAE); //--Turn off oled panel

    /*  
        以下三条命令是给页面模式用的，行模式和列模式不要设置，会引起问题
        行模式或列模式，要用0x21和0x22设置范围
        Following three commands are for Page Addressing Mode. Do not set them in Horizontal addressing mode or Vertical addressing mode, will rise problem.
        For Horizontal addressing mode or Vertical addressing mode, should use 0x21 and 0x22 set column and page address
    */
    //	OLED_Write_Command(0x00); //-Set Lower Column Start Address for Page Addressing Mode
    //	OLED_Write_Command(0x10); //-Set Higher Column Start Address for Page Addressing Mode
    //	OLED_Write_Command(0xB0); //-Set the page start address of the target display location by command for Page Addressing Mode

    OLED_Write_Command(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Write_Command(0x00); //--0x00: Horizontal addressing mode, 0x01: Vertical addressing mode, 0x02: Page addressing mode
    
    OLED_Write_Command(0x21); //-Set Column Address
    OLED_Write_Command(0x00); //--Start address
    OLED_Write_Command(0x7f); //--End address

    OLED_Write_Command(0x22); //-Set Page Address
    OLED_Write_Command(0x00); //---Start address
    OLED_Write_Command(0x07); //--End address

    OLED_Write_Command(0x40); //-Set Display Start Line (0x40h~0x7F)

    OLED_Write_Command(0x81);                 //-Set Contrast Control for BANK0
    OLED_Write_Command(OLED_Brightness * 10); //    -0x00 to 0xFF, The segment output current increases as the contrast step value increases

    OLED_Write_Command(0xA1); //-Set Segment Re-map. 0xA1: Normal, 0xA0: Re-map left and right
    OLED_Write_Command(0xC8); //-Set COM Output Scan Direction. 0xC8: Normal, 0xC0: Re-map up and down
    OLED_Write_Command(0xA6); //-Set Normal/Inverse Display, 0xA6:Normal, 0xA7: Inverse

    OLED_Write_Command(0xA8); //-Set Multiplex Ratio (16~63)
    OLED_Write_Command(0x3F); //--63 multiplex mode

    OLED_Write_Command(0xD3); //-Set Display Offset (0x00~0x3F)
    OLED_Write_Command(0x00); //--No offset

    OLED_Write_Command(0xD5); //-Set display clock divide ratio/oscillator frequency
    OLED_Write_Command(0x20); //--Set Clock as 60 Frames/Sec

    OLED_Write_Command(0xD9); //-Set pre-charge period
    OLED_Write_Command(0xF2); //--Set Pre-Charge as 15 Clocks & Discharge as 2 Clock

    OLED_Write_Command(0xDA); //-Set com pins hardware configuration
    OLED_Write_Command(0x12); //

    OLED_Write_Command(0xDB); //-Set VCOM Deselect Level
    OLED_Write_Command(0x30); //--0.83xVcc

    OLED_Write_Command(0x8D); //-Set Charge Pump enable/disable
    OLED_Write_Command(0x14); //--0x14: Enable, 0x10: Disable

    OLED_Write_Command(0xA4); //-Entire Display ON, 0xA4: Disable, 0xA5: Enable
    OLED_Write_Command(0xAF); //-Turn on oled panel
}

void lcd_init(void)
{

	OLED_Init();
	OLED_Clear();

	//LOGO
	//OLED_DrawBMP(18, 24, 18 + 82 - 1, 24 + 13 - 1, MINIDSO, sizeof(MINIDSO) / sizeof(MINIDSO[0]));
	OLED_Set_Pos(0, 0);	
	OLED_DrwICON_16x16("MSG");
	OLED_Set_Pos(16, 0);	
	OLED_DrwICON_16x16("BT");
	OLED_Set_Pos(128-16, 0);	
	OLED_DrwICON_16x16("ETH_CON");		
	//Version
	OLED_Set_Pos(50, 30);
	OLED_DrawString("V1.0");

	//CopyRight
	OLED_Set_Pos(30, 39);
	OLED_DrawString("By Zhaushion");
	OLED_Set_Pos(44, 48);
		
//	OLED_DrwCHS_16x16(0);
//	OLED_DrwCHS_16x16(1);
//	OLED_DrwCHS_16x16(2);
//	OLED_DrwCHS_16x16(3);
//	OLED_DrwCHS_16x16(4);

//  OLED_Overlap(0);

//	/* 选项
//	 Options */
//	/* DrawMode */
//	OLED_Set_Pos(0, 0);
//	OLED_DrawString("PMode=");
//	//    OLED_DrawString(PlotModeTxt[PlotMode]);

//	/* LSB */
//	OLED_Set_Pos(70, 0);
//	OLED_DrawString("LSB=");
//	OLED_DrawString("   ");
//	OLED_Set_Pos(94, 0);
//	OLED_DrawNum(Lsb, 3);

//	/* OLED_Brightness */
//	OLED_Set_Pos(70, 12);
//	OLED_DrawString("BRT=");
//	OLED_DrawString("   ");
//	OLED_Set_Pos(94, 12);
//	OLED_DrawNum(OLED_Brightness, 3);

//	/* Vbat */
//	OLED_Set_Pos(0, 12);
//	OLED_DrawString("Vbat=");
//	//OLED_DrawString(c_mv_V(VBat));
//	OLED_DrawString("V");

//	/* 选项标志，125对应左箭头
//	 Option indicator, 125 is Left arrow */
//	/* PlotMode */

//	OLED_DrawChar(64, 0, 125);
//	OLED_DrawChar(116, 0, ' ');
//	OLED_DrawChar(116, 12, ' ');

///* LSB */

//	OLED_DrawChar(64, 0, ' ');
//	OLED_DrawChar(116, 0, 125);
//	OLED_DrawChar(116, 12, ' ');

///* OLED_Brightness */

//	OLED_DrawChar(64, 0, ' ');
//	OLED_DrawChar(116, 0, ' ');
//	OLED_DrawChar(116, 12, 125);
//	OLED_Overlap(1);

}

#include "task.h"


void lcd_thread_entry(void *parameter)
{

	
	OLED_Set_Pos(90,2);	
	OLED_DrawNum(26,4);
	
	if(OLED_adjust_flag)
	{
		OLED_adjust_flag=0;
		OLED_Write_Command(0x81);
		OLED_Write_Command(OLED_Brightness*10);		
	}
    OLED_Display();	   
}

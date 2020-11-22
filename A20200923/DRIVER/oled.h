

#ifndef __OLED_H__
#define __OLED_H__

#include "pindef.h"
#include "delay.h"

/*  OLED Command */
//#define OLED_CS_Clr() OLED_CS = 0
//#define OLED_CS_Set() OLED_CS = 1

#define OLED_RST_Clr()    OLED_RST(0)
#define OLED_RST_Set()    OLED_RST(1)

#define OLED_DC_Clr()     OLED_DC(0)
#define OLED_DC_Set()     OLED_DC(1)

#define OLED_SCLK_Clr() 	OLED_SCL(0) 
#define OLED_SCLK_Set() 	OLED_SCL(1) 

#define OLED_SDIN_Clr()   OLED_SDA(0)
#define OLED_SDIN_Set()   OLED_SDA(1)

#ifndef _swap_char
#define _swap_char(a, b) \
    {                    \
        unsigned char t = a;     \
        a = b;           \
        b = t;           \
    }
#endif


/*  OLED Pixel */
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8

/*  OLED Brightness */
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 25


extern unsigned char  OLED_adjust_flag;		
extern unsigned char  OLED_Brightness;  //OLED¡¡∂»1-254
		
/*  OLED Driver */
void OLED_Init(void);
void OLED_Write_Command(unsigned char dat);
void OLED_Clear(void);
void OLED_Display(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_WR_Byte(unsigned char dat, unsigned char cmd);
void OLED_DrawChar(unsigned char x, unsigned char y, unsigned char chr);
void OLED_DrawString(unsigned char *p);
void OLED_DrawNum(unsigned int num, unsigned char len);
void OLED_DrwCHS_16x16(unsigned char chs_no);
void OLED_DrawPixel(unsigned char x, unsigned char y);
void OLED_DrawHLine(unsigned char x, unsigned char y, unsigned char w);
void OLED_DrawVLine(unsigned char x, unsigned char y, unsigned char w);
void OLED_DrawLine(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char *BMP, unsigned short int len);
void OLED_SetFontWidth(unsigned char w);
void OLED_Reverse(unsigned char i);
void OLED_Overlap(unsigned char i);

void lcd_init(void);
void lcd_thread_entry(void *parameter);

#endif





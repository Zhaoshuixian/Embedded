
#ifndef   __PINDEF_H__
#define   __PINDEF_H__

#include "at32f4xx.h"
#include "string.h"


//#define DEBUG_MODE
#define YH_DEBUG  printf

#define MAKE_DATA (__DATE__) /*获取编译日期*/
#define MAKE_TIME (__TIME__) /*获取编译时间*/
#define FILE_NAME (__FILE__) /*获取当前文件名*/

#define HW_NUM     ("V1.0")  /*定义当前硬件版本号*/
#define SW_NUM     ("V1.1")  /*定义当前软件版本号*/

#define OFF (0x00)
#define ON  (0x01)

#define LOCK    (0X00) //选择
#define UNLOCK  (0X01) //悬挂

//////////////////////AT24C_PORT/////////////////////////////////
#define AT24CXX_PCC               RCC_APB2PERIPH_GPIOC
#define AT24CXX_PORT              GPIOC
#define AT24CXX_SCL_PIN           GPIO_Pins_8
#define AT24CXX_SDA_PIN           GPIO_Pins_9

//////////////////////TS06_PORT/////////////////////////////////
#define TS06_SDA_PCC           RCC_APB2PERIPH_GPIOB
#define TS06_SDA_PORT          GPIOB
#define TS06_SDA_PIN           GPIO_Pins_8

#define TS06_SCL_PCC           RCC_APB2PERIPH_GPIOC
#define TS06_SCL_PORT          GPIOC
#define TS06_SCL_PIN           GPIO_Pins_0

//////////////////////USB_PORT/////////////////////////////////
#define USB_PCC               RCC_APB2PERIPH_GPIOA
#define USB_PORT              GPIOA
#define USB_DM_PIN            GPIO_Pins_11
#define USB_DP_PIN            GPIO_Pins_12
#define USB_EN_PIN            GPIO_Pins_15

//////////////////////USER_USART_PORT/////////////////////////////////
#define USER_USART_NUM            USART1
#define USER_USART_PCC            RCC_APB2PERIPH_GPIOA
#define USER_USART_PORT           GPIOA
#define USER_USART_TX_PIN         GPIO_Pins_9
#define USER_USART_RX_PIN         GPIO_Pins_10
#define USER_USART_EN_PIN         GPIO_Pins_11

#define USER_USART_EN(x)          (x)?(GPIO_SetBits(USER_USART_PORT,USER_USART_EN_PIN)):(GPIO_ResetBits(USER_USART_PORT,USER_USART_EN_PIN))

//////////////////////BEEP_PORT/////////////////////////////////
#define BEEP_PCC                  RCC_APB2PERIPH_GPIOC
#define BEEP_PORT                 GPIOC
#define BEEP_PIN                  GPIO_Pins_10
#define BEEP_SW(x)                (0==x)?(GPIO_ResetBits(BEEP_PORT,BEEP_PIN)):(GPIO_SetBits(BEEP_PORT,BEEP_PIN))

//////////////////////LED_PORT/////////////////////////////////
#define LED_PCC                   RCC_APB2PERIPH_GPIOC
#define LED_PORT                  GPIOC
#define BLED_PIN                  GPIO_Pins_6
#define RLED_PIN                  GPIO_Pins_7
#define BLED_SW(x)                (0==x)?(GPIO_ResetBits(LED_PORT,BLED_PIN)):(GPIO_SetBits(LED_PORT,BLED_PIN)) 
#define RLED_SW(x)                (0==x)?(GPIO_ResetBits(LED_PORT,RLED_PIN)):(GPIO_SetBits(LED_PORT,RLED_PIN)) 

//////////////////////IR_PORT/////////////////////////////////
#define IR_TX_PCC                RCC_APB2PERIPH_GPIOB 
#define IR_TX_PORT               GPIOB
#define IR_TX_PIN                GPIO_Pins_9//TMR4_CH4 / TMR11_CH1
#define IR_TX_SW(x)              (0==x)?(GPIO_ResetBits(IR_TX_PORT,IR_TX_PIN)):(GPIO_SetBits(IR_TX_PORT,IR_TX_PIN));

#define IR_RX_PCC                RCC_APB2PERIPH_GPIOB  
#define IR_RX_PORT               GPIOB
#define IR_RX_PIN                GPIO_Pins_7 
#define IR_RX_DATA()             GPIO_ReadInputDataBit(IR_RX_PORT,IR_RX_PIN)

//////////////////////NTC/18B20_PORT/////////////////////////////////
#define NTC_PCC               		 RCC_APB2PERIPH_GPIOC
#define NTC_PORT              		 GPIOC
#define NTC_PIN              			 GPIO_Pins_1

#define DS_PCC            		     RCC_APB2PERIPH_GPIOC
#define DS_PORT              		   GPIOC
#define DS_PIN              			 GPIO_Pins_1

//////////////////////SS_PORT/////////////////////////////////
#define SS_PCC                     RCC_APB2PERIPH_GPIOB
#define SS_PORT                    GPIOB
#define SS_PIN                     GPIO_Pins_1

//////////////////////0.96'OLED_PORT/////////////////////////////////
#define OLED_IIC_PCC               RCC_APB2PERIPH_GPIOA
#define OLED_IIC_PORT              GPIOA
#define OLED_SDA_PIN               GPIO_Pins_0
#define OLED_SCL_PIN               GPIO_Pins_1
#define OLED_SDA(x)                (0==x)?(GPIO_ResetBits(OLED_IIC_PORT,OLED_SDA_PIN)):(GPIO_SetBits(OLED_IIC_PORT,OLED_SDA_PIN))
#define OLED_SCL(x)                (0==x)?(GPIO_ResetBits(OLED_IIC_PORT,OLED_SCL_PIN)):(GPIO_SetBits(OLED_IIC_PORT,OLED_SCL_PIN))

#define OLED_CTRL_PCC              RCC_APB2PERIPH_GPIOC
#define OLED_CTRL_PORT             GPIOC
#define OLED_DC_PIN                GPIO_Pins_2
#define OLED_RST_PIN               GPIO_Pins_3
#define OLED_DC(x)                 (0==x)?(GPIO_ResetBits(OLED_CTRL_PORT,OLED_DC_PIN)):(GPIO_SetBits(OLED_CTRL_PORT,OLED_DC_PIN))
#define OLED_RST(x)                (0==x)?(GPIO_ResetBits(OLED_CTRL_PORT,OLED_RST_PIN)):(GPIO_SetBits(OLED_CTRL_PORT,OLED_RST_PIN))
//////////////////////+5V_PORT/////////////////////////////////
#define PWR5V_EN_PCC             RCC_APB2PERIPH_GPIOD
#define PWR5V_EN_PORT            GPIOD
#define PWR5V_EN_PIN             GPIO_Pins_2

#define PWR5V_EN_SW(x)           (x)?(GPIO_ResetBits(PWR5V_EN_PORT,PWR5V_EN_PIN)):(GPIO_SetBits(PWR5V_EN_PORT,PWR5V_EN_PIN)) 
#define PWR5V_READ()              GPIO_ReadInputDataBit(PWR5V_EN_PORT,PWR5V_EN_PIN)
//////////////////////+24V_PORT/////////////////////////////////
#define PWR24V_EN_PCC             RCC_APB2PERIPH_GPIOB
#define PWR24V_EN_PORT            GPIOB
#define PWR24V_EN_PIN             GPIO_Pins_3

#define PWR24V_EN_SW(x)           (0==x)?(GPIO_ResetBits(PWR24V_EN_PORT,PWR24V_EN_PIN)):(GPIO_SetBits(PWR24V_EN_PORT,PWR24V_EN_PIN)) 
#define PWR24V_READ()             GPIO_ReadInputDataBit(PWR24V_EN_PORT,PWR24V_EN_PIN)
//////////////////////AC220V_PORT/////////////////////////////////
#define AC220V_EN_PCC             RCC_APB2PERIPH_GPIOC
#define AC220V_EN_PORT            GPIOC
#define AC220V_EN_PIN             GPIO_Pins_5

#define AC220V_EN_SW(x)           (0==x)?(GPIO_ResetBits(AC220V_EN_PORT,AC220V_EN_PIN)):(GPIO_SetBits(AC220V_EN_PORT,AC220V_EN_PIN)) 
#define PWR220V_READ()            GPIO_ReadInputDataBit(AC220V_EN_PORT,AC220V_EN_PIN)

//////////////////////W25Q_PORT/////////////////////////////////
#define W25Q_CS_PCC               RCC_APB2PERIPH_GPIOA
#define W25Q_CS_PORT              GPIOA
#define W25Q_CS_PIN               GPIO_Pins_4  //

#define W25Q_CSN(x)               (0==x)?(GPIO_ResetBits(W25Q_CS_PORT,W25Q_CS_PIN)):(GPIO_SetBits(W25Q_CS_PORT,W25Q_CS_PIN))                 
          
//////////////////////NRF24L01_PORT/////////////////////////////////
#define NRF_CS_PCC                   RCC_APB2PERIPH_GPIOC
#define NRF_CS_PORT                  GPIOC
#define NRF_CS_PIN                   GPIO_Pins_4  //PC4   ---
#define NRF_CSN(x)                   (0==x)?(GPIO_ResetBits(NRF_CS_PORT,NRF_CS_PIN)):(GPIO_SetBits(NRF_CS_PORT,NRF_CS_PIN)) 

#define NRF_CE_PCC                   RCC_APB2PERIPH_GPIOC
#define NRF_CE_PORT                  GPIOC
#define NRF_CE_PIN                   GPIO_Pins_5  //PC5   ---
#define NRF_CE(x)                    (0==x)?(GPIO_ResetBits(NRF_CE_PORT,NRF_CE_PIN)):(GPIO_SetBits(NRF_CE_PORT,NRF_CE_PIN)) 

#define NRF_IRQ_PCC                   RCC_APB2PERIPH_GPIOB
#define NRF_IRQ_PORT                  GPIOB
#define NRF_IRQ_PIN                   GPIO_Pins_0  //P0   ---
#define NRF_IRQ_READ()                GPIO_ReadInputDataBit(NRF_IRQ_PORT,NRF_IRQ_PIN)

//////////////////////RS485_PORT/////////////////////////////////
#define  MODBUS_USART               USART1  
#define  MODBUS_USART_CLK           RCC_APB2Periph_USART1  
#define  MODBUS_USART_GPIO_CLK      RCC_APB2PERIPH_GPIOA  
#define  MODBUS_USART_TX_PORT       GPIOA  
#define  MODBUS_USART_RX_PORT       GPIOA 
#define  MODBUS_USART_EN_PIN        GPIO_Pins_8
#define  MODBUS_USART_TX_PIN        GPIO_Pins_9  
#define  MODBUS_USART_RX_PIN        GPIO_Pins_10 
#define  MODBUS_USART_IRQ           USART1_IRQn
#define  MODBUS_USART_IRQHandler    USART1_IRQHandler 

#define  MODBUS_TX_EN(x)            USER_USART_EN(x)   

#define enter_critical()  (__disable_irq())
#define exit_critical()   (__enable_irq() )


//---------延时函数指针-------
typedef void (*xdelay)(unsigned int);
typedef void (*sdelay)(unsigned int);

#endif


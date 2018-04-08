#include "stm8s.h"


/********TM1668相关IO口宏定义*********/
#define TM1668_DIO_PORT       GPIOD
#define TM1668_DIO_PIN        GPIO_PIN_0

#define TM1668_CLK_PORT       GPIOC
#define TM1668_CLK_PIN        GPIO_PIN_7

#define TM1668_STB_PORT       GPIOC
#define TM1668_STB_PIN        GPIO_PIN_6

#define TM1668_DIO_H     GPIO_WriteHigh(TM1668_DIO_PORT, TM1668_DIO_PIN)
#define TM1668_DIO_L     GPIO_WriteLow(TM1668_DIO_PORT,  TM1668_DIO_PIN)
#define TM1668_CLK_H     GPIO_WriteHigh(TM1668_CLK_PORT, TM1668_CLK_PIN)
#define TM1668_CLK_L     GPIO_WriteLow(TM1668_CLK_PORT,  TM1668_CLK_PIN)
#define TM1668_STB_H     GPIO_WriteHigh(TM1668_STB_PORT, TM1668_STB_PIN)
#define TM1668_STB_L     GPIO_WriteLow(TM1668_STB_PORT,  TM1668_STB_PIN)


#define TM1668COM_7SEG_11GRID         0x03

#define TM1668COM_ADDRESS_CONTINUE    0x40
#define TM1668COM_ADDRESS_CONSTANT    0x44

#define TM1668COM_DISPLAY_1_16        0x80
#define TM1668COM_DISPLAY_2_16        0x81
#define TM1668COM_DISPLAY_4_16        0x82
#define TM1668COM_DISPLAY_10_16       0x83
#define TM1668COM_DISPLAY_11_16       0x84
#define TM1668COM_DISPLAY_12_16       0x85
#define TM1668COM_DISPLAY_13_16       0x86
#define TM1668COM_DISPLAY_14_16       0x87
#define TM1668COM_DISPLAY_ON          0x88
#define TM1668COM_DISPLAY_OFF         0x80

#define TM1668COM_CONFIG_ADDRESS      0xC0

//
#define SEG_A 0x80
#define SEG_B 0x20
#define SEG_C 0x01
#define SEG_D 0x02
#define SEG_E 0x08
#define SEG_F 0x40
#define SEG_G 0x10
#define SEG_P 0x04



//special char
#define   DISP_S       0x05
#define   DISP_C       0x0a
#define   DISP_A       0x0b
#define   DISP_L       0x0c
#define   DISP_P       0x0d
#define   DISP_Q       0x0e
#define   DISP_F       0x0f
#define   DISP_U       0x10
#define   DISP_E       0x11
#define   DISP_r       0x12
#define   DISP_X       0x13  // -
#define   DISP_b       0x14
#define   DISP_o       0x15
#define   DISP_t       0x16
#define   DISP_d       0x17
#define   DISP_c       0x18
#define   DISP_N       0x19
#define   DISP_H       0x1a
#define   DISP_ALL     0x1b
#define   DISP_NULL    0x1c


extern u8 const display_BATTERY[];
extern u8 const display_LPMODE[]; 
extern u8 const display_FULL[]; 
extern u8 const display_COUNT[]; 
extern u8 const display_COUNTERR[];
extern u8 const display_USERCAL[];
extern u8 const display_USERCALERR[];

extern u8 const display_FACTORYCAL[];
extern u8 const display_LINECAL[];



extern const u8 display_code[];
/*******声明接口函数*******/

extern void TM1668_Update(void);  //数码管显示
extern void TM1668_DisplayAll(void);
extern void TM1668_DisplayMode(void);
extern void TM1668_Display_SpecialChar(u8 x);

extern void TM1668_Display_Factory(void);
extern void TM1668_Display_LineCal(void);
extern void TM1668_Display_UserCal(void);
extern void TM1668_Display_Normal(void);

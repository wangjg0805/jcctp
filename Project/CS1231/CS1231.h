
#ifndef __CS1231_h__
#define __CS1231_h__

//定义管脚及其动作

//////////////////////
#define CS1231_PDWN_H           GPIO_WriteHigh(GPIOD,  GPIO_PIN_3)
#define CS1231_PDWN_L           GPIO_WriteLow(GPIOD,  GPIO_PIN_3)

#define CS1231_CLK_H            GPIO_WriteHigh(GPIOD,  GPIO_PIN_2)
#define CS1231_CLK_L            GPIO_WriteLow(GPIOD,  GPIO_PIN_2)

#define CS1231_SDIO_MODE_OUT    GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_FAST)
#define CS1231_SDIO_MODE_IN     GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_IN_FL_NO_IT)

#define CS1231_SDIO_H           GPIO_WriteHigh(GPIOD,  GPIO_PIN_7)
#define CS1231_SDIO_L           GPIO_WriteLow(GPIOD,  GPIO_PIN_7)
					
#define CS1231_SDIO_MASK        0x80
#define READ_CS1231PORT         GPIO_ReadInputData(GPIOD)
#define READ_CS1231_SDO         GPIO_ReadInputPin(GPIOD, GPIO_PIN_7)
 
//extern void CS1231_Init(void);
extern void CS1237_Init(void);
extern u8 CS1231_Read(void);
#endif
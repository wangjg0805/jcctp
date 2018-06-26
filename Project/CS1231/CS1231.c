/********************************************************************************
********************************************************************************/
#include  "global.h"
#include  "CS1231.h"
#include "stdio.h" 
#include "stdlib.h"
 ///////////////////////////////////////////
static void delay(u32 length)
    {
    while (length >0)
        length--;
    }

static void CS1231_PinInit(void)
{
    //GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST); //DOUT
    GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST); //CLK
    GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST); //PD
   
}

void CS1231_Init()
{
    CS1231_PinInit();
    //设置CLK PDWN为输出 
    //IOSEL  = 0;
    CS1231_PDWN_H;
    
    CS1231_SDIO_MODE_OUT;
 
    CS1231_SDIO_H;//ADS1230_DOUT_SET;
    CS1231_CLK_L; //ADS1230_SCLK_CLR;
    delay(100);
    //CS1231_PDWN_H;//ADS1230_PDWN_SET;
    //////////////////////
    CS1231_SDIO_MODE_IN;
}

static u8 CS1231_Read8bits(void)
{
    u8 i,j;
    u8 dat=0;
    for(i=0;i<8;i++) {
        dat <<= 1;
        //delay(5);
	    CS1231_CLK_L;//ADS1230_SCLK_CLR;
        //delay(5);
	    CS1231_CLK_H;//ADS1230_SCLK_SET;
        j = READ_CS1231PORT;
        
        if(j & CS1231_SDIO_MASK) 
            dat++;
    }
	return dat;
}
 
u8 CS1231_Read(void)
{
    u8 i;
    u32 tmp;
    tmp = 0;
    
	//disableInterrupts();
    for (i = 0; i < 3; i++)
        tmp  = (tmp<<8)+CS1231_Read8bits();
	//delay(5);
    CS1231_CLK_L;//ADS1230_SCLK_CLR;
    //delay(5);//__no_operation();__no_operation();
    CS1231_CLK_H;//ADS1230_SCLK_SET;
    //delay(5);//__no_operation();__no_operation();
    CS1231_CLK_L;//ADS1230_SCLK_CLR;
    
    //enableInterrupts();
    
    if((tmp==0xffffff)||(tmp==0))
        return(0);
    else {
        MData.hx711_data = tmp>>1;	  //取 22bit
        //printf("hx711_data:%ld\r\n",MData.hx711_data);
	    return(1);
    }
}
	

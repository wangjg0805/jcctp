/********************************************************************************
********************************************************************************/
#include  "global.h"
#include  "CS1231.h"
#include "stdio.h" 
#include "stdlib.h"

#define DELAY_CNT     20



static void delay(u32 length)
{
    while(length >0)
        length--;
}

static void CS1237_PinInit(void)
{
    //GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST); //DOUT
    GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST); //CLK
    GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST); //PD
    CS1231_SDIO_MODE_OUT;
    
    
    CS1231_CLK_L;
    CS1231_SDIO_L;
    CS1231_SDIO_H;
    CS1231_CLK_H;
    
}

void CS1237_Config(void)
{
	unsigned char i;
	unsigned char dat;
	
	CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    CS1231_SDIO_MODE_IN;
    CS1231_CLK_L;
    
    while(1) {
        delay(1000);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }
    
	for(i=0;i<29;i++)// 1 - 29
	{
        CS1231_CLK_H;
		delay(DELAY_CNT);
        CS1231_CLK_L;
		delay(DELAY_CNT);
	}
    
	CS1231_SDIO_MODE_OUT;
    dat = 0xCA;
    for(i=0;i<7;i++) { //write 0x65
        CS1231_CLK_H;
        delay(DELAY_CNT);
        if(dat&0x80)
            CS1231_SDIO_H;
        else
            CS1231_SDIO_L;
        CS1231_CLK_L;
        delay(DELAY_CNT);
        dat <<= 1;
    }

	CS1231_CLK_H;	// CLK=1;
	delay(DELAY_CNT);
	CS1231_CLK_L;	// CLK=0;
	delay(DELAY_CNT);
    
	dat = 0x0c;
	for(i=0;i<8;i++){
        CS1231_CLK_H;	// CLK=1;
		delay(DELAY_CNT);
		if(dat&0x80)
			CS1231_SDIO_H;// OUT = 1
		else
			CS1231_SDIO_L;
        CS1231_CLK_L;
		dat <<= 1;
		delay(5);
	}
    
	CS1231_SDIO_H;// OUT = 1
	CS1231_CLK_H;	// CLK=1;
    delay(DELAY_CNT);
	CS1231_CLK_L;
	delay(DELAY_CNT);
}

// 读取芯片的配置数据
unsigned char CS1237_ReadConfig(void)
{
	unsigned char i,j;
	unsigned char dat=0;//读取到的数据
	
	CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    CS1231_SDIO_MODE_IN;
    CS1231_CLK_L;
    
    while(1) {
        delay(1000);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }

	for(i=0;i<29;i++)// 1 - 29
	{
        CS1231_CLK_H;
		delay(DELAY_CNT);
        CS1231_CLK_L;
		delay(DELAY_CNT);
	}
    
	CS1231_SDIO_MODE_OUT;
    
    dat = 0xAC;
    for(i=0;i<7;i++) { //write 0x56
        CS1231_CLK_H;
        delay(DELAY_CNT);
        if(dat&0x80)
            CS1231_SDIO_H;
        else
            CS1231_SDIO_L;
        CS1231_CLK_L;
        delay(DELAY_CNT);
        dat <<= 1;
    }
    	
    CS1231_SDIO_H;
    CS1231_CLK_H;  //the 37th pulses
    delay(DELAY_CNT);
    CS1231_CLK_L;
    delay(DELAY_CNT);
 
	dat=0;
	CS1231_SDIO_MODE_IN;
	for(i=0;i<8;i++) {
        CS1231_CLK_H;	// CLK=1;
		delay(DELAY_CNT);
		CS1231_CLK_L;	// CLK=0;
		delay(DELAY_CNT);
		dat <<= 1;
        j = READ_CS1231PORT;
        if(j & CS1231_SDIO_MASK)
            dat++;
	}
	
	//第46个脉冲
	CS1231_CLK_H;	// CLK=1;
	delay(DELAY_CNT);
	CS1231_CLK_L;	// CLK=0;
	delay(DELAY_CNT);
	
	CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    
	return dat;
}



void CS1237_Init()
{
    u8 i;
    
    CS1237_PinInit();

    CS1237_Config();    
    delay(20000);
    i = CS1237_ReadConfig();
    printf("CS1237_Read is %d  \r\n",i);
    
}

u8 CS1231_Read(void)
{
    u8 i,j;
    u32 dat;
    
    CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    CS1231_SDIO_MODE_IN;
    CS1231_CLK_L;
    while(1) {
        delay(1000);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }
    
    dat = 0;
    for(i=0;i<24;i++){
        CS1231_CLK_H;
        delay(DELAY_CNT);
		dat <<= 1;
        j = READ_CS1231PORT;
        if(j & CS1231_SDIO_MASK)
            dat++;
        CS1231_CLK_L;
        delay(DELAY_CNT);
	}
	
	for(i=0;i<3;i++) {
		 CS1231_CLK_H;
         delay(DELAY_CNT);
         CS1231_CLK_L;
         delay(DELAY_CNT);
	}
	
	CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    
    printf("hx711_data:%ld\r\n",dat);
    
    if((0==dat)||(0x7fffff==dat))
        return(0);
    else
        return(1);

}
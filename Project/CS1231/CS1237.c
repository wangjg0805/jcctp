/********************************************************************************
********************************************************************************/
#include  "global.h"
#include  "CS1231.h"
#include "stdio.h" 
#include "stdlib.h"

#define DELAY_CNT     2



static void delay(u32 length)
{
    while(length >0)
        length--;
}

static void CS1237_PinInit(void)
{
    //GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST); //DOUT
    GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_HIGH_FAST); //CLK
    GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST); //PD
   
    CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    CS1231_CLK_L;
    CS1231_SDIO_MODE_IN;
}

static void CS1237ClockUp(void)
{
    CS1231_CLK_H;
	delay(DELAY_CNT);
    CS1231_CLK_L;
	delay(DELAY_CNT);
    
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
        delay(100);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }
    
	for(i=0;i<29;i++)// 1 - 29
	    CS1237ClockUp();
    
	CS1231_SDIO_MODE_OUT;
    dat = 0xCA;
    for(i=0;i<7;i++) { //write 0x65
        CS1231_CLK_H;
        if(dat&0x80)
            CS1231_SDIO_H;
        else
            CS1231_SDIO_L;
        CS1231_CLK_L;
        dat <<= 1;
    }
    CS1237ClockUp();
	dat = 0x1c;
	for(i=0;i<8;i++){
        CS1231_CLK_H;
		if(dat&0x80)
			CS1231_SDIO_H;// OUT = 1
		else
			CS1231_SDIO_L;
        CS1231_CLK_L;
		dat <<= 1;
	}
    
    CS1237ClockUp();

}

// 读取芯片的配置数据
unsigned char CS1237_ReadConfig(void)
{
	unsigned char i,j;
	unsigned char dat=0;//读取到的数据
	   
    CS1231_CLK_L;
    while(1) {
        delay(100);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }

	for(i=0;i<29;i++)// 1 - 29
        CS1237ClockUp();
    
	CS1231_SDIO_MODE_OUT;
    
    dat = 0xAC;
    for(i=0;i<7;i++) { //write 0x56
       //CS1237ClockUp();
        CS1231_CLK_H;
        if(dat&0x80)
            CS1231_SDIO_H;
        else
            CS1231_SDIO_L;
        dat <<= 1;
        CS1231_CLK_L;
    }
    
    CS1231_SDIO_H;
    CS1237ClockUp();
	dat=0;
	CS1231_SDIO_MODE_IN;
	for(i=0;i<8;i++) {
        CS1237ClockUp();
		dat <<= 1;
        j = READ_CS1231PORT;
        if(j & CS1231_SDIO_MASK)
            dat++;
	}
	
	//第46个脉冲
    CS1237ClockUp();
    //CS1231_CLK_L;	// CLK=0;
    
	return dat;
}



void CS1237_Init(void)
{
    u8 i;
    
    CS1237_PinInit();

    CS1237_Config();    
    delay(0000);
    i = CS1237_ReadConfig();
    printf("CS1237_Read is %d  \r\n",i);
    
}

u8 CS1231_Read(void)
{
    u8 i,j;
    u32 dat;
    
    CS1231_SDIO_MODE_OUT;
    CS1231_SDIO_H;
    CS1231_CLK_L;
    
    CS1231_SDIO_MODE_IN;
    while(1) {
        delay(100);
        if(RESET == READ_CS1231_SDO)  //wait for cs1237 
            break;
    }
    
    dat = 0;
    for(i=0;i<24;i++){
        CS1231_CLK_H;
        dat <<= 1;
        j = READ_CS1231PORT;
        if(j & CS1231_SDIO_MASK)
            dat++;
        CS1231_CLK_L; 
  	}
	
    CS1237ClockUp();
    CS1237ClockUp();
    CS1237ClockUp();
    
    // printf("hx711_data:%ld\r\n",dat);
    MData.hx711_data = dat>>1;
    if((0==dat)||(0x7fffff==dat))
        return(0);
    else 
        return(1);

}
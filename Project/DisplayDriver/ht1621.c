#include "stm8s_conf.h"
#include "stdio.h"
#include "stdlib.h"

#include "math.h"

#include "global.h"

#define SEG_A 0x80
#define SEG_B 0x40
#define SEG_C 0x20
#define SEG_D 0x01
#define SEG_E 0x02
#define SEG_F 0x08
#define SEG_G 0x04
#define SEG_P 0x10


////////////////////////////
//HT1621 管脚初始化
///////////////////////////
static void HT1621_PinInit(void)
  {
   GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST); //CS
   GPIO_Init(GPIOD,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST); //WR
   GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST); //DATA
  } 

//-----------------------------------------------------------------------------   
//传送Length位数据给1621   
static void Send_LBit(u8 Sdata,u8 Length)   
{   
    u8 i=Length;   
    while(i--)   
    {   
        WRL;      //nWR=0;   
        if(Sdata&0x80)   
            DATAH;//DATA=1;   
        else   
            DATAL;//DATA=0;   
        Sdata<<=1;   
        WRH;      //nWR=1;   
            asm("nop");
            asm("nop");
    }      
} 
//-----------------------------------------------------------------------------   
//送控制指令   
static void Send_Com(u8 con,u8 sdata)   
{   
    CSL;//nCS=0;   
    Send_LBit(con,3);   
    Send_LBit(sdata,8);   
    Send_LBit(0x00,2);   
    CSH;//nCS=1;   
}   
//-----------------------------------------------------------------------------   
//初始化   
void HT1621_Init(void)   
{   
    u8 i;
    HT1621_PinInit();
    
    Send_Com(COMMAND,SYS_EN);   
    Send_Com(COMMAND,Rosc);   
    Send_Com(COMMAND,BIS1_2);   
    //Send_Com(COMMAND,NORMAL);        
    Send_Com(COMMAND,LCD_ON);   
    
    //DIS_Off_All();
}  
////////////////////////////
//HT1621 刷新显示区
///////////////////////////
void DIS_Update(void)
{
    u8 i,j,data;   
    CSL;   
    Send_LBit(WRITE,3);   
    Send_LBit(0x00,6);   
    for( i = 0; i < 16; i++) {
        data = display_buffer[15 - i];
        Send_LBit(data,8); 
    }
    CSH;
}
//-----------------------------------------------------------------------------   
//全部点亮   
void DIS_On_All(void)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_buffer[i] = 0xff;
    
    DIS_Update();
}   

//熄灭
void DIS_Off_All(void)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_buffer[i] = 0x00;
    
    //DIS_Update();
}   

//-----------------------------------------------------------------------------   
//全部点亮 
void DIS_SpecialChar(u8 data)   
{   
    u8 i;   
    for( i = 0; i < 16; i++)
        display_buffer[i] = display_code[data];
    
    DIS_Update();
}   
//-----------------------------------------------------------------------------   

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void DIS_BootInfo(void)
 {
    u8 i;    
	for(i=0;i<16;i++)	
        display_buffer[i] = display_code[display_BOOT_INFO[i]];
    
}

void DIS_LowPowerInfo(void)
{
 
}


void DIS_FactoryCfg(u8 step)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_FACTORY[i]];
     display_buffer[1] = display_code[step];
}


//////////////////////////////////////////////////////////////////////
void DIS_Flag(void)
{
    if(1 == RunData.stable_flag)
        display_buffer[4] |= SEG_P;
    
    //if(
    //stable_flag
    //neg flag
    //total flag    
}	

void DISP_ClearPreZero(u8 maxnum,u8 dot,u8* buf)
{
    u8 i;
    for(i = 0;i < maxnum - dot;i++) {
        if(*buf == 0) {
            *buf = DISP_NULL;
            buf++;
        } else
            break;
    }
}


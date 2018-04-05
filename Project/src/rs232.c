#include "stm8s_conf.h"
#include "stdio.h" 
#include "stdlib.h"
#include "string.h"



#include "global.h" 
////////////////////////////////////////////
//uart2 application
////////////////////////////////////////////
void UART2_INIT(void)
{
    UART2_DeInit();
    UART2_Init((u32)9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE, ENABLE);
    UART2_Cmd(ENABLE);
}


int putchar(int c)  
{  
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    UART2_SendData8((unsigned char)c);
    return (c);  
}
////////////////////////////////////////////
//
////////////////////////////////////////////
static void UART2_SendByte(char data)
{
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);  
    UART2_SendData8((unsigned char)data);
    /* Loop until the end of transmission */
}
////////////////////////////////////////////
//
////////////////////////////////////////////
void UART2_SendString(char* Data,u8 len)
{
    u8 i=0;
    for(;i<len;i++)
        UART2_SendByte(Data[i]);
    
}
////////////////////////////////////////////
//print init
////////////////////////////////////////////
void  UART2_PrinterInit(void) 
{
    char  OutString[8];
        
    OutString[0] =  0x1b;
    OutString[1] =  0x40;
    OutString[2] =  0x1b;
    OutString[3] =  0x63;
    OutString[4] =  0x00;
    UART2_SendString(OutString,5);
}


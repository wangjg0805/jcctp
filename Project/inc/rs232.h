#ifndef __RS232_H__
#define __RS232_H__


#include "stm8s.h"

extern void UART2_INIT(void);
extern void UART2_SendData(char* tip,u32 data); 
extern void UART2_SendString(char* Data,u8 len);
extern void UART2_PrinterInit(void);

#endif
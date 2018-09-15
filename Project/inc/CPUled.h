#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm8s.h"

#define SPEAKER_PORT   GPIOD
#define SPEAKER_PIN    GPIO_PIN_4

#define BKLIGHT_PORT   GPIOX
#define BKLIGHT_PIN    GPIO_PIN_X


extern void Speaker_Init(void);
extern void Speaker_On(void);
extern void Speaker_Off(void);
extern void Speaker_Proc(void);
////////////////////////////
//·äÃùÆ÷¹Ü½Å³õÊ¼»¯
///////////////////////////
extern void BkLight_Reverse(void);
extern void BkLight_On(void);
extern void BkLight_Off(void);

#endif
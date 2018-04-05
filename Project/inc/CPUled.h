#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm8s.h"

#define SPEAKER_PORT   GPIOD
#define SPEAKER_PIN    GPIO_PIN_4

#define BKLIGHT_PORT   GPIOX
#define BKLIGHT_PIN    GPIO_PIN_X


void Speaker_Init(void);
void Speaker_On(void);
void Speaker_Off(void);
////////////////////////////
//·äÃùÆ÷¹Ü½Å³õÊ¼»¯
///////////////////////////
void BkLight_Reverse(void);
void BkLight_On(void);
void BkLight_Off(void);

#endif
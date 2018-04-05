#ifndef __TIMER1_H__
#define __TIMER1_H__

extern  u8 flag_10ms,flag_100ms,flag_500ms;

void  TIM1_Init(void);
void  TIM2_Init(void);
void  Timer2_ISR();
#endif
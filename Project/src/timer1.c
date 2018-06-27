#include "stm8s.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"

#include "global.h"


static u8 T0ms02,T0ms10,T0ms100,T0ms500;

////////////////////////////////////
//定时器1初始化
////////////////////////////////////
void TIM1_Init(void)
 {
  TIM1_DeInit();
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_TimeBaseInit(1,TIM1_COUNTERMODE_UP,900,0);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_Cmd(ENABLE);
  }
////////////////////////////////////
//定时器2初始化
////////////////////////////////////
void TIM2_Init(void)
 {
  TIM2_DeInit();
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_TimeBaseInit(TIM2_PRESCALER_16,2000); //1M  计满2000个脉冲是 2ms 
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(ENABLE);
  }
////////////////////////////////////
//获取时间
////////////////////////////////////
void Timer2_ISR(void)
{    
    T0ms02++;	
    //========== 10ms Time ===========
	if(T0ms02<5)
		return;
	T0ms02=0;
    T0ms10++;
    Flag_10ms = 1;
 
  	//========== 100ms Time ============
	if(T0ms10<10)
	    return;
	T0ms10=0;
	T0ms100++;
    Flag_100ms = 1;
			
	//=========== 0.5s Time ============
	if(T0ms100 < 5)
		return;
    T0ms100=0;
    Flag_500ms = 1;
	T0ms500++;
    RunData.no_key_time++;      //500ms 
    RunData.keep_zero_time++;
    RunData.not_zero_time++;
    //LedCpu_Reverse();
  	//========= 10s Time ==========
	if(T0ms500 < 10)
	    return;
	T0ms500 = 0;
    Flag_5s = 1;
}
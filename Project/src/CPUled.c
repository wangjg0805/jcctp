#include "stm8s.h"
#include "global.h"
#include "CPUled.h"


void Speaker_Init(void)
 {  
    GPIO_Init(SPEAKER_PORT,SPEAKER_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
 }

void Speaker_On(void)
{      
    GPIO_WriteHigh(SPEAKER_PORT,  SPEAKER_PIN);
}

void Speaker_Off(void)
 {  
    GPIO_WriteLow(SPEAKER_PORT,  SPEAKER_PIN);
 }


/////////////////////////////////////////…˘“Ù¥¶¿Ì
void Speaker_Proc(void)
{
    if(0!=RunData.key_sound_time) {
        RunData.key_sound_time--;
        Speaker_On();
    } else {
        Speaker_Off();
    }
    
}
 

////////////////////////////
//LCD bklight pin
///////////////////////////
#if (DISPLAY_TYPE == LCD)
void Bklight_Init(void)
 {  
    GPIO_Init(GPIOF,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_FAST);
 }

void Bklight_On(void)
 {
    GPIO_WriteHigh(GPIOE, GPIO_PIN_5); 
   
 }
void Bklight_Off(void)
 {
    GPIO_WriteLow(GPIOE, GPIO_PIN_5); 
 }
#endif

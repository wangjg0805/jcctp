#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"
#include "normal.h"
#include "lowpower.h"

#include "TM1668.h"
#include "CPUled.h"
#include "i2c.h"
#include "ht1621.h"
#include "factory.h"
//
void Init_HSI(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_CPUDIV1);
    
}

void Init_ADC1(void)
{
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
              ADC1_CHANNEL_12,
              ADC1_PRESSEL_FCPU_D4,
              ADC1_EXTTRIG_TIM,
              DISABLE,ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL12,
              DISABLE
              );
    ADC1_StartConversion();
}
   
////////////////////////////
//主函数入口
/////////////////////////////

void main(void)       
{
    Init_HSI();
    Init_AWU();
    UART2_INIT();
    Speaker_Init();
    Key_Init();    
    ///////////////////////////////ADC1 init
    Init_ADC1();
    //I2c_Init();
    EEPROM_Init();
   
    TIM2_Init();
    enableInterrupts();

    MachData.ADCChip = CS1231; 
    if(MachData.ADCChip == CS1231)
        CS1231_Init();
    else
        CS1237_Init();

    ////////////////////////////ht1621 init 
#if (DISPLAY_TYPE == LCD)
    HT1621_Init();
    //BkLight_On();
#else
    TM1668_Init();
#endif  

    /////////////////////////////////////////////////////////////////////////	
    MachData.mode = System_Init();
    RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;

    if((MACHINE_NORMAL_MODE+MACHINE_FACTORY_MODE) == MachData.mode) {
        FactoryData.factorystep = FAC_EXIT;
    }
    FactoryGetFirstStepIndex(); //use for cal info
    Normal_Proc();
    
	while(1){;}
}  
 
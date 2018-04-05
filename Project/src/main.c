#include "stm8s_conf.h"

#include "stdio.h" 
#include "global.h"
#include "TM1668.h"
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
    u8  i;
 
    Init_HSI();
    Speaker_Init();
    Key_Init();
  
    CS1231_Init();
    ///////////////////////////////ADC1 init
    Init_ADC1();
    I2c_Init();
    UART2_INIT();

    TIM2_Init();
    enableInterrupts();
  
    ////////////////////////////ht1621 init 
#if (DISPLAY_TYPE == LCD)
    HT1621_Init();
    //BkLight_On();
#else
    TM1668_Init();
#endif
    /////////////////////////////////////////////////////////////////////////	
    MachData.mode = System_Init();

    if((MACHINE_NORMAL_MODE+MACHINE_FACTORY_MODE) == MachData.mode) {
        FactoryData.factorystep = FAC_FULL;
        FactoryGetFirstStepIndex();
    } else if((MACHINE_NORMAL_MODE+MACHINE_LINECAL_MODE) == MachData.mode) {
        CalData.linecalstart = 1;
        CalData.linecalstep = 1;
    }
    
    Normal_Pro();
    
	while(1){;}
}  
 
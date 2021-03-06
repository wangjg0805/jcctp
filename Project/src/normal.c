#include "stm8s_conf.h"

#include <math.h>
#include "stdio.h" 
#include "global.h"
#include "ad_filter.h"
#include "CPUled.h"
#include "lowpower.h"
#include "cs1231.h"


static void delay(u32 length)
{
    while(length >0)
        length--;
}

void Normal_Proc(void)
{
    u16 i;
    
    while(1){
        
        if(0 == ExitLpmodeflag) {
            if(1==LPmode_Check()) { //exit from lpmode
                ExitLpmodeflag = 1;  
                if(MachData.ADCChip == CS1237)
                    CS1237_ReInit();
                else
                    delay(1000);
            }
        }
      
        if(MachData.ADCChip == CS1231) {
            if(RESET == READ_CS1231_SDO){
                if(1 == CS1237_Read()) {
                    if(1==ExitLpmodeflag) 
                        ExitLpmodeflag++;
                    else {
                        ad_filter(MData.hx711_data);
                        MData_update_normal();
                        ExitLpmodeflag = 0;
                    }   
                }  
            }
        } else { 
        //the first data is not stable when exit from lp
        //so discard it and sample from 2th data        
            if(RESET == READ_CS1231_SDO){
                if(1 == CS1237_Read()) {
                    if(1==ExitLpmodeflag) 
                        ExitLpmodeflag++;
                    else {
                        ad_filter(MData.hx711_data);
                        MData_update_normal();
                        ExitLpmodeflag = 0;
                    }   
                }    
            }
        }
        
        if(Flag_10ms) {
            Flag_10ms = 0;
            Key_Scan();
            Speaker_Proc();
	    }
        
        if(Flag_100ms) {
            Flag_100ms = 0;
            i = Key_GetCode();
            if(0 != i) {
                RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
                switch(MachData.mode) {
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    Key_Proc_Factory(i);
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_USERCAL1_MODE:
                case MACHINE_NORMAL_MODE + MACHINE_USERCAL2_MODE:
                    Key_Proc_UserCal(i);
                    break;
                case MACHINE_NORMAL_MODE:
                    if(3 == MachData.keytype)
                        Key_Proc_3(i);
                    else
                        Key_Proc_4(i);
                    break;
                default:
                    break;
                }  
            }
                        
            switch(MachData.mode) {
            case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                TM1668_Display_Factory();
                break;
            case MACHINE_NORMAL_MODE + MACHINE_USERCAL1_MODE:
            case MACHINE_NORMAL_MODE + MACHINE_USERCAL2_MODE:
                TM1668_Display_UserCal();                  
                break;
            case MACHINE_NORMAL_MODE:
                TM1668_Display_Normal();  
                break;
            default:
                    break;
            }
        }
            
        if(1 == Flag_500ms) {
            Flag_500ms = 0; 
            RunData.no_key_time++;      //500ms 
            RunData.keep_zero_time++;
            RunData.not_zero_time++;
            if(STAT_CALCOUNTDOWN == RunData.current_mode) {
                RunData.CalCountDown_time--; 
                //printf("RunData.CalCountDown_time:%d \r\n",RunData.CalCountDown_time);
                if(0 == RunData.CalCountDown_time)
                    Key_EnterCal();
            }
         
            if(ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
                Battery_Filter(ADC1_GetConversionValue());
                Battery_Get();
            }
            
        //printf("weigh_ad_zero is: %ld \r\n",MData.weigh_ad_zero);
        } 
    }  
}





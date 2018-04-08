#include "stm8s_conf.h"

#include <math.h>
#include "stdio.h" 
#include "global.h"
#include "ad_filter.h"
#include "CPUled.h"


void LPmode_Check(void) 
{
    u8 tmp;
    tmp = 0; //nokey time & keepzero time MUST BE SATISFIED WITH THE PARAM
    if((RunData.no_key_time > MachData.weigh_lptime)&&(RunData.keep_zero_time > MachData.weigh_lptime))
        tmp++;
    //if((9!=MachData.weigh_bkofftime)&&(0!=MachData.weigh_bkofftime)&&(RunData.not_zero_time > (MachData.weigh_bkofftime*60*60*2)))
    //    tmp++;
    
    if(0==tmp) {
        RunData.lowpower_flag = 0;
    } else {
        RunData.lowpower_flag = 1;
    }
}

void Normal_Proc(void)
{
    u16 i;
    u8 NewDataFlag = 0;
    
    while(1){
        
         if(RESET == READ_CS1231_SDO) {
             if(1 == CS1231_Read()) {
                 ad_filter(MData.hx711_data);
                 NewDataFlag = 1;
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
                RunData.no_key_time = 0;
                switch(MachData.mode) {
                case MACHINE_NORMAL_MODE + MACHINE_LINECAL_MODE:
                    Key_Proc_Linecal(i);
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    Key_Proc_Factory(i);
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_USERCAL_MODE:
                    Key_Proc_UserCal(i);
                    break;
                case MACHINE_NORMAL_MODE:
                    Key_Proc(i);
                    break;
                default:
                    break;
                }  
            }
            
            if(1 == NewDataFlag) {
                NewDataFlag = 0;
                switch(MachData.mode) {
                case MACHINE_NORMAL_MODE + MACHINE_LINECAL_MODE:
                    MData_update_normal();
                    TM1668_Display_LineCal();
                    break;    
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    MData_update_normal();
                    TM1668_Display_Factory();
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_USERCAL_MODE:
                    //MData_update_cal();
                    TM1668_Display_UserCal();                  
                    break;
                case MACHINE_NORMAL_MODE:
                    MData_update_normal();
                    TM1668_Display_Normal();  
                    break;
                default:
                    break;
                }
            }
        }
            
        if(1 == Flag_500ms) {
            Flag_500ms = 0; 
            LPmode_Check();
            if(ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
                Battery_Filter(ADC1_GetConversionValue());
                Battery_Get();
            }
            
            //printf("weigh_ad_zero is: %ld \r\n",MData.weigh_ad_zero);
            //UART2_SendData("dat_avg",MData.ad_dat_avg);
            //UART2_SendData("ad_zero",MData.weigh_ad_zero);
        } 
    }  
}





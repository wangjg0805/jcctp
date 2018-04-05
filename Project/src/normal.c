#include "stm8s_conf.h"

#include <math.h>
#include "stdio.h" 
#include "global.h"

#if 0
void BkLight_Proc(void) 
{
    u8 tmp;
    tmp = 0;
    if((RunData.no_key_time > NO_KEY_BKOFF_TIME)&&(RunData.keep_zero_time > KEEPZERO_BKOFF_TIME))
        tmp++;
    if((9!=MachData.weigh_bkofftime)&&(0!=MachData.weigh_bkofftime)&&(RunData.not_zero_time > (MachData.weigh_bkofftime*60*60*2)))
        tmp++;
    
    if(0==tmp) {
        RunData.lowpower_flag = 0;
        BkLight_On();
    } else {
        RunData.lowpower_flag = 1;
        BkLight_Off();
    }
}
#endif

void Normal_Pro(void)
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
                    RunData.no_key_time = 0;
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
                    if(LCD == DISPLAY_TYPE)
                        Display_LineCal();
                    else
                        TM1668_Display_LineCal();
                    break;    
                case MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE:
                    MData_update_normal();
                    if(LCD == DISPLAY_TYPE)
                        Display_Factory();
                    else
                        TM1668_Display_Factory();
                    break;
                case MACHINE_NORMAL_MODE + MACHINE_USERCAL_MODE:
                    MData_update_cal();
                    if(LCD == DISPLAY_TYPE)
                       Display_UserCal();
                    else
                        TM1668_Display_UserCal();
                       
                    break;
                case MACHINE_NORMAL_MODE:
                    MData_update_normal();
                    if(LCD == DISPLAY_TYPE)
                        Display_Normal();
                    else
                        TM1668_Display_Normal();
                    
                    break;
                default:
                    break;
                }
            }
        }
            
        if(1 == Flag_500ms) {
            Flag_500ms = 0; 
            //BkLight_Proc();
            //printf("ad_dat_avg is: %ld \r\n",MData.ad_dat_avg);
            //printf("weigh_ad_zero is: %ld \r\n",MData.weigh_ad_zero);
            //UART2_SendData("dat_avg",MData.ad_dat_avg);
            //UART2_SendData("ad_zero",MData.weigh_ad_zero);
        } 
    }  
}





#include "stm8s_conf.h"
#include "lowpower.h"
#include "TM1668.h"
#include "global.h"
#include "ad_filter.h"


static u8 LPPhase = 0;

void Init_AWU(void)
{
    AWU_DeInit();
    //AWU_Init(AWU_TIMEBASE_2S);
    AWU_Init(AWU_TIMEBASE_512MS);
    AWU_Cmd(DISABLE);
}

void Sys_LPEnable(void)
{
    u8 i;
    
    LPPhase++;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_NULL[i]]; 
    if(LPPhase%3 == 0)
        display_buffer[0] = display_code[DISP_X];
    
    display_buffer[6] = 0x00; //
    MData_update_LED();
    TM1668_Update();

    AWU_Cmd(ENABLE);
    //CLK_SlowActiveHaltWakeUpCmd(ENABLE);
    halt();
}

void Sys_LPDisable(void)
{
    LPPhase = 0;
    //AWU_Cmd(DISABLE);
}


void LPmode_Check(void) 
{
    u8 tmp;
    tmp = 0; //nokey time & keepzero time MUST BE SATISFIED WITH THE PARAM
    
    if(RunData.no_key_time > MachData.weigh_lptime)
        tmp++;
    if(RunData.keep_zero_time > MachData.weigh_lptime)
        tmp++;
    if(0 == Filt_GetFastFilterFlag())
        tmp++;
    //if((9!=MachData.weigh_bkofftime)&&(0!=MachData.weigh_bkofftime)&&(RunData.not_zero_time > (MachData.weigh_bkofftime*60*60*2)))
    //    tmp++;
    
    if(3==tmp) {
        RunData.lowpower_flag = 1;
        printf("Enter LPmode \r\n");
        Sys_LPEnable();
    } else {        
        RunData.lowpower_flag = 0;
        Sys_LPDisable();
    }
}
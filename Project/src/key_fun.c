#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "ad_filter.h"
#include "factory.h"
//*************************************************************
const u16 CountList[] = {10,20,50,100,200,500,900,0};
static u8 CouIndex = 0;

void Key_ReleasedProc(void)
{
    if(STAT_CALCOUNTDOWN == RunData.current_mode) {
        RunData.current_mode = STAT_WEIGHT;
        RunData.CalCountDown_time = 0;
    }
}


void Key_CalCountDownProc(void)
{
    if(STAT_CALCOUNTDOWN != RunData.current_mode) {
        RunData.current_mode = STAT_CALCOUNTDOWN;
        RunData.CalCountDown_time = 3*2 + 1;
    }
}


void Key_Cal2Proc(void) 
{
    if(MachData.mode == MACHINE_NORMAL_MODE) {
        CalData.calstep = CAL_TIP;
        MachData.mode = MACHINE_NORMAL_MODE+MACHINE_USERCAL2_MODE;
        manual_break_stable();
    }
}

void Key_UnitProc(void)
{
    
    if(STAT_BATTERY == RunData.current_mode)
        RunData.current_mode = STAT_WEIGHT;
    
}        

void Key_LongPCSProc(void)
{
    //additional function for debug //
    if(MachData.mode == MACHINE_NORMAL_MODE)
        RunData.current_mode = STAT_BATTERY;

}

void Key_PCSProc(void)
{
    u32 i;
    
    if(STAT_WEIGHT == RunData.current_mode) {
        RunData.current_mode = STAT_PCS_SAMPLE;
        CouIndex = 0;
        RunData.PCSSample = CountList[CouIndex];
    } else if(STAT_PCS_SAMPLE == RunData.current_mode) {
        i = MData.ad_dat_avg - MData.ad_zero_data - MData.ad_tare_data; 
        if(i > RunData.PCSSample * FilterData.ad_filter_para) {
            RunData.PCSCoef = (i+0.1) / RunData.PCSSample ;
            RunData.current_mode = STAT_PCS;
        } else {
            RunData.current_mode = STAT_PCS_ERR;
        }
    } else if(STAT_PCS == RunData.current_mode) {
        RunData.current_mode = STAT_WEIGHT;
    } else {
        ;
    }
}


void Key_Cal1Proc(void)
{
    if(STAT_WEIGHT == RunData.current_mode) {
        CalData.calstep = CAL_TIP;
        MachData.mode = MACHINE_NORMAL_MODE+MACHINE_USERCAL1_MODE;
        manual_break_stable();
    }
}

void Key_TareProc(void)
{
    if(STAT_PCS_SAMPLE == RunData.current_mode) {
        CouIndex++;
        if(0 == CountList[CouIndex])
            CouIndex = 0;
        RunData.PCSSample = CountList[CouIndex];
    } else if(STAT_WEIGHT == RunData.current_mode){ //only in weigh mode
        RunData.do_zero_flag = 1;
    }
}        

void Key_Proc_3(u16 key)
{
    
    if(KEY_RELEASED == key)
        Key_ReleasedProc();
    
    if(KEY_PRESSING==(key&0xff00))
        RunData.key_sound_time = 0;
       
    switch(key)
    {
    //ADD PRESSING STATUS
    case KEY_PRESSING+KEY_TARECAL:
    case KEY_PRESSING+KEY_UNITMODE:
        Key_CalCountDownProc();
        break;
        
    case KEY_PRESSED+KEY_UNITMODE:
        Key_UnitProc();
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_PCSProc();
        break;        
    case KEY_PRESSED+KEY_TARECAL:
        Key_TareProc();
        break;
        
    case KEY_PRESSED_3S + KEY_TARECAL:
        Key_Cal1Proc(); 
        break;
    case KEY_PRESSED_3S + KEY_PCSCONFIRM:
        Key_LongPCSProc(); 
        break;
    case KEY_PRESSED_3S + KEY_UNITMODE:
        Key_Cal2Proc(); 
        break;
        
    default:
        break;
    }
}


void Key_Proc_4(u16 key)
{
    if(KEY_RELEASED == key)
        Key_ReleasedProc();
    if(KEY_PRESSING==(key&0xff00))
        RunData.key_sound_time = 0; 
    
    switch(key)
    {
        
    case KEY_PRESSING+KEY_CAL:
    case KEY_PRESSING+KEY_UNITMODE:
        Key_CalCountDownProc();
        
        break;    
        
    case KEY_PRESSED+KEY_UNITMODE:
        Key_UnitProc();
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_PCSProc();
        break;        
    case KEY_PRESSED+KEY_TARECAL:
        Key_TareProc();
        break;
      
    case KEY_PRESSED_3S+KEY_CAL:
        Key_Cal1Proc();  
        break;   
    case KEY_PRESSED_3S + KEY_PCSCONFIRM:
        Key_LongPCSProc(); 
        break;
    case KEY_PRESSED_3S + KEY_UNITMODE:
        Key_Cal2Proc(); 
        break;
        
    default:
        break;
    }
}


void Key_Proc_Factory(u16 key)
{
    
    switch(key){
    case KEY_PRESSED+KEY_UNITMODE:
        Key_FactoryUnitProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_FactoryPCSProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break;   
    case KEY_PRESSED+KEY_TARECAL:
        Key_FactoryTareProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break;
        
    default:
        break;
    }
}

void Key_Proc_UserCal(u16 key)
{
    switch(key){
    case KEY_PRESSED+KEY_UNITMODE:
        //Key_UserCalUnitProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_UserCalPCSProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break; 
    case KEY_PRESSED+KEY_TARECAL:
        //Key_UserCalTareProc();
        //RunData.key_sound_time = KEY_NORMAL_SOUND_TIME;
        break;   
    default:
        break;
    }
}








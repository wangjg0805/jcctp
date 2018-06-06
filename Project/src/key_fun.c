#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"
#include "ad_filter.h"
#include "factory.h"
//*************************************************************
const u16 CountList[] = {10,20,50,100,200,500,900,0};
static u8 CouIndex = 0;

void Key_LongUnitProc(void) 
{
    if(MachData.mode == MACHINE_NORMAL_MODE) {
        CalData.calstep = CAL_WAIT_ZERO;
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


void Key_LongTareProc(void)
{
    if(MachData.mode == MACHINE_NORMAL_MODE) {
        CalData.calstep = CAL_WAIT_ZERO;
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
    } else {
        RunData.do_zero_flag = 1;
    }
}        

void Key_Proc(u16 key)
{

    switch(key)
    {
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
        Key_LongTareProc(); 
        break;
    case KEY_PRESSED_3S + KEY_PCSCONFIRM:
        Key_LongPCSProc(); 
        break;
    case KEY_PRESSED_3S + KEY_UNITMODE:
        Key_LongUnitProc(); 
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
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_FactoryPCSProc();
        break;   
    case KEY_PRESSED+KEY_TARECAL:
        Key_FactoryTareProc();
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
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_UserCalPCSProc();
        break; 
    case KEY_PRESSED+KEY_TARECAL:
        //Key_UserCalTareProc();
        break;   
    default:
        break;
    }
}








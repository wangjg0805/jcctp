#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"

//*************************************************************
const u16 CountList[] = {10,20,50,100,200,500,900,0};
static u8 CouIndex = 0;


void Key_UnitProc(void)
{
    if(STAT_BATTERY == RunData.current_mode) {
        RunData.current_mode = STAT_WEIGHT;
    } else {
        ;
    }
}        


void Key_LongUnitProc(void)
{
    //additional function for debug
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
    //if(STAT_WEIGHT == RunData.current_mode) {    
        CalData.usercalstart = 1;
        CalData.usercalstep = 1;
        MachData.mode = MACHINE_NORMAL_MODE+MACHINE_USERCAL_MODE;
    //}
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
    case KEY_PRESSED_3S + KEY_UNITMODE:
        Key_LongUnitProc(); 
        break;
        
    default:
        break;
    }
}

//linecal key proc added
void Key_LineCalUnitProc(void)
{

}


void Key_LineCalPCSProc(void)
{
    switch(CalData.linecalstep) {
    case 1:  //save zero
    case 2:  //save 250
    case 3:  //save 500
        //MachData.weigh_linecalbuf[CalData.linecalstep-1] = MData.ad_dat_avg;
        CalData.linecalstep++;
        break;
    case 4:
        
        MachData.mode = MACHINE_NORMAL_MODE;
    default:
        break;
    }
}


void Key_LineCalTareProc(void)
{

}


void Key_Proc_Linecal(u16 key)
{
    switch(key)
    {
    case KEY_PRESSED+KEY_UNITMODE:
        Key_LineCalUnitProc();
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_LineCalPCSProc();
        break;        

    case KEY_PRESSED+KEY_TARECAL:
        Key_LineCalTareProc();
        break;        
    default:
        break;
    }
}






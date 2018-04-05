#include "stm8s.h"
#include "math.h"
#include "stdio.h"

#include "global.h"

//*************************************************************

void Key_UnitProc(void)
{
    
}        
        
void Key_PCSProc(void)
{

    
}


void Key_LongTareProc(void)
{    
    CalData.usercalstart = 1;
    CalData.usercalstep = 1;
    MachData.mode = MACHINE_NORMAL_MODE+MACHINE_USERCAL_MODE;
}

void Key_TareProc(void)
{
    RunData.do_tare_flag = 1;
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






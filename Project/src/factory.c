#include "stm8s_conf.h"
#include "stdio.h" 
#include "stdlib.h"
#include "math.h"

#include "global.h"

#include "timer1.h"

const u8 weigh_fullrange[] =     {1,  3,   6, 15,  30,  60,  100,   0}; //*1000
const u8 weigh_onestep[] =       {1,  1,   2,  5,  10,  20,  50,  100,  0};
const u8  weigh_dot[] =          {1,  1,   2,  3,   4,   0};
const u8  weigh_displaymin[] =   {1,  1,   2,  3,   4,   5,  0};
const u8  weigh_bkofftime[] =    {1,  1,   2,  3,   4,   9,  0};
const u8  poweron_zerorange[] =  {1,  4,  10,  20, 50, 100,  0};  
const u8  auto_loadtrackrange[] ={1,  1,   2,  3,   4,   5,  0}; 

void Key_UserCalUnitProc(void)
{

}

void Key_UserCalTareProc(void)
{

}


void Key_UserCalPCSProc(void)
{
    u8 buf[8];
    
    switch(CalData.usercalstep) {
    case 1:
        if(MData.ad_dat_avg < MACHINE_ZERO_AD_MIN) {
            CalData.usercalstep = 11; // zero too small
        } else if(MData.ad_dat_avg > MACHINE_ZERO_AD_MAX) {
            CalData.usercalstep = 12;
        } else {
            CalData.usercalstep++;
            MachData.ad_zero_data = MData.ad_dat_avg;
            MData.ad_zero_data = MData.ad_dat_avg;
            U32toBUF(MachData.ad_zero_data,buf);
            buf[4] = buf[0];
            buf[5] = buf[1];
            buf[6] = buf[2];
            buf[7] = buf[3];
            Write_EEPROM(EEP_WEIGHTZERO_ADDR,buf,8);
            buf[0] = CHECK_DATA;
            buf[1] = CHECK_DATA;        
            Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //erase caldata
        }
        break;
    case 2:
        if(labs(MData.ad_dat_avg-MachData.ad_zero_data) < MACHINE_LOAD_AD_MIN) {
            CalData.usercalstep = 13;
        } else if(labs(MData.ad_dat_avg-MachData.ad_zero_data) > MACHINE_LOAD_AD_MAX) {
            CalData.usercalstep = 14;
        } else {
            CalData.usercalstep++;
            MachData.weigh_ad_full = MData.ad_dat_avg -  MachData.ad_zero_data;
            MachData.weigh_coef = MachData.weigh_fullrange /  (MachData.weigh_ad_full + 0.1);
            U32toBUF(MachData.weigh_ad_full,buf);
            buf[4] = buf[0];
            buf[5] = buf[1];
            buf[6] = buf[2];
            buf[7] = buf[3]; 
            Write_EEPROM(EEP_WEIGHTFULL_ADDR,buf,8);
            buf[0] = CHECK_DATA;
            buf[1] = CHECK_DATA;        
            Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //caldata ok flag
        }
        break;
    case 3: 
    case 11:
    case 12:
    case 13:
    case 14:        
        CalData.usercalstart = 0;
        CalData.usercalstep = 0;
        MachData.mode = MachData.mode - MACHINE_USERCAL_MODE;
        break;
    default:
        break;
    } 
}

//key proc when factory mode
void Key_FactoryUnitProc(void)
{
    FactoryData.factoryindex++;
    switch(FactoryData.factorystep) {
    case FAC_FULL:
        if(weigh_fullrange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
    case FAC_FENDU:
        if(weigh_onestep[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
    case FAC_DOT:
        if(weigh_dot[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
    case FAC_DISPLAYMIN:
        if(weigh_displaymin[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
/*        
    case 5:
        if(weigh_bkofftime[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
    case 6:
        if(poweron_zerorange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
    case 7:
        if(auto_loadtrackrange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
*/        
        
    default:
        break;
    }
}


void Key_FactoryTareProc(void)
{
   FactoryData.factorystep++;
   if(FactoryData.factorystep == FAC_MAX)
       FactoryData.factorystep = FAC_FULL;
   
   switch(FactoryData.factorystep) {
   case FAC_FULL: 
       FactoryGetFirstStepIndex();
       break;
   case FAC_FENDU:
       switch(MachData.weigh_onestep) {
       case 1:    FactoryData.factoryindex = 1;break;
       case 2:    FactoryData.factoryindex = 2;break;
       case 5:    FactoryData.factoryindex = 3;break;
       case 10:   FactoryData.factoryindex = 4;break;
       case 20:   FactoryData.factoryindex = 5;break;
       case 50:   FactoryData.factoryindex = 6;break;
       case 100:  FactoryData.factoryindex = 7;break;
       default:   FactoryData.factoryindex = 1;break;
       }
       break;
   case FAC_DOT:
       FactoryData.factoryindex = MachData.weigh_dotpos;
       break;
   case FAC_DISPLAYMIN:
       FactoryData.factoryindex = MachData.weigh_displaymin;
       break;
/*       
   case FAC_:
       switch(MachData.weigh_bkofftime) {
       case 1:    FactoryData.factoryindex = 1;break;
       case 2:    FactoryData.factoryindex = 2;break;
       case 3:    FactoryData.factoryindex = 3;break;
       case 4:    FactoryData.factoryindex = 4;break;
       case 9:    FactoryData.factoryindex = 5;break;
       default:   FactoryData.factoryindex = 1;break;
       }
       break;
   case 6:
       switch(MachData.dozerorange) {
       case 4:    FactoryData.factoryindex = 1;break;
       case 10:   FactoryData.factoryindex = 2;break;
       case 20:   FactoryData.factoryindex = 3;break;
       case 50:   FactoryData.factoryindex = 4;break;
       case 100:  FactoryData.factoryindex = 5;break;
       default:   FactoryData.factoryindex = 1;break;
       }
       break;
   case 7:
       FactoryData.factoryindex = MachData.loadtrackrange;
       break;
*/
   default:
       FactoryData.factoryindex = 1;
   }
}
   
void Key_FactoryPCSProc(void)
{
    u8 buf[4];
    switch(FactoryData.factorystep) {
    case FAC_FULL:
        MachData.weigh_fullrange = weigh_fullrange[FactoryData.factoryindex] * 1000;
        U32toBUF(MachData.weigh_fullrange,buf);
        Write_EEPROM(EEP_SYS_FULLRANGE_ADDR,buf,4);
        break;
    case FAC_FENDU:
        MachData.weigh_onestep = weigh_onestep[FactoryData.factoryindex];
        U32toBUF(MachData.weigh_onestep,buf);
        Write_EEPROM(EEP_SYS_ONESTEP_ADDR,buf,4);
        break;  
    case FAC_DOT:
        MachData.weigh_dotpos = weigh_dot[FactoryData.factoryindex];
        U32toBUF(MachData.weigh_dotpos,buf);
        Write_EEPROM(EEP_SYS_DOT_ADDR,buf,4);
        break;
    case FAC_DISPLAYMIN:
        MachData.weigh_displaymin = weigh_displaymin[FactoryData.factoryindex];
        U32toBUF(MachData.weigh_displaymin,buf);
        Write_EEPROM(EEP_SYS_DISFENDU_ADDR,buf,4);
        break;
    case FAC_EXIT:    
        MachData.mode -= MACHINE_FACTORY_MODE; 
        break;
/*        
    case 5:
        MachData.weigh_bkofftime = weigh_bkofftime[FactoryData.factoryindex];
        U32toBUF(MachData.weigh_bkofftime,buf);
        Write_EEPROM(EEP_SYS_BKOFFTIME_ADDR,buf,4);
        break;  
    case 6:
        MachData.dozerorange = poweron_zerorange[FactoryData.factoryindex];
        U32toBUF(MachData.dozerorange,buf);
        Write_EEPROM(EEP_SYS_ZERORANGE_ADDR,buf,4);
        break;
    case 7:
        MachData.loadtrackrange = auto_loadtrackrange[FactoryData.factoryindex];
        U32toBUF(MachData.loadtrackrange,buf);
        Write_EEPROM(EEP_SYS_LOADTRACK_ADDR,buf,4);
        break;
    case 8:
        MachData.mode -= MACHINE_FACTORY_MODE;
*/        
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
    case KEY_PRESSED+KEY_TARECAL:
        Key_FactoryTareProc();
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_FactoryPCSProc();
        break;   
    default:
        break;
    }
}

void Key_Proc_UserCal(u16 key)
{
    switch(key){
    case KEY_PRESSED+KEY_UNITMODE:
        Key_UserCalUnitProc();
        break;
    case KEY_PRESSED+KEY_PCSCONFIRM:
        Key_UserCalPCSProc();
        break; 
    case KEY_PRESSED+KEY_TARECAL:
        Key_UserCalTareProc();
        break; 
        
    default:
        break;
    }
}
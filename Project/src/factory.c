#include "stm8s_conf.h"
#include "stdio.h" 
#include "stdlib.h"
#include "math.h"

#include "global.h"
#include "i2c.h"
#include "timer1.h"

const u32 weigh_calmode1[] =      {1,   1500,  3000,  10000,  15000,  25000,  30000,   50000,  100000,  500000,  500000,   500000,    0};
const u32 weigh_calmode2[2][20]= {{1,   1500,  3000,  10000,  15000,  25000,  30000,   50000,  100000,  500000,  500000,   500000,    0},
                                  {1,   3000,  6000,  20000,  30000,  50000,  60000,  100000,  200000,  500000,  500000,   500000,    0}};

const u8 weigh_fullrange[] =      {1,      3,     6,     10,     20,     30,     50,      60,      75,     100,     150,      200,    0}; //*1000

const u8 weigh_onestep[] =       {1,   1,    2,   5,   10,  20,  50,  100,  0};
const u8 weigh_dot[] =           {1,   1,    2,   3,    4,   5,   0};
const u8 weigh_displaymin[] =    {1,   1,    2,   3,    4,   5,   0};
const u8 auto_loadtrackrange[] = {1,   1,    2,   3,    4,   5,   9,    0};
const u8 weigh_lptime[] =        {1,   5,   15,  30,   60,  99,   0};
const u8 poweron_zerorange[] =   {1,   4,   20,  50,  100, 200,   0};  
const u8 key_count[] =           {1,   3,    4,  0}; 

static u8 Time180sCount = 0;

static void SaveToE2prom(u32 data, u16 addr,u8 len)
{
    u8 buf[8];
    
    U32toBUF(data,buf);
    buf[4] = buf[0];
    buf[5] = buf[1];
    buf[6] = buf[2];
    buf[7] = buf[3]; 
    Write_EEPROM(addr,buf,len);
}


void Save_Load2CalData(void)
{
    u8 buf[2];
    SaveToE2prom(MachData.weigh_ad_full,EEP_WEIGHTFULL2_ADDR,8);
     
    buf[0] = CHECK_DATA;
    buf[1] = CHECK_DATA;        
    Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);    
}


void Key_CalExit(void)
{
    MachData.mode = MACHINE_NORMAL_MODE;
    CalData.calstep = CAL_NULL;  
    RunData.keep_zero_time = 0;
    //printf("lpmodeflag: %d \r\n",RunData.lowpower_flag);
}


void Key_UserCalPCSProc(void)
{
    if(CAL_LOAD2_FLASH == CalData.calstep) {
        //update the second 
        MachData.weigh_ad_full = MachData.weigh_ad_Middle * 2;
        Save_Load2CalData();
        Init_UserCalParam();
    }
        
    Key_CalExit();  
}



void LoadFlashExitCheck(void)
{
    if(Flag_5s) {
        Flag_5s = 0;
        Time180sCount += 5;
        if(180 == Time180sCount) {
            Time180sCount = 0;
            Key_UserCalPCSProc();
        }
    }
}


void Key_CalProc1(void)
{
    u8 buf[8];
    
    if(labs(MData.ad_dat_avg-MData.ad_zero_data) < MACHINE_LOAD1_AD_MIN) {
        CalData.calstep = CAL_LOAD1_TOO_SMALL;
    } else if(labs(MData.ad_dat_avg-MData.ad_zero_data) > MACHINE_LOAD1_AD_MAX) {
        CalData.calstep = CAL_LOAD1_TOO_BIG;
    } else {
        CalData.calstep = CAL_PASS1;
        MachData.weigh_ad_Middle = MData.ad_dat_avg -  MData.ad_zero_data;  //use 1/2 
        MachData.weigh_ad_full = MachData.weigh_ad_Middle * 2; //use half of fullload 
        
        SaveToE2prom(MachData.weigh_ad_Middle,EEP_WEIGHTFULL1_ADDR,8);
        SaveToE2prom(MachData.weigh_ad_full,  EEP_WEIGHTFULL2_ADDR,8);
        buf[0] = CHECK_DATA;
        buf[1] = CHECK_DATA;        
        Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //caldata ok flag
        
        Init_UserCalParam();
    }    
}


void Key_CalProc2(void)
{   
    switch(CalData.calstep) {
    case CAL_LOAD1:
        if(labs(MData.ad_dat_avg-MData.ad_zero_data) < MACHINE_LOAD2A_AD_MIN) {
            CalData.calstep = CAL_LOAD1_TOO_SMALL;
        } else if(labs(MData.ad_dat_avg-MData.ad_zero_data) > MACHINE_LOAD2A_AD_MAX) {
            CalData.calstep = CAL_LOAD1_TOO_BIG;
        } else {
           CalData.calstep = CAL_PASS1;
           MachData.weigh_ad_Middle = MData.ad_dat_avg -  MData.ad_zero_data;
           SaveToE2prom(MachData.weigh_ad_Middle,EEP_WEIGHTFULL1_ADDR,8);
        }
        break;
    case CAL_LOAD2:
        /*
        if(labs(MData.ad_dat_avg-MData.ad_zero_data) < MACHINE_LOAD2B_AD_MIN) {
            CalData.calstep = CAL_LOAD2_TOO_SMALL;
        } else if(labs(MData.ad_dat_avg-MData.ad_zero_data) > MACHINE_LOAD2B_AD_MAX) {
            CalData.calstep = CAL_LOAD2_TOO_BIG;
        } else {
        */
           CalData.calstep = CAL_PASS2;
           MachData.weigh_ad_full = MData.ad_dat_avg -  MData.ad_zero_data;
           Save_Load2CalData();
           //update weigh coef
           Init_UserCalParam();
        //}
        break;
    default:
        break;
    }
}

//done it automatic 
void UserCalAutoProc(void)
{
    static u8 cnt = 0;
    u8 buf[8];
  
    switch(CalData.calstep) {
    case CAL_TIP:
    case CAL_PASS2:
        cnt++;
        if(cnt > 10) {
            cnt = 0;
            CalData.calstep++;
        }
        break;        
    case CAL_PASS1:
        cnt++;
        if(cnt > 10) {
            cnt = 0;
            if(MachData.mode == MACHINE_NORMAL_MODE+MACHINE_USERCAL1_MODE)
                CalData.calstep = CAL_OVER;
            else {
                CalData.calstep = CAL_LOAD2_FLASH;
                Time180sCount = 0;
            }
        }
        break;
    case CAL_WAIT_ZERO:  //waiting for stable ,get 0
        if(MData.ad_dat_avg < MACHINE_ZERO_AD_MIN) {
            CalData.calstep = CAL_ZERO_TOO_SMALL; // zero too small
        } else if(MData.ad_dat_avg > MACHINE_ZERO_AD_MAX) {
            CalData.calstep = CAL_ZERO_TOO_BIG;
        } else {
            CalData.calstep = CAL_LOAD1_FLASH;
            Time180sCount = 0;
            MData.ad_zero_data = MData.ad_dat_avg;
            U32toBUF(MData.ad_zero_data,buf);
            printf("zero data:0x%x,0x%x,0x%x,0x%x \r\n",buf[0],buf[1],buf[2],buf[3]);
            buf[4] = buf[0];
            buf[5] = buf[1];
            buf[6] = buf[2];
            buf[7] = buf[3];
            Write_EEPROM(EEP_WEIGHTZERO_ADDR,buf,8);
            //buf[0] = 0;
            //buf[1] = 0;        
            //Write_EEPROM(EEP_CALFLAG_ADDR,buf,2);  //erase caldata
        }
        break;
    case CAL_LOAD1_FLASH:     
        //if(labs(MData.ad_dat_avg-MData.ad_zero_data) > MachData.weigh_division *2) //
        if(labs(MData.ad_dat_avg-MData.ad_zero_data) > 10000) //
            
            CalData.calstep = CAL_LOAD1;
        break;
            
    case CAL_LOAD1:
        if(MachData.mode == MACHINE_NORMAL_MODE+MACHINE_USERCAL1_MODE)
            Key_CalProc1(); // confirm fullrange
        else
            Key_CalProc2(); //
        break;
        
    case CAL_LOAD2_FLASH:
        //if(labs(MData.ad_dat_avg-MData.ad_zero_data) > (MachData.weigh_ad_Middle+ MachData.weigh_division *2))
        if(labs(MData.ad_dat_avg-MData.ad_zero_data) > (MachData.weigh_ad_Middle+10000))      
            CalData.calstep = CAL_LOAD2;
        break;
    case CAL_LOAD2:
        Key_CalProc2();
        break;
    case CAL_OVER:
        Key_CalExit();
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
    case FAC_LOADTRACK:
        if(auto_loadtrackrange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;

    case FAC_ZEROLIMIT:
        if(poweron_zerorange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;

    case FAC_KEYCOUNT:
        if(key_count[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
        
/*
    case 7:
        if(auto_loadtrackrange[FactoryData.factoryindex] == 0)
            FactoryData.factoryindex = 1;
        break;
*/        
        
    default:
        break;
    }
}



void FactoryGetFirstStepIndex(void)
{
    switch(MachData.weigh_fullrange) { 
    case 3000:    FactoryData.factoryindex = 1;  break;
    case 6000:    FactoryData.factoryindex = 2;  break;
    case 10000:   FactoryData.factoryindex = 3;  break;    
    case 20000:   FactoryData.factoryindex = 4;  break;
    case 30000:   FactoryData.factoryindex = 5;  break;
    case 50000:   FactoryData.factoryindex = 6;  break;
    case 60000:   FactoryData.factoryindex = 7;  break;
    case 75000:   FactoryData.factoryindex = 8;  break;    
    case 100000:  FactoryData.factoryindex = 9;  break;
    case 150000:  FactoryData.factoryindex = 10; break;    
    case 200000:  FactoryData.factoryindex = 11; break;
    default:      FactoryData.factoryindex = 1;  break;
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
   case FAC_LOADTRACK:
       if(9 == MachData.loadtrackrange)
           FactoryData.factoryindex = 6;
       else
           FactoryData.factoryindex = MachData.loadtrackrange;
       break;       
   case FAC_ZEROLIMIT:
       switch(MachData.dozerorange) {
       case 4:    FactoryData.factoryindex = 1;break;
       case 20:   FactoryData.factoryindex = 2;break;
       case 50:   FactoryData.factoryindex = 3;break;
       case 100:  FactoryData.factoryindex = 4;break;
       case 200:  FactoryData.factoryindex = 5;break;
       default:   FactoryData.factoryindex = 1;break;
       }
       break;
       
   case FAC_KEYCOUNT:
       switch(MachData.keytype) {
       case 3:  FactoryData.factoryindex = 1;break;
       case 4:  FactoryData.factoryindex = 2;break;
       default:
                FactoryData.factoryindex = 1;
           break;
       }
       break;
       
   default:
       FactoryData.factoryindex = 1;
   }
}
   
void Key_FactoryPCSProc(void)
{
    u8 buf[4];
    switch(FactoryData.factorystep) {
    case FAC_FULL:
        MachData.weigh_fullrange = weigh_fullrange[FactoryData.factoryindex] * 1000L;
        printf("weigh_fullrange :%ld \r\n", MachData.weigh_fullrange);
        U32toBUF(MachData.weigh_fullrange,buf);
        Write_EEPROM(EEP_SYS_FULLRANGE_ADDR,buf,4);
        break;
    case FAC_FENDU:
        MachData.weigh_onestep = weigh_onestep[FactoryData.factoryindex];
        U32toBUF(MachData.weigh_onestep,buf);
        Write_EEPROM(EEP_SYS_ONESTEP_ADDR,&buf[0],4);
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

    case FAC_LOADTRACK:
        MachData.loadtrackrange = auto_loadtrackrange[FactoryData.factoryindex];
        U32toBUF(MachData.loadtrackrange,buf);
        Write_EEPROM(EEP_SYS_LOADTRACK_ADDR,buf,4);
        break;
        
    case FAC_ZEROLIMIT:
        MachData.dozerorange = poweron_zerorange[FactoryData.factoryindex];
        U32toBUF(MachData.dozerorange,buf);
        Write_EEPROM(EEP_SYS_ZERORANGE_ADDR,buf,4);
        break;

    case FAC_KEYCOUNT:
        MachData.keytype = key_count[FactoryData.factoryindex];
        U32toBUF(MachData.keytype,buf);
        Write_EEPROM(EEP_SYS_KEYTYPE_ADDR,buf,4);
        break;
        
    case FAC_EXIT:    
        MachData.mode -= MACHINE_FACTORY_MODE; 
        break;
     
    default:
        break;
    }
}


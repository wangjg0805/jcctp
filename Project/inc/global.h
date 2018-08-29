//**********************************************************************
//文件名:global.h
//**********************************************************************
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm8s.h"

#include "timer1.h"
#include "CS1231.h"
#include "TM1668.h"
#include "rs232.h"
#include "i2c.h"
#include "key.h"
#include "key_fun.h"

typedef enum {
    LED = 0,
    LCD = 1,
}ScreenType;

typedef enum {
    CS1231 = 0,
    CS1237 = 1,
}ADCChipModel;

        
#define DISPLAY_TYPE            LED

#define MACHINE_NORMAL_MODE     0x01
#define MACHINE_FACTORY_MODE    0x02
#define MACHINE_USERCAL1_MODE   0x04
#define MACHINE_USERCAL2_MODE   0x08


#define MACHINE_AD_ZERO         10000
#define MACHINE_FULL_ZERO       1000000

#define MACHINE_ZERO_AD_MIN     3000
#define MACHINE_ZERO_AD_MAX     6000000
#define MACHINE_LOAD1_AD_MIN     20000
#define MACHINE_LOAD1_AD_MAX     10000000

#define MACHINE_LOAD2A_AD_MIN     20000
#define MACHINE_LOAD2A_AD_MAX     10000000
#define MACHINE_LOAD2B_AD_MIN     20000
#define MACHINE_LOAD2B_AD_MAX     10000000


#define POWER_ON_WAIT_TIME    50   //*100ms  max time: 5s
#define AUTOZERO_TRACK_TIME     8   //
#define AUTOZERO_TRACK_RANGE    3    //

#define AUTOLOAD_TRACK_RANGE    5    //

#define KEY_NORMAL_SOUND_TIME   (10) //*10ms = 100ms
#define KEY_LONG_SOUND_TIME     (20)
#define FULL_SOUND_TIME         (5)

#define FULL_STEP_NUM             9 //over 9D ,and full
#define NEG_FULL_NUM          99999 //-9999,and full

//ZERO RANGE
//#define POWER_ON_ZERO_RANGE     10  //10%
#define MANUAL_ZERO_RANGE       4   //4%

//MACH STAT
typedef enum {
    STAT_WEIGHT = 0,
    STAT_PCS_SAMPLE,
    STAT_PCS_ERR,  
    STAT_PCS,
    STAT_BATTERY,
}SYSStatus;

typedef enum {
    UNIT_KG = 0,
    UNIT_G,
    UNIT_LB,
    UNIT_OZ,
}UNITType;


typedef enum {
    FAC_NULL = 0,
    FAC_FULL,
    FAC_FENDU,
    FAC_DOT,
    FAC_DISPLAYMIN,
    FAC_LOADTRACK,
    FAC_ZEROLIMIT,
    //added 
    FAC_KEYCOUNT,
    
    FAC_EXIT,
    FAC_MAX,
    
}FactoryCfgType;

typedef enum {
    CAL_NULL = 0,
    CAL_TIP,
    CAL_WAIT_ZERO,
    CAL_LOAD1_FLASH,
    CAL_LOAD1,
    CAL_PASS1,    
    CAL_LOAD2_FLASH,
    CAL_LOAD2,
    CAL_PASS2,
    CAL_OVER,
    CAL_ZERO_TOO_SMALL = 11,
    CAL_ZERO_TOO_BIG,
    CAL_LOAD1_TOO_SMALL,
    CAL_LOAD1_TOO_BIG,
    CAL_LOAD2_TOO_SMALL,
    CAL_LOAD2_TOO_BIG,           
    CAL_RSV1,
    
}UserCalType;


typedef struct{
    u8 mode;

    u32 weigh_fullrange;    
    u8 weigh_onestep;
    u8 weigh_dotpos;
    u8 weigh_displaymin;
    u8 loadtrackrange;
    u8 weigh_lptime;
    u8 dozerorange;
    u8 keytype;
    u8 ADCChip;
    
    u32 weigh_division; 
    u32 weigh_ad_full;
    u32 weigh_ad_Middle;
    float weigh_coef[2];
    
}MachineData;

typedef struct{
    u8 displaystep_min;
    u8 powerofftime;
    u8 holdenable;
    u8 peakenable;
    u8 cheatenable;
    u8 rsv1;
    u8 rsv2;
    u8 rsv3;
    u8 rsv4;
    
}UserConfigData;


typedef struct{
    //u8 do_tare_flag;
    u8 do_zero_flag;
    u8 full_flag;
    u8 positive_flag;
    u8 stable_flag;
    u8 zero_flag;
    u8 power_on_flag;
    u8 return_zero_flag;    
    u8 lowpower_flag;

    u8 power_on_cnt;
    u8 key_sound_time;
    
    u32 no_key_time;
    u32 keep_zero_time;
    u32 not_zero_time;
    u32 PCSSample;
    float Pcs;
    float PCSCoef;
    
    SYSStatus current_mode;
    UNITType current_unit;
}RuningData;

typedef struct{
    u16 bat_buf[8];
    u16 battery;   //6.42 = 642
    u8 batterybufindex;

    
    u32 hx711_data;
    u32 ad_dat_avg;
    u32 ad_zero_data;
    u32 ad_tare_data;
    u32 poweron_zero_data;  //for manual zero proc
    float grossw;
    float netw;
    float displayweight;

}MeasureData;
    
typedef struct{
    u8 zero_track_enable;
    u8 zero_track_cnt;
    u8 load_track_enable;
    u8 load_track_cnt;
    float ad_filter_para;
}FilterProcData;  
 
typedef struct{
    UserCalType calstep;
    //linecal rsv
    //u8 linecalstart;
    //u8 linecalstep;
}CalProcData;

typedef struct{
    FactoryCfgType factorystep;
    u8 factoryindex;
}FactoryProcData;

///////////////////////////////////////变量声明
extern u8 ExitLpmodeflag;
extern u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_5s;
extern u8 display_buffer[16];
extern u8 RS232_buf[16];

extern MachineData MachData; 
extern MeasureData MData;
extern RuningData RunData;
extern FilterProcData FilterData;
extern CalProcData CalData;
extern FactoryProcData FactoryData;
extern UserConfigData UserData;
////////////////////////////////////////////

extern void U32toBUF(u32 data,u8* p);
extern u32 BUFtoU32(u8* p);
extern u32 BUFtoU32_tmp(u8* p);
extern float displaytostep(float w);

extern u8 System_Init(void);
extern void Battery_Filter(u16 ad);
extern void Battery_Get(void);
extern void MData_update_LED(void);
extern void MData_update_normal(void);

extern void Init_UserCalParam(void);

//display function
extern void Display_ClearPreZero(u8 max,u8 dot,u8* buf);
extern void Display_InnerCode(u32 x);
extern void Display_Wait(void);
extern void Display_LPmode(void);
extern void Display_PrePCS(void);
extern void Display_PCSErr(void);
extern void Display_PCS(void);
extern void Display_Weight(void);
extern void Display_Battery(void);
extern void Display_ClearLED(void);
extern void Display_SwapBuffer(void);
#endif

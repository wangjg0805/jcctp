//**********************************************************************
//文件名:global.h
//**********************************************************************
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm8s.h"

#include "timer1.h"
#include "CS1231.h"
#include "ht1621.h"
#include "rs232.h"
#include "i2c.h"
#include "key.h"
#include "key_fun.h"

typedef enum {
    LED = 0,
    LCD = 1,
}ScreenType;

        
#define  DISPLAY_TYPE          LED

#define MACHINE_NORMAL_MODE     0x01
#define MACHINE_FACTORY_MODE    0x02
#define MACHINE_USERCAL_MODE    0x04
#define MACHINE_LINECAL_MODE    0x08


#define MACHINE_AD_ZERO         10000
#define MACHINE_FULL_ZERO       1000000

#define MACHINE_ZERO_AD_MIN     3000
#define MACHINE_ZERO_AD_MAX     6000000
#define MACHINE_LOAD_AD_MIN     20000
#define MACHINE_LOAD_AD_MAX     10000000


#define NO_KEY_BKOFF_TIME       (5*2)  //500ms
#define NO_KEY_CLRUNIT_TIME     (5*2)
#define KEEPZERO_BKOFF_TIME     (5*2)

#define POWER_ON_DOZERO_TIME    20   //*100ms 
#define AUTOZERO_TRACK_TIME     10   //
#define AUTOZERO_TRACK_RANGE    3    //

#define AUTOLOAD_TRACK_RANGE    5    //

#define KEY_NORMAL_SOUND_TIME   (10) //*10ms = 100ms
#define KEY_LONG_SOUND_TIME     (20)
#define FULL_SOUND_TIME         (5)

#define FULL_STEP_NUM           9 //over 9D ,and full
#define NEG_FULL_NUM          9999 //-9999,and full

//
#define USER_CAL_PASSWORD         97528
#define USER_TOJIN_PASSWORD       10001
#define USER_TOKG_PASSWORD        10002
#define USER_KILLFEN_PASSWORD     20001
#define USER_SAVEFEN_PASSWORD     20002

//ZERO RANGE
//#define POWER_ON_ZERO_RANGE     10  //10%
#define MANUAL_ZERO_RANGE       4   //4%

//MACH STAT
typedef enum {
    STAT_WEIGH = 0,
    STAT_CAL,
    STAT_CONFIG
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
    FAC_EXIT,
    FAC_MAX,
    FAC_RSV5,
    FAC_RSV6,
    FAC_RSV7,
    FAC_RSV8,
    
}FACTORYCfgType;



typedef struct{
    u8 mode;

    u32 weigh_fullrange;    
    u8 weigh_onestep;
    u8 weigh_dotpos;
    u8 weigh_displaymin;
    u8 weigh_bkofftime;
    u8 dozerorange;
    u8 loadtrackrange;
    
    u32 weigh_division;
    
    u8 weigh_calpoint_num;   
    u32 ad_zero_data;
    u32 weigh_ad_full;
    u32 weigh_ad_calpoint[10];  //multi point cal
    
    float weigh_coef;
    
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
    u8 do_tare_flag;
    u8 do_zero_flag;
    u8 full_flag;
    u8 positive_flag;
    u8 stable_flag;
    u8 zero_flag;
    u8 power_on_flag;
    u8 power_on_cnt;
   
    SYSStatus current_mode;
    UNITType current_unit;
    
    u8 key_sound_time;
    u32 no_key_time;
    u32 keep_zero_time;
    u32 not_zero_time;
    
    u8 return_zero_flag;
    
    u8 lowpower_flag;
}RuningData;

typedef struct{
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
    u8 ad_filter_para;
}FilterProcData;  
 
typedef struct{
    u8 usercalstart;
    u8 usercalstep;
    //linecal rsv
    u8 linecalstart;
    u8 linecalstep;
    
}CalProcData;

typedef struct{
    FACTORYCfgType factorystep;
    u8 factoryindex;
}FactoryProcData;

///////////////////////////////////////变量声明
extern u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
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
extern float displaytostep(float w);

extern void FactoryGetFirstStepIndex(void);
extern u8 System_Init(void);

extern void Display_ClearPreZero(u8 max,u8 dot,u8* buf);
#endif

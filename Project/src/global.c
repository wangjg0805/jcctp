#include "stm8s.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

#include "global.h"
#include "ad_proc.h"
#include "ad_filter.h"
#include "TM1668.h"
#include "CPUled.h"


u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_30s;
u8 display_buffer[16];
u8 RS232_buf[16];

MachineData MachData; 
MeasureData MData;
RuningData RunData;
FilterProcData FilterData;
CalProcData CalData;
FactoryProcData FactoryData;

UserConfigData UserData;

//////////


void Battery_Filter(u16 ad)
{
    MData.bat_buf[MData.batterybufindex++] = ad;
    if(MData.batterybufindex >= 8)
        MData.batterybufindex = 0;
}


void Battery_Get()
{
    u8 i;
    u32 tmp;
    tmp = 0;
    for(i=0;i<8;i++)
        tmp += MData.bat_buf[i];
    MData.battery = (tmp/8) * 500 / 512;
    //printf("battery: %03d \r\n",MData.battery);
}



void U32toBUF(u32 data,u8* p)
{
    *p = data&0xff;
    *(p+1) = (data>>8)&0xff;
    *(p+2) = (data>>16)&0xff;
    *(p+3) = (data>>24)&0xff;
    
}

u32 BUFtoU32(u8* p)
{
    u32 i,j,k,m;
    i = *(p+3);
    j = *(p+2);
    k = *(p+1);
    m = *(p+0);
    return((i<<24)+(j<<16)+(k<<8)+ m);
}

u32 BUFtoU32_tmp(u8* p)
{
    u32 i,j,k,m;
    i = *(p+3);
    j = *(p+2);
    k = *(p+1);
    m = *(p+0);
    return((m<<24)+(k<<16)+(j<<8)+ i);
}


//////////////////////////////////////////////////
//1
///////////////////////////////////////////////////
void InitGlobalVarible(void)
{
    u8 i;
    
	RunData.power_on_flag = 1;
    RunData.power_on_cnt = POWER_ON_WAIT_TIME;
    RunData.stable_flag = 0;
	RunData.full_flag = 0;
    RunData.key_sound_time = 0;
    RunData.current_mode = STAT_WEIGHT;
    RunData.current_unit = UNIT_KG;
    //RunData.do_zero_flag = 0;
    //RunData.do_tare_flag = 0;
    
    RunData.no_key_time = 0;
    RunData.keep_zero_time = 0;
      
    MData.ad_zero_data = 0;
    MData.ad_tare_data = 0;
    
    MData.battery = 0;
    MData.batterybufindex = 0;
    for(i=0;i<8;i++)
        MData.bat_buf[i] = 0;
     
    CalData.usercalstart = 0;
    CalData.usercalstep = 0;
    CalData.linecalstart = 0;
    CalData.linecalstep = 0;

    FactoryData.factorystep = FAC_NULL;
    FactoryData.factoryindex = 0;
}

//
void Get_LinecalParam(void)
{
    u32 i,j,k;

    i = (MachData.weigh_linecalbuf[2] - MachData.weigh_linecalbuf[0]) / 2 ; //
    j =  MachData.weigh_linecalbuf[1] - MachData.weigh_linecalbuf[0];
    k =  MachData.weigh_linecalbuf[2] - MachData.weigh_linecalbuf[1];
    

}

void  Init_LinecalParam(void)
{	 
	u8  buf[16];	 
   
    Read_EEPROM(EEP_LINECALFLAG_ADDR, buf, 16);

    if((CHECK_DATA==buf[0])&&(CHECK_DATA==buf[0])) {
        MachData.weigh_linecalbuf[0] = BUFtoU32_tmp(buf+4);
        MachData.weigh_linecalbuf[1] = BUFtoU32_tmp(buf+8);
        MachData.weigh_linecalbuf[2] = BUFtoU32_tmp(buf+12);
        MachData.weigh_linecalflag = 1;
        Get_LinecalParam();
    } else {
        MachData.weigh_linecalflag = 0;
    }
}
///////////////////////////////////////////////
//根据机器型号 初始化系统变量包含了
// 分辨率
// 满量程重量
// 满量程内码最小数值
// 零点内码最小数值
// 
// 四舍五入数值
// 最小显示数值
/////////////////////////////////////////////// 
void  Init_MachineParam(void)
{	 
    u8 buf[16];	 
    
    Read_EEPROM(EEP_SYS_FULLRANGE_ADDR, buf, 16);

    MachData.weigh_fullrange = BUFtoU32_tmp(buf);
    MachData.weigh_onestep = BUFtoU32_tmp(buf+4);
    MachData.weigh_dotpos = BUFtoU32_tmp(buf+8);
    MachData.weigh_displaymin = BUFtoU32_tmp(buf+12);
    
    Read_EEPROM(EEP_SYS_LOADTRACK_ADDR, buf, 8);

    MachData.loadtrackrange = BUFtoU32_tmp(buf);
    
  /*  
    Read_EEPROM(EEP_SYS_BKOFFTIME_ADDR, buf, 8); 
    MachData.weigh_bkofftime = BUFtoU32_tmp(buf);
    MachData.dozerorange = BUFtoU32_tmp(&buf[4]);
    
    Read_EEPROM(EEP_SYS_LOADTRACK_ADDR, buf, 4); 
    MachData.loadtrackrange = BUFtoU32_tmp(buf);
  
    MachData.weigh_division = MachData.weigh_fullrange/MachData.weigh_onestep;      
    MachData.weigh_calpoint_num = 1;
    */
/*    
    MachData.weigh_fullrange = 100000;
    MachData.weigh_onestep = 1;
    MachData.weigh_dotpos = 3;
    MachData.weigh_displaymin = 2;
*/  
    MachData.weigh_lptime = 10*2;  //5s
    MachData.dozerorange  = 20;
    MachData.weigh_division =  MachData.weigh_fullrange / MachData.weigh_onestep;
    
} 
		 		 

//////////////////////////////////////////////////
//初始化 用户参数2
///////////////////////////////////////////////////
void Init_UserConfigParam(void)
{
    u8 buf[16];	
    
    Read_EEPROM(EEP_CALFLAG_ADDR, buf, 2); 
    if((buf[0]==buf[1])&&(buf[0]== CHECK_DATA)) {
        Read_EEPROM(EEP_WEIGHTZERO_ADDR, buf, 16);
/*
        printf("read weight zero full buf :");
        for(i=0;i<16;i++)
            printf("0x%x ",buf[i]);
        printf("\r\n");
*/        
        MData.ad_zero_data = BUFtoU32_tmp(buf);
        MachData.weigh_ad_full = BUFtoU32_tmp(buf+8);
       
    } else {
        MData.ad_zero_data = MACHINE_AD_ZERO;
        MachData.weigh_ad_full = MACHINE_FULL_ZERO;
    }
    
	MachData.weigh_coef = MachData.weigh_fullrange / (MachData.weigh_ad_full + 0.1);
    FilterData.ad_filter_para = (MachData.weigh_ad_full+0.1) / MachData.weigh_division;
}

void Init_UserDataParam(void)
{
    /*
    u32 i,j;
	u8 buf[8];	
    Read_EEPROM(EEP_USR_FUN1_ADDR, buf, 8); 
 */
}

float displaytostep(float w)
{
    float tmp;
    u32 i;
    u8 j;
    if(w < MachData.weigh_displaymin * MachData.weigh_onestep)
        return(0);
    else {
        switch(MachData.weigh_onestep) {     
        case 2:
            tmp = w + 0.5;
            i = (u32)tmp;
            if(0 != i%2)
                tmp = tmp - 1;
             break;
        case 5:
            tmp = w + 0.5;
            i = (u32)tmp;
            i = i%5;
            if(i < 3)
                tmp = tmp - i;
            else
                tmp = tmp + 5 - i;
             break;
        case 10:
            tmp = w + 5.0;
            i = (u32)tmp;
            tmp = i - (i%10);
            break;
        case 20:
            i = (u32)w;
            j = i%20;
            if(j < 10)  // 0 2 4 6 8 
                tmp = i - j;
            else
                tmp = i - j + 20;
             break;
        case 50:
            tmp = w + 5;
            i = (u32)tmp;
            i = i%50;
            if(i < 30)
                tmp = tmp - i;
            else
                tmp = tmp + 50 - i;
             break;
        case 100:   
            tmp = w + 50;
            i = (u32)tmp;
            tmp = i - (i%100);
            break;    
        default:
            tmp = w;
            break;
        }
    }
    return(tmp);
}


void FactoryGetFirstStepIndex(void)
{
    switch(MachData.weigh_fullrange) {
    case 3000:   FactoryData.factoryindex = 1;break;
    case 6000:   FactoryData.factoryindex = 2;break;
    case 15000:  FactoryData.factoryindex = 3;break;
    case 30000:  FactoryData.factoryindex = 4;break;
    case 60000:  FactoryData.factoryindex = 5;break;
    case 100000: FactoryData.factoryindex = 6;break;
    default:     FactoryData.factoryindex = 1;break;
    }
}

///////////////////////////////////////////////////
u8  System_Init(void)
{
    u8 i = 0;
    u8 cnt = 0;
    u16 j,key_buf[3] = {0x00,0x00,0x00};
    
    do {
        switch(i){
        case 0:
            InitGlobalVarible();
            TM1668_DisplayAll();
            break;
        case 20:
            TM1668_DisplayMode();
            break;
        case 40:
            Display_Battery();
            TM1668_Update();
            break;
        case 3:
            Init_MachineParam();
            break;
        case 5:     
            Init_LinecalParam();  
            break;
        case 7:
            Init_UserConfigParam();
            break;
        case 9:
            Init_UserDataParam();
            break;
        default:
            break;
        }
        
        if(1 == Flag_10ms) {
            Flag_10ms = 0;
            Key_Scan();
        }
	    if(1 == Flag_100ms) {
            Flag_100ms = 0;
            i++;
            if(1 == CS1231_Read())
                ad_filter(MData.hx711_data);
            
            j = Key_GetCode();
            if(j!=0) {
                key_buf[cnt++] = j;
                if(cnt > 2)
                    cnt = 0;
            }
            //get battery
            if(ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
                Battery_Filter(ADC1_GetConversionValue());
                Battery_Get();
            }      
        }
        
    }while(i!=60);
    
    if((key_buf[0] == KEY_PRESSED+KEY_UNITMODE)&&
       (key_buf[1] == KEY_PRESSED+KEY_PCSCONFIRM))
        return(MACHINE_NORMAL_MODE + MACHINE_FACTORY_MODE);

    if((key_buf[0] == KEY_PRESSED+KEY_PCSCONFIRM)&&
       (key_buf[1] == KEY_PRESSED+KEY_TARECAL))
        return(MACHINE_NORMAL_MODE + MACHINE_LINECAL_MODE);
 
    return(MACHINE_NORMAL_MODE);
}

	
void MData_update_normal(void)
{
	u32 grossw_ad,netw_ad;
    //float tmp;
    //do zero proc when power_on
    if(1 == RunData.power_on_flag) {
        RunData.power_on_cnt--;
        if(RunData.power_on_cnt < POWER_ON_WAIT_TIME-10) { //waiting time : 1s min
            if((0 == RunData.power_on_cnt) ||
               (1 == RunData.stable_flag)) {
                RunData.power_on_flag = 0;
		 	    if(labs(MData.ad_dat_avg-MData.ad_zero_data) < (MachData.weigh_ad_full*MachData.dozerorange/100)) {
                    MData.ad_zero_data = MData.ad_dat_avg;              
                }
                /////////////////////////////
                MData.poweron_zero_data = MData.ad_zero_data;      //power on zero data   
            } 
        }
    }
   
    if((1 == RunData.do_zero_flag)&&(1==RunData.stable_flag)) {
        RunData.do_zero_flag = 0;
        do_zero_proc();
    }
/*   
    if((1 == RunData.do_tare_flag)&&(1==RunData.stable_flag)) {
        RunData.do_tare_flag = 0;
        do_tare_proc();
    }
*/    
    autozero_track();    
    autoload_track(); 
   
    if(MData.ad_dat_avg > MData.ad_zero_data) {
        grossw_ad = MData.ad_dat_avg - MData.ad_zero_data;
        if(grossw_ad > MData.ad_tare_data) {
            netw_ad = grossw_ad - MData.ad_tare_data;
            RunData.positive_flag = 1;
        } else {
            netw_ad = MData.ad_tare_data - grossw_ad;
            RunData.positive_flag = 0;
        }  
    } else {
        grossw_ad = MData.ad_zero_data - MData.ad_dat_avg;
        netw_ad = grossw_ad + MData.ad_tare_data;
        RunData.positive_flag = 0;
    }
 
    //printf("MData.ad_dat_avg:%ld,MData.ad_zero_data:%ld,MData.ad_tare_data:%ld \r\n",MData.ad_dat_avg,MData.ad_zero_data,MData.ad_tare_data);
    
    //pcs mode
    if(STAT_PCS == RunData.current_mode) {
        if(MData.ad_dat_avg <= MData.ad_zero_data+MData.ad_tare_data)
            RunData.Pcs = 0;
        else
            RunData.Pcs = (MData.ad_dat_avg - MData.ad_zero_data-MData.ad_tare_data) / RunData.PCSCoef;
    }
    
    MData.grossw = grossw_ad * MachData.weigh_coef + 0.5;
    MData.netw = netw_ad * MachData.weigh_coef + 0.5;
    MData.displayweight = displaytostep(MData.netw);

    if((1==RunData.stable_flag)&&(MData.displayweight<0.1)) {
        RunData.positive_flag = 1;
        RunData.return_zero_flag = 1;
        RunData.not_zero_time = 0;
    } else { 
        RunData.keep_zero_time = 0;
    }
    
    //full flag 
    RunData.full_flag = 0;
    if((1==RunData.positive_flag)&&(MData.grossw > (MachData.weigh_fullrange+FULL_STEP_NUM*MachData.weigh_onestep))) {
        RunData.full_flag = 1;
        //printf("full:positive,the grossw is %08.2f \r\n",MData.grossw);
    } else if((0==RunData.positive_flag)&&(MData.displayweight > NEG_FULL_NUM)) {
        RunData.full_flag = 1;
        //printf("full:negative,the displayweight is %08.2f \r\n",MData.displayweight);
    }
    
    if(1==RunData.full_flag)
        RunData.key_sound_time = FULL_SOUND_TIME;
}

////////////////////display function

void Display_ClearPreZero(u8 max,u8 dot,u8* buf)
{
    u8 i;
    for(i = 0;i < max - dot;i++) {
        if(*buf == 0) {
            *buf = DISP_NULL;
            buf++;
        } else
            break;
    }
}

void Display_InnerCode(u32 x)
{
    u8 i;
    sprintf((char*)display_buffer,"%06ld",x);
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;
   
    Display_ClearPreZero(6, 1, display_buffer);

    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];

}


void Display_LPmode(void) 
{
    u8 i;
    static u8 cnt = 0;
    cnt++;
    if(cnt >= 10)
        cnt = 0;
    
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_LPMODE[i]]; 
    if(cnt<3)
        display_buffer[5] = display_code[DISP_X];
}

void Display_PrePCS(void)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_COUNT[i]];
    if(RunData.PCSSample < 100)
        display_buffer[3] = display_code[DISP_NULL];
    else
        display_buffer[3] = display_code[RunData.PCSSample/100];
    
    display_buffer[4] = display_code[(RunData.PCSSample%100)/10];
    display_buffer[5] = display_code[RunData.PCSSample%10]; 
}


void Display_PCSErr(void)
{
    static u8 cnt = 0;
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_COUNTERR[i]];

    cnt++;
    if(cnt > 15) {//1.5s
        cnt = 0;
        RunData.current_mode = STAT_WEIGHT;
    }
}


void Display_PCS(void)
{
    u8 i;
    sprintf((char*)display_buffer,"%06ld",(u32)(RunData.Pcs + 0.5)); //实际PCS数 是否需要四舍五入??
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;   
    
    Display_ClearPreZero(6,  1,display_buffer);
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];    
}

void Display_Weight(void)
{
    u8 i;
    sprintf((char*)display_buffer,"%06ld",(u32)MData.displayweight);
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;        
    Display_ClearPreZero(5,MachData.weigh_dotpos,display_buffer);
    
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];
    
   
    display_buffer[5-MachData.weigh_dotpos] |= SEG_P;
    if(0 == RunData.positive_flag)
        display_buffer[0] = display_code[DISP_X];
    
}


void Display_Battery(void)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_BATTERY[i]];
    
    display_buffer[3] = display_code[MData.battery/100] | SEG_P;
    display_buffer[4] = display_code[(MData.battery%100)/10];
    display_buffer[5] = display_code[MData.battery%10];    
    
}


void Display_Wait(void)
{
    u8 i;
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[DISP_X];

}
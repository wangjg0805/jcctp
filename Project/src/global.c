#include "stm8s.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

#include "global.h"
#include "factory.h"
#include "ad_proc.h"
#include "ad_filter.h"
#include "TM1668.h"
#include "CPUled.h"

u8 ExitLpmodeflag;
u8 Flag_10ms,Flag_100ms,Flag_500ms,Flag_5s;
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
    if(MData.battery < LOWBATTERY_VOLT)
        RunData.lowbat_flag = 1;
    else
        RunData.lowbat_flag = 0;
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
    
    MData.battery = 600;  //default
    MData.batterybufindex = 0;
    for(i=0;i<8;i++)
        MData.bat_buf[i] =  MData.battery;
     
    CalData.calstep = CAL_NULL;

    FactoryData.factorystep = FAC_NULL;
    FactoryData.factoryindex = 0;
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
    u32 i;
    u8 buf[32];	 
    
    Read_EEPROM(EEP_SYS_FULLRANGE_ADDR, buf, 4*7);

    MachData.weigh_fullrange = BUFtoU32_tmp(buf);
    MachData.weigh_onestep = BUFtoU32_tmp(buf+4);
    MachData.weigh_dotpos = BUFtoU32_tmp(buf+8);
    MachData.weigh_displaymin = BUFtoU32_tmp(buf+12);

    //Read_EEPROM(EEP_SYS_LOADTRACK_ADDR, buf, 16);
    MachData.loadtrackrange = BUFtoU32_tmp(buf+16);
    MachData.dozerorange = BUFtoU32_tmp(buf+20);
    MachData.keytype = BUFtoU32_tmp(buf+24);

    //check error
    if((MachData.weigh_fullrange<3000) ||   
       (MachData.weigh_fullrange>300000) ||
       (0 != MachData.weigh_fullrange%1000)) 
    { 
       MachData.weigh_fullrange = 60000;  
       MachData.weigh_onestep = 1;     
       MachData.weigh_dotpos = 2; 
       MachData.weigh_displaymin = 2;
       MachData.loadtrackrange = 1;
       MachData.dozerorange = 100; //100%
       MachData.keytype = 3;
       //save to flash
       U32toBUF(MachData.weigh_fullrange,buf);
       U32toBUF(MachData.weigh_onestep,buf+4);
       U32toBUF(MachData.weigh_dotpos,buf+8);
       U32toBUF(MachData.weigh_displaymin,buf+12);
       U32toBUF(MachData.loadtrackrange,buf+16);
       U32toBUF(MachData.dozerorange,buf+20);
       U32toBUF(MachData.keytype,buf+24);
       Write_EEPROM(EEP_SYS_FULLRANGE_ADDR,buf,4*7);       
     }
    
    MachData.weigh_lptime = 10*2;  //5s
    MachData.weigh_division =  MachData.weigh_fullrange / MachData.weigh_onestep;
    FactoryGetFirstStepIndex(); //use for cal info
} 
		 		 

//////////////////////////////////////////////////
//初始化 用户参数2
///////////////////////////////////////////////////
void Init_UserCalParam(void)
{
    u8 buf[16];	
    
    Read_EEPROM(EEP_CALFLAG_ADDR, buf, 2); 
    //if(0) {
    if((buf[0]==buf[1])&&(buf[0]== CHECK_DATA)) {
        Read_EEPROM(EEP_WEIGHTZERO_ADDR, buf, 8); 
        MData.ad_zero_data = BUFtoU32_tmp(buf);
        
        Read_EEPROM(EEP_WEIGHTFULL1_ADDR, buf, 16);
        MachData.weigh_ad_Middle = BUFtoU32_tmp(buf);
        MachData.weigh_ad_full = BUFtoU32_tmp(buf+8);
        MachData.weigh_coef[0] = (MachData.weigh_fullrange/loadcal_coef[FactoryData.factoryindex]) / (MachData.weigh_ad_Middle + 0.1);
        MachData.weigh_coef[1] =  MachData.weigh_fullrange    / (MachData.weigh_ad_full + 0.1);
    } else {
        MData.ad_zero_data = MACHINE_AD_ZERO;
        MachData.weigh_ad_full = MACHINE_FULL_ZERO;
        MachData.weigh_ad_Middle = MachData.weigh_ad_full / 2;
        MachData.weigh_coef[0] = MachData.weigh_fullrange / (MachData.weigh_ad_full + 0.1);
        MachData.weigh_coef[1] = MachData.weigh_coef[0];
    }
    
    FilterData.ad_filter_para = (MachData.weigh_ad_full+0.1) / MachData.weigh_division;
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
            Display_ClearLED();
            TM1668_DisplayModel();
            TM1668_Update();
            break;
        case 40:
            Display_Battery();
            TM1668_Update();
            break;
        case 3:
            Init_MachineParam();
            break;
        case 5:     
            //Init_LinecalParam();  
            break;
        case 7:
            Init_UserCalParam();
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
            if(MachData.ADCChip == CS1231) {
                if(1 == CS1231_Read())
                ad_filter(MData.hx711_data);
            } else {
                if(1 == CS1237_Read())               
                ad_filter(MData.hx711_data);
            }
            
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

    return(MACHINE_NORMAL_MODE);
}

void MData_update_LED(void)
{
    if(1 == RunData.lowpower_flag)
        return;
    
    display_buffer[6] = 0x00;    
    if(1 == RunData.stable_flag)
        display_buffer[6] |= LED_STABLE;
    
    if(1 == RunData.zero_flag)
        display_buffer[6] |= LED_ZERO;
      
    if(STAT_PCS == RunData.current_mode)
        display_buffer[6] |= LED_COUNT;
}

//power on stage
u8 MData_PowerOnProc(void)
{
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
    
    return(RunData.power_on_flag);
}

//user cal stage
u8 MData_CalProc(void)
{   
    if(((MACHINE_NORMAL_MODE+MACHINE_USERCAL1_MODE) == MachData.mode)||
       ((MACHINE_NORMAL_MODE+MACHINE_USERCAL2_MODE) == MachData.mode)) {
         if(1 == RunData.stable_flag) {
            UserCalAutoProc();
         }
         if((MACHINE_NORMAL_MODE+MACHINE_USERCAL1_MODE == MachData.mode) &&
            (CAL_PASS1 == CalData.calstep))
             return(0);
         else if((MACHINE_NORMAL_MODE+MACHINE_USERCAL2_MODE == MachData.mode)&& 
            (CAL_PASS2 == CalData.calstep)) //update display buffer
             return(0);
         else      
             return(1);
    }
    else 
        return(0);   
}


void MData_update_normal(void)
{
	u32 grossw_ad,netw_ad;
    float tmp;
    //do zero proc when power_on
    if(1 == MData_PowerOnProc())
        return;
    if(1 == MData_CalProc())
        return;
    
    autozero_track();    
    autoload_track(); 
    if((1 == RunData.do_zero_flag)&&(1==RunData.stable_flag)) {
        RunData.do_zero_flag = 0;
        do_zero_proc();
    }
   
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
    
    if(grossw_ad  < MachData.weigh_ad_Middle) //use coef1
        tmp = MachData.weigh_coef[0];
    else
        tmp = MachData.weigh_coef[1]; 
    
    //printf("Netw: %ld \r\n",netw_ad);    
    MData.grossw = grossw_ad * tmp + 0.5;   
    MData.netw = netw_ad * tmp + 0.5;
    //MData.grossw = grossw_ad * tmp;   
    //MData.netw = netw_ad * tmp;
    MData.displayweight = displaytostep(MData.netw);
    
    if((1==RunData.stable_flag)&&(MData.displayweight < 1.0)) {
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
        if(*(buf+5-i) == 0) {
            *(buf+5-i) = DISP_NULL;
        } else
            break;
    }
}

void Display_Data(u32 x)
{
    u8 i;
    sprintf((char*)display_buffer,"%06ld",x);
    Display_SwapBuffer();
    
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
        display_buffer[i] = display_code[display_NULL[i]];
}

void Display_PrePCS(void)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_COUNT[i]];
    if(RunData.PCSSample < 100)
        display_buffer[2] = display_code[DISP_NULL];
    else
        display_buffer[2] = display_code[RunData.PCSSample/100];
    
    display_buffer[1] = display_code[(RunData.PCSSample%100)/10];
    display_buffer[0] = display_code[RunData.PCSSample%10]; 
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
    Display_SwapBuffer();
    
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
    Display_SwapBuffer();
    
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;     
    
    Display_ClearPreZero(5,MachData.weigh_dotpos,display_buffer);
    
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];
    

    display_buffer[MachData.weigh_dotpos] |= SEG_P;
    if(0 == RunData.positive_flag)
        display_buffer[5] = display_code[DISP_X];
    
}

void Display_CalCountDown(void)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_NULL[i]];
   
    display_buffer[2] = display_code[RunData.CalCountDown_time/2];
    
}

void Display_Battery(void)
{
    u8 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_BATTERY[i]];
    
    display_buffer[2] = display_code[MData.battery/100] | SEG_P;
    display_buffer[1] = display_code[(MData.battery%100)/10];
    display_buffer[0] = display_code[MData.battery%10];    
    
}


void Display_Wait(void)
{
    u8 i;
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[DISP_X];

}

void Display_ClearLED(void)
{
    display_buffer[6] = 0x00;
}

void Display_SwapBuffer(void)
{
    u8 i,j,k;
    
    i = display_buffer[0];
    j = display_buffer[1];
    k = display_buffer[2];
    display_buffer[0] = display_buffer[5];
    display_buffer[1] = display_buffer[4];
    display_buffer[2] = display_buffer[3];
    display_buffer[3] = k;
    display_buffer[4] = j;
    display_buffer[5] = i;

}
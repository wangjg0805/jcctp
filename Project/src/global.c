#include "stm8s.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"

#include "global.h"
#include "ad_proc.h"
#include "ad_filter.h"



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

/////////////////////////////////////////��������
void Speaker_Proc(void)
{
    if(0!=RunData.key_sound_time) {
        RunData.key_sound_time--;
        Speaker_On();
    } else {
        Speaker_Off();
    }
    
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

//////////////////////////////////////////////////
//1
///////////////////////////////////////////////////
void InitGlobalVarible(void)
{
    //
	RunData.power_on_flag = 1;
    RunData.power_on_cnt = POWER_ON_DOZERO_TIME;
    RunData.stable_flag = 0;
	RunData.full_flag = 0;
    RunData.key_sound_time = 0;
    RunData.current_mode = STAT_WEIGH;
    RunData.current_unit = UNIT_KG;
    RunData.do_zero_flag = 0;
    RunData.do_tare_flag = 0;
    
    RunData.no_key_time = 0;
    RunData.keep_zero_time = 0;
      
    MData.ad_zero_data = 0;
    MData.ad_tare_data = 0;
     
    CalData.usercalstart = 0;
    CalData.usercalstep = 0;
    CalData.linecalstart = 0;
    CalData.linecalstep = 0;

    FactoryData.factorystep = 0;
    FactoryData.factoryindex = 0;
}
 
///////////////////////////////////////////////
//���ݻ����ͺ� ��ʼ��ϵͳ����������
// �ֱ���
// ����������
// ������������С��ֵ
// ���������С��ֵ
// 
// ����������ֵ
// ��С��ʾ��ֵ
/////////////////////////////////////////////// 
void  Init_MachineParam(void)
{	 
    u32 i,j;
	u8  buf[8];	 
    /*
    Read_EEPROM(EEP_SYS_FULLRANGE_ADDR, buf, 8);
    MachData.weigh_fullrange = BUFtoU32(buf);
    MachData.weigh_onestep = BUFtoU32(&buf[4]);
    
    Read_EEPROM(EEP_SYS_DOT_ADDR, buf, 8); 
    MachData.weigh_dotpos = BUFtoU32(buf);
    MachData.weigh_displaymin = BUFtoU32(&buf[4]);
    
    Read_EEPROM(EEP_SYS_BKOFFTIME_ADDR, buf, 8); 
    MachData.weigh_bkofftime = BUFtoU32(buf);
    MachData.dozerorange = BUFtoU32(&buf[4]);
    
    Read_EEPROM(EEP_SYS_LOADTRACK_ADDR, buf, 4); 
    MachData.loadtrackrange = BUFtoU32(buf);
  
    MachData.weigh_division = MachData.weigh_fullrange/MachData.weigh_onestep;      
    MachData.weigh_calpoint_num = 1;
    */
    MachData.weigh_fullrange = 50000;
    MachData.weigh_onestep = 1;
    MachData.weigh_dotpos = 5;
    MachData.weigh_displaymin = 1;
  
    MachData.weigh_bkofftime = 30;
    MachData.dozerorange = 20;
    MachData.loadtrackrange = 10;
    MachData.weigh_division =  MachData.weigh_fullrange / MachData.weigh_onestep;
    
} 
		 		 

//////////////////////////////////////////////////
//��ʼ�� �û�����2
///////////////////////////////////////////////////
void Init_UserConfigParam(void)
{
    u32 i,j;
	u8 buf[4];	
    Read_EEPROM(EEP_CALFLAG_ADDR, buf, 2); 
    if((buf[0]==buf[1])&&(buf[0]== CHECK_DATA)) {
        Read_EEPROM(EEP_WEIGHTZERO_ADDR, buf, 4);
        MachData.ad_zero_data = BUFtoU32(buf);
        MData.ad_zero_data = MachData.ad_zero_data;
        
        Read_EEPROM(EEP_WEIGHTFULL_ADDR, buf, 4);
        MachData.weigh_ad_full = BUFtoU32(buf);
       
    } else {
        MachData.ad_zero_data = MACHINE_AD_ZERO;
        MData.ad_zero_data = MACHINE_AD_ZERO;
        MachData.weigh_ad_full = MACHINE_FULL_ZERO;
    }
    
	MachData.weigh_coef = MachData.weigh_fullrange / (MachData.weigh_ad_full + 0.1);
    FilterData.ad_filter_para = MachData.weigh_ad_full / MachData.weigh_division;
}

void Init_UserDataParam(void)
{
    u32 i,j;
	u8 buf[8];	
    Read_EEPROM(EEP_USR_FUN1_ADDR, buf, 8); 
 
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
            i = tmp;
            if(0 != i%2)
                tmp = tmp - 1;
             break;
        case 5:
            tmp = w + 0.5;
            i = tmp;
            i = i%5;
            if(i < 3)
                tmp = tmp - i;
            else
                tmp = tmp + 5 - i;
             break;
        case 10:
            tmp = w + 5.0;
            i = tmp;
            tmp = i - (i%10);
            break;
        case 20:
            i = w;
            j = i%20;
            if(j < 10)  // 0 2 4 6 8 
                tmp = i - j;
            else
                tmp = i - j + 20;
             break;
        case 50:
            tmp = w + 5;
            i = tmp;
            i = i%50;
            if(i < 30)
                tmp = tmp - i;
            else
                tmp = tmp + 50 - i;
             break;
        case 100:   
            tmp = w + 50;
            i = tmp;
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
            TM1668_DisplayAll();
            break;
        case 10:
            TM1668_DisplayMode();
            break;
        case 15:
            //TM1668_DisplayBat();
            break;
        case 20:
            TM1668_DisplayWait();
            break;
    
        case 1: 
            InitGlobalVarible();
            break;
        case 3:
            Init_MachineParam();
            break;
        case 5:
            Init_UserConfigParam();
            break;
        case 7:
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
        }
    }while(i!=30);
    
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
    float tmp;
    //do zero proc when power_on
    if((1 == RunData.power_on_flag)&&(1 == RunData.stable_flag)) {
        RunData.power_on_cnt--;
        if(0 == RunData.power_on_cnt) {
            RunData.power_on_flag = 0;
		 	if(labs(MData.ad_dat_avg-MachData.ad_zero_data) < (MachData.weigh_ad_full*MachData.dozerorange/100)) {
                MData.ad_zero_data = MData.ad_dat_avg;              
            }
            /////////////////////////////
            MData.poweron_zero_data = MData.ad_zero_data;      //power on zero data   
        } 
    }
   
    if((1 == RunData.do_zero_flag)&&(1==RunData.stable_flag)) {
        RunData.do_zero_flag = 0;
        do_zero_proc();
    }
    if((1 == RunData.do_tare_flag)&&(1==RunData.stable_flag)) {
        RunData.do_tare_flag = 0;
        do_tare_proc();
    }
    
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
    
    //zero flag    
    MData.grossw = grossw_ad * MachData.weigh_coef + 0.5;
    MData.netw = netw_ad * MachData.weigh_coef + 0.5;
    
    MData.displayweight = displaytostep(MData.netw);

    
    if((MData.displayweight<0.1)&&(0==MData.ad_tare_data)&&(1==RunData.stable_flag))
        RunData.zero_flag = 1;
    else
        RunData.zero_flag = 0;
      
    //total :return 0 flag 
    if(MData.displayweight < 0.1){
        RunData.return_zero_flag = 1;
        RunData.not_zero_time = 0;
    } else
        RunData.keep_zero_time = 0;
    
    //full flag 
    RunData.full_flag = 0;
    if((1==RunData.positive_flag)&&(MData.grossw > (MachData.weigh_fullrange+FULL_STEP_NUM*MachData.weigh_onestep))) {
        RunData.full_flag = 1;
        printf("full:positive,the grossw is %08.2f \r\n",MData.grossw);
    } else if((0==RunData.positive_flag)&&(MData.displayweight > NEG_FULL_NUM)) {
        RunData.full_flag = 1;
        printf("full:negative,the displayweight is %08.2f \r\n",MData.displayweight);
    }
    
    if(1==RunData.full_flag)
        RunData.key_sound_time = FULL_SOUND_TIME;
    
    //get price
#if 0
    if(0 == RunData.positive_flag)
        RunData.price = 0;
    else {
        i = MData.displayweight;
        tmp = i * RunData.unitprice / pow(10,MachData.weigh_dotpos);
        if(UserData.killfen == 1) {
            tmp = tmp+5;
            i = ((u32)tmp)%10;
            tmp = tmp - i;
        }
        RunData.price = tmp;
    }
#endif
    
}


void MData_update_cal(void)
{
	u32 grossw_code;

    //Cal_Proc(MData.ad_dat_avg); 
   
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
    sprintf(display_buffer,"%06ld",x);
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;
   
    Display_ClearPreZero(6, 1, display_buffer);

    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];
}

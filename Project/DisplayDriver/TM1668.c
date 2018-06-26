#include "stm8s.h"
#include "TM1668.h"
#include "factory.h"
#include "global.h"
#include  "stdlib.h"
#include  "math.h"


const u8 display_code[] = {SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F,      //0
                           SEG_B+SEG_C,                              //1  
                           SEG_A+SEG_B+SEG_D+SEG_E+SEG_G,            //2
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_G,            //3
                           SEG_B+SEG_C+SEG_F+SEG_G,                  //4
                           SEG_A+SEG_C+SEG_D+SEG_F+SEG_G,            //5
                           SEG_A+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,      //6
                           SEG_A+SEG_B+SEG_C,                        //7
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,//8
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_F+SEG_G,      //9
						  
                           SEG_A+SEG_D+SEG_E+SEG_F,                  //C    0x0a
                           SEG_A+SEG_B+SEG_C+SEG_E+SEG_F+SEG_G,      //A	  0x0b
						   SEG_D+SEG_E+SEG_F,				         //L    0x0c
						   SEG_A+SEG_B+SEG_E+SEG_F+SEG_G,            //P	  0x0d
						   SEG_A+SEG_B+SEG_C+SEG_F+SEG_G,			 //Q    0x0e	
						   SEG_A+SEG_E+SEG_F+SEG_G,                  //F    0x0f
						   SEG_B+SEG_C+SEG_D+SEG_E+SEG_F,            //U    0x10
                           SEG_A+SEG_D+SEG_E+SEG_F+SEG_G,            //E    0x11                      
                           SEG_E+SEG_F+SEG_G,                        //r    0x12
						   SEG_G,                                    //-	  0x13
						   SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,            //b..  0x14 
						   SEG_C+SEG_D+SEG_E+SEG_G,                  //o    0x15
                           SEG_D+SEG_E+SEG_F+SEG_G,                  //t  	0x16		
					  	   SEG_B+SEG_C+SEG_D+SEG_E+SEG_G,            //d  	0x17
                           SEG_D+SEG_E+SEG_G,                        //c    0x18
 						   SEG_A+SEG_B+SEG_C+SEG_E+SEG_F,            //N    0x19
                           SEG_B+SEG_C+SEG_E+SEG_F+SEG_G,            //H    0x1a
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G+SEG_P, //8. 0x1b
                           0                                         //NULL 0x1c                        
};


//BAT
u8 const display_BATTERY[]    = {DISP_NULL,  DISP_NULL,  DISP_NULL,  DISP_X,  DISP_c,  DISP_d};
//LPMODE
u8 const display_NULL[]     = {DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL};
//FULL
u8 const display_FULL[]        = {DISP_NULL,DISP_L,DISP_L,DISP_U,DISP_F,DISP_NULL}; 
//PCS
u8 const display_COUNT[]       = {DISP_X,  DISP_X  ,DISP_X,  DISP_U,  0,  DISP_C}; 
u8 const display_COUNTERR[]    = {DISP_r,  DISP_r  ,DISP_E,  DISP_U,  0,  DISP_C};
//USERCAL
u8 const display_USERCALTIP1[]  = { DISP_NULL,           1,      DISP_L,      DISP_A,     DISP_C,  DISP_NULL};
u8 const display_USERCALTIP2[]  = { DISP_NULL,           2,      DISP_L,      DISP_A,     DISP_C,  DISP_NULL};

u8 const display_USERCAL[]     = { DISP_NULL,   DISP_NULL,   DISP_NULL,   DISP_NULL,  DISP_NULL,  DISP_NULL};
u8 const display_USERCALPASS1[] = {        1,           5,           5,      DISP_A,     DISP_P,  DISP_NULL};
u8 const display_USERCALPASS2[] = {        2,           5,           5,      DISP_A,     DISP_P,  DISP_NULL};
u8 const display_USERCALERR[]  = {         9,      DISP_X,      DISP_r,      DISP_r,     DISP_E,  DISP_NULL};

//factory mode
u8 const display_FACTORY[] = {DISP_NULL,  DISP_NULL,  DISP_NULL,  DISP_NULL,  DISP_X,  DISP_F};
//Line cal
u8 const display_LINECAL[] = {DISP_X,  DISP_X,  DISP_X,  DISP_NULL,  DISP_N,  DISP_L};

/*************TM1668写字节函数***********/
static void TM1668_WriteByte(unsigned char Byte)
{
  unsigned char i;
  for(i=0;i<8;i++) {
      TM1668_CLK_L;
      if(Byte & (1 << i))
          TM1668_DIO_H;
      else 
          TM1668_DIO_L;
      TM1668_CLK_H; 
    }
}


/*************TM1668写指令函数***************/
static void TM1668_WriteCommand(unsigned char Cmd)
{ 
  TM1668_STB_L;   //使能
  TM1668_WriteByte(Cmd);   //写指令
  TM1668_STB_H;  //锁存
}

void TM1668_Init(void)
{

    GPIO_Init(TM1668_DIO_PORT,TM1668_DIO_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(TM1668_CLK_PORT,TM1668_CLK_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(TM1668_STB_PORT,TM1668_STB_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
    
    TM1668_WriteCommand(TM1668COM_7SEG_11GRID);    //7segment 11grid
    TM1668_WriteCommand(TM1668COM_DISPLAY_4_16);    //7segment 11grid
    TM1668_WriteCommand(TM1668COM_ADDRESS_CONTINUE);
  
}

/************TM1668写地址数据函数**************/
void TM1668_Update(void)
{
    unsigned char i;
    TM1668_STB_L;
    TM1668_WriteByte(TM1668COM_CONFIG_ADDRESS+0x00); 
    for(i=0;i<7;i++) {
        TM1668_WriteByte(display_buffer[i]);   //seg0-7
        TM1668_WriteByte(0x00);                //seg8 9
    }
    TM1668_STB_H;  //锁存
    
    TM1668_WriteCommand(TM1668COM_DISPLAY_ON);
}


void TM1668_DisplayAll(void)
{
    u8 i;
    for(i=0;i<7;i++)
        display_buffer[i] = display_code[DISP_ALL];
    
    TM1668_Update();
}

void TM1668_DisplayMode(void)
{   
    u32 i;
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_NULL[i]]; 
    
    i = MachData.weigh_fullrange / pow(10,MachData.weigh_dotpos);
    if(0 == i/1000)
        display_buffer[5] = display_code[DISP_NULL];
    else
        display_buffer[5] = display_code[i/1000];
    
    display_buffer[4] = display_code[i%1000/100];
    display_buffer[3] = display_code[i%100/10];
    display_buffer[2] = display_code[i%10] | SEG_P;
    
    i = MachData.weigh_onestep;
    
    display_buffer[1] = display_code[(i%100)/10];
    display_buffer[0] = display_code[i%10];

    TM1668_Update();
}

void TM1668_Display_Factory(void)
{
     u8 i;
    
     if(FAC_EXIT==FactoryData.factorystep) {
       Display_InnerCode(MData.ad_dat_avg);
       TM1668_Update();
       return;
     }
     
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_FACTORY[i]]; 
    display_buffer[4] = display_code[FactoryData.factorystep];
    
    i = 0;
    switch(FactoryData.factorystep) {
    case FAC_FULL:
        i = (u8)weigh_fullrange[FactoryData.factoryindex];
        break;
    case FAC_FENDU:
        i = weigh_onestep[FactoryData.factoryindex];
        break;
    case FAC_DOT:
        i = weigh_dot[FactoryData.factoryindex];
        break;
    case FAC_DISPLAYMIN:
        i = weigh_displaymin[FactoryData.factoryindex];
        break;
    case FAC_LOADTRACK:
        i = auto_loadtrackrange[FactoryData.factoryindex];
        break;     
    case FAC_ZEROLIMIT:
        i = poweron_zerorange[FactoryData.factoryindex];
        break;     

    case FAC_KEYCOUNT:
        i = key_count[FactoryData.factoryindex];
        break;     
        
    default:
        break;
    }
    
    display_buffer[2] = display_code[i/100];
    display_buffer[1] = display_code[(i%100)/10];
    display_buffer[0] = display_code[i%10];

    //MData_update_LED();
    TM1668_Update();
}

void  TM1668_Display_CalTips(void)
{
    u8 i;
    
    if(MACHINE_NORMAL_MODE + MACHINE_USERCAL1_MODE == MachData.mode) {
        for(i=0;i<6;i++)
            display_buffer[i] = display_code[display_USERCALTIP1[i]];
    } else {
        for(i=0;i<6;i++)
            display_buffer[i] = display_code[display_USERCALTIP2[i]];
    }
}

void  TM1668_Display_CalLoad(void)
{
    u8 i;
    u32 tmp;
    
    if(MACHINE_NORMAL_MODE + MACHINE_USERCAL1_MODE == MachData.mode)
        tmp = weigh_calmode1[FactoryData.factoryindex];
    else {
        if((CAL_LOAD1 == CalData.calstep)||(CAL_LOAD1_FLASH == CalData.calstep))
            tmp = weigh_calmode2[0][FactoryData.factoryindex];
        else
            tmp = weigh_calmode2[1][FactoryData.factoryindex];
    }
    
    sprintf((char*)display_buffer,"%06ld", tmp);
    Display_SwapBuffer();
    
    for(i=0;i<6;i++)
        display_buffer[i] -= 0x30;     
    
    Display_ClearPreZero(5, 0,display_buffer);
    
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[display_buffer[i]];
    
    display_buffer[MachData.weigh_dotpos] |= SEG_P;
}


void TM1668_Display_UserCal(void)
{
    u32 i;    
    static u8 cnt = 0;
    cnt++;
    if(cnt>=10)
        cnt = 0;
    
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_USERCAL[i]]; 
    
    switch(CalData.calstep) {
    case CAL_TIP:
        TM1668_Display_CalTips();
        break;
    case CAL_WAIT_ZERO:
        Display_InnerCode(MData.ad_dat_avg);
        break;
    case CAL_LOAD1_FLASH:
        if(cnt>5)
            TM1668_Display_CalLoad();
        break;
    case CAL_LOAD1:
        TM1668_Display_CalLoad();
        break;
    case CAL_LOAD2_FLASH:
        if(cnt>5)
            TM1668_Display_CalLoad();
        break;
    case CAL_LOAD2:
        TM1668_Display_CalLoad();
        break;
    case CAL_PASS1:
        for(i=0;i<6;i++)	
            display_buffer[i] = display_code[display_USERCALPASS1[i]];
        break;
    case CAL_PASS2:
        for(i=0;i<6;i++)	
            display_buffer[i] = display_code[display_USERCALPASS2[i]];
        break;
        
    case CAL_ZERO_TOO_SMALL: //err
    case CAL_ZERO_TOO_BIG:
    case CAL_LOAD1_TOO_SMALL:
    case CAL_LOAD1_TOO_BIG:
    case CAL_LOAD2_TOO_SMALL:
    case CAL_LOAD2_TOO_BIG:
        for(i=0;i<6;i++)	
            display_buffer[i] = display_code[display_USERCALERR[i]];
        display_buffer[0] = display_code[CalData.calstep - 10];
        break;    
    default:break;
    }
     
    TM1668_Update();
}

void TM1668_Display_Normal(void)
{
    u32 i;
    //display proc 
    if(1 == RunData.power_on_flag) {
        Display_Wait();
    } else if(1 == RunData.lowpower_flag) {
        for(i=0;i<6;i++)	
           display_buffer[i] = display_code[display_NULL[i]];
        
    } else if(1 == RunData.full_flag){
        for(i=0;i<6;i++)	
           display_buffer[i] = display_code[display_FULL[i]]; 
        if(0==RunData.positive_flag) //
           display_buffer[5] = display_code[DISP_X]; 
    } else {
        switch(RunData.current_mode){
        case STAT_BATTERY:
            Display_Battery();
            break;
        case STAT_PCS_SAMPLE:
            Display_PrePCS();
            break;
        case STAT_PCS_ERR:
            Display_PCSErr();
            break;
        case STAT_PCS:
            Display_PCS();
            break;
        case STAT_WEIGHT:
            Display_Weight();
            break;
        default:
            break;
        }
    }
    MData_update_LED();
    TM1668_Update();
}
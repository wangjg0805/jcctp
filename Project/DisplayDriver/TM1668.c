#include "stm8s.h"
#include "TM1668.h"
#include "factory.h"
#include "global.h"


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
 						   SEG_A+SEG_B+SEG_C+SEG_E+SEG_F,            //N    0x18
                           SEG_B+SEG_C+SEG_E+SEG_F+SEG_G,            //H    0x19
                           SEG_A+SEG_B+SEG_C+SEG_D+SEG_E+SEG_F+SEG_G+SEG_P, //8. 0x1a
                           0                                         //NULL 0x1b                        
};

//LPMODE
u8 const display_LPMODE[]  = {DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL,DISP_NULL};
//FULL
u8 const display_FULL[]  = {DISP_NULL,DISP_F,DISP_U,DISP_L,DISP_L,DISP_NULL}; 
//USERCAL
u8 const display_USERCAL[] = { DISP_C,   DISP_A,   DISP_L,   DISP_X,  DISP_X,  DISP_X};
u8 const display_USERCALERR[] = { DISP_E,   DISP_r,   DISP_r,   DISP_X,  DISP_X,  DISP_X};
//FACTORYCAL
u8 const display_FACTORYCAL_ZERO[] = { DISP_L,   1,   DISP_N,   DISP_X,  DISP_X, DISP_X};
//factory mode
u8 const display_FACTORY[] = {DISP_F,  DISP_X,  DISP_NULL,  DISP_NULL,  DISP_NULL,  DISP_NULL};
u8 const display_LINECAL[] = {DISP_L,  DISP_N,     DISP_X,     DISP_X,     DISP_X,  DISP_X};

u8 const display_COUNT[]  = {DISP_C,  0  ,DISP_U,  DISP_X,  DISP_X,  DISP_X}; 


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
    TM1668_WriteCommand(TM1668COM_ADDRESS_CONTINUE);
  
}




/************TM1668写地址数据函数**************/
static void TM1668_Write1Data(unsigned char Addr,u8 Data)
{
    TM1668_STB_L;
    TM1668_WriteByte(Addr);
    TM1668_WriteByte(Data&0xff);
    //TM1668_WriteByte((Data>>8)&0xff); //for seg9 seg10
    TM1668_STB_H;
}



/************TM1668写地址数据函数**************/
void TM1668_Update(void)
{
    unsigned char i;
    TM1668_STB_L;
    TM1668_WriteByte(TM1668COM_CONFIG_ADDRESS+0x00); 
    for(i=0;i<6;i++) {
        TM1668_WriteByte(display_buffer[5-i]);   //seg0-7
        TM1668_WriteByte(0x00);                //seg8 9
    }
    TM1668_STB_H;  //锁存
    
    TM1668_WriteCommand(TM1668COM_DISPLAY_ON);
}


void TM1668_DisplayAll(void)
{
    u8 i;
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[DISP_ALL];
    
    TM1668_Update();
}

void TM1668_DisplayMode(void)
{   
    u8 i;
    i = MachData.weigh_fullrange/1000;
    
    display_buffer[0] = display_code[i/100];
    display_buffer[1] = display_code[(i%100)/10];
    display_buffer[2] = display_code[i%10];
    i = MachData.weigh_onestep;
    display_buffer[3] = display_code[i/100];
    display_buffer[4] = display_code[(i%100)/10];
    display_buffer[5] = display_code[i%10];

    TM1668_Update();
}


void TM1668_DisplayWait(void)
{
    u8 i;
    for(i=0;i<6;i++)
        display_buffer[i] = display_code[DISP_X];
    
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
    display_buffer[1] = display_code[FactoryData.factorystep];
    
    i = 0;
    switch(FactoryData.factorystep) {
    case FAC_FULL:
        i = weigh_fullrange[FactoryData.factoryindex];
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
    default:
        break;
    }
    
    display_buffer[3] = display_code[i/100];
    display_buffer[4] = display_code[(i%100)/10];
    display_buffer[5] = display_code[i%10];

    TM1668_Update();
}


void TM1668_Display_LineCal(void)
{
    u8 i;
    static u32 cnt = 0;
    cnt++;
    if(cnt>=10)
        cnt = 0;
     
    for(i=0;i<6;i++)	
        display_buffer[i] = display_code[display_LINECAL[i]]; 
    
    switch(CalData.linecalstep) {
    case 1:
        Display_InnerCode();  //Display innercode
        break;
    case 2:
        display_buffer[4] = display_code[0];
        break;
    case 3: //flash
        if(MData.hx711_data < MData.ad_zero_data + 1000) {
            if(cnt<5) {
                display_buffer[4] = display_code[DISP_F];
            } else {
                display_buffer[4] = display_code[DISP_X];
            } 
        } else {
            display_buffer[4] = DISP_F;
        }
        break;
    default:
        break;
    }
    
    TM1668_Update();
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
    
    switch(CalData.usercalstep) {
    case 1:
        Display_InnerCode(MData.ad_dat_avg);
        break;
    case 2:
        display_buffer[4] = display_code[DISP_F];
        break;
    case 3:
        display_buffer[4] = display_code[DISP_F];
        break;
        
    case 11: //err
    case 12:
    case 13:
    case 14:
        for(i=0;i<6;i++)	
            display_buffer[i] = display_code[display_USERCALERR[i]];
        display_buffer[4] = display_code[CalData.usercalstep - 10];
        break;    
    default:break;
    }
     
    TM1668_Update();

}

void TM1668_Display_Normal(void)
{
    u32 i;
    //display proc 
    if(1 == RunData.lowpower_flag) {
        Display_LPmode();
    } else if(1 == RunData.full_flag){
        for(i=0;i<6;i++)	
           display_buffer[i] = display_code[display_FULL[i]]; 
    } else {
        switch(RunData.current_mode){
        case STAT_PCS_SAMPLE:
            Display_PrePCS();
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
    
    TM1668_Update();
}
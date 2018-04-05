#include "stm8s.h"

#include "global.h"


static u16 key_press_time[20];    //记录每个按键按下的时间
static u8 key_released_flag = 0;
static u8 key_read_flag = 1;

void ScanDriveLine(u8 index)
{
    switch(index) 
    {
    case 0:
        GPIO_WriteLow(GPIOB,   GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            
    case 1:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            

    case 2:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_2);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_3);
        break;            

    case 3:
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_0);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_1);
        GPIO_WriteHigh(GPIOB,  GPIO_PIN_2);
        GPIO_WriteLow(GPIOB,   GPIO_PIN_3);
        break;            

    default:break;
    }
}
////////////////////////////
//键盘管脚初始化
///////////////////////////
void Key_Init(void)
 {  //6个按键
    u8 i;
    GPIO_Init(GPIOC,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
    
    for(i=0;i<4;i++)
        key_press_time[i] = 0;
    
 }



//**********************************************************************
//函数  名:Key_Scan
//函数功能:
//入口参数:无
//出口参数:无
//说    明:每2ms扫描一次
//**********************************************************************

void  Key_Scan(void)
{
    u8 i,key_tmp;
   
    key_tmp = (GPIO_ReadInputData(GPIOC) >> 1) & 0x0f; //某位为0表示有按键
    if(0x0f == key_tmp) {
        key_released_flag = 1;
        if(1 == key_read_flag){
            for(i=0;i<4;i++)
                key_press_time[i] = 0;  //清零
        }
    } else {//有键按下
        key_released_flag = 0;
        key_read_flag = 0;        
        key_press_time[key_tmp]++;
    }   
}
/****************************************************************************
* 名称：Get_KeyCode()
* 功能：读键码
* 入口参数：无
* 出口参数：低8位键码(0~15), 高8位动作(0 未按,1 按下,2 按下2s, 3 按下5s)
****************************************************************************/
u16 Key_GetCode(void)
{
    u8 i;
    u16 Key_CodeTmp = 0;
    
    if((key_read_flag == 0) && (key_released_flag == 1)) {        
        for(i=0;i<16;i++) {
            if(key_press_time[i] > KEY_PRESS_TIME_3S) {
                key_press_time[i] = 0;
                Key_CodeTmp = KEY_PRESSED_3S + i;
            }
        }
        
        if(0 == Key_CodeTmp) {
            for(i=0;i<16;i++) {
                if(key_press_time[i] > KEY_PRESS_TIME) {
                    key_press_time[i] = 0;
                    Key_CodeTmp = KEY_PRESSED + i;
                }
            }
        }
    
    key_read_flag = 1;
   }
   
    return(Key_CodeTmp);
}

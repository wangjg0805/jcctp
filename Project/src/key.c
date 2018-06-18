#include "stm8s.h"

#include "global.h"


static u16 key_press_time[20];    //��¼ÿ���������µ�ʱ��
static u8 key_released_flag = 0;
static u8 key_read_flag = 1;

////////////////////////////
//���̹ܽų�ʼ��
///////////////////////////
void Key_Init(void)
 {  //6������
    u8 i;
    GPIO_Init(GPIOC,GPIO_PIN_1,GPIO_MODE_IN_PU_IT);
    GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_IN_PU_IT);
    GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_IT);
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_IN_PU_IT);
    
    for(i=0;i<4;i++)
        key_press_time[i] = 0;
    
    EXTI_DeInit();      
    EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);//�½��ش����ж�
    EXTI_SetExtIntSensitivity((EXTI_PORT_GPIOC),EXTI_SENSITIVITY_FALL_ONLY);
  
    enableInterrupts();
    
 }



void  Key_ISR(void)
{
    //ONLY FOR WAKE UP 
    RunData.no_key_time = 0;
    printf("exti int occured...\r\n");

}

//**********************************************************************
//����  ��:Key_Scan
//��������:
//��ڲ���:��
//���ڲ���:��
//˵    ��:ÿ2msɨ��һ��
//**********************************************************************

    
void  Key_Scan(void)
{
    u8 i,key_tmp;
   
    key_tmp = (GPIO_ReadInputData(GPIOC) >> 1) & 0x0f; //ĳλΪ0��ʾ�а���
    if(0x0f == key_tmp) {
        key_released_flag = 1;
        if(1 == key_read_flag){
            for(i=0;i<4;i++)
                key_press_time[i] = 0;  //����
        }
    } else {//�м�����
        key_released_flag = 0;
        key_read_flag = 0;        
        key_press_time[key_tmp]++;
    }   
}

/****************************************************************************
* ���ƣ�Get_KeyCode()
* ���ܣ�������
* ��ڲ�������
* ���ڲ�������8λ����(0~15), ��8λ����(0 δ��,1 ����,2 ����2s, 3 ����5s)
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

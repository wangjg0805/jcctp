#ifndef __KEY_H__
#define __KEY_H__
#include "stm8s.h"

#define KEY_NUM              16       
//2ms ɨ��һ�ΰ���
#define KEY_PRESS_TIME       10       //100ms
#define KEY_PRESS_TIME_3S    300      //3s


//����ʶ����
//����״̬��־  2ms
#define KEY_PRESSED          0x1000
#define KEY_PRESSED_3S       0x2000

#define KEY_NULL          0

#define KEY_UNITMODE         0x07
#define KEY_PCSCONFIRM       0x0B
#define KEY_TARECAL          0x0D
#define KEY_RSV              0x0E


void Key_Init(void);
void Key_Scan(void);
u16 Key_GetCode(void);
#endif
#ifndef __KEY_H__
#define __KEY_H__
#include "stm8s.h"

#define KEY_NUM                 16       
//2ms 扫描一次按键
#define KEY_PRESS_TIME          15       //150ms
#define KEY_LONGPRESS_START     50       //ui action from 500ms 
#define KEY_PRESS_TIME_3S      300       //3s



//按键识别码
//按键状态标志  2ms
#define KEY_PRESSED          0x1000
#define KEY_PRESSING         0x2000
#define KEY_PRESSED_3S       0x4000
#define KEY_RELEASED         0x8000

#define KEY_NULL          0

#define KEY_UNITMODE         0x07
#define KEY_PCSCONFIRM       0x0B
#define KEY_TARECAL          0x0D
#define KEY_CAL              0x0E
#define KEY_RSV              0x00

void Key_Init(void);
void Key_Scan(void);
u16 Key_GetCode(void);
#endif
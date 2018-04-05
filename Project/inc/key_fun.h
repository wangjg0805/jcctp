#ifndef __KEYFUN_H__
#define __KEYFUN_H__

#include "stm8s.h"
#include "global.h"

//------------------------------------------------------------
void Key_TareProc(void);
void Key_ZeroProc(void);
void Key_TotalProc(void);

void Key_Proc(u16 key);

#endif
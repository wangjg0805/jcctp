#ifndef __KEYFUN_H__
#define __KEYFUN_H__

#include "stm8s.h"
#include "global.h"

//------------------------------------------------------------
extern void Key_TareProc(void);
extern void Key_ZeroProc(void);
extern void Key_TotalProc(void);

extern void Key_Proc(u16 key);

extern void Key_Proc_Linecal(u16 key);
extern void Key_Proc_Factory(u16 key);
extern void Key_Proc_UserCal(u16 key);

#endif
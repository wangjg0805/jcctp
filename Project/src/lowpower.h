#ifndef __LOWPOWER_H__
#define __LOWPOWER_H__
#include "stm8s.h"


extern void Init_AWU(void);
extern void Sys_LPEnable(void);
extern void Sys_LPDisable(void);
extern u8 LPmode_Check(void);

#endif
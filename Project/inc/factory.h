#ifndef __FACTORY_H__
#define __FACTORY_H__

extern const u8 weigh_fullrange[];
extern const u8 weigh_onestep[];
extern const u8  weigh_dot[];
extern const u8  weigh_displaymin[];
extern const u8  weigh_bkofftime[];
extern const u8  poweron_zerorange[]; 
extern const u8  auto_loadtrackrange[]; 


extern void Key_Proc_Factory(u16 i);
extern void Key_Proc_Cal(u16 i);
extern void Display_Factory(void);
extern void Display_UserCal(void);
extern void Display_LineCal(void);

#endif
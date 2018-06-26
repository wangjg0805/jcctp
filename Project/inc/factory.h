#ifndef __FACTORY_H__
#define __FACTORY_H__

extern const u32 weigh_calmode1[];
extern const u32 weigh_calmode2[2][10];

extern const u8 weigh_fullrange[];
extern const u8 weigh_onestep[];
extern const u8 weigh_dot[];
extern const u8 weigh_displaymin[];
extern const u8 weigh_bkofftime[];
extern const u8 poweron_zerorange[]; 
extern const u8 auto_loadtrackrange[]; 
extern const u8 key_count[]; 


extern void Key_CalExit(void);
extern void FactoryGetFirstStepIndex(void);
extern void Key_FactoryUnitProc(void);
extern void Key_FactoryTareProc(void);
extern void Key_FactoryPCSProc(void);

extern void Key_UserCalUnitProc(void);
extern void Key_UserCalPCSProc(void);
extern void Key_UserCalAutoProc(void); 

extern void Display_Factory(void);
extern void Display_UserCal(void);
extern void Display_LineCal(void);

#endif
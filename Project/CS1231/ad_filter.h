//ad_filter.h
#ifndef __AD_FILTER_H__
#define __AD_FILTER_H__

#define RAW_DATA_MAG       1  //Ñ¹Ëõ¼¸±¶
#define AD_FILTER_LENGTH   8

extern void Filter_Init(void);
extern void ad_filter(u32 ad);
extern void manual_break_stable(void);
extern u8 Filt_GetFastFilterFlag(void);
#endif


	

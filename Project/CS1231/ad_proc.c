//ad_proc.c
//--------------------------------------------------------------
#include  "global.h"
#include  "ad_filter.h"

#include  "i2c.h"
#include  "stdio.h" 
#include  "stdlib.h" //int abs
#include  "math.h"   //float abs

/*
void autoload_track(void)
{
    static u32 trackcnt = 0;
	static u32 lock_ad_last = 0;
	u32 weigh_tmp;
    if(FALSE == RunData.stable_flag) {
        trackcnt = 0;
        return;
    }
    if(1==RunData.power_on_flag) {
        trackcnt = 0;
        return;
    }
    if((MACHINE_NORMAL_MODE+MACHINE_USERCAL_MODE) == MachData.mode) {
        trackcnt = 0;
        return;
    }
    if(abs(MData.ad_dat_avg-MData.weigh_ad_zero) < FilterData.ad_filter_para*100) {
        trackcnt = 0;
        return;
    }
    trackcnt++;
	if(1 == trackcnt) {
        lock_ad_last = MData.ad_dat_avg - MData.weigh_ad_zero;
    } else if(0 == trackcnt % 15) {
        weigh_tmp = MData.ad_dat_avg-MData.weigh_ad_zero;
        if(abs(weigh_tmp-lock_ad_last) <  FilterData.ad_filter_para*3) {
            if(weigh_tmp > lock_ad_last)
                MData.weigh_ad_zero += (weigh_tmp - lock_ad_last);
            else
                MData.weigh_ad_zero -= (lock_ad_last - weigh_tmp);
            printf("load track... weigh_ad_zero is: %ld \r\n", MData.weigh_ad_zero);
            MData.ad_dat_avg = lock_ad_last + MData.weigh_ad_zero;
        } else {
            lock_ad_last = MData.ad_dat_avg - MData.weigh_ad_zero;
            RunData.stable_flag = FALSE;
            printf("load track is broken,lock_ad_last is: %ld \r\n",lock_ad_last);
		}
    } else {
        MData.ad_dat_avg = lock_ad_last + MData.weigh_ad_zero;      
    }
}
*/ 

void autoload_track(void)
{
    static u32 trackcnt = 0;
	static u32 lock_ad_last = 0;
	u32 weigh_tmp;
    if(FALSE == RunData.stable_flag) {
        trackcnt = 0;
        return;
    }
    if(1==RunData.power_on_flag) {
        trackcnt = 0;
        return;
    }
    if((MACHINE_NORMAL_MODE+MACHINE_USERCAL_MODE) == MachData.mode) {
        trackcnt = 0;
        return;
    }
    if(abs(MData.ad_dat_avg-MData.ad_zero_data) < FilterData.ad_filter_para*100) {
        trackcnt = 0;
        return;
    }
    trackcnt++;
	if(1 == trackcnt) {
        lock_ad_last = MData.ad_dat_avg - MData.ad_zero_data;
    } else if(0 == trackcnt % 15) {
        weigh_tmp = MData.ad_dat_avg-MData.ad_zero_data;
        if(abs(weigh_tmp-lock_ad_last) <  FilterData.ad_filter_para*MachData.loadtrackrange) {
            MData.ad_dat_avg = lock_ad_last + MData.ad_zero_data;
        } else {
            lock_ad_last = MData.ad_dat_avg - MData.ad_zero_data;
            RunData.stable_flag = FALSE;
            printf("load track is broken,lock_ad_last is: %ld \r\n",lock_ad_last);
            trackcnt = 1;
		}
    } else {
        MData.ad_dat_avg = lock_ad_last + MData.ad_zero_data;      
    }
}


void autozero_track(void)
{ 
    if(0==FilterData.zero_track_enable)
        return;
    if(0!=MData.ad_tare_data)
        return;
    if(1==RunData.power_on_flag)
        return;
    if((MACHINE_NORMAL_MODE+MACHINE_USERCAL_MODE) == MachData.mode)
        return;
    if(abs(MData.ad_dat_avg-MData.ad_zero_data) < FilterData.ad_filter_para*AUTOZERO_TRACK_RANGE) {
	    FilterData.zero_track_cnt++;
	    if(FilterData.zero_track_cnt >= AUTOZERO_TRACK_TIME) {
	        FilterData.zero_track_cnt = 0;
	        MData.ad_zero_data = MData.ad_dat_avg;
            printf("auto zero working,weigh_ad_zero is %ld \r\n",MData.ad_zero_data);
	    }
	} else  
	    FilterData.zero_track_cnt = 0;
}

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void  do_tare_proc(void)
{	
    MData.ad_tare_data = MData.ad_dat_avg - MData.ad_zero_data;
  
}

//******************************************************************
void do_zero_proc(void)//手动置零+-2%
{
    //手动置零 条件
    if(labs(MData.ad_dat_avg-MData.poweron_zero_data) < (MachData.weigh_ad_full * MANUAL_ZERO_RANGE /100)) {
        MData.ad_zero_data = MData.ad_dat_avg;
        MData.ad_tare_data = 0;
        RunData.zero_flag = 1;
    } else {
        do_tare_proc();
    }
}


//********************************************************************



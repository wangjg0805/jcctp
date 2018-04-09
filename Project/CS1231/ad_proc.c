//ad_proc.c
//--------------------------------------------------------------
#include  "global.h"
#include  "ad_filter.h"

#include  "i2c.h"
#include  "stdio.h" 
#include  "stdlib.h" //int abs
#include  "math.h"   //float abs

void autoload_track(void)
{
    static u8 debugtimes = 0;
    static u32 trackcnt = 0;
	static u32 netweight = 0;
	u32 weigh_tmp;
    if((0==RunData.stable_flag)||(1==RunData.power_on_flag)) {
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
    
    if(9 == MachData.loadtrackrange)
        return;
    
    trackcnt++;
	if(1 == trackcnt) {
        netweight = MData.ad_dat_avg - MData.ad_zero_data;
        printf("start load track..... netweigth: %ld \r\n",netweight);
    } else if(0 == trackcnt % 15) {
        weigh_tmp = MData.ad_dat_avg-MData.ad_zero_data;
        if(abs(weigh_tmp-netweight) < FilterData.ad_filter_para*MachData.loadtrackrange) {
            MData.ad_dat_avg = netweight + MData.ad_zero_data;
            //debugtimes++;
            printf("auto load track working , MData.ad_dat_avg is: %ld \r\n",MData.ad_dat_avg);
        } else {
            //netweight = MData.ad_dat_avg - MData.ad_zero_data;
            trackcnt = 0;
            printf("trackcnt = 0 , MData.ad_dat_avg is: %ld \r\n",MData.ad_dat_avg);
            //printf("restart auto load track...... \r\n");
		}
    } else {
        MData.ad_dat_avg = netweight + MData.ad_zero_data;      
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



//ad_proc.c
//--------------------------------------------------------------
#include  "global.h"
#include  "ad_filter.h"

#include  "i2c.h"
#include  "stdio.h" 
#include  "stdlib.h" //int abs
#include  "math.h"   //float abs

static u32 trackcnt = 0;
static u32 loadtrack_load = 0;
    
static int loadtrackoffset = 0;

static void autoload_exit(void)
{
    trackcnt = 0;
    loadtrackoffset = 0;
    //loadtracksign = 0;
}


void autoload_track(void)
{
	u32 weigh_tmp,tmp;

    if(0==RunData.stable_flag) {
       autoload_exit();
       return;
    }
    if(abs(MData.ad_dat_avg-MData.ad_zero_data) < FilterData.ad_filter_para*50) {
        autoload_exit();
        return;
    }
    if(9 == MachData.loadtrackrange) { //user config disable this function
        autoload_exit();
        return;
    }
    
    trackcnt++;
    if(1 == trackcnt) {
        //loadtrack_zero = MData.ad_zero_data;
        loadtrack_load = MData.ad_dat_avg - MData.ad_zero_data;
    } else if(0 == trackcnt%10) {
        weigh_tmp = MData.ad_dat_avg - MData.ad_zero_data;       
        tmp = abs(weigh_tmp - loadtrack_load);
        if(tmp < FilterData.ad_filter_para*MachData.loadtrackrange) {
            if(weigh_tmp > loadtrack_load) {
                MData.ad_zero_data = MData.ad_zero_data + tmp;
                loadtrackoffset += tmp;
                printf("MData.ad_zero_data: %ld \r\n",MData.ad_zero_data);
            } else {
                MData.ad_zero_data = MData.ad_zero_data - tmp;
                loadtrackoffset -= tmp;
                printf("MData.ad_zero_data: %ld \r\n",MData.ad_zero_data);
            }
            
            if(loadtrackoffset > FilterData.ad_filter_para*(MachData.loadtrackrange+0.5)) {
                 MData.ad_zero_data = MData.ad_zero_data - loadtrackoffset;
                 printf("autoload track working , loadtrackoffset: %ld \r\n",loadtrackoffset);
                 autoload_exit();
            }
                
        } else {
            trackcnt = 0;
            //printf("trackcnt = 0 , MData.ad_dat_avg is: %ld \r\n",MData.ad_dat_avg);
		}
    } else {
        MData.ad_dat_avg = MData.ad_zero_data + loadtrack_load;      
    }
}


void autozero_track(void)
{ 
    
    if(0 == RunData.stable_flag) {
        FilterData.zero_track_cnt = 0;
        return;
    }
    
    if(0!=MData.ad_tare_data) {
        //printf("MData.ad_tare_data is %ld \r\n",MData.ad_tare_data);
        return;
    }
        
    if(abs(MData.ad_dat_avg-MData.ad_zero_data) < FilterData.ad_filter_para*AUTOZERO_TRACK_RANGE) {
	    FilterData.zero_track_cnt++;
	    if(FilterData.zero_track_cnt >= AUTOZERO_TRACK_TIME) {
	        FilterData.zero_track_cnt = 0;
	        MData.ad_zero_data = MData.ad_dat_avg;
           // printf("auto zero working,weigh_ad_zero is %ld \r\n",MData.ad_zero_data);
            RunData.zero_flag = 1;
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
    } else if(MData.ad_dat_avg > MData.poweron_zero_data) {  //only in positive status
        do_tare_proc();
    }
}


//********************************************************************




//ad_filter.c
//--------------------------------------------------------------
#include  "global.h"
#include  "stdlib.h"
#include  "stdio.h"

#include  "ad_filter.h"
#include  "math.h"		


 
static u32 ad_dat[AD_FILTER_LENGTH];
static u8  ad_dat_idx;
static u8  ad_stable_cnt;

static u8  flag_ad_buf_full = 0;
static u8  fast_filter_flag = 0;

//--------------------------------------------------------------
void Filter_Init(void)
{
	u8 i;
	for(i=0;i<AD_FILTER_LENGTH;i++)
		ad_dat[i]=0;
	ad_dat_idx = 0;
 	ad_stable_cnt = 0;
    flag_ad_buf_full = 0;
    fast_filter_flag = 0;
    
    FilterData.ad_filter_para = 5.0;
    FilterData.zero_track_enable = 0;
    FilterData.zero_track_cnt = 0;
    //FilterData.load_track_enable = 0;
    //FilterData.load_track_cnt = 0; 
}

//--------------------------------------------------------------
//*************************************************************
u32 ad_filter0(u32 ad_new_dat)
{
    static u32 direct_add_cnt     = 0;
    static u32 direct_sub_cnt     = 0;
    static u8 filter0_stable_cnt = 0;    
    static u32 ad_last_dat  = 0;  
    u32 tmp;
    tmp = abs(ad_new_dat - ad_last_dat);
    
    if(tmp > FilterData.ad_filter_para*3) {  //�����������ݲ��ܴ� 	         
        if(ad_new_dat >= ad_last_dat) {
            direct_add_cnt++;
            direct_sub_cnt = 0;
	    } else {
	        direct_sub_cnt++;
            direct_add_cnt = 0;
	    }
        
	    if((direct_add_cnt > 2)||(direct_sub_cnt > 2)) { //�������ݴ���2D �����仯3�ξͽ�������˲�ģʽ
	     fast_filter_flag = 1;
	     filter0_stable_cnt = 0;
	     flag_ad_buf_full = 0;  //�ƻ��������ݣ����¿�ʼ���� 
 	     ad_dat_idx       = 0;
	    }
	    //�������α仯����2D �Ͳ������Զ�����
        FilterData.zero_track_enable  = 0;
        FilterData.zero_track_cnt     = 0;
        //FilterData.load_track_enable  = 0;
        //FilterData.load_track_cnt     = 0;
   	    ad_last_dat = ad_new_dat; 
     } else {
         fast_filter_flag = 0;
         filter0_stable_cnt++;
         if(filter0_stable_cnt > 200)
             filter0_stable_cnt =  30;
         //���ȶ���ʹ����������ֵ��Ȩ��Ϊ��������  
         if(filter0_stable_cnt < 5) {   //�Ӵ�С����  
             ad_last_dat = ad_new_dat;
         } else if(filter0_stable_cnt < 10) {
			 ad_last_dat = (ad_new_dat*9+ad_last_dat)/10; 
         } else {//if(filter0_stable_cnt < 15) {
             ad_last_dat = (ad_new_dat*8+ad_last_dat*2)/10;    //����
         }
         /*
         } else {
             ad_last_dat = (ad_new_dat*7+ad_last_dat*3)/10;    //����
         } 
         */
    }   	  
	return ad_last_dat;
}    
//********************************************************
u32 get_buf_data(void)
{   //��ǰ������������ȡ���������Сֵ���ƽ��ֵ
    //���û����  ����ȡ �������ݵ�ƽ��ֵ
    u32 sum;
    u8 pass,i;
    if(1 == flag_ad_buf_full) {
        for(pass = 0;pass < AD_FILTER_LENGTH - 1 ;pass++) {
            for( i = 0;i < (AD_FILTER_LENGTH - 1 - pass);i++) {
                if(ad_dat[i] > ad_dat[i+1]) {
                    sum = ad_dat[i];
		   	        ad_dat[i] = ad_dat[i+1];
			        ad_dat[i+1] = sum ;
			    }
		    }
	    }
	    sum = 0;  
	    for(i=2;i<6;i++)   
	        sum += ad_dat[i]; //+ad_dat[3]+ad_dat[4]+ad_dat[5]+2;
	        return(sum/4);
    } else {
        sum = 0;
        for(i=0;i<ad_dat_idx;i++)
            sum += ad_dat[i];         
 	    return(sum/ad_dat_idx); 
    }
}     
///////////////////////////////////////////////////////////////////////////   
           
void ad_filter(u32 ad_data) 
{
    u32 tmp;   
    //printf("new ad: %ld \r\n", ad_data);
    tmp = ad_filter0(ad_data); 
    //////////////////////////////////////////
    if(1 == fast_filter_flag) {
        ad_stable_cnt  = 0;
        RunData.stable_flag    = 0;
		//auto_off_cnt   = 0;
 	    MData.ad_dat_avg = tmp/RAW_DATA_MAG;   //���ٱ仯�׶�
         printf("fast_filter stage...... \r\n");
        return;
    }
    
    
    if(0 == flag_ad_buf_full) {
        ad_dat[ad_dat_idx++] = tmp;    //save  
        if(AD_FILTER_LENGTH == ad_dat_idx) {
            flag_ad_buf_full = 1; 
            ad_dat_idx = 0;
        }
        MData.ad_dat_avg = get_buf_data()/RAW_DATA_MAG;  
        printf("fill filter_buf stage...... \r\n");
        return;
    }
    
    ad_stable_cnt++;
 	if(ad_stable_cnt  > 250)
 	    ad_stable_cnt  = 50; 
 	///////////////////////////�ȶ���ʱ
	if(ad_stable_cnt > 7)
		RunData.stable_flag = 1;
    else {//�������ȶ�
		//auto_off_cnt = 0;
        RunData.not_zero_time = 0;
	}

    if(ad_stable_cnt < 5)
        ad_dat[ad_dat_idx++] = (ad_data*9+MData.ad_dat_avg)/10;
    else if(ad_stable_cnt < 10)
        ad_dat[ad_dat_idx++] = (ad_data*8+MData.ad_dat_avg*2)/10;
    else 
        ad_dat[ad_dat_idx++] = (ad_data*7+MData.ad_dat_avg*3)/10;
    
    if(AD_FILTER_LENGTH == ad_dat_idx) {
            ad_dat_idx = 0;
    }
    /*
    if(ad_stable_cnt < 5) {
        ad_dat[ad_dat_idx++] = (tmp*9+ad_dat[AD_FILTER_LENGTH-1])/10;
 	    else
 	        ad_dat[ad_dat_idx++] = (tmp*9+ad_dat[ad_dat_idx-1])/10;     
            } else if(ad_stable_cnt<10){
                if(0 == ad_dat_idx)
                    ad_dat[ad_dat_idx++] = (tmp*8+ad_dat[AD_FILTER_LENGTH-1]*2)/10;
 	            else
 	                ad_dat[ad_dat_idx++] = (tmp*8+ad_dat[ad_dat_idx-1]*2)/10;  
            } else {
                if(0 == ad_dat_idx)
 	                ad_dat[ad_dat_idx++] = (tmp*6+ad_dat[AD_FILTER_LENGTH-1]*4)/10;
 	            else
 	                ad_dat[ad_dat_idx++] = (tmp*6+ad_dat[ad_dat_idx-1]*4)/10;  
            }       
 	    }
    */
 	//�õ�������  
 	MData.ad_dat_avg = get_buf_data()/RAW_DATA_MAG;  
    //printf("stable stage,new ad: %ld \r\n", MData.ad_dat_avg);
}


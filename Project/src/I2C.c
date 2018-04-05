//***********************************************************************
// ��������I2C�ӿ�, ����I2C0����(400K), I2C1����(100K)
//***********************************************************************
#include "stm8s_conf.h"
#include "string.h"
#include "global.h"

////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void Wait(u16 length)
	{
	 while(length >0)
    	 length--;
	}

/****************************************************************************
* ������: I2C0_Start
* ����: ��I2C0��ʼ����
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
static void I2C_Start(void)
{
    SET_I2C_OUTPUT;   
	SET_I2C_SDA;
	Wait(15);
	SET_I2C_SCL;
	Wait(15);
	CLR_I2C_SDA;
	Wait(15);
	CLR_I2C_SCL;
	Wait(15);
}

/****************************************************************************
* ������: I2C0_Stop
* ����: ��I2C0��������
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
static void I2C_Stop(void)
{
    SET_I2C_OUTPUT;     
	CLR_I2C_SCL;
	Wait(15);
	CLR_I2C_SDA;
	Wait(15);
	SET_I2C_SCL;
	Wait(15);
	SET_I2C_SDA;
	Wait(15);
}
/****************************************************************************
* ������: Init_I2C0
* ����: ��ʼ��I2C0
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
static void I2c_PinInit(void)
{
    GPIO_Init(I2C_PORT, I2C_SCL, GPIO_MODE_OUT_PP_LOW_FAST);
    SET_I2C_OUTPUT;  
}

void I2c_Init(void)
{
    I2c_PinInit();
    Wait(1000);
    I2C_Start();
    I2C_Stop();
}
/****************************************************************************
* ������: I2C0_Recv_Byte
* ����: I2C0����һ���ֽ�
* ��ڲ���: �Ƿ�ACK(1��,0����)
* ���ڲ���: �ֽ�����
****************************************************************************/
static u8 I2C_Recv_Byte(u8 ack)
{
    u32 i;
    u8 rb;
    SET_I2C_INPUT;		 //����ģʽ
	for(i=0; i<8; i++) {
		rb <<= 1;
		if(READ_I2C_SDA)
            rb |= 1;
		SET_I2C_SCL;
		Wait(15);
		CLR_I2C_SCL;
		Wait(15);
    }
    
	SET_I2C_OUTPUT;		//���ģʽ
	if(ack) 
        CLR_I2C_SDA; 
    else
        SET_I2C_SDA;	//�Ƿ�ACK
	Wait(15);
	SET_I2C_SCL;
	Wait(15);
	CLR_I2C_SCL;
	Wait(15);
	return(rb);
}

/****************************************************************************
* ������: I2C0_Send_Byte
* ����: I2C0����һ���ֽ�
* ��ڲ���: ���͵��ֽ�
* ���ڲ���: �Ƿ��յ�ACK(1�յ�,0δ�յ�)
****************************************************************************/
static u8 I2C_Send_Byte(u8 wb)
{
    u32 i;
    u8 ack;
    SET_I2C_OUTPUT;
	for(i=0; i<8; i++) {
		if(wb & 0x80)
            SET_I2C_SDA; 
        else
            CLR_I2C_SDA;
		wb <<= 1;
		Wait(15);
		SET_I2C_SCL;
		Wait(15);
		CLR_I2C_SCL;
	}
	SET_I2C_INPUT;			//����ģʽ
	Wait(1000);
	if(READ_I2C_SDA == 0)
        ack = TRUE;
    else 
        ack = FALSE;	//�Ƿ��յ�ACK
	SET_I2C_SCL;
	Wait(15);
	CLR_I2C_SCL;
	Wait(15);
	SET_I2C_OUTPUT;		//���ģʽ
	return(ack);
}

/****************************************************************************
* ������: Read_MainBoard_EEPROM
* ����: ������EEPROM����һ������
* ��ڲ���: addr��EEPROM��ַ(11λ),rdptr��������ָ��,rlen�������ֽ���
* ���ڲ���: ��
****************************************************************************/
void Read_EEPROM(u16 addr, void *rdptr, u16 rlen)
{
    u8 *rptr;
    u16 eepaddr,rl;
	rptr = (u8 *)rdptr;
	while(rlen) {//δ����
		rl = 256 - (addr & 0xFF);
		if(rl>rlen) rl = rlen;
		eepaddr = AT24C16_ADDR | ((addr >> 7) & 0x0E);
		I2C_Start();
		I2C_Send_Byte(eepaddr);
		I2C_Send_Byte((u8)addr);	
		I2C_Start();
		I2C_Send_Byte(eepaddr+1);
		addr += rl;
		rlen -= rl;
		while(rl--) *rptr++ = I2C_Recv_Byte(rl>0);
		I2C_Stop();
	}
}
/****************************************************************************
* ������: Write_MainBoard_EEPROM
* ����: ������EEPROMд��һ������
* ��ڲ���: addr��EEPROM��ַ(11λ),wrptrд������ָ��,wlenд���ֽ���
* ���ڲ���: �����ɹ����(��0��ʾʧ��)
****************************************************************************/
u8 Write_EEPROM(u16 addr, void *wrptr, u16 wlen)
{
    u8 rbuf[16],*wptr,ack,flag;
    u16 eepaddr,wl;
    u32 i;
	wptr = (u8 *)wrptr;
	flag = 0;
	while(wlen) {//δд��
		wl = 16 - (addr & 0x0F);	//һ��д�벻�ܳ���16���ֽںͿ��,�������
		if(wl>wlen) wl = wlen;
		//��������ݱȽ�,��ͬ�Ĳ�����д��
		Read_EEPROM(addr, rbuf, wl);	//����ԭ������
		if(memcmp(wptr, rbuf, wl)!=0) {//�в�ͬ��Ҫд
			eepaddr = AT24C16_ADDR | ((addr >> 7) & 0x0E);
            //
			I2C_Start();
			I2C_Send_Byte(eepaddr);
			I2C_Send_Byte((u8)addr);
			for(i=0; i<wl; i++) I2C_Send_Byte(wptr[i]);
			I2C_Stop();
			//�ȴ���д���
			for(i=0; i<200; i++)
			{
				I2C_Start();
				ack = I2C_Send_Byte(eepaddr);
				I2C_Stop();
				if(ack) break;
			}
			if(!flag)
			{//��ǰУ�鶼��ȷ����²Ž���У��
				Read_EEPROM(addr, rbuf, wl);	//����д������
				flag = memcmp(wptr, rbuf, wl);
			}
		}
        
		addr += wl;
		wptr += wl;
		wlen -= wl;
	}
    
	return(flag);
}

//end of I2C.c
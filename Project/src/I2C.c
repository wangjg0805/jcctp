//***********************************************************************
// 定义两个I2C接口, 其中I2C0高速(400K), I2C1低速(100K)
//***********************************************************************
#include "stm8s_conf.h"
#include "string.h"
#include "global.h"

#if 0
////////////////////////////////////////////
/* 1 delay */
////////////////////////////////////////////
static void Wait(u16 length)
	{
	 while(length >0)
    	 length--;
	}

/****************************************************************************
* 函数名: I2C0_Start
* 功能: 发I2C0起始条件
* 入口参数: 无
* 出口参数: 无
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
* 函数名: I2C0_Stop
* 功能: 发I2C0结束条件
* 入口参数: 无
* 出口参数: 无
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
* 函数名: Init_I2C0
* 功能: 初始化I2C0
* 入口参数: 无
* 出口参数: 无
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
* 函数名: I2C0_Recv_Byte
* 功能: I2C0接收一个字节
* 入口参数: 是否发ACK(1发,0不发)
* 出口参数: 字节内容
****************************************************************************/
static u8 I2C_Recv_Byte(u8 ack)
{
    u32 i;
    u8 rb;
    SET_I2C_INPUT;		 //输入模式
	for(i=0; i<8; i++) {
		rb <<= 1;
		if(READ_I2C_SDA)
            rb |= 1;
		SET_I2C_SCL;
		Wait(15);
		CLR_I2C_SCL;
		Wait(15);
    }
    
	SET_I2C_OUTPUT;		//输出模式
	if(ack) 
        CLR_I2C_SDA; 
    else
        SET_I2C_SDA;	//是否发ACK
	Wait(15);
	SET_I2C_SCL;
	Wait(15);
	CLR_I2C_SCL;
	Wait(15);
	return(rb);
}

/****************************************************************************
* 函数名: I2C0_Send_Byte
* 功能: I2C0发送一个字节
* 入口参数: 发送的字节
* 出口参数: 是否收到ACK(1收到,0未收到)
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
	SET_I2C_INPUT;			//输入模式
	Wait(1000);
	if(READ_I2C_SDA == 0)
        ack = TRUE;
    else 
        ack = FALSE;	//是否收到ACK
	SET_I2C_SCL;
	Wait(15);
	CLR_I2C_SCL;
	Wait(15);
	SET_I2C_OUTPUT;		//输出模式
	return(ack);
}

/****************************************************************************
* 函数名: Read_MainBoard_EEPROM
* 功能: 从主板EEPROM读出一块数据
* 入口参数: addr是EEPROM地址(11位),rdptr读缓冲区指针,rlen读出的字节数
* 出口参数: 无
****************************************************************************/
/*
void Read_EEPROM_i2c(u16 addr, void *rdptr, u16 rlen)
{    
    u8 *rptr;
    u16 eepaddr,rl;
	rptr = (u8 *)rdptr;
	while(rlen) {//未读完
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
*/

/****************************************************************************
* 函数名: Write_MainBoard_EEPROM
* 功能: 向主板EEPROM写入一块数据
* 入口参数: addr是EEPROM地址(11位),wrptr写缓冲区指针,wlen写入字节数
* 出口参数: 操作成功与否(非0表示失败)
****************************************************************************/

/*
u8 Write_EEPROM_i2c(u16 addr, void *wrptr, u16 wlen)
{
    u8 rbuf[16],*wptr,ack,flag;
    u16 eepaddr,wl;
    u32 i;
	wptr = (u8 *)wrptr;
	flag = 0;
	while(wlen) {//未写完
		wl = 16 - (addr & 0x0F);	//一次写入不能超过16个字节和跨段,否则卷绕
		if(wl>wlen) wl = wlen;
		//与读出内容比较,相同的不必再写入
		Read_EEPROM(addr, rbuf, wl);	//读出原来内容
		if(memcmp(wptr, rbuf, wl)!=0) {//有不同的要写
			eepaddr = AT24C16_ADDR | ((addr >> 7) & 0x0E);
            //
			I2C_Start();
			I2C_Send_Byte(eepaddr);
			I2C_Send_Byte((u8)addr);
			for(i=0; i<wl; i++) I2C_Send_Byte(wptr[i]);
			I2C_Stop();
			//等待烧写完成
			for(i=0; i<200; i++)
			{
				I2C_Start();
				ack = I2C_Send_Byte(eepaddr);
				I2C_Stop();
				if(ack) break;
			}
			if(!flag)
			{//以前校验都正确情况下才进行校验
				Read_EEPROM(addr, rbuf, wl);	//读出写入内容
				flag = memcmp(wptr, rbuf, wl);
			}
		}
        
		addr += wl;
		wptr += wl;
		wlen -= wl;
	}
    
	return(flag);
}
*/

#endif

//added bt wjg 20180408
void Read_EEPROM(u16 addr, u8* buf, u16 rlen)
{
    u8 i;
    
    for(i=0;i<rlen;i++) {
        *buf = FLASH_ReadByte(STM32EEPROM_BASE_ADDR + addr);
        addr++;
        buf++;
    }
}


u8 Write_EEPROM(u16 addr, u8* buf, u16 wlen)
{
    u32 tmp;
    disableInterrupts();
    
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
    switch(wlen) {
    case 2:
        FLASH_ProgramByte(STM32EEPROM_BASE_ADDR+addr,  *buf++);
        FLASH_ProgramByte(STM32EEPROM_BASE_ADDR+addr+1,*buf);
        break;
    case 4:
        printf("0x%x,0x%x,0x%x,0x%x \r\n",*buf,*(buf+1),*(buf+2),*(buf+3));
        tmp = BUFtoU32(buf);
        printf("FLASH_Program4byte:%ld \r\n",tmp);
        FLASH_ProgramWord(STM32EEPROM_BASE_ADDR+addr,  tmp);
        break;
    case 8:
        tmp = BUFtoU32(buf);
        printf("FLASH_Program8byte:%ld \r\n",tmp);
        FLASH_ProgramWord(STM32EEPROM_BASE_ADDR+addr,  tmp);
        tmp = BUFtoU32(buf+4);
        printf("FLASH_ProgramWord:%ld \r\n",tmp);
        FLASH_ProgramWord(STM32EEPROM_BASE_ADDR+addr+4,  tmp);
        break;
    }
    
    for(tmp=0;tmp<80000;tmp++){
        ;
    }
    
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    enableInterrupts();
    return(1);
}


///////added by wjg 20180408


/*******************************************************************************
*                         ==EEPROM初始化函数==
* FLASH_DeInit();                                          --> 复位EEPROM的寄存器值
* FLASH_Unlock(FLASH_MEMTYPE_DATA);                         --> 解锁 对 Data EEPROM memory 进行操作
*FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);     --> 标准编程时间
*******************************************************************************/
void EEPROM_Init(void)
{
    FLASH_DeInit();
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}


//end of I2C.c
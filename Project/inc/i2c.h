#ifndef	_I2C_H_
#define	_I2C_H_

#define AT24C16_ADDR	0xA0
#define CHECK_DATA      0x55

//定义I2C0的时钟线和数据线
#define I2C_PORT        GPIOB
#define I2C_SCL		    GPIO_PIN_4
#define I2C_SDA		    GPIO_PIN_5

//定义I2C0基本操作
#define SET_I2C_INPUT	GPIO_Init(I2C_PORT,I2C_SDA,   GPIO_MODE_IN_FL_NO_IT)
#define SET_I2C_OUTPUT	GPIO_Init(I2C_PORT,I2C_SDA,   GPIO_MODE_OUT_OD_LOW_FAST)

#define SET_I2C_SDA		GPIO_WriteHigh(I2C_PORT, I2C_SDA)
#define SET_I2C_SCL		GPIO_WriteHigh(I2C_PORT, I2C_SCL)
#define CLR_I2C_SDA		GPIO_WriteLow(I2C_PORT,  I2C_SDA)
#define CLR_I2C_SCL		GPIO_WriteLow(I2C_PORT,  I2C_SCL)

#define READ_I2C_SDA	GPIO_ReadInputPin(I2C_PORT,I2C_SDA)

//save param
//ZERO DATA

#define    STM32EEPROM_BASE_ADDR          0x4000

#define    EEP_CALFLAG_ADDR               0x00    //cal ok 

#define    EEP_WEIGHTZERO_ADDR            0x10    //线性校准零点
#define    EEP_WEIGHTZERO_ADDR_BAK        0x14    //备份
//FULL DATA
#define    EEP_WEIGHTFULL_ADDR            0x18    //满量程内码
#define    EEP_WEIGHTFULL_ADDR_BAK        0x1C    //备份

//linecal param
#define    EEP_LINECALFLAG_ADDR           0x20    //flag
#define    EEP_LINECAL_P0_ADDR            0x24    // 0
#define    EEP_LINECAL_P1_ADDR            0x28    // 1
#define    EEP_LINECAL_P2_ADDR            0x2C    // 2
#define    EEP_LINECAL_RSV_ADDR           0x30    // 3


//system config area
#define   EEP_SYS_FULLRANGE_ADDR          0x50  //
#define   EEP_SYS_ONESTEP_ADDR            0x54  //
#define   EEP_SYS_DOT_ADDR                0x58  //
#define   EEP_SYS_DISFENDU_ADDR           0x5C  //
#define   EEP_SYS_BKOFFTIME_ADDR          0x60  //
#define   EEP_SYS_ZERORANGE_ADDR          0x64  //
#define   EEP_SYS_LOADTRACK_ADDR          0x68  //

//用户配置 参数 0xB0-0XD0
#define   EEP_USR_FUN1_ADDR               0xB0  //  
#define   EEP_USR_FUN2_ADDR               0xB2  //  
#define   EEP_USR_FUN3_ADDR               0xB4  //
#define   EEP_USR_FUN4_ADDR               0xB6  //
#define   EEP_USR_FUN5_ADDR               0xB8  //
#define   EEP_USR_FUN6_ADDR               0xBA  // 
#define   EEP_USR_FUN7_ADDR               0xBC  //
#define   EEP_USR_FUN8_ADDR               0xBE  //

//////////////////////////////////////////////////////////////////////////////////
//TEST
#define   EEP_TEST_ADDR                  0xF0    //EEPROM TEST
//??????????????
//AT24C01 128Bytes   AT24C08 1024Bytes   AT24C64   8192Bytes   
//AT24C02 256Bytes   AT24C16 2048Bytes   AT24C128 16384Bytes
//AT24C04 512Bytes   AT24C32 4096Bytes   AT24C256 32768Bytes
//???? 100*16=1600BYTES ????24C16 ???????16BYTE
//????24C32??? ??????,????????
#define    EEP_REC_TOTAL_ADDR             0x100     //??????
#define    EEP_REC_TOTAL_ADDR_BAK         0x104     //??????   
#define    EEP_REC_START_ADDR             0x110     //??????????

#define    EEP_BYTE_PER_REC               0x10      //???????????
#define    MAX_REC_NUM                    100       //????????
//#define    RECNUM_PER_PAGE              1         //???10???



////////////////////////////////////////////////////定义EEPROM ADDRESS
//以下信息用于存储用户测量数据
//AT24C01 128Bytes   AT24C08 1024Bytes   AT24C64   8192Bytes   
//AT24C02 256Bytes   AT24C16 2048Bytes   AT24C128 16384Bytes
//AT24C04 512Bytes   AT24C32 4096Bytes   AT24C256 32768Bytes
//计算公式 2048/4 = 512 - 4  
//使用AT24C16 可以存储多达508条记录，目前使用到500条

#define EEPROM_REC_TOTAL_ADDR         0x00   // 最多保存200条记录
#define EEPROM_REC_TOTAL_ADDR_BAK     0x01  

#define REC_DATA_LENGTH               0x04   //每条记录占用4个字节
#define EEPROM_REC_START_ADDR         0x10




//extern void I2c_Init(void);

extern u8 Write_EEPROM(u16 addr, u8* buf, u16 wlen);
extern void Read_EEPROM(u16 addr, u8* buf, u16 rlen);
extern void EEPROM_Init(void);
#endif	/*_I2C_H_*/
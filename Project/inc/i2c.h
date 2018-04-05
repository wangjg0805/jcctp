#ifndef	_I2C_H_
#define	_I2C_H_

#define AT24C16_ADDR	0xA0
#define CHECK_DATA      0x55

//����I2C0��ʱ���ߺ�������
#define I2C_PORT        GPIOB
#define I2C_SCL		    GPIO_PIN_4
#define I2C_SDA		    GPIO_PIN_5

//����I2C0��������
#define SET_I2C_INPUT	GPIO_Init(I2C_PORT,I2C_SDA,   GPIO_MODE_IN_FL_NO_IT)
#define SET_I2C_OUTPUT	GPIO_Init(I2C_PORT,I2C_SDA,   GPIO_MODE_OUT_OD_LOW_FAST)

#define SET_I2C_SDA		GPIO_WriteHigh(I2C_PORT, I2C_SDA)
#define SET_I2C_SCL		GPIO_WriteHigh(I2C_PORT, I2C_SCL)
#define CLR_I2C_SDA		GPIO_WriteLow(I2C_PORT,  I2C_SDA)
#define CLR_I2C_SCL		GPIO_WriteLow(I2C_PORT,  I2C_SCL)

#define READ_I2C_SDA	GPIO_ReadInputPin(I2C_PORT,I2C_SDA)

//save param
//ZERO DATA
#define    EEP_CALFLAG_ADDR               0x00    //cal ok 

#define    EEP_WEIGHTZERO_ADDR            0x10    //����У׼���
#define    EEP_WEIGHTZERO_ADDR_BAK        0x14    //����
//FULL DATA
#define    EEP_WEIGHTFULL_ADDR            0x18    //����������
#define    EEP_WEIGHTFULL_ADDR_BAK        0x1C    //����

//system config area
#define   EEP_SYS_FULLRANGE_ADDR          0x50  //
#define   EEP_SYS_ONESTEP_ADDR            0x54  //
#define   EEP_SYS_DOT_ADDR                0x58  //
#define   EEP_SYS_DISFENDU_ADDR           0x5C  //
#define   EEP_SYS_BKOFFTIME_ADDR          0x60  //
#define   EEP_SYS_ZERORANGE_ADDR          0x64  //
#define   EEP_SYS_LOADTRACK_ADDR          0x68  //

//�û����� ���� 0xB0-0XD0
#define   EEP_USR_FUN1_ADDR               0xB0  //  
#define   EEP_USR_FUN2_ADDR               0xB2  //  
#define   EEP_USR_FUN3_ADDR               0xB4  //
#define   EEP_USR_FUN4_ADDR               0xB6  //
#define   EEP_USR_FUN5_ADDR               0xB8  //
#define   EEP_USR_FUN6_ADDR               0xBA  // 
#define   EEP_USR_FUN7_ADDR               0xBC  //
#define   EEP_USR_FUN8_ADDR               0xBE  //

//ϵͳ���ò��� 0xD0-0xF0
#define   EEP_SYS_FULL_ADDR               0xD0  //
#define   EEP_SYS_DIV_ADDR                0xD4  // 
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



////////////////////////////////////////////////////����EEPROM ADDRESS
//������Ϣ���ڴ洢�û���������
//AT24C01 128Bytes   AT24C08 1024Bytes   AT24C64   8192Bytes   
//AT24C02 256Bytes   AT24C16 2048Bytes   AT24C128 16384Bytes
//AT24C04 512Bytes   AT24C32 4096Bytes   AT24C256 32768Bytes
//���㹫ʽ 2048/4 = 512 - 4  
//ʹ��AT24C16 ���Դ洢���508����¼��Ŀǰʹ�õ�500��

#define EEPROM_REC_TOTAL_ADDR         0x00   // ��ౣ��200����¼
#define EEPROM_REC_TOTAL_ADDR_BAK     0x01  

#define REC_DATA_LENGTH               0x04   //ÿ����¼ռ��4���ֽ�
#define EEPROM_REC_START_ADDR         0x10




void I2c_Init(void);
u8 Write_EEPROM(u16 addr, void *wrptr, u16 wlen);
void Read_EEPROM(u16 addr, void *rdptr, u16 rlen);
#endif	/*_I2C_H_*/
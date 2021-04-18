#ifndef		__FDC2214_H
#define		__FDC2214_H

#define FDC2214_ADDR 0x2A//ADDR�ӵ�
//#define FDC2214_ADDR 0x2A//ADDR�ӵ�Դ

/*FDC2214��ؼĴ�������*/
#define DATA_CH0 0x00     				//?????
#define DATA_LSB_CH0 0x01
#define DATA_CH1 0x02
#define DATA_LSB_CH1 0x03
#define DATA_CH2 0x04
#define DATA_LSB_CH2 0x05
#define DATA_CH3 0x06
#define DATA_LSB_CH3 0x07
#define RCOUNT_CH0 0x08    //
#define RCOUNT_CH1 0x09
#define RCOUNT_CH2 0x0A
#define RCOUNT_CH3 0x0B
//#define OFFSET_CH0 0x0C  //FDC2114
//#define OFFSET_CH1 0x0D
//#define OFFSET_CH2 0x0E
//#define OFFSET_CH3 0x0F
#define SETTLECOUNT_CH0 0x10//ת��֮ǰ���ȶ�ʱ��(T=(SETTLECOUNT_CHx*16)/Frefx)
#define SETTLECOUNT_CH1 0x11
#define SETTLECOUNT_CH2 0x12
#define SETTLECOUNT_CH3 0x13
#define CLOCK_DIVIDERS_C_CH0 0x14       //������Ƶ��ѡ��
#define CLOCK_DIVIDERS_C_CH1 0x15
#define CLOCK_DIVIDERS_C_CH2 0x16
#define CLOCK_DIVIDERS_C_CH3 0x17
#define STATUS 0x18                     //����״̬����
#define ERROR_CONFIG 0x19 				//״̬�ʹ��󱨸�����
#define CONFIG 0x1A  
#define MUX_CONFIG 0x1B
#define RESET_DEV 0x1C
#define DRIVE_CURRENT_CH0 0x1E          //����������ʱ��+ת��ʱ�����������
#define DRIVE_CURRENT_CH1 0x1F
#define DRIVE_CURRENT_CH2 0x20
#define DRIVE_CURRENT_CH3 0x21
#define MANUFACTURER_ID 0x7E      //ֻ��MANUFACTURER ID:0x5449
#define DEVICE_ID 0x7F            //ֻ��DEVICE ID:0x3055



bit FDC2214Init(void);
//void Delay(unsigned char DelayCount);
void FDC2214IICStart(void);
void FDC2214IICStop(void);
bit  FDC2214IICReadAck(void);
bit FDC2214SetParameters(unsigned char RegisterAddress,unsigned char ParaMSB,unsigned char ParaLSB);//����FDC2214
unsigned int FDC2214Read2Bytes(unsigned char RegisterAddress);//��������ַΪ:RegisterAddress��2���ֽ�

void FDC2214IICNoAck(void);
void FDC2214IICAck(void);
void FDC2214IICSendByte(unsigned char sendbyte);
unsigned char FDC2214IICReadByte(void);
unsigned long int FCD2214ReadCH(unsigned char index);//��ͨ��ֵ
unsigned long FilteredFCD2214ReadCH(unsigned char index);//�Զ�ȡֵ�������˲�
#endif
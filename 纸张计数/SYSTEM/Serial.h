#ifndef		__SERIAL_H
#define		__SERIAL_H


#include	<config.h>

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位



#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
//sfr P_SW1   = 0xA2;             //外设功能切换寄存器1

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
//sfr P_SW2   = 0xBA;             //外设功能切换寄存器2
#define S2_S0 0x01              //P_SW2.0
void SerialInit(void);
	//定时2作为波特率发生器，默认8位数据，
//串口1(P3.0/RxD, P3.1/TxD)，串口2S2_S0=1 (P1.0/RxD2_2, P1.1/TxD2_2)
	//中断方式

void S1SendData(unsigned char dat);
void S1SendString(unsigned char *s);
void S2SendData(unsigned char dat);
void S2SendString(unsigned char *s);
#endif
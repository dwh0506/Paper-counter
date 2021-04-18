#ifndef		__SERIAL_H
#define		__SERIAL_H


#include	<config.h>

#define NONE_PARITY     0       //��У��
#define ODD_PARITY      1       //��У��
#define EVEN_PARITY     2       //żУ��
#define MARK_PARITY     3       //���У��
#define SPACE_PARITY    4       //�հ�У��

#define PARITYBIT NONE_PARITY   //����У��λ



#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
//sfr P_SW1   = 0xA2;             //���蹦���л��Ĵ���1

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
//sfr P_SW2   = 0xBA;             //���蹦���л��Ĵ���2
#define S2_S0 0x01              //P_SW2.0
void SerialInit(void);
	//��ʱ2��Ϊ�����ʷ�������Ĭ��8λ���ݣ�
//����1(P3.0/RxD, P3.1/TxD)������2S2_S0=1 (P1.0/RxD2_2, P1.1/TxD2_2)
	//�жϷ�ʽ

void S1SendData(unsigned char dat);
void S1SendString(unsigned char *s);
void S2SendData(unsigned char dat);
void S2SendString(unsigned char *s);
#endif
#ifndef __MYSELF_H__
#define __MYSELF_H__
#include	"intrins.h"

void DelayNms(int n);//��ʱNms
void Delay1ms()	;	//@11.0592MHz


void Timer0Init(void);		//1����@11.0592MHz//��ʱ��0��ʼ��
bit KeyPress(bit KeyIO);//	����������������������ɺ���
unsigned char JudgeKeyNum(unsigned char ADCValue);//����ADCֵ�ж�5�򰴼���ֵ
unsigned char* Hex2ASCII(unsigned long long28Value);//8λֵת��ΪASCII
unsigned char* Hex2ASCII3Figure(unsigned int cNum);//3λʮ����ֵת��ΪASCII
unsigned int JudgePaperNum(unsigned long longFDC2214);//���ݶ����FDC2214ֵ����FDC2214ThresholdValue[170]ֵ���ж�ֽ������
unsigned char JudgeKeypress(void);//�жϰ���
unsigned char* Num2Speaker(unsigned int cNum);//��������������ת���ɷ��͵Ĵ��ڵ���Ƶ�ַ���

#endif
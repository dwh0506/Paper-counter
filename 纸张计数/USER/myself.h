#ifndef __MYSELF_H__
#define __MYSELF_H__
#include	"intrins.h"

void DelayNms(int n);//延时Nms
void Delay1ms()	;	//@11.0592MHz


void Timer0Init(void);		//1毫秒@11.0592MHz//定时器0初始化
bit KeyPress(bit KeyIO);//	将单个按键，按单次整理成函数
unsigned char JudgeKeyNum(unsigned char ADCValue);//根据ADC值判断5向按键键值
unsigned char* Hex2ASCII(unsigned long long28Value);//8位值转化为ASCII
unsigned char* Hex2ASCII3Figure(unsigned int cNum);//3位十进制值转化为ASCII
unsigned int JudgePaperNum(unsigned long longFDC2214);//根据读入的FDC2214值，和FDC2214ThresholdValue[170]值，判断纸张张数
unsigned char JudgeKeypress(void);//判断按键
unsigned char* Num2Speaker(unsigned int cNum);//将测量出的数据转换成发送的串口的音频字符串

#endif
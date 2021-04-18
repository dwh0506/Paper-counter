#include	"config.h"
#include	"intrins.h"
#include	"Serial.h"//串口1(P3.0/RxD, P3.1/TxD)
#include <myself.h>
#include	"STC8EEPROM.h"
#include	"string.h"
#include	"AD.h"
bit bFlagTimer0=0,bFlag2ms=0,bFlag3s=0;
bit TestKeyLastTime=1;
unsigned char xdata cJudgeKey=0;//用于判断按键的变量
unsigned char xdata T0Counter2ms=0;
unsigned int xdata T0Counter3s=0;//3s计数器

unsigned char xdata KeyNum[3]={0,0,0};//存放连续3次判断的键值
unsigned char code KeyThreshold[7]={0x40,0x95,0xB6,0xC7,0xD1,0xEA,0xFF};
//	键名				电压值		测量AD值			键值区间
//0	KeyStartStop 0     		0x00		   	0x00-0x40
//1	KeyDown  		1/2Vcc		0x81			0x41-0x95	
//2	KeyMiddle  	2/3Vcc		0xAB-0xAC		0x96-0xB6
//3	KeyRight  	3/4Vcc		0xC0-0xC1		0xB7-0xC7
//4	KeyLeft  		4/5Vcc		0xCE-0xCD		0xC8-0xD1
//5	KeyUp    		5/6Vcc		0xD6			0xD2-0xEA
//6	NoneKey  			Vcc 		0xFF			0xEB-0xFF

void DelayNms(int n)//延时Nms//@11.0592MHz
{
	int i=0;
	for(i=0;i<n;i++) 
		Delay1ms();
}

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}
	

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
  ET0 = 1;	//使能定时器0中断

}

void Timer0Interrupt() interrupt 1 using 2 //定时器0的中断
{
 unsigned char xdata i=0;


	T0Counter2ms++;

	if(T0Counter2ms>=2)
	{	
		ADC_CONTR |= ADC_START;                          //启动ADC转换	
		bFlag2ms=1;
		T0Counter2ms=0;
	}
	if(bFlag3s==1) T0Counter3s++;
	
}


//单个按键识别，本程序未用到
//按键读下连续读8次（间隔2ms）,如全为按下cJudgeKey==0x00，记录到TestKeyLastTime中
//在下一个连续读8次，如全为抬起cJudgeKey==0xFF，且前一状态为按下，
//确认此时为按键有效按下，且有效抬起，进行相应的操作
bit KeyPress(bit KeyIO)
{
	cJudgeKey=(cJudgeKey<<1) | KeyIO; // 判断值左移一位 ，并将当前扫描值入最低
	if(cJudgeKey==0x00)//按键按下，状态稳定。KeyIO按下为0，抬起为1
	{
		TestKeyLastTime=KeyIO;//记录状态
	}
	else if((cJudgeKey==0xFF) && (TestKeyLastTime!=KeyIO))//按键从按下到抬起，稳定
	{
		TestKeyLastTime=KeyIO;
		
		return 1;
	}			
	else 
	{

	}
	return 0;
}



unsigned char JudgeKeyNum(unsigned char ADCValue)//根据ADC值判断5向按键键值
{

	static unsigned char xdata cFlag5D=0;//保存连接读入的键值
	static unsigned char xdata c5DkeyLastTime=0;//保存上次按键值
//	static unsigned char xdata cNumLastTime=6;//记录上次确认的键值	
	static unsigned char xdata cKeyPressNum;
	unsigned char c5DkeyNow=7;

	
//	unsigned char c5DkeyNow=7;
//	unsigned char i=0;
//	//判断当前读入ADC对应的按键值

//	思路与单个按键相似
	//根据AD值得到当前键值
	if(ADCValue<=KeyThreshold[0]) c5DkeyNow=0;
	else if (ADCValue>KeyThreshold[0] && ADCValue<=KeyThreshold[1]) c5DkeyNow=1;
	else if (ADCValue>KeyThreshold[1] && ADCValue<=KeyThreshold[2]) c5DkeyNow=2;
	else if (ADCValue>KeyThreshold[2] && ADCValue<=KeyThreshold[3]) c5DkeyNow=3;
	else if (ADCValue>KeyThreshold[3] && ADCValue<=KeyThreshold[4]) c5DkeyNow=4;
	else if (ADCValue>KeyThreshold[4] && ADCValue<=KeyThreshold[5]) c5DkeyNow=5;
	else  c5DkeyNow=6;

	//记录按键
	if(c5DkeyNow==6)//抬起，记录一次
		cFlag5D=(cFlag5D<<1)&0;//左移记录1次
	else if(c5DkeyNow==c5DkeyLastTime)//AD判断的键值与上次相同，
		cFlag5D=(cFlag5D<<1)|1;//左移记录1次
	else //特殊情况，本次非抬起，也与上次不同，基本不可能出现
		cFlag5D=(cFlag5D<<1)& 0;//左移记录1次
	
	c5DkeyLastTime=c5DkeyNow;//记录当前AD读的键值	
	
	//判断键值
	if(cFlag5D==0xFF)//连续8次读入一样
		cKeyPressNum=c5DkeyNow;	//记录当前键值
	
  if(cFlag5D==0x00 && cKeyPressNum !=6 )//按键有效抬起，且前一次为有效按键
	{
		c5DkeyNow=cKeyPressNum;	
		cKeyPressNum=0x06;
		return c5DkeyNow;
	}
	else
		return 0x06;	

}

unsigned char* Hex2ASCII(unsigned long long28Value)//8位值转化为ASCII
{
	unsigned char xdata cindexTempHex[8]={0,0,0,0,0,0,0,0};
	char xdata i=0;
	
	for(i=7;i>=0;i=i-2)//高位在前
	{
		cindexTempHex[i]=long28Value;//利用默认数据类型转换，char为8位，取出lont int 的低8位
		cindexTempHex[i-1]=cindexTempHex[i]>>4;//取出8位中高4位	
		cindexTempHex[i]=cindexTempHex[i]-(cindexTempHex[i-1]<<4);//取出8位中的低4位
		long28Value=long28Value>>8;//低8位处理完毕，右移			
	}
//	S1SendData(0xAA);		
	for(i=0;i<=7;i++)
	{

//		S1SendData(cindexTempHex[i]);		
		if(cindexTempHex[i]<=9) cindexTempHex[i]+=0x30;//小于9转成ASCII
		else cindexTempHex[i]=cindexTempHex[i]+55;//大于9的数转成ASCII		
//		S1SendData(cindexTempHex[i]);			
	}
	cindexTempHex[8]=0;//数组后加一个结束符
//	S1SendString(cindexTempHex);//
//	S1SendData(13);
//	S1SendData(10);	
	return cindexTempHex;
	
}

unsigned char* Hex2ASCII3Figure(unsigned int cNum)//3位十进制值转化为ASCII
{
	//unsigned char xdata * cindexTempHex=0;//此句有错（语法上没有问题）
	unsigned char xdata cindexTempHex[3]={0,0,0};
	char xdata i=0;
	
	cindexTempHex[0]=cNum /100;//利用数据类型自动转换为整型
	cindexTempHex[1]=(cNum  - cindexTempHex[0] * 100) /10;	
	cindexTempHex[2]= cNum % 10;
	for(i=0;i<=3;i++) cindexTempHex[i]+= 0x30;//转成ASCII
	

	cindexTempHex[3]=0;//数组后加一个结束符
//	S1SendString(cindexTempHex);//
//	S1SendData(13);
//	S1SendData(10);	
	return cindexTempHex;
	
}

//根据读入的FDC2214值，和FDC2214ThresholdValue[170]值，判断纸张张数
unsigned int JudgePaperNum(unsigned long longFDC2214Value)
{
	extern unsigned long xdata FDC2214ThresholdValue[170];//各数量纸张对应点的中点值作为阈值
	unsigned char cPaperNum=0,i=0;
	if(longFDC2214Value < FDC2214ThresholdValue[0])
		return(1);
	else
		for(i=1;i<169;i++)
		{
			if((longFDC2214Value >= FDC2214ThresholdValue[i-1]) && (longFDC2214Value < FDC2214ThresholdValue[i]) )
				return(i+1);//随着纸张增加，传感器值增加，"<"
		}	
	return(999);//显示999表示超出范围了
	
	
}


unsigned char* Num2Speaker(unsigned int cNum)//将测量出的数据转换成发送的串口的音频字符串
{
	unsigned char xdata ctemp100=0,ctemp10=0,ctemp1=0;
	unsigned char xdata str1[18]="B7:";
	unsigned char xdata i=3;
	unsigned char * pt=str1;
	unsigned char * str2='\0';
	
//		str1=str1+4;//B
//		* str1=0x37;str1++;//7
//		* str1=0x3A;str1++;//:
//		* str1=0x30;str1++;//0
//		* str1='\0';	
	if(cNum>169) 	return("A7:00021");//超出测量范围
	else if (cNum==100)  return("B7:011112");	//一百张
	else if (cNum==10)  return("B7:1012");	//十张	
	else
	{
		ctemp100=cNum/100;	
		ctemp10=(cNum/10-10*ctemp100);		
		ctemp1=cNum % 10;		
		if(ctemp100>0) // >100的百位数
		{
			str1[i++]=0x30; str1[i++]=ctemp100+0x30;//百位
			str1[i++]=0x31;	str1[i++]=0x31;//百
		}
		if(ctemp100>0 && (ctemp10==0) )// >100，中间为零
		{
			str1[i++]=0x31;	str1[i++]=0x33;//零,
		}
		if(ctemp10>1) // 十位，
		{
			str1[i++]=0x30;	str1[i++]=ctemp10+0x30;// N
			str1[i++]=0x31;	str1[i++]=0x30;// 十
		}
		else if(ctemp10==1)//十几张
		{
			str1[i++]=0x31;	str1[i++]=0x30;// 十			
		}
		if(ctemp1>0) // 个位，
		{
			str1[i++]=0x30;	str1[i++]=ctemp1+0x30;// N
		}		
		str1[i++]=0x31;	str1[i++]=0x32;str1[i]='\0';// 张	
		return(str1);
	}

	
}

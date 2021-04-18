#include	"config.h"
#include	"intrins.h"
#include	"Serial.h"//����1(P3.0/RxD, P3.1/TxD)
#include <myself.h>
#include	"STC8EEPROM.h"
#include	"string.h"
#include	"AD.h"
bit bFlagTimer0=0,bFlag2ms=0,bFlag3s=0;
bit TestKeyLastTime=1;
unsigned char xdata cJudgeKey=0;//�����жϰ����ı���
unsigned char xdata T0Counter2ms=0;
unsigned int xdata T0Counter3s=0;//3s������

unsigned char xdata KeyNum[3]={0,0,0};//�������3���жϵļ�ֵ
unsigned char code KeyThreshold[7]={0x40,0x95,0xB6,0xC7,0xD1,0xEA,0xFF};
//	����				��ѹֵ		����ADֵ			��ֵ����
//0	KeyStartStop 0     		0x00		   	0x00-0x40
//1	KeyDown  		1/2Vcc		0x81			0x41-0x95	
//2	KeyMiddle  	2/3Vcc		0xAB-0xAC		0x96-0xB6
//3	KeyRight  	3/4Vcc		0xC0-0xC1		0xB7-0xC7
//4	KeyLeft  		4/5Vcc		0xCE-0xCD		0xC8-0xD1
//5	KeyUp    		5/6Vcc		0xD6			0xD2-0xEA
//6	NoneKey  			Vcc 		0xFF			0xEB-0xFF

void DelayNms(int n)//��ʱNms//@11.0592MHz
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
	

void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
  ET0 = 1;	//ʹ�ܶ�ʱ��0�ж�

}

void Timer0Interrupt() interrupt 1 using 2 //��ʱ��0���ж�
{
 unsigned char xdata i=0;


	T0Counter2ms++;

	if(T0Counter2ms>=2)
	{	
		ADC_CONTR |= ADC_START;                          //����ADCת��	
		bFlag2ms=1;
		T0Counter2ms=0;
	}
	if(bFlag3s==1) T0Counter3s++;
	
}


//��������ʶ�𣬱�����δ�õ�
//��������������8�Σ����2ms��,��ȫΪ����cJudgeKey==0x00����¼��TestKeyLastTime��
//����һ��������8�Σ���ȫΪ̧��cJudgeKey==0xFF����ǰһ״̬Ϊ���£�
//ȷ�ϴ�ʱΪ������Ч���£�����Ч̧�𣬽�����Ӧ�Ĳ���
bit KeyPress(bit KeyIO)
{
	cJudgeKey=(cJudgeKey<<1) | KeyIO; // �ж�ֵ����һλ ��������ǰɨ��ֵ�����
	if(cJudgeKey==0x00)//�������£�״̬�ȶ���KeyIO����Ϊ0��̧��Ϊ1
	{
		TestKeyLastTime=KeyIO;//��¼״̬
	}
	else if((cJudgeKey==0xFF) && (TestKeyLastTime!=KeyIO))//�����Ӱ��µ�̧���ȶ�
	{
		TestKeyLastTime=KeyIO;
		
		return 1;
	}			
	else 
	{

	}
	return 0;
}



unsigned char JudgeKeyNum(unsigned char ADCValue)//����ADCֵ�ж�5�򰴼���ֵ
{

	static unsigned char xdata cFlag5D=0;//�������Ӷ���ļ�ֵ
	static unsigned char xdata c5DkeyLastTime=0;//�����ϴΰ���ֵ
//	static unsigned char xdata cNumLastTime=6;//��¼�ϴ�ȷ�ϵļ�ֵ	
	static unsigned char xdata cKeyPressNum;
	unsigned char c5DkeyNow=7;

	
//	unsigned char c5DkeyNow=7;
//	unsigned char i=0;
//	//�жϵ�ǰ����ADC��Ӧ�İ���ֵ

//	˼·�뵥����������
	//����ADֵ�õ���ǰ��ֵ
	if(ADCValue<=KeyThreshold[0]) c5DkeyNow=0;
	else if (ADCValue>KeyThreshold[0] && ADCValue<=KeyThreshold[1]) c5DkeyNow=1;
	else if (ADCValue>KeyThreshold[1] && ADCValue<=KeyThreshold[2]) c5DkeyNow=2;
	else if (ADCValue>KeyThreshold[2] && ADCValue<=KeyThreshold[3]) c5DkeyNow=3;
	else if (ADCValue>KeyThreshold[3] && ADCValue<=KeyThreshold[4]) c5DkeyNow=4;
	else if (ADCValue>KeyThreshold[4] && ADCValue<=KeyThreshold[5]) c5DkeyNow=5;
	else  c5DkeyNow=6;

	//��¼����
	if(c5DkeyNow==6)//̧�𣬼�¼һ��
		cFlag5D=(cFlag5D<<1)&0;//���Ƽ�¼1��
	else if(c5DkeyNow==c5DkeyLastTime)//AD�жϵļ�ֵ���ϴ���ͬ��
		cFlag5D=(cFlag5D<<1)|1;//���Ƽ�¼1��
	else //������������η�̧��Ҳ���ϴβ�ͬ�����������ܳ���
		cFlag5D=(cFlag5D<<1)& 0;//���Ƽ�¼1��
	
	c5DkeyLastTime=c5DkeyNow;//��¼��ǰAD���ļ�ֵ	
	
	//�жϼ�ֵ
	if(cFlag5D==0xFF)//����8�ζ���һ��
		cKeyPressNum=c5DkeyNow;	//��¼��ǰ��ֵ
	
  if(cFlag5D==0x00 && cKeyPressNum !=6 )//������Ч̧����ǰһ��Ϊ��Ч����
	{
		c5DkeyNow=cKeyPressNum;	
		cKeyPressNum=0x06;
		return c5DkeyNow;
	}
	else
		return 0x06;	

}

unsigned char* Hex2ASCII(unsigned long long28Value)//8λֵת��ΪASCII
{
	unsigned char xdata cindexTempHex[8]={0,0,0,0,0,0,0,0};
	char xdata i=0;
	
	for(i=7;i>=0;i=i-2)//��λ��ǰ
	{
		cindexTempHex[i]=long28Value;//����Ĭ����������ת����charΪ8λ��ȡ��lont int �ĵ�8λ
		cindexTempHex[i-1]=cindexTempHex[i]>>4;//ȡ��8λ�и�4λ	
		cindexTempHex[i]=cindexTempHex[i]-(cindexTempHex[i-1]<<4);//ȡ��8λ�еĵ�4λ
		long28Value=long28Value>>8;//��8λ������ϣ�����			
	}
//	S1SendData(0xAA);		
	for(i=0;i<=7;i++)
	{

//		S1SendData(cindexTempHex[i]);		
		if(cindexTempHex[i]<=9) cindexTempHex[i]+=0x30;//С��9ת��ASCII
		else cindexTempHex[i]=cindexTempHex[i]+55;//����9����ת��ASCII		
//		S1SendData(cindexTempHex[i]);			
	}
	cindexTempHex[8]=0;//������һ��������
//	S1SendString(cindexTempHex);//
//	S1SendData(13);
//	S1SendData(10);	
	return cindexTempHex;
	
}

unsigned char* Hex2ASCII3Figure(unsigned int cNum)//3λʮ����ֵת��ΪASCII
{
	//unsigned char xdata * cindexTempHex=0;//�˾��д��﷨��û�����⣩
	unsigned char xdata cindexTempHex[3]={0,0,0};
	char xdata i=0;
	
	cindexTempHex[0]=cNum /100;//�������������Զ�ת��Ϊ����
	cindexTempHex[1]=(cNum  - cindexTempHex[0] * 100) /10;	
	cindexTempHex[2]= cNum % 10;
	for(i=0;i<=3;i++) cindexTempHex[i]+= 0x30;//ת��ASCII
	

	cindexTempHex[3]=0;//������һ��������
//	S1SendString(cindexTempHex);//
//	S1SendData(13);
//	S1SendData(10);	
	return cindexTempHex;
	
}

//���ݶ����FDC2214ֵ����FDC2214ThresholdValue[170]ֵ���ж�ֽ������
unsigned int JudgePaperNum(unsigned long longFDC2214Value)
{
	extern unsigned long xdata FDC2214ThresholdValue[170];//������ֽ�Ŷ�Ӧ����е�ֵ��Ϊ��ֵ
	unsigned char cPaperNum=0,i=0;
	if(longFDC2214Value < FDC2214ThresholdValue[0])
		return(1);
	else
		for(i=1;i<169;i++)
		{
			if((longFDC2214Value >= FDC2214ThresholdValue[i-1]) && (longFDC2214Value < FDC2214ThresholdValue[i]) )
				return(i+1);//����ֽ�����ӣ�������ֵ���ӣ�"<"
		}	
	return(999);//��ʾ999��ʾ������Χ��
	
	
}


unsigned char* Num2Speaker(unsigned int cNum)//��������������ת���ɷ��͵Ĵ��ڵ���Ƶ�ַ���
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
	if(cNum>169) 	return("A7:00021");//����������Χ
	else if (cNum==100)  return("B7:011112");	//һ����
	else if (cNum==10)  return("B7:1012");	//ʮ��	
	else
	{
		ctemp100=cNum/100;	
		ctemp10=(cNum/10-10*ctemp100);		
		ctemp1=cNum % 10;		
		if(ctemp100>0) // >100�İ�λ��
		{
			str1[i++]=0x30; str1[i++]=ctemp100+0x30;//��λ
			str1[i++]=0x31;	str1[i++]=0x31;//��
		}
		if(ctemp100>0 && (ctemp10==0) )// >100���м�Ϊ��
		{
			str1[i++]=0x31;	str1[i++]=0x33;//��,
		}
		if(ctemp10>1) // ʮλ��
		{
			str1[i++]=0x30;	str1[i++]=ctemp10+0x30;// N
			str1[i++]=0x31;	str1[i++]=0x30;// ʮ
		}
		else if(ctemp10==1)//ʮ����
		{
			str1[i++]=0x31;	str1[i++]=0x30;// ʮ			
		}
		if(ctemp1>0) // ��λ��
		{
			str1[i++]=0x30;	str1[i++]=ctemp1+0x30;// N
		}		
		str1[i++]=0x31;	str1[i++]=0x32;str1[i]='\0';// ��	
		return(str1);
	}

	
}

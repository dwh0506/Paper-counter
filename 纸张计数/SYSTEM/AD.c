#include	<STC15F2K60S2.h>
#include	"AD.h"
#include <myself.h>
#include	"Serial.h"
bit bFlagAD=0;
unsigned char cADCResult=0;

bit bFlagReflashFDC2214=0;//������ʾ��ͨ��ֵ��ʾˢ��������AD�ж��У�
//ֻҪ�⵽�а�������ͨ������־ֹͣˢ����ʾFDC2214ͨ��ֵ
//���������ʾFDC2214��ͨ��ֵʱ����Ҫ��ʱ�䰴���Ҽ��л��˵�������

void ADCInit()
{

    P1ASF = 0x08;                   //����P13��ΪAD��
    ADC_RES = 0;                    //�������Ĵ���
    ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | 0x03;//��Ϊ����ٶ�
		//IE = IE | 0xa0 ;                      //ʹ��ADC�ж�
		
		EADC = 1;    //ʹ��ADC�ж�
		DelayNms(2);//ADC�ϵ粢��ʱ
		

		
}

void adc_isr() interrupt 5 using 3//AD �жϴ�����
{
	  extern unsigned char code KeyThreshold[7];
    ADC_CONTR &= ~ADC_FLAG;                         //���жϱ�־
    cADCResult = ADC_RES;                        //��ȡADC���
		bFlagAD=1;
//    ADC_CONTR |= ADC_START;                          //����ADת��  //��2ms��ʱ�ж��п���ADC
	

	  bFlagReflashFDC2214=(cADCResult< KeyThreshold[5])?0:1;//ˢ�±�־���������У�ֻ�а���̧�������ſ���ˢ�µ�1ҳ��ͨ������	
//	cADCResult< KeyThreshold[5]���а������µ����	
}


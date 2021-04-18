#include	<STC15F2K60S2.h>
#include	"AD.h"
#include <myself.h>
#include	"Serial.h"
bit bFlagAD=0;
unsigned char cADCResult=0;

bit bFlagReflashFDC2214=0;//用于显示屏通道值显示刷新允许，在AD中断中，
//只要测到有按键，就通过本标志停止刷新显示FDC2214通道值
//解决了在显示FDC2214各通道值时，需要长时间按向右键切换菜单的问题

void ADCInit()
{

    P1ASF = 0x08;                   //设置P13口为AD口
    ADC_RES = 0;                    //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDHH | ADC_START | 0x03;//改为最高速度
		//IE = IE | 0xa0 ;                      //使能ADC中断
		
		EADC = 1;    //使能ADC中断
		DelayNms(2);//ADC上电并延时
		

		
}

void adc_isr() interrupt 5 using 3//AD 中断处理函数
{
	  extern unsigned char code KeyThreshold[7];
    ADC_CONTR &= ~ADC_FLAG;                         //清中断标志
    cADCResult = ADC_RES;                        //读取ADC结果
		bFlagAD=1;
//    ADC_CONTR |= ADC_START;                          //继续AD转换  //在2ms定时中断中开启ADC
	

	  bFlagReflashFDC2214=(cADCResult< KeyThreshold[5])?0:1;//刷新标志，主程序中，只有按键抬起来，才可以刷新第1页各通道仁政	
//	cADCResult< KeyThreshold[5]是有按键按下的情况	
}


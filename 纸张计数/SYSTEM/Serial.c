#include	"Serial.h"

bit bS1TrBusy=0;//串口1发送忙标志
bit bS2TrBusy=0;//串口2发送忙标志
bit bS1Rec=0;//串口1接收到数据标志
bit bS2Rec=0;//串口2接收到数据标志
unsigned char xdata cS1Rec=0;//串口1接收到字符
unsigned char xdata cS2Rec=0;//串口2接收到字符





void SerialInit(void)
{	//定时2作为波特率发生器，默认8位数据，
 //串口1(P3.0/RxD, P3.1/TxD)，串口2S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
	//中断方式
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
		// P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)

		S2CON = 0x50;               //8位可变波特率

		SCON = 0x50;                //8位可变波特率

	  AUXR |= 0x01;		//串口1选择定时器2为波特率发生器	
		AUXR |= 0x04;		//定时器2时钟为MAIN_Fosc,即1T
	
    T2L = (65536 - (MAIN_Fosc/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (MAIN_Fosc/4/BAUD))>>8;


		AUXR |= 0x10;		//启动定时器2
		
		
		IE2 = 0x01;     //使能串口2中断
		ES=1;           //使能串口1中断
    EA = 1;
	
}


void Uart1() interrupt 4 using 1
{

	if(RI)
	{
		RI=0;				//清除RI位
		cS1Rec = SBUF; //将数据读出
		bS1Rec=1;//置接收到数据标志位

		
	}
	if(TI)
	{
//		P55=0;
		TI=0;					//清除TI位
		bS1TrBusy=0; //清忙标志
	}
	

/*----------------------------
UART 中断服务程序
-----------------------------*/	
}
void Uart2() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
			cS2Rec = S2BUF; //将数据读出
			bS2Rec=1;//置接收到数据标志位
      S2CON &= ~S2RI;         //清除S2RI位
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除S2TI位
        bS2TrBusy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void S1SendData(unsigned char dat)
{
    while (bS1TrBusy);//等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)

    bS1TrBusy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void S1SendString(unsigned char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        S1SendData(*s++);         //发送当前字符
    }
}

void S2SendData(unsigned char dat)
{
    while (bS2TrBusy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)

    bS2TrBusy = 1;
    S2BUF = ACC;                //写数据到UART2数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void S2SendString(unsigned char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        S2SendData(*s++);         //发送当前字符
    }
}



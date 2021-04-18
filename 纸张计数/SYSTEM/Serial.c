#include	"Serial.h"

bit bS1TrBusy=0;//����1����æ��־
bit bS2TrBusy=0;//����2����æ��־
bit bS1Rec=0;//����1���յ����ݱ�־
bit bS2Rec=0;//����2���յ����ݱ�־
unsigned char xdata cS1Rec=0;//����1���յ��ַ�
unsigned char xdata cS2Rec=0;//����2���յ��ַ�





void SerialInit(void)
{	//��ʱ2��Ϊ�����ʷ�������Ĭ��8λ���ݣ�
 //����1(P3.0/RxD, P3.1/TxD)������2S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)
	//�жϷ�ʽ
	  ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
		P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
		// P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)

		S2CON = 0x50;               //8λ�ɱ䲨����

		SCON = 0x50;                //8λ�ɱ䲨����

	  AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����	
		AUXR |= 0x04;		//��ʱ��2ʱ��ΪMAIN_Fosc,��1T
	
    T2L = (65536 - (MAIN_Fosc/4/BAUD));   //���ò�������װֵ
    T2H = (65536 - (MAIN_Fosc/4/BAUD))>>8;


		AUXR |= 0x10;		//������ʱ��2
		
		
		IE2 = 0x01;     //ʹ�ܴ���2�ж�
		ES=1;           //ʹ�ܴ���1�ж�
    EA = 1;
	
}


void Uart1() interrupt 4 using 1
{

	if(RI)
	{
		RI=0;				//���RIλ
		cS1Rec = SBUF; //�����ݶ���
		bS1Rec=1;//�ý��յ����ݱ�־λ

		
	}
	if(TI)
	{
//		P55=0;
		TI=0;					//���TIλ
		bS1TrBusy=0; //��æ��־
	}
	

/*----------------------------
UART �жϷ������
-----------------------------*/	
}
void Uart2() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
			cS2Rec = S2BUF; //�����ݶ���
			bS2Rec=1;//�ý��յ����ݱ�־λ
      S2CON &= ~S2RI;         //���S2RIλ
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���S2TIλ
        bS2TrBusy = 0;               //��æ��־
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void S1SendData(unsigned char dat)
{
    while (bS1TrBusy);//�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)

    bS1TrBusy = 1;
    SBUF = ACC;                 //д���ݵ�UART���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void S1SendString(unsigned char *s)
{
    while (*s)                  //����ַ���������־
    {
        S1SendData(*s++);         //���͵�ǰ�ַ�
    }
}

void S2SendData(unsigned char dat)
{
    while (bS2TrBusy);               //�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)

    bS2TrBusy = 1;
    S2BUF = ACC;                //д���ݵ�UART2���ݼĴ���
}

/*----------------------------
�����ַ���
----------------------------*/
void S2SendString(unsigned char *s)
{
    while (*s)                  //����ַ���������־
    {
        S2SendData(*s++);         //���͵�ǰ�ַ�
    }
}



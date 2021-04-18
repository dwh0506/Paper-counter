#include  "FDC2214.h"
#include	<STC15F2K60S2.h>
#include	"Serial.h"
#include	"intrins.h"
#include <myself.h>
sbit FDC2214SCL=P0^6;
sbit FDC2214SDA=P0^7;
unsigned long longFDC2214Data;
#define nop _nop_();

// ------------------------------------------------------------
// IO��ģ��I2Cͨ��
// ------------------------------------------------------------


/*****************�����Ƕ�IIC���ߵĲ����ӳ���***/
/*****************��������**********************/
void FDC2214IICStart(void)
{
	FDC2214SCL=0;                  //
	FDC2214SDA=1;	
	nop;nop;	
	
	FDC2214SCL=1;
	nop;nop;nop;nop;nop;nop;nop;//

	FDC2214SDA=0;
	nop;nop;nop;nop;nop;nop;nop;
	
	FDC2214SCL=0;

                  //
}

/*****************ֹͣIIC����****************/
void FDC2214IICStop(void)
{
	FDC2214SCL=0;                  //
	FDC2214SDA=0;	
	nop;nop;	
		
	FDC2214SCL=1;
	nop;nop;nop;nop;nop;nop;nop;//

	FDC2214SDA=1;
	nop;nop;nop;nop;nop;nop;nop;

	FDC2214SCL=0;
}


/***************��IIC���߲���Ӧ��*******************/
void FDC2214IICACK(void)
{
	FDC2214SCL=0;                  
	FDC2214SDA=0;//
	nop;nop;

	FDC2214SCL=1;
	nop;nop;nop;nop;nop;nop;nop;

	FDC2214SCL=0;

}

/**************���Ӧ��λ*******************/
//�ȴ�Ӧ���źŵ���
//����ֵ: 0,����Ӧ��ʧ��
//        1,����Ӧ��ɹ�
bit FDC2214IICReadAck(void)
{
	unsigned char ucErrTime=0;

	FDC2214SCL=0;                  
	FDC2214SDA=1;//��IO���øߵ�ƽ	
	nop;nop;
	nop;nop;	
	FDC2214SCL=1;
	nop;nop;nop;nop;

	while(FDC2214SDA==1)
	{
		ucErrTime++;
		if(ucErrTime>100)
		{

			FDC2214IICStop( );
			return(0);//δ���յ�Ӧ��
		}
	}
	nop;nop;	

	FDC2214SCL=0;
	
	//����nop���ӣ���������ͨѶ	
	nop;nop;nop;nop;
	nop;nop;
	return(1);
}

/*****************����IIC���߲���Ӧ��***************/
void FDC2214IICNoAck(void)
{
	
	FDC2214SCL=0;                  
	FDC2214SDA=1;//
	nop;nop;
	nop;nop;	
	FDC2214SCL=1;
	nop;nop;nop;nop;nop;nop;nop;

	FDC2214SCL=0;

}

/*******************��IIC����д����*********************/
void FDC2214IICSendByte(unsigned char sendbyte)
{
	unsigned char data j=8;

	for(;j>0;j--)
	{
		 FDC2214SCL=0;
		 sendbyte<<=1;        //����C51����ʵ�����������ʼ�ջ�ʹCY=sendbyte^7;
		 FDC2214SDA=CY;
		 nop;nop;
		 nop;nop;			 
		 FDC2214SCL=1;
		 nop;nop;nop;nop;nop;nop;nop;	
			 
	}
	FDC2214SCL=0;
 
}

/**********************��IIC�����϶������ӳ���**********/
unsigned char FDC2214IICReadByte(void)
{
	unsigned char cReceiveData=0,i=8;
	FDC2214SCL=0;
	FDC2214SDA=1;//��IO���øߵ�ƽ	
	nop;nop;
	while(i--)
	   {
			 FDC2214SCL=1;
			 nop;nop;nop;nop;
		 
			 cReceiveData=(cReceiveData<<1)|FDC2214SDA;
			 FDC2214SCL=0;
			 nop;nop;nop;nop;nop;nop
			 nop;nop;nop;nop;nop;nop
			 
	   }
	return(cReceiveData);
}


//��������ַΪ��RegisterAddress��2���ֽ�
unsigned int FDC2214Read2Bytes(unsigned char RegisterAddress)//
{
	unsigned int RegisterData;//��Ŷ�����2���ֽ�
  FDC2214IICStart(); //��ʼ�ź�
	nop;nop;nop;nop;nop;nop	
	FDC2214IICSendByte((FDC2214_ADDR<<1)|0);//����������ַ+д����
	if(FDC2214IICReadAck()==1)		//�ȴ�Ӧ��
	{		

		FDC2214IICSendByte(RegisterAddress);	//д�Ĵ�����ַ
		if(FDC2214IICReadAck()==0) return(0);		//�ȴ�Ӧ��
		
		FDC2214IICStart();

		FDC2214IICSendByte((FDC2214_ADDR<<1)|1);//����������ַ+������
		if(FDC2214IICReadAck()==0) return(0);		//�ȴ�Ӧ��

		RegisterData=FDC2214IICReadByte()<<8;//��ȡ����
		FDC2214IICAck();	//����ACK
		
		RegisterData|=FDC2214IICReadByte();  //��ȡ����
		FDC2214IICNoAck();	//����nACK
		
		FDC2214IICStop();			//����һ��ֹͣ����
		return RegisterData;		
	}
	else//��Ӧ��������LED����
	{
//		TestLed=0;
		return 0;	
		
	}
}

///////////////////FDC2214���ú���////////////////////
/* *IICд2���ֽ�
 *RegisterAddress:�Ĵ�����ַ
 *data1:����1
 *data2:����2
 *����ֵ:1      ����
 *       0  ����
*/
bit FDC2214SetParameters(unsigned char RegisterAddress,unsigned char ParaMSB,unsigned char ParaLSB)
{ 
  FDC2214IICStart(); //��ʼ�ź�
	FDC2214IICSendByte((FDC2214_ADDR<<1)|0);//����������ַ+д����	
	if(FDC2214IICReadAck()==1)		//�ȴ�Ӧ��
	{		
		FDC2214IICSendByte(RegisterAddress);	//д�Ĵ�����ַ
		if(FDC2214IICReadAck()==0) return(0);		//�ȴ�Ӧ��

		FDC2214IICSendByte(ParaMSB);//���͸�λ����
		if(FDC2214IICReadAck()==0) return(0);		//�ȴ�Ӧ��

		FDC2214IICSendByte(ParaLSB);//���͵�λ����
		if(FDC2214IICReadAck()==0) return(0);		//�ȴ�Ӧ��

		FDC2214IICStop();			//����һ��ֹͣ����
		return (1);		//���ͳɹ�������1
	}
	else//��Ӧ��������LED����
	{
//		TestLed=0;
		return 0;	
		
	}	
}


bit FDC2214Init(void)
{

  unsigned int res;
	//���ID�Ƿ���ȷ���ٳ�ʼ���Ĵ���
	res=FDC2214Read2Bytes(MANUFACTURER_ID);
	if(res==0x5449)
	{

//		//����FDC2214�Ĵ���
		//����FDC2214SetParameters�Ĵ���
		FDC2214SetParameters(RCOUNT_CH0,0x34,0xFB);//�ο�����ת�����ʱ��(T=(RCOUNT_CH0*16)/Frefx)
		FDC2214SetParameters(RCOUNT_CH1,0x34,0xFB);
		FDC2214SetParameters(RCOUNT_CH2,0x34,0xFB);
		FDC2214SetParameters(RCOUNT_CH3,0x34,0xFB);
		
		FDC2214SetParameters(SETTLECOUNT_CH0,0x00,0x1B);//ת��֮ǰ���ȶ�ʱ��(T=(SETTLECOUNT_CHx*16)/Frefx)
		FDC2214SetParameters(SETTLECOUNT_CH1,0x00,0x1B);
		FDC2214SetParameters(SETTLECOUNT_CH2,0x00,0x1B);
		FDC2214SetParameters(SETTLECOUNT_CH3,0x00,0x1B);

		FDC2214SetParameters(CLOCK_DIVIDERS_C_CH0,0x20,0x02);//ѡ����0.01MHz ~ 10MHz�Ĵ�����Ƶ��
		FDC2214SetParameters(CLOCK_DIVIDERS_C_CH1,0x20,0x02);//Frefx = Fclk = 43.4MHz/2(2��Ƶ)
		FDC2214SetParameters(CLOCK_DIVIDERS_C_CH2,0x20,0x02);//CHx_REF_DIVIDER=2;CHx_FIN_SEL=2
		FDC2214SetParameters(CLOCK_DIVIDERS_C_CH3,0x20,0x02);
		
		FDC2214SetParameters(DRIVE_CURRENT_CH0,0x78,0x00);//0.146ma(����������ʱ��+ת��ʱ�����������)
		FDC2214SetParameters(DRIVE_CURRENT_CH1,0x78,0x00);
		FDC2214SetParameters(DRIVE_CURRENT_CH2,0x78,0x00);
		FDC2214SetParameters(DRIVE_CURRENT_CH3,0x78,0x00);
		
		FDC2214SetParameters(ERROR_CONFIG,0x00,0x00);//ȫ����ֹ���󱨸�
		//FDC2214SetParameters(MUX_CONFIG,0x52,0x0D);
		FDC2214SetParameters(MUX_CONFIG,0xC2,0x0D);//ͨ��0,1,2 ,3;ѡ��10MhzΪ�����𵴲���Ƶ�ʵ�������ã���ͨ������ͨ��
		
		FDC2214SetParameters(CONFIG,0x14,0x01);//����ģʽ��ʹ���ڲ��������ο�Ƶ�ʣ�INTB���Ż���״̬�Ĵ������±���λ
//		FDC2214SetParameters(CONFIG,0x15,0x01);//����ģʽ��ʹ�����40M��Դ�������ο�Ƶ��

	}
	else 
	{
		return 0;
	}
	return 1;

}

unsigned long FCD2214ReadCH(unsigned char index)
{
	unsigned long xdata result;
	switch(index)
	{
		case 0:
		  result = FDC2214Read2Bytes(DATA_CH0)&0x0FFF;
		  result = (result<<16)|(FDC2214Read2Bytes(DATA_LSB_CH0));
			break;
		case 1:
			result = FDC2214Read2Bytes(DATA_CH1)&0x0FFF;
		  result = (result<<16)|(FDC2214Read2Bytes(DATA_LSB_CH1));
		break;
		case 2:
			result = FDC2214Read2Bytes(DATA_CH2)&0x0FFF;
		  result = (result<<16)|(FDC2214Read2Bytes(DATA_LSB_CH2));
		break;
		case 3:
			result = FDC2214Read2Bytes(DATA_CH3)&0x0FFF;
		  result = (result<<16)|(FDC2214Read2Bytes(DATA_LSB_CH3));
		break;
		default:break;
	}
	result =result & 0x0FFFFFFF;
	return result;
}

unsigned long FilteredFCD2214ReadCH(unsigned char index)//�Զ�ȡֵ�������˲�
{
	unsigned char i=0;
	unsigned long xdata temp[6]=0,longSum=0,tempMax=0,tempMin=0;

	for(i=0;i<=5;i++)	
	{
		temp[i]=(FCD2214ReadCH(index) | 0x000003FF) & 0xFFFFFC00;//ȡ��18λ
		Delay1ms();		
	}
	longSum=temp[0];
	tempMax=temp[0];
	tempMin=temp[0];
	for(i=1;i<=5;i++)
	{
		if(tempMax<temp[i])tempMax=temp[i];//��¼��ֵ
		if(tempMin>temp[i])tempMin=temp[i];//��¼��Сֵ
		longSum=temp[i]+longSum;//�ۼ�
		
	}

	return((longSum-tempMax-tempMin)/4);//6��ֵ��ȥ�����ֵ��ȥ����Сֵ������4��ֵȡƽ��
}


float Cap_Calculate(unsigned char index)//�������ֵ
{

	longFDC2214Data = FCD2214ReadCH(index);

//	Cap = 56645.763f/((float)longFDC2214Data);
//	return ((Cap*Cap)-33);
    //Cap = 232021045.248/(longFDC2214Data);
	longFDC2214Data=longFDC2214Data/1000;
	return (longFDC2214Data);//return (longFDC2214Data);
}

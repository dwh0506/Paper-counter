//EEPROM��Ϊ1K��ǰ510�ֽڣ�0-0x1FD�����У׼�������,0x1FF��һ��־λ

#include	"config.h"
#include	"Serial.h"//����1(P3.0/RxD, P3.1/TxD)
#include <myself.h>
#include	"AD.h"
#include  "FDC2214.h"
#include "stdio.h"
#include "oled.h"
#include	"STC8EEPROM.h"
#define ShortCircuitValue 0x04000000//�������·���ж���ֵ
#define FDC2214ChannelNum 0//������ͨ���ţ�ֻ�õ�FDC2214��һ·ͨ��
#define BeepOnOff 1//���������أ�1Ϊ����������0Ϊ��

//longValueInROM�д�ų���У׼�㣬
//��һ�γ�ʼ��ʱ����longValueInROM���Ƶ�EEPROM��,����ֵ������FDC2214ThresholdValue
//�ǵ�һ�γ�ʼ��ʱ��EEPROM�е�У׼�㣬�����ֵ������FDC2214ThresholdValue
sbit Beep=P1^2;



void main()
{
	extern bit bS1Rec;//���ڽ��յ����ݱ�־
	extern bit bFlag3s;//3s��ʱ��־
	extern bit bFlag2ms;//2ms��־	
	extern bit bFlagAD;//ADת����־
	extern unsigned int xdata T0Counter3s;//3s������	
	extern unsigned char cS1Rec;//���ڽ��յ��ַ�
	extern unsigned char cADCResult;//AD��8λ	
	extern unsigned long xdata FDC2214ThresholdValue[];//������ֽ�Ŷ�Ӧ����е�ֵ��Ϊ��ֵ
	extern unsigned long xdata FDC2214CalibrateValue[];//���ڷ�У׼��ֵ
	extern unsigned char xdata cThePointOfCalibrate[];//У׼���־λ	
	extern bit bFlagReflashFDC2214;//���ڵ�1����ͨ��ֵ��ʾˢ������
	unsigned char xdata cKeyPressNumber=6;//�洢5�򰴼�ֵ

	unsigned long xdata FDC2214CH[4]={0,0,0,0};	//��Ŷ�ȡ��FDC2214ֵ��4ͨ��
	unsigned char xdata i=0,cCalibrateNum=1;//У׼��������ʾ������
	unsigned int xdata intTemp=0,cMeasureNum=255;	//������������ʾ������
	bit bFunctionIndex=0;//����ָʾ��־��0ΪУ׼��1Ϊ����
#include <STC15F2K60S2.H>
	bit bIndicatorIcon=0;	//����ͼ����ʽ��0Ϊ����Բ��1Ϊʵ��Բ�����Ǻ��ֱ�����18+bIndicatorIcon
	bit bShowPaperNum=0;//��ʾ����ֽ��������־
	bit bReCalibrate=0;//��У׼ѡ���а��¹��м��ȷ�ϣ�˵��У׼�����иı䣬��λΪ1
	bit bCalibrateValueNoSaved=0;//���ڵ�ǰУ׼���ݱ����־
	bit bSelectSaveItOrNot=0;//�뿪У׼ѡ��ʱ����ʾȷ���Ƿ�Ҫ�������ݽ���
	bit bSaveCalibrateData=0;//����У׼����
	bit bCalibrateDataReset=0;//У׼���ݻָ��������ñ�־

//Beep=1;
//while(1);	
	bFlag2ms=0;

	P1M1=P1M1& 0xFB; //����P1.2Ϊ����ʽ���
	P1M0=P1M1| 0x04;	
		


	SerialInit();//����1��2��ʼ��

	S2SendString("AF:28");//������ģ������	
	Timer0Init();//��ʱ��0��ʼ��
	ADCInit();

	FDC2214Init();
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear() ; 	
	EEPROMInit()	;//EEPROM��ʼ��

	


//**********�����û�����Ӻ�FDC2214***************
	intTemp=FDC2214Read2Bytes(MANUFACTURER_ID);
	if(intTemp!=0x5449)
		S2SendString("A7:00019");//�����Ӻô�����	
	while(intTemp!=0x5449)
	{
			OLED_ShowString(0,0,"Have no FDC2214!",16);
			OLED_ShowString(0,2,"Please connect",16);		
			OLED_ShowString(0,4,"it correctly!",16);			
			intTemp=FDC2214Read2Bytes(MANUFACTURER_ID);
	}
	
//************************************************
	OLED_Clear() ; 	
	S2SendString("A7:00014");//��У׼ֽ��
	
	OLED_PaperNumTitle();	
//	DelayNms(1000);	

  while(1)
	{		

			if(bFlagAD==1)//��ⰴ��//�ж��Ƿ��а�������
			{
				bFlagAD=0;
				cKeyPressNumber=JudgeKeyNum(cADCResult);//��ȡ����ֵ
//				S1SendData(cADCResult);              //����ֵ���͵�����					
			}	

			if(cKeyPressNumber<0x06)//�м����£��ж�Ϊ�ĸ������£������߼����ܱ�ʶ��־λ
			{
//				S1SendData(cKeyPressNumber);              //����ֵ���͵�����	
				
				switch(cKeyPressNumber) //���ݰ���ִ�� 
				{ 
					case 0:// KeyStartStop		
					{
						//��������3�룬�ָ�ԭʼУ׼����
						//Ҫ��3���ж�
						if(bFlag3s==0)bFlag3s=1;
						if(bFlag3s==1 && T0Counter3s>3000)
						{
							bCalibrateDataReset=1;//�ָ�ԭʼУ׼���ݱ�־
							bFlag3s=0;
							T0Counter3s=0;
						}
						//S1SendData(0x00);
						break;//���ڷ���	; 			
					}
					case 1://KeyDown
					{
						if(bFunctionIndex==0)//��У׼�����У������¼�
						{
							if(bReCalibrate==1)//���У׼�����ݣ�˵��У׼�����������Ի���Ҫ��ȷ���Ƿ񱣴�У׼���ݣ�
							{
							
								bSelectSaveItOrNot=1;//�Ƿ���ʾ����ѡ����־
						//		cCalibrateNum=1;//У׼���������ֹ�1
							
							}
							else//���û��У׼�����ݣ���ֱ������ǰ��У׼���ݲ���
							{
							
								bFunctionIndex=1;//����Ƶ�����ѡ��
							}
						}
					//	S1SendData(0x01);//���ڷ���	; 
						break;
					}
					case 2:// KeyMiddle 
					{
						if(BeepOnOff)
						{
							Beep=0;	
							DelayNms(50);
							Beep=1;	
						}
						if(bFunctionIndex==0)//��У׼������
						{
							bReCalibrate=1;//��ʶ�����ݱ�У׼��
							bCalibrateValueNoSaved=1;//��ʶ��ǰ����δ������
							S2SendString("A7:00020");	//��ȷ��2

						}
						else////�˵�ѡ��Ϊ��������
						{
							bShowPaperNum=1;
						}
						
						bIndicatorIcon=1;//����ͼ����ʽ��Ϊʵ��Բ�������У׼�У���ʾ��ǰ������У׼������ڲ����У���ʾ��ȷ����ʾ����
						//S1SendData(0x02);//���ڷ���	; 
						break;
					}
					case 3://	KeyRight
					{
						if(bFunctionIndex==0 & (cCalibrateNum<=170))//�˵�ѡ��ΪУ׼����
						{
							cCalibrateNum++;
						}
	//					S1SendData(0x03);//���ڷ���	; 
						bIndicatorIcon=0;//��ʾ����Բ����ʾ��ǰ���ݵ�δ��¼У׼ֵ					
						break;

					}
					case 4:// KeyLeft
					{
						if(bFunctionIndex==0 & cCalibrateNum>1)//�˵�ѡ��ΪУ׼����
						{
							cCalibrateNum--;
						}
	//					S1SendData(0x04);
						bIndicatorIcon=0;//��ʾ����Բ����ʾ��ǰ���ݵ�δ��¼У׼ֵ
						break;//���ڷ���	;  
						
					}
					case 5:// KeyUp
					{
						//S1SendData(0x05);//���ڷ���	; 
						if(bFunctionIndex==1)	//��ǰ�ڲ�������
						{
							S2SendString("A7:00017");	
							bFunctionIndex=0;
							bShowPaperNum=0;
						//	cCalibrateNum=1;//У׼���������ֹ�1
							bIndicatorIcon=0;//��ʾ����Բ����ʾ��ǰ���ݵ�δ��¼У׼ֵ							
						}
						else;//��У׼������
						break;
					}
					default:
					break;//
				}	
				cKeyPressNumber=0x06;//����ֵ������Ĭ�ϰ���̧��
			}


			
			//���ݰ���������ʾ
			if(bFlagReflashFDC2214)//û�а�������ʱ��ˢ����ʾ���м�����ʱ��OLED��ˢ�¡���������������OLEDˢ�ºͶ�ȡFDC2214ռ��CPUʱ�䣬������Ӧ������
			{
				
				if(bCalibrateDataReset==1)//�ָ�ԭʼУ׼���ݱ�־
				{
					bCalibrateDataReset=0;
					bIndicatorIcon=0;
					S2SendString("A7:00018");	//�ָ��������� 
					EEPROMReset();
				}
				if(bSelectSaveItOrNot==1)//��ʾ��������ѡ����棬��У׼ѡ�����뿪����Ҫ����У׼����
				{
					S2SendString("A7:00015");			
					OLED_ShowString(0,2,"                ",16);//����2,3,4����ʾ
					OLED_ShowString(0,4,"                ",16);//			
					OLED_ShowString(0,6,"                ",16);//		

					//���ñ���У׼���ݵĽ���
					OLED_ShowCHinese(0,2,21);	//��			
					OLED_ShowCHinese(16,2,22);//��
					OLED_ShowCHinese(32,2,23);//��	
					OLED_ShowCHinese(48,2,24);//��	
					OLED_ShowCHinese(64,2,3);	//��
					OLED_ShowCHinese(80,2,25);//��	
					OLED_ShowString(96,2,"?",16);//?
					OLED_ShowCHinese(16,6,21);//��						
					OLED_ShowCHinese(64,6,27);//��

//						
//					
					while(cKeyPressNumber!=0x02)//�ȴ�ȷ�ϱ���򲻱���
					{
						if(bFlagAD==1)
						{
							bFlagAD=0;
							cKeyPressNumber=JudgeKeyNum(cADCResult);//��ȡ����ֵ
					//S1SendData(cKeyPressNumber);              //����ֵ���͵�����	
						}	

						if(cKeyPressNumber==4)//�����
						{
								OLED_ShowCHinese(16,6,26);						
								OLED_ShowCHinese(64,6,22);
								bSaveCalibrateData=1;//����
						}
						else if(cKeyPressNumber==3)//���Ҽ�
						{
								OLED_ShowCHinese(16,6,21);						
								OLED_ShowCHinese(64,6,27);
								bSaveCalibrateData=0;//������
						}							
					}						

						
	
					if(bSaveCalibrateData==1)//ѡ�񱣴�
					{
//						cCalibrateNum=1;//��ʾ��У׼������0
						EEPROMSave();			//	��У׼���ݱ��浽EEPROM��һҳ��	
						bFunctionIndex=1;//����Ƶ�����ѡ��
						bReCalibrate=0;	//����У׼��־								
					}
					else
					{
						bFunctionIndex=0;//�������У׼ѡ��
					}
					
					bSelectSaveItOrNot=0;//������ʾ����Ի���					
					cKeyPressNumber=0x06;//����ֵ������Ĭ�ϰ���̧��					
				
					OLED_PaperNumTitle();//�ָ���ʾ������
					bIndicatorIcon=0;//��ʾ��Ȧ		
					
				}
				else//��ʾ����������	
				{
			
					FDC2214CH[FDC2214ChannelNum]=FCD2214ReadCH(FDC2214ChannelNum);
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>24);
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>16);						
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>8);	
////						S1SendData(FDC2214CH[FDC2214ChannelNum]);	
					if(FDC2214CH[FDC2214ChannelNum]>ShortCircuitValue)//FDC2214��·
					{
						if(BeepOnOff) Beep=0;	
						S2SendString("A7:00013");		//�����·					
						OLED_Clear();

						OLED_ShowCHinese(0,0,8);		//��·��ʾ		
						OLED_ShowCHinese(16,0,9);	

						while(FDC2214CH[FDC2214ChannelNum]>ShortCircuitValue)
						{
							
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++						
							
							FDC2214CH[FDC2214ChannelNum]=FilteredFCD2214ReadCH(FDC2214ChannelNum);
							//FDC2214CH[FDC2214ChannelNum]=FCD2214ReadCH(FDC2214ChannelNum);
							DelayNms(1);
							//S1SendData(0x01);
						}//����ͣ������ֱ����·���
						if(BeepOnOff) Beep=1;
						//S1SendData(0x02);
						OLED_PaperNumTitle();//������ʾԭ���Ľ���							
					}
					else//FDC2214���岻��·
					{
						OLED_ShowString(34,6,Hex2ASCII(FDC2214CH[FDC2214ChannelNum]),16);	//���һ����ʾ��ǰFDC2214�����ת����8λʮ��������ʾ

						if(bFunctionIndex==0 )//��У׼�����У�
						{
							
							if(bCalibrateValueNoSaved==1)//�������ݱ�У׼
							 {
									//S2SendString("A7:00017");	
									//+++++++++��ȡFDC2214ֵ����[cCalibrateNum-1]Ϊ�±꣬��������+++++++++++++			
									cThePointOfCalibrate[cCalibrateNum-1]=1;//�ı��־λ 
									FDC2214CalibrateValue[cCalibrateNum-1]=FilteredFCD2214ReadCH(FDC2214ChannelNum);//������뾭���˲���У׼ֵ								
									bCalibrateValueNoSaved=0;//��ǰ�����ѱ�����										 
							 }		
					 
							 OLED_ShowString(48,2,Hex2ASCII3Figure(cCalibrateNum),16);	//��ʾУ׼����							 
							 OLED_ShowCHinese(112,4,20);	//���ݵ�ǰ���ܱ�־������ԭ��ԲȦ���	
							 OLED_ShowCHinese(112,2,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾʵԲȦ��Ϊ0��ʾ��ԲȦ
							 OLED_ShowString(48,4,"   ",16);//����������������ʾ������
							 
					 
							 
						}
						else//�ڲ��������У�
						{
							
							if(bShowPaperNum==1)//��ʾ��������
							{
								//++++++++++++++++
								//+++++++++++++++++
								//����FDC2214CH[1]�ж�������
								
										
								cMeasureNum=JudgePaperNum(FilteredFCD2214ReadCH(FDC2214ChannelNum));
								S2SendString(Num2Speaker(cMeasureNum));//����������
								
								bIndicatorIcon=1;							
								P55=0;
								OLED_ShowString(48,4,"   ",16);//������ʾ������
							  OLED_ShowCHinese(112,4,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾʵԲȦ��Ϊ0��ʾ��ԲȦ									
								DelayNms(500);
								//+++++++++++++++��ʾ��������������������ʾ++++++++++++++++++
								
								OLED_ShowString(48,4,Hex2ASCII3Figure(cMeasureNum),16);		//��ʾ��������	
  							P55=1;
								bIndicatorIcon=0;
								bShowPaperNum=0;
							}
							OLED_PaperNumTitle();//������ʾԭ���Ľ���									
							OLED_ShowCHinese(112,2,20);	//���ݵ�ǰ���ܱ�־������ԭ��ԲȦ���	
							OLED_ShowCHinese(112,4,18+(bIndicatorIcon?1:0));	//����bIndicatorIconֵ��Ϊ1��ʾʵԲȦ��Ϊ0��ʾ��ԲȦ																	
							OLED_ShowString(48,2,"   ",16);//����У׼��������ʾ������						

						}
					}

				}
			}
	}
}



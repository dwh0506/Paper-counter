//EEPROM设为1K，前510字节（0-0x1FD）存放校准后的数据,0x1FF放一标志位

#include	"config.h"
#include	"Serial.h"//串口1(P3.0/RxD, P3.1/TxD)
#include <myself.h>
#include	"AD.h"
#include  "FDC2214.h"
#include "stdio.h"
#include "oled.h"
#include	"STC8EEPROM.h"
#define ShortCircuitValue 0x04000000//两极板短路的判断阈值
#define FDC2214ChannelNum 0//采样的通道号，只用到FDC2214的一路通道
#define BeepOnOff 1//蜂鸣器开关，1为正常工作，0为关

//longValueInROM中存放出厂校准点，
//第一次初始化时，从longValueInROM复制到EEPROM中,并阈值，存入FDC2214ThresholdValue
//非第一次初始化时，EEPROM中的校准点，算出阈值，存入FDC2214ThresholdValue
sbit Beep=P1^2;



void main()
{
	extern bit bS1Rec;//串口接收到数据标志
	extern bit bFlag3s;//3s计时标志
	extern bit bFlag2ms;//2ms标志	
	extern bit bFlagAD;//AD转换标志
	extern unsigned int xdata T0Counter3s;//3s计数器	
	extern unsigned char cS1Rec;//串口接收到字符
	extern unsigned char cADCResult;//AD高8位	
	extern unsigned long xdata FDC2214ThresholdValue[];//各数量纸张对应点的中点值作为阈值
	extern unsigned long xdata FDC2214CalibrateValue[];//用于放校准的值
	extern unsigned char xdata cThePointOfCalibrate[];//校准点标志位	
	extern bit bFlagReflashFDC2214;//用于第1屏各通道值显示刷新允许
	unsigned char xdata cKeyPressNumber=6;//存储5向按键值

	unsigned long xdata FDC2214CH[4]={0,0,0,0};	//存放读取的FDC2214值，4通道
	unsigned char xdata i=0,cCalibrateNum=1;//校准功能中显示的张数
	unsigned int xdata intTemp=0,cMeasureNum=255;	//测量功能中显示的张数
	bit bFunctionIndex=0;//功能指示标志，0为校准，1为测试
#include <STC15F2K60S2.H>
	bit bIndicatorIcon=0;	//功能图标样式，0为空心圆，1为实心圆，就是汉字表索引18+bIndicatorIcon
	bit bShowPaperNum=0;//显示测试纸张数量标志
	bit bReCalibrate=0;//在校准选项中按下过中间键确认，说明校准数据有改变，此位为1
	bit bCalibrateValueNoSaved=0;//用于当前校准数据保存标志
	bit bSelectSaveItOrNot=0;//离开校准选项时，显示确认是否要保存数据界面
	bit bSaveCalibrateData=0;//保存校准数据
	bit bCalibrateDataReset=0;//校准数据恢复出厂设置标志

//Beep=1;
//while(1);	
	bFlag2ms=0;

	P1M1=P1M1& 0xFB; //设置P1.2为推挽式输出
	P1M0=P1M1| 0x04;	
		


	SerialInit();//串口1、2初始化

	S2SendString("AF:28");//调声音模块音量	
	Timer0Init();//定时器0初始化
	ADCInit();

	FDC2214Init();
	OLED_Init();			//初始化OLED  
	OLED_Clear() ; 	
	EEPROMInit()	;//EEPROM初始化

	


//**********检测有没有连接好FDC2214***************
	intTemp=FDC2214Read2Bytes(MANUFACTURER_ID);
	if(intTemp!=0x5449)
		S2SendString("A7:00019");//请连接好传感器	
	while(intTemp!=0x5449)
	{
			OLED_ShowString(0,0,"Have no FDC2214!",16);
			OLED_ShowString(0,2,"Please connect",16);		
			OLED_ShowString(0,4,"it correctly!",16);			
			intTemp=FDC2214Read2Bytes(MANUFACTURER_ID);
	}
	
//************************************************
	OLED_Clear() ; 	
	S2SendString("A7:00014");//请校准纸张
	
	OLED_PaperNumTitle();	
//	DelayNms(1000);	

  while(1)
	{		

			if(bFlagAD==1)//检测按键//判断是否有按键按下
			{
				bFlagAD=0;
				cKeyPressNumber=JudgeKeyNum(cADCResult);//读取按键值
//				S1SendData(cADCResult);              //按键值发送到串口					
			}	

			if(cKeyPressNumber<0x06)//有键按下，判断为哪个键按下，根据逻辑功能标识标志位
			{
//				S1SendData(cKeyPressNumber);              //按键值发送到串口	
				
				switch(cKeyPressNumber) //根据按键执行 
				{ 
					case 0:// KeyStartStop		
					{
						//按键超过3秒，恢复原始校准数据
						//要加3秒判断
						if(bFlag3s==0)bFlag3s=1;
						if(bFlag3s==1 && T0Counter3s>3000)
						{
							bCalibrateDataReset=1;//恢复原始校准数据标志
							bFlag3s=0;
							T0Counter3s=0;
						}
						//S1SendData(0x00);
						break;//串口发送	; 			
					}
					case 1://KeyDown
					{
						if(bFunctionIndex==0)//在校准功能中，按向下键
						{
							if(bReCalibrate==1)//如果校准过数据，说明校准结束，跳出对话框，要求确认是否保存校准数据，
							{
							
								bSelectSaveItOrNot=1;//是否显示保存选择框标志
						//		cCalibrateNum=1;//校准功能中数字归1
							
							}
							else//如果没有校准过数据，就直接用以前的校准数据测量
							{
							
								bFunctionIndex=1;//光标移到测量选项
							}
						}
					//	S1SendData(0x01);//串口发送	; 
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
						if(bFunctionIndex==0)//在校准功能中
						{
							bReCalibrate=1;//标识有数据被校准过
							bCalibrateValueNoSaved=1;//标识当前数据未被保存
							S2SendString("A7:00020");	//已确认2

						}
						else////菜单选择为计数操作
						{
							bShowPaperNum=1;
						}
						
						bIndicatorIcon=1;//功能图标样式改为实心圆。如果在校准中，表示当前数据已校准；如果在测量中，表示已确认显示张数
						//S1SendData(0x02);//串口发送	; 
						break;
					}
					case 3://	KeyRight
					{
						if(bFunctionIndex==0 & (cCalibrateNum<=170))//菜单选择为校准操作
						{
							cCalibrateNum++;
						}
	//					S1SendData(0x03);//串口发送	; 
						bIndicatorIcon=0;//显示空心圆，表示当前数据点未记录校准值					
						break;

					}
					case 4:// KeyLeft
					{
						if(bFunctionIndex==0 & cCalibrateNum>1)//菜单选择为校准操作
						{
							cCalibrateNum--;
						}
	//					S1SendData(0x04);
						bIndicatorIcon=0;//显示空心圆，表示当前数据点未记录校准值
						break;//串口发送	;  
						
					}
					case 5:// KeyUp
					{
						//S1SendData(0x05);//串口发送	; 
						if(bFunctionIndex==1)	//当前在测量功能
						{
							S2SendString("A7:00017");	
							bFunctionIndex=0;
							bShowPaperNum=0;
						//	cCalibrateNum=1;//校准功能中数字归1
							bIndicatorIcon=0;//显示空心圆，表示当前数据点未记录校准值							
						}
						else;//在校准功能中
						break;
					}
					default:
					break;//
				}	
				cKeyPressNumber=0x06;//读键值结束后即默认按键抬起
			}


			
			//根据按键处理，显示
			if(bFlagReflashFDC2214)//没有按键按下时，刷新显示；有键按下时，OLED不刷新。这样处理是由于OLED刷新和读取FDC2214占用CPU时间，按键响应不流畅
			{
				
				if(bCalibrateDataReset==1)//恢复原始校准数据标志
				{
					bCalibrateDataReset=0;
					bIndicatorIcon=0;
					S2SendString("A7:00018");	//恢复出厂数据 
					EEPROMReset();
				}
				if(bSelectSaveItOrNot==1)//显示保存数据选择界面，在校准选项中离开，需要保存校准数据
				{
					S2SendString("A7:00015");			
					OLED_ShowString(0,2,"                ",16);//擦除2,3,4行显示
					OLED_ShowString(0,4,"                ",16);//			
					OLED_ShowString(0,6,"                ",16);//		

					//调用保存校准数据的界面
					OLED_ShowCHinese(0,2,21);	//是			
					OLED_ShowCHinese(16,2,22);//否
					OLED_ShowCHinese(32,2,23);//保	
					OLED_ShowCHinese(48,2,24);//存	
					OLED_ShowCHinese(64,2,3);	//数
					OLED_ShowCHinese(80,2,25);//据	
					OLED_ShowString(96,2,"?",16);//?
					OLED_ShowCHinese(16,6,21);//是						
					OLED_ShowCHinese(64,6,27);//否

//						
//					
					while(cKeyPressNumber!=0x02)//等待确认保存或不保存
					{
						if(bFlagAD==1)
						{
							bFlagAD=0;
							cKeyPressNumber=JudgeKeyNum(cADCResult);//读取按键值
					//S1SendData(cKeyPressNumber);              //按键值发送到串口	
						}	

						if(cKeyPressNumber==4)//向左键
						{
								OLED_ShowCHinese(16,6,26);						
								OLED_ShowCHinese(64,6,22);
								bSaveCalibrateData=1;//保存
						}
						else if(cKeyPressNumber==3)//向右键
						{
								OLED_ShowCHinese(16,6,21);						
								OLED_ShowCHinese(64,6,27);
								bSaveCalibrateData=0;//不保存
						}							
					}						

						
	
					if(bSaveCalibrateData==1)//选择保存
					{
//						cCalibrateNum=1;//显示的校准张数归0
						EEPROMSave();			//	将校准数据保存到EEPROM第一页面	
						bFunctionIndex=1;//光标移到测量选项
						bReCalibrate=0;	//清重校准标志								
					}
					else
					{
						bFunctionIndex=0;//光标留在校准选项
					}
					
					bSelectSaveItOrNot=0;//不再显示保存对话框					
					cKeyPressNumber=0x06;//读键值结束后即默认按键抬起					
				
					OLED_PaperNumTitle();//恢复显示主界面
					bIndicatorIcon=0;//显示空圈		
					
				}
				else//显示正常主界面	
				{
			
					FDC2214CH[FDC2214ChannelNum]=FCD2214ReadCH(FDC2214ChannelNum);
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>24);
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>16);						
////						S1SendData(FDC2214CH[FDC2214ChannelNum]>>8);	
////						S1SendData(FDC2214CH[FDC2214ChannelNum]);	
					if(FDC2214CH[FDC2214ChannelNum]>ShortCircuitValue)//FDC2214短路
					{
						if(BeepOnOff) Beep=0;	
						S2SendString("A7:00013");		//极板短路					
						OLED_Clear();

						OLED_ShowCHinese(0,0,8);		//短路提示		
						OLED_ShowCHinese(16,0,9);	

						while(FDC2214CH[FDC2214ChannelNum]>ShortCircuitValue)
						{
							
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++						
							
							FDC2214CH[FDC2214ChannelNum]=FilteredFCD2214ReadCH(FDC2214ChannelNum);
							//FDC2214CH[FDC2214ChannelNum]=FCD2214ReadCH(FDC2214ChannelNum);
							DelayNms(1);
							//S1SendData(0x01);
						}//程序停在这里直到短路解除
						if(BeepOnOff) Beep=1;
						//S1SendData(0x02);
						OLED_PaperNumTitle();//重新显示原来的界面							
					}
					else//FDC2214极板不短路
					{
						OLED_ShowString(34,6,Hex2ASCII(FDC2214CH[FDC2214ChannelNum]),16);	//最后一行显示当前FDC2214结果，转换成8位十六进制显示

						if(bFunctionIndex==0 )//在校准功能中，
						{
							
							if(bCalibrateValueNoSaved==1)//且有数据被校准
							 {
									//S2SendString("A7:00017");	
									//+++++++++读取FDC2214值，以[cCalibrateNum-1]为下标，存入数组+++++++++++++			
									cThePointOfCalibrate[cCalibrateNum-1]=1;//改变标志位 
									FDC2214CalibrateValue[cCalibrateNum-1]=FilteredFCD2214ReadCH(FDC2214ChannelNum);//保存读入经过滤波的校准值								
									bCalibrateValueNoSaved=0;//当前数据已被保存										 
							 }		
					 
							 OLED_ShowString(48,2,Hex2ASCII3Figure(cCalibrateNum),16);	//显示校准张数							 
							 OLED_ShowCHinese(112,4,20);	//根据当前功能标志，擦除原来圆圈光标	
							 OLED_ShowCHinese(112,2,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示实圆圈，为0显示空圆圈
							 OLED_ShowString(48,4,"   ",16);//擦除测量功能上显示的张数
							 
					 
							 
						}
						else//在测量功能中，
						{
							
							if(bShowPaperNum==1)//显示测量张数
							{
								//++++++++++++++++
								//+++++++++++++++++
								//根据FDC2214CH[1]判断张数，
								
										
								cMeasureNum=JudgePaperNum(FilteredFCD2214ReadCH(FDC2214ChannelNum));
								S2SendString(Num2Speaker(cMeasureNum));//语音报张数
								
								bIndicatorIcon=1;							
								P55=0;
								OLED_ShowString(48,4,"   ",16);//擦除显示的张数
							  OLED_ShowCHinese(112,4,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示实圆圈，为0显示空圆圈									
								DelayNms(500);
								//+++++++++++++++显示计数张数，调用声音提示++++++++++++++++++
								
								OLED_ShowString(48,4,Hex2ASCII3Figure(cMeasureNum),16);		//显示测量张数	
  							P55=1;
								bIndicatorIcon=0;
								bShowPaperNum=0;
							}
							OLED_PaperNumTitle();//重新显示原来的界面									
							OLED_ShowCHinese(112,2,20);	//根据当前功能标志，擦除原来圆圈光标	
							OLED_ShowCHinese(112,4,18+(bIndicatorIcon?1:0));	//根据bIndicatorIcon值，为1显示实圆圈，为0显示空圆圈																	
							OLED_ShowString(48,2,"   ",16);//擦除校准功能上显示的张数						

						}
					}

				}
			}
	}
}



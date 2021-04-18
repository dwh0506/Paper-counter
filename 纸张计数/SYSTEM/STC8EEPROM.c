#include	"STC8EEPROM.h"
#include	"intrins.h"
#include	<STC15F2K60S2.h>
#include	"Serial.h"//串口1(P3.0/RxD, P3.1/TxD)
#include  "FDC2214.h"
#define MaxPaperNum 170//校准时最多170张，测量时169张
//unsigned int xdata cNumOfBeCalibrated=0;//校准的最大张数
unsigned char xdata cThePointOfCalibrate[170]//校准点标志位
	={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long xdata FDC2214ThresholdValue[MaxPaperNum]//各数量纸张对应点的中点值作为阈值
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
unsigned long xdata FDC2214CalibrateValue[MaxPaperNum]//用于放校准的值
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0};
	
	/*//unsigned long xdata FDC2214CalibrateValue[170]=//以下为测试用数据
//	{18082304,0,0,0,0,0,0,0,0,0,22296576,0,0,0,0,0,0,0,0,0,23644672,
//0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24941568,0,0,0,0,0,0,0,0,0,0,0,
//0,0,0,0,0,0,0,0,25485312,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//25798656,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26001408,0,0,
//0,0,0,0,0,0,0,26079232,0,0,0,0,26112000,0,0,0,0,26143744,
//0,0,0,0,0,0,0,0,0,26195968,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//0,0,0,26286080,0,0,0,0,26309632,0,0,0,0,26327040,0,0,0,
//0,26344448,0,0,0,26356736};//用于放校准的值
*/
unsigned long code longValueInROM[170]=  //存放在ROM中的阈值
{18164736,18887680,19426304,19871744,20256768,20636672,20975616,21270528,21553152,21801984,
22028288,22241280,22446080,22627328,22792192,22958080,23097344,23236608,23361536,23486464,
23588864,23698432,23803904,23884800,23964672,24062976,24129536,24206336,24273920,24342528,
24400896,24466432,24524800,24577024,24623104,24681472,24723712,24768512,24808448,24859648,
24893440,24932352,24972288,25003008,25040896,25071616,25106432,25134080,25163776,25189376,
25218048,25245696,25274368,25300992,25323776,25340928,25362432,25391104,25412608,25432064,
25455616,25472000,25496576,25512960,25526272,25549824,25565184,25583616,25597952,25614336,
25628672,25646080,25660416,25672704,25691136,25702400,25718784,25733120,25744384,25756672,
25768960,25783296,25796608,25809920,25821184,25830400,25839616,25849856,25862144,25874432,
25884672,25894912,25904128,25914368,25924864,25932800,25940992,25952256,25957376,25968640,
25977856,25986048,25993216,26000384,26008576,26017792,26024960,26031104,26040320,26046464,
26053632,26062848,26066944,26074112,26080256,26086648,26091520,26098688,26104832,26114048,
26120192,26124288,26131456,26136528,26141696,26148864,26153984,26161200,26165596,26169344,
26174464,26180608,26186752,26191872,26198016,26201088,26206208,26211400,26215424,26221568,
26228736,26231808,26237952,26241024,26244096,26248192,26254136,26257408,26262528,26265600,
26269720,26272768,26277888,26282708,26285056,26288076,26292320,26295696,26298368,26302464,
26305584,26310864,26314752,26317824,26319872,26324168,26330112,26333184,26336256,26342400};//采样后，改成校准用的数据


void IapIdle()
{
    IAP_CONTR = 0;                              //关闭IAP功能
    IAP_CMD = 0;                                //清除命令寄存器
    IAP_TRIG = 0;                               //清除触发寄存器
    IAP_ADDRH = 0x80;                           //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

unsigned char IapRead(unsigned int addr)
{
    char dat;

    IAP_CONTR = WT_24M;                         //使能IAP
    IAP_CMD = 1;                                //设置IAP读命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
    dat = IAP_DATA;                             //读IAP数据
    IapIdle();                                  //关闭IAP功能

    return dat;
}

void IapProgram(unsigned int addr, unsigned char dat)
{
    IAP_CONTR = WT_24M;                         //使能IAP
    IAP_CMD = 2;                                //设置IAP写命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_DATA = dat;                             //写IAP数据
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();
    IapIdle();                                  //关闭IAP功能
}

void IapErase(unsigned int addr)
{
    IAP_CONTR = WT_24M;                         //使能IAP
    IAP_CMD = 3;                                //设置IAP擦除命令
    IAP_ADDRL = addr;                           //设置IAP低地址
    IAP_ADDRH = addr >> 8;                      //设置IAP高地址
    IAP_TRIG = 0x5a;                            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                            //写触发命令(0xa5)
    _nop_();                                    //
    IapIdle();                                  //关闭IAP功能
}


//longValueInROM中存放出厂校准点，
//第一次初始化时，从longValueInROM复制到EEPROM中（存高24位）,并算出阈值，存入FDC2214ThresholdValue
//非第一次初始化时，EEPROM中的校准点，算出阈值，存入FDC2214ThresholdValue
void EEPROMInit(void)		//将校准数据从EEPROM中调入到XDATA内存中
{
	unsigned char xdata i=0,cTemp=0;
	unsigned char xdata cTemp0=0,cTemp1=0,cTemp2=0;
	unsigned long xdata longTemp0=0,longTemp1=0,longTemp2=0,longTemp3;//	
	//以第1扇区最后一个字节作为标志位
	if(IapRead(0x1FF)!=1)//下载程序后，第一次运行程序时，0x1FF地址EEPROM的值肯定不是1，正常应该是0xFF（下载程序会擦除不用的ROM区域）
	{
		IapErase(0);//擦除第1扇区
//		IapErase(0x200);//擦除第2扇区
		for(i=0;i<170;i++)//第一次运行程序，将出厂存储于EEPROM中第1个扇区，170个数据，每个3字节，一共510个字节，占第一个扇区
		{
	

			cTemp0=(longValueInROM[i]>>20);//取高24位
			cTemp1=(longValueInROM[i]>>12);
			cTemp2=(longValueInROM[i]>>4);

			
			IapProgram(0+i*3, cTemp0);//写入第一扇区
			IapProgram(1+i*3, cTemp1);
			IapProgram(2+i*3, cTemp2);
		}
		for(i=1;i<170;i++)//第一次运行程序，算出阈值，存入FDC2214ThresholdValue	
			FDC2214ThresholdValue[i-1]=(longValueInROM[i-1]+longValueInROM[i])/2;//只有前169个阈值数据有效		

		  IapProgram(0x1FF, 1);//改变标志位
	
	}
	else//不是第一次运行，则根据第一个扇区的校准值计算阈值存到FDC2214ThresholdValue[i]
	{
		longTemp1=IapRead(0);
		longTemp2=IapRead(1);
		longTemp3=IapRead(2);//读取第0组出厂校准值
		longTemp0=((((longTemp1<<8)+longTemp2)<<8)+longTemp3)<<4;	
			
		
		for(i=1;i<170;i++)
		{//将当前读出的值与上一值到中点作为判断阈值
			longTemp1=IapRead(0+i*3);
			longTemp2=IapRead(1+i*3) ;
			longTemp3=IapRead(2+i*3);//读取后一组出厂校准值
			longTemp1=((((longTemp1<<8)+longTemp2)<<8)+longTemp3)<<4;		
			
			FDC2214ThresholdValue[i-1]=(longTemp0+longTemp1)/2;//只有前169个数据有效	
			longTemp0=longTemp1;
		}
	}	
}
		
		

//根据已校准的点据FDC2214CalibrateValue[]中不为0的点（EEPROMInit()中FDC2214CalibrateValue[]已初始化为0），
//及原始出厂数据，拟合出所有新的校准点，存入EEPROM，并算出新的阈值，存入FDC2214ThresholdValue
void EEPROMSave(void)		//将校准数据从XDATA内存保存至EEPROM中
{//根据已校准的点（可能没有170个），及原始出厂数据，拟合出所有新的校准点，存入EEPROM，并算出新的阈值，存入FDC2214ThresholdValue
	unsigned char xdata i=0,j=0,cTemp0=0,cTemp1=0,cTemp2=0;
	float xdata fTemp1=0,fTemp2=0;
	unsigned long xdata longTemp0=0,longTemp1=0,longTemp2=0,longTemp3=0;
	cTemp1=0;
	for(i=1;i<170;i++)//校准时，至少测1张(即i=0)和另一个数量
	{
		if(FDC2214CalibrateValue[i]!=0)
		{//找出非0的校准点，根据此校准点和上一点校准点数值,及已有的测量曲线，推算出中间区域各点数值
			cTemp2=i;
			for(j=cTemp1+1;j<cTemp2;j++)
			{//INT(($B2-$B$1)*(H$21-H$1)/($B$21-$B$1)+H$1)
				fTemp1=(float)(FDC2214CalibrateValue[cTemp2]-FDC2214CalibrateValue[cTemp1]);
				fTemp2=(float)(longValueInROM[cTemp2]-longValueInROM[cTemp1]);
				fTemp1=fTemp1/fTemp2;
				fTemp2=(float)	(longValueInROM[j]-longValueInROM[cTemp1]);
				fTemp1=fTemp1*fTemp2;
				FDC2214CalibrateValue[j]= (unsigned long)fTemp1+FDC2214CalibrateValue[cTemp1];//
				
			}
			cTemp1=cTemp2;//向后递进
		}
	
	}	
		
		
	IapErase(0);//擦除第1扇区
	for(i=0;i<170;i++)//将校准数据存储于EEPROM中第1个扇区，170个数据，每个3字节，一共510个字节，占第一个扇区
	{
		
			cTemp0=(FDC2214CalibrateValue[i]>>20);//取高24位
			cTemp1=(FDC2214CalibrateValue[i]>>12);
			cTemp2=(FDC2214CalibrateValue[i]>>4);

			
			IapProgram(0+i*3, cTemp0);//写入第一扇区
			IapProgram(1+i*3, cTemp1);
			IapProgram(2+i*3, cTemp2);		

	}
	IapProgram(0x1FF,1);//改变标志位，重新启动时不覆盖校准数据	

	
	
	for(i=1;i<170;i++)//算出阈值，存入FDC2214ThresholdValue	
		FDC2214ThresholdValue[i-1]=(FDC2214CalibrateValue[i-1]+FDC2214CalibrateValue[i])/2;//只有前169个阈值数据有效		

	//处理完后，根据标志位，将线性拟合出的校准数据点清零，保留原始数据点
	for(i=0;i<170;i++)
	{
		if((cThePointOfCalibrate[i])==0)FDC2214CalibrateValue[i]=0;
	}		

}

void EEPROMReset(void)		//恢复出厂EEPROM数据
{
		IapErase(0x0);		
		IapErase(0x200);	
		IAP_CONTR=0xA8;//产生软件复位
}
#ifndef __STC8EEPROM_H__
#define __STC8EEPROM_H__



#define WT_30M          0x80
#define WT_24M          0x81
#define WT_20M          0x82
#define WT_12M          0x83
#define WT_6M           0x84
#define WT_3M           0x85
#define WT_2M           0x86
#define WT_1M           0x87

void IapIdle();//结束EEPROM操作
unsigned char IapRead(unsigned int addr);//读取
void IapProgram(unsigned int addr, unsigned char dat);//写入
void IapErase(unsigned int addr);//EERPOM擦除


void EEPROMInit(void);		//将校准数据从EEPROM中调入到XDATA内存中
void EEPROMSave(void);		//将校准数据从XDATA内存保存至EEPROM中
void EEPROMReset(void);		//恢复出厂EEPROM数据
#endif
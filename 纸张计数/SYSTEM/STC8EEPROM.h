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

void IapIdle();//����EEPROM����
unsigned char IapRead(unsigned int addr);//��ȡ
void IapProgram(unsigned int addr, unsigned char dat);//д��
void IapErase(unsigned int addr);//EERPOM����


void EEPROMInit(void);		//��У׼���ݴ�EEPROM�е��뵽XDATA�ڴ���
void EEPROMSave(void);		//��У׼���ݴ�XDATA�ڴ汣����EEPROM��
void EEPROMReset(void);		//�ָ�����EEPROM����
#endif
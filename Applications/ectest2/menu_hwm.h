/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  MENU_TEST.CPP
* Purpose : Test function menu
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _MENU_HWM_H
#define _MENU_HWM_H
#include "global.h"
#include "mbxapp.h"

#define tmlProHiLimit   70
#define tmlProLoLimit   50
#define testLoopCnt     0x7FFFFFF

#define flashMarkBank   0x00
#define flashMark       0x0C
#define hwWDRstEC       0x11

#define WDIRQNumBank    0x02
#define WDIRQNum        0x52

#define WDDelayTime     50  // *100ms

#define NMIVecOff       2

#define ACPIBank        0x01
#define LVDS1Off        0x50
#define LVDS2Off        0x52
#define OnOffBit        BIT(7)
#define LVDSLvlMax      10

//md UEFI no support INT start
// typedef void interrupt (*Func_ISR)(...);
// static const int	PIC1_base = 0x20;
// static const int 	PIC2_base = 0xa0;
// static const uint8_t PIC1_vector_offset=8;
// static const uint8_t PIC2_vector_offset = 0x70-8;
// static const int 	EOI = 0x20;
// static const int 	OCW1 = 1;
//md UEFI no support INT end
#define spdLowLimit     1000
#define spdHighLimit    1000
#define fanoffdelay     15
#define fanondelay      2

#define SMBEEPTestCode  0x78

void hwmMenu(int key);
void tmlPorThrottleMenu(int key);
void tmlPorShutdownMenu(int key);
//md UEFI no support INT:
//void wdIRQNMIMenu(int key);
void wdCldRstMenu(int key);
void wdWrmRstMenu(int key);
void wdWDPinMenu(int key);
void sFanTestMenu(int key);
void gpioTestMenu(int key);
void lvdsOnOffTestMenu(int key);
void lvdsLevelTestMenu(int key);
void smbRWMenu(int key);
#endif //_MENU_HWM_H

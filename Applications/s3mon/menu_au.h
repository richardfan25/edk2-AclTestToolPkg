/******************************************************************
* Copyright(c) , Advantech Corporation 2014
* Module Name:  MENU_AU.CPP
* Purpose : Auto Test function menu
* Data    : 2014/04/03
* Author  : kk.chien
********************************************************************/
#ifndef _MENU_AU_H
#define _MENU_AU_H
#include "global.h"
#include "mbxapp.h"

enum _auto_step
{
    _as_start,
    _as_fan,
    _as_gpio,
    _as_eeprom,
};

extern int autoFlags;
#define startFlag       BIT(0)
#define fileFlag        BIT(1)

#define fanPollTime     200
#define fanOffDelayCnt  75
#define fabFullDelayCnt 10

#define gpioPollTime    1000

#define eepromPollTime  1000

void autoMenu(int key);
#endif //_MENU_AU_H

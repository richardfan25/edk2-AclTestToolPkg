/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  menu_rd.h
* Purpose : RD menu
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _MENU_RD_H
#define _MENU_RD_H
#include "global.h"
#include "mbxapp.h"

#define stepLimit 5
#define hitempLimit 100
#define lowtempLimit 0
#define sfsm_inputfinsh BIT(0)
#define sfsm_initDone BIT(1)
#define sfsm_increase BIT(2)
#define sfsm_decrease BIT(3)
#define sfsm_finish BIT(4)

#define Nullitem 0xFFF

void rdMenu(int key);
void dynamictblMenu(int key);
void loaddefaultMenu(int key);
void tmlProMenu(int key);
void tmlSrcMenu(int key);
void smartfantblMenu(int key);
void smartfanSimMenu(int key);
#endif //_MENU_RD_H

/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  mbxapp.h
* Purpose : Mailbox object
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _MBXAPP_H_
#define _MBXAPP_H_
#include <stdio.h>

#include "global.h"

#include "dynatbl.h"

#include "mbx_dyna.h"
#include "mbx_ec.h"
#include "mbx_gpio.h"
#include "mbx_wdt.h"

typedef struct _stMboxApp
{
    int		(*init)(void);
    void 	(*exit)(void);

    //mailbox dev function
	stMbxDyna	*dyna;
	stMbxGpio	*gpio;
	stMbxWdt 	*wdt;
	stMbxEc 	*ec;
    //
} stMboxApp;

//mailbox global variable
extern stMboxApp mApp;

#endif //_MBXAPP_H_

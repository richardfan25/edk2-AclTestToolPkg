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

#include "dev/dev_fan.h"
#include "dev/dev_tml.h"
#include "dev/dev_tpro.h"
#include "dev/dynatbl.h"

#include "mbx/mbx_dyna.h"
#include "mbx/mbx_ec.h"
#include "mbx/mbx_eep.h"
#include "mbx/mbx_fan.h"
#include "mbx/mbx_gpio.h"
#include "mbx/mbx_led.h"
#include "mbx/mbx_nvcfg.h"
#include "mbx/mbx_pwm.h"
#include "mbx/mbx_smb.h"
#include "mbx/mbx_tml.h"
#include "mbx/mbx_tpro.h"
#include "mbx/mbx_wdt.h"

typedef struct _stMboxApp
{
    int		(*init)(void);
    void 	(*exit)(void);

    //mailbox dev function
	stMbxDyna	*dyna;
	stMbxTpro	*tpro;
	stMbxTml	*tml;
	stMbxFan	*fan;
	stMbxGpio	*gpio;
	stMbxPwm	*pwm;
	stMbxWdt 	*wdt;
	stMbxLed 	*led;
	stMbxEep 	*eep;
	stMbxEc 	*ec;
	stMbxSmb	*smb;
    stMbxNvCfg	*nvcfg;
    //
} stMboxApp;

//mailbox global variable
extern stMboxApp mApp;

#endif //_MBXAPP_H_

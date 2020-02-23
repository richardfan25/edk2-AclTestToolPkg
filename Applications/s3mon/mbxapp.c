/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  MAILBOX.CPP
* Purpose : Mailbox object
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include <stdlib.h>

#include "mbx/mbx.h"
#include "mbxapp.h"
/*==============================================================*/
int mapp_init(void);
void mapp_exit(void);

/*==============================================================*/
stMboxApp	mApp = {
		/* *init	*/	&mapp_init,
		/* *exit	*/	&mapp_exit,

		//mailbox dev function
		&mbx_dyna,
		&mbx_tpro,
		&mbx_tml,
		&mbx_fan,
		&mbx_gpio,
		&mbx_pwm,
		&mbx_wdt,
		&mbx_led,
		&mbx_eep,
		&mbx_ec,
		&mbx_smb,
		&mbx_nvcfg,
};

/*===========================================================
 * Name  : mapp_init
 * Pupose: clear variable
 * Input : NA
 * Output: NA
 *===========================================================*/
int mapp_init(void)
{

    // init all mailbox function
	if(mbox.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_ec.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_eep.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_fan.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_led.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_pwm.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_smb.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_tml.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_wdt.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_gpio.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_tpro.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_dyna.init() != _err_noerror) return _err_MBoxAccessFail;
	if(mbx_nvcfg.init() != _err_noerror) return _err_MBoxAccessFail;

    return _err_noerror;
}
/*===========================================================
 * Name  : mapp_exit
 * Pupose: clear variable
 * Input : NA
 * Output: NA
 *===========================================================*/
void mapp_exit(void)
{
	mbx_dyna.exit();
}

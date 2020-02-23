
/******************************************************************
* Copyright(c) , Advantech Corporation 2017
* Module Name:  dev_tpro
* Purpose : thermal protect
* Data    : 2017/05/26
* Author  : Duncan.Tasi
********************************************************************/
#include <string.h>
#include "dev_tpro.h"
#include "dev_tml.h"

stTProDev tprodev[TPRO_CH_MAX];

/*===========================================================
 * Name  : tpro_clearData
 * Pupose: init thermal protect device
 * Input : NA
 * Output: None
 *===========================================================*/
void tpro_clearData(uint8_t dev_no)
{
	if (dev_no >= TPRO_CH_MAX) return;

	tprodev[dev_no].src			= TMLType_Nodefine;
	tprodev[dev_no].type		= TPType_NoDefine;
	tprodev[dev_no].hi_limit	= 0;
	tprodev[dev_no].lo_limit	= 0;
}

/*===========================================================
 * Name  : tpro_updateData
 * Pupose: assign data
 * Input : rdata-input data, len-data length
 * Output: NA
 *===========================================================*/
void tpro_updateData(uint8_t dev_no, uint8_t *rdata, uint8_t len)
{
    uint8_t idx = 0;
	
    if (idx >= len)
		return;
    tprodev[dev_no].src = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tprodev[dev_no].type = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tprodev[dev_no].hi_limit = (char)rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tprodev[dev_no].lo_limit = (char)rdata[idx];
}
/*===========================================================
 * Name  : tpro_getTypeStr
 * Pupose: convert thermal protect type id to string
 * Input : NA
 * Output: NA
 *===========================================================*/
void tpro_getTypeStr(eTPROType type, char * data)
{
    data[0] = '\0';
	
    switch (type)
	{
    case TPType_shutDn:
        strcpy(data, "ShutDown");
		break;
    case TPType_throttle:
        strcpy(data, "Throttle");
		break;
    case TPType_pwrOff:
        strcpy(data, "Power_button");
		break;
    default:
        strcpy(data, "Unknown");
		break;
    }
}
/*===========================================================
 * Name  : toArray
 * Pupose: to array
 * Input : odata-output data, len-data length
 * Output: NA
 *===========================================================*/
void tpro_toArray(uint8_t dev_no, uint8_t *odata, uint8_t *len)
{
    *len = 4;
    odata[0] = tprodev[dev_no].src;
    odata[1] = tprodev[dev_no].type;
    odata[2] = tprodev[dev_no].hi_limit;
    odata[3] = tprodev[dev_no].lo_limit;
}



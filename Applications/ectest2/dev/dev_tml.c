/******************************************************************
* Copyright(c) , Advantech Corporation 2017
* Module Name:  dev_tml
* Purpose : thermal information handle
* Data    : 2017/05/26
* Author  : Duncan.Tasi
********************************************************************/
#include <string.h>
#include "dev_tml.h"

stTmlDev tmldev[TML_SRC_MAX];

/*===========================================================
 * Name  : tml_getTypeStr
 * Pupose: convert thermal source type id to string
 * Input : NA
 * Output: NA
 *===========================================================*/
void tml_getTypeStr(eTMLType type, char *data)
{
    data[0] = '\0';
    switch (type)
	{
    case TMLType_1L:
        strcpy(data, "SYS0_Temperature");
		break;
    case TMLType_1R:
        strcpy(data, "CPU0_Temperature");
		break;
    case TMLType_2L:
        strcpy(data, "SYS1_Temperature");
		break;
    case TMLType_2R:
        strcpy(data, "CPU1_Temperature");
		break;
    default:
        strcpy(data, "Unknown");
		break;
    }
}

/*===========================================================
 * Name  : tml_clearDevice
 * Pupose: init thermal device
 * Input : NA
 * Output: enum _fanControlMode
 *===========================================================*/
void tml_clearData(uint8_t dev_no)
{
	if (dev_no >= TML_SRC_MAX) return;

	tmldev[dev_no].smb_ch	= 0;
	tmldev[dev_no].addr		= 0;
	tmldev[dev_no].cmd		= 0;
	tmldev[dev_no].status	= 0;
	tmldev[dev_no].fan		= 0;
	tmldev[dev_no].temperature = 0;
	tmldev[dev_no].fan_id	= 0;
	tmldev[dev_no].smb_id	= 0;
	
	tmldev[dev_no].tempID	= 0;
}

/*===========================================================
 * Name  : tml_updateData
 * Pupose: constructor
 * Input : NA
 * Output: NA
 *===========================================================*/
void tml_updateData(uint8_t dev_no, uint8_t smbdid, uint8_t fanid, uint8_t *rdata, uint8_t len)
{
    uint8_t idx = 0;
	
	if (dev_no >= TML_SRC_MAX) return;

    tmldev[dev_no].fan_id = fanid;
    tmldev[dev_no].smb_id = smbdid;

    if (idx >= len)
		return;
    tmldev[dev_no].smb_ch = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].addr = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].cmd = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].status = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].fan = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].temperature = (char)rdata[idx];
}
/*===========================================================
 * Name  : tml_updateData2
 * Pupose: constructor
 * Input : NA
 * Output: NA
 *===========================================================*/
void tml_updateData2(uint8_t dev_no, uint8_t *rdata, uint8_t len)
{
    uint8_t idx = 0;
	
	if (dev_no >= TML_SRC_MAX) return;

    if (idx >= len)
		return;
    tmldev[dev_no].smb_ch = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].addr = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].cmd = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].status = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].tempID = rdata[idx];
	
    idx++;
    if (idx >= len)
		return;
    tmldev[dev_no].temperature = (char)rdata[idx];
}
/*===========================================================
 * Name  : tml_getType
 * Pupose: get thermal type
 * Input : NA
 * Output: NA
 *===========================================================*/
eTMLType tml_getType(uint8_t dev_no)
{
	if (dev_no >= TML_SRC_MAX) return TMLType_Nodefine;

    return (tmldev[dev_no].status >> 5);
}


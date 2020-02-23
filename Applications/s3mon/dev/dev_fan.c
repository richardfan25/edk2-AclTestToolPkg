/******************************************************************
* Copyright(c) , Advantech Corporation 2017
* Module Name:  dev_fan
* Purpose : fan control
* Data    : 2017/05/26
* Author  : Duncan.Tasi
********************************************************************/
#include "dynatbl.h"
#include "dev_fan.h"

stFanDev fandev[FAN_CH_MAX];

/*===========================================================
 * Name  : fan_clearData
 * Pupose: init fan device
 * Input : NA
 * Output: None
 *===========================================================*/
void fan_clearData(uint8_t dev_no)
{
	if (dev_no >= FAN_CH_MAX) return;

	fandev[dev_no].code			= 0;
	fandev[dev_no].status		= 0;
	fandev[dev_no].ctrlMode		= 0;
	fandev[dev_no].tempThresH	= 0;
	fandev[dev_no].tempThresL	= 0;
	fandev[dev_no].tempThresGap = 0;
	fandev[dev_no].PWM_Hi		= 0;
	fandev[dev_no].PWM_Lo		= 0;
	fandev[dev_no].Speed_Hi		= 0;
	fandev[dev_no].Speed_Lo		= 0;
	fandev[dev_no].debounce		= 0;
	fandev[dev_no].tempCtrlMode	= 0;
	fandev[dev_no].speedCtrlMode = 0;
	fandev[dev_no].fan_id		= DDevNo_Nodefine;
	fandev[dev_no].tacho_id		= DDevNo_Nodefine;
}

/*===========================================================
 * Name  : updatData
 * Pupose: assign data
 * Input : dev_no-fan device number, fanid-fan device id, tachoid-tacho device id, 
 *          rdata-input data, len-input data length 
 * Output: NA
 *===========================================================*/
void fan_updatData(uint8_t dev_no, uint8_t fanid, uint8_t tachoid, uint8_t *rdata, uint8_t len)
{
    uint8_t idx = 0;
	
	if (dev_no >= FAN_CH_MAX) return;

    fandev[dev_no].fan_id	= fanid;
    fandev[dev_no].tacho_id = tachoid;
	
    if (idx >= len) return;
    fandev[dev_no].code 	= rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].status 	= rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].ctrlMode = rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].tempThresH = rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].tempThresL = rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].tempThresGap = rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].PWM_Hi 	= rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].PWM_Lo 	= rdata[idx];
	
    idx++;
    idx++;
    if (idx >= len) return;
    fandev[dev_no].Speed_Hi = (uint16_t)rdata[idx - 1] << 8 | ((uint16_t)rdata[idx]);
	
    idx++;
    idx++;
    if (idx >= len) return;
    fandev[dev_no].Speed_Lo = (uint16_t)rdata[idx - 1] << 8 | ((uint16_t)rdata[idx]);
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].debounce = rdata[idx];
	
    idx++;
    if (idx >= len) return;
    fandev[dev_no].tempCtrlMode = rdata[idx];
	
    idx++;
    idx++;
    if (idx >= len) return;
    fandev[dev_no].speedCtrlMode = (uint16_t)rdata[idx - 1] << 8 | ((uint16_t)rdata[idx]);
}
/*===========================================================
 * Name  : toArray
 * Pupose: to array
 * Input : odata-output array data(16byte), len-output data length
 * Output: NA
 *===========================================================*/
void fan_toArray(uint8_t dev_no, uint8_t *odata, uint8_t *len)
{
	if (dev_no >= FAN_CH_MAX) return;

    *len = 16;
    odata[0] = fandev[dev_no].code;
    odata[1] = fandev[dev_no].status;
    odata[2] = fandev[dev_no].ctrlMode;
    odata[3] = fandev[dev_no].tempThresH;
    odata[4] = fandev[dev_no].tempThresL;
    odata[5] = fandev[dev_no].tempThresGap;
    odata[6] = fandev[dev_no].PWM_Hi;
    odata[7] = fandev[dev_no].PWM_Lo;
    odata[8] = (uint8_t)(fandev[dev_no].Speed_Hi >> 8 & 0xFF);
    odata[9] = (uint8_t)(fandev[dev_no].Speed_Hi & 0xFF);
    odata[10] = (uint8_t)(fandev[dev_no].Speed_Lo >> 8 & 0xFF);
    odata[11] = (uint8_t)(fandev[dev_no].Speed_Lo & 0xFF);
    odata[12] = fandev[dev_no].debounce;
    odata[13] = fandev[dev_no].tempCtrlMode;
    odata[14] = (uint8_t)(fandev[dev_no].speedCtrlMode >> 8 & 0xFF);
    odata[15] = (uint8_t)(fandev[dev_no].speedCtrlMode & 0xFF);
}
/*===========================================================
 * Name  : getCtrlMode
 * Pupose: get fan control mode
 * Input : NA
 * Output: enum _fanControlMode
 *===========================================================*/
uint8_t fan_getCtrlMode(uint8_t dev_no)
{
	if (dev_no >= FAN_CH_MAX) return FCtrl_FanEnd;

    return (fandev[dev_no].ctrlMode >> 6 & 0x03);
}
/*===========================================================
 * Name  : setCtrlMode
 * Pupose: set fan control mode
 * Input : mode-enum _fanControlMode
 * Output: NA
 *===========================================================*/
void fan_setCtrlMode(uint8_t dev_no, uint8_t mode)
{
	if (dev_no >= FAN_CH_MAX) return;

    if (mode > FCtrl_FanSmart) return;

    fandev[dev_no].ctrlMode = (fandev[dev_no].ctrlMode & 0x3F) | ((mode & 0x03) << 6);
}
////////////////////smartFan end//////////////////////////////

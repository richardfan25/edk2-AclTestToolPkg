/*==========================================================*/
/* The Device Function For TI LM95245 File					*/
/* Date: 2017.05.08											*/
/*==========================================================*/
#ifndef _NCT7802_H
#define _NCT7802_H

#include "hwm.h"

#define NCT7802_VENDOR_ID		0x50					// 0x50
#define NCT7802_CHIP_ID			0xC3					// 0xC3
#define NCT7802_DEV_ID			0x21					// 0x2n, n=1,2,3,...

#define NCT7802_MAXCH_CURR		0

#define NCT7802_RESOLUTION_VOLT	4						// NCT7802_RESOLUTION_VOLT / HMW_RESOLUTION_BASE = 0.004V
#define NCT7802_RESOLUTION_CURR	100
#define NCT7802_RESOLUTION_TEMP	1000
#define NCT7802_RESOLUTION_FAN	1000

enum{
	NCT7802_INDEX_VCC,
	NCT7802_INDEX_VCORE,
	NCT7802_INDEX_VSEN1,
	NCT7802_INDEX_VSEN2,
	NCT7802_INDEX_VSEN3,
	NCT7802_MAXCH_VOLT
};
enum{
	NCT7802_INDEX_LTEMP,
	NCT7802_MAXCH_TEMP
};
enum{
	NCT7802_INDEX_FAN1,
	NCT7802_INDEX_FAN2,
	NCT7802_INDEX_FAN3,
	NCT7802_MAXCH_FAN
};
/*------------------------------------*/
// SMBus Address
/*------------------------------------*/
#define NCT7802_DEF_ADDR			0x50				// Default SMBus Address 0x50~0x5E
														// It can change by write the register FANCTRL1~3


/*------------------------------------*/
// Register list
/*------------------------------------*/
// Bank0 Register
#define NCT7802_REG_BANK_SEL		0x00				// Bank Select
#define NCT7802_REG_MNTRTD1			0x01				// Remote Temp 1 High Byte
#define NCT7802_REG_MNTRTD2			0x02				// Remote Temp 2 High Byte
#define NCT7802_REG_MNTRTD3			0x03				// Remote Temp 3 High Byte
#define NCT7802_REG_MNTLTD			0x04				// Locate Temp High Byte
#define NCT7802_REG_MNTTD_LSB		0x05				// Temperature Low Byte
#define NCT7802_REG_MNTPECI0		0x06				// PECI0 Temp High Byte
#define NCT7802_REG_MNTPECI1		0x07				// PECI1 Temp High Byte
#define NCT7802_REG_MNTPECI_LSB		0x08				// PECI Temp Low Byte
#define NCT7802_REG_MNTVCC			0x09				// Readout of VCC High Byte
#define NCT7802_REG_MNTVCORE		0x0A				// Readout of VCORE High Byte
#define NCT7802_REG_MNTVSEN1		0x0C				// Readout of VSEN1 High Byte
#define NCT7802_REG_MNTVSEN2 		0x0D				// Readout of VSEN2 High Byte
#define NCT7802_REG_MNTVSEN3		0x0E				// Readout of VSEN3 High Byte
#define NCT7802_REG_MNTV_LSB		0x0F				// Readout of Voltage Low Byte
#define NCT7802_REG_MNTFAN1			0x10				// Readout of Fan Count 1 High Byte
#define NCT7802_REG_MNTFAN2			0x11				// Readout of Fan Count 2 High Byte
#define NCT7802_REG_MNTFAN3			0x12				// Readout of Fan Count 3 High Byte
#define NCT7802_REG_MNTFAN_LSB		0x13				// Readout of Fan Count Low Byte
#define NCT7802_REG_IMON_PERCENT	0x15				// IMON Percent, the voltage input is VCore (0.9V=100%)
#define NCT7802_REG_DIODE_STA		0x17				// Diode Fault Alert Status
#define NCT7802_REG_TEMP_STA_LOW	0x18				// Temperature Low Alert Status
#define NCT7802_REG_TEMP_STA_HIGH	0x19				// Temperature High Alert Status
#define NCT7802_REG_FAN_STA			0x1A				// Fan Alert Status
#define NCT7802_REG_TCRIT_STA		0x1B				// T-CRIT# Alert Status
#define NCT7802_REG_GPIO_STA		0x1C				// GPIO Alert Status
#define NCT7802_REG_TEMP_SMI_STA	0x1D				// Temperature SMI Status
#define NCT7802_REG_VOLT_SMI_STA	0x1E				// Voltage SMI Status
#define NCT7802_REG_FAN_SMI_STA		0x1F				// Fan SMI Status
#define NCT7802_REG_TCRIT_RT_STA	0x20				// T_CRIT# Real Time Status
#define NCT7802_REG_ADC_START		0x21				// ADC Start bit & ALERT# output Mark bit 
#define NCT7802_REG_MODE_SEL		0x22				// Mode Selection
#define NCT7802_REG_PECI_EN			0x23				// PECI Enadle 
#define NCT7802_REG_FAB_EN			0x24				// Fan Enadle
#define NCT7802_REG_VOLT_EN			0x25				// Voltage Monitor Enable
#define NCT7802_REG_CONV_RATE		0x26				// Conversion Rate
#define NCT7802_REG_FAULT_QUEUE		0x27				// Fault Queue

#define NCT7802_REG_VENDOR_ID		0xFD				// 0x50
#define NCT7802_REG_CHIP_ID			0xFE				// 0xC3
#define NCT7802_REG_DEV_ID			0xFF				// 0x2n, n=1,2,3,...

// Bank1 Register

EFI_STATUS hwm_nct7802_init(
	IN	SMB_BUS 		*SmbusHc,
	OUT	HWM_HANDLE_ST 	**Hwm
);

#endif //_NCT7802_H


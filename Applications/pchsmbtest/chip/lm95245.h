/*==========================================================*/
/* The Device Function For TI LM95245 File					*/
/* Date: 2017.05.08											*/
/*==========================================================*/
#ifndef __LM95245_H
#define __LM95245_H

/*------------------------------------*/
// SMBus Address
/*------------------------------------*/
#define LM_SMB_ADDR_NUM			5					// The count of LM supported address
// LM95245CIMM
#define LM_SMB_ADDR0			0x98				// A0 pin High Level
#define LM_SMB_ADDR1			0x30				// A0 pin Low Level
// LM95245CIMM-1
#define LM_SMB_ADDR2			0x9A				// A0 pin High Level
#define LM_SMB_ADDR3			0x32				// A0 pin Low Level
// LM95245CIMM/LM95245CIMM-1
#define LM_SMB_ADDR4			0x52				// A0 pin Mid-Supply

#define LM_MFU_ID				0x01
#define LM_IC_REV				0xB3				// initial Revision

/*------------------------------------*/
// Register list
/*------------------------------------*/
// Status Register
#define LM_REG_STATUS_1			0x02				// Status1(RO) - Busy bit, and status bits
#define LM_REG_STATUS_2			0x33				// Status2(RO) - Not Ready bit
// General Configuration Register
#define LM_REG_CFG_1			0x03				// Configuration Register 1(RW)
#define LM_REG_CONV_RATE		0x04				// Conversion Rate (RW)
#define LM_REG_ONE_SHOT			0x0F				// One-Shot(WO) - A write to this register activates one conversion if STOP/RUN bit = 1
// Diode Configuration Register
#define LM_REG_CFG_2			0xBF				// Configuration Register 2(RW)
#define LM_REG_REMOTE_OFFSET_H	0x11				// Remote Offset High byte
#define LM_REG_REMOTE_OFFSET_L	0x12				// Remote Offset Low byte
// Temperature Signed Value Register
#define LM_REG_TEMP_H			0x00				// Local Temperature High Byte(RO) 
#define LM_REG_TEMP_L			0x30				// Local Temperature Low Byte(RO) 
#define LM_REG_REMOTE_TEMP_H	0x01				// Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_TEMP_L	0x10				// Remote Temperature Low Byte(RO) 
// Temperature Unsigned Value Register
#define LM_REG_REMOTE_UTEMP_H	0x31				// Unsigned Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_UTEMP_L	0x32				// Unsigned Remote Temperature Low Byte(RO) 
// Limit Register
#define LM_REG_REMOTE_OS_LMT	0x07				// Remote OS Limit 
#define LM_REG_REMOTE_CRIT_LMT	0x19				// Remote T_Crit Limit 
#define LM_REG_T_CRIT_LMT		0x20				// Local Shared OS and T_Crit Limit 
#define LM_REG_COMM_HYSTERESIS	0x21				// Common Hysteresis
// Identification Register
#define LM_REG_MFU_ID			0xFE				// Manufacturer ID (RO) - Always returns 0x01
#define LM_REG_CHIP_REV			0xFF				// Revision ID (RO) - Returns revision number

/*------------------------------------*/
// Status Register Bit Define
/*------------------------------------*/
// Status 1
#define LM_STA1_BIT_BUSY		0x80				// When set to "1" the part is converting
#define LM_STA1_BIT_ROS			0x10				// Status bit for Remote OS
#define LM_STA1_BIT_DIODE		0x04				// Status bit for missing diode (Either D+ is shorted to GND, or VDD; or D+ is floating.)
#define LM_STA1_BIT_RTCRIT		0x02				// Status bit for Remote TCRIT.
#define LM_STA1_BIT_LOC			0x01				// Status bit for the shared Local OS and TCRIT
// Status 2
#define LM_STA2_BIT_NOTREADY	0x80				// Waiting for 30 ms power-up sequence to end

#endif //__LM95245_H


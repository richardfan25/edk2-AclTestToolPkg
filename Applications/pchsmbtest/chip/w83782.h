/*==========================================================*/
/* The Device Function For Winbond W83782 File				*/
/* Date: 2017.06.30											*/
/*==========================================================*/
#ifndef _W83782_H
#define _W83782_H

#include "hwm.h"

#define W83782_DEV_ID			0x02					// Device ID excludes the bit0. (it must be masked when detection)
#define W83782_VENDOR_ID		0x5CA3					// Vendor ID
#define W83782_CIP_ID			0x30					// Chip ID

#define W83782_MAXCH_VOLT		9                   	
#define W83782_MAXCH_CURR		0                   	
#define W83782_MAXCH_TEMP		3                   	
#define W83782_MAXCH_FAN		3                   	

#define W83782_RESOLUTION_VOLT	16						// W83782_RESOLUTION_VOLT / HMW_RESOLUTION_BASE = 0.016V
#define W83782_RESOLUTION_CURR	100                 	
#define W83782_RESOLUTION_TEMP	500                 	
#define W83782_RESOLUTION_FAN	1000                	

/*------------------------------------*/            	
// SMBus Address                                    	
/*------------------------------------*/            	
#define W83782_DEF_ADDR			0x50					// Default SMBus Address 0x50~0x5E
														// It can change by write the register 0x48 and H/W pin 45~47

#define W83782_TEMP_ADDR		0x90					// TEMP 2/3 SMBus Address 0x90~0x9E
														// It can change by write the register 0x4A

/*------------------------------------*/
// Common Register
/*------------------------------------*/
// Value Ram Register
#define W83782_REG_VCOREA			0x20				// Vcore A reading
#define W83782_REG_VINR0			0x21				// Vin R0 reading
#define W83782_REG_VIN3P3V			0x22				// Vin +3.3V reading
#define W83782_REG_VIN5V			0x23				// Vin +5V reading
#define W83782_REG_VIN12V			0x24				// Vin +12V reading
#define W83782_REG_VINN12V			0x25				// Vin -12V reading
#define W83782_REG_VINN5V			0x26				// Vin -5V reading
#define W83782_REG_TEMP				0x27				// Temperature reading
#define W83782_REG_FAN1				0x28				// Fan 1 reading
#define W83782_REG_FAN2				0x29				// Fan 2 reading
#define W83782_REG_FAN3				0x2A				// Fan 3 reading
#define W83782_REG_VCOREA_LMTH		0x2B				// Vcore A Limit High Byte
#define W83782_REG_VCOREA_LMTL		0x2C				// Vcore A Limit Low Byte
#define W83782_REG_VINR0_LMTH		0x2D				// Vin R0 Limit High Byte
#define W83782_REG_VINR0_LMTL		0x2E				// Vin R0 Limit Low Byte
#define W83782_REG_VIN3P3V_LMTH		0x2F				// Vin +3.3V Limit High Byte
#define W83782_REG_VIN3P3V_LMTL		0x30				// Vin +3.3V Limit Low Byte
#define W83782_REG_VIN5V_LMTH		0x31				// Vin +5V Limit High Byte
#define W83782_REG_VIN5V_LMTL		0x32				// Vin +5V Limit Low Byte
#define W83782_REG_VIN12V_LMTH		0x33				// Vin +12V Limit High Byte
#define W83782_REG_VIN12V_LMTL		0x34				// Vin +12V Limit Low Byte
#define W83782_REG_VINN12V_LMTH		0x35				// Vin -12V Limit High Byte
#define W83782_REG_VINN12V_LMTL		0x36				// Vin -12V Limit Low Byte
#define W83782_REG_VINN5V_LMTH		0x37				// Vin -5V Limit High Byte
#define W83782_REG_VINN5V_LMTL		0x38				// Vin -5V Limit Low Byte
#define W83782_REG_TEMP_LMTH		0x39				// Temperature Limit High Byte
#define W83782_REG_TEMP_LMTL		0x3A				// Temperature Limit Low Byte
#define W83782_REG_FAN1_LMT			0x3B				// Fan 1 Count Limit
#define W83782_REG_FAN2_LMT			0x3C				// Fan 2 Count Limit
#define W83782_REG_FAN3_LMT			0x3D				// Fan 3 Ciybt Limit
// Config or Status Register
#define W83782_REG_CONFIG			0x40				// Configuration (see W83782_ST_CONFIG for detail)
#define W83782_REG_INT_STS1			0x41				// Interrupt Status 1 (see W83782_ST_INT_STS1 for detail)
#define W83782_REG_INT_STS2			0x42				// Interrupt Status 1 (see W83782_ST_INT_STS2 for detail)
#define W83782_REG_SMI_MASK1		0x43				// SMI# Mask 1 (see W83782_ST_SMI_MASK1 for detail)
#define W83782_REG_SMI_MASK2		0x44				// SMI# Mask 2 (see W83782_ST_SMI_MASK2 for detail)
#define W83782_REG_CHASSIS			0x46				// Chassis Clear
#define W83782_REG_FAN12_DIVL		0x47				// bit[7:4] Fan 1 and 2 Divisor
#define W83782_REG_VOLT_ID0_3		0x47				// bit[3:0] Voltage ID 0~3
#define W83782_REG_SMB_ADDR			0x48				// Smbus Address
#define W83782_REG_DEV_ID			0x49				// bit[6:0] Device ID
#define W83782_REG_VOLT_ID4			0x49				// bit[7] Voltage ID 4
#define W83782_REG_TEMP_SMB_ADDR	0x4A				// Temperature 2&3 SMBus Address
#define W83782_REG_FAN3_DIVL		0x4B				// bit[7:4] Fan 1 and 2 Divisor
#define W83782_REG_PIN_CTRL			0x4B				// Pin Control
#define W83782_REG_PROPERTY_SEL		0x4C				// IRQ# / OVT# Property Select
#define W83782_REG_FAN_GP0_CTRL		0x4D				// Fan in/out and BEEP/GP0# control
#define W83782_REG_BANK_SEL			0x4E				// Bank Select for register 0x50~0x5F, bit[7] 1: VID high byte, 0: VID low byte
#define W83782_REG_VID				0x4F				// Vendor ID, High or Low select by REG_BANK_SEL.bit7

/*------------------------------------*/
// Bnak0 Register
/*------------------------------------*/
#define W83782_REG_BEEP_CTRL1		0x56				// Beep Control 1
#define W83782_REG_BEEP_CTRL2		0x57				// Beep Control 2
#define W83782_REG_CHIP_ID			0x58				// Chip ID
#define W83782_REG_DIODE_SEL		0x59				// Diode Selection
#define W83782_REG_PWM2_CTRL		0x5A				// PWMOUT2 Control
#define W83782_REG_PWM1_CTRL		0x5B				// PWMOUT1 Control
#define W83782_REG_PWM12_CLK		0x5C				// PWM1/2 Clock Select
#define W83782_REG_FAN123_DIVH		0x5D				// bit[7:4] Fan 1 and 2 Divisor
#define W83782_REG_VBAT_CTRL		0x5D				// Vbat monitor control
#define W83782_REG_PWM3_CTRL		0x5E				// PWMOUT3 Control
#define W83782_REG_PWM4_CTRL		0x5F				// PWMOUT4 Control

/*------------------------------------*/
// Bnak1 Register (access by temperature 2 address only)
/*------------------------------------*/
#define W83782_REG_TEMP2_H			0x50				// temperature 2 Reading High Byte
#define W83782_REG_TEMP2_L			0x51				// temperature 2 Reading Low Byte
#define W83782_REG_TEMP2_CFG		0x52				// temperature 2 Configuration
#define W83782_REG_TEMP2_HYSIS_H	0x53				// temperature 2 Hystersis High Byte
#define W83782_REG_TEMP2_HYSIS_L	0x54				// temperature 2 Hystersis Low Byte
#define W83782_REG_TEMP2_OVER_H		0x55				// temperature 2 Over temperature High Byte
#define W83782_REG_TEMP2_OVER_L		0x56				// temperature 2 Over temperature Low Byte

/*------------------------------------*/
// Bnak2 Register (access by temperature 3 address only)
/*------------------------------------*/
#define W83782_REG_TEMP3_H			0x50				// temperature 3 Reading High Byte
#define W83782_REG_TEMP3_L			0x51				// temperature 3 Reading Low Byte
#define W83782_REG_TEMP3_CFG		0x52				// temperature 3 Configuration
#define W83782_REG_TEMP3_HYSIS_H	0x53				// temperature 3 Hystersis High Byte
#define W83782_REG_TEMP3_HYSIS_L	0x54				// temperature 3 Hystersis Low Byte
#define W83782_REG_TEMP3_OVER_H		0x55				// temperature 3 Over temperature High Byte
#define W83782_REG_TEMP3_OVER_L		0x56				// temperature 3 Over temperature Low Byte

/*------------------------------------*/
// Bnak4 Register
/*------------------------------------*/
#define W83782_REG_INT_STS3			0x50				// Interrupt Status 3
#define W83782_REG_SMI_MASK3		0x51				// SMI# Mask 3 (see W83782_ST_SMI_MASK1 for detail)
#define W83782_REG_BEEP_CTRL3		0x53				// Beep Control 1
#define W83782_REG_RT_HW_STS1		0x59				// Real Time Hardware Status 1
#define W83782_REG_RT_HW_STS2		0x5A				// Real Time Hardware Status 2
#define W83782_REG_RT_HW_STS3		0x5B				// Real Time Hardware Status 3
#define W83782_REG_PWM34_CLK		0x5C				// PWM3/4 Clock Select
/*------------------------------------*/
// Bnak5 Register
/*------------------------------------*/
#define W83782_REG_5VSB				0x50				// 5Vsb reading
#define W83782_REG_VBAT				0x51				// V battery reading
#define W83782_REG_5VSB_LMTH		0x54				// 5Vsb Limit High Byte
#define W83782_REG_5VSB_LMTL		0x55				// 5Vsb Limit Low Byte
#define W83782_REG_VBAT_LMTH		0x56				// battery Limit High Byte
#define W83782_REG_VBAT_LMTL		0x57				// battery Limit Low Byte

// W83782_REG_CONFIG
typedef union _W83782_ST_CONFIG{
	struct{
		uint8_t	start		: 1;						// 1: start monitoring, 0: standby mode
		uint8_t	smi_en		: 1;						// 1: enable SMI#, 0: disable
		uint8_t	rsvd0		: 1;
		uint8_t	int_clr		: 1;						// 1: disable the SMI output whithout affecting the contents of imterrupt status Registers and monitoring operation
		uint8_t	rsvd1		: 2;
		uint8_t	gpo_beep	: 1;						// 1: The logical 1 in this bit drives a zero on the BEEP/GP0# pin
		uint8_t	init		: 1;						// 1: reset all registers to default value except the SMBus Address Register
	} bits;
	uint8_t	byte;
} W83782_ST_CONFIG;

// W83782_REG_INT_STS1
typedef union _W83782_ST_INT_STS1{
	struct{
		uint8_t	vcorea		: 1;						// 1: indicates a High or Low limit of Vcore A has been exceeded.
		uint8_t	vinr0		: 1;						// 1: Vin R0 event 
		uint8_t	vin3p3		: 1;						// 1: Vin +3.3V event 
		uint8_t	vin5		: 1;						// 1: Vin +5V event 
		uint8_t	temp1		: 1;						// 1: Temperature 1 event 
		uint8_t	temp2		: 1;						// 1: Temperature 2 event 
		uint8_t	fan1		: 1;						// 1: Fan 1 event 
		uint8_t	fan2		: 1;						// 1: Fan 2 event 
	} bits;
	uint8_t	byte;
} W83782_ST_INT_STS1;

// W83782_REG_INT_STS2
typedef union _W83782_ST_INT_STS2{
	struct{
		uint8_t	vin12		: 1;						// 1: indicates a High or Low limit of Vin +12V has been exceeded.
		uint8_t	vinn12		: 1;						// 1: Vin -12V event 
		uint8_t	vinn5		: 1;						// 1: Vin -5V event 
		uint8_t	fan3		: 1;						// 1: Fan 3 event
		uint8_t	chassis		: 1;						// 1: Chassis Intrusion event
		uint8_t	temp3		: 1;						// 1: Temperature 3 event 
		uint8_t	rvsd		: 2;
	} bits;
	uint8_t	byte;
} W83782_ST_INT_STS2;

// w83782_smi_mask1
typedef union _W83782_ST_SMI_MASK1{						// 1: disable the corresponding interrupt status bit for SMI interrupt.
	struct{
		uint8_t	vcorea		: 1;
		uint8_t	vinr0		: 1;
		uint8_t	vin3p3		: 1;
		uint8_t	vin5		: 1;
		uint8_t	temp1		: 1;
		uint8_t	temp2		: 1;
		uint8_t	fan1		: 1;
		uint8_t	fan2		: 1;
	} bits;
	uint8_t	byte;
} W83782_ST_SMI_MASK1;

// w83782_smi_mask2
typedef union _W83782_ST_SMI_MASK2{						// 1: disable the corresponding interrupt status bit for SMI interrupt.
	struct{
		uint8_t	vin12		: 1;						
		uint8_t	vinn12		: 1;
		uint8_t	vinn5		: 1;
		uint8_t	fan3		: 1;
		uint8_t	chassis		: 1;
		uint8_t	temp3		: 1;
		uint8_t	rvsd		: 2;
	} bits;
	uint8_t	byte;
} W83782_ST_SMI_MASK2;

EFI_STATUS hwm_w83782_init(
	SMB_BUS 		IN 	*SmbusHc,
	HWM_HANDLE_ST 	OUT	**Hwm
);

#endif //_W83782_H


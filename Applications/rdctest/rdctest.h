#ifndef __RDCTEST_H__
#define __RDCTEST_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <Register/Msr.h>

#include "a9610/common.h"
#include "a9610/smb.h"
#include "a9610/i2c.h"
#include "a9610/can.h"
#include "a9610/gpio.h"

#include "util.h"
#include "global.h"

#include "pci.h"

#include "io.h"
#include "aec.h"
#include "rdcec.h"


#define RDC_SMBUS_CLK_SRC		50000			// 50Mhz (unit: khz)
#define RDC_I2C_CLK_SRC			50000			// 50Mhz (unit: khz)

//retries
#define SMB_WAIT_RETRIES		3000

#define RDC_CHIP_ID				0x9610

/*==============================================================*/
// IO space
/*==============================================================*/
#define RDC_PNP_INDEX			0x299
#define RDC_PNP_DATA			0x29A

//smb register address offset
#define SMB_STS_REG_OFFSET		0x00
#define SMB_STS2_REG_OFFSET		0x01
#define SMB_CTL_REG_OFFSET			0x02
#define SMB_CMD_REG_OFFSET		0x03
#define SMB_SADD_REG_OFFSET		0x04
#define SMB_DAT0_REG_OFFSET		0x05
#define SMB_DAT1_REG_OFFSET		0x06
#define SMB_BLK_REG_OFFSET			0x07
#define SMB_PEC_REG_OFFSET		0x08
#define SMB_RSADD_REG_OFFSET		0x09
#define SMB_RSDAT0_REG_OFFSET		0x0A
#define SMB_RSDAT1_REG_OFFSET		0x0B
#define SMB_CTL2_REG_OFFSET		0x0C
#define SMB_PINCTL_REG_OFFSET		0x0D
#define SMB_CTL3_REG_OFFSET		0x0E
#define SMB_CTL4_REG_OFFSET		0x0F
#define SMB_NDADD_REG_OFFSET		0x10
#define SMB_NDLSB_REG_OFFSET		0x11
#define SMB_NDMSB_REG_OFFSET		0x12
#define SMB_CLKCTL1_REG_OFFSET	0x13
#define SMB_CLKCTL2_REG_OFFSET	0x14
#define SMB_EXCTL_REG_OFFSET		0x15

//smb err code : TBD
#define SMB_OK						0x00
#define SMB_ERR_WAIT_FREE_TMOUT	0x01
#define SMB_ERR_PROTOCOL			0x02
#define SMB_ERR_TXDONE_TMOUT		0x03
#define SMB_ERR_FAILED				0x04
#define SMB_ERR_ARBI				0x05
#define SMB_ERR_TMOUT				0x06
#define SMB_ERR_PEC					0x07
#define SMB_ERR_NACK				0x08
#define SMB_ERR_BUF					0x09
#define SMB_ERR_PARAMETERS		0x0A

//smb command : smb_data->mode
#define SMB_CMD_QUICK			0
#define SMB_CMD_BYTE			1
#define SMB_CMD_BYTE_DATA		2
#define SMB_CMD_WORD_DATA	3
#define SMB_CMD_PROC_CALL		4	// process call
#define SMB_CMD_BLOCK			5
#define SMB_CMD_I2C_READ		6
#define SMB_CMD_BLOCK_PROC	7
#define SMB_CMD_MASK			0x7	// not 0xF
#define SMB_CMD_PEC_MASK		0x80

//smb trans->addr
#define SMB_RW_WRITE		0
#define SMB_RW_READ		1
#define SMB_RW_MASK		0x1


//i2c register address offset
#define I2C_CTL_REG_OFFSET		0x00
#define I2C_STS_REG_OFFSET		0x01
#define I2C_MYADD_REG_OFFSET	0x02
#define I2C_TXADD_REG_OFFSET	0x03
#define I2C_DAT_REG_OFFSET		0x04
#define I2C_CLK1_REG_OFFSET	0x05
#define I2C_CLK2_REG_OFFSET	0x06
#define I2C_EXCTL_REG_OFFSET	0x07
#define I2C_SEM_REG_OFFSET		0x08


/***************************************THERMAL IC*************************************************/
#define LM_SMB_ADDR_NUM		5					// The count of LM supported address
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
#define LM_REG_CFG_1				0x03				// Configuration Register 1(RW)
#define LM_REG_CONV_RATE			0x04				// Conversion Rate (RW)
#define LM_REG_ONE_SHOT			0x0F				// One-Shot(WO) - A write to this register activates one conversion if STOP/RUN bit = 1
// Diode Configuration Register
#define LM_REG_CFG_2				0xBF				// Configuration Register 2(RW)
#define LM_REG_REMOTE_OFFSET_H	0x11				// Remote Offset High byte
#define LM_REG_REMOTE_OFFSET_L	0x12				// Remote Offset Low byte
// Temperature Signed Value Register
#define LM_REG_TEMP_H				0x00				// Local Temperature High Byte(RO) 
#define LM_REG_TEMP_L				0x30				// Local Temperature Low Byte(RO) 
#define LM_REG_REMOTE_TEMP_H		0x01				// Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_TEMP_L		0x10				// Remote Temperature Low Byte(RO) 
// Temperature Unsigned Value Register
#define LM_REG_REMOTE_UTEMP_H	0x31				// Unsigned Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_UTEMP_L		0x32				// Unsigned Remote Temperature Low Byte(RO) 
// Limit Register
#define LM_REG_REMOTE_OS_LMT		0x07				// Remote OS Limit 
#define LM_REG_REMOTE_CRIT_LMT	0x19				// Remote T_Crit Limit 
#define LM_REG_T_CRIT_LMT			0x20				// Local Shared OS and T_Crit Limit 
#define LM_REG_COMM_HYSTERESIS	0x21				// Common Hysteresis
// Identification Register
#define LM_REG_MFU_ID				0xFE				// Manufacturer ID (RO) - Always returns 0x01
#define LM_REG_CHIP_REV			0xFF				// Revision ID (RO) - Returns revision number

/*------------------------------------*/
// Status Register Bit Define
/*------------------------------------*/
// Status 1
#define LM_STA1_BIT_BUSY		0x80				// When set to "1" the part is converting
#define LM_STA1_BIT_ROS		0x10				// Status bit for Remote OS
#define LM_STA1_BIT_DIODE		0x04				// Status bit for missing diode (Either D+ is shorted to GND, or VDD; or D+ is floating.)
#define LM_STA1_BIT_RTCRIT		0x02				// Status bit for Remote TCRIT.
#define LM_STA1_BIT_LOC		0x01				// Status bit for the shared Local OS and TCRIT
// Status 2
#define LM_STA2_BIT_NOTREADY	0x80				// Waiting for 30 ms power-up sequence to end

/***************************************DIO BOARD*************************************************/
/*------------------------------------*/
// Register list
/*------------------------------------*/
// Status Register
#define DIO_REG_IN_PORT0		0x00				// Input port 0
#define DIO_REG_IN_PORT1		0x01				// Input port 1
#define DIO_REG_OUT_PORT0		0x02				// Output port 0
#define DIO_REG_OUT_PORT1		0x03				// Output port 1
#define DIO_REG_INV_PORT0		0x04				// Polarity Inversion port 0 - allows the user to invert the polarity of the Input port register data.
#define DIO_REG_INV_PORT1		0x05				// Polarity Inversion port 1
#define DIO_REG_CONFIG0		0x06				// Configuration port 0
#define DIO_REG_CONFIG1		0x07				// Configuration port 1



//#define DEBUG_P 1
#ifdef DEBUG_P
	#define D_Print(format, ...) Print(format, __VA_ARGS__)
#else
	#define D_Print(format, ...)
#endif



typedef struct
{
	uint8_t id;
	uint16_t *str;
	void (*funcfw)(void);
	void (*funcsw)(void);
}MODULE_FUNCTION;

typedef struct
{
	struct
	{
		uint8_t init	: 1;
		uint8_t rsvd	: 7;
	}Flag;
	uint16_t BaseAddr;
	uint16_t Freq;

	uint8_t DevAddr[32];
	uint8_t Cnt;
}RDC_SMBUS;

typedef struct
{
	uint8_t ch;		//SMBus channel
	uint8_t mode;	//Quick, Byte, Word, etc...
	uint8_t cmd;		//Command
	uint8_t addr;		//slave address [7:1], r/w [0]
	
	uint8_t wlen;		//length of write buffer
	uint8_t *wbuf;	//write buffer
	uint8_t rlen;		//length of read buffer
	uint8_t *rbuf;		//read buffer
	uint8_t *blen;		//block length

	uint8_t pec;
}SMBUS_RW;

typedef struct
{
	struct
	{
		uint8_t init	: 1;
		uint8_t rsvd	: 7;
	}Flag;
	uint16_t BaseAddr;
	uint16_t Freq;

	uint8_t DevAddr[32];
	uint8_t Cnt;
}RDC_I2C;

typedef struct
{
	struct
	{
		uint8_t init	: 1;
		uint8_t rsvd	: 7;
	}Flag;
	uint16_t BaseAddr;
	uint16_t Freq;
	
	uint8_t format;
	uint32_t id;
	uint8_t length;
	uint8_t *wbuf;
	uint8_t *rbuf;
}RDC_CANBUS;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//eeprom channel
	uint8_t freq;		//eeprom frequncy
	uint8_t type;		//eeprom type, byte, word
	uint8_t addr;		//eeprom slave address
	uint16_t spos;	//eeprom start position
	uint16_t rwlen;	//eeprom rw data length
	uint8_t wsval;	//eeprom write start value
}Eeprom;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//thermal channel
	uint8_t freq;		//thermal frequncy
	uint8_t addr;		//thermal slave address
	uint8_t wsval;	//thermal write start value
}Thermal;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//dio board channel
	uint8_t freq;		//dio board frequncy
	uint8_t addr;		//dio board slave address
	uint16_t wsval;	//dio board write start value
}DIO;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//smart battery channel
	uint8_t freq;		//smart battery frequncy
	uint8_t addr;		//smart battery slave address
}BATTERY;

typedef struct
{
	Eeprom *eeprom;
	Thermal *thermal;
	DIO *dio;
}I2CVariableConfig;

typedef struct
{
	Eeprom *eeprom;
	Thermal *thermal;
	DIO *dio;
	BATTERY *battery;
}SmbusVariableConfig;

typedef struct
{
	uint8_t init;
	uint8_t txch;		//tx channel
	uint8_t rxch;		//rx channel
	uint8_t freq;		//frequncy
	uint8_t format;	//frame format
	uint8_t mode;	//id mode
	uint32_t id;		//frame id
	uint8_t length;	//data length
	uint8_t data;		//data
	uint32_t loop;		//send time
}CAN;

typedef struct
{
	CAN *can;
}CanVariableConfig;



uint8_t CheckBusLDN(uint8_t type);

void ShowBoardInformation(void);
void ShowHardwareMonitor(void);
void ThermalTest(void);
void WatchDogTest(void);
void FanTest(void);
void GPIOTest(void);
void PanelTest(void);
void SmbusTest(void);
void I2CTest(void);
void CaseOpenTest(void);
void StorageTest(void);
void CANBusTest(void);
void SmbusUseI2CModeTest(void);


extern uint16_t *gThermalType[];
extern uint16_t *gFanType[];
extern uint16_t *gVoltageType[];
extern uint16_t *gVoltageTypeSW[];
extern uint8_t gLM95245Address[LM_SMB_ADDR_NUM];
extern UINT8 gPCA9555Address[8];

#endif

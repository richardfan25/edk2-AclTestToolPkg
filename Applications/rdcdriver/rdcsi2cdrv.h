#ifndef _RDCSI2CDRV_H__
#define _RDCSI2CDRV_H__



#include <Protocol/I2cMaster.h>



#define RDC_SMBUS_CLK_SRC		50000			// 50Mhz (unit: khz)

#define SS_SPEED	(100*1000)	//100kbps
#define FS_SPEED	(400*1000)	//400kbps
#define FP_SPEED	(1000*1000)	//1Mbps
#define HS_SPEED	(3400*1000)	//3.4Mbps


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

//smb command mode
#define RDC_SMB_CMD_QUICK			0
#define RDC_SMB_CMD_BYTE			1
#define RDC_SMB_CMD_BYTE_DATA	2
#define RDC_SMB_CMD_WORD_DATA	3
#define RDC_SMB_CMD_PROC_CALL		4	// process call
#define RDC_SMB_CMD_BLOCK			5
#define RDC_SMB_CMD_I2C_READ		6
#define RDC_SMB_CMD_BLOCK_PROC	7
#define RDC_SMB_CMD_MASK			0x7	// not 0xF
#define RDC_SMB_CMD_PEC_MASK		0x80



typedef struct _SI2C_CONFIGURATION
{
	UINT32 RxFifoDepth;
	UINT32 TxFifoDepth;
}SI2C_CONFIGURATION;

typedef struct _SI2CMASTER_PRIVATE
{
	EFI_I2C_MASTER_PROTOCOL Si2cController;
	EFI_DEVICE_PATH_PROTOCOL *DevPath;
	EFI_HANDLE Handle;	///< Handle
	UINT16 Si2cBusIO;
	UINT8 Si2cBusId;
	UINTN SlaveAddress;
	UINTN ClockFrequency;
	UINT8 *WriteData;
	UINT32 WriteCount;
	UINT8 *ReadData;
	UINT32 ReadCount;
	UINT32 TransferCount;
	SI2C_CONFIGURATION Si2cConfiguration;
}SI2CMASTER_PRIVATE;

/// Private SI2C Master Data Block Structure
typedef struct _SI2C_MASTER_PRIVATE
{
	SI2CMASTER_PRIVATE Si2cMaster[2];		///< SI2c0~1 Data Block
}SI2C_MASTER_PRIVATE;

typedef struct
{
	DEVICE_LOGICAL_UNIT_DEVICE_PATH RDCSi2cLDN;
	//MEMMAP_DEVICE_PATH SI2cMemMapDevPath;
	CONTROLLER_DEVICE_PATH Si2cBus;
	EFI_DEVICE_PATH_PROTOCOL End;
}SI2C_DEVICE_PATH; 



EFI_STATUS EFIAPI RDCSi2cMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable);



#endif


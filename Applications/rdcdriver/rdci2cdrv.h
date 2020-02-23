#ifndef _RDCI2CDRV_H__
#define _RDCI2CDRV_H__



#include <Protocol/I2cMaster.h>



#define RDC_I2C_CLK_SRC		5000

#define SS_SPEED	(100*1000)	//100kbps
#define FS_SPEED	(400*1000)	//400kbps
#define FP_SPEED	(1000*1000)	//1Mbps
#define HS_SPEED	(3400*1000)	//3.4Mbps

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



typedef struct _I2C_CONFIGURATION
{
	UINT32 RxFifoDepth;
	UINT32 TxFifoDepth;
}I2C_CONFIGURATION;

typedef struct _I2CMASTER_PRIVATE
{
	EFI_I2C_MASTER_PROTOCOL I2cController;
	EFI_DEVICE_PATH_PROTOCOL *DevPath;
	EFI_HANDLE Handle;	///< Handle
	UINT16 I2cBusIO;
	UINT8 I2cBusId;
	UINTN SlaveAddress;
	UINTN ClockFrequency;
	UINT8 *WriteData;
	UINT32 WriteCount;
	UINT8 *ReadData;
	UINT32 ReadCount;
	UINT32 TransferCount;
	I2C_CONFIGURATION I2cConfiguration;
}I2CMASTER_PRIVATE;

/// Private I2C Master Data Block Structure
typedef struct _I2C_MASTER_PRIVATE
{
	I2CMASTER_PRIVATE I2cMaster[2];		///< I2c0~1 Data Block
}I2C_MASTER_PRIVATE;

typedef struct
{
	DEVICE_LOGICAL_UNIT_DEVICE_PATH RDCI2cLDN;
	//MEMMAP_DEVICE_PATH I2cMemMapDevPath;
	CONTROLLER_DEVICE_PATH I2cBus;
	EFI_DEVICE_PATH_PROTOCOL End;
}I2C_DEVICE_PATH; 



EFI_STATUS EFIAPI RDCI2cMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable);



#endif

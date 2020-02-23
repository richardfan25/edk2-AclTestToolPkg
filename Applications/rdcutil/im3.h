#ifndef _IMANAGER3_H_
#define _IMANAGER3_H_
#include "targets/crossos.h"

/* EC definitions */
#define EC_ID					0x9610

// LPC PNP
#define PNP_INDEX				0x299
#define PNP_DATA				0x29A

// LPC LDN
#define LDN_UART0				0x02
#define LDN_UART1				0x03
#define LDN_UART2				0x04
#define LDN_UART3				0x05
#define LDN_UART4				0x06
#define LDN_UART5				0x07
#define LDN_UART6				0x08
#define LDN_UART7				0x09
#define LDN_LPT					0x0A
#define LDN_KBC					0x0B
#define LDN_PMC0				0x0C
#define LDN_PMC1				0x0D
#define LDN_PMCMB				0x0E
#define LDN_PMCIO				0x0F
#define LDN_UART8				0x10
#define LDN_CAN0				0x18
#define LDN_CAN1				0x19
#define LDN_I2C0				0x20
#define LDN_I2C1				0x21
#define LDN_SMBUS0				0x22
#define LDN_SMBUS1				0x23
#define LDN_GPIO0				0x24
#define LDN_GPIO1				0x25

// PCI ID
#define EC_VID					0x13FE		// Advantech Corp, Ltd.
#define DID_PMC					0x0832
#define DID_GPIO0				0x1901
#define DID_GPIO1				0x1902
#define DID_I2C					0x1501
#define DID_SMB					0x1511

#define I2C0_PCI_DEV			6
#define I2C0_PCI_FUN			0
#define I2C1_PCI_DEV			6
#define I2C1_PCI_FUN			1
#define SMB0_PCI_DEV			6
#define SMB0_PCI_FUN			2
#define SMB1_PCI_DEV			6
#define SMB1_PCI_FUN			3
#define GPIO0_PCI_DEV			6
#define GPIO0_PCI_FUN			4
#define GPIO1_PCI_DEV			6
#define GPIO1_PCI_FUN			5
#define PMC_PCI_DEV				3
#define PMC_PCI_FUN				0

#define BOARD_MANUFACTURER_NAME_SIZE	16
#define BOARD_NAME_SIZE					16
#define BOARD_SERIAL_NUMBER_SIZE		16
#define BOARD_BIOS_REVISION_SIZE		4
#define BOARD_PLATFORM_TYPE_SIZE		16

/* Structure */
typedef struct pmc_port {
	uint16_t cmd;
	uint16_t data;
} pmc_port_t;

typedef struct im3dev {
	uint8_t isLPC : 1;

	pmc_port_t pmc[2];

	struct {
		uint16_t index;
		uint16_t data;
	} pmcmb;

	struct {
		uint16_t addr;
		uint16_t data;
	} pmcio;				// Need 32-bit access
} im3dev_t;

extern im3dev_t im3;

typedef struct _EIOIS200ROInfo
{
	uint32_t powerOnCount;
	uint32_t runningHour;
	uint8_t ByteProtect[8];
	char BoardManufacturerName[BOARD_MANUFACTURER_NAME_SIZE];	// ADVANTECH
	char BoardName[BOARD_NAME_SIZE];							// EIO-IS200
	char BoardSerialNumber[BOARD_SERIAL_NUMBER_SIZE];			// EPA0000001
	char BoardBIOSRevision[BOARD_BIOS_REVISION_SIZE];			// V005
	char BoardPlatformType[BOARD_PLATFORM_TYPE_SIZE];			// COMExpress
	uint32_t EAPISpecVersion;									// 1,0 (0x01000000)
	uint32_t BoardPnpidVal;										// ADV,0x123 (0xF9604123)
	uint32_t BoardPlatformRevVal;								// 2,0 (0x02000000)
	uint32_t BoardDriverVersion;								// 1.0.0 (0x01000000)
	uint32_t BoardLibraryVersion;								// 1.0.0 (0x01000000)
	uint32_t BoardFirmwareVersion;								// 1.0.0 (0x01000000)
} EIOIS200ROInfo, *PEIOIS200ROInfo;

/* Functions */
int im3_Initialize(void);
int im3_EnterUpgradeMode(void);
void im3_LeaveUpgradeMode(void);
void im3_ResetEC(void);
int im3_FactoryBurn(const PEIOIS200ROInfo roInfo);
int im3_FactoryRead(const PEIOIS200ROInfo roInfo);

void outPNPIndex(uint8_t index);
void outPNPData(uint8_t data);
uint8_t inPNPData(void);

#include "im3io.h"
#include "im3spi.h"
#include "im3flash.h"
#include "im3bin.h"

#endif /* _IMANAGER3_H_ */

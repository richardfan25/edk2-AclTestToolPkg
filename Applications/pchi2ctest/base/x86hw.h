#ifndef _X86HW_H
#define _X86HW_H

#include "global.h"
//=============================================================================
//  define
//=============================================================================
#define	PCI_CMD_PORT	0x0CF8
#define	PCI_DAT_PORT	0x0CFC

#define CPUIDTOBASEFAMILY(cpuid)(((cpuid) >> 8) & 0xf)
#define CPUIDTOBASEMODEL(cpuid)	(((cpuid) >> 4) & 0xf)
#define CPUIDTOEXTFAMILY(cpuid)	(((cpuid) >> 20) & 0xff)
#define CPUIDTOEXTMODEL(cpuid)	(((cpuid) >> 16) & 0xf)

#define CPUIDTOFAMILY(cpuid)	(CPUIDTOBASEFAMILY(cpuid) + ((CPUIDTOBASEFAMILY(cpuid) != 0x0f) ? 0 : CPUIDTOEXTFAMILY(cpuid)))
#define CPUIDTOMODEL(cpuid)		(CPUIDTOBASEMODEL(cpuid) | ((CPUIDTOBASEFAMILY(cpuid) != 0x0f) && (CPUIDTOBASEFAMILY(cpuid) != 0x06) ? 0 : (CPUIDTOEXTMODEL(cpuid) << 4)))

#define VENDORID_AMD	0x1022

#define SMN_ADDR_REG	0x60
#define SMN_DATA_REG	0x64

#define NBIF0EPF7CFGx00000004 0x10147004

//C2P Mailbox
#define MP2_C2PMSG_0      0x03E10500
#define MP2_C2PMSG_1      0x03E10504
#define MP2_C2PMSG_2      0x03E10508
#define MP2_C2PMSG_3      0x03E1050C
#define MP2_C2PMSG_4      0x03E10510
#define MP2_C2PMSG_5      0x03E10514
#define MP2_C2PMSG_6      0x03E10518
#define MP2_C2PMSG_7      0x03E1051C
#define MP2_C2PMSG_8      0x03E10520
#define MP2_C2PMSG_9      0x03E10524
#define MP2_C2PMSG_10     0x03E10528
#define MP2_C2PMSG_11     0x03E1052C
#define MP2_C2PMSG_12     0x03E10530
#define MP2_C2PMSG_13     0x03E10534
#define MP2_C2PMSG_14     0x03E10538 
#define MP2_C2PMSG_15     0x03E1053C

//P2C Mailbox 
#define MP2_P2CMSG_0      0x03E10680 
#define MP2_P2CMSG_1      0x03E10684
#define MP2_P2CMSG_2      0x03E10688
#define MP2_P2CMSG_3      0x03E1068C
#define MP2_P2CMSG_INTEN  0x03E10690  ///< shall be enabled by the MP2 firmware to generate interrupt.
#define MP2_P2CMSG_INTSTS 0x03E10694 



//=============================================================================
//  PCI_DEVICE
//=============================================================================
typedef struct st_pci_dev {

	uint8_t			bus;
	uint8_t			dev;
	uint8_t			fun;
	uint16_t		vendor_id;		// 00h
	uint16_t		device_id;		// 02h
	uint16_t		command;		// 04h
	uint16_t		status;			// 06h
	uint8_t			rev_id;			// 08h-rev ID
	uint8_t			class_code[3];	// 09h~0Bh
	uint8_t			cache_line_sz;	// 0Ch
	uint8_t			latency_timer;	// 0Dh
	uint8_t			header_type;	// 0Eh
	uint8_t			bist;			// 0Fh
	uint32_t		bar[6];			// 10h~27h
	uint32_t		cb_cis_ptr;		// 28h
	uint16_t		ssys_vendor_id;	// 2Ch
	uint16_t		ssys_id;		// 2Eh
	uint32_t		exp_rom_base;	// 30h
	uint8_t			capability_ptr;	// 34h
	uint8_t			res[7];			// 35h~3Bh
	uint8_t			int_line;		// 3Ch
	uint8_t			int_pin;		// 3Dh
	uint8_t			min_gnt;		// 3Eh
	uint8_t			max_lat;		// 3Fh

	struct st_pci_dev	*prev;
	
	struct st_pci_dev	*next;

} PCI_DEVICE;

//=============================================================================
//  PCI_BRIDGE
//=============================================================================
typedef struct st_pci_bridge {

	uint8_t			bus;
	uint8_t			dev;
	uint8_t			fun;
	uint16_t		vendor_id;		// 00h
	uint16_t		device_id;		// 02h
	uint16_t		command;		// 04h
	uint16_t		status;			// 06h
	uint8_t			rev_id;			// 08h-rev ID
	uint8_t			class_code[3];	// 09h~0Bh
	uint8_t			cache_line_sz;	// 0Ch
	uint8_t			latency_timer;	// 0Dh
	uint8_t			header_type;	// 0Eh
	uint8_t			bist;			// 0Fh
	uint32_t		bar[6];			// 10h~27h
	uint32_t		cb_cis_ptr;		// 28h
	uint16_t		ssys_vendor_id;	// 2Ch
	uint16_t		ssys_id;		// 2Eh
	uint32_t		exp_rom_base;	// 30h
	uint8_t			capability_ptr;	// 34h
	uint8_t			res[7];			// 35h~3Bh
	uint8_t			int_line;		// 3Ch
	uint8_t			int_pin;		// 3Dh
	uint8_t			min_gnt;		// 3Eh
	uint8_t			max_lat;		// 3Fh

	struct st_pci_dev	*prev;
	struct st_pci_dev	*next;

} PCI_BRIDGE;
//=============================================================================
//  PCI_BUS
//=============================================================================
typedef struct st_pci_bus {

	PCI_DEVICE		*head;
	int				num_dev;
	uint32_t		iocmd;
	
} PCI_BUS;

//=============================================================================
//  SMB Controller
//=============================================================================
// Intel SMBus
#define SMB_HST_STS		(0)	// offset
#define SMB_HST_CNT		(2)
#define SMB_HST_CMD		(3)
#define SMB_HST_ADD		(4)
#define SMB_HST_DAT0	(5)
#define SMB_HST_DAT1	(6)
#define SMB_HST_BLKDAT	(7)
#define SMB_HST_PEC		(8)		// ICH3 and later
#define SMB_HST_AUXSTS	(12)	// ICH4 and later
#define SMB_HST_AUXCTL	(13)	// ICH4 and later


//=============================================================================
//  SMB_DEV_TYPE
//=============================================================================
#define SMB_DEV_SPD_SDR		(0x00)
#define SMB_DEV_SPD_DDR		(0x01)
#define SMB_DEV_SPD_DDR2	(0x02)
#define SMB_DEV_SPD_DDR3	(0x03)
#define SMB_DEV_SPD_DDR4	(0x04)
#define SMB_DEV_EEPROM		(0x05)
#define SMB_DEV_RTC			(0x06)
#define SMB_DEV_LED_BOARD	(0x07)
#define SMB_DEV_THERMAL_IC	(0x08)
#define SMB_DEV_UNKNOWN		(0xFE)

//=============================================================================
//  SMB_DEVICE
//=============================================================================
typedef struct st_smb_device {

	uint8_t		addr;		// slave address
	uint8_t		type;		// SMB_DEV_xxx

	struct st_smb_device	*prev;
	struct st_smb_device	*next;

} SMB_DEVICE;

//=============================================================================
//  SMB_BUS
//=============================================================================
typedef struct st_smb_bus {
	
	uint8_t			found;
	uint8_t			dev_num;
	uint16_t		base;
	uint16_t		vendor;
	uint8_t			(*read_byte)();
	uint16_t		(*read_word)();
	uint8_t			(*write_byte)();
	uint8_t			(*write_word)();
	SMB_DEVICE		*dev_head;

} SMB_BUS;


//=============================================================================
//  AMD MP2
//=============================================================================
typedef union {
  UINT32                          Raw;                        ///< MP2 I2C Command Reg
  struct {                                                    ///
    UINT32                        i2c_cmd:4;                  ///< I2c read write command
    UINT32                        i2cBusId :4;                ///< I2c bus index 
    UINT32                        devAddr:8;                  ///< Device address or Bus Speed
    UINT32                        length:12;                  ///< Read/write length 
    UINT32                        i2cSpeed:3;                 ///< Register address to be removed later
    UINT32                        memTypeI2cMode:1;           ///< Mem type or I2C 7bit/10bit mode 
  }                               Field;                      ///
} AMD_MP2_I2C_CMD_REG; 

typedef union {
  UINT32                          Raw;                        ///< MP2 I2C Command Reg
  struct {                                                    ///
    UINT32                        response:2;                 ///< I2C response_type
    UINT32                        status:5;                   ///< Status_type
    UINT32                        memType:1;                  ///< 0-output in DRAM,1-Output in C2PMsg
    UINT32                        i2cBusId:4;                 ///< I2C Bus ID
    UINT32                        length:12;                  ///< Length
    UINT32                        slaveAddr:8;                ///< Debug message include in p2c msg 1-2 
  }                               Field;                      ///
} AMD_MP2_I2C_RESPOND_REG;

typedef enum i2c_speed {
  speed100k  = 0,
  speed400k  = 1,
  speed1000k = 2,
  speed1400k = 3,
  speed3400k = 4
}i2c_speed;

typedef enum i2c_cmd {
  i2cRead,
  i2cWrite,
  i2cEnable,
  i2cDisable,
  NumberOfSensorDiscovered,
  isMP2Active,
  InvalidCommand=0xF,
}i2c_cmd;

typedef enum response_type {
  InvalidResponse= 0,
  CommandSuccess = 1,
  CommandFailed  = 2,
}response_type;

typedef enum status_type {
  I2CReadCompleteEvent  = 0,
  I2CReadFailEvent      = 1,
  I2CWriteCompleteEvent = 2,
  I2CWriteFailEvent     = 3,
  I2CBusEnableComplete  = 4,
  I2CBusEnableFailed    = 5,
  I2CBusDisableComplete = 6,
  I2CBusDisableFailed   = 7,
  Invalid_Data_Length   = 8,
  Invalid_Slave_Address = 9,
  Invalid_I2CBus_Id     = 10,
  Invalid_DRAM_Addr     = 11,
  Invalid_Command       = 12,
  MP2Active             = 13,
  NumberOfSensorsDiscoveredResp = 14,
  I2CBusNotInitialized
}status_type;

typedef enum i2c_bus_index{
  I2C_BUS_0 = 0,
  I2C_BUS_1 = 1,
  I2C_BUS_MAX
}i2c_bus_index;

typedef enum mem_type {
  useDRAM   = 0,
  useC2PMsg = 1,
}mem_type;

//=============================================================================
//  function
//=============================================================================

// pci
void pci_select_device(PCI_BUS *pbus, uint8_t b, uint8_t d, uint8_t f);

uint32_t pci_read_dword(PCI_BUS *pbus, uint8_t reg);
uint8_t	 pci_read_byte(PCI_BUS *pbus, uint8_t reg);

void pci_write_dword(PCI_BUS *pbus, uint8_t reg, uint32_t data);
void pci_write_byte(PCI_BUS *pbus, uint8_t reg, uint8_t data);

PCI_BUS *pci_bus_scan(void);
void pci_bus_free(PCI_BUS *pbus);

PCI_DEVICE* pci_find_vendor(PCI_BUS *pbus, uint16_t vendor);
PCI_DEVICE* pci_find_device(PCI_BUS *pbus, uint16_t device);
PCI_DEVICE* pci_find_vendor_device(PCI_BUS *pbus, uint16_t vendor, uint16_t device);
PCI_DEVICE* pci_find_function(PCI_BUS *pbus, uint8_t bus, uint8_t dev, uint8_t fun);
PCI_DEVICE* pci_find_class_subclass_code(PCI_BUS *pbus, uint8_t class_code, uint8_t subclass_code);

uint8_t EFIPCIInit(void);
EFI_STATUS Mp2I2CEnable(IN UINT8 BusId, IN UINTN ClockSpeed);
EFI_STATUS Mp2I2cRead(IN UINT8 BusId, IN UINT8 SlaveAddress, IN UINTN ClockSpeed, IN UINT32 Length, IN UINT8 *Buffer);
EFI_STATUS Mp2I2cWrite(IN UINT8 BusId, IN UINT8 SlaveAddress, IN UINTN ClockSpeed, IN UINT32 Length, IN UINT8 *Buffer);

// smbus
//void smb_delay(void);
//void smb_scan_bus_intel_amd(SMB_MASTER *sm);
//void smb_scan_bus_nvidia(SMB_MASTER *sm);
//uint8_t	smb_read_byte_intel_amd(uint8_t slave_addr, uint8_t reg);
//uint8_t	smb_read_byte_nvidia(uint8_t slave_addr, uint8_t reg);

SMB_BUS *smb_init(void);
void smb_exit(SMB_BUS *smbus);
uint8_t	smb_read_byte_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg);
uint16_t smb_read_word_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg);
uint8_t	smb_write_byte_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg, uint8_t data);
uint8_t	smb_write_word_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg, uint16_t data);


void full_reset(void);
void hard_reset(void);
void soft_reset(void);
void soft92_reset(void);
void kbc_reset(void);

#endif

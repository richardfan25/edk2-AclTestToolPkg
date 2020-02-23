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
	
	uint8_t			found[2];
	uint8_t			dev_num[2];
	uint8_t			dev_addr[2][128];
	uint16_t		base[2];
	uint16_t		vendor;

} SMB_BUS;

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
//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SMB (System Management Bus)                                        *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_SMB_H
#define __X86_SMB_H

#include "typedef.h"
#include "pci.h"

#pragma pack(1)
//=============================================================================
//  define
//=============================================================================
// error code
#define SMB_OK					0
#define SMB_ERR_NULL_BUS		1
#define SMB_ERR_MALLOC_BUS		2
#define SMB_ERR_PCI_INIT		3
#define SMB_ERR_PCI_EXIT		4
#define SMB_ERR_PLAT_NOT_FOUND	5
#define SMB_ERR_CLA_NOT_FOUND	6
#define SMB_ERR_MALLOC_DEV		7
#define SMB_ERR_WRITE_BYTE		8
#define SMB_ERR_DEV_NOT_FOUND	9
#define SMB_ERR_CTLR_NOT_FOUND	10
#define SMB_ERR_CTLR_NOT_SUP	11

// SMBus device type
#define SMB_DEV_SPD_SDR		(0x00)
#define SMB_DEV_SPD_DDR		(0x01)
#define SMB_DEV_SPD_DDR2	(0x02)
#define SMB_DEV_SPD_DDR3	(0x03)
#define SMB_DEV_SPD_DDR4	(0x04)
#define SMB_DEV_EEPROM		(0x05)
#define SMB_DEV_RTC			(0x06)
#define SMB_DEV_LED_BOARD	(0x07)
#define SMB_DEV_UNKNOWN		(0x08)

#define SMB_DEV_TYPE_UNKNOWN	(255)

//=============================================================================
//  smb_dev_t
//=============================================================================
typedef struct _smb_dev_t
{
	uint8_t		addr;	// slave address
	uint8_t		type;	// device type : SMB_DEV_xxx
	uint8_t		prot;	// todo : protocol
	uint8_t		rsvd;	// reserved

	struct _smb_dev_t	*prev;
	struct _smb_dev_t	*next;

} smb_dev_t;

//=============================================================================
//  smb_bus_t
//=============================================================================
typedef struct _smb_bus_t
{
	smb_dev_t	*dev;

	uint8_t		found;		// bus found ?
	uint8_t		num_dev;	// number of device
	uint16_t	base;		// base address
	uint16_t	plat_id;	// platform id
	uint16_t	ven_id;		// vendor id
	uint16_t	dev_id;		// device id

	pci_dev_t	*pcidev;	// pci device

	uint8_t		(*quick_command)();
	
	//uint8_t		(*receive_byte)();
	uint8_t		(*read_byte)();
	
	//uint8_t	(*read_byte)();
	//uint8_t	(*write_byte)();
	uint8_t		(*read_byte_data)();
	uint8_t		(*write_byte_data)();
	
	//uint16_t	(*read_word)();		// word
	//uint8_t	(*write_word)();
	uint16_t	(*read_word_data)();
	uint8_t		(*write_word_data)();
	
	uint16_t	(*process_call)();

	uint8_t		(*read_block_data)();
	uint8_t		(*write_block_data)();
	
	// todo : rest of SMBus protocol

	uint32_t	status;		// initialized = 0x55AA33CC

} smb_bus_t;
#pragma pack()
//=============================================================================
//  extern
//=============================================================================
extern smb_bus_t	*smb;

//=============================================================================
//  functions
//=============================================================================
smb_dev_t *smb_add_dev(void);
void smb_delay(void);

uint8_t smb_init(void);
uint8_t smb_exit(void);
uint8_t smb_scan_dev(uint8_t addr);

uint8_t smb_scan_bus(uint8_t st_addr, uint8_t en_addr);
uint8_t	smb_find_dev(uint8_t addr);
uint32_t smb_status(void);
void smb_amd_select_port(uint8_t port);

#endif

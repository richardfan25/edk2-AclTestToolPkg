//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - CSM (Compatiable Support Module)                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_CSM_H
#define __X86_CSM_H

#include "typedef.h"

//=============================================================================
//  typedef : csm_info_t
//=============================================================================
#pragma pack(1)
typedef struct _csm_info_t
{
	uint32_t	sig;				// 00 signature
	uint8_t		tab_sum;			// 04 table checksum
	uint8_t		tab_len;			// 05 table length
	uint8_t		efi_major_rev;		// 06 efi major revision
	uint8_t		efi_minor_rev;		// 07 efi minor revision
	uint8_t		tab_major_rev;		// 08 table major revision
	uint8_t		tab_minor_rev;		// 09 table minor revision
	uint16_t	rsvd;				// 0A reserved

	uint16_t	csm_call_seg;		// 0C CSM16 call segment
	uint16_t	csm_call_off;		// 0E CSM16 call offset
	uint16_t	pnp_inst_seg;		// 10 PnP installation check segment
	uint16_t	pnp_inst_off;		// 12 PnP installation check offset

	uint32_t	efi_sys_tab;		// 14 efi system table
	uint32_t	oemid_str_ptr;		// 18 OEM ID string pointer
	uint32_t	acpi_rsd_ptr;		// 1C ACPI Rsd pointer
	uint16_t	oem_rev;			// 20 OEM revision

	uint32_t	e820_ptr;			// 22 E820 pointer
	uint32_t	e820_len;			// 26 E820 length
	uint32_t	irq_rt_ptr;			// 2A IRQ routing table pointer
	uint32_t	irq_rt_len;			// 2E IRQ routing table length
	uint32_t	mp_tab_ptr;			// 32 MP table pointer
	uint32_t	mp_tab_len;			// 36 MP table length

	uint16_t	oem_int_seg;		// 3A OEM interrupt segment
	uint16_t	oem_int_off;		// 3C OEM interrupt offset

	uint16_t	oem32_seg;			// 3E OEM32 segment
	uint16_t	oem32_off;			// 40 OEM32 offset
	uint16_t	oem16_seg;			// 42 OEM16 segment
	uint16_t	oem16_off;			// 44 OEM16 offset

	uint16_t	tpm_seg;			// 46 TPM segment
	uint16_t	tpm_off;			// 48 TPM offset
	uint32_t	ibv_ptr;			// 4A IBV pointer
	uint32_t	pcie_base;			// 4E PCI-Express base address
	uint8_t		last_pci_bus;		// 52 Last PCI bus
	
	// EFI CSM spec v0.98
	uint32_t	uma_addr;			// 53 UMA address
	uint32_t	uma_sz;				// 57 UMA size
	uint32_t	hi_pmem_addr;		// 5B Hi permanent memory address
	uint32_t	hi_pmem_sz;			// 5F Hi permanent memory size
									// 63

	
	//uint32_t	last_oprom_addr;	// 53 Last OPROM address
									// 57 = 87 bytes
} csm_info_t;

//=============================================================================
//  typedef : csm_t
//=============================================================================
typedef struct _csm_t
{
	uint8_t		sts;

	csm_info_t	info;

} csm_t;
#pragma pack()
//=============================================================================
//  extern
//=============================================================================
extern csm_t	csm;

//=============================================================================
//  functions
//=============================================================================
int			csm_init(void);
csm_t		*csm_get(void);

#endif

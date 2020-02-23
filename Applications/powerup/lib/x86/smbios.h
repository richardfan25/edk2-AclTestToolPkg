//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SMBIOS (System Mangement BIOS)                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_SMBIOS_H
#define __X86_SMBIOS_H

#include "typedef.h"

#define SMBIOS_OK				0
#define SMBIOS_ERR_NOT_FOUND	1
#define SMBIOS_ERR_MALLOC		2
#define SMBIOS_ERR_MEMCPY		3

#pragma pack(1)
//=============================================================================
//  smbios_hdr_t
//=============================================================================
// SMBIOS v2.1
typedef struct _smbios_hdr_t
{
	uint8_t		sig[4];			// [00] _SM_
	uint8_t		sum;			// [04]
	uint8_t		len;			// [05]
	uint8_t		major_ver;		// [06]
	uint8_t		minor_ver;		// [07]
	uint16_t	sz_tab;			// [08]
	uint8_t		eps;			// [0A]
	uint8_t		eps_format[5];	// [0B]
	uint8_t		dmi_sig[5];		// [10] _DMI_
	uint8_t		dmi_sum;		// [15]
	uint16_t	smbios_len;		// [16]
	uint32_t	smbios_addr;	// [18]
	uint16_t	smbios_ent_num;	// [1C]
	uint8_t		smbios_bcd;		// [1E]

} smbios_hdr_t;

//=============================================================================
//  smbios_v3hdr_t
//=============================================================================
// SMBIOS v3.0
typedef struct _smbios_v3hdr_t
{
	uint8_t		sig[5];			// [00] _SM3_
	uint8_t		sum;			// [05] EPS : entry point structure checksum
	uint8_t		len;			// [06] entry point length
	uint8_t		major_ver;		// [07]
	uint8_t		minor_ver;		// [08]
	uint8_t		doc_rev;		// [09]
	uint8_t		eps_rev;		// [0A]
	uint8_t		rsvd;			// [0B]
	uint32_t	tab_sz;			// [0C]

#ifdef __WATCOMC__
	uint64_t	tab_addr;		// [10]	64-bits
#endif
#ifdef __BORLANDC__
	uint32_t	tab_addr_lsb;	// [10]~[13]
	uint32_t	tab_addr_msb;	// [14]~[17]
#endif

} smbios_v3hdr_t;
	
//=============================================================================
//  smbios_t
//=============================================================================
typedef struct _smbios_t
{
	smbios_hdr_t		*hdr;
	smbios_v3hdr_t		*v3hdr;

} smbios_t;

//=============================================================================
//  dmi_type_t
//=============================================================================
typedef struct _dmi_type_t
{
	uint8_t		id;
	uint8_t		len;
	uint16_t	handle;
	
} dmi_type_t;

//=============================================================================
//  dmi_bios_info_t
//=============================================================================
typedef struct _dmi_bios_info_t
{
	uint8_t		id;
	uint8_t		len;
	uint16_t	handle;
	uint8_t		vendor;
	uint8_t		ver;
	uint16_t	imgsz;	// (0x10000-imgsz)*16
	uint8_t		rdate;
	uint8_t		romsz;
	uint8_t		feature[8];
	uint8_t		ext_feature[16];	// TODO

} dmi_bios_info_t;
#pragma pack()

//=============================================================================
//  smbios_00_bios_info
//=============================================================================
typedef struct _smbios_00_bios_info
{
	uint8_t		id;			// 00
	uint8_t		len;		// 01
	uint16_t	handle;		// 02
	uint8_t		vendor;		// 04
	uint8_t		ver;		// 05
	uint16_t	st_seg;		// 06
	uint8_t		rel_date;	// 08
	uint8_t		rom_sz;		// 09
	uint8_t		chr[8];		// 0A
	uint8_t		chr_ext[2];	// 12	2.4
	uint8_t		major_rel;	// 14	2.4
	uint8_t		minor_rel;	// 15	2.4
	uint8_t		ec_major;	// 16	2.4
	uint8_t		ec_minor;	// 17	2.4
	uint16_t	ext_rom_sz;	// 18	3.1
							// 1A = 26 bytes

} smbios_00_bios_info;
	
//=============================================================================
//  extern
//=============================================================================
extern smbios_t		smbios;

//=============================================================================
//  functions
//=============================================================================
uint8_t	smbios_init(void);
uint8_t	smbios_exit(void);

#endif

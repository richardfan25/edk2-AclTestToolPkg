//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ACL - AEC (Advantech Embedded Controller)                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//  ENE KB3920
//  ITE IT8512
//  ITE IT8516
//  ITE IT8518
//  ITE IT8528
//  ITE IT5121
//  RDC A9610 (Advantech EIO-IS200)
//  RDC A9620 (Advantech EIO-201, EIO-211)
//*****************************************************************************
#ifndef __ACL_AEC_H
#define __ACL_AEC_H

#include "typedef.h"

//===========================================================================
//  AEC chip ID
//===========================================================================
#define AEC_CHIP_ENE_KB3920		0x3920
#define AEC_CHIP_ITE_IT8512		0x8512
#define AEC_CHIP_ITE_IT8516		0x8516
#define AEC_CHIP_ITE_IT8518		0x8518
#define AEC_CHIP_ITE_IT8528		0x8528
#define AEC_CHIP_ITE_IT5121		0x5121
#define AEC_CHIP_RDC_A9610		0x9610	// EIO-IS200
#define AEC_CHIP_RDC_A9620		0x9620	// EIO-201, EIO-211

//===========================================================================
//  AEC firmware type
//===========================================================================
#define AEC_TYPE_ENE_3920		0x3920	// ENE KB3920
#define AEC_TYPE_ITE_8528		0x8528	// ITE IT8528
#define AEC_TYPE_ITE_8518		0x8518	// ITE IT8518
#define AEC_TYPE_ITE_8516		0x8516	// ITE IT8516
#define AEC_TYPE_ITE_8512		0x8512	// ITE IT8512
#define AEC_TYPE_ITE_5121		0x5121	// ITE IT5121
#define AEC_TYPE_RDC_SW			0x9610	// RDC A9610
#define AEC_TYPE_RDC_FW			0x961F	// RDC A9610
#define AEC_TYPE_UNKNOWN		0xFFFF

//===========================================================================
//  error code
//===========================================================================
#define AEC_OK					0x00
#define AEC_ERR_MALLOC			0x01
#define AEC_ERR_INIT			0x02
#define AEC_ERR_FW_TYPE			0x03
#define AEC_ERR_TMOUT_IBE		0x04
#define AEC_ERR_TMOUT_OBE		0x05
#define AEC_ERR_TMOUT_OBF		0x06
#define AEC_ERR_TMOUT_MBOX		0x07
#define AEC_ERR_SMB				0x08
#define AEC_ERR_ACPIEC			0x09

#define AEC_ERR_CHECK(res)	{ if (res != AEC_OK) return res; }

#pragma pack(1)
//===========================================================================
//  aec_info_t
//===========================================================================
typedef struct _aec_info_t
{
	uint16_t	chip_id;	// Conf[20h-21h] 0x8528 or 0x9610
	uint8_t		chip_ver;	// Conf[22h]
	uint8_t		chip_ctl;	// Conf[23h]
	uint8_t		vendor;		// Acpi[FAh] 'I', 'R'
	uint8_t		id;			// Acpi[FBh] 0x28, 0x10
	uint8_t		code;		// Acpi[FCh] firmware code base, 0x00=SW, 0x80=FW

} aec_info_t;

//===========================================================================
//  aec_port_t
//===========================================================================
typedef struct _aec_port_t
{
	uint16_t	cmd;	// index
	uint16_t	sts;
	uint16_t	dat;	// data

} aec_port_t;

//===========================================================================
//  aec_fw_t
//===========================================================================
#ifdef __WATCOMC__
typedef struct _aec_fw_t
{
	uint8_t model[16];

	union
	{
		// ITE IT8528 EC
		struct
		{
			uint8_t	tab_code;		// table code
			uint8_t kver_major;		// kernel version
			uint8_t	kver_minor;
			uint8_t chip_vendor;	// 'I'=ITE
			uint8_t chip_id;		// 28h=8528
			uint8_t	prj_id;			// project id
			uint8_t type;			// project type : V=formal, X=test, A=OEM...
			uint8_t ver_major;		// project version
			uint8_t ver_minor;
		}ite;

		// RDC A9610 EC (EIO-IS200)
		struct
		{
			uint8_t ver_str[16];
		}rdc;
	}ec;
	
} aec_fw_t;
#endif
//===========================================================================
//  aec_t
//===========================================================================
typedef struct _aec_t
{
	uint16_t	type;

	aec_info_t	info;
	
#ifdef __WATCOMC__
	aec_fw_t	fw;		// firmware version
#endif
	
	aec_port_t	cfg;
	aec_port_t	acpi;
	aec_port_t	acl;

	uint8_t		err;

} aec_t;

#pragma pack()

//=============================================================================
//  extern
//=============================================================================
//extern aec_t	aec;

//=============================================================================
//  functions
//=============================================================================
uint16_t	aec_get_type(aec_t *aec);		// ITE, RDC
uint16_t	aec_get_chip_id(aec_t *aec);	// PnP[21][20]
uint8_t		aec_get_chip_ver(aec_t *aec);	// PnP[22]
uint8_t		aec_get_chip_ctl(aec_t *aec);	// PnP[23]
uint8_t		aec_get_vendor(aec_t *aec);		// ECRAM[FA]:'I','R'
uint8_t		aec_get_id(aec_t *aec);			// ECRAM[FB]:28h,18h,10h,20h
uint8_t		aec_get_code(aec_t *aec);		// ECRAM[FC]:00h=RDC_SW, 80h=RDC_FW

aec_t*		aec_init(void);
void		aec_exit(aec_t *aec);

#endif

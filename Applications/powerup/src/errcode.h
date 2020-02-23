//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ERRCODE : Error Code                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __ERRCODE_H
#define __ERRCODE_H

#include "file.h"

//===========================================================================
//  error code
//===========================================================================
// general
#define PU_OK					0x00
#define PU_STS_TMR_UPD			0x01
#define PU_STS_IMC_FOUND		0x02

// 
#define PU_ERR_TMR_DEAD			0x10
#define PU_ERR_MALLOC			0x11
#define PU_ERR_E820				0x12
#define PU_ERR_GET_CPU_VENDOR	0x13
#define PU_ERR_GET_CPU_NAME		0x14
#define PU_ERR_SCAN_PCI			0x15
#define PU_ERR_CONFIRM_PCI		0x16
#define PU_ERR_SCAN_SMBUS		0x17
#define PU_ERR_GET_ABIOS		0x18
#define PU_ERR_GET_AEC			0x19
#define PU_ERR_NO_SMB_CTLR		0x1A
#define PU_ERR_SMB_CTLR			0x1B
#define PU_ERR_GET_SMBIOS		0x1C
#define PU_ERR_NON_ACL_MB		0x1D
#define PU_ERR_NO_SPD_FOUND		0x1E
#define PU_ERR_CANT_SUPPORT_SPD	0x1F

// init
#define PU_ERR_DAT_INIT_DAT		0x20
#define PU_ERR_DAT_INIT_HDR		0x21
#define PU_ERR_DAT_INIT_PAR		0x22
#define PU_ERR_DAT_INIT_DES		0x23

// read
#define PU_ERR_DAT_RD_HDR		0x30
#define PU_ERR_DAT_RD_PAR		0x31
#define PU_ERR_DAT_RD_DES		0x32
#define PU_ERR_DAT_RD_RAW		0x33
#define PU_ERR_DAT_RD_REC		0x34

// check
#define PU_ERR_DAT_CHK_OPEN		0x40
#define PU_ERR_DAT_CHK_FSZ		0x41
#define PU_ERR_DAT_CHK_BRD		0x42

#define PU_ERR_DAT_CHK_HDR_MAG	0x43
#define PU_ERR_DAT_CHK_PAR_MAG	0x44
#define PU_ERR_DAT_CHK_DES_MAG	0x45

#define PU_ERR_DAT_CHK_HDR_CRC	0x46
#define PU_ERR_DAT_CHK_PAR_CRC	0x47
#define PU_ERR_DAT_CHK_DES_CRC	0x48
#define PU_ERR_DAT_CHK_RAW_CRC	0x49
#define PU_ERR_DAT_CHK_REC_CRC	0x4A

#define PU_ERR_DAT_RD_MALLOC	0x51
#define PU_ERR_DAT_RD_CHK_RAW	0x52

#define PU_ERR_DAT_RD_RAW_MEM	0x53
#define PU_ERR_DAT_RD_REC_MEM	0x54

#define PU_ERR_DAT_MALLOC		0x64
#define PU_ERR_DAT_OPEN			0x65
#define PU_ERR_DAT_CREATE		0x66
#define PU_ERR_DAT_FILE_SIZE	0x67
#define PU_ERR_DAT_RENAME		0x68
#define PU_ERR_DAT_FILE_FORMAT	0x69
#define PU_ERR_DAT_GEN_REPORT	0x6A

#define PU_ERR_REC_NOT_FOUND	0x72

#define PU_ERR_INF_MALLOC		0x96

// 
#define PU_ERR_ESC_ABORT		0xFA

#pragma pack(1)
//=============================================================================
//  pu_err_t
//=============================================================================
typedef struct _pu_err_t
{
	uint8_t		code;
	char		*str;
	
} pu_err_t;

//=============================================================================
//  pu_err_raw_t
//=============================================================================
typedef struct _pu_err_raw_t
{
	uint32_t	id;		// raw id
	uint32_t	len;	// raw data len
	uint16_t	no;		// record number
	uint16_t	rsvd0;
	uint32_t	rsvd1;
	
} pu_err_raw_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
char *pu_err_str(uint8_t code);
void pu_err_raw_add(uint32_t id, uint32_t len, uint16_t no, uint8_t *data);
uint8_t	*pu_err_raw_find(uint16_t no, uint32_t id, file_t *ef);
void pu_err_raw_output(void);
void pu_raw_output(void);

#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ACPI_EC - ACPI Embedded Controller                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __ACL_ACPI_EC_H
#define __ACL_ACPI_EC_H

#include "typedef.h"

//===========================================================================
//  Port : ACPI EC
//===========================================================================
#define ACPI_EC_CMD_PORT		0x66
#define ACPI_EC_STS_PORT		ACPI_EC_CMD_PORT
#define ACPI_EC_DAT_PORT		0x62

#define ACPI_EC_RETRIES			(65535)
//=============================================================================
//  ACPI_EC_STS_PORT (66h)
//=============================================================================
//
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  | Bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  |Name | GPF | SMI | SCI |BURST| CMD | GPF2| IBF | OBF |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//         ALERT                          BUSY
//
//=============================================================================
#define ACPI_EC_STS_ALERT			0x80	// GPF
#define ACPI_EC_STS_SMI				0x40
#define ACPI_EC_STS_SCI				0x20
#define ACPI_EC_STS_BURST			0x10
#define ACPI_EC_STS_CMD				0x08
#define ACPI_EC_STS_BUSY			0x04	// GPF2
#define ACPI_EC_STS_IBF				0x02
#define ACPI_EC_STS_OBF				0x01

//===========================================================================
//  error code
//===========================================================================
#define ACPI_EC_OK					0x00
#define ACPI_EC_ERR_INIT			0x01
#define ACPI_EC_ERR_FW_TYPE			0x02
#define ACPI_EC_ERR_TMOUT_IBE		0x03
#define ACPI_EC_ERR_TMOUT_OBE		0x04
#define ACPI_EC_ERR_TMOUT_OBF		0x05
#define ACPI_EC_ERR_TMOUT_MBOX		0x06
#define ACPI_EC_ERR_SMB				0x07

#define ACPI_EC_ERR_CHECK(res)	{ if (res != ACPI_EC_OK) return res; }

//===========================================================================
//  ACPI-EC Command
//===========================================================================
#define ACPI_EC_CMD_READ			0x80
#define ACPI_EC_CMD_WRITE			0x81
#define ACPI_EC_CMD_BURST_EN		0x82
#define ACPI_EC_CMD_BURST_DIS		0x83
#define ACPI_EC_CMD_QUERY			0x84

//===========================================================================
//  ACPI-EC-SMB
//===========================================================================
#define ACPI_EC_SMB_BASE	0x00	// defined by EC firmware

#define ACPI_EC_SMB_PROT	(ACPI_EC_SMB_BASE + 0x00)
#define ACPI_EC_SMB_STS		(ACPI_EC_SMB_BASE + 0x01)
#define ACPI_EC_SMB_SLA		(ACPI_EC_SMB_BASE + 0x02)
#define ACPI_EC_SMB_CMD		(ACPI_EC_SMB_BASE + 0x03)
#define ACPI_EC_SMB_DAT0	(ACPI_EC_SMB_BASE + 0x04)	// 32bytes
#define ACPI_EC_SMB_DAT1	(ACPI_EC_SMB_BASE + 0x05)
#define ACPI_EC_SMB_BCNT	(ACPI_EC_SMB_BASE + 0x24)
#define ACPI_EC_SMB_ALARM	(ACPI_EC_SMB_BASE + 0x25)
#define ACPI_EC_SMB_ALARM0	(ACPI_EC_SMB_BASE + 0x26)
#define ACPI_EC_SMB_ALARM1	(ACPI_EC_SMB_BASE + 0x27)

//===========================================================================
//  ACPI-EC-SMB : Protocol
//===========================================================================
#define ACPI_EC_SMB_PROT_WRITE_QUICK		0x02
#define ACPI_EC_SMB_PROT_READ_QUICK			0x03
#define ACPI_EC_SMB_PROT_SEND_BYTE			0x04
#define ACPI_EC_SMB_PROT_RECV_BYTE			0x05
#define ACPI_EC_SMB_PROT_WRITE_BYTE			0x06
#define ACPI_EC_SMB_PROT_READ_BYTE			0x07
#define ACPI_EC_SMB_PROT_WRITE_WORD			0x08
#define ACPI_EC_SMB_PROT_READ_WORD			0x09
#define ACPI_EC_SMB_PROT_WRITE_BLOCK		0x0A
#define ACPI_EC_SMB_PROT_READ_BLOCK			0x0B
#define ACPI_EC_SMB_PROT_PROC_CALL			0x0C
#define ACPI_EC_SMB_PROT_BLOCK_PROC_CALL	0x0D

//===========================================================================
//  functions
//===========================================================================
uint8_t	acpi_ec_read(uint8_t idx, uint8_t *datap);
uint8_t	acpi_ec_write(uint8_t idx, uint8_t data);
void	acpi_ec_smb_trans(uint8_t sla, uint8_t cmd, uint8_t prot, uint8_t *buf, uint8_t *bcnt);
uint8_t acpi_ec_read_ram(uint8_t *ram);

#endif

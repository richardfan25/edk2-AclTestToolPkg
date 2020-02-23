//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE_EC - ITE IT8528/IT8518 Embedded Controller                           *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __ACL_ITE_EC_H
#define __ACL_ITE_EC_H

#include "typedef.h"

//===========================================================================
//  Port : ITE EC
//===========================================================================
#define ITE_CMD_PORT			0x29E			// ITE_CMD/DAT
#define ITE_STS_PORT			ITE_CMD_PORT
#define ITE_DAT_PORT			0x29F
#define ITE_RETRIES				(1000)

#define ITE_EC_CMD_PORT			0x29A			// ADT_CMD/DAT
#define ITE_EC_STS_PORT			ITE_EC_CMD_PORT
#define ITE_EC_DAT_PORT			0x299
#define ITE_EC_RETRIES			(65535)

#define ITE_CFG_IDX_PORT		0x29C
#define ITE_CFG_DAT_PORT		0x29D

//=============================================================================
//  ITE_EC_STS
//=============================================================================
#define ITE_EC_STS_ALERT		0x80	// GPF
#define ITE_EC_STS_SMI			0x40
#define ITE_EC_STS_SCI			0x20
#define ITE_EC_STS_BURST		0x10
#define ITE_EC_STS_CMD			0x08
#define ITE_EC_STS_BUSY			0x04	// GPF2
#define ITE_EC_STS_IBF			0x02
#define ITE_EC_STS_OBF			0x01

#define ITE_STS_ALERT			0x80	// GPF
#define ITE_STS_SMI				0x40
#define ITE_STS_SCI				0x20
#define ITE_STS_BURST			0x10
#define ITE_STS_CMD				0x08
#define ITE_STS_BUSY			0x04	// GPF2
#define ITE_STS_IBF				0x02
#define ITE_STS_OBF				0x01

//===========================================================================
//  error code
//===========================================================================
#define ITE_EC_OK				0x00
#define ITE_EC_ERR_MALLOC		0x01
#define ITE_EC_ERR_INIT			0x02
#define ITE_EC_ERR_FW_TYPE		0x03
#define ITE_EC_ERR_TMOUT_IBE	0x04
#define ITE_EC_ERR_TMOUT_OBE	0x05
#define ITE_EC_ERR_TMOUT_OBF	0x06
#define ITE_EC_ERR_TMOUT_MBOX	0x07
#define ITE_EC_ERR_SMB			0x08

#define ITE_EC_ERR_CHECK(res)	{ if (res != ITE_EC_OK) return res; }

#define ITE_OK					0x00
#define ITE_ERR_MALLOC			0x01
#define ITE_ERR_INIT			0x02
#define ITE_ERR_FW_TYPE			0x03
#define ITE_ERR_TMOUT_IBE		0x04
#define ITE_ERR_TMOUT_OBE		0x05
#define ITE_ERR_TMOUT_OBF		0x06
#define ITE_ERR_TMOUT_MBOX		0x07
#define ITE_ERR_SMB				0x08
#define ITE_ERR_MBOX			0x09

#define ITE_ERR_CHECK(res)		{ if (res != ITE_OK) return res; }

//===========================================================================
//  command
//===========================================================================
// ITE IT8528/ IT8518


// ITE IT5121
#define ITE_CMD_READ			0x80
#define ITE_CMD_WRITE			0x81

#define ITE_IDX_CHIP_VENDOR		0xFA
#define ITE_IDX_CHIP_ID			0xFB
#define ITE_IDX_PRJ_ID			0xFC
#define ITE_IDX_PRJ_TYPE		0xFD

#define ITE_IDX_FW_VER_MAJOR	0xFE
#define ITE_IDX_FW_VER_MINOR	0xFF


// mbox : offset
#define ITE_MBOX_OFF_CMD		0
#define ITE_MBOX_OFF_STS		1
#define ITE_MBOX_OFF_PAR		2
#define ITE_MBOX_OFF_DAT		3
#define ITE_MBOX_OFF_DAT0		3
#define ITE_MBOX_OFF_DAT45		47

// mbox : cmd
#define ITE_MBOX_CMD_CLR_BUF	0xC0
#define ITE_MBOX_CMD_VER		0xF0
#define ITE_MBOX_CMD_CLR		0xFF


//===========================================================================
//  ite_fw_prot_t
//===========================================================================
typedef struct _ite_fw_prot_t
{
	uint8_t		cmd;
	uint8_t		idx;
	uint8_t		off;
	uint8_t		len;
	uint8_t		*dat;

	uint8_t		didx;
	uint8_t		dadd;
	uint8_t		dcmd;

	uint8_t		*rb;
	uint32_t	rblen;
	uint8_t		*wb;
	uint32_t	wblen;

} ite_fw_prot_t;

//===========================================================================
//  functions
//===========================================================================
void	ite_ec_get_ldn_cfg(uint8_t ldn, uint8_t *cfg);
uint8_t	ite_ec_probe_port(void);

uint8_t ite_ec_read_acpi_ram(uint8_t off, uint8_t *dat);
uint8_t ite_ec_write_acpi_ram(uint8_t off, uint8_t dat);

uint8_t ite_ec_fw_prot_read(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_smb_read_quick(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_smb_read_byte(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_smb_read_word(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_smb_write_word(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_smb_read_block(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_i2c_read_proc(ite_fw_prot_t *prot);
uint8_t ite_ec_fw_prot_i2c_write_proc(ite_fw_prot_t *prot);

/*
uint8_t ite_read_acpi_ram(uint8_t off, uint8_t *dat);
uint8_t ite_write_acpi_ram(uint8_t off, uint8_t dat);
*/

/*
uint8_t ite_fw_prot_read(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_write(ite_fw_prot_t *prot);
*/

/*
uint8_t ite_fw_prot_smb_read_quick(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_smb_read_byte(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_smb_read_word(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_smb_write_word(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_smb_read_block(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_i2c_read_proc(ite_fw_prot_t *prot);
uint8_t ite_fw_prot_i2c_write_proc(ite_fw_prot_t *prot);
*/

uint8_t ite_mbox_write(uint8_t off, uint8_t dat);
uint8_t ite_mbox_read(uint8_t off, uint8_t *dat);
uint8_t ite_mbox_wait(void);
void	ite_mbox_clear(void);
uint8_t ite_mbox_read_fw_info(uint8_t *ver);

#endif

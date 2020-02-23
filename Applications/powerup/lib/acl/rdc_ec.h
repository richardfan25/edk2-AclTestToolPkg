//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - RDC A9610 Embedded Controller                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __ACL_RDC_EC_H
#define __ACL_RDC_EC_H

#include "typedef.h"

//===========================================================================
//  Port : RDC EC
//===========================================================================
#define RDC_EC_CMD_PORT			0x2F6
#define RDC_EC_STS_PORT			RDC_EC_CMD_PORT
#define RDC_EC_DAT_PORT			0x2F2
#define RDC_CFG_IDX_PORT		0x299	// 299h/499h
#define RDC_CFG_DAT_PORT		0x29A

#define RDC_EC_RETRIES			(65535)

//=============================================================================
//  RDC_EC_STS (66h)
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
#define RDC_EC_STS_ALERT		0x80	// GPF
#define RDC_EC_STS_SMI			0x40
#define RDC_EC_STS_SCI			0x20
#define RDC_EC_STS_BURST		0x10
#define RDC_EC_STS_CMD			0x08
#define RDC_EC_STS_BUSY			0x04	// GPF2
#define RDC_EC_STS_IBF			0x02
#define RDC_EC_STS_OBF			0x01

//===========================================================================
//  error code
//===========================================================================
#define RDC_EC_OK				0x00
#define RDC_EC_ERR_MALLOC		0x01
#define RDC_EC_ERR_INIT			0x02
#define RDC_EC_ERR_FW_TYPE		0x03
#define RDC_EC_ERR_TMOUT_IBE	0x04
#define RDC_EC_ERR_TMOUT_OBE	0x05
#define RDC_EC_ERR_TMOUT_OBF	0x06
#define RDC_EC_ERR_TMOUT_MBOX	0x07
#define RDC_EC_ERR_SMB			0x08
#define RDC_EC_ERR_ACPIEC		0x09

#define RDC_EC_ERR_CHECK(res)	{ if (res != RDC_EC_OK) return res; }

//===========================================================================
//  RDC_LDN : Logical Device Number
//===========================================================================
#define RDC_LDN_UART0			0x02
#define RDC_LDN_UART1			0x03
#define RDC_LDN_UART2			0x04
#define RDC_LDN_UART3			0x05
#define RDC_LDN_UART4			0x06
#define RDC_LDN_UART5			0x07
#define RDC_LDN_UART6			0x08
#define RDC_LDN_UART7			0x09
#define RDC_LDN_LPT				0x0A
#define RDC_LDN_KBC				0x0B
#define RDC_LDN_PMC0			0x0C
#define RDC_LDN_PMC1			0x0D
#define RDC_LDN_PMCMB			0x0E
#define RDC_LDN_ECIO			0x0F
#define RDC_LDN_CAN0			0x18
#define RDC_LDN_CAN1			0x19
#define RDC_LDN_I2C0			0x20
#define RDC_LDN_I2C1			0x21
#define RDC_LDN_SMB0			0x22
#define RDC_LDN_SMB1			0x23
#define RDC_LDN_GPIO0			0x24

//===========================================================================
//  RDC_FW : protocol
//===========================================================================
#define RDC_FW_BUF_SZ			(256)

#define RDC_FW_CMD_PMC0			0x00
#define RDC_FW_CMD_PMC1			0x01
#define RDC_FW_CMD_PMC_CFG		0x03	// PMC configuration
#define	RDC_FW_CMD_WR_THERM		0x10	// thermal
#define	RDC_FW_CMD_RD_THERM		0x11
#define RDC_FW_CMD_WR_VOLT		0x12	// voltage
#define RDC_FW_CMD_RD_VOLT		0x13
#define RDC_FW_CMD_WR_PWM		0x14	// pwm
#define RDC_FW_CMD_RD_PWM		0x15
#define RDC_FW_CMD_WR_TACH		0x16	// tachometer
#define RDC_FW_CMD_RD_TACH		0x17
#define RDC_FW_CMD_WR_GPIO		0x18	// gpio
#define RDC_FW_CMD_RD_GPIO		0x19
#define RDC_FW_CMD_WR_CURR		0x1A	// current
#define RDC_FW_CMD_RD_CURR		0x1B
#define RDC_FW_CMD_WR_PANEL		0x20	// panel
#define RDC_FW_CMD_RD_PANEL		0x21
#define RDC_FW_CMD_WR_FAN		0x24	// fan
#define RDC_FW_CMD_RD_FAN		0x25
#define RDC_FW_CMD_WR_CASEOPEN	0x28	// case open
#define RDC_FW_CMD_RD_CASEOPEN	0x29
#define RDC_FW_CMD_WR_WDT		0x2A	// watchdog
#define RDC_FW_CMD_RD_WDT		0x2B
#define RDC_FW_CMD_WR_ECRAM		0x30	// ecram
#define RDC_FW_CMD_RD_ECRAM		0x31
#define RDC_FW_CMD_UPDATE		0x40	// PMC1 only
#define RDC_FW_CMD_WR_STORAGE	0x50	// storage
#define RDC_FW_CMD_RD_STORAGE	0x51
#define RDC_FW_CMD_WR_BOARD		0x52	// board info
#define RDC_FW_CMD_RD_BOARD		0x53
#define RDC_FW_CMD_WR_SYSTEM	0x54	// system info
#define RDC_FW_CMD_RD_SYSTEM	0x55

//===========================================================================
//  RDC_SW : protocol
//===========================================================================
#define RDC_SW_BUF_SZ			(256)

#define RDC_SW_CMD_PLAT_INFO	0x41
#define RDC_SW_CMD_RD_FWVER		0x4B

//===========================================================================
//  error code : protocol
//===========================================================================
#define RDC_FW_PMC_OK			0	// NO Error
#define RDC_FW_PMC_ERR_CMD		1	// Unsupport command code
#define RDC_FW_PMC_ERR_CTL		2	// Unsupport control code
#define RDC_FW_PMC_ERR_DEV		3	// Unsupport device id
#define RDC_FW_PMC_ERR_LEN		4	// Invalid length
#define RDC_FW_PMC_ERR_DAT		5	// Invalid data
#define RDC_FW_PMC_ERR_PROT		6	// Unknown protocol
#define RDC_FW_PMC_ERR_RO		7	// Unsupport Write
#define RDC_FW_PMC_ERR_WO		8	// Unsupport Read


#pragma pack(1)
//===========================================================================
//  rdc_fw_prot_t
//===========================================================================
typedef struct _rdc_fw_prot_t
{
	uint8_t		cmd;
	uint8_t		ctl;
	uint8_t		dev;
	uint8_t		len;
	uint8_t		*dat;

} rdc_fw_prot_t;

//===========================================================================
//  rdc_sw_prot_t
//===========================================================================
typedef struct _rdc_sw_prot_t
{
	uint8_t		cmd;
	uint8_t		idx;
	uint8_t		off;
	uint8_t		len;
	uint8_t		*dat;

} rdc_sw_prot_t;
#pragma pack()
//===========================================================================
//  functions
//===========================================================================
void	rdc_ec_get_ldn_cfg(uint8_t ldn, uint8_t *cfg);

uint8_t	rdc_fw_prot_pmc(rdc_fw_prot_t *prot);
uint8_t	rdc_fw_prot_read(rdc_fw_prot_t *prot);
uint8_t	rdc_fw_prot_write(rdc_fw_prot_t *prot);
uint8_t	rdc_fw_prot_read_drt(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf);
uint8_t	rdc_fw_prot_write_drt(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf);

uint8_t	rdc_sw_prot_idx_read(rdc_sw_prot_t *prot);
uint8_t	rdc_sw_prot_idx_write(rdc_sw_prot_t *prot);
uint8_t	rdc_sw_prot_noidx_read(rdc_sw_prot_t *prot);
uint8_t	rdc_sw_prot_noidx_write(rdc_sw_prot_t *prot);
uint8_t	rdc_sw_prot_info_read(rdc_sw_prot_t *prot);

#endif

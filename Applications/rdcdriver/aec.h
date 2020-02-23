//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ACL - AEC (Advantech Embedded Controller)                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//  ITE IT8518
//  ITE IT8528
//  RDC A9610 (Advantech EIO-IS200)
//*****************************************************************************
#ifndef __ACL_AEC_H
#define __ACL_AEC_H

//#include <stdint.h>
//#include "typedef.h"

//===========================================================================
//  AEC chip ID
//===========================================================================
#define AEC_CHIP_ITE_IT8518		0x8518
#define AEC_CHIP_ITE_IT8528		0x8528
#define AEC_CHIP_RDC_A9610		0x9610

//===========================================================================
//  AEC firmware type
//===========================================================================
#define AEC_TYPE_ITE_8528		0x8528
#define AEC_TYPE_ITE_8518		0x8518
#define AEC_TYPE_RDC_SW			0x9610
#define AEC_TYPE_RDC_FW			0x961F
#define AEC_TYPE_UNKNOWN		0xFFFF

//===========================================================================
//  Port : ACPI EC
//===========================================================================
#define ACPI_EC_CMD_PORT		0x66
#define ACPI_EC_STS_PORT		ACPI_EC_CMD_PORT
#define ACPI_EC_DAT_PORT		0x62

//===========================================================================
//  Port : ITE EC
//===========================================================================
#define ITE_CMD_PORT			0x29E			// ITE_CMD/DAT
#define ITE_DAT_PORT			0x29F
#define ITE_EC_CMD_PORT			0x29A			// ADT_CMD/DAT
#define ITE_EC_STS_PORT			ITE_EC_CMD_PORT
#define ITE_EC_DAT_PORT			0x299
#define ITE_CFG_IDX_PORT		0x29C
#define ITE_CFG_DAT_PORT		0x29D

//===========================================================================
//  Port : RDC EC
//===========================================================================
#define RDC_EC_CMD_PORT			0x2F6
#define RDC_EC_STS_PORT			RDC_EC_CMD_PORT
#define RDC_EC_DAT_PORT			0x2F2
#define RDC_CFG_IDX_PORT		0x299	// 299h/499h
#define RDC_CFG_DAT_PORT		0x29A

//===========================================================================
//  Common
//===========================================================================

//=============================================================================
//  AEC_STS (66h)
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
#define AEC_STS_ALERT			0x80	// GPF
#define AEC_STS_SMI				0x40
#define AEC_STS_SCI				0x20
#define AEC_STS_BURST			0x10
#define AEC_STS_CMD				0x08
#define AEC_STS_BUSY			0x04	// GPF2
#define AEC_STS_IBF				0x02
#define AEC_STS_OBF				0x01


#define AEC_RETRIES				(65535)
#define ITE_RETRIES				(100)

//===========================================================================
//  flag
//===========================================================================
#define AEC_FL_INIT				0x80

//===========================================================================
//  error code
//===========================================================================
#define AEC_OK					0x00
#define AEC_ERR_INIT			0x01
#define AEC_ERR_FW_TYPE			0x02
#define AEC_ERR_TMOUT_IBE		0x03
#define AEC_ERR_TMOUT_OBE		0x04
#define AEC_ERR_TMOUT_OBF		0x05
#define AEC_ERR_TMOUT_MBOX		0x06

#define AEC_ERR_CHECK(res)	{ if (res != AEC_OK) return res; }

//===========================================================================
//  error code : AEC PMC
//===========================================================================
#define AEC_PMC_OK				0	// NO Error
#define AEC_PMC_ERR_CMD			1	// Unsupport command code
#define AEC_PMC_ERR_CTL			2	// Unsupport control code
#define AEC_PMC_ERR_DEV			3	// Unsupport device id
#define AEC_PMC_ERR_LEN			4	// Invalid length
#define AEC_PMC_ERR_DAT			5	// Invalid data
#define AEC_PMC_ERR_PROT		6	// Unknown protocol
#define AEC_PMC_ERR_RO			7	// Unsupport Write
#define AEC_PMC_ERR_WO			8	// Unsupport Read

//===========================================================================
//  ACPI EC
//===========================================================================
#define ACPI_EC_CMD_READ		0x80
#define ACPI_EC_CMD_WRITE		0x81
#define ACPI_EC_CMD_BURST_EN	0x82
#define ACPI_EC_CMD_BURST_DIS	0x83
#define ACPI_EC_CMD_QUERY		0x84

#define ACPI_EC_SMB_BASE		0x00
#define ACPI_EC_SMB_PROT		(ACPI_EC_SMB_BASE + 0x00)
#define ACPI_EC_SMB_STS			(ACPI_EC_SMB_BASE + 0x01)
#define ACPI_EC_SMB_SLA			(ACPI_EC_SMB_BASE + 0x02)
#define ACPI_EC_SMB_CMD			(ACPI_EC_SMB_BASE + 0x03)
#define ACPI_EC_SMB_DAT0		(ACPI_EC_SMB_BASE + 0x04)	// DAT0 ~ DAT31 
#define ACPI_EC_SMB_DAT1		(ACPI_EC_SMB_BASE + 0x05)	// 0x04 ~ 0x23 : 32bytes
#define ACPI_EC_SMB_BCNT		(ACPI_EC_SMB_BASE + 0x24)
#define ACPI_EC_SMB_ALARM		(ACPI_EC_SMB_BASE + 0x25)
#define ACPI_EC_SMB_ALARM0		(ACPI_EC_SMB_BASE + 0x26)
#define ACPI_EC_SMB_ALARM1		(ACPI_EC_SMB_BASE + 0x27)

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
#define RDC_LDN_SMBUS0			0x22
#define RDC_LDN_SMBUS1			0x23
#define RDC_LDN_GPIO0			0x24

//===========================================================================
//  RDC_SW
//===========================================================================
#define RDC_SW_BUF_SZ			(256)

#define RDC_SW_CMD_PMC0			0x00
#define RDC_SW_CMD_PMC1			0x01
#define RDC_SW_CMD_PMC_CFG		0x03	// PMC configuration
#define RDC_SW_CMD_WR_THERM		0x10	// thermal
#define RDC_SW_CMD_RD_THERM		0x11
#define RDC_SW_CMD_WR_VOLT		0x12	// voltage
#define RDC_SW_CMD_RD_VOLT		0x13
#define RDC_SW_CMD_WR_PWM		0x14	// pwm
#define RDC_SW_CMD_RD_PWM		0x15
#define RDC_SW_CMD_WR_TACH		0x16	// tachometer
#define RDC_SW_CMD_RD_TACH		0x17
#define RDC_SW_CMD_WR_PANEL		0x20	// panel
#define RDC_SW_CMD_RD_PANEL		0x21
#define RDC_SW_CMD_WR_THERM_PRO	0x22	// thermal protect
#define RDC_SW_CMD_RD_THERM_PRO	0x23
#define RDC_SW_CMD_WR_FAN		0x24	// fan
#define RDC_SW_CMD_RD_FAN			0x25
#define RDC_SW_CMD_WR_CASEOPEN	0x28	// case open
#define RDC_SW_CMD_RD_CASEOPEN	0x29
#define RDC_SW_CMD_WR_WDT		0x2A	// watchdog
#define RDC_SW_CMD_RD_WDT		0x2B
#define RDC_SW_CMD_WDT_START		0x2C	// watchdog start
#define RDC_SW_CMD_WDT_STOP		0x2D	// watchdog stop
#define RDC_SW_CMD_WDT_REF		0x2E	// watchdog refresh
#define RDC_SW_CMD_PWR_CNT_ICS	0x2F	// increase power-on count
#define RDC_SW_CMD_WR_ECRAM		0x30	// ecram
#define RDC_SW_CMD_RD_ECRAM		0x31
#define RDC_SW_CMD_UPDATE			0x40	// PMC1 only
#define RDC_SW_CMD_PLAT_INFO		0x41	// platform infomation
#define RDC_SW_CMD_GPIO_STS		0x42	// gpio status
#define RDC_SW_CMD_WR_GPIO		0x43	// gpio
#define RDC_SW_CMD_RD_GPIO		0x44
#define RDC_SW_CMD_RD_EC_INFO	0x4A	// EC infomation
#define RDC_SW_CMD_RD_FWVER		0x4B	// firmware infomation
#define RDC_SW_CMD_WR_INFO		0x50	// infomation
#define RDC_SW_CMD_RD_INFO		0x51
#define RDC_SW_CMD_WR_FACTORY	0x52	// factory information
#define RDC_SW_CMD_RD_FACTORY	0x53
#define RDC_SW_CMD_SAVE_CFG		0x56	// save configuration
#define RDC_SW_CMD_STORAGE_LK	0x5C	// storage lock
#define RDC_SW_CMD_STORAGE_ULK	0x5D	// storage unlock
#define RDC_SW_CMD_WR_STORAGE	0x5E	// storage
#define RDC_SW_CMD_RD_STORAGE	0x5F
#define RDC_SW_CMD_ACPI_OFF		0x7D	// ACPI on / off
#define RDC_SW_CMD_ACPI_ON		0x7F


//===========================================================================
//  RDC_FW
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


#pragma pack(1)
//===========================================================================
//  aec_info_t
//===========================================================================
typedef struct _aec_info_t
{
	UINT16	chip_id;	// Conf[20h-21h] 0x8528 or 0x9610
	UINT8		chip_ver;	// Conf[22h]
	UINT8		chip_ctl;	// Conf[23h]
	UINT8		vendor;		// Acpi[FAh] 'I', 'R'
	UINT8		id;			// Acpi[FBh] 0x28, 0x10
	UINT8		code;		// Acpi[FCh] firmware code base, 0x00=SW, 0x80=FW

} aec_info_t;

//===========================================================================
//  aec_port_t
//===========================================================================
typedef struct _aec_port_t
{
	UINT16	cmd;	// index
	UINT16	sts;
	UINT16	dat;	// data

} aec_port_t;

//===========================================================================
//  aec_fw_t
//===========================================================================
//#ifdef __WATCOMC__
typedef struct _aec_fw_t
{
	UINT8 model[16];

	union
	{
		// ITE IT8528 EC
		struct
		{
			UINT8	tab_code;		// table code
			UINT8 kver_major;		// kernel version
			UINT8	kver_minor;
			UINT8 chip_vendor;	// 'I'=ITE
			UINT8 chip_id;		// 28h=8528
			UINT8	prj_id;			// project id
			UINT8 type;			// project type : V=formal, X=test, A=OEM...
			UINT8 ver_major;		// project version
			UINT8 ver_minor;
		}ite;

		// RDC A9610 EC (EIO-IS200)
		struct
		{
			UINT8 *ver_str;
		}rdc;
	}ec;
	
} aec_fw_t;
//#endif
//===========================================================================
//  aec_t
//===========================================================================
typedef struct _aec_t
{
	UINT16	type;

	aec_info_t	info;
	
//#ifdef __WATCOMC__
	aec_fw_t	fw;		// firmware version
//#endif
	
	aec_port_t	cfg;
	aec_port_t	acpi;
	aec_port_t	acl;

	UINT8		flag;
	UINT8		err;

} aec_t;

//===========================================================================
//  rdc_fw_prot_t
//===========================================================================
typedef struct _rdc_fw_prot_t
{
	UINT8		cmd;
	UINT8		ctl;
	UINT8		dev;
	UINT8		len;
	UINT8		*dat;

} rdc_fw_prot_t;

//===========================================================================
//  rdc_sw_prot_t
//===========================================================================
typedef struct _rdc_sw_prot_t
{
	UINT8		cmd;
	UINT8		idx;
	UINT8		off;
	UINT8		len;
	UINT8		*dat;

} rdc_sw_prot_t;

//===========================================================================
//  ite_prot_t
//===========================================================================
typedef struct _ite_fw_prot_t
{
	UINT8		cmd;
	UINT8		idx;
	UINT8		off;
	UINT8		len;
	UINT8		*dat;

	UINT8		didx;
	UINT8		dadd;
	UINT8		dcmd;

} ite_fw_prot_t;

#pragma pack()

//=============================================================================
//  extern
//=============================================================================
extern aec_t	aec;

//=============================================================================
//  functions
//=============================================================================
UINT8		aec_init(void);
void		aec_exit(void);
UINT8		aec_probe(void);
UINT16	aec_get_type(void);
UINT16	aec_get_chip_id(void);
UINT8		aec_get_chip_ver(void);
UINT8		aec_get_chip_ctl(void);
UINT8		aec_get_vendor(void);
UINT8		aec_get_id(void);
UINT8		aec_get_code(void);
UINT8		aec_get_ec_ram(UINT8 *ram);
void		aec_get_ldn_cfg(UINT8 ldn, UINT8 *cfg);

UINT8		aec_read_dat(aec_port_t *port, UINT8 *dat);
UINT8		aec_write_dat(aec_port_t *port, UINT8 dat);
UINT8		aec_write_cmd(aec_port_t *port, UINT8 cmd);

UINT8		acpi_ec_read(UINT8 idx, UINT8 *datap);
UINT8		acpi_ec_write(UINT8 idx, UINT8 data);

// RDC_FW protocols
UINT8		rdc_fw_prot_pmc(rdc_fw_prot_t *p);
UINT8		rdc_fw_prot_read(rdc_fw_prot_t *p);
UINT8		rdc_fw_prot_write(rdc_fw_prot_t *p);

// RDC_SW protocols
UINT8		rdc_sw_prot_idx_read(rdc_sw_prot_t *p);
UINT8		rdc_sw_prot_idx_write(rdc_sw_prot_t *p);
UINT8		rdc_sw_prot_noidx_read(rdc_sw_prot_t *p);
UINT8		rdc_sw_prot_noidx_write(rdc_sw_prot_t *p);
UINT8		rdc_sw_prot_info_read(rdc_sw_prot_t *prot);

UINT8 RDCSWProtocolPMC(rdc_sw_prot_t * prot);
UINT8 RDCSWProtocolGPIOStatusRead(rdc_sw_prot_t *prot, UINT8 flag);
UINT8 RDCSWProtocolGPIOStatusWrite(rdc_sw_prot_t *prot, UINT8 flag);
int RDCSWProtocolStatusReplaceIndexRead(rdc_sw_prot_t *prot, UINT8 *status);
int RDCSWProtocolStatusReplaceIndexWrite(rdc_sw_prot_t *prot, UINT8 *status, UINT8 type);

// ITE_FW protocols
// TODO
//UINT8	ite_fw_prot_read(ite_fw_prot_t *p);
//UINT8	ite_fw_prot_write(ite_fw_prot_t *p);
#if 0
UINT8		ite_read_acpi_ram(aec_port_t *port, UINT8 off, UINT8 *dat);
UINT8		ite_write_acpi_ram(aec_port_t *port, UINT8 off, UINT8 dat);
UINT8		ite_fw_prot_read(ite_fw_prot_t *p);
UINT8		ite_fw_prot_smb_read_byte(ite_fw_prot_t *prot);
UINT8		ite_fw_prot_smb_read_word(ite_fw_prot_t *prot);
UINT8		ite_fw_prot_smb_read_block(ite_fw_prot_t *prot);
#endif
#endif

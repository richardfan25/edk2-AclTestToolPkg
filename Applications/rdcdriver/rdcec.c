#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "typedef.h"
#include "aec.h"
//#include "io.h"
#include "rdcdriver.h"

//=============================================================================
//  EC_STS (66h)
//=============================================================================
//
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  | Bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  |Name | GPF | SMI | SCI |BURST| CMD | GPF2| IBF | OBF |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//         ALERT                          BUSY
//

#define ACPI_EC_CMD		0x66
#define ACPI_EC_STS		0x66
#define ACPI_EC_DAT		0x62

#define EC_CMD	0x2F6
#define EC_STS	EC_CMD
#define EC_DAT	0x2F2

//#define EC_CMD	0x66
//#define EC_STS	EC_CMD
//#define EC_DAT	0x62

#define EC_STS_OBF		0x1		// bit0
#define EC_STS_IBF		0x2		// bit1
#define EC_STS_IBF_OBF	0x3		// bit0/bit1
#define EC_STS_BUSY		0x4		// bit2
#define EC_STS_CMD		0x8		// bit3: 1=CMD,0=DAT
#define EC_STS_BURST	0x10	// bit4
#define EC_STS_SCI		0x20	// bit5
#define EC_STS_SMI		0x40	// bit6
#define EC_STS_ALERT	0x80	// bit7


#define EC_RETRIES	(1000000)

//typedef unsigned char	UINT8;
//typedef unsigned int	UINT16;
//typedef unsigned long	UINT32;

unsigned char	baseaddr = 0;

//UINT16		eccmd = 0x2f6;
//UINT16		ecdat = 0x2f2;

char	pmc_err_str[9][16] =
{
	"OK",
	"[0]cmd error!",
	"[1]ctl error!",
	"[2]dev error!",
	"[3]len error!",
	"[4]dat error!",
	"Protocol error!",
	"Read-Only!",
	"Write-Only!"
};

//=============================================================================
//  ec_wait_ibe
//=============================================================================
UINT8 ec_wait_ibe(void)
{
	UINT32	retries = EC_RETRIES;
	UINT8		sts;
	UINT8		alert = 0;

	do
	{
		sts = IoRead8(EC_STS);
		
		if ((sts & EC_STS_IBF)==0)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  ec_wait_obe
//=============================================================================
UINT8 ec_wait_obe(void)
{
	UINT32	retries = EC_RETRIES;
	UINT8		sts;
	UINT8		alert = 0;

	do
	{
		sts = IoRead8(EC_STS);
		
		if ((sts & EC_STS_OBF)==0)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

		IoRead8(EC_DAT);

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  ec_wait_obf
//=============================================================================
UINT8 ec_wait_obf(void)
{
	UINT32	retries = EC_RETRIES;
	UINT8		sts;
	UINT8		alert = 0;

	do
	{
		sts = IoRead8(EC_STS);
		
		if (sts & EC_STS_OBF)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  acpi_wait_ibe
//=============================================================================
UINT8 acpi_wait_ibe(void)
{
	UINT32	retries = EC_RETRIES;
	UINT8		sts;

	do
	{
		sts = IoRead8(ACPI_EC_STS);
		
		if ((sts & EC_STS_IBF) == 0)
			break;

		if (sts & EC_STS_ALERT)
		{
			retries = 0;
			break;
		}

	} while(retries--);

	if (retries)
		return 0;
	else
		return 0xFF;
}

//=============================================================================
//  acpi_wait_obf
//=============================================================================
UINT8 acpi_wait_obf(void)
{
	UINT32	retries = EC_RETRIES;
	UINT8		sts;

	do
	{
		sts = IoRead8(ACPI_EC_STS);
		
		if (sts & EC_STS_OBF)
			break;

		if (sts & EC_STS_ALERT)
		{
			retries = 0;
			break;
		}

	} while(retries--);

	if (retries)
		return 0;
	else
		return 0xFF;
}

//=============================================================================
//  ec_write_cmd
//=============================================================================
UINT8 ec_write_cmd(UINT8 cmd)
{
	//UINT32	retries = 50000;
	UINT8		res;

	// make sure data port is empty
	/*
	do
	{
		if ((inportb(EC_STS) & EC_STS_OBF) == 0)
			break;

		inportb(EC_DAT);

	} while(retries--);
	*/
	/*
	while (inportb(EC_STS) & EC_STS_OBF)
	{
		inportb(EC_DAT);
		retries--;
		if (retries == 0)
			break;
	}
	*/

	res = ec_wait_obe();
	if (!res)
	{
		res = ec_wait_ibe();
		if (!res)
			IoWrite8(EC_CMD, cmd);
	}

	return res;
}

//=============================================================================
//  ec_write_dat
//=============================================================================
UINT8 ec_write_dat(UINT8 dat)
{
	UINT8	res;

	res = ec_wait_ibe();
	if (!res)
		IoWrite8(EC_DAT, dat);

	return res;
}

//=============================================================================
//  ec_read_dat
//=============================================================================
UINT8 ec_read_dat(UINT8 *dat)
{
	UINT8	res;

	res = ec_wait_obf();
	if (!res)
		*dat = IoRead8(EC_DAT);

	return res;
}

//=============================================================================
//  rdc_ec_read_prot
//=============================================================================
UINT8 rdc_ec_read_prot(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *buf)
{
	rdc_fw_prot_t	prot;

	prot.cmd = cmd;
	prot.ctl = ctl;
	prot.dev = dev;
	prot.len = len;
	prot.dat = buf;

	return rdc_fw_prot_read(&prot);
}
	
//=============================================================================
//  rdc_ec_write_prot
//=============================================================================
UINT8 rdc_ec_write_prot(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *buf)
{
	rdc_fw_prot_t	prot;

	prot.cmd = cmd;
	prot.ctl = ctl;
	prot.dev = dev;
	prot.len = len;
	prot.dat = buf;

	return rdc_fw_prot_write(&prot);
}

//=============================================================================
//  rdc_ec_read_pmc_status
//=============================================================================
UINT8 rdc_ec_read_pmc_status(UINT8 cmd, UINT8 *buf)
{
	UINT8		res;

	res = ec_write_cmd(cmd);
	if (res)
		return res;
	
	res = ec_read_dat(buf);
	if (res)
		return res;

	return res;
}

//=============================================================================
//  rdc_sw_ec_read_prot
//=============================================================================
UINT8 rdc_sw_ec_read_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(index == 0xFF)
	{
		return rdc_sw_prot_noidx_read(&prot);
	}
	else
	{
		return rdc_sw_prot_idx_read(&prot);
	}
}
	
//=============================================================================
//  rdc_sw_ec_write_prot
//=============================================================================
UINT8 rdc_sw_ec_write_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(index == 0xFF)
	{
		return rdc_sw_prot_noidx_write(&prot);
	}
	else
	{
		return rdc_sw_prot_idx_write(&prot);
	}
}

//=============================================================================
//  rdc_sw_ec_read_info_prot
//=============================================================================
UINT8 rdc_sw_ec_read_info_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return rdc_sw_prot_info_read(&prot);
}

//=============================================================================
//  show_prot
//=============================================================================
int ProtocolStatus(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *dat)
{
	UINT8		res;
	
	rdc_fw_prot_t	prot;
	
	prot.cmd = cmd;
	prot.dat = dat;
	res = rdc_fw_prot_pmc(&prot);
	AEC_ERR_CHECK(res);

	return 0;
}

//=============================================================================
//  ShowSWProtocol
//=============================================================================
int SWProtocolStatus(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;
	UINT8 res;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(index == 0xFF)
	{
		res = rdc_sw_prot_noidx_read(&prot);
	}
	else
	{
		res = rdc_sw_prot_idx_read(&prot);
	}

	return 0;
}

UINT8 SWProtocolOnlyWritePMC(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return RDCSWProtocolPMC(&prot);
}

int StorageSWProtocolSetLockUnlock(UINT8 cmd, UINT8 *index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;
	int ret;

	prot.cmd = cmd;
	prot.idx = *index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	ret = RDCSWProtocolStatusReplaceIndexWrite(&prot, index, 0);
	
	return ret;
}

int SWRWProtocolStatusReplaceIndex(UINT8 cmd, UINT8 *index, UINT8 offset, UINT8 len, UINT8 *buf)
{
	rdc_sw_prot_t	prot;
	int ret;

	prot.cmd = cmd;
	prot.idx = *index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(cmd & 0x01)
	{
		ret = RDCSWProtocolStatusReplaceIndexRead(&prot, index);
		return ret;
	}
	else
	{
		ret = RDCSWProtocolStatusReplaceIndexWrite(&prot, index, 1);
		return ret;
	}
}

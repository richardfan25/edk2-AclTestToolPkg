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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __BORLANDC__
#include <dos.h>
#endif

#ifdef __WATCOMC__
#include <i86.h>
#include <conio.h>
#endif

#include "typedef.h"
#include "aec.h"
#include "x86io.h"
#include "acpi_ec.h"
#include "ite_ec.h"
#include "rdc_ec.h"

//=============================================================================
//  aec_probe_ene
//=============================================================================
static void aec_probe_ene(aec_t *aec)
{
	// TODO : no idea what to do
}

//=============================================================================
//  aec_probe_ite
//=============================================================================
static void aec_probe_ite(aec_t *aec)
{
	uint8_t			cid = aec->info.id;
	uint16_t		id;
	char			ver[32];
	uint8_t			res, tmp;
	ite_fw_prot_t	iprot;


	//-------------------------------------------------------------------------
	//  ITE IT8528/IT8518
	//-------------------------------------------------------------------------
	if (cid == 0x12 || cid == 0x16 || cid == 0x18 || cid == 0x28)
	{
		outb(ITE_CFG_IDX_PORT, 0x87);	// ITE : enter config
		outb(ITE_CFG_IDX_PORT, 0x87);

		outb(ITE_CFG_IDX_PORT, 0x20);	// ID0
		id = inb(ITE_CFG_DAT_PORT);
	
		outb(ITE_CFG_IDX_PORT, 0x21);	// ID1
		id <<= 8;
		id |= inb(ITE_CFG_DAT_PORT);

		// 8528 or 8518
		if (id == AEC_CHIP_ITE_IT8528 || id == AEC_CHIP_ITE_IT8518)
		{
			aec->info.chip_id = id;
	
			outb(ITE_CFG_IDX_PORT, 0x22);	// chip ver.
			aec->info.chip_ver = inb(ITE_CFG_DAT_PORT);

			outb(ITE_CFG_IDX_PORT, 0x23);	// chip ctl.
			aec->info.chip_ctl = inb(ITE_CFG_DAT_PORT);

			ite_ec_probe_port();

			aec->acl.cmd  = ITE_EC_CMD_PORT;
			aec->acl.sts  = ITE_EC_STS_PORT;
			aec->acl.dat  = ITE_EC_DAT_PORT;

			aec->cfg.cmd  = ITE_CFG_IDX_PORT;
			aec->cfg.sts  = ITE_CFG_IDX_PORT;
			aec->cfg.dat  = ITE_CFG_DAT_PORT;

			// AEC firmware type
			if (aec->info.vendor == 'I')
			{
				if (aec->info.id == 0x28)
					aec->type = AEC_TYPE_ITE_8528;
				else if (aec->info.id == 0x18)
					aec->type = AEC_TYPE_ITE_8518;
			}

			if (aec->type != AEC_TYPE_UNKNOWN)
			{
				outb(ITE_CFG_IDX_PORT, 0xAA);	// exit config
				return;							// no need to check RDC EC
			}

			// AEC firmware version
			iprot.cmd = 0xF0;	// get version
			iprot.len = 17;
			iprot.dat = ver;
			res = ite_ec_fw_prot_read(&iprot);
			if (res != ITE_EC_OK)
				sprintf(ver, "err! 0x%02X", res);
			else
				memcpy(&aec->fw.ec.ite.tab_code, &ver[8], 9);	// len=9
		}
		outb(ITE_CFG_IDX_PORT, 0xAA);	// ITE : exit config
	}
	//-------------------------------------------------------------------------
	//  ITE IT5121
	//-------------------------------------------------------------------------
	else if (cid == 0x21)
	{
		// TODO ITE IT5121
		outb(ITE_CFG_IDX_PORT, 0x87);	// ITE : enter config
		outb(ITE_CFG_IDX_PORT, 0x87);

		outb(ITE_CFG_IDX_PORT, 0x20);	// ID0
		id = inb(ITE_CFG_DAT_PORT);
	
		outb(ITE_CFG_IDX_PORT, 0x21);	// ID1
		id <<= 8;
		id |= inb(ITE_CFG_DAT_PORT);

		// 5121
		if (id == AEC_CHIP_ITE_IT5121)
		{
			aec->info.chip_id = id;
		
			outb(ITE_CFG_IDX_PORT, 0x22);	// chip ver.
			aec->info.chip_ver = inb(ITE_CFG_DAT_PORT);
		
			outb(ITE_CFG_IDX_PORT, 0x23);	// chip ctl.
			aec->info.chip_ctl = inb(ITE_CFG_DAT_PORT);

			ite_ec_probe_port();

			aec->acl.cmd  = ITE_CMD_PORT;
			aec->acl.sts  = ITE_STS_PORT;
			aec->acl.dat  = ITE_DAT_PORT;

			aec->cfg.cmd  = ITE_CFG_IDX_PORT;
			aec->cfg.sts  = ITE_CFG_IDX_PORT;
			aec->cfg.dat  = ITE_CFG_DAT_PORT;

			// AEC firmware type
			if (aec->info.vendor == 'I')
			{
				if (aec->info.id == 0x21)
					aec->type = AEC_TYPE_ITE_5121;
			}
		}
		outb(ITE_CFG_IDX_PORT, 0xAA);	// ITE : exit config
		
		// IT5121 initialization
		if (aec->type == AEC_TYPE_ITE_5121)
		{
			ite_mbox_write(ITE_MBOX_OFF_CMD, ITE_MBOX_CMD_CLR_BUF);

			ite_mbox_wait();

			#if 0	
			res = ite_mbox_read(ITE_MBOX_OFF_STS, &tmp);
			if (res != ITE_OK)
			{
				// error!
				// weird : ite_mbox_read always return OK
			}
			#endif

			ite_mbox_read(ITE_MBOX_OFF_STS, &tmp);
			if (tmp != 0x1)
			{
				ite_mbox_write(ITE_MBOX_OFF_CMD, ITE_MBOX_CMD_CLR_BUF);
				ite_mbox_wait();
			}
			
			#if 0
			res = ite_mbox_read(ITE_MBOX_OFF_STS, &tmp);
			if (res != ITE_OK || tmp != 0x1)
			{
				// error mail box : force to unknown type of EC
				aec->type = AEC_TYPE_UNKNOWN;
			}
			#endif

			ite_mbox_read(ITE_MBOX_OFF_STS, &tmp);
			if (tmp != 0x1)
			{
				// error mail box : force to unknown type of EC
				aec->type = AEC_TYPE_UNKNOWN;
			}
		}
	}
}

//=============================================================================
//  aec_probe_rdc
//=============================================================================
static void aec_probe_rdc(aec_t *aec)
{
	uint8_t			cid = aec->info.id;
	uint16_t		id;
	uint16_t		addr;
	uint8_t		res;
	char			ver[32];

	rdc_fw_prot_t	fprot;
	rdc_sw_prot_t	sprot;
	
	if (cid == 0x10 || cid == 0x20)
	{
		outb(RDC_CFG_IDX_PORT, 0x87);	// RDC : enter config
		outb(RDC_CFG_IDX_PORT, 0x87);

		outb(RDC_CFG_IDX_PORT, 0x20);	// ID0
		id = inb(RDC_CFG_DAT_PORT);

		outb(RDC_CFG_IDX_PORT, 0x21);	// ID1
		id <<= 8;
		id |= inb(RDC_CFG_DAT_PORT);

		// A9610 : EIO-IS200
		// A9620 : EIO-201, EIO-211
		if (id == AEC_CHIP_RDC_A9610 || id == AEC_CHIP_RDC_A9620)
		{
			aec->info.chip_id = id;
	
			// chip version
			outb(RDC_CFG_IDX_PORT, 0x22);
			aec->info.chip_ver = inb(RDC_CFG_DAT_PORT);

			// sioc ctrl
			outb(RDC_CFG_IDX_PORT, 0x23);
			aec->info.chip_ctl = inb(RDC_CFG_DAT_PORT);

			// RDC A9610 config port
			aec->cfg.cmd  = RDC_CFG_IDX_PORT;
			aec->cfg.sts  = RDC_CFG_IDX_PORT;
			aec->cfg.dat  = RDC_CFG_DAT_PORT;

			// PMC0 must be configured 62h/66h

			//----------------------------------------------
			// LDN : PMC1
			//----------------------------------------------
			outb(RDC_CFG_IDX_PORT, 0x07);
			outb(RDC_CFG_DAT_PORT, RDC_LDN_PMC1);

			// base
			outb(RDC_CFG_IDX_PORT, 0x60);
			addr = inb(RDC_CFG_DAT_PORT);
			outb(RDC_CFG_IDX_PORT, 0x61);
			addr <<= 8;
			addr |= inb(RDC_CFG_DAT_PORT);
			aec->acl.dat  = addr;

			outb(RDC_CFG_IDX_PORT, 0x62);
			addr = inb(RDC_CFG_DAT_PORT);
			outb(RDC_CFG_IDX_PORT, 0x63);
			addr <<= 8;
			addr |= inb(RDC_CFG_DAT_PORT);
			aec->acl.cmd  = addr;
			aec->acl.sts  = addr;

			// AEC firmware type
			if (aec->info.code == 0x00)
			{
				aec->type = AEC_TYPE_RDC_SW;	// ECG-SW

				// AEC firmware version
				sprot.cmd = RDC_SW_CMD_RD_FWVER;
				sprot.off = 0x00;
				sprot.len = 9;
				sprot.dat = ver;
				res = rdc_sw_prot_noidx_read(&sprot);
				if (res != RDC_EC_OK)
					strcpy(ver, "err!");
				else
					strcpy(aec->fw.ec.rdc.ver_str, ver);
			}
			else if (aec->info.code == 0x80)
			{
				aec->type = AEC_TYPE_RDC_FW;	// ECG-FW

				// AEC firmware version
				fprot.cmd = RDC_FW_CMD_RD_BOARD;
				fprot.ctl = 0x22;
				fprot.dev = 0;
				fprot.len = 16;
				fprot.dat = ver;
				res = rdc_fw_prot_read(&fprot);
				if (res != RDC_EC_OK)
					strcpy(ver, "err!");
				else
					strcpy(aec->fw.ec.rdc.ver_str, ver);
			}
		}

		outb(RDC_CFG_IDX_PORT, 0xAA);	// RDC : exit config
	}
}

//=============================================================================
//  aec_probe
//=============================================================================
static uint8_t aec_probe(aec_t *aec)
{
	uint8_t		res;
	uint8_t		data;


	aec->type = AEC_TYPE_UNKNOWN;

	//-------------------------------------------------------
	//  ACL EC identification
	//-------------------------------------------------------
	//  FAh : IC vendor
	//        49h='I' (ITE) IT8528, IT8518, IT8516, IT8512, IT5121
	//        45h='E' (ENE) KB3920
	//        52h='R' (RDC) A9610, A9620
	res = acpi_ec_read(0xFA, &data);
	if (res != ACPI_EC_OK)
		return AEC_ERR_ACPIEC;

	aec->info.vendor = data;

	//  FBh : IC code
	//        20h= ENE KB3920
	//        28h= ITE IT8528
	//        18h= ITE IT8518
	//        16h= ITE IT8516
	//        12h= ITE IT8512
	//        21h= ITE IT5121
	//        10h= RDC A9610
	//        20h= RDC A9620
	res = acpi_ec_read(0xFB, &data);
	if (res != ACPI_EC_OK)
		return AEC_ERR_ACPIEC;

	aec->info.id = data;

	//  FCh : project id (ITE/ENE)
	
	//  FCh : code base  (RDC)
	//        00=ECG-SW
	//        80=ECG-FW	
	res = acpi_ec_read(0xFC, &data);
	if (res != ACPI_EC_OK)
		return AEC_ERR_ACPIEC;

	aec->info.code = data;

	//---------------------------------------------------------------------------
	//  ENE KB3920
	//---------------------------------------------------------------------------
	//  IC vendor : 'E' = 0x45
	//  IC code   : 0x20
	//  Core      : 8051
	//  Clock     : 4/8/16 MHz
	//  RAM       : 128B + 2KB
	//---------------------------------------------------------------------------
	if (aec->info.vendor == 'E')
	{
		aec_probe_ene(aec);
	}

	//---------------------------------------------------------------------------
	//  ITE IT8390, IT8380
	//---------------------------------------------------------------------------
	//  There are newer EC than IT8528 (experimental)
	//
	//  IC vendor : 'I' = 0x49
	//  IC code   : 0x90 or 0x80
	//  Core      : Andes n801-S
	//	Clock     : 96MHz
	//---------------------------------------------------------------------------
	//  IT8390 : not verified
	//  IT8380 : not verified
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//  ITE IT8528, IT8518, IT8516, IT8512
	//---------------------------------------------------------------------------
	//  Clock : 9MHz
	//---------------------------------------------------------------------------
	//  IT8528 : verified
	//  IT8518 : verified
	//  IT8516 : not verified
	//  IT8512 : not verified
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//  ITE IT5121
	//---------------------------------------------------------------------------
	//  Clock : 80MHz
	//---------------------------------------------------------------------------
	//  IT5121 : to do verified
	//---------------------------------------------------------------------------
	else if (aec->info.vendor == 'I')
	{
		aec_probe_ite(aec);
	}

	//---------------------------------------------------------------------------
	//  RDC A9610 (aka EIO-IS200)
	//  RDC A9620 (aka EIO-201, EIO-211)
	//---------------------------------------------------------------------------
	else if (aec->info.vendor == 'R')
	{
		aec_probe_rdc(aec);
	}
			
	if (aec->type == AEC_TYPE_UNKNOWN)
		return AEC_ERR_FW_TYPE;
			
	return AEC_OK;
}

//=============================================================================
//  aec_get_type
//=============================================================================
uint16_t aec_get_type(aec_t *aec)
{
	return aec->type;
}

//=============================================================================
//  aec_get_chip_id
//=============================================================================
uint16_t aec_get_chip_id(aec_t *aec)
{
	return aec->info.chip_id;
}

//=============================================================================
//  aec_get_chip_ver
//=============================================================================
uint8_t aec_get_chip_ver(aec_t *aec)
{
	return aec->info.chip_ver;
}

//=============================================================================
//  aec_get_chip_ctl
//=============================================================================
uint8_t aec_get_chip_ctl(aec_t *aec)
{
	return aec->info.chip_ctl;
}

//=============================================================================
//  aec_get_vendor
//=============================================================================
uint8_t aec_get_vendor(aec_t *aec)
{
	return aec->info.vendor;
}
			
//=============================================================================
//  aec_get_id
//=============================================================================
uint8_t aec_get_id(aec_t *aec)
{
	return aec->info.id;
}

//=============================================================================
//  aec_get_code
//=============================================================================
uint8_t aec_get_code(aec_t *aec)
{
	return aec->info.code;
}

//=============================================================================
//  aec_init
//=============================================================================
aec_t *aec_init(void)
{
	aec_t	*aec;
	uint8_t	res;

	aec = (aec_t *)malloc(sizeof(aec_t));
	if (!aec)
		return NULL;	//AEC_ERR_MALLOC;

	memset(aec, 0, sizeof(aec_t));

	aec->acpi.cmd = ACPI_EC_CMD_PORT;
	aec->acpi.sts = ACPI_EC_STS_PORT;
	aec->acpi.dat = ACPI_EC_DAT_PORT;

	res = aec_probe(aec);
				
	if (res != AEC_OK)
	{
		// unknown AEC
		free(aec);
		return NULL;
	}

	return aec;
}

//=============================================================================
//  aec_exit
//=============================================================================
void aec_exit(aec_t *aec)
{
	if (aec)
	{
		memset(aec, 0, sizeof(aec_t));
		free(aec);
	}
}

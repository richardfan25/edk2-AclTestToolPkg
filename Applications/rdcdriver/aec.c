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

//#include "typedef.h"
//#include "io.h"
#include "aec.h"
#include "rdcdriver.h"

//#include "base/global.h"

//=============================================================================
//  variables
//=============================================================================
aec_t	aec;

//=============================================================================
//  aec_init
//=============================================================================
UINT8 aec_init(void)
{
	aec_t	*ec;
	UINT8	res;

	ec = &aec;
	gBS->SetMem(ec, sizeof(aec_t), 0);

	ec->acpi.cmd = ACPI_EC_CMD_PORT;
	ec->acpi.sts = ACPI_EC_STS_PORT;
	ec->acpi.dat = ACPI_EC_DAT_PORT;

	res = aec_probe();

	if (ec->type != AEC_TYPE_UNKNOWN)
		ec->flag |= AEC_FL_INIT;
	else
		ec->flag &= ~AEC_FL_INIT;

	return res;
}

//=============================================================================
//  aec_exit
//=============================================================================
void aec_exit(void)
{
	gBS->SetMem(&aec, sizeof(aec_t), 0);
}

//=============================================================================
//  ite_probe_port
//=============================================================================
UINT8	ite_probe_port(void)
{
	return 0;
}

//=============================================================================
//  aec_get_type
//=============================================================================
UINT16 aec_get_type(void)
{
	return aec.type;
}

//=============================================================================
//  aec_get_chip_id
//=============================================================================
UINT16 aec_get_chip_id(void)
{
	return aec.info.chip_id;
}

//=============================================================================
//  aec_get_chip_ver
//=============================================================================
UINT8 aec_get_chip_ver(void)
{
	return aec.info.chip_ver;
}

//=============================================================================
//  aec_get_chip_ctl
//=============================================================================
UINT8 aec_get_chip_ctl(void)
{
	return aec.info.chip_ctl;
}

//=============================================================================
//  aec_get_vendor
//=============================================================================
UINT8 aec_get_vendor(void)
{
	return aec.info.vendor;
}

//=============================================================================
//  aec_get_id
//=============================================================================
UINT8 aec_get_id(void)
{
	return aec.info.id;
}

//=============================================================================
//  aec_get_code
//=============================================================================
UINT8 aec_get_code(void)
{
	return aec.info.code;
}

//=============================================================================
//  aec_get_ec_ram
//=============================================================================
UINT8 aec_get_ec_ram(UINT8 *ram)
{
	UINT8 res;
	int		idx;
	
	for (idx=0; idx<256; idx++)
	{
		res = acpi_ec_read((UINT8)idx, &ram[idx]);
		AEC_ERR_CHECK(res);
	}

	return res;
}

//=============================================================================
//  aec_get_ldn_cfg
//=============================================================================
void aec_get_ldn_cfg(UINT8 ldn, UINT8 *cfg)
{
	UINT16	i;
	UINT8		idx;

	// enter config
	IoWrite8(aec.cfg.cmd, 0x87);
	IoWrite8(aec.cfg.cmd, 0x87);
	
	IoWrite8(aec.cfg.cmd, 0x07);	// select LDN
	IoWrite8(aec.cfg.dat, ldn);

	for (i=0, idx=0; i<256; i++, idx++)
	{
		IoWrite8(aec.cfg.cmd, idx);	// read PMC1 base
		cfg[i] = IoRead8(aec.cfg.dat);
	}
	
	IoWrite8(aec.cfg.cmd, 0xAA);	// exit config	
}

//=============================================================================
//  aec_wait_ibe
//=============================================================================
static UINT8 aec_wait_ibe(aec_port_t *port)
{
	UINT16	retries = AEC_RETRIES;

	// wait for the input buffer empty
	while (IoRead8(port->sts) & AEC_STS_IBF)
	{
		if (--retries == 0)
			return AEC_ERR_TMOUT_IBE;

	}

	return AEC_OK;
}

//=============================================================================
//  aec_wait_obe
//=============================================================================
static UINT8 aec_wait_obe(aec_port_t *port)
{
	UINT16	retries = AEC_RETRIES;

	// wait for the output buffer empty
	while (IoRead8(port->sts) & AEC_STS_OBF)
	{
		if (--retries == 0)
			return AEC_ERR_TMOUT_OBE;

		IoRead8(port->dat);	// reading output buffer if OBF
	}

	return AEC_OK;
}

//=============================================================================
//  aec_wait_obf
//=============================================================================
static UINT8 aec_wait_obf(aec_port_t *port)
{
	UINT16	retries = AEC_RETRIES;

	// wait for the output buffer full
	while ((IoRead8(port->sts) & AEC_STS_OBF) == 0)
	{
		if (--retries == 0)
			return AEC_ERR_TMOUT_OBF;
		
	}

	return AEC_OK;
}

//=============================================================================
//  aec_read_dat
//=============================================================================
UINT8 aec_read_dat(aec_port_t *port, UINT8 *dat)
{
	UINT8	res;

	res = aec_wait_obf(port);
	AEC_ERR_CHECK(res);

	*dat = (UINT8)IoRead8(port->dat);

	return AEC_OK;
}

//=============================================================================
//  aec_write_dat
//=============================================================================
UINT8 aec_write_dat(aec_port_t *port, UINT8 dat)
{
	UINT8	res;

	res = aec_wait_ibe(port);
	AEC_ERR_CHECK(res);

	IoWrite8(port->dat, dat);
	
	return AEC_OK;
}

//=============================================================================
//  aec_write_cmd
//=============================================================================
UINT8 aec_write_cmd(aec_port_t *port, UINT8 cmd)
{
	UINT8	res;

	res = aec_wait_obe(port);
	AEC_ERR_CHECK(res);
	
	res = aec_wait_ibe(port);
	AEC_ERR_CHECK(res);

	IoWrite8(port->cmd, cmd);

	return AEC_OK;
}

//=============================================================================
//  acpi_ec_read
//=============================================================================
UINT8	acpi_ec_read(UINT8 idx, UINT8 *datap)
{
	UINT8		res;
	aec_port_t	*acpi = &aec.acpi;

	res = aec_wait_ibe(acpi);
	AEC_ERR_CHECK(res);
	IoWrite8(acpi->cmd, ACPI_EC_CMD_READ);	// ACPI EC Read

	res = aec_wait_ibe(acpi);
	AEC_ERR_CHECK(res);
	IoWrite8(acpi->dat, idx);				// ACPI EC RAM index

	res = aec_wait_obf(acpi);
	AEC_ERR_CHECK(res);
	*datap = (UINT8)IoRead8(acpi->dat);

	return AEC_OK;
}

//=============================================================================
//  acpi_ec_write
//=============================================================================
UINT8	acpi_ec_write(UINT8 idx, UINT8 data)
{
	UINT8		res;
	aec_port_t	*acpi = &aec.acpi;

	res = aec_wait_ibe(acpi);
	AEC_ERR_CHECK(res);
	IoWrite8(acpi->cmd, ACPI_EC_CMD_WRITE);	// ACPI EC Write

	res = aec_wait_ibe(acpi);
	AEC_ERR_CHECK(res);
	IoWrite8(acpi->dat, idx);				// ACPI EC RAM index

	res = aec_wait_ibe(acpi);
	AEC_ERR_CHECK(res);
	IoWrite8(acpi->dat, data);				// ACPI EC RAM data

	return AEC_OK;
}

//=============================================================================
//  rdc_fw_prot_pmc
//=============================================================================
UINT8	rdc_fw_prot_pmc(rdc_fw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	prot->dat[0] = 0;
	
	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: dat reading
	res = aec_read_dat(acl, prot->dat);
	AEC_ERR_CHECK(res);

	return AEC_OK;
}

//=============================================================================
//  rdc_fw_prot_read
//=============================================================================
UINT8	rdc_fw_prot_read(rdc_fw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		*datap;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);
	
	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: ctl
	res = aec_write_dat(acl, prot->ctl);
	AEC_ERR_CHECK(res);

	// 3rd: dev
	if ((prot->cmd & 0xFE) == RDC_FW_CMD_WR_ECRAM)
	{
		// skip dev step for compliant old protocol of Susi
	}
	else
	{
		res = aec_write_dat(acl, prot->dev);
		AEC_ERR_CHECK(res);
	}

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);
	
	
	datap = prot->dat;	// point to data buffer

	// 5th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = aec_read_dat(acl, datap++);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_fw_prot_write
//=============================================================================
UINT8	rdc_fw_prot_write(rdc_fw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: ctl
	res = aec_write_dat(acl, prot->ctl);
	AEC_ERR_CHECK(res);
	
	// 3rd: dev
	if ((prot->cmd & 0xFE) == RDC_FW_CMD_WR_ECRAM)
	{
		// skip dev step for compliant old protocol of Susi
	}
	else
	{
		res = aec_write_dat(acl, prot->dev);
		AEC_ERR_CHECK(res);
	}

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 5th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = aec_write_dat(acl, prot->dat[i]);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_sw_prot_idx_read
//=============================================================================
UINT8	rdc_sw_prot_idx_read(rdc_sw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		*datap;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: idx
	res = aec_write_dat(acl, prot->idx);
	AEC_ERR_CHECK(res);

	// 3rd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 5th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = aec_read_dat(acl, datap++);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_sw_prot_idx_write
//=============================================================================
UINT8	rdc_sw_prot_idx_write(rdc_sw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: idx
	res = aec_write_dat(acl, prot->idx);
	AEC_ERR_CHECK(res);
	
	// 3rd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 5th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = aec_write_dat(acl, prot->dat[i]);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_sw_prot_noidx_read
//=============================================================================
UINT8	rdc_sw_prot_noidx_read(rdc_sw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		*datap;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 3rd: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 4th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = aec_read_dat(acl, datap++);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_sw_prot_noidx_write
//=============================================================================
UINT8	rdc_sw_prot_noidx_write(rdc_sw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 3rd: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 4th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = aec_write_dat(acl, prot->dat[i]);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  rdc_sw_prot_info_read
//=============================================================================
UINT8	rdc_sw_prot_info_read(rdc_sw_prot_t *prot)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		*datap;
	UINT8		sts;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: sts
	res = aec_read_dat(acl, &sts);
	AEC_ERR_CHECK(res);
	// todo : checking available and un-busy
	if(sts != 0x01)
	{
		return AEC_ERR_INIT;
	}

	// 3rd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 4th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = aec_read_dat(acl, datap++);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  RDCSWProtocolPMC
//=============================================================================
UINT8 RDCSWProtocolPMC(rdc_sw_prot_t * prot)
{
	aec_port_t	*acl;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	return AEC_OK;
}

//=============================================================================
//  RDCSWProtocolGPIOStatusRead
//=============================================================================
UINT8 RDCSWProtocolGPIOStatusRead(rdc_sw_prot_t *prot, UINT8 flag)
{
	aec_port_t	*acl;
	UINT8		res;
	UINT8		*datap;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: Group
	res = aec_write_dat(acl, prot->idx);
	AEC_ERR_CHECK(res);

	// 3rd: Port
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: Mask
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 5th: flag
	res = aec_write_dat(acl, flag);
	AEC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 6th: dat reading
	res = aec_read_dat(acl, datap);
	AEC_ERR_CHECK(res);

	return AEC_OK;
}

//=============================================================================
//  RDCSWProtocolGPIOStatusWrite
//=============================================================================
UINT8 RDCSWProtocolGPIOStatusWrite(rdc_sw_prot_t *prot, UINT8 flag)
{
	aec_port_t	*acl;
	UINT8		res;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: Group
	res = aec_write_dat(acl, prot->idx);
	AEC_ERR_CHECK(res);

	// 3rd: Port
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: Mask
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 5th: flag
	res = aec_write_dat(acl, flag);
	AEC_ERR_CHECK(res);

	// 6th: dat writing
	res = aec_write_dat(acl, prot->dat[0]);
	AEC_ERR_CHECK(res);

	return AEC_OK;
}

//=============================================================================
//  RDCSWProtocolStorageRead
//=============================================================================
int RDCSWProtocolStatusReplaceIndexRead(rdc_sw_prot_t *prot, UINT8 *status)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		*datap;
	UINT8		sts;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	gBS->SetMem(prot->dat, prot->len, 0);

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: sts
	res = aec_read_dat(acl, &sts);
	AEC_ERR_CHECK(res);
	*status = sts;
	// todo : checking available
	/*if((sts & 0x01) == 0x00)
	{
		return -1;
	}*/
	// todo : checking busy
	/*if((sts & 0x02) == 0x02)
	{
		return -2;
	}*/

	// 3rd: off
	res = aec_write_dat(acl, prot->off);
	AEC_ERR_CHECK(res);

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 4th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = aec_read_dat(acl, datap++);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

//=============================================================================
//  RDCSWProtocolStorageWrite
//=============================================================================
int RDCSWProtocolStatusReplaceIndexWrite(rdc_sw_prot_t *prot, UINT8 *status, UINT8 type)
{
	aec_port_t	*acl;
	UINT8		i;
	UINT8		res;
	UINT8		sts;

	if (aec.flag & AEC_FL_INIT)
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;

	// 1st: cmd
	res = aec_write_cmd(acl, prot->cmd);
	AEC_ERR_CHECK(res);

	// 2nd: sts
	res = aec_read_dat(acl, &sts);
	AEC_ERR_CHECK(res);
	*status = sts;
	// todo : checking available
	/*if((sts & 0x01) == 0x00)
	{
		return sts;
	}*/
	// todo : checking busy
	/*if((sts & 0x02) == 0x02)
	{
		return sts;
	}*/

	//if set lock/unlock
	if(type)
	{
		// 3rd: off
		res = aec_write_dat(acl, prot->off);
		AEC_ERR_CHECK(res);
	}

	// 4th: len
	res = aec_write_dat(acl, prot->len);
	AEC_ERR_CHECK(res);

	// 4th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = aec_write_dat(acl, prot->dat[i]);
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}
#if 0
//=============================================================================
//  ite_fw_prot_read
//=============================================================================
// TODO

//=============================================================================
//  ite_fw_prot_write
//=============================================================================
// TODO

//#############################################################################
//#############################################################################
//
//   ITE EC IT8528/IT8518
//
//#############################################################################
//#############################################################################

//=============================================================================
//  ite_wait_ibe (aec_wait_ibf)
//=============================================================================
static UINT8 ite_wait_ibe(aec_port_t *port)
{
	UINT16	retries = ITE_RETRIES;
	UINT8		res = AEC_ERR_TMOUT_IBE;

	// wait for the input buffer empty
	while(retries--)
	{
		if ((IoRead8(port->sts) & AEC_STS_IBF) == 0)
		{
			res = AEC_OK;
			break;
		}
		delay(1);
	}

	return res;
}

//=============================================================================
//  ite_wait_obf (aec_wait_obf)
//=============================================================================
static UINT8 ite_wait_obf(aec_port_t *port)
{
	UINT16	retries = ITE_RETRIES;
	UINT8		res = AEC_ERR_TMOUT_OBF;

	// wait for the output buffer full
	while (retries--)
	{
		if (IoRead8(port->sts) & AEC_STS_OBF)
		{
			res = AEC_OK;
			break;
		}
		delay(1);
	}

	return res;
}

//=============================================================================
//  ite_read_acpi_ram (aec_acpi_ram_read)
//=============================================================================
UINT8 ite_read_acpi_ram(aec_port_t *port, UINT8 off, UINT8 *dat)
{
	UINT8		res = AEC_OK;

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// clear ec_obf
	IoRead8(port->dat);

	IoWrite8(port->cmd, 0x80);		// 0x80:AEC_CMD_ACPI_READ

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	IoWrite8(port->dat, off);

	res = ite_wait_obf(port);
	AEC_ERR_CHECK(res);

	*dat = IoRead8(port->dat);

	return res;
}

//=============================================================================
//  ite_write_acpi_ram (aec_acpi_ram_write)
//=============================================================================
UINT8 ite_write_acpi_ram(aec_port_t *port, UINT8 off, UINT8 dat)
{
	UINT8		res = AEC_OK;

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// clear ec_obf
	IoRead8(port->dat);

	IoWrite8(port->cmd, 0x81);	// 0x81:AEC_CMD_ACPI_WRITE

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// offset
	IoWrite8(port->dat, off);

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// data
	IoWrite8(port->dat, dat);

	return res;
}

//=============================================================================
//  ite_read_mbox (aec_mbox_read)
//=============================================================================
static UINT8 ite_read_mbox(aec_port_t *port, UINT8 off, UINT8 *dat)
{
	UINT8 res = AEC_OK;

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// clear ec_obf
	IoRead8(port->dat);

	// offset : issue cmd
	IoWrite8(port->cmd, 0xA0+off);	// A0h=AEC_CMD_ADT_READ+offset

	res = ite_wait_obf(port);
	AEC_ERR_CHECK(res);

	// data
	*dat = IoRead8(port->dat);

	return res;
}

//=============================================================================
//  ite_write_mbox (aec_mbox_write)
//=============================================================================
static UINT8 ite_write_mbox(aec_port_t *port, UINT8 off, UINT8 dat)
{
	UINT8 res = AEC_OK;

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	// clear ec_obf
	IoRead8(port->dat);

	// offset : issue cmd
	IoWrite8(port->cmd, 0x50+off);	// 50h=AEC_CMD_ADT_WRITE+offset

	res = ite_wait_ibe(port);
	AEC_ERR_CHECK(res);

	IoWrite8(port->dat, dat);

	return AEC_OK;
}

//=============================================================================
//  ite_wait_cmd_done (aec_wait_cmd_clear)
//=============================================================================
static UINT8	ite_wait_cmd_done(aec_port_t *port)
{
	UINT8		res;
	UINT8		data;
	UINT16	retries = 1000;

	while (retries--)
	{
		// 00h:AEC_MBOX_CMD
		res = ite_read_mbox(port, 0x00, &data);

		if ((res == AEC_OK) && (data == 0))
			return AEC_OK;
		
		delay(1);
	}

	return AEC_ERR_TMOUT_MBOX;
}

//=============================================================================
//  ite_clear_mbox
//=============================================================================
static UINT8 ite_clear_mbox(aec_port_t *port)
{
	UINT8		res;

	res = ite_wait_cmd_done(port);
	AEC_ERR_CHECK(res);

	// 00h:AEC_MBOX_CMD
	// FFh:AEC_CMD_MBOX_CLEAR
	res = ite_write_mbox(port, 0x00, 0xFF);
	AEC_ERR_CHECK(res);

	delay(10);

	return AEC_OK;
}

//=============================================================================
//  ite_fw_prot_read
//=============================================================================
//  ite_fw_prot_read
//    aec_mbox_xx
// 
//=============================================================================
UINT8 ite_fw_prot_read(ite_fw_prot_t *prot)
{
	aec_port_t	*acl;

	UINT8		res;
	UINT8		data;
	int			i;

	if (TST_BIT_MASK(aec.flag, AEC_FL_INIT))
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;
	
	res = ite_clear_mbox(acl);
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	//cmd=0xF0 : AEC_CMD_MBOX_VER
	res = ite_write_mbox(acl, 0x00, prot->cmd);
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	// aec_mbox_read(AEC_MBOX_STA) //
	res = ite_read_mbox(acl, 0x01, &data);	// 01h:AEC_MBOX_STA
	AEC_ERR_CHECK(res);
	// todo : need to be checking data ?

	for (i=0; i<prot->len; i++)
	{
		res = ite_read_mbox(acl, 0x03+i, &data);	// 03h:AEC_MBOX_DAT
		prot->dat[i] = (UINT8)data;
		AEC_ERR_CHECK(res);
	}

	return AEC_OK;
}

UINT8 ite_fw_prot_smb_read_byte(ite_fw_prot_t *prot)
{
	aec_port_t	*acl;

	UINT8		res;
	UINT8		data;

	if (TST_BIT_MASK(aec.flag, AEC_FL_INIT))
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;
	
	res = ite_clear_mbox(acl);
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	// didx
	res = ite_write_mbox(acl, 0x02, prot->didx);	// 0x02=AEC_MBOX_PAR
	AEC_ERR_CHECK(res);
	
	// dadd
	res = ite_write_mbox(acl, 0x03, prot->dadd);	// 0x03=AEC_MBOX_DAT
	AEC_ERR_CHECK(res);

	// dcmd
	res = ite_write_mbox(acl, 0x03+1, prot->dcmd);	// 0x04=AEC_MBOX_DAT+1
	AEC_ERR_CHECK(res);
	
// read byte

	// cmd
	res = ite_write_mbox(acl, 0x00, prot->cmd);		// 0x00=AEC_MBOX_CMD
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	res = ite_read_mbox(acl, 0x01, &data);	// 01h:AEC_MBOX_STA
	AEC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return AEC_OK;	// ?

// read byte
	res = ite_read_mbox(acl, 0x03+4, &data);	// 07h:AEC_MBOX_DAT+4
	AEC_ERR_CHECK(res);
	prot->dat[0] = (UINT8)data;

	return AEC_OK;
}

UINT8 ite_fw_prot_smb_read_word(ite_fw_prot_t *prot)
{
	aec_port_t	*acl;

	UINT8		res;
	UINT8		data;
	//int			i;

	if (TST_BIT_MASK(aec.flag, AEC_FL_INIT))
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;
	
	res = ite_clear_mbox(acl);
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	// didx
	res = ite_write_mbox(acl, 0x02, prot->didx);	// 0x02=AEC_MBOX_PAR
	AEC_ERR_CHECK(res);
	
	// dadd
	res = ite_write_mbox(acl, 0x03, prot->dadd);	// 0x03=AEC_MBOX_DAT
	AEC_ERR_CHECK(res);

	// dcmd
	res = ite_write_mbox(acl, 0x03+1, prot->dcmd);	// 0x04=AEC_MBOX_DAT+1
	AEC_ERR_CHECK(res);
	
// read word

	// cmd
	res = ite_write_mbox(acl, 0x00, prot->cmd);		// 0x00=AEC_MBOX_CMD
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	res = ite_read_mbox(acl, 0x01, &data);	// 01h:AEC_MBOX_STA
	AEC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return AEC_OK;	// ?

// read word
	res = ite_read_mbox(acl, 0x03+4, &data);	// 07h:AEC_MBOX_DAT+4
	AEC_ERR_CHECK(res);
	prot->dat[0] = (UINT8)data;

	res = ite_read_mbox(acl, 0x03+5, &data);	// 07h:AEC_MBOX_DAT+5
	AEC_ERR_CHECK(res);
	prot->dat[1] = (UINT8)data;

	return AEC_OK;
}

UINT8 ite_fw_prot_smb_read_block(ite_fw_prot_t *prot)
{
	aec_port_t	*acl;

	UINT8		res;
	UINT8		data;
	int			i;

	if (TST_BIT_MASK(aec.flag, AEC_FL_INIT))
		acl = &aec.acl;
	else
		return AEC_ERR_INIT;
	
	res = ite_clear_mbox(acl);
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	// didx
	res = ite_write_mbox(acl, 0x02, prot->didx);	// 0x02=AEC_MBOX_PAR
	AEC_ERR_CHECK(res);
	
	// dadd
	res = ite_write_mbox(acl, 0x03, prot->dadd);	// 0x03=AEC_MBOX_DAT
	AEC_ERR_CHECK(res);

	// dcmd
	res = ite_write_mbox(acl, 0x03+1, prot->dcmd);	// 0x04=AEC_MBOX_DAT+1
	AEC_ERR_CHECK(res);
	
// read block

	// cmd
	res = ite_write_mbox(acl, 0x00, prot->cmd);		// 0x00=AEC_MBOX_CMD
	AEC_ERR_CHECK(res);

	res = ite_wait_cmd_done(acl);
	AEC_ERR_CHECK(res);

	res = ite_read_mbox(acl, 0x01, &data);	// 01h:AEC_MBOX_STA
	AEC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return AEC_OK;	// ?

// read block
	for (i=0; i<16; i++)
	{
		res = ite_read_mbox(acl, 0x03+4+i, &data);	// 07h:AEC_MBOX_DAT+4
		AEC_ERR_CHECK(res);
		prot->dat[i] = (UINT8)data;
	}

	return AEC_OK;
}
#endif
//=============================================================================
//  aec_probe
//=============================================================================
UINT8 aec_probe(void)
{
	UINT16	id;
	UINT8		res;
	UINT16	addr;
	int			ret;

	rdc_fw_prot_t	fprot;
	rdc_sw_prot_t	sprot;
//	ite_fw_prot_t	iprot;
	char			ver[32];

	aec.type = AEC_TYPE_UNKNOWN;

	//-------------------------------------------------------
	//  ACL EC identification
	//-------------------------------------------------------
	//  FAh : IC vendor
	//        49h='I' (ITE) IT8518, IT8528
	//        45h='E' (ENE)
	//        52h='R' (RDC) A9610
	res = acpi_ec_read(0xFA, &aec.info.vendor);
	AEC_ERR_CHECK(res);

	//  FBh : IC code
	//        28h= ITE IT8528
	//        18h= ITE IT8518
	//        10h= RDC A9610
	res = acpi_ec_read(0xFB, &aec.info.id);
	AEC_ERR_CHECK(res);

	//  FCh : project id (ITE)
	
	//  FCh : code base  (RDC)
	//        00=ECG-SW
	//        80=ECG-FW	
	res = acpi_ec_read(0xFC, &aec.info.code);
	AEC_ERR_CHECK(res);

	//---------------------------------------------------------------------------
	//  ITE IT8528, IT8518
	//---------------------------------------------------------------------------
	if ((aec.info.vendor == 'I') && (aec.info.id == 0x18 || aec.info.id == 0x28))
	{
		//-------------------------------------------------------
		//  ITE IT8528
		//-------------------------------------------------------
		//  LDN : 01h ~ 19h (Logical Device Number)
		//
		//  LDN  Devices (15 devices)
		//--------------------------------------------------------
		//  01h  Serial Port 1 (UART1)
		//  02h  Serial Port 2 (UART2)
		//  04h  System Wake-Up Control (SWUC)
		//  05h  KBC/Mouse Interface
		//  06h  KBC/Keyboard Interface
		//  0Ah  Consumer IR (CIR)
		//  0Fh  Shared Memory/Flash Interface (SMFI)
		//  10h  RTC-like Timer (RTCT)
		//  11h  Power Management I/F Channel 1 (PMC1)
		//  12h  Power Management I/F Channel 2 (PMC2)
		//  13h  Serial Peripheral Interface (SSPI)
		//  14h  Platform Environment Control Interface (PECI)
		//  17h  Power Management I/F Channel 3 (PMC3)
		//  18h  Power Management I/F Channel 4 (PMC4)
		//  19h  Power Management I/F Channel 5 (PMC5)
		//-------------------------------------------------------
		IoWrite8(ITE_CFG_IDX_PORT, 0x87);	// ITE : enter config
		IoWrite8(ITE_CFG_IDX_PORT, 0x87);

		IoWrite8(ITE_CFG_IDX_PORT, 0x20);	// ID0
		id = (UINT16)IoRead8(ITE_CFG_DAT_PORT);
		
		IoWrite8(ITE_CFG_IDX_PORT, 0x21);	// ID1
		id <<= 8;
		id |= IoRead8(ITE_CFG_DAT_PORT);

		// 8528 or 8518
		if (id == 0x8528 || id == 0x8518)
		{
			aec.info.chip_id = id;
			
			IoWrite8(ITE_CFG_IDX_PORT, 0x22);	// chip ver.
			aec.info.chip_ver = IoRead8(ITE_CFG_DAT_PORT);
			
			IoWrite8(ITE_CFG_IDX_PORT, 0x23);	// chip ctl.
			aec.info.chip_ctl = IoRead8(ITE_CFG_DAT_PORT);

			ite_probe_port();

			aec.acl.cmd  = ITE_EC_CMD_PORT;
			aec.acl.sts  = ITE_EC_STS_PORT;
			aec.acl.dat  = ITE_EC_DAT_PORT;

			aec.cfg.cmd  = ITE_CFG_IDX_PORT;
			aec.cfg.sts  = ITE_CFG_IDX_PORT;
			aec.cfg.dat  = ITE_CFG_DAT_PORT;

			// AEC firmware type
			if (aec.info.vendor == 'I')
			{
				if (aec.info.id == 0x28)
					aec.type = AEC_TYPE_ITE_8528;
				else if (aec.info.id == 0x18)
					aec.type = AEC_TYPE_ITE_8518;
			}

			if (aec.type != AEC_TYPE_UNKNOWN)
			{
				IoWrite8(ITE_CFG_IDX_PORT, 0xAA);	// exit config
				return AEC_OK;	// no need to check RDC EC
			}
			
			// AEC firmware version
			/*iprot.cmd = 0xF0;	// get version
			iprot.len = 17;
			iprot.dat = ver;
			ret = ite_fw_prot_read(&iprot);
			if (ret != AEC_OK)
				sprintf(ver, "err! 0x%02X", ret);
			else
				memcpy(&aec.fw.tab_code, &ver[8], 9);	// len=9*/
		}
		IoWrite8(ITE_CFG_IDX_PORT, 0xAA);	// ITE : exit config
	}
	//---------------------------------------------------------------------------
	//  RDC A9610 (aka EIO-IS200)
	//---------------------------------------------------------------------------
	else if ((aec.info.vendor == 'R') && (aec.info.id == 0x10))
	{
		//-------------------------------------------------------
		//  RDC A9610
		//-------------------------------------------------------	
		//  LDN : 02h ~ 24h (Logical Device Number)
		//
		//  LDN  Devices (21 devices)
		//--------------------------------------------------------
		//  02h  UART0 (COM1)
		//  03h  UART1 (COM2)
		//  04h  UART2 (COM3)
		//  05h  UART3 (COM4)
		//  06h  UART4 (COM5)
		//  07h  UART5 (COM6)
		//  08h  UART6 (COM7)
		//  09h  UART7 (COM8)
		//  0Ah  LPT
		//  0Bh  KBC
		//  0Ch  PMC0  (62h/66h)
		//  0Dh  PMC1  (299h/29Ah)
		//  0Eh  PMCMB
		//  0Fh  ECIO
		//  18h  CAN0
		//  19h  CAN1
		//  20h  I2C0
		//  21h  I2C1
		//  22h  SMB0 (SMBus)
		//  23h  SMB1 (SMBus)
		//  24h  GPIO0
		//-------------------------------------------------------
		IoWrite8(RDC_CFG_IDX_PORT, 0x87);	// RDC : enter config
		IoWrite8(RDC_CFG_IDX_PORT, 0x87);

		IoWrite8(RDC_CFG_IDX_PORT, 0x20);	// ID0
		id = IoRead8(RDC_CFG_DAT_PORT);
		
		IoWrite8(RDC_CFG_IDX_PORT, 0x21);	// ID1
		id <<= 8;
		id |= IoRead8(RDC_CFG_DAT_PORT);

		// A9610
		if (id == 0x9610)
		{
			aec.info.chip_id = id;
			
			// chip version
			IoWrite8(RDC_CFG_IDX_PORT, 0x22);
			aec.info.chip_ver = IoRead8(RDC_CFG_DAT_PORT);
			
			// sioc ctrl
			IoWrite8(RDC_CFG_IDX_PORT, 0x23);
			aec.info.chip_ctl = IoRead8(RDC_CFG_DAT_PORT);

			// RDC A9610 config port
			aec.cfg.cmd  = RDC_CFG_IDX_PORT;
			aec.cfg.sts  = RDC_CFG_IDX_PORT;
			aec.cfg.dat  = RDC_CFG_DAT_PORT;
			
			// PMC0 must be configured 62h/66h

			//----------------------------------------------
			// LDN : PMC1
			//----------------------------------------------
			IoWrite8(RDC_CFG_IDX_PORT, 0x07);
			IoWrite8(RDC_CFG_DAT_PORT, RDC_LDN_PMC1);

			// base
			IoWrite8(RDC_CFG_IDX_PORT, 0x60);
			addr = IoRead8(RDC_CFG_DAT_PORT);
			IoWrite8(RDC_CFG_IDX_PORT, 0x61);
			addr <<= 8;
			addr |= IoRead8(RDC_CFG_DAT_PORT);
			aec.acl.dat  = addr;

			IoWrite8(RDC_CFG_IDX_PORT, 0x62);
			addr = IoRead8(RDC_CFG_DAT_PORT);
			IoWrite8(RDC_CFG_IDX_PORT, 0x63);
			addr <<= 8;
			addr |= IoRead8(RDC_CFG_DAT_PORT);
			aec.acl.cmd  = addr;
			aec.acl.sts  = addr;

			// AEC firmware type
			if (aec.info.code == 0x00)
			{
				aec.type = AEC_TYPE_RDC_SW;	// ECG-SW
				
				// AEC firmware version
				sprot.cmd = RDC_SW_CMD_RD_FWVER;
				sprot.off = 0x00;
				sprot.len = 9;
				sprot.dat = ver;
				ret = rdc_sw_prot_noidx_read(&sprot);
				if (ret != AEC_OK)
				{
					//strcpy(ver, "err!");
					ver[0] = 'e';
					ver[1] = 'r';
					ver[2] = 'r';
					ver[3] = '!';
				}
				else
				{
					//strcpy(aec.fw.ec.rdc.ver_str, ver);
					aec.fw.ec.rdc.ver_str = ver;
				}
			}
			else if (aec.info.code == 0x80)
			{
				aec.type = AEC_TYPE_RDC_FW;	// ECG-FW

				// AEC firmware version
				fprot.cmd = RDC_FW_CMD_RD_BOARD;
				fprot.ctl = 0x22;
				fprot.dev = 0;
				fprot.len = 16;
				fprot.dat = ver;
				ret = rdc_fw_prot_read(&fprot);
				if (ret != AEC_OK)
				{
					//strcpy(ver, "err!");
					ver[0] = 'e';
					ver[1] = 'r';
					ver[2] = 'r';
					ver[3] = '!';
				}
				else
				{
					//strcpy(aec.fw.ec.rdc.ver_str, ver);
					aec.fw.ec.rdc.ver_str = ver;
				}
			}
		}

		IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC : exit config
	}

	if (aec.type == AEC_TYPE_UNKNOWN)
	{
		return AEC_ERR_FW_TYPE;
	}

	return AEC_OK;
}

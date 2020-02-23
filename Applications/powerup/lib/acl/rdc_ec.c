//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - RDC A9610 Embedded Controller                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <string.h>

#include "typedef.h"
#include "x86io.h"
#include "rdc_ec.h"

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - Configuration                                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  rdc_ec_get_ldn_cfg
//=============================================================================
void rdc_ec_get_ldn_cfg(uint8_t ldn, uint8_t *cfg)
{
	uint16_t	i;
	uint8_t		idx;
	
	outb(RDC_CFG_IDX_PORT, 0x87);		// enter config
	outb(RDC_CFG_IDX_PORT, 0x87);

	//-------------------------------------------------------
	//  RDC A9610/A9620
	//-------------------------------------------------------	
	//  LDN : 02h ~ 24h (Logical Device Number) : 21 devices
	//
	//  LDN  Devices       Base~End   Len  IRQ
	//--------------------------------------------------------
	//  02h  UART0 (COM1)  3F8h~3FFh  8    4
	//  03h  UART1 (COM2)  2F8h~2FFh  8    3
	//  04h  UART2 (COM3)  3E8h~3EFh  8    10
	//  05h  UART3 (COM4)  2E8h~2EFh  8    10
	//  06h  UART4 (COM5)  C80h~C87h  8    4
	//  07h  UART5 (COM6)  C88h~C8Fh  8    3
	//  08h  UART6 (COM7)  C90h~C97h  8    11
	//  09h  UART7 (COM8)  C98h~C9Fh  8    10
	//  0Ah  LPT           3BCh~37Fh  8    7
	//  0Bh  KBC           60h/64h    2    1
	//  0Ch  PMC0          62h/66h    2    -
	//  0Dh  PMC1          2F2h/2F6h  2    -
	//  0Eh  PMCMB         29Eh~29Fh  2    -
	//  0Fh  ECIO          290h~297h  8    -  1-byte, weird ? conflict I2C1
	//  10h  UART8 (DBG?)  CA0h~CA7h  8    4
	//  18h  CAN0          200h~27Fh  128  6
	//  19h  CAN1          300h~37Fh  128  6
	//  20h  I2C0          280h~28Fh  16   6  9-bytes size
	//  21h  I2C1          290h~29Fh  16   6  ?
	//  22h  SMB0          2A0h~2BFh  32   6  22-bytes size
	//  23h  SMB1          ?          32 
	//  24h  GPIO0         2C0h~2D0h  17   6
	//-------------------------------------------------------
	outb(RDC_CFG_IDX_PORT, 0x07);		// select LDN
	outb(RDC_CFG_DAT_PORT, ldn);

	for (i=0, idx=0; i<256; i++, idx++)
	{
		outb(RDC_CFG_IDX_PORT, idx);	// select offset
		cfg[i] = inb(RDC_CFG_DAT_PORT);
	}
	
	outb(RDC_CFG_IDX_PORT, 0xAA);		// exit config	
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - Common                                                          *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  rdc_ec_wait_ibe
//=============================================================================
static uint8_t rdc_ec_wait_ibe(void)
{
	uint16_t	retries = RDC_EC_RETRIES;

	// wait for the input buffer empty
	while (inb(RDC_EC_STS_PORT) & RDC_EC_STS_IBF)
	{
		if (--retries == 0)
			return RDC_EC_ERR_TMOUT_IBE;

	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_ec_wait_obe
//=============================================================================
static uint8_t rdc_ec_wait_obe(void)
{
	uint16_t	retries = RDC_EC_RETRIES;

	// wait for the output buffer empty
	while (inb(RDC_EC_STS_PORT) & RDC_EC_STS_OBF)
	{
		if (--retries == 0)
			return RDC_EC_ERR_TMOUT_OBE;

		inb(RDC_EC_DAT_PORT);	// reading output buffer if OBF
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_ec_wait_obf
//=============================================================================
static uint8_t rdc_ec_wait_obf(void)
{
	uint16_t	retries = RDC_EC_RETRIES;

	// wait for the output buffer full
	while ((inb(RDC_EC_STS_PORT) & RDC_EC_STS_OBF) == 0)
	{
		if (--retries == 0)
			return RDC_EC_ERR_TMOUT_OBF;
		
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_ec_read_dat
//=============================================================================
static uint8_t rdc_ec_read_dat(uint8_t *dat)
{
	uint8_t	res;

	res = rdc_ec_wait_obf();
	RDC_EC_ERR_CHECK(res);

	*dat = inb(RDC_EC_DAT_PORT);

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_ec_write_dat
//=============================================================================
static uint8_t rdc_ec_write_dat(uint8_t dat)
{
	uint8_t	res;

	res = rdc_ec_wait_ibe();
	RDC_EC_ERR_CHECK(res);

	outb(RDC_EC_DAT_PORT, dat);
	
	return RDC_EC_OK;
}

//=============================================================================
//  rdc_ec_write_cmd
//=============================================================================
static uint8_t rdc_ec_write_cmd(uint8_t cmd)
{
	uint8_t	res;

	res = rdc_ec_wait_obe();
	RDC_EC_ERR_CHECK(res);
	
	res = rdc_ec_wait_ibe();
	RDC_EC_ERR_CHECK(res);

	outb(RDC_EC_CMD_PORT, cmd);

	return RDC_EC_OK;
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - F/W team protocol                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  rdc_fw_prot_pmc
//=============================================================================
uint8_t	rdc_fw_prot_pmc(rdc_fw_prot_t *prot)
{
	uint8_t		res;

	prot->dat[0] = 0;
	
	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: dat reading
	res = rdc_ec_read_dat(prot->dat);
	RDC_EC_ERR_CHECK(res);

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_fw_prot_read
//=============================================================================
uint8_t	rdc_fw_prot_read(rdc_fw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;
	uint8_t		*datap;

	memset(prot->dat, 0, prot->len);
	
	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: ctl
	res = rdc_ec_write_dat(prot->ctl);
	RDC_EC_ERR_CHECK(res);

	// 3rd: dev
	if ((prot->cmd & 0xFE) == RDC_FW_CMD_WR_ECRAM)
	{
		// skip dev step for compliant old protocol of Susi
	}
	else
	{
		res = rdc_ec_write_dat(prot->dev);
		RDC_EC_ERR_CHECK(res);
	}

	// 4th: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);
	
	
	datap = prot->dat;	// point to data buffer

	// 5th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_read_dat(datap++);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_fw_prot_write
//=============================================================================
uint8_t	rdc_fw_prot_write(rdc_fw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;


	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: ctl
	res = rdc_ec_write_dat(prot->ctl);
	RDC_EC_ERR_CHECK(res);
	
	// 3rd: dev
	if ((prot->cmd & 0xFE) == RDC_FW_CMD_WR_ECRAM)
	{
		// skip dev step for compliant old protocol of Susi
	}
	else
	{
		res = rdc_ec_write_dat(prot->dev);
		RDC_EC_ERR_CHECK(res);
	}

	// 4th: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// 5th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_write_dat(prot->dat[i]);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_fw_prot_read_drt : issue read command directly
//=============================================================================
uint8_t	rdc_fw_prot_read_drt(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf)
{
	rdc_fw_prot_t	pt;
	
	pt.cmd	= cmd;
	pt.ctl	= ctl;
	pt.dev	= dev;
	pt.len	= len;
	pt.dat	= buf;

	return rdc_fw_prot_read(&pt);
}

//=============================================================================
//  rdc_fw_prot_write_drt : issue write command directly
//=============================================================================
uint8_t	rdc_fw_prot_write_drt(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf)
{
	rdc_fw_prot_t	pt;
	
	pt.cmd	= cmd;
	pt.ctl	= ctl;
	pt.dev	= dev;
	pt.len	= len;
	pt.dat	= buf;

	return rdc_fw_prot_write(&pt);
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RDC_EC - S/W team protocol                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  rdc_sw_prot_idx_read
//=============================================================================
uint8_t	rdc_sw_prot_idx_read(rdc_sw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;
	uint8_t		*datap;


	memset(prot->dat, 0, prot->len);

	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: idx
	res = rdc_ec_write_dat(prot->idx);
	RDC_EC_ERR_CHECK(res);

	// 3rd: off
	res = rdc_ec_write_dat(prot->off);
	RDC_EC_ERR_CHECK(res);

	// 4th: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 5th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_read_dat(datap++);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_sw_prot_idx_write
//=============================================================================
uint8_t	rdc_sw_prot_idx_write(rdc_sw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;


	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: idx
	res = rdc_ec_write_dat(prot->idx);
	RDC_EC_ERR_CHECK(res);
	
	// 3rd: off
	res = rdc_ec_write_dat(prot->off);
	RDC_EC_ERR_CHECK(res);

	// 4th: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// 5th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_write_dat(prot->dat[i]);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_sw_prot_noidx_read
//=============================================================================
uint8_t	rdc_sw_prot_noidx_read(rdc_sw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;
	uint8_t		*datap;


	memset(prot->dat, 0, prot->len);

	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: off
	res = rdc_ec_write_dat(prot->off);
	RDC_EC_ERR_CHECK(res);

	// 3rd: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 4th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_read_dat(datap++);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_sw_prot_noidx_write
//=============================================================================
uint8_t	rdc_sw_prot_noidx_write(rdc_sw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;


	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: off
	res = rdc_ec_write_dat(prot->off);
	RDC_EC_ERR_CHECK(res);

	// 3rd: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// 4th: dat writing
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_write_dat(prot->dat[i]);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

//=============================================================================
//  rdc_sw_prot_info_read
//=============================================================================
uint8_t	rdc_sw_prot_info_read(rdc_sw_prot_t *prot)
{
	uint8_t		i;
	uint8_t		res;
	uint8_t		*datap;
	uint8_t		sts;


	memset(prot->dat, 0, prot->len);

	// 1st: cmd
	res = rdc_ec_write_cmd(prot->cmd);
	RDC_EC_ERR_CHECK(res);

	// 2nd: sts
	res = rdc_ec_read_dat(&sts);
	RDC_EC_ERR_CHECK(res);
	// todo : checking available and un-busy

	// 3rd: off
	res = rdc_ec_write_dat(prot->off);
	RDC_EC_ERR_CHECK(res);

	// 4th: len
	res = rdc_ec_write_dat(prot->len);
	RDC_EC_ERR_CHECK(res);

	// point to data buffer
	datap = prot->dat;

	// 4th: dat reading
	for (i=0; i<prot->len; i++)
	{
		res = rdc_ec_read_dat(datap++);
		RDC_EC_ERR_CHECK(res);
	}

	return RDC_EC_OK;
}

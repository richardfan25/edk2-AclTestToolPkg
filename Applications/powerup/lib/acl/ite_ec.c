//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE_EC - ITE IT8528/IT8518 Embedded Controller                           *
//*           ITE IT5121        Embedded Controller                           *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <dos.h>

#include "typedef.h"
#include "x86io.h"
#include "ite_ec.h"

// The prefix name

// ite_ec_* : for IT8528/IT8518
// ite_*    : for IT5121

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE_EC - Configuration                                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  ite_ec_get_ldn_cfg
//=============================================================================
void ite_ec_get_ldn_cfg(uint8_t ldn, uint8_t *cfg)
{
	uint16_t	i;
	uint8_t		idx;
	
	outb(ITE_CFG_IDX_PORT, 0x87);		// enter config
	outb(ITE_CFG_IDX_PORT, 0x87);

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
	outb(ITE_CFG_IDX_PORT, 0x07);		// select LDN
	outb(ITE_CFG_DAT_PORT, ldn);

	for (i=0, idx=0; i<256; i++, idx++)
	{
		outb(ITE_CFG_IDX_PORT, idx);	// select offset
		cfg[i] = inb(ITE_CFG_DAT_PORT);
	}
	
	//-------------------------------------------------------
	//  ITE IT8518
	//-------------------------------------------------------

	//-------------------------------------------------------
	//  ITE IT5121
	//-------------------------------------------------------
	
	outb(ITE_CFG_IDX_PORT, 0xAA);		// exit config	
}

//=============================================================================
//  ite_ec_probe_port
//=============================================================================
uint8_t	ite_ec_probe_port(void)
{
	// TODO : 
	return 0;
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE_EC - ADT (299h/29Ah)                                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  ite_ec_wait_ibe
//=============================================================================
static uint8_t ite_ec_wait_ibe(void)
{
	uint16_t	retries = ITE_EC_RETRIES;
	uint8_t		res = ITE_EC_ERR_TMOUT_IBE;

	// wait for the input buffer empty
	while(retries--)
	{
		if ((inb(ITE_EC_STS_PORT) & ITE_EC_STS_IBF) == 0)
		{
			res = ITE_EC_OK;
			break;
		}
	}

	return res;
}

//=============================================================================
//  ite_ec_wait_obf
//=============================================================================
static uint8_t ite_ec_wait_obf(void)
{
	uint16_t	retries = ITE_EC_RETRIES;
	uint8_t		res = ITE_EC_ERR_TMOUT_OBF;

	// wait for the output buffer full
	while (retries--)
	{
		if (inb(ITE_EC_STS_PORT) & ITE_EC_STS_OBF)
		{
			res = ITE_EC_OK;
			break;
		}
	}

	return res;
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE_EC - Protocol                                                        *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  ite_ec_read_acpi_ram
//=============================================================================
uint8_t ite_ec_read_acpi_ram(uint8_t off, uint8_t *dat)
{
	uint8_t		res;


	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// clear ec_obf
	inb(ITE_EC_DAT_PORT);

	outb(ITE_EC_CMD_PORT, 0x80);		// 0x80:ITE_EC_CMD_ACPI_READ

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	outb(ITE_EC_DAT_PORT, off);

	res = ite_ec_wait_obf();
	ITE_EC_ERR_CHECK(res);

	*dat = inb(ITE_EC_DAT_PORT);

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_write_acpi_ram
//=============================================================================
uint8_t ite_ec_write_acpi_ram(uint8_t off, uint8_t dat)
{
	uint8_t		res;

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// clear ec_obf
	inb(ITE_EC_DAT_PORT);

	outb(ITE_EC_CMD_PORT, 0x81);	// 0x81:ITE_EC_CMD_ACPI_WRITE

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// offset
	outb(ITE_EC_DAT_PORT, off);

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// data
	outb(ITE_EC_DAT_PORT, dat);

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_read_mbox (aec_mbox_read)
//=============================================================================
static uint8_t ite_ec_read_mbox(uint8_t off, uint8_t *dat)
{
	uint8_t		res;

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// clear ec_obf
	inb(ITE_EC_DAT_PORT);

	// offset : issue cmd
	outb(ITE_EC_CMD_PORT, 0xA0+off);	// A0h=ITE_EC_CMD_ADT_READ+offset

	res = ite_ec_wait_obf();
	ITE_EC_ERR_CHECK(res);

	// data
	*dat = inb(ITE_EC_DAT_PORT);

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_write_mbox (aec_mbox_write)
//=============================================================================
static uint8_t ite_ec_write_mbox(uint8_t off, uint8_t dat)
{
	uint8_t res;

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	// clear ec_obf
	inb(ITE_EC_DAT_PORT);

	// offset : issue cmd
	outb(ITE_EC_CMD_PORT, 0x50+off);	// 50h=ITE_EC_CMD_ADT_WRITE+offset

	res = ite_ec_wait_ibe();
	ITE_EC_ERR_CHECK(res);

	outb(ITE_EC_DAT_PORT, dat);

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_wait_cmd_done (aec_wait_cmd_clear)
//=============================================================================
static uint8_t	ite_ec_wait_cmd_done(void)
{
	uint8_t		res;
	uint8_t		data;
	uint16_t	retries = 1000;

	while (retries--)
	{
		// 00h:ITE_EC_MBOX_CMD
		res = ite_ec_read_mbox(0x00, &data);

		if ((res == ITE_EC_OK) && (data == 0))
			return ITE_EC_OK;
		
		//delay(1);
	}

	return ITE_EC_ERR_TMOUT_MBOX;
}

//=============================================================================
//  ite_ec_clear_mbox
//=============================================================================
static uint8_t ite_ec_clear_mbox(void)
{
	uint8_t		res;

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// 00h:ITE_EC_MBOX_CMD
	// FFh:ITE_EC_CMD_MBOX_CLEAR
	res = ite_ec_write_mbox(0x00, 0xFF);
	ITE_EC_ERR_CHECK(res);

	delay(10);

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_read
//=============================================================================
//  ite_ec_fw_prot_read
//    aec_mbox_xx
// 
//=============================================================================
uint8_t ite_ec_fw_prot_read(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;
	int			i;

	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	//cmd=0xF0 : ITE_EC_CMD_MBOX_VER
	res = ite_ec_write_mbox(0x00, prot->cmd);
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// aec_mbox_read(ITE_EC_MBOX_STA) //
	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	// todo : need to be checking data ?

	for (i=0; i<prot->len; i++)
	{
		res = ite_ec_read_mbox(0x03+(uint8_t)i, &data);	// 03h:ITE_EC_MBOX_DAT
		prot->dat[i] = (uint8_t)data;
		ITE_EC_ERR_CHECK(res);
	}

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_smb_read_quick
//=============================================================================
uint8_t ite_ec_fw_prot_smb_read_quick(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;


	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// cmd
	res = ite_ec_write_mbox(0x00, prot->cmd);	// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
	{
		return data;
	}

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_smb_read_byte
//=============================================================================
uint8_t ite_ec_fw_prot_smb_read_byte(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;

	
	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
// read byte

	// cmd
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return ITE_EC_OK;	// ?

// read byte
	res = ite_ec_read_mbox(0x03+4, &data);	// 07h:ITE_EC_MBOX_DAT+4
	ITE_EC_ERR_CHECK(res);
	prot->dat[0] = (uint8_t)data;

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_smb_read_word
//=============================================================================
uint8_t ite_ec_fw_prot_smb_read_word(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;

	
	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
// read word

	// cmd
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return ITE_EC_OK;	// ?

// read word
	res = ite_ec_read_mbox(0x03+4, &data);	// 07h:ITE_EC_MBOX_DAT+4
	ITE_EC_ERR_CHECK(res);
	prot->dat[0] = (uint8_t)data;

	res = ite_ec_read_mbox(0x03+5, &data);	// 07h:ITE_EC_MBOX_DAT+5
	ITE_EC_ERR_CHECK(res);
	prot->dat[1] = (uint8_t)data;

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_smb_write_word
//=============================================================================
uint8_t ite_ec_fw_prot_smb_write_word(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;

	
	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx : device id
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd : slave address
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd : command code : as MSB address
	res = ite_ec_write_mbox(0x04, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);

	// wb[0] : as LSB address
	res = ite_ec_write_mbox(0x07, prot->wb[0]);	//
	ITE_EC_ERR_CHECK(res);
	
	// wb[1] : 1 byte data to write
	res = ite_ec_write_mbox(0x08, prot->wb[1]);	//
	ITE_EC_ERR_CHECK(res);
	
	// start : 
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);

	// todo : need to be checking data ?
	if (data != 0x80)
		return ITE_EC_ERR_SMB;

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_smb_read_block
//=============================================================================
uint8_t ite_ec_fw_prot_smb_read_block(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;
	int			i;

	
	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
	// read block

	// cmd
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	// todo : need to be checking data ?
	if (data != 0x80)
		return ITE_EC_OK;	// ?

	// read block
	for (i=0; i<16; i++)
	{
		res = ite_ec_read_mbox(0x03+4+(uint8_t)i, &data);	// 07h:ITE_EC_MBOX_DAT+4
		ITE_EC_ERR_CHECK(res);
		prot->dat[i] = (uint8_t)data;
	}

	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_i2c_read_proc
//=============================================================================
uint8_t ite_ec_fw_prot_i2c_read_proc(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;
	uint32_t	bcnt, len;
	int			i, j, k, banknum;


	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx : device id = 0x28,0x2B,0x2F
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd : slave address
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd : command code (SMBus)
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
	//----------------------------------------------------------------------
	// i2c_process_call : i2c_write_read_buf = 0x01
	//----------------------------------------------------------------------
	//res = ite_ec_wait_cmd_done();
	//ITE_EC_ERR_CHECK(res);

	// clear buffer ram
	res = ite_ec_write_mbox(0x00, 0xc0);			// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// status
	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	
	if (data != 0x01)
		return ITE_EC_ERR_SMB;

	// r buffer 
	len = prot->rblen;
	if (len > 255)
		len = 0;
	res = ite_ec_write_mbox(0x05, (uint8_t)len);
	ITE_EC_ERR_CHECK(res);

	// w buffer
	len = prot->wblen;
	if (len > 41)
		len = 41;	// ?
	res = ite_ec_write_mbox(0x06, (uint8_t)len);
	ITE_EC_ERR_CHECK(res);

	for (i=0; (uint32_t)i<len; i++)
	{
		res = ite_ec_write_mbox(0x07+(uint8_t)i, prot->wb[i]);	
		ITE_EC_ERR_CHECK(res);
	}
	
	// start : cmd : I2C_WRITE_READ_BUF=0x01
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD,
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// status
	res = ite_ec_read_mbox(0x01, &data);
	ITE_EC_ERR_CHECK(res);

	if (data != 0x80)
		return ITE_EC_ERR_SMB;

	// buffer ram read
	len = prot->rblen;
	if (len >= 256)
		len = 256;

	banknum = (len + 31) / 32;	// 32 bytes/bank

	for (i=0, k=0; i<banknum; i++)
	{
		res = ite_ec_write_mbox(0x02, (uint8_t)i);	// PAR
		ITE_EC_ERR_CHECK(res);

		res = ite_ec_write_mbox(0x00, 0xc1);			// CMD
		ITE_EC_ERR_CHECK(res);	
		
		res = ite_ec_wait_cmd_done();
		ITE_EC_ERR_CHECK(res);
	
		// status
		res = ite_ec_read_mbox(0x01, &data);			// STA
		ITE_EC_ERR_CHECK(res);

		if (data != 0x01)
			return ITE_EC_ERR_SMB;
	
		if (len >= 32)
			bcnt = 32;
		else
			bcnt = (int)len;

		for (j=0; (uint32_t)j<bcnt; j++, k++)
		{
			res = ite_ec_read_mbox(0x03+(uint8_t)j, &prot->rb[k]);
			ITE_EC_ERR_CHECK(res);
		}

		len -= bcnt;
	}
	return ITE_EC_OK;
}

//=============================================================================
//  ite_ec_fw_prot_i2c_write_proc
//=============================================================================
uint8_t ite_ec_fw_prot_i2c_write_proc(ite_fw_prot_t *prot)
{
	uint8_t		res;
	uint8_t		data;
	uint32_t	bcnt, len;
	int			i, j, k, banknum;


	res = ite_ec_clear_mbox();
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx : device id = 0x28,0x2B,0x2F
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd : slave address
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd : command code (SMBus)
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
	//----------------------------------------------------------------------
	// i2c_process_call : i2c_write_read_buf = 0x01
	//----------------------------------------------------------------------
	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// clear buffer ram
	res = ite_ec_write_mbox(0x00, 0xc0);			// 0x00=ITE_EC_MBOX_CMD
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// status
	res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
	ITE_EC_ERR_CHECK(res);
	
	if (data != 0x01)
		return ITE_EC_ERR_SMB;

#if 0
	// r buffer len
	len = 0;
	res = ite_ec_write_mbox(0x05, len);	// forced 0 for write process
	ITE_EC_ERR_CHECK(res);

	// w buffer len
	len = prot->wblen;
	if (len >= 41)
		len = 41;
	res = ite_ec_write_mbox(0x06, len);
	ITE_EC_ERR_CHECK(res);
#endif

	//for (i=0; i<len; i++)
	//{
	//	res = ite_ec_write_mbox(0x07+i, prot->wb[i]);	
	//	ITE_EC_ERR_CHECK(res);
	//}
	
	//	res = ite_ec_write_mbox(0x06, bcnt);
	//	ITE_EC_ERR_CHECK(res);

	// depends on eeprom page size
	// 512kb = 64KB = 65536 : 128 bytes
	// 256kb = 32KB = 32768 :  64 bytes
	// 128kb = 16KB = 16384 :  64 bytes
	//  64kb =  8KB =  8192 :  32 bytes
	//  32kb =  4KB =  4096 :  32 bytes

	len = prot->wblen;

	banknum = (len + 31) / 32;	// 32 bytes/bank

	for (i=0, k=0; i<banknum; i++)
	{
		res = ite_ec_wait_cmd_done();
		ITE_EC_ERR_CHECK(res);

		res = ite_ec_write_mbox(0x02, (uint8_t)i);	// PAR
		ITE_EC_ERR_CHECK(res);

		if (len >= 32)
			bcnt = 32;
		else
			bcnt = (int)len;

		for (j=0; (uint32_t)j<bcnt; j++, k++)
		{
			res = ite_ec_write_mbox(0x03+(uint8_t)j, prot->wb[k]);
			ITE_EC_ERR_CHECK(res);
		}
	
		res = ite_ec_write_mbox(0x00, 0xc2);	// buffer ram write
		ITE_EC_ERR_CHECK(res);

		res = ite_ec_wait_cmd_done();
		ITE_EC_ERR_CHECK(res);

		// status
		res = ite_ec_read_mbox(0x01, &data);	// 01h:ITE_EC_MBOX_STA
		ITE_EC_ERR_CHECK(res);
	
		if (data != 0x01)
			return ITE_EC_ERR_SMB;

		len -= bcnt;
	}

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// didx : device id = 0x28,0x2B,0x2F
	res = ite_ec_write_mbox(0x02, prot->didx);	// 0x02=ITE_EC_MBOX_PAR
	ITE_EC_ERR_CHECK(res);
	
	// dadd : slave address
	res = ite_ec_write_mbox(0x03, prot->dadd);	// 0x03=ITE_EC_MBOX_DAT
	ITE_EC_ERR_CHECK(res);

	// dcmd : command code (SMBus)
	res = ite_ec_write_mbox(0x03+1, prot->dcmd);	// 0x04=ITE_EC_MBOX_DAT+1
	ITE_EC_ERR_CHECK(res);
	
	// r buffer len
	len = 0;
	res = ite_ec_write_mbox(0x05, (uint8_t)len);	// forced 0 for write process
	ITE_EC_ERR_CHECK(res);

	// w buffer len
	len = prot->wblen;
	if (len >= 41)
		len = 41;
	res = ite_ec_write_mbox(0x06, (uint8_t)len);
	ITE_EC_ERR_CHECK(res);

	// start : cmd : I2C_WRITE_READ_BUF=0x01
	res = ite_ec_write_mbox(0x00, prot->cmd);		// 0x00=ITE_EC_MBOX_CMD,
	ITE_EC_ERR_CHECK(res);

	res = ite_ec_wait_cmd_done();
	ITE_EC_ERR_CHECK(res);

	// status
	res = ite_ec_read_mbox(0x01, &data);
	ITE_EC_ERR_CHECK(res);

	if (data != 0x80)
		return ITE_EC_ERR_SMB;

	return ITE_EC_OK;
}


//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ITE - ITE (29Eh/29FH)                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  ite_mbox_write
//=============================================================================
uint8_t ite_mbox_write(uint8_t off, uint8_t dat)
{
	// clear OBF
	inb(ITE_DAT_PORT);

	outb(ITE_CMD_PORT, off + 0x80);

	outb(ITE_DAT_PORT, dat);

	return ITE_OK;
}

//=============================================================================
//  ite_mbox_read
//=============================================================================
uint8_t ite_mbox_read(uint8_t off, uint8_t *dat)
{
	// clear OBF
	inb(ITE_DAT_PORT);

	outb(ITE_CMD_PORT, off + 0x80);

	*dat = inb(ITE_DAT_PORT);

	return ITE_OK;
}

//=============================================================================
//  ite_mbox_wait
//=============================================================================
uint8_t ite_mbox_wait(void)
{
	uint16_t	cnt = ITE_RETRIES;
	
	do
	{
		inb(ITE_DAT_PORT);

		// 0x00 : MBOX_OFFSET_CMD
		// 0x80 : HW_OFFSET
		outb(ITE_CMD_PORT, 0x00 + 0x80);

		if (inb(ITE_DAT_PORT) == 0)
			return ITE_OK;

		delay(1);
	
	}while (--cnt);
	
	return ITE_ERR_MBOX;
}

//=============================================================================
//  ite_mbox_clear
//=============================================================================
void ite_mbox_clear(void)
{
	// weird : without error handling here
	ite_mbox_wait();

	ite_mbox_write(ITE_MBOX_OFF_CMD, ITE_MBOX_CMD_CLR);

	delay(10);
}

//=============================================================================
//  ite_mbox_clear
//=============================================================================
uint8_t ite_mbox_read_fw_info(uint8_t *ver)
{
	uint8_t		res;
	uint8_t		sts, dat;
	int			i;


	ite_mbox_clear();

	res = ite_mbox_wait();
	ITE_ERR_CHECK(res);

	res = ite_mbox_write(ITE_MBOX_OFF_CMD, ITE_MBOX_CMD_VER);
	ITE_ERR_CHECK(res);

	res = ite_mbox_wait();
	ITE_ERR_CHECK(res);

	res = ite_mbox_read(ITE_MBOX_OFF_STS, &sts);
	ITE_ERR_CHECK(res);

	// length : 17
	for (i=0; i<17; i++)
	{
		ite_mbox_read(ITE_MBOX_OFF_DAT + (uint8_t)i, &dat);
		ver[i] = dat;
	}

	return ITE_OK;
}

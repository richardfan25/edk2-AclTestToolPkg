//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ACPI_EC - ACPI Embedded Controller                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include "typedef.h"
#include "x86io.h"
#include "acpi_ec.h"

//=============================================================================
//  acpi_ec_wait_ibe
//=============================================================================
static uint8_t acpi_ec_wait_ibe(void)
{
	uint16_t	retries = ACPI_EC_RETRIES;

	// wait for the input buffer empty
	while (inb(ACPI_EC_STS_PORT) & ACPI_EC_STS_IBF)
	{
		if (--retries == 0)
			return ACPI_EC_ERR_TMOUT_IBE;

	}

	return ACPI_EC_OK;
}

//=============================================================================
//  acpi_ec_wait_obf
//=============================================================================
static uint8_t acpi_ec_wait_obf(void)
{
	uint16_t	retries = ACPI_EC_RETRIES;

	// wait for the output buffer full
	while ((inb(ACPI_EC_STS_PORT) & ACPI_EC_STS_OBF) == 0)
	{
		if (--retries == 0)
			return ACPI_EC_ERR_TMOUT_OBF;
		
	}

	return ACPI_EC_OK;
}

//=============================================================================
//  acpi_ec_read
//=============================================================================
uint8_t	acpi_ec_read(uint8_t idx, uint8_t *datap)
{
	uint8_t		res;

	res = acpi_ec_wait_ibe();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC Read
	outb(ACPI_EC_CMD_PORT, ACPI_EC_CMD_READ);	

	res = acpi_ec_wait_ibe();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC RAM index
	outb(ACPI_EC_DAT_PORT, idx);				

	res = acpi_ec_wait_obf();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC RAM data
	*datap = inb(ACPI_EC_DAT_PORT);

	return ACPI_EC_OK;
}

//=============================================================================
//  acpi_ec_write
//=============================================================================
uint8_t	acpi_ec_write(uint8_t idx, uint8_t data)
{
	uint8_t		res;

	res = acpi_ec_wait_ibe();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC Write
	outb(ACPI_EC_CMD_PORT, ACPI_EC_CMD_WRITE);	

	res = acpi_ec_wait_ibe();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC RAM index
	outb(ACPI_EC_DAT_PORT, idx);

	res = acpi_ec_wait_ibe();
	ACPI_EC_ERR_CHECK(res);

	// ACPI EC RAM data
	outb(ACPI_EC_DAT_PORT, data);

	return ACPI_EC_OK;
}

//=============================================================================
//  acpi_ec_smb_trans
//=============================================================================
void acpi_ec_smb_trans(uint8_t sla, uint8_t cmd, uint8_t prot, uint8_t *buf, uint8_t *bcnt)
{
	int			i;
	uint8_t		*datap = buf;

	acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x81);				// EC write	
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_SLA);	// slave address @ EC RAM
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, sla);				// smart battery slave address

	acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x81);				// EC write	
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_CMD);	// cmd @ EC RAM
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, cmd);				// BatteryStatus()

	switch(prot)
	{
		case ACPI_EC_SMB_PROT_WRITE_WORD:
			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x81);				// EC write
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT0);	// data0 @ EC RAM
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, buf[0]);

			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x81);				// EC write
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT1);	// data1 @ EC RAM
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, buf[1]);	
			break;
	
		default:
			break;
	}

	acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x81);				// EC write
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_PROT);	// protocol @ EC RAM
	acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, prot);				// Read Word Protocol

	prot &= ~0x40;

	switch(prot)
	{
		case ACPI_EC_SMB_PROT_RECV_BYTE:
			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT0);	// data0 @ EC RAM
			acpi_ec_wait_obf();	*datap++ = inb(ACPI_EC_DAT_PORT);
			break;

		case ACPI_EC_SMB_PROT_READ_BYTE:
			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT0);	// data0 @ EC RAM
			acpi_ec_wait_obf();	*datap++ = inb(ACPI_EC_DAT_PORT);
			break;

		case ACPI_EC_SMB_PROT_READ_WORD:
			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT0);	// data0 @ EC RAM
			acpi_ec_wait_obf();	*datap++ = inb(ACPI_EC_DAT_PORT);

			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT1);	// data1 @ EC RAM
			acpi_ec_wait_obf();	*datap++ = inb(ACPI_EC_DAT_PORT);
			break;

		case ACPI_EC_SMB_PROT_READ_BLOCK:
			acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
			acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_BCNT);
			acpi_ec_wait_obf();	*bcnt = inb(ACPI_EC_DAT_PORT);
			
			for (i=0; i<*bcnt; i++)
			{
				acpi_ec_wait_ibe();	outb(ACPI_EC_CMD_PORT, 0x80);				// EC read
				acpi_ec_wait_ibe();	outb(ACPI_EC_DAT_PORT, ACPI_EC_SMB_DAT0+(uint8_t)i);	// data0 @ EC RAM
				acpi_ec_wait_obf();	*datap++ = inb(ACPI_EC_DAT_PORT);	
			}
			*datap = 0;
			break;

		default:
			break;
	}
}

//=============================================================================
//  acpi_ec_read_ram
//=============================================================================
uint8_t acpi_ec_read_ram(uint8_t *ram)
{
	uint8_t res;
	int		idx;
	
	for (idx=0; idx<256; idx++)
	{
		res = acpi_ec_read((uint8_t)idx, &ram[idx]);
		ACPI_EC_ERR_CHECK(res);
	}

	return res;
}

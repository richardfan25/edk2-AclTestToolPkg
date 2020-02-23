//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SMB (System Management Bus)                                        *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <dos.h>

#include "x86io.h"
#include "pci.h"
#include "smb.h"

//=============================================================================
//  SMB Controller
//=============================================================================
// Intel SMBus Controller

// 00h : status register
#define SMB_HST_STS					(0)	// offset
#define   SMB_HST_STS_DS			0x80
#define   SMB_HST_STS_INUSE			0x40
#define   SMB_HST_STS_SMB_ALERT		0x20
#define   SMB_HST_STS_FAILED		0x10
#define   SMB_HST_STS_BUS_ERR		0x08
#define   SMB_HST_STS_DEV_ERR		0x04
#define   SMB_HST_STS_INTR			0x02
#define   SMB_HST_STS_HOST_BUSY		0x01

// 02h : control register
#define SMB_HST_CNT					(2)
#define   SMB_HST_CNT_PEC_EN		0x80
#define   SMB_HST_CNT_START			0x40
#define   SMB_HST_SMB_CMD			0x1C
#define     SMB_CMD_QUICK			(0x00)
#define     SMB_CMD_BYTE			(0x04)
#define     SMB_CMD_BYTE_DATA		(0x08)
#define     SMB_CMD_WORD_DATA		(0x0C)
#define     SMB_CMD_PROC_CALL		(0x10)
#define     SMB_CMD_BLOCK			(0x14)
#define     SMB_CMD_I2C_READ		(0x18)
#define     SMB_CMD_RSVD			(0x1C)
#define   SMB_HST_CNT_KILL			0x02
#define   SMB_HST_CNT_INTR_EN		0x01

#define SMB_HST_CMD			(3)
#define SMB_HST_ADD			(4)
#define SMB_XMIT_SLVA		(4)
#define SMB_HST_DAT0		(5)
#define SMB_HST_DAT1		(6)
#define SMB_HST_BLKDAT		(7)
#define SMB_HST_PEC			(8)		// ICH3 and later
#define SMB_HST_RCV_SLVA	(9)
#define SMB_SLV_DAT0		(10)
#define SMB_SLV_DAT1		(11)
#define SMB_AUX_STS			(12)	// ICH4 and later

#define SMB_AUX_CTL			(13)	// ICH4 and later
#define   SMB_AUX_CTL_CRC	0x01	// h/w PEC
#define   SMB_AUX_CTL_E32B	0x02	// E32B

#define SMB_SMLINK_CTL		(14)	// SMLink pin control
#define SMB_SMBUS_CTL		(15)	// SMBus
#define SMB_SLV_STS			(16)	// ICH3 and later
#define SMB_SLV_CMD			(17)	// ICH3 and later
#define SMB_NTFD_ADD		(20)	// ICH3 and later
#define SMB_NTFD_DAT_LSB	(22)	// ICH3 and later
#define SMB_NTFD_DAT_MSB	(23)



	// SMB_HST_STS
	//-------------
	// [7]-BYTE_DONE
	// [6]-INUSE_STS
	// [5]-SMBALERT_STS
	// [4]-FAILED
	// [3]-BUS_ERR
	// [2]-DEV_ERR
	// [1]-INTR
	// [0]-HOST_BUSY
	//---------------
	// STS_ERR_FLAGS : [4] | [3] | [2] = 0x1C
	// STS_FLAGS     : [7] | [1] | ERR_FLAG = 0x9E


	// SMB_HST_CNT
	//-------------
	// [7]-PEC_EN
	// [6]-START
	// [5]-LAST_BYTE
	// [4:2]-SMB_CMD
	//   000-Quick
	//   001-Byte
	//   010-Byte Read
	//   011-Word Data
	//   100-Process Call
	//   101-Block
	//   110-I2C Read
	//   111-Block Process
	// [1]-KILL
	// [0]-INTREN

// SMB RW bit
#define SMB_RW_WRITE			0x0
#define SMB_RW_READ				0x1


#define SMB_WR_REG(reg, val) outp(smb->base + reg, val)
#define SMB_RD_REG(reg)		(uint8_t)inp(smb->base + reg)
//=============================================================================
//  variable
//=============================================================================
smb_bus_t	*smb = NULL;

// skip smbus slaveaddress
// 0x00,	// general call address
// 0x02,	// CBUS address
// 0x04,	// Address reserved for different bus format
// 0x06,	// Reserved for future use
// 0x08~0xF	// Reserved for future use
// 0x10		// SMBus host
// 0x12		// Smart Battery Charger
// 0x14		// Smart Battery Selector, Smart Battery System Manager
// 0x16		// Smart Battery
// 0x18		// SMBus Alert Respond Address
// 0x50		// PMBus ZONE READ
// 0x50		// Reserved for ACCESS.bus host
// 0x58		// LCD contrast controller ?
// 0x5A		// CCFL backlight driver ?
// 0x6E		// Reserved for ACCESS.bus default address
// 0x6E		// PMBus ZONE WRITE
// 0x80~0x86// PCMCIA Socket controller ?
// 0x88		// Graphic controller ?
// 0x90~0x96// Prototype Addresses
// 0xC2		// SMBus device default address
// 0xF0~0xF6	// 10-bit slave addressing
// 0xF8~0xFE	// reserved for future use

//=============================================================================
//  smb_add_dev
//=============================================================================
smb_dev_t *smb_add_dev(void)
{
	smb_dev_t	*dev;

	dev = (smb_dev_t *)malloc(sizeof(smb_dev_t));
	if (!dev)
		return NULL;

	memset(dev, 0, sizeof(smb_dev_t));

	dev->addr = 0xFF;	// unknown address ?
	dev->type = SMB_DEV_TYPE_UNKNOWN;

	return dev;
}

//=============================================================================
//  smb_delay
//=============================================================================
void smb_delay(void)
{
	outp(0x80, 0x80);	// just for delay
}

//=============================================================================
//  smb_scan_dev : Intel, AMD, ATI
//=============================================================================
uint8_t smb_scan_dev(uint8_t addr)
{
	uint8_t		add;
	uint8_t		sts;

	if (smb->ven_id == 0x1022)
	{
		// skip slave address
		if (addr < 0x10 || addr > 0xEF)
			return SMB_ERR_DEV_NOT_FOUND;

		add = addr & ~0x1; // force write bit

		SMB_WR_REG(SMB_HST_STS, SMB_RD_REG(SMB_HST_STS));	// reset
		SMB_WR_REG(SMB_HST_ADD, add);	// slave address
		SMB_WR_REG(SMB_HST_CMD, 0x00);
		SMB_WR_REG(SMB_HST_CNT, 0x40);	// Quick Command

		while (1)
		{
			sts = SMB_RD_REG(SMB_HST_STS);
			if ((sts & 0x01) == 0)
				break;
			//delay(1);
		}

		if ((sts & 0x06) == 0x02)
			return SMB_OK;
	
		return SMB_ERR_DEV_NOT_FOUND;
	}
	else
	{
		// skip slave address
		if (addr < 0x10 || addr > 0xEF)
			return SMB_ERR_DEV_NOT_FOUND;

		add = addr | 0x1;	// force read bit
	
		SMB_WR_REG(SMB_HST_STS, 0xFE);
		SMB_WR_REG(SMB_HST_ADD, add);	// slave address
		SMB_WR_REG(SMB_HST_CMD, 0x00);
		SMB_WR_REG(SMB_HST_CNT, 0x48);

		while (1)
		{
			sts = SMB_RD_REG(SMB_HST_STS);
			if ((sts & 0x01) == 0)
				break;
			//delay(1);
		}
		
		if ((sts & 0x06) == 0x02)
			return SMB_OK;
		
		return SMB_ERR_DEV_NOT_FOUND;
	}
}

//=============================================================================
//  smb_scan_bus : Intel, AMD, ATI
//=============================================================================
uint8_t smb_scan_bus(uint8_t st_addr, uint8_t en_addr)
{
	smb_dev_t	*sd;
	smb_dev_t	*sdev;

	int			i;
	uint8_t		addr;
	uint8_t		sts;

	smb_exit();
	smb_init();

	// 8-bit address (including R/W bit)
	for (i=st_addr; i<=en_addr; i+=2)
	{
		// skip slave address
		if (i < 0x10 || i > 0xEF)
			continue;

		addr = (uint8_t)(i + 1);
    	
		SMB_WR_REG(SMB_HST_STS, 0xFE);
		SMB_WR_REG(SMB_HST_ADD, addr);
		SMB_WR_REG(SMB_HST_CMD, 0x00);
		SMB_WR_REG(SMB_HST_CNT, 0x48);

    	// Todo : might block here
		while (1)
		{
			sts = SMB_RD_REG(SMB_HST_STS);
			if ((sts & 0x01) == 0)
				break;
			//delay(1);

		}

		// sts 
    	// 0x42 device found ?
    	// 0x44 device not found ?

    	if ((sts & 0x06) == 0x02)
    	{
			sd = smb_add_dev();
			if (!sd)
				return SMB_ERR_MALLOC_DEV;

			sd->addr = addr & 0xfe;	// mask R/W bit

			// device link-list
			if (smb->num_dev == 0)
			{
				smb->dev = sd;		// 1st device
			}
			else
			{
				sdev->next	= sd;	// 2nd device...
				sd->prev	= sdev;
			}
			sdev = sd;
			smb->num_dev++;
    	}
	}

	return SMB_OK;
}

//=============================================================================
//  smb_scan_bus_nvidia : nVidia
//=============================================================================
uint8_t smb_scan_bus_nvidia(void)
{
	smb_dev_t	*sd;
	smb_dev_t	*sdev;

	int			i;
	uint8_t		addr;
	uint8_t		sts;

	uint32_t	loops;

	// 8-bit address (including R/W bit)
	for (i=0; i<256; i+=2)
	{
		// skip slave address
		if (i < 0x10 || i > 0xEF)
			continue;

		addr = (uint8_t)(i + 1);
    	
		SMB_WR_REG(0x2, addr);
		smb_delay();
		SMB_WR_REG(0x3, 0x00);
		smb_delay();
		SMB_WR_REG(0x0, 0x07);
		smb_delay();

		// busy waiting
		loops = 1000000;
		while (loops--)
		{
			sts = SMB_RD_REG(0x1);
			if (sts)
				break;
		} 

		sts = SMB_RD_REG(0x1) & 0x80;
		if (sts == 0x80)
		{
			sd = smb_add_dev();
			if (!sd)
				return SMB_ERR_MALLOC_DEV;

			sd->addr = addr & 0xfe;	// mask R/W bit
			
		}

		// device link-list
		if (smb->num_dev == 0)
		{
			smb->dev = sd;		// 1st device
		}
		else
		{
			sdev->next	= sd;	// 2nd device...
			sd->prev	= sdev;
		}
		sdev = sd;
		smb->num_dev++;
	}

	return SMB_OK;
}

//=============================================================================
//  smb_quick_command
//=============================================================================
uint8_t smb_quick_command(uint8_t addr)
{
	uint8_t		sts;
	//uint16_t	retries = 2000;
	uint16_t	retries = 65535;

	SMB_WR_REG(SMB_HST_STS, 0x1e);
	SMB_WR_REG(SMB_HST_ADD, addr);
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_QUICK);	// START + read 

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & 0x01) == 0)
			break;
		//delay(1);
	} while (--retries);
	
	return sts;
}

//=============================================================================
//  smb_quick_command_ati
//=============================================================================
uint8_t smb_quick_command_ati(uint8_t addr)
{
	uint8_t		sts;
	//uint16_t	retries = 2000;
	uint16_t	retries = 65535;

	SMB_WR_REG(SMB_HST_STS, SMB_RD_REG(SMB_HST_STS));
	SMB_WR_REG(SMB_HST_ADD, addr);
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_QUICK);	// START + read 

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & 0x01) == 0)
			break;
		//delay(1);
	} while (--retries);
	
	return sts;
}

//=============================================================================
//  smb_read_byte
//=============================================================================
uint8_t smb_read_byte(uint8_t addr)
{
	uint8_t		sts;
	SMB_WR_REG(SMB_HST_STS, 0x1e);
	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BYTE);	// START + read 

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & 0x01) == 0)
			break;
		//delay(1);
	} while (1);
	
	return SMB_RD_REG(SMB_HST_DAT0);
}

//=============================================================================
//  smb_read_byte_ati
//=============================================================================
uint8_t smb_read_byte_ati(uint8_t addr)
{
	uint8_t		sts;
	SMB_WR_REG(SMB_HST_STS, SMB_RD_REG(SMB_HST_STS));
	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BYTE);	// START + read 

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & 0x01) == 0)
			break;
		//delay(1);
	} while (1);
	
	return SMB_RD_REG(SMB_HST_DAT0);
}

//=============================================================================
//  smb_read_byte_data
//=============================================================================
uint8_t smb_read_byte_data(uint8_t addr, uint8_t cmd)
{
	uint8_t		sts;

	//SMB_WR_REG(SMB_HST_STS, 0x1e);
	SMB_WR_REG(SMB_HST_STS, 0xfe);
	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CMD, cmd);					// device register address
	// protocol start
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BYTE_DATA);		// START + read byte

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);

		if ((sts & 0x01) == 0)
			break;
		//delay(1);	// ms

	} while (1);

	return SMB_RD_REG(SMB_HST_DAT0);
}

#if 0
uint8_t smb_read_byte_data(uint8_t addr, uint8_t cmd)
{
	uint8_t		sts;
	uint16_t	tmout;

	// ?
	//SMB_WR_REG(SMB_HST_STS, 0x1e);

	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CMD, cmd);					// device register address

	// disable pec
	SMB_WR_REG(SMB_AUX_CTL, SMB_RD_REG(SMB_AUX_CTL) & ~SMB_AUX_CTL_CRC);

	sts = SMB_RD_REG(SMB_HST_STS);
	if (sts & SMB_HST_STS_HOST_BUSY)
		return 0xFF;	// ?

	sts &= 0x9E;
	if (sts)
	{
		SMB_WR_REG(SMB_HST_STS, sts);
		sts = SMB_RD_REG(SMB_HST_STS);
		if (sts & 0x9E)
			return 0xFF;	// ?
	}
	
	// protocol start
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BYTE_DATA);		// START + read byte

	tmout = 0;
	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);

		//if ((sts & 0x01) == 0)
		//	break;
		
		// error checking ?

		delay(1);	// ms

	} while (((sts & 0x01) ||	// host busy
			 !(sts & 0x1C))	&& // error flag
			 (tmout++ < 1000));

	if (tmout >= 1000)
		return 0xFF;
	
	return SMB_RD_REG(SMB_HST_DAT0);
}
#endif

//=============================================================================
//  smb_read_byte_data_ati
//=============================================================================
uint8_t smb_read_byte_data_ati(uint8_t addr, uint8_t cmd)
{
	uint8_t		sts;

	//SMB_WR_REG(SMB_HST_STS, 0x1e);
	SMB_WR_REG(SMB_HST_STS, SMB_RD_REG(SMB_HST_STS));
	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CMD, cmd);					// device register address
	// protocol start
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BYTE_DATA);		// START + read byte

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);

		if ((sts & 0x01) == 0)
			break;
		//delay(1);	// ms

	} while (1);

	return SMB_RD_REG(SMB_HST_DAT0);
}

//=============================================================================
//  smb_read_word_data
//=============================================================================
uint16_t smb_read_word_data(uint8_t addr, uint8_t cmd)
{
	uint16_t	data;

	SMB_WR_REG(SMB_HST_STS, 0xfe);
	delay(10);

	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);	// slave address + read
	SMB_WR_REG(SMB_HST_CMD, cmd);
	SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_WORD_DATA);

	while (SMB_RD_REG(SMB_HST_STS) & 0x1)
	{
		delay(1);
	}

	data = SMB_RD_REG(SMB_HST_DAT1);
	data <<= 8;
	data |= SMB_RD_REG(SMB_HST_DAT0);

	return data;
}

//=============================================================================
//  smb_read_block_data
//=============================================================================
uint8_t smb_read_block_data(uint8_t addr, uint8_t cmd, uint8_t *data)
{
	int			i;
	uint8_t		sts;
	uint8_t		blkcnt;
	uint8_t		val;

	SMB_WR_REG(SMB_HST_STS, 0x1e);

	// slave address + read
	SMB_WR_REG(SMB_HST_ADD, addr | SMB_RW_READ);
	
	// command code
	SMB_WR_REG(SMB_HST_CMD, cmd);

	// disable hw pec
	val = SMB_RD_REG(SMB_AUX_CTL);
	val &= ~SMB_AUX_CTL_CRC;
	SMB_WR_REG(SMB_AUX_CTL, val);

	// set block buffer mode
	val = SMB_RD_REG(SMB_AUX_CTL);
	val |= SMB_AUX_CTL_E32B;
	SMB_WR_REG(SMB_AUX_CTL, val);
	
	val = SMB_RD_REG(SMB_AUX_CTL);
	if (val & SMB_AUX_CTL_E32B)
	{
		// i801_block_transaction_by_block
		val = SMB_RD_REG(SMB_HST_CNT);	// dummy read
		
	
		// i801_transaction : START + read block
		SMB_WR_REG(SMB_HST_CNT, SMB_HST_CNT_START | SMB_CMD_BLOCK);
	}
	else
	{
		// i801_block_transaction_byte_by_byte
	}

	do
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & 0x01) == 0)
			break;
		//delay(1);
	} while (1);
	
	blkcnt = SMB_RD_REG(SMB_HST_DAT0);

	for (i=0; i<blkcnt; i++)
		data[i] = SMB_RD_REG(SMB_HST_BLKDAT);

	// disable hw pec and e32b
	val = SMB_RD_REG(SMB_AUX_CTL);
	val &= ~(SMB_AUX_CTL_CRC | SMB_AUX_CTL_E32B);
	SMB_WR_REG(SMB_AUX_CTL, val);

	return blkcnt;
}

//=============================================================================
//  smb_write_byte_data
//=============================================================================
uint8_t smb_write_byte_data(uint8_t addr, uint8_t cmd, uint8_t data)
{
	while (SMB_RD_REG(SMB_HST_STS) & 0x1)
		//delay(1);
	SMB_WR_REG(SMB_HST_STS,  0xFE);
	delay(10);

	SMB_WR_REG(SMB_HST_ADD,  addr | SMB_RW_WRITE);	// slave address + write
	SMB_WR_REG(SMB_HST_CMD,  cmd);					// write the offset
	SMB_WR_REG(SMB_HST_DAT0, data);					// write byte count
	SMB_WR_REG(SMB_HST_CNT,  SMB_HST_CNT_START | SMB_CMD_BYTE_DATA);

	return 0;
}
#if 0
uint8_t smb_write_byte_data(uint8_t addr, uint8_t cmd, uint8_t data)
{
	uint16_t	loops;

	SMB_RD_REG(SMB_HST_CNT);			// reset index pointer of 32-bytes buffer

	SMB_WR_REG(SMB_HST_BLKDAT, data);	// set data you want to write
	SMB_WR_REG(SMB_HST_STS, 0x1f);		// check bus ready, clear all status bits

	while (SMB_RD_REG(SMB_HST_STS) & 0x1)
	{
		delay(1);
	}

	SMB_WR_REG(SMB_HST_CMD,  cmd);					// write the offset
	SMB_WR_REG(SMB_HST_ADD,  addr | SMB_RW_WRITE);	// slave address + write
	SMB_WR_REG(SMB_HST_DAT0, 0x1);					// write byte count
	SMB_WR_REG(SMB_HST_CNT,  0x54);					// read byte protocol and start

	loops = 10000;
	while (loops--)
	{
		if ((SMB_RD_REG(SMB_HST_STS) & 0x1) == 0)
			break;
		delay(1);
	}

	if ((SMB_RD_REG(SMB_HST_STS) & 0x1c))
		return SMB_ERR_WRITE_BYTE;

	return SMB_OK;
}
#endif

//=============================================================================
//  smb_write_word_data
//=============================================================================
uint8_t smb_write_word_data(uint8_t addr, uint8_t cmd, uint16_t data)
{

	SMB_WR_REG(SMB_HST_STS, 0xfe);
	delay(10);

	SMB_WR_REG(SMB_HST_ADD,  addr | SMB_RW_WRITE);	// slave address + write
	SMB_WR_REG(SMB_HST_CMD,  cmd);					// write the offset

	SMB_WR_REG(SMB_HST_DAT0, data & 0xff);			// write lsb
	SMB_WR_REG(SMB_HST_DAT1, (data>>8) & 0xff);		// write msb

	SMB_WR_REG(SMB_HST_CNT,  SMB_HST_CNT_START | SMB_CMD_WORD_DATA);

	return SMB_OK;
}

//=============================================================================
//  smb_write_block_data
//=============================================================================
uint8_t smb_write_block_data(uint8_t addr, uint8_t cmd, uint8_t cnt, uint8_t *data)
{
	int			i;

	while (SMB_RD_REG(SMB_HST_STS) & 0x1)
		//delay(1);
	SMB_WR_REG(SMB_HST_STS,  0xFE);
	delay(10);

	SMB_WR_REG(SMB_HST_ADD,  addr | SMB_RW_WRITE);	// slave address + write
	SMB_WR_REG(SMB_HST_CMD,  cmd);					// write the offset
	SMB_WR_REG(SMB_HST_DAT0, cnt);
	
	for (i=0; i<cnt; i++)
		SMB_WR_REG(SMB_HST_BLKDAT, data[i]);	
	
	SMB_WR_REG(SMB_HST_CNT,  SMB_HST_CNT_START | SMB_CMD_BLOCK);

	return 0;
}

//=============================================================================
//  smb_process_call
//=============================================================================
uint16_t smb_process_call(uint8_t addr, uint16_t ee_addr)
{
	uint8_t		sts;
	uint16_t	data;

	SMB_WR_REG(SMB_HST_STS, 0xfe);
	delay(10);
	
	// slave address + write
	SMB_WR_REG(SMB_HST_ADD,  addr | SMB_RW_WRITE);

	// command code : write the offset
	//SMB_WR_REG(SMB_HST_CMD,  reg);

	// data 0 : eeprom address MSB
	SMB_WR_REG(SMB_HST_DAT0, (uint8_t)((ee_addr >> 8) & 0xff));
	
	// data 1 : eeprom address LSB
	SMB_WR_REG(SMB_HST_DAT1, (uint8_t)(ee_addr & 0xff));

	// protocol
	SMB_WR_REG(SMB_HST_CNT,  SMB_HST_CNT_START | SMB_CMD_PROC_CALL);

	// wait for complete
#if 0
	loops = 1000000;
	while (--loops)
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if (sts & SMB_HST_STS_INTR)
		{
			SMB_WR_REG(SMB_HST_STS, sts);	// clear
			sts = SMB_RD_REG(SMB_HST_STS);	// read again
			delay(100);
			break;
		}
		else if (sts & (SMB_HST_STS_FAILED | SMB_HST_STS_BUS_ERR | SMB_HST_STS_DEV_ERR))
		{
			SMB_WR_REG(SMB_HST_STS, sts);	// clear
			delay(1);
		}
		else if (sts & SMB_HST_STS_HOST_BUSY)
		{
			delay(1);
		}
		else
		{
			break;
		}
	}
#endif

	while (1)
	{
		sts = SMB_RD_REG(SMB_HST_STS);
		if ((sts & SMB_HST_STS_HOST_BUSY) == 0)
			break;
	}

	data = SMB_RD_REG(SMB_HST_DAT0);
	data <<= 8;
	data |= SMB_RD_REG(SMB_HST_DAT1);

	return data;
}
	
//=============================================================================
//  smb_read_byte_data_nvidia
//=============================================================================
uint8_t smb_read_byte_data_nvidia(uint8_t addr, uint8_t cmd)
{
	uint32_t	loops;

	SMB_WR_REG(0x2, addr | SMB_RW_READ);	// slave address + read
	smb_delay();

	SMB_WR_REG(0x3, cmd);	// device register address
	smb_delay();
	
	SMB_WR_REG(0x0, 0x07);
	smb_delay();

	loops = 1000000;
	while (loops--)
	{
		if (SMB_RD_REG(0x1))
			break;
		smb_delay();
	}
	
	return SMB_RD_REG(0x4);
}

//=============================================================================
//  smb_find_dev
//=============================================================================
uint8_t	smb_find_dev(uint8_t addr)
{
	int			i;
	smb_dev_t	*dev;

	if (smb->num_dev == 0)
		return SMB_ERR_DEV_NOT_FOUND;
	
	dev = smb->dev;
	for (i=0; i<smb->num_dev; i++)
	{
		if (dev->addr == addr)
			return SMB_OK;

		dev = dev->next;
	}

	return SMB_ERR_DEV_NOT_FOUND;
}

//=============================================================================
//  smb_status
//=============================================================================
uint32_t smb_status(void)
{
	if (!smb)
		return 0;

	return smb->status;
}

//=============================================================================
//  smb_amd_select_port
//=============================================================================
void smb_amd_select_port(uint8_t port)
{
	uint8_t		data8;
	uint8_t		dirty;

	// AMD SMBus Selection @ PMIO Reg 0x00 bit[20:19]
	// (PMIO:0xFED80300) @ bit[20:19]
	// (PMIO:Port IDX=CD7h,DAT=CD8h) Reg[02] bit[4:3]

	// select SMBus Port 0
	outp(0xCD6, 0x02);
	data8 = inp(0xCD7);
	dirty = 0;
	
	if (port)
	{
		// Port 2 : TSI
		if ((data8 & 0x18) == 0x00)
		{
			data8 |= 0x08;		// 01=Port 2
			dirty = 1;
		}
	}
	else
	{
		// Port 0
		if ((data8 & 0x18) != 0x00)
		{
			data8 &= ~0x18;		// 00=Port 0
			dirty = 1;
		}
	}

	if (dirty)
	{
		outp(0xCD6, 0x02);
		outp(0xCD7, data8);
	}
}

//=============================================================================
//  smb_init
//=============================================================================
uint8_t smb_init(void)
{
	pci_dev_t	*pd = NULL;
	pci_dev_t	*ps = NULL;
	uint8_t		err;
	uint8_t		data;
	
	smb = (smb_bus_t *)malloc(sizeof(smb_bus_t));
	if (!smb)
		return SMB_ERR_MALLOC_BUS;

	memset(smb, 0, sizeof(smb_bus_t));
	
	// need to scan pci bus for finding SMBus controller
	if (pci_status() != 0x55AA33CC)
	{
		err = SMB_ERR_PCI_INIT;
		goto err_smb_init;
	}

	// get platform vendor : bus=0, dev=0, fun=0
	if ((pd=pci_find_bus_dev_fun(0, 0, 0)) == NULL)
	{
		err = SMB_ERR_PLAT_NOT_FOUND;
		goto err_smb_init;
	}
	
	smb->plat_id = pd->cfg.ven_id;

	// AMD    SMBus : Bus 0, Device 31, Function 3 = 8086:8C22 (DPX-S435)
	// Intel  SMBus : Bus 0, Device 20, Function 0 = 1002:4385 (DPX-E130)
	// nVidia SMBus : Bus 0, Device  3, Function 2 = 10DE:XXXX

	switch(smb->plat_id)
	{
		case 0x8086:	// Intel
		case 0x1002:	// ATI
		case 0x1022:	// AMD

			// serial controller - SMBus controller
			if ((ps=pci_find_cla_subcla(0x0C, 0x05)) == NULL)
			{
				err = SMB_ERR_CLA_NOT_FOUND;
				goto err_smb_init;
			}

			smb->ven_id = ps->cfg.ven_id;
			smb->dev_id = ps->cfg.dev_id;

			smb->pcidev	= ps;
			
			// check mem/io enable
			if ((ps->cfg.cmd & 0x3) != 0x3)
			{
				pci_sel_dev(ps->bus, ps->dev, ps->fun);
				data = pci_read_byte(0x4);	// command+status
				data |= 0x3;	// enable mem/io
				pci_write_byte(0x04, data);
			}

			if (smb->ven_id == 0x8086)
			{
				// Intel
				smb->base	= (uint16_t)(ps->cfg.bar[4] & 0xffe0);	// 20h
				smb->found	= 1;
			}
			else if (smb->ven_id == 0x1022)
			{
				// default select port 0
				smb_amd_select_port(0);

				// AMD
				smb->base	= (uint16_t)(0xb00);
				smb->found	= 1;
			}
			else if (smb->ven_id == 0x1002)
			{
				// ATi
				smb->base	= (uint16_t)(ps->cfg.bar[0] & 0xffe0);	// 10h
				smb->found	= 1;
			}

			if (smb->found)
			{
				// scan bus : too slow here
				/*
				if (scan)
				{
					err = smb_scan_bus();
					if (err != SMB_OK)
						goto err_smb_init;
				}
				*/
				
				//smb->quick_command = smb_quick_command;
				//smb->receive_byte = smb_receive_byte;
				//smb->read_byte	= smb_read_byte;
				//smb->read_word	= smb_read_word;
				//smb->write_byte	= smb_write_byte;
				//smb->write_word	= smb_write_word;
				//smb->proc_call	= smb_proc_call;

				if (smb->ven_id == 0x1022)
				{
					smb->quick_command		= smb_quick_command_ati;
					smb->read_byte			= smb_read_byte_ati;
					smb->read_byte_data		= smb_read_byte_data_ati;
				}
				else
				{
					smb->quick_command		= smb_quick_command;
					smb->read_byte			= smb_read_byte;
					smb->read_byte_data		= smb_read_byte_data;
				}
				smb->write_byte_data	= smb_write_byte_data;
				smb->read_word_data		= smb_read_word_data;
				smb->write_word_data	= smb_write_word_data;
				smb->process_call		= smb_process_call;
				smb->read_block_data	= smb_read_block_data;
				smb->write_block_data	= smb_write_block_data;
			}
			
			break;

		case 0x10DE:	// nVidia

			// serial controller - SMBus controller
			if ((ps=pci_find_cla_subcla(0x0C, 0x05)) == NULL)
			{
				err = SMB_ERR_CLA_NOT_FOUND;
				goto err_smb_init;
			}
			

			smb->ven_id = ps->cfg.ven_id;
			smb->dev_id = ps->cfg.dev_id;
			
			if (smb->ven_id == 0x10DE)
			{
				// Intel
				smb->base	= (uint16_t)(ps->cfg.bar[5] & 0xfffe);	// 24h
				smb->found	= 1;
			}
				
			if (smb->found)
			{
				// scan bus : too slow here
				/*
				if (scan)
				{
					err = smb_scan_bus_nvidia();
					if (err != SMB_OK)
						goto err_smb_init;
				}
				*/
				/*
				smb->receive_byte = smb_receive_byte;
				smb->read_byte	= smb_read_byte_nvidia;
				smb->read_word	= smb_read_word;
				smb->write_byte	= smb_write_byte;
				smb->write_word	= smb_write_word;
				smb->proc_call	= smb_proc_call;
				*/

				smb->quick_command		= smb_quick_command;
				smb->read_byte			= smb_read_byte;
				smb->read_byte_data		= smb_read_byte_data_nvidia;
				smb->write_byte_data	= smb_write_byte_data;
				smb->read_word_data		= smb_read_word_data;
				smb->write_word_data	= smb_write_word_data;
				smb->process_call		= smb_process_call;
				smb->read_block_data	= smb_read_block_data;
				smb->write_block_data	= smb_write_block_data;
			}

			break;

		default:
			break;
	}

	smb->status = 0x55AA33CC;	// initiailized

	return SMB_OK;

err_smb_init:

	smb->status = 0;

	smb_exit();

	return err;
}

//=============================================================================
//  smb_exit
//=============================================================================
uint8_t smb_exit(void)
{
	smb_dev_t	*sd;
	smb_dev_t	*sdev;
	int			i;

	if (!smb)
		return SMB_ERR_NULL_BUS;

	// smb device
	if (smb->dev)
	{
		for (i=0, sdev=smb->dev; i<smb->num_dev; i++)
		{
			sd		= sdev;
			sdev	= sd->next;
			if (sd)
				free(sd);
		}
	}

	free(smb);

	smb = NULL;

	return SMB_OK;
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - IMC (Integrated Memory Controller)                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <dos.h>

#include "typedef.h"
#include "x86io.h"
#include "pci.h"
#include "imc.h"

//=============================================================================
//  variables
//=============================================================================
imc_info_t	*imc = NULL;

//=============================================================================
//  imc_sel_ch
//=============================================================================
void imc_sel_ch(uint8_t ch)
{
	imc->ch_no = !!ch;
}

//=============================================================================
//  imc_read_dword
//=============================================================================
uint32_t imc_read_dword(uint16_t reg)
{
#if 0
	return *(uint32_t *)(imc->pci.mmio + reg);
#else
	uint32_t d32;
	imc->pci.IoDev->Pci.Read(imc->pci.IoDev, EfiPciWidthUint32, EFI_PCI_ADDRESS(imc->pci.bus, imc->pci.dev, imc->pci.fun, reg), 1, &d32);
	return d32;
#endif
}

//=============================================================================
//  imc_write_dword
//=============================================================================
void imc_write_dword(uint16_t reg, uint32_t data)
{
#if 0
	*(uint32_t *)(imc->pci.mmio + reg) = data;
#else
	imc->pci.IoDev->Pci.Write(imc->pci.IoDev, EfiPciWidthUint32, EFI_PCI_ADDRESS(imc->pci.bus, imc->pci.dev, imc->pci.fun, reg), 1, &data);
#endif
}

//=============================================================================
//  imc_can_off_tsod
//=============================================================================
static uint8_t imc_can_off_tsod(void)
{
	uint32_t	bak;
	uint32_t	ctl;
	uint8_t		ch = imc->ch_no;

	bak = imc_read_dword(IMC_REG_SMBCTL(ch));
	ctl = bak & ~IMC_SMBCTL_TSOD_POLL_EN;
	imc_write_dword(IMC_REG_SMBCTL(ch), ctl);

	ctl = imc_read_dword(IMC_REG_SMBCTL(ch));
	if (ctl & IMC_SMBCTL_TSOD_POLL_EN)
		return 0;

	// restore
	imc_write_dword(IMC_REG_SMBCTL(ch), bak);

	return 1;
}

//=============================================================================
//  imc_can_write
//=============================================================================
static uint8_t imc_can_write(void)
{
	uint8_t		ch = imc->ch_no;

	if (imc_read_dword(IMC_REG_SMBCTL(ch)) & IMC_SMBCTL_DIS_WRT)
		return 0;
	return 1;
}

//=============================================================================
//  imc_reset
//=============================================================================
/*
static void imc_reset(uint8_t ch)
{
	uint32_t	ctl;

	ctl = imc_read_dword(IMC_REG_SMBCTL(ch));
	ctl |= IMC_SMBCTL_SOFT_RST;
	imc_write_dword(IMC_REG_SMBCTL(ch), ctl);

	delay(1);

	ctl &= ~IMC_SMBCTL_SOFT_RST;
	imc_write_dword(IMC_REG_SMBCTL(ch), ctl);
}
*/

//=============================================================================
//  imc_wait_not_busy
//=============================================================================
static uint8_t imc_wait_not_busy(uint32_t *status)
{
	int			i;
	uint32_t	sts;
	uint8_t		ch = imc->ch_no;

	// 16777216
	for (i=0; i<0xFFFFFF; i++)
	{
		sts = imc_read_dword(IMC_REG_SMBSTS(ch));
		*status = sts;
		if (!(sts & IMC_SMBSTS_BUSY))
			return IMC_OK;
	}

	return IMC_ERR_TMOUT;
}

//=============================================================================
//  imc_smbus_xfer
//=============================================================================
//  ch      - channel (0 or 1)
//  addr    - slave address (7-bit)
//  rw      - R/W (1=R, 0=W)
//  command - command code
//  size    - byte or word
//  *data   - data buffer for R/W
//=============================================================================
static uint8_t imc_smbus_xfer(uint16_t addr, uint8_t rw, uint8_t command, uint8_t size, uint16_t *data)
{
	uint32_t	cmd;
	uint16_t	dat;
	uint32_t	ctl;
	uint32_t	sts;
	uint8_t		ret;
	uint32_t	final_cmd;
	uint32_t	final_ctl;
	uint8_t		ch = imc->ch_no;
	uint32_t cnt = 0;

	cmd = 0;
	cmd |= ((uint32_t)addr & 0x7) << IMC_SMBCMD_SA_SHFT;
	cmd |= ((uint32_t)command) << IMC_SMBCMD_BA_SHFT;
	if (size == IMC_SMBUS_WORD)
	{
		cmd |= IMC_SMBCMD_WORD_ACCESS;
	}

	if (rw == IMC_SMBUS_READ)
	{
		// read
		cmd &= ~IMC_SMBCMD_WRT_CMD;
	}
	else
	{
		// write
		cmd |= IMC_SMBCMD_WRT_CMD;
		if (size == IMC_SMBUS_WORD)
		{
			// word : swap 16
			dat = *data & 0xFF;
			dat <<= 8;
			dat |= (*data >> 8);
		}
		else
		{
			// read
			dat = *data;
		}
		cmd |= (uint32_t)dat;
	}

	// disable TSOD
	ctl = imc_read_dword(IMC_REG_SMBCTL(ch));
	imc->prev_tsod_poll = !!(ctl & IMC_SMBCTL_TSOD_POLL_EN);
	while(ctl & IMC_SMBCTL_TSOD_POLL_EN)
	{
		cnt++;
		if(cnt >= 0xFFFFFF)	// 16777216
			return IMC_ERR_BUSY;
		
		ctl &= ~IMC_SMBCTL_TSOD_POLL_EN;
		imc_write_dword(IMC_REG_SMBCTL(ch), ctl);

		// sometimes the hw won't let go ?
		ctl = imc_read_dword(IMC_REG_SMBCTL(ch));
	}

	// wait for ready
	ret = imc_wait_not_busy(&sts);
	if (ret != IMC_OK)
		return ret;

	// DTI : device identifier : 0x50 -> 101...
	ctl = imc_read_dword(IMC_REG_SMBCTL(ch));
	ctl &= ~IMC_SMBCTL_DTI_MASK;
	ctl |= ((uint32_t)addr >> 3) << IMC_SMBCTL_DTI_SHFT;
	imc_write_dword(IMC_REG_SMBCTL(ch), ctl);

	// trigger
	cmd |= IMC_SMBCMD_TRIGGER;
	imc_write_dword(IMC_REG_SMBCMD(ch), cmd);

	// wait for ready
	ret = imc_wait_not_busy(&sts);
	if (ret != IMC_OK)
		return ret;

	// final check
	final_cmd = imc_read_dword(IMC_REG_SMBCMD(ch));
	final_ctl = imc_read_dword(IMC_REG_SMBCTL(ch));

	if (((cmd ^ final_cmd) & IMC_SMBCMD_OUR_BITS) || ((ctl ^ final_ctl) & IMC_SMBCTL_OUR_BITS))
	{
		return IMC_ERR_IO;
	}

	// SMBus error
	if (sts & IMC_SMBSTS_SBE)
	{
		// clear error bit
		imc_write_dword(IMC_REG_SMBSTS(ch), 0);
		return IMC_ERR_BUS;
	}

	if (rw == IMC_SMBUS_READ)
	{
		// SMBus read
		if ((sts & IMC_SMBSTS_RDO) == 0)
			return IMC_ERR_READ;

		if (size == IMC_SMBUS_WORD)
		{
			// read word 
			dat = (uint16_t)(sts & IMC_SMBSTS_RDATA_MASK);

			*data = dat & 0xFF;
			*data <<= 8;
			*data |= (dat >> 8);
		}
		else
		{
			// read byte
			dat = (uint16_t)(sts & IMC_SMBSTS_RDATA_MASK);

			*data = dat & 0xFF;
		}
	}
	else
	{
		// SMBus write
		if ((sts & IMC_SMBSTS_WOD) == 0)
			return IMC_ERR_WRITE;
	}

	return IMC_OK;
}

//=============================================================================
//  imc_set_spd_page
//=============================================================================
void imc_set_spd_page(uint8_t page)
{
	uint16_t	data = 0;

	if (page == 0)
	{
		imc_smbus_xfer(0x36, IMC_SMBUS_WRITE, 0x00, IMC_SMBUS_BYTE, &data);
	}
	else
	{
		imc_smbus_xfer(0x37, IMC_SMBUS_WRITE, 0x00, IMC_SMBUS_BYTE, &data);
	}

	delay(100);
}
		
//=============================================================================
//  imc_probe_spd
//=============================================================================
void imc_probe_spd(void)
{
	int			slot;
	uint8_t		i, addr, ret, n;
	uint16_t	data;
	uint32_t	sts;

	// clear info
	imc->spd_num = 0;
	for (i=0; i<16; i++)
	{
		imc->spd[i].ch = 0xFF;
		imc->spd[i].addr = 0xFF;
	}

	for (i=0; i<2; i++)
	{
		imc_sel_ch(i);

		for (slot=0; slot<8; slot++)
		{
			// SPD : 0x50~0x57, 7-bit slave address
			addr = (uint8_t)(0x50|slot);
			ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, 0x00, IMC_SMBUS_BYTE, &data);
			if (ret != IMC_OK)
				continue;

			sts = imc_read_dword(IMC_REG_SMBSTS(i));
			if ((sts & 0xC000FF00) != 0xC0000000)
				continue;
		
			// debug
			/*
			fprintf(stderr, "[%d] %08X %08X %08X %08X\n",
					slot,
					imc_read_dword(IMC_REG_SMBSTS(ch)),
					imc_read_dword(IMC_REG_SMBCMD(ch)),
					imc_read_dword(IMC_REG_SMBCTL(ch)),
					imc_read_dword(IMC_REG_SMBTCNT(ch)));
			*/

			n = imc->spd_num;
			imc->spd[n].ch = i;
			imc->spd[n].addr = addr;
			imc->spd_num++;
		}
	}
}

//=============================================================================
//  imc_probe_tsod
//=============================================================================
void imc_probe_tsod(void)
{
	int			slot;
	uint8_t		i, addr, ret, n;
	uint16_t	data;
	uint32_t	sts;

	// clear info
	imc->tsod_num = 0;
	for (i=0; i<16; i++)
	{
		imc->tsod[i].ch = 0xFF;
		imc->tsod[i].addr = 0xFF;
	}

	for (i=0; i<2; i++)
	{
		imc_sel_ch(i);

		for (slot=0; slot<8; slot++)
		{
			// TSOD : 0x18~0x1F, 7-bit slave address
			// temperature sensor on DIMM
			addr = (uint8_t)(0x18|slot);
			ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, 0x00, IMC_SMBUS_BYTE, &data);
			if (ret != IMC_OK)
				continue;

			// todo : need further confirm
			sts = imc_read_dword(IMC_REG_SMBSTS(i));
			if ((sts & 0xC000FF00) != 0xC0000000)
				continue;
		
			n = imc->tsod_num;
			imc->tsod[n].ch = i;
			imc->tsod[n].addr = addr;
			imc->tsod_num++;
		}
	}
}

//=============================================================================
//  imc_scan_spd
//=============================================================================
uint8_t imc_scan_spd(uint8_t ch, uint8_t addr)
{
	uint8_t		i;

	if (imc->spd_num == 0)
		return IMC_ERR_SPD_NOT_FOUND;
	
	for (i=0; i<imc->spd_num; i++)
	{
		if ((imc->spd[i].ch == ch ) && (imc->spd[i].addr == addr))
			return IMC_OK;
	}
	
	return IMC_ERR_SPD_NOT_FOUND;
}

//=============================================================================
//  imc_scan_tsod
//=============================================================================
uint8_t imc_scan_tsod(uint8_t ch, uint8_t addr)
{
	uint8_t		i;

	if (imc->tsod_num == 0)
		return IMC_ERR_TSOD_NOT_FOUND;
	
	for (i=0; i<imc->tsod_num; i++)
	{
		if ((imc->tsod[i].ch == ch ) && (imc->tsod[i].addr == addr))
			return IMC_OK;
	}
	
	return IMC_ERR_TSOD_NOT_FOUND;
}

//=============================================================================
//  imc_read_spd
//=============================================================================
uint16_t imc_read_spd(uint8_t addr, uint8_t *buf)
{
	uint8_t		ret;
	uint16_t	data;
	int			i;
	uint16_t	err = 0;
	uint8_t		ch = imc->ch_no;

	for (i=0; i<256; )
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, (uint8_t)i, IMC_SMBUS_BYTE, &data);
		if (ret == IMC_OK)
		{
			buf[i] = (uint8_t)(data & 0xFF);
			i++;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
			err++;
		}
	}

	return err;
}

//=============================================================================
//  imc_read_tsod
//=============================================================================
uint16_t imc_read_tsod(uint8_t addr, uint16_t *buf)
{
	uint8_t		ret;
	uint16_t	data;
	int			i;
	uint16_t	err = 0;
	uint8_t		ch = imc->ch_no;

	for (i=0; i<16; )
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, (uint8_t)i, IMC_SMBUS_WORD, &data);
		if (ret == IMC_OK)
		{
			buf[i] = data;
			i++;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
			err++;
		}
	}

	return err;
}

//=============================================================================
//  imc_read_byte
//=============================================================================
uint8_t imc_read_byte(uint8_t addr, uint8_t cmd)
{
	uint8_t		ret, data;
	uint16_t	cnt, data16;
	uint8_t		ch = imc->ch_no;

	cnt = 1000;
	do
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, cmd, IMC_SMBUS_BYTE, &data16);
		if (ret == IMC_OK)
		{
			data = (uint8_t)(data16 & 0xFF);
			break;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
			data = 0;
		}
	} while (--cnt);

	return data;
}

//=============================================================================
//  imc_write_byte
//=============================================================================
uint8_t imc_write_byte(uint8_t addr, uint8_t cmd, uint8_t data)
{
	uint8_t		ret;
	uint16_t	cnt, data16;
	uint8_t		ch = imc->ch_no;

	data16 = (uint8_t)data;
	cnt = 1000;
	do
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_WRITE, cmd, IMC_SMBUS_BYTE, &data16);
		if (ret == IMC_OK)
		{
			data = (uint8_t)(data16 & 0xFF);
			break;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
			data = 0;
		}
	} while (--cnt);

	return ret;
}

//=============================================================================
//  imc_read_word
//=============================================================================
uint16_t imc_read_word(uint8_t addr, uint8_t cmd)
{
	uint8_t		ret;
	uint16_t	cnt, data16;
	uint8_t		ch = imc->ch_no;

	cnt = 1000;
	do
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_READ, cmd, IMC_SMBUS_WORD, &data16);
		if (ret == IMC_OK)
		{
			break;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
		}
	} while (--cnt);

	return data16;
}

//=============================================================================
//  imc_write_word
//=============================================================================
uint8_t imc_write_word(uint8_t addr, uint8_t cmd, uint16_t data)
{
	uint8_t		ret;
	uint16_t	cnt, data16;
	uint8_t		ch = imc->ch_no;

	data16 = data;
	cnt = 1000;
	do
	{
		ret = imc_smbus_xfer(addr, IMC_SMBUS_WRITE, cmd, IMC_SMBUS_WORD, &data16);
		if (ret == IMC_OK)
		{
			break;
		}
		else
		{
			// todo : clear error bit ?
			imc_write_dword(IMC_REG_SMBSTS(ch), 0);
			data = 0;
		}
	} while (--cnt);

	return ret;
}

//=============================================================================
//  imc_exit
//=============================================================================
void imc_exit(void)
{
	memset(imc, 0, sizeof(imc_info_t));
	if (imc)
		free(imc);
	imc = NULL;
}

//=============================================================================
//  imc_init
//=============================================================================
uint8_t imc_init(void)
{
	pci_dev_t	*pcid = NULL;
	int			i;

	// platform supporting list :
	// platform id : vendor_id + device_id
	uint16_t	imc_id[4][2] =
	{
		// ff.13.0 = 8086:6FA8 (broadwell de) : iMC Memory Controller
		{ 0x8086, 0x6FA8 },
		// ff.0f.0 = 8086:3CA8 (sandy bridge) : iMC Memory Controller
		{ 0x8086, 0x3CA8 },
		// ff.xx.x = end of search
		{ 0xFFFF, 0xFFFF },
	};

	if (pci_init() != PCI_OK)
		return IMC_ERR_PCI;

	for (i=0; i<3; i++)
	{
		// end of search
		if (imc_id[i][0] == 0xFFFF)
		{
			pcid = NULL;
			break;
		}

		// [0] : vendor id
		// [1] : device id
		pcid = pci_find_ven_dev(imc_id[i][0], imc_id[i][1]);
		if (pcid)
			break;
	}

	// platform id not found
	if (!pcid)
		return IMC_ERR_PCI_DEV;

	if (imc)
		imc_exit();

	// allocation
	imc = (imc_info_t *)malloc(sizeof(imc_info_t));
	if (!imc)
		return IMC_ERR_MALLOC;

	memset(imc, 0, sizeof(imc_info_t));

	// load parameters
	pci_set_dev_info(pcid, &imc->pci);
	
	for (i=0; i<2; i++)
	{
		imc_sel_ch((uint8_t)i);

		if (imc_can_off_tsod())
			imc->ch[i].can_off_tsod = 1;
		else
			imc->ch[i].can_off_tsod = 0;

		if (imc_can_write())
			imc->ch[i].can_write = 1;
		else
			imc->ch[i].can_write = 0;
	}

	imc->sts = IMC_OK;

	return IMC_OK;
}

//*****************************************************************************
//*****************************************************************************
//*
//*   Debug IMC
//*
//*****************************************************************************
//*****************************************************************************
#if DEBUG_X86_IMC
#include "spd.h"

//=============================================================================
//  dump_spd
//=============================================================================
void dump_spd(uint8_t *buf)
{
	int			i, j;
	uint8_t		c;

	for (i=0; i<256; i++)
	{
		if ((i&0xF)==0)
			fprintf(stderr, "[%02X] : ", i);
		if ((i&0xF)==8)
			fprintf(stderr, "- ");
		fprintf(stderr, "%02X ", buf[i]);
	
		if ((i&0xF)==15)
		{
			fprintf(stderr, " ");
			for (j=15; j>=0; j--)
			{
				c = buf[i-j];
				if (c >= 0x20 && c <= 0x7E)
					fprintf(stderr, "%c", c);
				else
					fprintf(stderr, ".");
			}
			fprintf(stderr, "\n");
		}
	}
}

//=============================================================================
//  dump_spd_info
//=============================================================================
void dump_spd_info(void)
{
	int		i, j;

	fprintf(stderr, "spd_num : %d\n", imc->spd_num);

	if (imc->spd_num)
	{
		for (j=0; j<2; j++)
		{
			fprintf(stderr, "ch[%d] : ", j);
			for (i=0; i<imc->spd_num; i++)
			{
				if (imc->spd[i].ch == j)
					fprintf(stderr, "%02X ", imc->spd[i].addr);
			}
			fprintf(stderr, "\n");
		}
	}
}
	
//=============================================================================
//  main
//=============================================================================
int main(void)
{
	uint8_t		ret;
	uint8_t		buf[256];
	uint16_t	err;

	ret = imc_init();
	if (ret != IMC_OK)
	{
		fprintf(stderr, "imc_init err! code : 0x%02X", ret);
		return -1;
	}

	fprintf(stderr, "%02X:%02X.%02X = %04X %04X @ 0x%08X\n",
		imc->pci.bus, imc->pci.dev, imc->pci.fun, imc->pci.vid, imc->pci.did, imc->pci.mmio);

	imc_probe_spd();
	dump_spd_info();
	
	fprintf(stderr, "press any key to continue.\n");
	getch();
	fprintf(stderr, "reading spd page 0.....\n");
	
	// ch 0
	imc_sel_ch(0);
	
	// page 0
	// reading spd
	memset(buf, 0, sizeof(buf));
	imc_set_spd_page(0);

	err = imc_read_spd(0x50, buf);
	fprintf(stderr, "err = %d\n", err);
	dump_spd(buf);
	fprintf(stderr, "crc=0x%04X\n", spd_get_crc(buf, &ret));
	fprintf(stderr, "press any key to continue.\n");
	getch();
	fprintf(stderr, "reading spd page 1.....\n");

	// page 1
	memset(buf, 0, sizeof(buf));
	imc_set_spd_page(1);

	err = imc_read_spd(0x50, buf);
	fprintf(stderr, "err = %d\n", err);
	dump_spd(buf);
	fprintf(stderr, "press any key to continue.\n");
	getch();

	imc_exit();

	return 0;
}

#endif



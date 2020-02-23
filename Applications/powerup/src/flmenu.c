//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  FLMENU : Flag Menu                                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
//#include <dos.h>

#include "typedef.h"
#include "t_video.h"
#include "t_vlbox.h"
#include "t_msgbox.h"

#include "abios.h"
#include "aec.h"
#include "e820.h"
#include "pci.h"
#include "smbios.h"
#include "key.h"
#include "rtc.h"

#include "dat.h"
#include "upd.h"
#include "pu.h"

//=============================================================================
//  flmenu_abios_show
//=============================================================================
static void flmenu_abios_show(void)
{
	pu_raw_t	*raw;
	char		msg[1024];
	char		str[256];
	int			i, j, len, idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);

	// abios : model
	raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
	if (raw)
	{
		memcpy(&msg[idx], "\rModel   : ", 11);
		idx += 11;
		
		// model : padding 0x00 in the tail
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
			if (raw->data[i] == 0)
				break;
		}
		len = (int)strlen(str);
		memcpy(&msg[idx], str, len);
		idx += len;

		msg[idx++] = '\n';
	}

	// abios : ver
	raw = raw_find_id(RAW_ID_ACL_BIOS_VER);
	if (raw)
	{
		memcpy(&msg[idx], "\rVersion : ", 11);
		idx += 11;

		// ver : padding 0x00 in the tail
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
			if (raw->data[i] == 0)
				break;
		}
		len = (int)strlen(str);		
		memcpy(&msg[idx], str, len);
		idx += len;

		msg[idx++] = '\n';
	}

	// abios : magic
	raw = raw_find_id(RAW_ID_ACL_BIOS);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
			
			// 32 chars/line
			if ((i & 0x1F) == 0x1F)
			{
				str[j++] = '\n';
				str[j++] = '\r';
			}
		}
		str[j] = 0;

		sprintf(&msg[idx], "\r%s\n", str);
		idx += (int)strlen(str);
		idx += 2;
	}

	msg[idx++] = '\n';

	sprintf(&msg[idx], "\r\b%cBoard >\n", T_ARG_YELLOW);
	idx = (int)strlen(msg);
	
	// abios : model
	len = (int)strlen(abios.model);
	if (len > 0)
	{
		sprintf(&msg[idx], "\rModel   : %s\n", abios.model);
		idx += (int)strlen(abios.model);
		idx += 12;
	}

	// abios : ver
	len = (int)strlen(abios.ver);
	if (len > 0)
	{
		sprintf(&msg[idx], "\rVersion : %s\n", abios.ver);
		idx += (int)strlen(abios.ver);
		idx += 12;
	}
	
	// abios : magic
	if (pu->abios.magic[0])
	{
		len = (int)strlen(pu->abios.magic);
		
		for (i=0, j=0; i<len; i++)
		{
			str[j++] = pu->abios.magic[i];
			
			// 32 chars/line
			if ((i & 0x1F) == 0x1F)
			{
				str[j++] = '\n';
				str[j++] = '\r';
			}
		}
		str[j] = 0;

		sprintf(&msg[idx], "\r%s\n", str);
		idx += (int)strlen(str);
		idx += 2;
	}

	msg[idx] = 0;
	
	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - AB : ACL BIOS", msg, 4, 1);
}

//=============================================================================
//  flmenu_aec_show
//=============================================================================
static void flmenu_aec_show(void)
{
	pu_raw_t	*raw;
	char		msg[1024];
	char		str[128];
	int			i, j, idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);
	
	raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
		}
		str[j] = 0;

		sprintf(&msg[idx], "\rChip    : %s\n", str);
		idx += (int)strlen(str);
		idx += 12;
	}

	raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
		}
		str[j] = 0;

		sprintf(&msg[idx], "\rBoard   : %s\n", str);
		idx += (int)strlen(str);
		idx += 12;
	}

	raw = raw_find_id(RAW_ID_ACL_EC_VER);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
		}
		str[j] = 0;

		sprintf(&msg[idx], "\rVersion : %s\n", str);
		idx += (int)strlen(str);
		idx += 12;

	}

	msg[idx++] = '\n';

	sprintf(&msg[idx], "\r\b%cBoard >\n", T_ARG_YELLOW);
	idx = (int)strlen(msg);

	if (pu->aec.chip[0])
	{
		sprintf(&msg[idx], "\rChip    : %s\n", pu->aec.chip);
		idx = (int)strlen(msg);
	}

	if (pu->aec.board[0])
	{
		sprintf(&msg[idx], "\rBoard   : %s\n", pu->aec.board);
		idx = (int)strlen(msg);
	}

	if (pu->aec.ver[0])
	{
		sprintf(&msg[idx], "\rVersion : %s\n", pu->aec.ver);
		idx = (int)strlen(msg);
	}

	msg[idx] = 0;

	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - AEC : ACL EC", msg, 4, 1);
}

//=============================================================================
//  flmenu_cpu_show
//=============================================================================
static void flmenu_cpu_show(void)
{
	pu_raw_t	*raw;
	char		msg[1024];
	char		str[128];
	int			i, j, idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);

	raw = raw_find_id(RAW_ID_CPU_VENDOR);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
		}
		str[j] = 0;

		sprintf(&msg[idx], "\rVendor : %s\n", str);
		idx += (int)strlen(str);
		idx += 11;
	}

	raw = raw_find_id(RAW_ID_CPU_NAME);
	if (raw)
	{
		for (i=0, j=0; (uint32_t)i<raw->len; i++)
		{
			str[j++] = raw->data[i];
		}
		str[j] = 0;

		sprintf(&msg[idx], "\rName   : \n\r%s\n", str);
		idx += (int)strlen(str);
		idx += 13;
	}

/*
	raw = raw_find_id(RAW_ID_CPU_CLOCK);
	if (raw)
	{
		sprintf(str0, "%d MHz", *(uint32_t *)raw->data);
		
		memcpy(&msg[idx], str0, strlen(str0));
		idx += strlen(str0);
		msg[idx++] = '\n';
		msg[idx++] = '\n';
	}
*/
	msg[idx++] = '\n';

	sprintf(&msg[idx], "\r\b%cBoard >\n", T_ARG_YELLOW);
	idx = (int)strlen(msg);

	if (pu->cpu.vendor[0])
	{
		sprintf(&msg[idx], "\rVendor : %s\n", pu->cpu.vendor);
		idx = (int)strlen(msg);
	}

	if (pu->cpu.name[0])
	{
		sprintf(&msg[idx], "\rName   : \n\r%s\n", pu->cpu.name);
		idx = (int)strlen(msg);
	}

/*
	sprintf(str0, "%d MHz", pu->cpu.clock);
	memcpy(&msg[idx], str0, strlen(str0));
	idx += strlen(str0);
	msg[idx++] = 0;
*/
	
	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - CP : CPU Info", msg, 4, 1);
}

//=============================================================================
//  flmenu_e820_show
//=============================================================================
static void flmenu_e820_show(void)
{
	pu_raw_t	*raw;
	char		msg[256];
	int			idx;


	memset(msg, 0, sizeof(msg));
	idx = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);
	
	raw = raw_find_id(RAW_ID_MEM_E820_NUM);
	if (raw)
	{
		sprintf(&msg[idx], "\rNum  : %d\n", *(uint8_t *)raw->data);
		idx = (int)strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_MEM_E820_KSZ);
	if (raw)
	{
		sprintf(&msg[idx], "\rSize : %d KiB\n", *(uint32_t *)raw->data);
		idx = (int)strlen(msg);
	}

	raw = raw_find_id(RAW_ID_MEM_E820_MSZ);
	if (raw)
	{
		sprintf(&msg[idx], "\rSize : %d MiB\n", *(uint32_t *)raw->data);
		idx = (int)strlen(msg);
	}

	msg[idx++] = '\n';

	sprintf(&msg[idx], "\r\b%cBoard >\n", T_ARG_YELLOW);
	idx = (int)strlen(msg);

	sprintf(&msg[idx], "\rNum  : %d\n", e820.num_item);
	idx = (int)strlen(msg);

	sprintf(&msg[idx], "\rSize : %d KiB\n", e820.kbsz);
	idx = (int)strlen(msg);

	sprintf(&msg[idx], "\rSize : %d MiB\n", e820.mbsz);
	idx = (int)strlen(msg);

	msg[idx] = 0;

	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - E8 : E820 Size", msg, 4, 1);
}

//=============================================================================
//  flmenu_smbios_show
//=============================================================================
static void flmenu_smbios_show(void)
{
	pu_raw_t	*raw;
	char		msg[384];
	int			idx = 0;
	
	smbios_hdr_t	*hdr;
	smbios_v3hdr_t	*v3hdr;

	memset(msg, 0, sizeof(msg));
	
	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);

	raw = raw_find_id(RAW_ID_DMI_SMBIOS_HDR);
	if (raw)
	{
		hdr = (smbios_hdr_t *)raw->data;
		sprintf(&msg[idx], "\r    Addr : 0x%08X    Len  : %d = 0x%X    Ver  : %d.%d\n\rDMI Addr : 0x%08X    Size : 0x%X = %d    Num  : %d",
				hdr, hdr->len, hdr->len,
				hdr->major_ver,hdr->minor_ver,
				hdr->smbios_addr,
				hdr->smbios_len, hdr->smbios_len,
				hdr->smbios_ent_num);
		idx = (int)strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_V3HDR);
	if (raw)
	{
		v3hdr = (smbios_v3hdr_t *)raw->data;
		
		sprintf(&msg[idx], "\n\r V3 Addr : 0x%08X    Len  : %d = 0x%X    Ver  : %d.%d\n\rDMI Addr : 0x%08X    Size : %d    Doc  : %d    Rev  : %d",
				v3hdr, v3hdr->len, v3hdr->len,
				v3hdr->major_ver, v3hdr->minor_ver,
				v3hdr->tab_addr,
				v3hdr->tab_sz,
				v3hdr->doc_rev,
				v3hdr->eps_rev);
	}

	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - SM : SMBIOS Data", msg, 4, 0);
}

//=============================================================================
//  flmenu_pci_show
//=============================================================================
static void flmenu_pci_show(void)
{
	int			sx, sy, sw, sh;
	int			num, num_raw;
	pu_raw_t	*raw;
	pci_dev_t	*pd = NULL;
	uint8_t		*raw_bdf;
	uint8_t		*raw_vdid;
	int			p, num_page;
	int			i, j;
	char		msg[128];
	uint8_t		fg, bg, found;
	
	uint8_t		*bdf = NULL, bus, dev, fun;
	uint16_t	*vdid = NULL, ven_id, dev_id, key;
	char		*title = "Check Flag - PC : PCI Scan";

	raw_bdf	= NULL;
	raw_vdid= NULL;

	fg = T_WHITE;
	bg = T_BLACK;

	raw = raw_find_id(RAW_ID_PCI_DEV_NUM);
	if (!raw)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "No PCI num record in DAT file!", 4, 1);
		return;
	}
	else
	{
		num_raw = (int)*(uint32_t *)raw->data;
	}
	
	num = num_raw;

	if (num < pci->num_dev)
		num = pci->num_dev;

	num_page = (num + 15)/16;

	raw = raw_find_id(RAW_ID_PCI_DEV_BDF);
	if (!raw)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "No PCI bdf record in DAT file!", 4, 1);
		return;
	}
	else
	{
		raw_bdf = (uint8_t *)malloc(sizeof(uint8_t) * raw->len);
		if (!raw_bdf)
		{
			msgbox_waitkey(T_WHITE, T_RED, title, "malloc err!", 4, 1);
			return;
		}
		memcpy(raw_bdf, raw->data, raw->len);
	}

	raw = raw_find_id(RAW_ID_PCI_DEV_LST);
	if (!raw)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "No PCI bdf record in DAT file!", 4, 1);
		return;
	}
	else
	{
		raw_vdid = (uint8_t *)malloc(sizeof(uint8_t) * raw->len);
		if (!raw_vdid)
		{
			msgbox_waitkey(T_WHITE, T_RED, title, "malloc err!", 4, 1);
			return;
		}
		memcpy(raw_vdid, raw->data, raw->len);
	}


	sx = 7;
	sy = 2;
	sw = 67 + 1;	// shadow
	sh = 20 + 1;	// shadow

	t_copy_region(sx, sy, sx+sw, sy+sh);
	
	p = 0;

	while (1)
	{
		// point to dev
		//pd = pci->dev;
		//if (p > 0)
		//{
		//	for (j=0; j<p*16; j++)
		//		pd = pd->next;
		//}

		// canvas
		t_color(fg, bg);
		t_put_win_single_double(sx, sy, sw, sh);

		for (i=sx+1; i<=sx+sw; i++)
			t_shadow_char(i, sy+sh);	// shadow : bottom border

		for (i=sy+1; i<=sy+sh; i++)
			t_shadow_char(sx+sw+0, i);	// shadow : right border

		sprintf(msg, "PC : PCI Scan Flag : %d - %d", num_raw, pci->num_dev);
		t_xy_puts(sx+1+(64-(int)strlen(msg))/2, 3, msg);
		
		sprintf(msg, " Page: %d/%d ", p+1, num_page);
		t_xy_puts(sx+sw-14, 3, msg);

		t_xy_puts(sx+3, 5, "No  B  D  F   Ven  Dev (File)      B  D  F   Ven  Dev (Board)");

		//   No  B  D  F   Ven  Dev (File)      B  D  F   Ven  Dev (Board)
		//0         1         2         3         4         5         6
		//0123456789012345678901234567890123456789012345678901234567890123456
		//| 999  XX:XX.XX  xxxx xxxx            XX:XX.XX  xxxx xxxx         |
		
		// idx
		for (i=0; i<16; i++)
		{
			if ((p*16+i) >= num)
				break;

			sprintf(msg, "%4d", p*16+i+1);
			t_xy_puts(sx+1, 6+i, msg);
		}

		// file
		for (i=0; i<16; i++)
		{
			if ((p*16+i) >= num_raw)
				break;

			if (i==0)
			{
				bdf = &raw_bdf[p*16*3];
				vdid = (uint16_t *)raw_vdid;
				if (p > 0)
				{
					for (j=0; j<p*16*2; j++)
						vdid++;
				}
			}
			
			bus = *bdf++;		// bus
			dev = *bdf++;		// dev
			fun = *bdf++;		// fun
			ven_id = *vdid++;		// vid
			dev_id = *vdid++;		// did
			
			
			sprintf(msg, "%02X:%02X.%02X  %04X %04X", bus, dev, fun, ven_id, dev_id);

			pd = pci_find_bus_dev_fun_vid_did(bus, dev, fun, ven_id, dev_id);
			if (pd)
				t_xy_cl_puts(sx+7, 6+i, T_LIGHTGRAY, bg, msg);	// found in pci bus
			else
				t_xy_cl_puts(sx+7, 6+i, T_YELLOW, bg, msg);	// not found in pci bus
		}

		// board
		for (i=0; i<16; i++)
		{
			if ((p*16+i) >= pci->num_dev)
				break;

			if (i==0)
			{
				pd = pci->dev;
				if (p > 0)
				{
					for (j=0; j<p*16; j++)
						pd = pd->next;
				}
			}
		
			sprintf(msg, "%02X:%02X.%02X  %04X %04X",
				pd->bus, pd->dev, pd->fun,
				pd->cfg.ven_id, pd->cfg.dev_id);

			// search raw list
			bdf = (uint8_t *)raw_bdf;
			vdid = (uint16_t *)raw_vdid;
			for (j=0, found=0; j<num_raw; j++)
			{
				bus = *bdf++;		// bus
				dev = *bdf++;		// dev
				fun = *bdf++;		// fun
				ven_id = *vdid++;		// vid
				dev_id = *vdid++;		// did

				if ((bus == pd->bus) &&	(dev == pd->dev) &&	(fun == pd->fun) &&
					(ven_id == pd->cfg.ven_id) && (dev_id == pd->cfg.dev_id))
				{
					found = 1;
					break;
				}
			}
				
			if (found)
				t_xy_cl_puts(sx+38, 6+i, T_LIGHTGRAY, bg, msg);
			else
				t_xy_cl_puts(sx+38, 6+i, T_YELLOW, bg, msg);
			
			pd = pd->next;

			if (!pd)
				break;	// last device
		}

		key = (uint16_t)bioskey(0);//key = key_blk_read_sc();

		if (key == (SCAN_PAGE_UP << 8))
		{
			// KEY : PGUP = last page
			p--;
			if (p < 0)
				p = num_page - 1;	// roll back last page
		}
		else if (key == (SCAN_PAGE_DOWN << 8) || key == ' ')
		{
			// KEY : PGDN = next page
			p++;
			if (p >= num_page)
				p = 0;
		}
		else if (key == (SCAN_HOME << 8))
		{
			p = 0;
		}
		else if (key == (SCAN_END << 8))
		{
			p = num_page - 1;
		}
		else if (key == CHAR_CARRIAGE_RETURN || key == (SCAN_ESC << 8))
		{
			break;
		}
	}

	t_paste_region(-1, -1);
		
	if (raw_bdf)
		free(raw_bdf);

	if (raw_vdid)
		free(raw_vdid);
}

//=============================================================================
//  flmenu_smbus_show
//=============================================================================
static void flmenu_smbus_show(void)
{
	pu_raw_t	*raw;
	char		msg[1024];
	int			i, idx;
	uint8_t		num, *ptr;

	memset(msg, 0, sizeof(msg));
	idx = 0;
	num = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);
	
	raw = raw_find_id(RAW_ID_SMB_DEV_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(&msg[idx], "\rNum : %d\n", *(uint8_t *)raw->data);
		idx = (int)strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_SMB_DEV_LST);
	if (raw)
	{
		sprintf(&msg[idx], "\r\b%cDev : \n", T_ARG_LIGHTGREEN);
		idx = (int)strlen(msg);
		
		if (num <= 16)
			idx--;

		ptr = raw->data;
		if (num > 16)
		{
			msg[idx++] = '\r';
			msg[idx++] = '\b';
			msg[idx++] = T_ARG_LIGHTGREEN;
		}

		for (i=0; i<num; i++)
		{
			sprintf(&msg[idx], "%02X ", *ptr++);
			idx = (int)strlen(msg);
			if ((i & 0xF) == 0xF)
			{
				msg[idx++] = '\n';
				msg[idx++] = '\r';
				msg[idx++] = '\b';
				msg[idx++] = T_ARG_LIGHTGREEN;
			}
		}
		msg[idx++] = '\n';
	}

	msg[idx] = 0;

	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - SM : SMBus Scan", msg, 4, 1);

#if 0
	sprintf(&msg[idx], "%s", "\n****** SPD ******\n");
	idx = strlen(msg);

	raw = raw_find_id(RAW_ID_SMB_SPD_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(&msg[idx], "\rNum of SPD : %d\n", num);
		idx = strlen(msg);
	}

	raw = raw_find_id(RAW_ID_SMB_SPD_LST);
	if (raw)
	{
		sprintf(&msg[idx], "%s", "\rSPD List : ");
		idx = strlen(msg);
		ptr = raw->data;
	
		for (i=0; i<num; i++)
		{
			sprintf(&msg[idx], "%02X ", *ptr++);
			idx = strlen(msg);
		}
		msg[idx++] = '\n';
	}
#endif


}

//=============================================================================
//  flmenu_spd_show
//=============================================================================
static void flmenu_spd_show(void)
{
	pu_raw_t	*raw;
	char		msg[1024];
	int			i, idx;
	uint8_t		num, *ptr;

	memset(msg, 0, sizeof(msg));
	idx = 0;
	num = 0;

	sprintf(msg, "\r\b%cDAT File >\n", T_ARG_LIGHTGREEN);
	idx = (int)strlen(msg);
	
	raw = raw_find_id(RAW_ID_SMB_SPD_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(&msg[idx], "\rNum : %d\n", *(uint8_t *)raw->data);
		idx = (int)strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_SMB_SPD_LST);
	if (raw)
	{
		sprintf(&msg[idx], "\r\b%cSPD : \n", T_ARG_LIGHTGREEN);
		idx = (int)strlen(msg);

		if (num <= 16)
			idx--;

		ptr = raw->data;
		if (num > 16)
		{
			msg[idx++] = '\r';
			msg[idx++] = '\b';
			msg[idx++] = T_ARG_LIGHTGREEN;
		}

		for (i=0; i<num; i++)
		{
			sprintf(&msg[idx], "%02X ", *ptr++);
			idx = (int)strlen(msg);
			if ((i & 0xF) == 0xF)
			{
				msg[idx++] = '\n';
				msg[idx++] = '\r';
				msg[idx++] = '\b';
				msg[idx++] = T_ARG_LIGHTGREEN;
			}
		}
		msg[idx++] = '\n';
	}

	msg[idx] = 0;
	
	msgbox_waitkey(T_WHITE, T_BLACK, "Check Flag - SP : SPD Detect", msg, 4, 1);
}

//=============================================================================
//  flmenu_rtc_show
//=============================================================================
static void flmenu_rtc_show(void)
{
	int				i;
	char			str[128];
	uint8_t			cmos_ram[16];
	uint8_t			flag;
	t_msgbox_msg_t	msg;
	uint8_t sec_bak = 0;

	flag = 0;
	while (1)
	{
		if (bioskey(1))
			break;
		
		msgbox_msg_init(&msg);

		sprintf(str, "\r\b%c 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F \n\r\b%c", T_ARG_LIGHTRED, T_ARG_WHITE);
		msgbox_msg_add_str(str, &msg);

		for (i=0; i<16; i++)
			cmos_ram[i] = rtc_read_reg((uint8_t)i);

		for (i=0; i<16; i++)
		{
			sprintf(str, "%02X ", cmos_ram[i]);
			msgbox_msg_add_str(str, &msg);
		}
		msgbox_msg_add_char('\n', &msg);
		msgbox_msg_add_char('\n', &msg);
		
		sprintf(str, "\r\b%cDate  = %02X-%02X-%02X\n", T_ARG_YELLOW, cmos_ram[9], cmos_ram[8], cmos_ram[7]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rWeek  = %02X\n", cmos_ram[6]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rTime  = %02X:%02X:%02X\n", cmos_ram[4], cmos_ram[2], cmos_ram[0]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rAlarm = %02X:%02X:%02X\n", cmos_ram[5], cmos_ram[3], cmos_ram[1]);
		msgbox_msg_add_str(str, &msg);

		msgbox_msg_add_char('\n', &msg);
		
		msgbox_msg_add_char('\r', &msg);
		msgbox_msg_add_char('\b', &msg);
		msgbox_msg_add_char(T_ARG_LIGHTGREEN, &msg);

		sprintf(str, "RTC-0Ah = %02Xh : RTC crystal\n", 0x26);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC-0Bh = %02Xh : RTC status\n", 0x02);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC-0Dh = %02Xh : RTC power (bit7:CMOS_BAT)", 0x80);
		msgbox_msg_add_str(str, &msg);

		if (flag == 0)
		{
			msgbox_progress(T_WHITE, T_BLACK, "Check Flag - RTC : RTC - CMOS RAM", msg.buf, 4, 0);
			flag = 1;
		}
		else
		{
			if(cmos_ram[0] != sec_bak)
			{
				msgbox_progress_cont("Check Flag - RTC : RTC - CMOS RAM", msg.buf);
				sec_bak = cmos_ram[0];
			}
		}

		delay(200);
	}

	msgbox_progress_done();

	key_flush_buf();
	
#if 0
	memset(msg, 0, sizeof(msg));
	idx = 0;
	num = 0;

	for (i=0; i<16; i++)
		cmos_ram[i] = rtc_read_reg(i);
	
	sprintf(msg, "\r%s\n", "RTC (CMOS RAM) >");
	idx = strlen(msg);
	
	sprintf(&msg[idx], "%s", "\r 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F \n");
	idx = strlen(msg);
	
	sprintf(&msg[idx], "\r\b%c", T_ARG_YELLOW);
	idx = strlen(msg);
	
	for (i=0; i<16; i++)
	{
		sprintf(&msg[idx], "%02X ", cmos_ram[i]);
		idx = strlen(msg);
	}
	
	msg[idx] = 0;
	
	msgbox_waitkey(T_WHITE, T_RED, "RTC: RTC - CMOS RAM", msg, 4, 1);
#endif

}

//=============================================================================
//  flmenu_vlbox
//=============================================================================
char *flmenu_item[9] =
{
	" AB  : ACL BIOS",
	" AEC : ACL EC",
	" CP  : CPU Info",
	" E8  : E820 Size",
	" SM  : SMBIOS Data",
	" PC  : PCI Bus Scan",
	" SB  : SMBus Scan",
	" SP  : SPD Detect",
	" RTC : RTC Check"
};

char *flmenu_help[9] =
{
	"o:Model Name   o:BIOS firmware version",
	"o:Chip Name   o:Board Name   o:EC firmeare version",
	"o:CPU vendor   o:CPU name",
	"o:E820 Memory Map Size",
	"o:SMBIOS Data Checking",
	"o:PCI Bus Scanning",
	"o:SMBus Scanning",
	"o:SPD Detecting",
	"o:RTC crystal   o:RTC status   o:RTC power"
};

t_vlbox_t	flmenu_vlbox;

//=============================================================================
//  flmenu_show
//=============================================================================
void flmenu_show(void)
{
	char	str[32];
	uint8_t	sel;

	flmenu_vlbox.title	= NULL;
	flmenu_vlbox.item	= flmenu_item;
	flmenu_vlbox.help	= flmenu_help;

	flmenu_vlbox.num_item = 9;
	flmenu_vlbox.num_help = 9;

	flmenu_vlbox.fg = T_BLACK;
	flmenu_vlbox.bg = T_LIGHTGRAY;
	flmenu_vlbox.fs = T_YELLOW;
	flmenu_vlbox.bs = T_BLUE;
	flmenu_vlbox.fh = T_RED;
	flmenu_vlbox.bh = T_LIGHTGRAY;

	flmenu_vlbox.hm = 1;
	flmenu_vlbox.vm = 0;
	flmenu_vlbox.ha = 0;
	flmenu_vlbox.va = 0;
	
	flmenu_vlbox.align	= T_AL_CM,
	flmenu_vlbox.tialign= ((T_AL_CENTER<<4) | (T_AL_LEFT)),
	flmenu_vlbox.flag	= ( T_VLBOX_FL_TITLE   |
							T_VLBOX_FL_HELP	   |
							T_VLBOX_FL_KEYHELP |
							T_VLBOX_FL_SHADOW  |
							T_VLBOX_FL_FRAME );
				
	flmenu_vlbox.sel	= 0;

	
	sprintf(str, "%s", "Check Flag Menu");
	flmenu_vlbox.title = (char *)malloc(strlen(str)+1);
	if (!flmenu_vlbox.title)
		return;
	sprintf(flmenu_vlbox.title, "%s", str);

	flmenu_vlbox.sel = 0;
	
	sel = t_show_vlbox(&flmenu_vlbox);

	free(flmenu_vlbox.title);
	
	if (sel == 0xFA)	// abort
		return;

	switch(sel)
	{
		case 0:		flmenu_abios_show();	break;
		case 1:		flmenu_aec_show();		break;
		case 2:		flmenu_cpu_show();		break;
		case 3:		flmenu_e820_show();		break;
		case 4:		flmenu_smbios_show();	break;
		case 5:		flmenu_pci_show();		break;
		case 6:		flmenu_smbus_show();	break;
		case 7:		flmenu_spd_show();		break;
		case 8:		flmenu_rtc_show();		break;
	}
}

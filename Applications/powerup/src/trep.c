//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TREP : Text Report                                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "t_scrcap.h"
#include "sound.h"
#include "abios.h"
#include "smbios.h"
#include "dat.h"
#include "key.h"
#include "datlst.h"
#include "strlst.h"
#include "fbuf.h"
#include "pu.h"
#include "conv.h"
#include "errcode.h"
#include "rt.h"
#include "upd.h"
#include "statis.h"

//=============================================================================
//  pu_trep_show
//=============================================================================
int pu_trep_show(char *fname, char ftype)
{
	str_list_t		strlst;
	str_list_str_t	*s;

	char		str[256];
	char		sstr[256];
	char		tx_buf[80];
	uint8_t		loops;
	int			i = 0, j = 0, m = 0, len = 0;
	int			sx, sy;
	int			mw, mh;
	int			tx, tx_max, hsz, vsz;
	uint8_t		fg, bg;
	uint16_t		key, dirty, cr;

	int			page_max_idx;
	int			page_st_idx;
	int			sb_sta, sb_end, sb_len;

	FILE		*fp;
	uint32_t	fsz;

	memset(&strlst, 0, sizeof(str_list_t));

	fp = fopen(fname, "rb");
	if (!fp)
		return -1;

	fseek(fp, 0L, SEEK_END);
	fsz = ftell(fp);

	tx_max = -1;
	
	rewind(fp);
	while (!feof(fp))
	{
		fgets(str, sizeof(str)-1, fp);
		len = (int)strlen(str);
		//m = (len + 78) / 79;	// 79 chars / line

		for (i=0, j=0, cr=0; i<len; i++)
		{
			// \n=0xa : LF = line feed
			// \r=0xd : CR = carriage return
			// 79= max chars of line
			if (str[i] == '\r')
			{
				cr = 1;
				m = i + 1;	// look forward \n
				continue;
			}

			if (str[i] == '\n')
			{
				if (cr && i == m)
					cr = 0;
				
				sstr[j] = 0;

				if (j > tx_max)
					tx_max = j;
				
				str_list_add(sstr, &strlst);
				j = 0;
				continue;
			}
			
			if (j == 255)
			{
				sstr[j] = 0;
				
				if (j > tx_max)
					tx_max = j;

				str_list_add(sstr, &strlst);

				i--;
				j = 0;
				continue;
			}

			if (str[i] < 0x20 || str[i] > 0x7E)
				sstr[j++] = ' ';	// don't show non-printable char
			else
				sstr[j++] = str[i];
		}

		if (j > 0)
		{
			if (j > tx_max)
				tx_max = j;

			sstr[j] = 0;
			str_list_add(sstr, &strlst);
		}
	}
	fclose(fp);
	
	//-------------------------------------------------------------------------
	//  show
	//-------------------------------------------------------------------------

	sx = 0;
	sy = 0;
	mw = 80;
	mh = 25;
	tx = 0;
	
	vsz = 22;	// vertical scroll size   (height=22)
	hsz = 79;	// horizontal scroll size (width=79)

	fg = T_LIGHTGRAY;
	bg = T_BLUE;

	page_max_idx = strlst.num - 1;
	page_st_idx = 0;

	t_copy_region(sx, sy, sx+mw, sy+mh);
	t_clear_color_screen(fg, bg);

	// title, bottom
	t_color(T_BLACK, T_LIGHTGRAY);
	t_put_hline(0, 24, 80);
	t_put_hline(0,  0, 80);

	// title : name
	if (ftype == 0)
		t_xy_puts(2, 0, "POWERUP v3 Text Report");
	else
		t_xy_puts(2, 0, "POWERUP v3 Log");

	// tile : file , size
	sprintf(str, "File : %s      Size : %d Bytes", fname, fsz);
	t_xy_cl_puts(32, 0, T_BLUE, T_LIGHTGRAY, str);

	// bottom
	t_xy_cl_puts( 2, 24, T_RED, 	T_LIGHTGRAY, "ESC");
	t_xy_cl_puts( 6, 24, T_BLACK,	T_LIGHTGRAY, "Quit");

	t_xy_cl_puts(12, 24, T_RED, 	T_LIGHTGRAY, "\x18\x19 PgUp PgDn Home End Ins Del");
	t_xy_cl_puts(42, 24, T_BLACK,	T_LIGHTGRAY, "Scroll");
	
	
	t_color(fg, bg);
	
	loops = 1;
	dirty = 1;
	while (loops)
	{
		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if(key)//(key != KEY_SC_NULL)
		{
			switch(key)
			{

			case (SCAN_ESC << 8):
			//case KEY_ENTER:
				loops = 0;
				break;

			case (SCAN_PAGE_UP << 8):
			case (SCAN_UP << 8):

				if (key == (SCAN_PAGE_UP << 8))
					page_st_idx -= vsz;
				else if (key == (SCAN_UP << 8))
					page_st_idx--;

				if (page_st_idx < 0)
					page_st_idx = 0;
			
				dirty = 1;
				break;

			case (SCAN_PAGE_DOWN << 8):
			case (SCAN_DOWN << 8):
			case CHAR_CARRIAGE_RETURN:
			case ' ':

				if (key == (SCAN_PAGE_DOWN << 8))
					page_st_idx += vsz;
				else if (key == (SCAN_DOWN << 8))
					page_st_idx++;

				if (page_max_idx < vsz)
				{
					page_st_idx = 0;
				}
				else
				{
					if (page_st_idx > (page_max_idx - (vsz-1)))
						page_st_idx = page_max_idx - (vsz-1);
				}

				dirty = 1;
				break;

			// pan left
			case (SCAN_LEFT << 8):
				if (tx > 0)
					tx--;
				dirty = 1;
				break;

			// pan right
			case (SCAN_RIGHT << 8):
				if (tx_max - tx > hsz)
					tx++;
				dirty = 1;
				break;

			case (SCAN_HOME << 8):
				page_st_idx = 0;
				dirty = 1;
				break;

			case (SCAN_END << 8):
				page_st_idx = page_max_idx - (vsz-1);
				dirty = 1;
				break;

			case (SCAN_INSERT << 8):
				tx = 0;
				dirty = 1;
				break;

			case (SCAN_DELETE << 8):
				tx = tx_max - hsz;
				dirty = 1;
				break;
			
			case (SCAN_F10 << 8):
				t_scr_capture();
				break;
			}
		}

		if (dirty)
		{
			// point to str of strlst
			s = strlst.head;
			if (page_st_idx > 0)
			{
				for (i=0; i<page_st_idx; i++)
					s = s->next;
			}

			// clear display area
			for (i=0; i<vsz; i++)
				t_put_hline(0, i+1, 80);

			// show display area
			for (i=0; i<vsz; i++)
			{
				if ((page_st_idx+i) > page_max_idx)
				{
					t_put_hline(0, i+1, 80);
				}
				else
				{
					// show string : max len = 79
					memset(tx_buf, 0, sizeof(tx_buf));
					
					len = (int)strlen(s->str);
					if (tx >= len)
					{
						s = s->next;
						continue;
					}

					for (j=0; j<hsz; j++)
					{
						if ((tx+j) > len)
						{
							tx_buf[j] = 0;
							break;
						}
							
						tx_buf[j] = s->str[tx+j];
						//if (tx_buf[j] == 0)
						//	break;
						
					}
					t_xy_puts(0, i+1, tx_buf);

					//t_xy_puts(0, i+1, s->str);
					s = s->next;
				}
			}

			// vertical scroll bar
			if (strlst.num > vsz)
			{
				sb_len = vsz * vsz / strlst.num;

				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (page_st_idx == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if ((page_st_idx + (vsz-1)) == page_max_idx)
				{
					sb_end = (vsz - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (page_st_idx + 1) * vsz / strlst.num;
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (vsz-2))
					{
						sb_end = vsz - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}

				t_color(fg, bg);

				// 22: 0~21
				for (j=0; j<vsz; j++)
				{
					if (j<sb_sta || j>sb_end)
						t_xy_putc(79, j+1, 0xb0);
					else
						t_xy_putc(79, j+1, 0xdb);	//0xb2
				}
			}
				
			// horizontal scroll bar
			if (tx_max > hsz)
			{
				sb_len = hsz * hsz / tx_max;

				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// left
				if (tx == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// right
				else if (tx == (tx_max-hsz))
				{
					sb_end = (hsz - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle				
				else
				{
					sb_sta = (tx + 1) * hsz / tx_max;
					if (sb_sta == 0)
						sb_sta += 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (hsz-2))
					{
						sb_end = hsz - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}
				
				t_color(fg, bg);

				// 79 : 0~78
				for (j=0; j<hsz; j++)
				{
					if (j<sb_sta || j>sb_end)
						t_xy_putc(j, 23, 0xb0);
					else
						t_xy_putc(j, 23, 0xdb);	//0xb2
				}
			}

			//sprintf(str, "%4d of %4d     %.2f%%", page_st_idx+1, page_max_idx+1, (float)(page_st_idx+1)*100.0/(float)(page_max_idx+1));
			sprintf(str, "H: %3d / %3d  V: %4d / %4d", tx+1, tx_max, page_st_idx+1, page_max_idx+1);
			t_xy_cl_puts(50, 24, T_BLUE, T_LIGHTGRAY, str);
			
			t_color(fg, bg);
			
			dirty = 0;
		}
	}
	
	str_list_del_all(&strlst);

	t_paste_region(-1, -1);

	return 0;
}

//=============================================================================
//  pu_trep_select
//=============================================================================
int pu_trep_select(void)
{
	int		i, ret;
	char	c;
	char	fname[64];
	
	ret = datlst_show(fname, "*.TXT");

	if (ret < 0)
	{
		sound_end();
		//msgbox_waitkey(T_WHITE, T_RED, "Text Report", ".TXT file not found!", 4, 1);
		msgbox_waitkey(T_WHITE, T_RED, "Text Report", "Aborted selection!", 4, 1);
		return ret;
	}

	for (i=0; i<strlen(fname); i++)
	{
		c = fname[i];
		if (c >= 'a' && c <= 'z')
			fname[i] &= 0xDF;	// toupper
	}

	sound_ok();

	ret = pu_trep_show(fname, 0);

	return ret;
}

//===========================================================================
//  pu_add_trep_separator
//===========================================================================
static void pu_add_trep_separator(fbuf_t *fb, uint8_t typ)
{
	if (typ == 2)
		fbuf_puts(fb, "===============================================================================\r\n");
	else
		fbuf_puts(fb, "-------------------------------------------------------------------------------\r\n");
}

//===========================================================================
//  pu_add_trep_newline
//===========================================================================
static void pu_add_trep_newline(fbuf_t *fb, uint8_t num)
{
	int		i;

	for (i=0; i<num; i++)
		fbuf_puts(fb, "\r\n");
}

//===========================================================================
//  pu_create_trep
//===========================================================================
uint8_t pu_create_trep(void)
{
	char	fname[32];
	char	*ptr;
	int		i;

	fbuf_t	*fb;
	uint8_t	res;
	time_t	now;
	char	str[256];
	char	opt[32];
	
	pu_raw_t	*raw;
	
	smbios_hdr_t	*smbhdr;
	smbios_v3hdr_t	*smbv3hdr;
	uint32_t		num = 0;

	uint16_t	*data16p;
	uint8_t		*data8p;
	uint8_t		bus, dev, fun;
	uint16_t	vid, did;

	pu_rec_t	*rec;
	pu_statis_cnt_t	*ps;
	

	if (pu->mode == PU_MODE_ON_LINE)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Warning", "Can not create report in on-line mode", 4, 1);
		return 0xFF;
	}

	sound_end();

	// fname
	ptr = pu->file_name;
	for (i=0; i<28; i++)
	{
		if (*ptr == '.' || *ptr == 0)
		{
			fname[i++] = '.';
			fname[i++] = 'T';
			fname[i++] = 'X';
			fname[i++] = 'T';
			fname[i] = 0;
			break;
		}
		else
		{
			fname[i] = *ptr++;
		}
	}

	sprintf(str, "Creating text report : %s", fname);
	msgbox_tmout(T_WHITE, T_MAGENTA, "Report", str, 4, 1, 200);

	// fbuf init
	fb = fbuf_init(fname, &res);
	if (res != FBUF_OK)
	{
		sprintf(str, "fbuf_init err! res = %d", res);
		msgbox_waitkey(T_WHITE, T_RED, "Report", str, 4, 1);
		return  PU_ERR_DAT_GEN_REPORT;
	}

	// report : title
	pu_add_trep_separator(fb, 2);
	fbuf_printf(fb, "%s %s - %s\r\n", PU_DES_NAME, PU_DES_VER, PU_DES_DESC);
	pu_add_trep_separator(fb, 2);

	raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
	if (raw)
	{
		for (i=0; (uint32_t)i<raw->len; i++)
			str[i] = raw->data[i];
		str[i] = 0;
	}
	else
	{
		sprintf(str, "%s", "n/a");
	}
	//fbuf_printf(fb, "         Board : %s\r\n", abios.model);
	fbuf_printf(fb, "         Board : %s\r\n", str);

	// report : info
	now = time(NULL);
	conv_tm_str(&now, str);
	fbuf_printf(fb, "Report created : %s\r\n", str);

	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : description
	//---------------------------------------------------------
	fbuf_puts(fb, "Description\r\n");
	pu_add_trep_separator(fb, 1);

	// desc : magic
	memcpy(str, dat->des->magic, 8);	str[8] = 0;
	fbuf_printf(fb, "           Tag : %s\r\n", str);
	fbuf_printf(fb, "          Size : %d = 0x%0X\r\n", dat->des->dsz, dat->des->dsz);
	memcpy(str, dat->des->uname, 8);	str[8] = 0;
	fbuf_printf(fb, "  Utility Name : %s\r\n", str);
	memcpy(str, dat->des->uver, 8);		str[8] = 0;
	fbuf_printf(fb, "  Utility Ver  : %s\r\n", str);
	memcpy(str, dat->des->udesc, 48);	str[48] = 0;
	fbuf_printf(fb, "  Utility Desc : %s\r\n", str);	
	memcpy(str, dat->des->author, 16);	str[16] = 0;
	fbuf_printf(fb, "        Arthor : %s\r\n", str);	
	memcpy(str, dat->des->email, 32);	str[32] = 0;
	fbuf_printf(fb, "        E-mail : %s\r\n", str);
	memcpy(str, dat->des->div, 16);		str[16] = 0;
	fbuf_printf(fb, "      Division : %s\r\n", str);
	memcpy(str, dat->des->organ, 48);	str[48] = 0;
	fbuf_printf(fb, "  Organization : %s\r\n", str);
	memcpy(str, dat->des->copyr, 64);	str[64] = 0;
	fbuf_printf(fb, "     Copyright : %s\r\n", str);
	
	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//-------------------------------------------------------------------------
	// report : cpu info
	//-------------------------------------------------------------------------
	fbuf_puts(fb, "CPU Info\r\n");
	pu_add_trep_separator(fb, 1);

	// cpu vendor
	raw = raw_find_id(RAW_ID_CPU_VENDOR);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, "  Vendor : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, "  Vendor : n/a\r\n");
	}

	// cpu clock
	raw = raw_find_id(RAW_ID_CPU_CLOCK);
	if (raw)
		fbuf_printf(fb, "   Clock : %d MHz\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, "   Clock : n/a\r\n");

	// cpu name
	raw = raw_find_id(RAW_ID_CPU_NAME);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, "    Name : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, "    Name : n/a\r\n");
	}

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//-------------------------------------------------------------------------
	// report : e820 info
	//-------------------------------------------------------------------------
	fbuf_puts(fb, "Memory E820 Info\r\n");
	pu_add_trep_separator(fb, 1);

	// e820 item number
	raw = raw_find_id(RAW_ID_MEM_E820_NUM);
	if (raw)
		fbuf_printf(fb, "Num of Item : %d\r\n", raw->data[0]);
	else
		fbuf_puts(fb, "Num of Item : n/a\r\n");

	// e820 ksz
	raw = raw_find_id(RAW_ID_MEM_E820_KSZ);
	if (raw)
		fbuf_printf(fb, "    KB Size : %d KB\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, "    KB Size : n/a\r\n");

	// e820 msz
	raw = raw_find_id(RAW_ID_MEM_E820_MSZ);
	if (raw)
		fbuf_printf(fb, "    MB Size : %d MB\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, "    MB Size : n/a\r\n");

	// e820 data : todo

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);
	
	//---------------------------------------------------------	
	// report : bios info
	//---------------------------------------------------------
	fbuf_puts(fb, "BIOS Info\r\n");
	pu_add_trep_separator(fb, 1);

	// bios model
	raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, "   Model : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, "   Model : n/a\r\n");
	}
	
	// bios address
	raw = raw_find_id(RAW_ID_ACL_BIOS_ADDR);
	if (raw)
		fbuf_printf(fb, " Address : 0x%08X\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, " Address : n/a\r\n");

	// bios version
	raw = raw_find_id(RAW_ID_ACL_BIOS);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, " Version : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, " Version : n/a\r\n");
	}
	
	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);
	
	//---------------------------------------------------------
	// report : ec info
	//---------------------------------------------------------
	fbuf_puts(fb, "EC Info\r\n");
	pu_add_trep_separator(fb, 1);

	// ec chip
	raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, "    Chip : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, "    Chip : n/a\r\n");
	}

	// ec board
	raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, "   Board : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, "   Board : n/a\r\n");
	}

	// ec ver
	raw = raw_find_id(RAW_ID_ACL_EC_VER);
	if (raw)
	{
		memcpy(str, raw->data, raw->len);
		str[raw->len] = 0;
		fbuf_printf(fb, " Version : %s\r\n", str);
	}
	else
	{
		fbuf_puts(fb, " Version : n/a\r\n");
	}

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : smbios info
	//---------------------------------------------------------
	fbuf_puts(fb, "SMBIOS Info\r\n");
	pu_add_trep_separator(fb, 1);

	// smbios header
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_HDR);
	if (raw)
	{
		smbhdr = (smbios_hdr_t *)raw->data;
		fbuf_printf(fb, "     Len : %d\r\n",		smbhdr->len);
		fbuf_printf(fb, "     Ver : %d.%d\r\n",		smbhdr->major_ver, smbhdr->minor_ver);
		fbuf_printf(fb, "DMI  Num : %d\r\n",		smbhdr->smbios_ent_num);
		fbuf_printf(fb, "DMI Addr : 0x%08X\r\n",	smbhdr->smbios_addr);
		fbuf_printf(fb, "DMI Size : %d = 0x%X\r\n",	smbhdr->smbios_len, smbhdr->smbios_len);
	}
	else
	{
		fbuf_puts(fb, "  SMBIOS : n/a\r\n");
	}

	// smbios address
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_ADDR);
	if (raw)
		fbuf_printf(fb, " Address : 0x%08X\r\n\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, " Address : n/a\r\n\r\n");


	// smbios v3 header
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_V3HDR);
	if (raw)
	{
		smbv3hdr = (smbios_v3hdr_t *)raw->data;
		fbuf_printf(fb, "  V3 Len : %d\r\n",		smbv3hdr->len);
		fbuf_printf(fb, "  V3 Ver : %d.%d\r\n",		smbv3hdr->major_ver, smbv3hdr->minor_ver);
		fbuf_printf(fb, " V3 Addr : 0x%08X\r\n",	smbv3hdr->tab_addr);
		fbuf_printf(fb, " V3 Size : %d = 0x%X\r\n",	smbv3hdr->tab_sz, smbv3hdr->tab_sz);
		fbuf_printf(fb, " DOC Rev : %d\r\n",		smbv3hdr->doc_rev);
		fbuf_printf(fb, " EPS Rev : %d\r\n",		smbv3hdr->eps_rev);
	}
	else
	{
		fbuf_puts(fb, "V3 SMBIOS: n/a\r\n");
	}
	
	// smbios v3 address
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_V3ADDR);
	if (raw)
		fbuf_printf(fb, " Address : 0x%08X\r\n", *(uint32_t *)raw->data);
	else
		fbuf_puts(fb, " Address : n/a\r\n");

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : pci info
	//---------------------------------------------------------
	fbuf_puts(fb, "PCI Info\r\n");
	pu_add_trep_separator(fb, 1);

	// num of pci dev
	raw = raw_find_id(RAW_ID_PCI_DEV_NUM);
	if (raw)
	{
		num = *(uint32_t *)raw->data;
		fbuf_printf(fb, "Num of Dev : %d\r\n\r\n", num);
	}
	else
	{
		fbuf_puts(fb, "Num of Dev : n/a\r\n\r\n");
	}

	// pci list : vid/did
	raw = raw_find_id(RAW_ID_PCI_DEV_LST);
	if (raw)
	{
		data16p = (uint16_t *)raw->data;
	}
	else
	{
		fbuf_puts(fb, "Lst of Dev : n/a\r\n");
		data16p = 0;
	}

	// pci bdf : bus/dev/func
	raw = raw_find_id(RAW_ID_PCI_DEV_BDF);
	if (raw)
	{
		data8p = (uint8_t *)raw->data;
	}
	else
	{
		fbuf_puts(fb, "BDF of Dev : n/a\r\n");
		data8p = 0;
	}

	if (data16p && data8p && (num > 0))
	{
		fbuf_puts(fb,   " No# Bus Dev Fun  VID   DID\r\n");
		for (i=0; (uint32_t)i<num; i++)
		{
			bus = *data8p++;
			dev = *data8p++;
			fun = *data8p++;
			vid = *data16p++;
			did = *data16p++;
			fbuf_printf(fb, "%3d: %02X  %02X  %02X   %04X  %04X\r\n", i+1, bus, dev, fun, vid, did);
		}
	}

	// todo : RAW_ID_PCI_DEV_CFG

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : smbus info
	//---------------------------------------------------------
	fbuf_puts(fb, "SMBus Info\r\n");
	pu_add_trep_separator(fb, 1);

	// num of smbus dev
	raw = raw_find_id(RAW_ID_SMB_DEV_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		fbuf_printf(fb, " Num of SMB : %d\r\n", num);
	}
	else
	{
		fbuf_puts(fb, " Num of SMB : n/a\r\n");
	}

	// list of smbus dev
	raw = raw_find_id(RAW_ID_SMB_DEV_LST);
	if (raw)
	{
		data8p = (uint8_t *)raw->data;
		fbuf_printf(fb, "List of SMB : ");
		for (i=0; (uint32_t)i<num; i++)
			fbuf_printf(fb, "%02X ", *data8p++);
		fbuf_puts(fb, "\r\n");
	}
	else
	{
		fbuf_puts(fb, "List of SMB : n/a\r\n");
	}

	// num of spd dev
	raw = raw_find_id(RAW_ID_SMB_SPD_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		fbuf_printf(fb, " Num of SPD : %d\r\n", num);
	}
	else
	{
		fbuf_puts(fb, " Num of SPD : n/a\r\n");
	}

	// list of spd dev
	raw = raw_find_id(RAW_ID_SMB_SPD_LST);
	if (raw)
	{
		data8p = (uint8_t *)raw->data;
		fbuf_printf(fb, "List of SPD : ");
		for (i=0; (uint32_t)i<num; i++)
			fbuf_printf(fb, "%02X ", *data8p++);
		fbuf_puts(fb, "\r\n");
	}
	else
	{
		fbuf_puts(fb, "List of SPD : n/a\r\n");
	}

	// todo : RAW_ID_SMB_SPD

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : time info
	//---------------------------------------------------------
	fbuf_puts(fb, "Time Info\r\n");
	pu_add_trep_separator(fb, 1);

	conv_tm_str(&dat->par->tm_init, str);
	fbuf_printf(fb, "  Initial : %s\r\n", str);
	
	conv_tm_str(&dat->par->tm_zero, str);
	fbuf_printf(fb, "    Start : %s\r\n", str);

	conv_tm_str(&dat->rec.tm_rtc, str);
	fbuf_printf(fb, "  Current : %s\r\n", str);
	
	conv_tm_elapsed_str(dat->par->tm_zero, dat->rec.tm_rtc, str);
	fbuf_printf(fb, "  Elapsed : %s\r\n\r\n", str);

	// interval
	if (dat->par->rec_cnt < 10)
		fbuf_puts(fb, " Interval : n/a\r\n");
	else
		fbuf_printf(fb, " Interval : %d seconds\r\n", dat->par->intv);

	// tolerance
	fbuf_printf(fb, "Tolerance : %d seconds\r\n", dat->par->tol);
	
	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);	

	//---------------------------------------------------------
	// report : count info
	//---------------------------------------------------------
	fbuf_puts(fb, "Count Info\r\n");
	pu_add_trep_separator(fb, 1);

	// total
	fbuf_printf(fb, "Total : %5d  100.00%%\r\n", dat->par->rec_cnt);
	
	sprintf(str, " Pass : %5d  %6.2f%% (OK)\r\n", dat->sta.ok, (float)(dat->sta.ok*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);

	sprintf(str, " Fail : %5d  %6.2f%% (GG)\r\n", dat->sta.gg, (float)(dat->sta.gg*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);

	sprintf(str, "Error : %5d  %6.2f%% (ER)\r\n", dat->sta.er, (float)(dat->sta.er*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);

	sprintf(str, "  N/A : %5d  %6.2f%% (NA)\r\n", dat->sta.na, (float)(dat->sta.na*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);
	
	sprintf(str, "   oo : %5d  %6.2f%% (oo)\r\n", dat->sta.oo, (float)(dat->sta.oo*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);

	sprintf(str, "   xx : %5d  %6.2f%% (xx)\r\n", dat->sta.xx, (float)(dat->sta.xx*100)/(float)dat->sta.to);
	fbuf_puts(fb, str);

	sprintf(str, "   nx : %5d\r\n", dat->sta.nx);
	fbuf_puts(fb, str);
	
	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);	

	//---------------------------------------------------------
	// report : parameter info
	//---------------------------------------------------------
	fbuf_puts(fb, "Parameter Info\r\n");
	pu_add_trep_separator(fb, 1);

	// RTC Battery
	fbuf_puts(fb, "  RTC Battery : ");
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		fbuf_puts(fb, "No\r\n");
	else
		fbuf_puts(fb, "Yes\r\n");

	// AT/ATX
	fbuf_puts(fb, "       AT/ATX : ");
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
		fbuf_puts(fb, "ATX\r\n");
	else
		fbuf_puts(fb, "AT\r\n");

	// ALARM
	fbuf_puts(fb, "        Alarm : ");
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW))
		fbuf_puts(fb, "Yes\r\n");
	else
		fbuf_puts(fb, "No\r\n");
	fbuf_printf(fb, "Alarm Hi Freq : %d Hz\r\n", dat->par->alm_hi);
	fbuf_printf(fb, "Alarm Lo Freq : %d Hz\r\n", dat->par->alm_lo);

	// Reset Type
	fbuf_puts(fb, "   Reset Type : ");
	switch (dat->par->rst_typ)
	{
		case PU_PAR_RST_NONE:	fbuf_puts(fb, "None\r\n");	break;
		case PU_PAR_RST_KBC:	fbuf_puts(fb, "KBC\r\n");	break;
		case PU_PAR_RST_SOFT92:	fbuf_puts(fb, "Soft 92h\r\n");		break;
		case PU_PAR_RST_SOFT:	fbuf_puts(fb, "Soft\r\n");	break;
		case PU_PAR_RST_HARD:	fbuf_puts(fb, "Hard\r\n");	break;
		case PU_PAR_RST_FULL:	fbuf_puts(fb, "Full\r\n");	break;
	}

	// Reset Timeout
	if (dat->par->rst_typ != PU_PAR_RST_NONE)
	{
		fbuf_puts(fb, "  Reset Tmout : ");
		fbuf_printf(fb, "%d seconds\r\n", dat->par->rst_tmo);
	}
	
	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);	
	
	//---------------------------------------------------------
	// report : Features
	//---------------------------------------------------------
	fbuf_puts(fb, "Features\r\n");
	pu_add_trep_separator(fb, 1);

	// rtc
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		sprintf(opt, "%s", "No");
	else
		sprintf(opt, "%s", "Yes");
	sprintf(str, "RTC    : %s\r\n", opt);
	fbuf_puts(fb, str);

	// at/atx
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
		sprintf(opt, "%s", "ATX");
	else
		sprintf(opt, "%s", "AT");
	sprintf(str, "AT/ATX : %s\r\n", opt);
	fbuf_puts(fb, str);

	// alarm
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW))
		sprintf(opt, "%s", "Yes");
	else
		sprintf(opt, "%s", "No");
	sprintf(str, "Alarm  : %s\r\n", opt);
	fbuf_puts(fb, str);

	// reset
	switch (dat->par->rst_typ)
	{
		case PU_PAR_RST_NONE:	sprintf(opt, "%s", "None");		break;
		case PU_PAR_RST_KBC:	sprintf(opt, "KBC  %d sec",		dat->par->rst_tmo);	break;
		case PU_PAR_RST_SOFT92:	sprintf(opt, "SOFT92  %d sec",	dat->par->rst_tmo);	break;
		case PU_PAR_RST_SOFT:	sprintf(opt, "SOFT  %d sec",	dat->par->rst_tmo);	break;
		case PU_PAR_RST_HARD:	sprintf(opt, "HARD  %d sec",	dat->par->rst_tmo);	break;
		case PU_PAR_RST_FULL:	sprintf(opt, "FULL  %d sec",	dat->par->rst_tmo);	break;
	}
	sprintf(str, "Reset  : %s\r\n", opt);
	fbuf_puts(fb, str);
	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : Interval Statistics
	//---------------------------------------------------------
	pu_statis_init();

	fbuf_puts(fb, "Interval Statistics\r\n");
	pu_add_trep_separator(fb, 1);

	fbuf_puts(fb, "  No       Interval     Count      Rate\r\n");

	ps = pu->stt.head;
	for (i=0; i<pu->stt.num; i++)
	{
		sprintf(str, " %3d   %12d  :  %5d   %6.2f%%\r\n",
				i+1, ps->intv, ps->cnt, (float)ps->cnt*100.0/(float)dat->par->rec_cnt);

		fbuf_puts(fb, str);

		ps = ps->next;
	}
	
	sprintf(str, "       Total Average : %.2f\r\n", pu->stt.avr.total);
	fbuf_puts(fb, str);

	sprintf(str, "        Pass Average : %.2f\r\n", pu->stt.avr.pass);
	fbuf_puts(fb, str);

	sprintf(str, "        Fail Average : %.2f\r\n", pu->stt.avr.fail);
	fbuf_puts(fb, str);

	sprintf(str, "            Variance : %.2f\r\n", pu->stt.avr.tvar);
	fbuf_puts(fb, str);

	sprintf(str, "  Standard Deviation : %.2f\r\n", pu->stt.avr.stdv);
	fbuf_puts(fb, str);

	pu_add_trep_newline(fb, 2);

	//---------------------------------------------------------
	// report : record no_inc
	//---------------------------------------------------------
	fbuf_puts(fb, "Record : increased by no.\r\n");
	pu_add_trep_separator(fb, 1);

	// sort by no_inc
	rec = (pu_rec_t *)&dat->rec_pool[0];
	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_no_inc);

	for (i=0; i<dat->par->rec_cnt; i++, rec++)
	{
		// No : ....1
		fbuf_printf(fb, "%5d ", rec->no);
	
		// RTC : yyyy/mm/dd hh:mm:ss
		conv_tm_str(&rec->tm_rtc, str);
		fbuf_puts(fb, str);

		fbuf_putc(fb, ' ');

		// AB : ABIOS : model
		if (rec->acl_flag & PU_REC_ACL_ABIOS_MODEL)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		// AB : ABIOS : ver
		if (rec->acl_flag & PU_REC_ACL_ABIOS_VER)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');
		
		// AE : AEC : chip
		if (rec->acl_flag & PU_REC_ACL_AEC_CHIP)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		// AE : AEC : board
		if (rec->acl_flag & PU_REC_ACL_AEC_BOARD)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		// AE : AEC : ver
		if (rec->acl_flag & PU_REC_ACL_AEC_VER)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// CP : CPU vendor
		if (rec->sys_flag & PU_REC_SYS_CPU_VENDOR)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		// CP : CPU name
		if (rec->sys_flag & PU_REC_SYS_CPU_NAME)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// E8 : E820
		if (rec->sys_flag & PU_REC_SYS_E820)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// SM : SMBIOS
		if (rec->sys_flag & PU_REC_SYS_SMBIOS)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// PC : PCI
		if (rec->sys_flag & PU_REC_SYS_PCI_SCAN)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// SB : SMBUS
		if (rec->sys_flag & PU_REC_SYS_SMB_SCAN)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// SP : SPD
		if (rec->sys_flag & PU_REC_SYS_SMB_SPD)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');

		fbuf_putc(fb, ' ');

		// TMP
		fbuf_printf(fb, "%3d", rec->cpu_temp);

		fbuf_putc(fb, ' ');

		// TSC
		sprintf(str, "%6.2f", rec->cpu_tsc);
		fbuf_puts(fb, str);

		fbuf_putc(fb, ' ');

		// INTV
		fbuf_printf(fb, "%9d", rec->intv);

		fbuf_putc(fb, ' ');

		// QC
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		{
			fbuf_puts(fb, "--");
		}
		else
		{
			if (dat->par->rec_cnt < 10)
			{
				fbuf_puts(fb, "? ");
			}
			else
			{
				if (rec->intv == 0)
					fbuf_puts(fb, "NA");
				else if (rec->intv < 0)
					fbuf_puts(fb, "ER");
				else if (rec->intv > dat->par->intv)
					fbuf_puts(fb, "GG");
				else
					fbuf_puts(fb, "OK");
			}
		}
		fbuf_puts(fb, "\r\n");
	}

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);	

	//---------------------------------------------------------
	// report : record intv_inc
	//---------------------------------------------------------
	fbuf_puts(fb, "Record : increased by interval\r\n");
	pu_add_trep_separator(fb, 1);

	// sort by no_inc
	rec = (pu_rec_t *)&dat->rec_pool[0];
	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_intv_inc);

	for (i=0; i<dat->par->rec_cnt; i++, rec++)
	{
		// No : ....1
		fbuf_printf(fb, "%5d ", rec->no);
	
		// RTC : yyyy/mm/dd hh:mm:ss
		conv_tm_str(&rec->tm_rtc, str);
		fbuf_puts(fb, str);

		fbuf_putc(fb, ' ');

		if (dat->par->cmp & PU_PAR_CMP_AB)
		{
			// AB : ABIOS : model
			if (rec->acl_flag & PU_REC_ACL_ABIOS_MODEL)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
			// AB : ABIOS : ver
			if (rec->acl_flag & PU_REC_ACL_ABIOS_VER)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');
		
		if (dat->par->cmp & PU_PAR_CMP_AEC)
		{
			// AE : AEC : chip
			if (rec->acl_flag & PU_REC_ACL_AEC_CHIP)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
			// AE : AEC : board
			if (rec->acl_flag & PU_REC_ACL_AEC_BOARD)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
			// AE : AEC : ver
			if (rec->acl_flag & PU_REC_ACL_AEC_VER)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
			fbuf_putc(fb, '_');
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		if (dat->par->cmp & PU_PAR_CMP_CP)
		{
			// CP : CPU vendor
			if (rec->sys_flag & PU_REC_SYS_CPU_VENDOR)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
			// CP : CPU name
			if (rec->sys_flag & PU_REC_SYS_CPU_NAME)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// E8 : E820
		if (dat->par->cmp & PU_PAR_CMP_E8)
		{
			if (rec->sys_flag & PU_REC_SYS_E820)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// SM : SMBIOS
		if (dat->par->cmp & PU_PAR_CMP_SM)
		{
			if (rec->sys_flag & PU_REC_SYS_SMBIOS)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// PC : PCI
		if (dat->par->cmp & PU_PAR_CMP_PC)
		{
			if (rec->sys_flag & PU_REC_SYS_PCI_SCAN)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// SB : SMBUS
		if (dat->par->cmp & PU_PAR_CMP_SB)
		{
			if (rec->sys_flag & PU_REC_SYS_SMB_SCAN)	fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// SP : SPD
		if (dat->par->cmp & PU_PAR_CMP_SP)
		{
			if (rec->sys_flag & PU_REC_SYS_SMB_SPD)		fbuf_putc(fb, 'o');	else fbuf_putc(fb, 'x');
		}
		else
		{
			fbuf_putc(fb, '_');
		}

		fbuf_putc(fb, ' ');

		// TMP
		fbuf_printf(fb, "%3d", rec->cpu_temp);

		fbuf_putc(fb, ' ');

		// TSC
		sprintf(str, "%6.2f", rec->cpu_tsc);
		fbuf_puts(fb, str);

		fbuf_putc(fb, ' ');

		// INTV
		fbuf_printf(fb, "%9d", rec->intv);

		fbuf_putc(fb, ' ');

		// QC
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		{
			fbuf_puts(fb, "--");
		}
		else
		{
			if (dat->par->rec_cnt < 10)
			{
				fbuf_puts(fb, "? ");
			}
			else
			{
				if (rec->intv == 0)
					fbuf_puts(fb, "NA");
				else if (rec->intv < 0)
					fbuf_puts(fb, "ER");
				else if (rec->intv > dat->par->intv)
					fbuf_puts(fb, "GG");
				else
					fbuf_puts(fb, "OK");
			}
		}
		fbuf_puts(fb, "\r\n");
	}

	pu_add_trep_separator(fb, 1);
	pu_add_trep_newline(fb, 2);	


	// close report
	fbuf_flush(fb);
	fbuf_exit(fb);

	msgbox_doing_done();

	sound_ok();
	
	sprintf(str, "Creating text report : %s done!", fname);
	msgbox_waitkey(T_WHITE, T_RED, "Report", str, 4, 1);

	return PU_OK;
}

//=============================================================================
//  pu_log_select
//=============================================================================
int pu_log_select(void)
{
	int		i, ret;
	char	c;
	char	fname[64];
	
	ret = datlst_show(fname, "*.LOG");

	if (ret < 0)
	{
		sound_end();
		//msgbox_waitkey(T_WHITE, T_RED, "Log File", ".LOG file not found!", 4, 1);
		msgbox_waitkey(T_WHITE, T_RED, "Log File", "Aborted selection!", 4, 1);
		return ret;
	}

	for (i=0; i<strlen(fname); i++)
	{
		c = fname[i];
		if (c >= 'a' && c <= 'z')
			fname[i] &= 0xDF;	// toupper
	}

	sound_ok();

	ret = pu_trep_show(fname, 1);

	return ret;
}
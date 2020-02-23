//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  CHKREC : Check Record Menu                                               *
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
#include "file.h"
#include "log.h"
#include "conv.h"

#include "dat.h"
#include "pu.h"
#include "upd.h"
#include "scr.h"
#include "errcode.h"
#include "chkrec.h"

//=============================================================================
//  variables
//=============================================================================
pu_err_raw_t	*er = NULL;


//=============================================================================
//  pu_find_rec_no
//=============================================================================
pu_rec_t *pu_find_rec_no(uint16_t no)
{
	int			i;
	pu_rec_t	*rec = NULL;

	for (i=0; i<dat->par->rec_cnt; i++)
	{
		rec = &dat->rec_pool[i];
		if (rec->no == no)
		{
			rec = &dat->rec_pool[i];
			break;
		}
	}

	return rec; 
}
	
//=============================================================================
//  chkrec_abios_show
//=============================================================================
static void chkrec_abios_show(uint16_t no, uint32_t mask, file_t *ef)
{
	int				i;
	uint8_t			fg, model, ver;
	char			str[256];
	uint8_t			*ebuf;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;


	msgbox_msg_init(&msg);

	model	= GET_VAR_GV(mask, CHKREC_GM_AB_MODEL, CHKREC_GP_AB_MODEL);
	ver		= GET_VAR_GV(mask, CHKREC_GM_AB_VER,   CHKREC_GP_AB_VER);

	if (model == CHKREC_ERR || ver == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else if (model == CHKREC_NOFEAT || ver == CHKREC_NOFEAT)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else if (model == CHKREC_NOCHK || ver == CHKREC_NOCHK)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else
	{
		fg = T_ARG_LIGHTCYAN;
	}

	//---------------------------------------------------------
	//  Init > AB
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	// abios : model
	raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
	if (raw)
	{
		msgbox_msg_add_str("\r  Model : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\r  Model : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);


	// abios : ver
	raw = raw_find_id(RAW_ID_ACL_BIOS_VER);
	if (raw)
	{
		msgbox_msg_add_str("\rVersion : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rVersion : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);

	// abios : magic (biosstring)
	raw = raw_find_id(RAW_ID_ACL_BIOS);
	if (raw)
	{
		msgbox_msg_add_str("\r String :\n", &msg);
		msgbox_msg_add_char('\b', &msg);
		msgbox_msg_add_char(T_ARG_YELLOW, &msg);
		
		for (i=0; (uint32_t)i<raw->len; i++)
		{
			msgbox_msg_add_char(raw->data[i], &msg);
			
			// 32 chars/line
			if ((i & 0x1F) == 0x1F)
			{
				msgbox_msg_add_char('\n', &msg);
				msgbox_msg_add_char('\r', &msg);
			}
		}
	}
	else
	{
		msgbox_msg_add_str("\r String : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > AB
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	if (model == CHKREC_NOCHK || ver == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_ab;
	}

	if (model == CHKREC_NOFEAT || ver == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_ab;
	}

	// AB:MODEL
	if (model == CHKREC_OK)
	{
		sprintf(str, "\r\b%c  Model : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (model == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_ACL_BIOS_MODEL, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%c  Model : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%c  Model : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);

			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

	msgbox_msg_add_char('\n', &msg);

	// AB:VER
	if (ver == CHKREC_OK)
	{
		sprintf(str, "\r\b%cVersion : OK\n", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (ver == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_ACL_BIOS_VER, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cVersion : n/a\n", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cVersion : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

end_ab:

	sprintf(str, "%d : Check Record - AB : ACL BIOS", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_aec_show
//=============================================================================
static void chkrec_aec_show(uint16_t no, uint32_t mask, file_t *ef)
{
	uint8_t			fg, chip, board, ver;
	char			str[256];
	uint8_t			*ebuf;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;


	msgbox_msg_init(&msg);

	chip	= GET_VAR_GV(mask, CHKREC_GM_AEC_CHIP,  CHKREC_GP_AEC_CHIP);
	board	= GET_VAR_GV(mask, CHKREC_GM_AEC_BOARD, CHKREC_GP_AEC_BOARD);
	ver		= (uint8_t)GET_VAR_GV(mask, CHKREC_GM_AEC_VER,   CHKREC_GP_AEC_VER);

	if (chip == CHKREC_ERR || board == CHKREC_ERR || ver == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else if (chip == CHKREC_NOFEAT || board == CHKREC_NOFEAT || ver == CHKREC_NOFEAT)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else if (chip == CHKREC_NOCHK || board == CHKREC_NOCHK || ver == CHKREC_NOCHK)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else
	{
		fg = T_ARG_LIGHTCYAN;
	}

	//---------------------------------------------------------
	//  Init > AEC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
	if (raw)
	{
		msgbox_msg_add_str("\r   Chip : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\r   Chip : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);


	raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
	if (raw)
	{
		msgbox_msg_add_str("\r  Board : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\r  Board : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);


	raw = raw_find_id(RAW_ID_ACL_EC_VER);
	if (raw)
	{
		msgbox_msg_add_str("\rVersion : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rVersion : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > AEC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	if (chip == CHKREC_NOCHK || board == CHKREC_NOCHK || ver == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_aec;
	}

	if (chip == CHKREC_NOFEAT || board == CHKREC_NOFEAT || ver == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a\n", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_aec;
	}

	// AEC:Chip
	if (chip == CHKREC_OK)
	{
		sprintf(str, "\r\b%c   Chip : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (chip == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_ACL_EC_CHIP, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%c   Chip : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%c   Chip : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

	msgbox_msg_add_char('\n', &msg);

	// AEC:Board
	if (board == CHKREC_OK)
	{
		sprintf(str, "\r\b%c  Board : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (board == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_ACL_EC_BOARD, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%c  Board : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%c  Board : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

	msgbox_msg_add_char('\n', &msg);

	// AEC:Ver
	if (ver == CHKREC_OK)
	{
		sprintf(str, "\r\b%cVersion : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (ver == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_ACL_EC_VER, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cVersion : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cVersion : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

end_aec:

	sprintf(str, "%d : Check Record - AEC : ACL EC", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_cpu_show
//=============================================================================
static void chkrec_cpu_show(uint16_t no, uint32_t mask, file_t *ef)
{
	uint8_t			fg, vendor, name;
	char			str[256];
	uint8_t			*ebuf;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;


	msgbox_msg_init(&msg);

	vendor	= (uint8_t)GET_VAR_GV(mask, CHKREC_GM_CP_VENDOR,  CHKREC_GP_CP_VENDOR);
	name	= (uint8_t)GET_VAR_GV(mask, CHKREC_GM_CP_NAME,    CHKREC_GP_CP_NAME);

	if (vendor == CHKREC_ERR || name == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else if (vendor == CHKREC_NOFEAT || name == CHKREC_NOFEAT)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else if (vendor == CHKREC_NOCHK || name == CHKREC_NOCHK)
	{
		fg = T_ARG_LIGHTGRAY;
	}
	else
	{
		fg = T_ARG_LIGHTCYAN;
	}

	//---------------------------------------------------------
	//  Init > CP
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	raw = raw_find_id(RAW_ID_CPU_VENDOR);
	if (raw)
	{
		msgbox_msg_add_str("\rVendor : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rVendor : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);
	
	raw = raw_find_id(RAW_ID_CPU_NAME);
	if (raw)
	{
		msgbox_msg_add_str("\r  Name : ", &msg);
		msgbox_msg_add_strn(raw->data, raw->len, &msg);
	}
	else
	{
		msgbox_msg_add_str("\r  Name : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);
	
	//---------------------------------------------------------
	//  Check > CP
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	if (vendor == CHKREC_NOCHK || name == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_cp;
	}

	if (vendor == CHKREC_NOFEAT || name == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_cp;
	}

	// CP:Vendor
	if (vendor == CHKREC_OK)
	{
		sprintf(str, "\r\b%cVendor : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (vendor == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_CPU_VENDOR, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cVendor : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cVendor : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

	msgbox_msg_add_char('\n', &msg);

	// CP:Name
	if (name == CHKREC_OK)
	{
		sprintf(str, "\r\b%c  Name : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (name == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_CPU_NAME, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%c  Name : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%c  Name : \n\r", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
			
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			msgbox_msg_add_strn(ebuf, eraw->len, &msg);
		}
	}

end_cp:

	sprintf(str, "%d : Check Record - CP : CPU Info", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_e820_show
//=============================================================================
static void chkrec_e820_show(uint16_t no, uint32_t mask, file_t *ef)
{
	uint8_t			fg, e820chk;
	char			str[256];
	uint8_t			*ebuf;
	uint32_t		mbsz;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;


	msgbox_msg_init(&msg);

	e820chk = (uint8_t)GET_VAR_GV(mask, CHKREC_GM_E8_SIZE, CHKREC_GP_E8_SIZE);
	
	if (e820chk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTCYAN;
	}
	else if (e820chk == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else
	{
		fg = T_ARG_LIGHTGRAY;
	}

	//---------------------------------------------------------
	//  Init > E8
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	raw = raw_find_id(RAW_ID_MEM_E820_NUM);
	if (raw)
	{
		sprintf(str, "\r    Num : %d", *(uint8_t *)raw->data);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		msgbox_msg_add_str("\r    Num : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);

	raw = raw_find_id(RAW_ID_MEM_E820_KSZ);
	if (raw)
	{
		sprintf(str, "\rKB Size : %d KiB", *(uint32_t *)raw->data);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rKB Size : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);

	raw = raw_find_id(RAW_ID_MEM_E820_MSZ);
	if (raw)
	{
		mbsz = *(uint32_t *)raw->data;
		sprintf(str, "\rMB Size : %d MiB", mbsz);
		msgbox_msg_add_str(str, &msg);

		msgbox_msg_add_char('\n', &msg);

		sprintf(str, "\rGB Size : %.2f GiB", (float)mbsz / 1024.0);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rMB Size : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > E8
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	if (e820chk == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_e8;
	}

	if (e820chk == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_e8;
	}

	// E8: size check
	if (e820chk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cKB Size : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (e820chk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_MEM_E820_KSZ, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cKB Size : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			sprintf(str, "\r\b%cKB Size : %d KiB", T_ARG_LIGHTMAGENTA, *(uint32_t *)ebuf);
			msgbox_msg_add_str(str, &msg);
		}
	}

end_e8:
	
	sprintf(str, "%d : Check Record - E8 : E820 Size", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_smbios_err_show
//=============================================================================
static void chkrec_smbios_err_show(uint16_t no, uint8_t *dat, uint32_t dlen, uint8_t *err, uint32_t elen)
{
	int			i, j;
	int			sx, sy, ex, ey;
	int			tx, ty, mw, mh;
	uint8_t		fg, bg, dirty;
	char		str[256];
	int			pg, pi, pg_max;
	uint8_t		scan;
	int			sb_sta, sb_end, sb_len;
	uint16_t key;

	//-------------------------------------------------
	//  error compare
	//-------------------------------------------------
	// 0         1         2         3         4         5         6
	// 01234567890123456789012345678901234567890123456789012345678901234
	// +------------------------------------------------------------+
	// |   Check Record - SM : SMBIOS Data                            |
	// +------------------------------------------------------------+
	// | Address   Init                     Check                   |
	// | FFFF1230  00 11 22 33 44 55 66 77  00 11 22 33 44 55 66 77 |
	// | FFFF1238  88 99 AA BB CC DD EE FF  88 99 AA BB CC DD EE FF |
	// |........

	mw = 62 + 1;
	mh = 21;
	sx = (80 - mw)/2;
	sy = (25 - mh)/2;
	ex = sx + mw;	// with shadow
	ey = sy + mh;	// with shadow
	fg = T_WHITE;
	bg = T_BLACK;

	t_copy_region(sx, sy, ex, ey);

	t_color(fg, bg);

	for (j=sy; j<sy+mh; j++)
	{
		t_gotoxy(sx, j);

		for (i=sx; i<sx+mw; i++)
		{
			// top border
			if (j == sy)
			{
				if (i==sx)				t_putc(0xda);	// left-top
				else if (i==sx+mw-1) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			// bottom border
			else if (j==sy+mh-1)
			{
				if (i==sx)				t_putc(0xc0);	// left-bottom
				else if (i==sx+mw-1) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
			}
			// title seperator
			else if (j==sy+2)
			{
				if (i==sx)				t_putc(0xc6);	// left-middle
				else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
				else					t_putc(0xcd);	// --
			}
			// left/right border
			else
			{
				if (i==sx || i==sx+mw-1)	t_putc(0xb3);	// |
				else						t_putc(0x20);
			}
		}
	}
	// 0         1         2         3         4         5         6
	// 01234567890123456789012345678901234567890123456789012345678901234
	// +------------------------------------------------------------+
	// |   Check Record - SM : SMBIOS Data                            |
	// +------------------------------------------------------------+
	// | Address   Init                     Check                   |
	// | FFFF1230  00 11 22 33 44 55 66 77  00 11 22 33 44 55 66 77 |
	// | FFFF1238  88 99 AA BB CC DD EE FF  88 99 AA BB CC DD EE FF |
	// |........
	sprintf(str, "%d : Check Record - SM : SMBIOS Data", no);
	t_xy_puts(sx+(mw-(int)strlen(str))/2, sy+1, str);	// align : center

	t_xy_cl_puts(sx+2,  sy+3, T_LIGHTGRAY, bg, "Address");
	t_xy_cl_puts(sx+12, sy+3, T_LIGHTGRAY, bg, "Init");
	t_xy_cl_puts(sx+37, sy+3, T_LIGHTGRAY, bg, "Check");
	t_mark_hline(sx+1,  sy+3, mw-2, T_BLUE);

	// shadow
	for (i=sx+1; i<=sx+mw; i++)
		t_shadow_char(i, sy+mh);
		
	for (i=sy+1; i<=sy+mh; i++)
		t_shadow_char(sx+mw, i);
	
	dirty = 1;
	
	pg_max	= dlen / 128;
	pg		= elen / 128;
	if (pg > pg_max)
		pg_max = pg;
	pg = 0;

	while (1)
	{
		if (dirty)
		{
			dirty = 0;
			t_color(fg, bg);
			t_xy_put_rect_solid(sx+2, sy+4, sx+60, sy+19);
			
			for (j=0; j<16; j++)
			{
				pi = pg * 128 + j * 8;	// index

				if ((uint32_t)pi >= dlen && (uint32_t)pi >= elen)
					break;

				// address
				sprintf(str, "%8X", pi & 0xFFFFFFF8);
				t_xy_cl_puts(sx+2, sy+4+j, T_YELLOW, bg, str);

				t_color(fg, bg);

				ty = sy + 4;
				for (i=0; i<8; i++, pi++)
				{
					if ((uint32_t)pi < dlen)
					{
						// dat: hex data
						sprintf(str, "%02X", dat[pi]);
						t_xy_puts(sx+12+i*3, ty+j, str);
					}
					
					if ((uint32_t)pi < elen)
					{
						// err: hex data
						sprintf(str, "%02X", err[pi]);
						t_xy_puts(sx+37+i*3, ty+j, str);
					}
				}
			}
			
			// mark error
			for (j=0; j<16; j++)
			{
				pi = pg * 128 + j * 8;	// index

				for (i=0; i<8; i++, pi++)
				{
					if ((uint32_t)pi < dlen && (uint32_t)pi < elen)
					{
						if (dat[pi] != err[pi])
						{
							t_mark_hline(sx+12+i*3, sy+4+j, 2, T_RED);
							t_mark_hline(sx+37+i*3, sy+4+j, 2, T_RED);
						}
					}
					else
					{
						break;
					}
				}
			}//mark error

			// scroll bar : one of dlen/elen > 128 bytes
			if (dlen > 128 || elen > 128)
			{
				sb_len = 16 / (pg_max+1);

				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (pg == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if (pg  == pg_max)
				{
					sb_end = (16 - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (pg + 1) * 16 / (pg_max + 1);
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (16-2))
					{
						sb_end = 16 - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}
				
				t_color(T_WHITE, T_BLACK);

				// 16: 0~15
				tx = sx + mw - 2;
				for (j=0; j<16; j++)
				{
					if (j<sb_sta || j>sb_end)
						t_xy_putc(tx, j+6, 0xb0);
					else
						t_xy_putc(tx, j+6, 0xb2);
				}
			}

			t_color(fg, bg);

			// page index
			if (pg == pg_max)
				sprintf(str, " %4d of %4d ", (pg_max+1)*16, (pg_max+1)*16);
			else
				sprintf(str, " %4d of %4d ", (pg+1)*16, (pg_max+1)*16);
			t_xy_puts(sx+mw-17, sy+mh-1, str);
		}

		key = (uint16_t)bioskey(1);
		if (key)
		{
			scan = (key >> 8);//scan = key_blk_read_sc();
			if (scan == SCAN_ESC || key == CHAR_CARRIAGE_RETURN)
			{
				break;
			}
			else if (scan == SCAN_PAGE_DOWN || scan == SCAN_DOWN)
			{
				pg++;
				if (pg > pg_max)
					pg = pg_max;
				dirty = 1;
			}
			else if (scan == SCAN_PAGE_UP || scan == SCAN_UP)
			{
				pg--;
				if (pg < 0)
					pg = 0;
				dirty = 1;
			}
			else if (scan == SCAN_HOME)
			{
				pg = 0;
				dirty = 1;
			}
			else if (scan == SCAN_END)
			{
				pg = pg_max;
				dirty = 1;
			}
		}
	}

	t_paste_region(-1, -1);
}

//=============================================================================
//  chkrec_smbios_show
//=============================================================================
static void chkrec_smbios_show(uint16_t no, uint32_t mask, file_t *ef)
{
	uint8_t			fg, smchk;
	char			str[256];
	uint8_t			*ebuf;
	smbios_hdr_t	*hdr;
	smbios_v3hdr_t	*v3hdr;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	int				elen;
	t_msgbox_msg_t	msg;


	msgbox_msg_init(&msg);
	
	smchk = GET_VAR_GV(mask, CHKREC_GM_SM_DATA, CHKREC_GP_SM_DATA);
	if (smchk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTCYAN;
	}
	else if (smchk == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else
	{
		fg = T_ARG_LIGHTGRAY;
	}

	//---------------------------------------------------------
	//  Init >
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);


	raw = raw_find_id(RAW_ID_DMI_SMBIOS_HDR);
	if (raw)
	{
		hdr = (smbios_hdr_t *)raw->data;
		sprintf(str, "\r    Addr : 0x%08X    Len  : %4d = 0x%4X    Ver  : %d.%d\n\rDMI Addr : 0x%08X    Size : %4d = 0x%4X    Num  : %d",
				hdr, hdr->len, hdr->len,
				hdr->major_ver,hdr->minor_ver,
				hdr->smbios_addr,
				hdr->smbios_len, hdr->smbios_len,
				hdr->smbios_ent_num);
	}
	else
	{
		msgbox_msg_add_str("\r SMBIOS : n/a", &msg);
	}
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);


	raw = raw_find_id(RAW_ID_DMI_SMBIOS_V3HDR);
	if (raw)
	{
		v3hdr = (smbios_v3hdr_t *)raw->data;
		
		sprintf(str, "\n\r V3 Addr : 0x%08X    Len  : %4d = 0x%4X    Ver  : %d.%d\n\rDMI Addr : 0x%08LX    Size : %4d = 0x%4X    Rev  : %d.%d",
				v3hdr, v3hdr->len, v3hdr->len, v3hdr->major_ver, v3hdr->minor_ver,
				v3hdr->tab_addr, v3hdr->tab_sz, v3hdr->tab_sz, v3hdr->doc_rev, v3hdr->eps_rev);
	}
	else
	{
		msgbox_msg_add_str("\r SMBIOS V3 : n/a", &msg);
	}
	msgbox_msg_add_str(str, &msg);

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check >
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	if (smchk == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sm;
	}

	if (smchk == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sm;
	}

	if (smchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cSize : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (smchk == CHKREC_ERR)
	{
#if 0
		ebuf = pu_err_raw_find(no, RAW_ID_MEM_E820_KSZ, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cSize : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			sprintf(str, "\r\b%cSize : %d KiB", T_ARG_LIGHTMAGENTA, *(uint32_t *)ebuf);
			msgbox_msg_add_str(str, &msg);
		}
#endif

		raw = raw_find_id(RAW_ID_DMI_SMBIOS);
		ebuf = pu_err_raw_find(no, RAW_ID_DMI_SMBIOS, ef);
		if (ebuf)
		{
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			elen = eraw->len;
		}
		else
		{
			elen = 0;
			ebuf = NULL;
		}

		chkrec_smbios_err_show(no, raw->data, raw->len, ebuf, elen);
		return;
	}

end_sm:
	
	sprintf(str, "%d : Check Record - SM : SMBIOS Data", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//===========================================================================
//  chkrec_sort_rpci_inc
//===========================================================================
int chkrec_sort_rpci_inc(const void *a, const void *b)
{
	chkrec_pci_dev_t	*ia = (chkrec_pci_dev_t *)a;
	chkrec_pci_dev_t	*ib = (chkrec_pci_dev_t *)b;

	// 1st key : bdf
	if (ia->bdf != ib->bdf)
	{
		return (ia->bdf - ib->bdf);
	}
	else
	{
		// 2nd key : vid
		if (ia->vid != ib->vid)
		{
			return (ia->vid - ib->vid);
		}
		else
		{
			// 3rd key : did
			if (ia->did != ib->did)
			{
				return (ia->did - ib->did);
			}
			else
			{
				// 4th key : flg
				return (ia->flg - ib->flg);
			}
		}
	}
}

//=============================================================================
//  chkrec_pci_err_show
//=============================================================================
static void chkrec_pci_err_show(uint16_t no, chkrec_pci_dev_t *rpci, int rnum)
{
	int			i, j;
	int			sx, sy, ex, ey;
	int			tx, mw, mh;
	uint8_t		fg, bg, dirty;
	char		str[256];
	int			pg, pi, pg_max;
	uint8_t		scan;
	int			sb_sta, sb_end, sb_len;

	chkrec_pci_dev_t	*rp;
	uint16_t key;


	// 0         1         2         3         4         5         6
	// 01234567890123456789012345678901234567890123456789012345678901234
	// +-----------------------------------------------+
	// |      Check Record - PC : PCI Bus Scan         |
	// +-----------------------------------------------+
	// | B  D  F    DAT_VID_DID   ERR_VID_DID   BDF    |
	// | 00 00 00   8086:1234     FFFF:5678     ABCD  X|
	// |........

	//-------------------------------------------------
	//  error compare
	//-------------------------------------------------
	mw = 49 + 1;
	mh = 22;
	sx = (80 - mw)/2;
	sy = (25 - mh)/2;
	ex = sx + mw;	// with shadow
	ey = sy + mh;	// with shadow
	fg = T_WHITE;
	bg = T_BLACK;

	t_copy_region(sx, sy, ex, ey);

	t_color(fg, bg);

	for (j=sy; j<sy+mh; j++)
	{
		t_gotoxy(sx, j);

		for (i=sx; i<sx+mw; i++)
		{
			// top border
			if (j == sy)
			{
				if (i==sx)				t_putc(0xda);	// left-top
				else if (i==sx+mw-1) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			// bottom border
			else if (j==sy+mh-1)
			{
				if (i==sx)				t_putc(0xc0);	// left-bottom
				else if (i==sx+mw-1) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
			}
			// title seperator
			else if (j==sy+2)
			{
				if (i==sx)				t_putc(0xc6);	// left-middle
				else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
				else					t_putc(0xcd);	// --
			}
			// left/right border
			else
			{
				if (i==sx || i==sx+mw-1)	t_putc(0xb3);	// |
				else						t_putc(0x20);
			}
		}
	}

	// 0         1         2         3         4         5         6
	// 01234567890123456789012345678901234567890123456789012345678901234
	// +-----------------------------------------------+
	// |      Check Record - PC : PCI Bus Scan         |
	// +-----------------------------------------------+
	// | B  D  F    DAT_VID_DID   ERR_VID_DID   BDF    |
	// | 00 00 00   8086:1234     FFFF:5678     ABCD  X|
	// |........
	sprintf(str, "%d : Check Record - PC : PCI Bus Scan", no);
	t_xy_puts(sx+(mw-(int)strlen(str))/2, sy+1, str);
	t_xy_cl_puts(sx+2,  sy+3, T_LIGHTGRAY, T_BLUE, "B  D  F    DAT_VID_DID   ERR_VID_DID   BDF");
	//t_mark_hline(sx+1,  sy+3, mw-2, T_BLUE);
	t_pen_hline(sx+13,	sy+3, 11, T_LIGHTGREEN);
	t_pen_hline(sx+27,	sy+3, 11, T_LIGHTRED);

	// shadow
	for (i=sx+1; i<=sx+mw; i++)
		t_shadow_char(i, sy+mh);
		
	for (i=sy+1; i<=sy+mh; i++)
		t_shadow_char(sx+mw, i);
	
	dirty = 1;
	
	pg_max	= (rnum - 1) / 16;
	pg = 0;

	while (1)
	{
		if (dirty)
		{
			dirty = 0;

			t_color(fg, bg);
			t_xy_put_rect_solid(sx+2, sy+4, sx+47, sy+19);
			
			// 0         1         2         3         4         5         6
			// 01234567890123456789012345678901234567890123456789012345678901234
			// +-------------------------------------------+
			// |   Check Record - PC : PCI Bus Scan          |
			// +-------------------------------------------+
			// | B  D  F    VID  DID    VID  DID    BDF    |
			// | 00 00 00   8086:1234   FFFF:5678   ABCD  X|
			// |........
	
			for (j=0; j<16; j++)
			{
				pi = pg*16 + j;
				
				if (pi >= rnum)
					break;

				rp = &rpci[pi];

				// B D F
				sprintf(str, "%02X %02X %02X",
					(rp->bdf >> 8) & 0xFF, (rp->bdf >> 3) & 0x1F, rp->bdf & 0x7);
				t_xy_cl_puts(sx+2, sy+4+j, T_YELLOW, bg, str);
				
				// VID:DID
				if (rp->flg == 0x3)
				{
					sprintf(str, "%04X:%04X     %04X:%04X", rp->vid, rp->did, rp->vid, rp->did);
					t_xy_cl_puts(sx+13, sy+4+j, fg, bg, str);
				}
				else if (rp->flg == 0x1)
				{
					sprintf(str, "%04X:%04X            ", rp->vid, rp->did);
					t_xy_cl_puts(sx+13, sy+4+j, T_LIGHTRED, bg, str);
				}
				else if (rp->flg == 0x2)
				{
					sprintf(str, "              %04X:%04X", rp->vid, rp->did);
					t_xy_cl_puts(sx+13, sy+4+j, T_LIGHTRED, bg, str);
				}

				// BDF
				sprintf(str, "%04X", rp->bdf);
				t_xy_cl_puts(sx+41, sy+4+j, T_LIGHTGRAY, bg, str);
			}

			// scroll bar
			if (rnum > 16)
			{
				sb_len = 16 / (pg_max+1);

				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (pg == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if (pg  == pg_max)
				{
					sb_end = (16 - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (pg + 1) * 16 / (pg_max + 1);
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (16-2))
					{
						sb_end = 16 - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}
				
				t_color(T_WHITE, T_BLACK);

				// 16: 0~15
				tx = sx + mw - 2;
				for (j=0; j<16; j++)
				{
					if (j<sb_sta || j>sb_end)
						t_xy_putc(tx, j+5, 0xb0);
					else
						t_xy_putc(tx, j+5, 0xdb);	//0xb2
				}
			}

			t_color(fg, bg);

			// page index
			if (pg == pg_max)
				sprintf(str, " %4d of %4d ", rnum, rnum);
			else
				sprintf(str, " %4d of %4d ", (pg+1)*16, rnum);
			t_xy_cl_puts(sx+mw-15,	sy+mh-2, T_WHITE, T_BLUE, str);
			t_mark_hline(sx+1,		sy+mh-2, mw-2, T_BLUE);
			
		} //dirty

		key = (uint16_t)bioskey(1);
		if (key)
		{
			scan = (key >> 8);//scan = key_blk_read_sc();
			if (scan == SCAN_ESC || key == CHAR_CARRIAGE_RETURN)
			{
				break;
			}
			else if (scan == SCAN_PAGE_DOWN || scan == SCAN_DOWN)
			{
				pg++;
				if (pg > pg_max)
					pg = pg_max;
				dirty = 1;
			}
			else if (scan == SCAN_PAGE_UP || scan == SCAN_UP)
			{
				pg--;
				if (pg < 0)
					pg = 0;
				dirty = 1;
			}
			else if (scan == SCAN_HOME)
			{
				pg = 0;
				dirty = 1;
			}
			else if (scan == SCAN_END)
			{
				pg = pg_max;
				dirty = 1;
			}
		}
	}

	t_paste_region(-1, -1);
}

//=============================================================================
//  chkrec_pci_show
//=============================================================================
static void chkrec_pci_show(uint16_t no, uint32_t mask, file_t *ef)
{
	int			i, j;
	uint8_t		fg, pcchk, found;
	int			rnum, cnum, nnum;
	uint16_t	bdf, vid, did;
	pu_raw_t	*rawnum, *rawbdf, *rawlst;
	uint8_t		*chknum, *chkbdf, *chklst;
	uint8_t		/**buf,*/ *pbdf, *plst;
	char		str[256];
//	FILE		*fp;	// debug
	
	chkrec_pci_dev_t	*rpci;
	chkrec_pci_dev_t	*rp;
	t_msgbox_msg_t		msg;


	msgbox_msg_init(&msg);

	pcchk = GET_VAR_GV(mask, CHKREC_GM_PC_SCAN, CHKREC_GP_PC_SCAN);
	if (pcchk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTCYAN;
	}
	else if (pcchk == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else
	{
		fg = T_ARG_LIGHTGRAY;
	}

	//---------------------------------------------------------
	//  RAW : pci : raw data in POWERUP.DAT
	//---------------------------------------------------------
	rawnum = raw_find_id(RAW_ID_PCI_DEV_NUM);
	rawbdf = raw_find_id(RAW_ID_PCI_DEV_BDF);
	rawlst = raw_find_id(RAW_ID_PCI_DEV_LST);

	if (!rawnum)
	{
		log_printf("[%d] chkrec : pci num : raw data n/a", pu->no);
		goto raw_na;
	}

	if (!rawbdf)
	{
		log_printf("[%d] chkrec : pci bdf : raw data n/a", pu->no);
		goto raw_na;
	}

	if (!rawlst)
	{
		log_printf("[%d] chkrec : pci lst : raw data n/a", pu->no);
		goto raw_na;
	}

	rnum = *(uint32_t *)rawnum->data;	// RAW : num of dev

	//---------------------------------------------------------
	//  CHK : pci : error data in POWERUP.ERR
	//---------------------------------------------------------
	//
	// (pu_err_raw_t)---(error logging data ....)
	//     16 bytes      variable length
	//    <header>        
	//
	//---------------------------------------------------------
	chknum = pu_err_raw_find(no, RAW_ID_PCI_DEV_NUM, ef);
	chkbdf = pu_err_raw_find(no, RAW_ID_PCI_DEV_BDF, ef);
	chklst = pu_err_raw_find(no, RAW_ID_PCI_DEV_LST, ef);

	if (chknum && chkbdf && chklst)
	{
		//buf = (uint8_t *)(chknum);
		//buf += sizeof(pu_err_raw_t);	// point to error data
			
		//cnum = *(uint32_t *)buf;		// CHK : num of dev
			
		cnum = (int)*(uint32_t*)&chknum[16];	// CHK : num of dev
	}
	else
	{
		cnum = 0;
	}
	//---------------------------------------------------------
	//  Init > PC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	sprintf(str, "\rNum of Dev : %d", rnum);
	msgbox_msg_add_str(str, &msg);

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > PC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	if (pcchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cDev List : OK", fg);
	}
	else
	{
		if (chknum)
			sprintf(str, "\r\b%cDev List : Error\n\rNum of Error Dev : %d", T_ARG_LIGHTMAGENTA, cnum);
		else
			//sprintf(str, "\r\b%cDev List : Error", T_ARG_LIGHTMAGENTA);
			sprintf(str, "\r\b%cDev List : Error\n\rErr Log  : not found in POWERUP.ERR", T_ARG_LIGHTRED);
	}
		
	msgbox_msg_add_str(str, &msg);

	sprintf(str, "%d : Check Record - PC : PCI Bus Scan", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);

	//---------------------------------------------------------
	//  Combine the pci device list of RAW and CHK
	//---------------------------------------------------------
	// need to allocate max space of two list (rnum + cnum)
	//---------------------------------------------------------
	rpci = (chkrec_pci_dev_t *)malloc((rnum+cnum) * sizeof(chkrec_pci_dev_t));
	if (!rpci)
	{
		log_printf("[%d] chkrec : rpci malloc err!", pu->no);
		goto raw_na;
	}

	pbdf = rawbdf->data;
	plst = rawlst->data;

	// feeding rpci : dev list of RAW
	for (i=0; i<rnum; i++)
	{
		rp = &rpci[i];

		// bdf : merge bus, dev and fun into bdf (2-bytes)
		// bdf : b=[15:8]; d=[7:3], f=[2:0]
		bdf = (*pbdf++ & 0xFF);		// bus
		bdf <<= 5;
		bdf |= (*pbdf++ & 0x1F);	// dev
		bdf <<= 3;
		bdf |= (*pbdf++ & 0x7);		// fun

		rp->bdf = bdf;

		rp->vid = *(uint16_t *)plst;
		plst += 2;

		rp->did = *(uint16_t *)plst;
		plst += 2;

		rp->flg = 0x1;	// [0]= flag : raw
	}
	
	// debug
#if 0
	fp = fopen("rpci0.txt", "w");
	if (fp)
	{
		for (i=0; i<rnum; i++)
		{
			rp = &rpci[i];
			//fprintf(fp, "%4d > %04X = %04X:%04X  %d\n", i, rp->bdf, rp->vid, rp->did, rp->flg);
			fprintf(fp, "%04X = %04X:%04X  %d\n", rp->bdf, rp->vid, rp->did, rp->flg);
		}
		fclose(fp);
	}
#endif

	// count new dev which is not existed in dev list of RAW
	nnum = 0;

	//---------------------------------------------------------
	//  CHK : pci : error existed ?
	//---------------------------------------------------------
	if (!chknum)
	{
		log_printf("[%d] chkrec : pci num : check data n/a", pu->no);
		goto chk_na;
	}

	if (!chkbdf)
	{
		log_printf("[%d] chkrec : pci bdf : check data n/a", pu->no);
		goto chk_na;
	}

	if (!chklst)
	{
		log_printf("[%d] chkrec : pci lst : check data n/a", pu->no);
		goto chk_na;
	}

	if (cnum == 0)
	{
		log_printf("[%d] chkrec : pci num : num of dev = 0", pu->no);
		goto chk_na;
	}

	// feeding dev list of CHK
	
	// (pu_err_raw_t)---(error logging data ....)
	//pbdf = chkbdf + sizeof(chkrec_pci_dev_t);
	//plst = chklst + sizeof(chkrec_pci_dev_t);
	pbdf = chkbdf + sizeof(pu_err_raw_t);
	plst = chklst + sizeof(pu_err_raw_t);

	for (i=0; i<cnum; i++)
	{
		// bdf : b=[15:8]; d=[7:3], f=[2:0]
		bdf = (*pbdf++ & 0xFF);		// bus
		bdf <<= 5;
		bdf |= (*pbdf++ & 0x1F);	// dev
		bdf <<= 3;
		bdf |= (*pbdf++ & 0x7);		// fun

		vid = *(uint16_t *)plst;
		plst += 2;

		did = *(uint16_t *)plst;
		plst += 2;

		// bdf, vid and did of CHK are ready, start to combine RAW and CHK

		found = 0;

		// find the same device in RAW
		for (j=0; j<rnum; j++)
		{
			rp = &rpci[j];

			if (bdf == rp->bdf && vid == rp->vid && did == rp->did)
			{
				// matched!
				rp->flg |= 0x2;	// [1] flag : CHK
				found = 1;
				break;
			}
		}

		// new dev
		if (found == 0)
		{
			// feeding
			rp = &rpci[rnum + nnum];

			rp->bdf = bdf;
			rp->vid = vid;
			rp->did = did;
			rp->flg = 0x2;	// [1]= flag : CHK

			nnum++;
		}
	}

chk_na:

	rnum += nnum;	// real size of list

	// realloc() : now we got the real size of list, need to shrink it.
	rpci = (chkrec_pci_dev_t *)realloc(rpci, rnum * sizeof(chkrec_pci_dev_t));
	if (!rpci)
	{
		log_printf("[%d] chkrec : rpci realloc err!", pu->no);
		msgbox_waitkey(T_WHITE, T_RED, "PCI", "rpci realloc error!", 4, 1);
		return;
	}

	// debug
#if 0
	fp = fopen("rpci1.txt", "w");
	if (fp)
	{
		for (i=0; i<rnum; i++)
		{
			rp = &rpci[i];
			//fprintf(fp, "%4d > %04X = %04X:%04X  %d\n", i, rp->bdf, rp->vid, rp->did, rp->flg);
			fprintf(fp, "%04X = %04X:%04X  %d\n", rp->bdf, rp->vid, rp->did, rp->flg);
		}
		fclose(fp);
	}
#endif

	// sort list by bdf, vid and did respectively to get sorted list
	qsort(rpci, rnum, sizeof(chkrec_pci_dev_t), chkrec_sort_rpci_inc);
	
	// debug
#if 0
	fp = fopen("rpci2.txt", "w");
	if (fp)
	{
		for (i=0; i<rnum; i++)
		{
			rp = &rpci[i];
			//fprintf(fp, "%4d > %04X = %04X:%04X  %d\n", i, rp->bdf, rp->vid, rp->did, rp->flg);
			fprintf(fp, "%04X = %04X:%04X  %d\n", rp->bdf, rp->vid, rp->did, rp->flg);
		}
		fclose(fp);
	}
#endif

	chkrec_pci_err_show(no, rpci, rnum);

	return;

raw_na:

	msgbox_waitkey(T_WHITE, T_RED, "PCI", "RAW n/a", 4, 1);
	return;

}

//=============================================================================
//  chkrec_smbus_show
//=============================================================================
static void chkrec_smbus_show(uint16_t no, uint32_t mask, file_t *ef)
{
	int				i;
	uint8_t			num, *ptr;
	uint8_t			fg, sbchk;
	char			str[256];
	uint8_t			*ebuf;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;

	
	msgbox_msg_init(&msg);

	num = 0;

	sbchk = GET_VAR_GV(mask, CHKREC_GM_SB_SCAN, CHKREC_GP_SB_SCAN);
	if (sbchk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTCYAN;
	}
	else if (sbchk == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else
	{
		fg = T_ARG_LIGHTGRAY;
	}
		 
	//---------------------------------------------------------
	//  Init > SB
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	raw = raw_find_id(RAW_ID_SMB_DEV_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(str, "\rNum : %d", *(uint8_t *)raw->data);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rNum : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);

	raw = raw_find_id(RAW_ID_SMB_DEV_LST);
	if (raw)
	{
		msgbox_msg_add_str("\rDev : ", &msg);
	
		ptr = raw->data;
		
		for (i=0; i<num; i++)
		{
			if ((i & 0xF) == 0 && i > 0)
			{
				msgbox_msg_add_char('\n', &msg);
				msgbox_msg_add_char('\r', &msg);
				msgbox_msg_add_char('\b', &msg);
				msgbox_msg_add_char(T_ARG_LIGHTGREEN, &msg);
				msgbox_msg_add_str("      ", &msg);
			}
			
			sprintf(str, "%02X ", *ptr++);
			msgbox_msg_add_str(str, &msg);
		}
	}
	else
	{
		msgbox_msg_add_str("\rDev : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > SB
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	if (sbchk == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sb;
	}

	if (sbchk == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sb;
	}

	// SB: num
	if (sbchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cNum : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (sbchk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_SMB_DEV_NUM, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cNum : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			sprintf(str, "\r\b%cNum : %d", T_ARG_LIGHTMAGENTA, *(uint8_t *)ebuf);
			msgbox_msg_add_str(str, &msg);
		}
	}
	msgbox_msg_add_char('\n', &msg);

	// SB: dev
	if (sbchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cDev : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (sbchk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_SMB_DEV_LST, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cDev : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cDev : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
				
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			ptr = ebuf;

			for (i=0; (uint32_t)i<eraw->len; i++)
			{
				if ((i & 0xF) == 0 && i > 0)
				{
					msgbox_msg_add_char('\n', &msg);
					msgbox_msg_add_char('\r', &msg);
					msgbox_msg_add_char('\b', &msg);
					msgbox_msg_add_char(T_ARG_LIGHTMAGENTA, &msg);
					msgbox_msg_add_str("      ", &msg);
				}
			
				sprintf(str, "%02X ", *ptr++);
				msgbox_msg_add_str(str, &msg);
			}
		}
	}
	
end_sb:

	sprintf(str, "%d : Check Record - SM : SMBus Scan", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_spd_show
//=============================================================================
static void chkrec_spd_show(uint16_t no, uint32_t mask, file_t *ef)
{
	int				i;
	uint8_t			fg, spchk;
	uint8_t			num, *ptr;
	char			str[256];
	uint8_t			*ebuf;
	pu_raw_t		*raw;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;

	
	msgbox_msg_init(&msg);

	spchk = GET_VAR_GV(mask, CHKREC_GM_SP_SCAN, CHKREC_GP_SP_SCAN);
	if (spchk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTCYAN;
	}
	else if (spchk == CHKREC_OK)
	{
		fg = T_ARG_LIGHTMAGENTA;
	}
	else
	{
		fg = T_ARG_LIGHTGRAY;
	}

	//---------------------------------------------------------
	//  Init > SP
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	// SP : num
	raw = raw_find_id(RAW_ID_SMB_SPD_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(str, "\rNum : %d", *(uint8_t *)raw->data);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		msgbox_msg_add_str("\rNum : n/a", &msg);
	}
	msgbox_msg_add_char('\n', &msg);

	// SP : lst
	raw = raw_find_id(RAW_ID_SMB_SPD_LST);
	if (raw)
	{
		msgbox_msg_add_str("\rSPD : ", &msg);
	
		ptr = raw->data;
		
		for (i=0; (uint32_t)i<raw->len; i++)
		{
			if ((i & 0xF) == 0 && i > 0)
			{
				msgbox_msg_add_char('\n', &msg);
				msgbox_msg_add_char('\r', &msg);
				msgbox_msg_add_char('\b', &msg);
				msgbox_msg_add_char(T_ARG_LIGHTGREEN, &msg);
				msgbox_msg_add_str("      ", &msg);
			}
			
			sprintf(str, "%02X ", *ptr++);
			msgbox_msg_add_str(str, &msg);
		}
	}
	else
	{
		msgbox_msg_add_str("\rSPD : n/a", &msg);
	}

	msgbox_msg_add_char('\n', &msg);
	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > SP
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);
	
	if (spchk == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sp;
	}

	if (spchk == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_sp;
	}
	
	// SP : num
	if (spchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cNum : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (spchk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_SMB_SPD_NUM, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cNum : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			sprintf(str, "\r\b%cNum : %d", T_ARG_LIGHTMAGENTA, *(uint8_t *)ebuf);
			msgbox_msg_add_str(str, &msg);
		}
	}
	msgbox_msg_add_char('\n', &msg);
	
	// SP: list
	if (spchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cSPD : OK", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);
	}
	else if (spchk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_SMB_SPD_LST, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cSPD : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cSPD : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
				
			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);
			
			ptr = ebuf;

			for (i=0; (uint32_t)i<eraw->len; i++)
			{
				if ((i & 0xF) == 0 && i > 0)
				{
					msgbox_msg_add_char('\n', &msg);
					msgbox_msg_add_char('\r', &msg);
					msgbox_msg_add_char('\b', &msg);
					msgbox_msg_add_char(T_ARG_LIGHTMAGENTA, &msg);
					msgbox_msg_add_str("      ", &msg);
				}
			
				sprintf(str, "%02X ", *ptr++);
				msgbox_msg_add_str(str, &msg);
			}
		}
	}
	
end_sp:
	
	sprintf(str, "%d : Check Record - SP : SPD Detect", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_rtc_show
//=============================================================================
static void chkrec_rtc_show(uint16_t no, uint32_t mask, file_t *ef)
{
	uint8_t			fg, rtcchk;
	uint8_t			rtc0a, rtc0b, rtc0d;
	char			str[256];
	uint8_t			*ebuf;
	pu_err_raw_t	*eraw;
	t_msgbox_msg_t	msg;

	
	msgbox_msg_init(&msg);
	
	rtc0a = GET_VAR_GV(mask, CHKREC_GM_RTC_0A, CHKREC_GP_RTC_0A);
	rtc0b = GET_VAR_GV(mask, CHKREC_GM_RTC_0B, CHKREC_GP_RTC_0B);
	rtc0d = GET_VAR_GV(mask, CHKREC_GM_RTC_0D, CHKREC_GP_RTC_0D);
	
	if (rtc0a == CHKREC_ERR || rtc0b == CHKREC_ERR || rtc0d == CHKREC_ERR)
	{
		fg = T_ARG_LIGHTMAGENTA;
		rtcchk = CHKREC_ERR;
	}
	else if (rtc0a == CHKREC_NOFEAT || rtc0b == CHKREC_NOFEAT || rtc0d == CHKREC_NOFEAT)
	{
		fg = T_ARG_LIGHTGRAY;
		rtcchk = CHKREC_NOFEAT;
	}
	else if (rtc0a == CHKREC_NOCHK || rtc0b == CHKREC_NOCHK || rtc0d == CHKREC_NOCHK)
	{
		fg = T_ARG_LIGHTGRAY;
		rtcchk = CHKREC_NOCHK;
	}
	else
	{
		fg = T_ARG_LIGHTCYAN;
		rtcchk = CHKREC_OK;
	}

	//---------------------------------------------------------
	//  Init > RTC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cInit >", T_ARG_LIGHTGREEN);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	msgbox_msg_add_str("\rRTC[0A] : RTC crystal : & 0x7F = 0x26\n", &msg);
	msgbox_msg_add_str("\rRTC[0B] : RTC status  :        = 0x02\n", &msg);
	msgbox_msg_add_str("\rRTC[0D] : RTC power   : & 0x80 = 0x80\n", &msg);

	msgbox_msg_add_char('\n', &msg);

	//---------------------------------------------------------
	//  Check > RTC
	//---------------------------------------------------------
	sprintf(str, "\r\b%cCheck >", fg);
	msgbox_msg_add_str(str, &msg);
	msgbox_msg_add_char('\n', &msg);

	if (rtcchk == CHKREC_NOCHK)
	{
		sprintf(str, "\r\b%cCheck : off", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_rtc;
	}

	if (rtcchk == CHKREC_NOFEAT)
	{
		sprintf(str, "\r\b%cFeature : n/a", T_ARG_LIGHTGRAY);
		msgbox_msg_add_str(str, &msg);

		goto end_rtc;
	}

	// RTC : ABD
	if (rtcchk == CHKREC_OK)
	{
		sprintf(str, "\r\b%cRTC[0A] : OK\n", T_ARG_LIGHTCYAN);
		msgbox_msg_add_str(str, &msg);

		msgbox_msg_add_str("\rRTC[0B] : OK\n", &msg);
		msgbox_msg_add_str("\rRTC[0D] : OK\n", &msg);
	}
	else if (rtcchk == CHKREC_ERR)
	{
		ebuf = pu_err_raw_find(no, RAW_ID_RTC_ABD, ef);
		if (!ebuf)
		{
			sprintf(str, "\r\b%cRTC : n/a", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);
		}
		else
		{
			sprintf(str, "\r\b%cRTC : ", T_ARG_LIGHTMAGENTA);
			msgbox_msg_add_str(str, &msg);

			eraw = (pu_err_raw_t *)(ebuf);
			ebuf += sizeof(pu_err_raw_t);

			sprintf(str, "RTC[0A] : RTC crystal = 0x%02X", *ebuf++);
			msgbox_msg_add_str(str, &msg);
			sprintf(str, "RTC[0B] : RTC status  = 0x%02X", *ebuf++);
			msgbox_msg_add_str(str, &msg);
			sprintf(str, "RTC[0D] : RTC power   = 0x%02X", *ebuf);
			msgbox_msg_add_str(str, &msg);
		}
	}

end_rtc:
	
	sprintf(str, "%d : Check Record - RTC : RTC - CMOS RAM", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_tmp_show
//=============================================================================
static void chkrec_tmp_show(uint16_t no)
{
	uint8_t			fg;
	char			str[256];
	t_msgbox_msg_t	msg;
	pu_rec_t		*rec;

	rec = pu_find_rec_no(no);
	if (!rec)
		return;
	
	msgbox_msg_init(&msg);

	if (rec->cpu_temp & 0x80)
	{
		sprintf(str, "\b%c-%2d C", T_ARG_LIGHTBLUE, 0xFF - rec->cpu_temp + 1);
	}
	else
	{
		if (rec->cpu_temp < 30)
			fg = T_ARG_LIGHTBLUE;		//   ~30
		else if (rec->cpu_temp < 40)
			fg = T_ARG_LIGHTCYAN;		// 30~39
		else if (rec->cpu_temp < 50)
			fg = T_ARG_LIGHTGREEN;		// 40~49
		else if (rec->cpu_temp < 60)
			fg = T_ARG_YELLOW;			// 50~59
		else if (rec->cpu_temp < 70)
			fg = T_ARG_LIGHTMAGENTA;	// 60~69
		else if (rec->cpu_temp < 80)
			fg = T_ARG_LIGHTRED;		// 70~79
		else
			fg = T_ARG_WHITE;			// 80~
		sprintf(str, "\b%c%3d %cC", fg, rec->cpu_temp, 0xf8);
	}
	msgbox_msg_add_str(str, &msg);
	
	sprintf(str, "%d : TMP : Temperature", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_tsc_show
//=============================================================================
static void chkrec_tsc_show(uint16_t no)
{
	char			str[256];
	t_msgbox_msg_t	msg;
	pu_rec_t		*rec;

	rec = pu_find_rec_no(no);
	if (!rec)
		return;
	
	msgbox_msg_init(&msg);

	// TSC
	if (rec->cpu_tsc < 1000.0)
		sprintf(str, "\b%c%6.2f Giga", T_ARG_LIGHTCYAN, rec->cpu_tsc);
	else if (rec->cpu_tsc < 10000.0)
		sprintf(str, "\b%c%6.1f Giga", T_ARG_LIGHTCYAN, rec->cpu_tsc);
	else if (rec->cpu_tsc < 100000.0)
		sprintf(str, "\b%c%6d Giga", T_ARG_LIGHTCYAN, (int)rec->cpu_tsc);
	else
		sprintf(str, "\b%c%d Giga", T_ARG_LIGHTCYAN, (int)rec->cpu_tsc);

	msgbox_msg_add_str(str, &msg);
	
	sprintf(str, "%d : TSC : Time Stamp Count", no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

//=============================================================================
//  chkrec_intv_show
//=============================================================================
static void chkrec_intv_show(uint16_t no)
{
	char			str[256];
	char			from_str[32], to_str[32];
	char			res_str[8], intv_str[32];
	uint8_t			fg;
	t_msgbox_msg_t	msg;
	pu_rec_t		*rec, *rec0 = NULL;
	int32_t			offtm;

	rec = pu_find_rec_no(no);
	if (!rec)
		return;

	// previous record
	rec0 = pu_find_rec_no(rec->no-1);
	if (!rec0)
		return;
	
	/*
	if (no > 1)
		rec0 = &dat->rec_pool[no-2];
	rec = &dat->rec_pool[no-1];
	*/

#if 0
	// previous record
	rno = dat->rec_pool[no-1].no - 1;
	if (rno > 0)
	{
		rec0 = NULL;
		for (i=0; i<dat->par->rec_cnt; i++)
		{
			rec = &dat->rec_pool[i];
			if (rno == rec->no)
			{
				rec0 = &dat->rec_pool[i];
				break;
			}
		}
	}

	rec = &dat->rec_pool[no-1];
#endif

	
	msgbox_msg_init(&msg);

	// INTV
	offtm = (int32_t)rec->intv;


	if (offtm == 0)
	{
		fg = T_ARG_LIGHTGRAY;
		sprintf(res_str, "%s", "N/A");
		sprintf(intv_str, "= %d",  dat->par->intv);
	}
	else if (offtm < 0)
	{
		fg = T_ARG_YELLOW;
		sprintf(res_str, "%s", "Error");
		sprintf(intv_str, "%s", "(back to the past)");
	}
	else if (offtm > dat->par->intv)
	{
		fg = T_ARG_LIGHTRED;
		sprintf(res_str, "%s", "Fail");
		sprintf(intv_str, "> %d",  dat->par->intv);
	}
	else
	{
		fg = T_ARG_LIGHTGREEN;
		sprintf(res_str, "%s", "OK");
		sprintf(intv_str, "<= %d",  dat->par->intv);
	}
	
	if (dat->par->rec_cnt < 10)
	{
		// Int : n/a
		fg = T_ARG_WHITE;
	}
	
	if (rec->no == 1)
		conv_tm_str(&dat->par->tm_zero, from_str);
	else
		conv_tm_str(&rec0->tm_rtc, from_str);

	conv_tm_str(&rec->tm_rtc, to_str);
	
	sprintf(str, "\r\b%cFrom : %s\n", T_ARG_LIGHTGRAY, from_str);
	msgbox_msg_add_str(str, &msg);

	sprintf(str, "\r\b%c  To : %s\n\n", T_ARG_LIGHTGRAY, to_str);
	msgbox_msg_add_str(str, &msg);
	
	if (dat->par->rec_cnt < 10)
	{
		sprintf(str, "\r\b%cINTV : n/a\n", fg);
		msgbox_msg_add_str(str, &msg);
	
		sprintf(str, "\r\b%c  QC : ?", fg);
		msgbox_msg_add_str(str, &msg);
	}
	else
	{
		sprintf(str, "\r\b%cINTV : %d sec  %s\n", fg, offtm, intv_str);
		msgbox_msg_add_str(str, &msg);
	
		sprintf(str, "\r\b%c  QC : %s", fg, res_str);
		msgbox_msg_add_str(str, &msg);
	}
	
	sprintf(str, "%d : INTV : Time Intervals", rec->no);
	msgbox_waitkey(T_WHITE, T_BLACK, str, msg.buf, 4, 1);
}

// 0  1   2  3  4  5  6  7  8
// AB AEC CP E8 SM PC SB SP RTC

//=============================================================================
//  chkrec_vlbox
//=============================================================================
/*
char *chkrec_item[9] = 
{
	" oo : AB  - ACL BIOS",
	"ooo : AEC - ACL EC",
	" oo : CP  - CPU Info",
	"  o : E8  - E820 Size",
	"  o : SM  - SMBIOS Data",
	"  o : PC  - PCI Bus Scan",
	"  o : SB  - SMBus Scan",
	"  o : SP  - SPD Detect",
	"ooo : RTC - RTC Check"
};
*/
char *chkrec_item[12];

t_vlbox_t	chkrec_vlbox;

//=============================================================================
//  chkrec_update_vlbox_ooxx
//=============================================================================
int chkrec_update_vlbox_ooxx(int no, uint32_t *mask, uint8_t fg, uint8_t bg, uint8_t oc, uint8_t xc)
{
	pu_rec_t	*rec = NULL;
	int			i;
	uint8_t		item[4];
	uint8_t		icol[4];
	char		str[256];
	uint32_t	m = 0;


	// oc : o color
	// xc : x color

	for (i=0; i<dat->par->rec_cnt; i++)
	{
		if (no == dat->rec_pool[i].no)
		{
			rec = &dat->rec_pool[i];
			break;
		}
	}

	if (!rec)
		return -1;



	memset(chkrec_item, 0, sizeof(chkrec_item));

	// convert to arg color
	fg |= T_ARG_COLOR_MASK;
	bg |= T_ARG_COLOR_MASK;

	//-------------------------------------------------------------
	//  ooxx mask
	//-------------------------------------------------------------
	// 00=x, 01=o, 10=-(no feature), 11=_(no compare)
	// [31:30] AB  model
	// [29:28] AB  ver
	// [27:26] AEC chip
	// [25:24] AEC board
	// [23:22] AEC ver
	// [21:20] CP  vendor
	// [19:18] CP  name
	// [17:16] E8  E820 size
	// [15:14] SM  SMBIOS data
	// [13:12] PC  PCI bus scan
	// [11:10] SB  SMBus scan
	// [9:8]   SP  SPD
	// [7:6]   RTC A
	// [5:4]   RTC B
	// [3:2]   RTC D
	// [1:0]   RSVD

	//-------------------------------------------------------------
	//  AB : ACL BIOS
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AB))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS))
		{
			// AB : ABIOS : model
			if (rec->acl_flag & PU_REC_ACL_ABIOS_MODEL)
			{
				item[0] = 'o';	icol[0] = oc;
				SET_VAR_GV(m, CHKREC_GM_AB_MODEL, CHKREC_GP_AB_MODEL, CHKREC_OK);
			}
			else
			{
				item[0] = 'x';	icol[0] = xc;
				SET_VAR_GV(m, CHKREC_GM_AB_MODEL, CHKREC_GP_AB_MODEL, CHKREC_ERR);
			}
			
			// AB : ABIOS : ver
			if (rec->acl_flag & PU_REC_ACL_ABIOS_VER)
			{
				item[1] = 'o';	icol[1] = oc;
				SET_VAR_GV(m, CHKREC_GM_AB_VER, CHKREC_GP_AB_VER, CHKREC_OK);
			}
			else
			{
				item[1] = 'x';	icol[1] = xc;
				SET_VAR_GV(m, CHKREC_GM_AB_VER, CHKREC_GP_AB_VER, CHKREC_ERR);
			}
		}
		else
		{
			item[0] = item[1] = '-';
			icol[0] = icol[1] = fg;
			SET_VAR_GV(m, CHKREC_GM_AB_MODEL, CHKREC_GP_AB_MODEL, CHKREC_NOFEAT);
			SET_VAR_GV(m, CHKREC_GM_AB_VER,   CHKREC_GP_AB_VER,   CHKREC_NOFEAT);
		}
	}
	else
	{
		item[0] = item[1] = '_';
		icol[0] = icol[1] = fg;
		SET_VAR_GV(m, CHKREC_GM_AB_MODEL, CHKREC_GP_AB_MODEL, CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_AB_VER,   CHKREC_GP_AB_VER,   CHKREC_NOCHK);
	}

	// AB
	sprintf(str, "\b%c \f%c%c\f%c%c\f%c - AB  : ACL BIOS", bg, icol[0], item[0], icol[1], item[1], fg);
	chkrec_item[0] = malloc(strlen(str) + 1);
	if (!chkrec_item[0])
		return -1;
	sprintf(chkrec_item[0], "%s", str);
	
	//-------------------------------------------------------------
	//  AEC: ACL EC
	//-------------------------------------------------------------
	// "ooo - AEC: ACL EC"
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC))
		{
			// AE : AEC : chip
			if (rec->acl_flag & PU_REC_ACL_AEC_CHIP)
			{
				item[0] = 'o';	icol[0] = oc;
				SET_VAR_GV(m, CHKREC_GM_AEC_CHIP, CHKREC_GP_AEC_CHIP, CHKREC_OK);
			}
			else
			{
				item[0] = 'x';	icol[0] = xc;
				SET_VAR_GV(m, CHKREC_GM_AEC_CHIP, CHKREC_GP_AEC_CHIP, CHKREC_ERR);
			}
			
			// AE : AEC : board
			if (rec->acl_flag & PU_REC_ACL_AEC_BOARD)
			{
				item[1] = 'o';	icol[1] = oc;
				SET_VAR_GV(m, CHKREC_GM_AEC_BOARD, CHKREC_GP_AEC_BOARD, CHKREC_OK);
			}
			else
			{
				item[1] = 'x';	icol[1] = xc;
				SET_VAR_GV(m, CHKREC_GM_AEC_BOARD, CHKREC_GP_AEC_BOARD, CHKREC_ERR);
			}
			// AE : AEC : ver
			if (rec->acl_flag & PU_REC_ACL_AEC_VER)
			{
				item[2] = 'o';	icol[2] = oc;
				SET_VAR_GV(m, CHKREC_GM_AEC_VER, CHKREC_GP_AEC_VER, CHKREC_OK);
			}
			else
			{
				item[2] = 'x';	icol[2] = xc;
				SET_VAR_GV(m, CHKREC_GM_AEC_VER, CHKREC_GP_AEC_VER, CHKREC_ERR);
			}
		}
		else
		{
			item[0] = item[1] = item[2] = '-';
			icol[0] = icol[1] = item[2] = fg;
			SET_VAR_GV(m, CHKREC_GM_AEC_CHIP,  CHKREC_GP_AEC_CHIP,  CHKREC_NOFEAT);
			SET_VAR_GV(m, CHKREC_GM_AEC_BOARD, CHKREC_GP_AEC_BOARD, CHKREC_NOFEAT);
			SET_VAR_GV(m, CHKREC_GM_AEC_VER,   CHKREC_GP_AEC_VER,   CHKREC_NOFEAT);
		}
		sprintf(str, "\b%c\f%c%c\f%c%c\f%c%c\f%c - AEC : ACL EC", bg, icol[0], item[0], icol[1], item[1], icol[2], item[2], fg);
	}
	else
	{
		sprintf(str, "\b%c\f%c___ - AEC : ACL EC", bg, fg);

		SET_VAR_GV(m, CHKREC_GM_AEC_CHIP,  CHKREC_GP_AEC_CHIP,  CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_AEC_BOARD, CHKREC_GP_AEC_BOARD, CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_AEC_VER,   CHKREC_GP_AEC_VER,   CHKREC_NOCHK);
	}

	// AEC
	chkrec_item[1] = malloc(strlen(str) + 1);
	if (!chkrec_item[1])
		return -1;
	sprintf(chkrec_item[1], "%s", str);

	//-------------------------------------------------------------
	//  CP : CPU Info
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_CP))
	{
		// CP : CPU vendor
		if (rec->sys_flag & PU_REC_SYS_CPU_VENDOR)
		{
			item[0] = 'o';	icol[0] = oc;
			SET_VAR_GV(m, CHKREC_GM_CP_VENDOR, CHKREC_GP_CP_VENDOR, CHKREC_OK);
		}
		else
		{
			item[0] = 'x';	icol[0] = xc;
			SET_VAR_GV(m, CHKREC_GM_CP_VENDOR, CHKREC_GP_CP_VENDOR, CHKREC_ERR);
		}
		
		// CP : CPU name
		if (rec->sys_flag & PU_REC_SYS_CPU_NAME)
		{
			item[1] = 'o';	icol[1] = oc;
			SET_VAR_GV(m, CHKREC_GM_CP_NAME, CHKREC_GP_CP_NAME, CHKREC_OK);
		}
		else
		{
			item[1] = 'x';	icol[1] = xc;
			SET_VAR_GV(m, CHKREC_GM_CP_NAME, CHKREC_GP_CP_NAME, CHKREC_ERR);
		}
	}
	else
	{
		item[0] = item[1] = '_';
		icol[0] = icol[1] = fg;
		SET_VAR_GV(m, CHKREC_GM_CP_VENDOR, CHKREC_GP_CP_VENDOR, CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_CP_NAME,   CHKREC_GP_CP_NAME,   CHKREC_NOCHK);
	}

	// CP
	sprintf(str, "\b%c \f%c%c\f%c%c\f%c - CP  : CPU Info", bg, icol[0], item[0], icol[1], item[1], fg);
	chkrec_item[2] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[2])
		return -1;
	sprintf(chkrec_item[2], "%s", str);

	//-------------------------------------------------------------
	//  E8 : E820 Size
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_E8))
	{
		if (rec->sys_flag & PU_REC_SYS_E820)
		{
			item[0] = 'o';	icol[0] = oc;
			SET_VAR_GV(m, CHKREC_GM_E8_SIZE, CHKREC_GP_E8_SIZE, CHKREC_OK);
		}
		else
		{
			item[0] = 'x';	icol[0] = xc;
			SET_VAR_GV(m, CHKREC_GM_E8_SIZE, CHKREC_GP_E8_SIZE, CHKREC_ERR);
		}
	}
	else
	{
		item[0] = '_';
		icol[0] = fg;
		SET_VAR_GV(m, CHKREC_GM_E8_SIZE, CHKREC_GP_E8_SIZE, CHKREC_NOCHK);
	}

	// E8
	sprintf(str, "\b%c  \f%c%c\f%c - E8  : E820 Size", bg, icol[0], item[0], fg);
	chkrec_item[3] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[3])
		return -1;
	sprintf(chkrec_item[3], "%s", str);

	//-------------------------------------------------------------
	//  SM : SMBIOS Data
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SM))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS))
		{
			if (rec->sys_flag & PU_REC_SYS_SMBIOS)
			{
				item[0] = 'o';	icol[0] = oc;
				SET_VAR_GV(m, CHKREC_GM_SM_DATA, CHKREC_GP_SM_DATA, CHKREC_OK);
			}
			else
			{
				item[0] = 'x';	icol[0] = xc;
				SET_VAR_GV(m, CHKREC_GM_SM_DATA, CHKREC_GP_SM_DATA, CHKREC_ERR);
			}
		}
		else
		{
			item[0] = '-';
			icol[0] = fg;
			SET_VAR_GV(m, CHKREC_GM_SM_DATA, CHKREC_GP_SM_DATA, CHKREC_NOFEAT);
		}
	}
	else
	{
		item[0] = '_';
		icol[0] = fg;
		SET_VAR_GV(m, CHKREC_GM_SM_DATA, CHKREC_GP_SM_DATA, CHKREC_NOCHK);
	}

	// SM
	sprintf(str, "\b%c  \f%c%c\f%c - SM  : SMBIOS Data", bg, icol[0], item[0], fg);
	chkrec_item[4] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[4])
		return -1;
	sprintf(chkrec_item[4], "%s", str);

	//-------------------------------------------------------------
	//  PC : PCI Bus Scan
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_PC))
	{
		if (rec->sys_flag & PU_REC_SYS_PCI_SCAN)
		{
			item[0] = 'o';	icol[0] = oc;
			SET_VAR_GV(m, CHKREC_GM_PC_SCAN, CHKREC_GP_PC_SCAN, CHKREC_OK);
		}
		else
		{
			item[0] = 'x';	icol[0] = xc;
			SET_VAR_GV(m, CHKREC_GM_PC_SCAN, CHKREC_GP_PC_SCAN, CHKREC_ERR);
		}
	}
	else
	{
		item[0] = '_';
		icol[0] = fg;
		SET_VAR_GV(m, CHKREC_GM_PC_SCAN, CHKREC_GP_PC_SCAN, CHKREC_NOCHK);
	}

	// PC
	sprintf(str, "\b%c  \f%c%c\f%c - PC  : PCI Bus Scan", bg, icol[0], item[0], fg);
	chkrec_item[5] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[5])
		return -1;
	sprintf(chkrec_item[5], "%s", str);

	//-------------------------------------------------------------
	//  SB : SMBus Scan
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SB))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB))
		{
			if (rec->sys_flag & PU_REC_SYS_SMB_SCAN)
			{
				item[0] = 'o';	icol[0] = oc;
				SET_VAR_GV(m, CHKREC_GM_SB_SCAN, CHKREC_GP_SB_SCAN, CHKREC_OK);
			}
			else
			{
				item[0] = 'x';	icol[0] = xc;
				SET_VAR_GV(m, CHKREC_GM_SB_SCAN, CHKREC_GP_SB_SCAN, CHKREC_ERR);
			}
		}
		else
		{
			item[0] = '-';
			icol[0] = fg;
			SET_VAR_GV(m, CHKREC_GM_SB_SCAN, CHKREC_GP_SB_SCAN, CHKREC_NOFEAT);
		}
	}
	else
	{
		item[0] = '_';
		icol[0] = fg;
		SET_VAR_GV(m, CHKREC_GM_SB_SCAN, CHKREC_GP_SB_SCAN, CHKREC_NOCHK);
	}

	// SB
	sprintf(str, "\b%c  \f%c%c\f%c - SB  : SMBus Scan", bg, icol[0], item[0], fg);
	chkrec_item[6] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[6])
		return -1;
	sprintf(chkrec_item[6], "%s", str);

	//-------------------------------------------------------------
	//  SP : SPD Detect
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SP))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD))
		{
			// SPD present
			if (rec->sys_flag & PU_REC_SYS_SMB_SPD)
			{
				item[0] = 'o';	icol[0] = oc;
				SET_VAR_GV(m, CHKREC_GM_SP_SCAN, CHKREC_GP_SP_SCAN, CHKREC_OK);
			}
			else
			{
				item[0] = 'x';	icol[0] = xc;
				SET_VAR_GV(m, CHKREC_GM_SP_SCAN, CHKREC_GP_SP_SCAN, CHKREC_ERR);
			}
		}
		else
		{
			item[0] = '-';
			icol[0] = fg;
			SET_VAR_GV(m, CHKREC_GM_SP_SCAN, CHKREC_GP_SP_SCAN, CHKREC_NOFEAT);
		}
	}
	else
	{
		item[0] = '_';
		icol[0] = fg;
		SET_VAR_GV(m, CHKREC_GM_SP_SCAN, CHKREC_GP_SP_SCAN, CHKREC_NOCHK);
	}

	// SP
	sprintf(str, "\b%c  \f%c%c\f%c - SP  : SPD Detect", bg, icol[0], item[0], fg);
	chkrec_item[7] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[7])
		return -1;
	sprintf(chkrec_item[7], "%s", str);

	//-------------------------------------------------------------
	//  RTC : RTC
	//-------------------------------------------------------------
	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_RTC))
	{
		if (rec->sys_flag & PU_REC_SYS_RTC_0A)
		{
			item[0] = 'o';	icol[0] = oc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0A, CHKREC_GP_RTC_0A, CHKREC_OK);
		}
		else
		{
			item[0] = 'x';	icol[0] = xc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0A, CHKREC_GP_RTC_0A, CHKREC_ERR);
		}

		if (rec->sys_flag & PU_REC_SYS_RTC_0B)
		{
			item[1] = 'o';	icol[1] = oc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0B, CHKREC_GP_RTC_0B, CHKREC_OK);
		}
		else
		{
			item[1] = 'x';	icol[1] = xc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0B, CHKREC_GP_RTC_0B, CHKREC_ERR);
		}

		if (rec->sys_flag & PU_REC_SYS_RTC_0D)
		{
			item[2] = 'o';	icol[2] = oc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0D, CHKREC_GP_RTC_0D, CHKREC_OK);
		}
		else
		{
			item[2] = 'x';	icol[2] = xc;
			SET_VAR_GV(m, CHKREC_GM_RTC_0D, CHKREC_GP_RTC_0D, CHKREC_ERR);
		}
	}
	else
	{
		item[0] = item[1] = item[2] = '_';
		icol[0] = icol[1] = icol[2] = fg;
		SET_VAR_GV(m, CHKREC_GM_RTC_0A, CHKREC_GP_RTC_0A, CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_RTC_0B, CHKREC_GP_RTC_0B, CHKREC_NOCHK);
		SET_VAR_GV(m, CHKREC_GM_RTC_0D, CHKREC_GP_RTC_0D, CHKREC_NOCHK);
	}

	// RTC
	sprintf(str, "\b%c\f%c%c\f%c%c\f%c%c\f%c - RTC : RTC Check", bg, icol[0], item[0], icol[1], item[1], icol[2], item[2], fg);
	chkrec_item[8] = malloc(sizeof(uint8_t) * (strlen(str) + 1));
	if (!chkrec_item[8])
		return -1;
	sprintf(chkrec_item[8], "%s", str);

	//-------------------------------------------------------------
	//  TMP
	//-------------------------------------------------------------
	sprintf(str, "%s", "  - - TMP : Temperature");
	chkrec_item[9] = malloc(strlen(str) + 1);
	if (!chkrec_item[9])
		return -1;
	sprintf(chkrec_item[9], "%s", str);

	
	//-------------------------------------------------------------
	//  TSC
	//-------------------------------------------------------------
	sprintf(str, "%s", "  - - TSC : Time Stamp Count");
	chkrec_item[10] = malloc(strlen(str) + 1);
	if (!chkrec_item[10])
		return -1;
	sprintf(chkrec_item[10], "%s", str);

	//-------------------------------------------------------------
	//  INTV
	//-------------------------------------------------------------
	sprintf(str, "%s", "  - -INTV : Time Intervals");
	chkrec_item[11] = malloc(strlen(str) + 1);
	if (!chkrec_item[11])
		return -1;
	sprintf(chkrec_item[11], "%s", str);
	
	*mask = m;

	return 0;
}

//=============================================================================
//  chkrec_show
//=============================================================================
int chkrec_show(int idx)
{
	uint8_t		sel, sel0;
	uint8_t		bk;
	int			iret = 1;
	char		title[64];
	int			i;
	uint16_t	no;
	uint32_t	mask = 0;
	file_t		*ef = NULL;
	uint8_t		res;

	chkrec_vlbox.title	= NULL;
	chkrec_vlbox.item	= chkrec_item;
	chkrec_vlbox.help	= NULL;

	chkrec_vlbox.num_item = 12;
	chkrec_vlbox.num_help = 0;

	chkrec_vlbox.fg = T_BLACK;
	chkrec_vlbox.bg = T_LIGHTGRAY;
	chkrec_vlbox.fs = T_YELLOW;
	chkrec_vlbox.bs = T_BLUE;
	chkrec_vlbox.fh = T_RED;
	chkrec_vlbox.bh = T_LIGHTGRAY;

	chkrec_vlbox.hm = 1;
	chkrec_vlbox.vm = 0;
	chkrec_vlbox.ha = 0;
	chkrec_vlbox.va = 0;
	
	chkrec_vlbox.align	= T_AL_CM;
	chkrec_vlbox.tialign= ((T_AL_CENTER<<4) | (T_AL_LEFT));
	chkrec_vlbox.flag	= ( T_VLBOX_FL_TITLE   |
				T_VLBOX_FL_KEYHELP |
				T_VLBOX_FL_SHADOW  |
				T_VLBOX_FL_FRAME );
	chkrec_vlbox.sel	= 0;

	// maybe POWERUP.ERR not existed
	ef = file_read_to_buf("POWERUP.ERR", &res);

	// no : record no (sorted inc no)
	no = dat->rec_pool[idx].no;
	
	sel0 = 0;
	
	// 0  1   2  3  4  5  6  7  8
	// AB AEC CP E8 SM PC SB SP RTC
	bk = t_get_color();

re_chkrec:

	//sprintf(title, "Check Record : \f%c%d", T_ARG_BLUE, no);
	sprintf(title, "Check Record : \f%c%d", T_ARG_BLUE, no);

	chkrec_vlbox.title = (char *)malloc(strlen(title)+1);
	if (!chkrec_vlbox.title)
		return 0;
	sprintf(chkrec_vlbox.title, "%s", title);
	
	chkrec_vlbox.sel = sel0;

	if (chkrec_update_vlbox_ooxx(no, &mask, chkrec_vlbox.fg, chkrec_vlbox.bg, T_ARG_LIGHTCYAN, T_ARG_LIGHTMAGENTA) < 0)
	{
		free(chkrec_vlbox.title);
		return 0xFF;
	}
	
	// debug
	//sprintf(title, "no = %d, mask = 0x%08X", no, mask);
	//msgbox_waitkey(T_WHITE, T_BROWN, "Debug", title, 4, 1);
	
	
	sel = t_show_vlbox(&chkrec_vlbox);

	free(chkrec_vlbox.title);
	for (i=0; i<12; i++)
	{
		if (chkrec_item[i])
			free(chkrec_item[i]);
	}
	
	if (sel == 0xFA)			// abort : exit
	{
		scr_show_main_keyhlp();	// recover key help
		t_set_color(bk);
		
		return iret;
	}

	switch(sel)
	{
		case 0: chkrec_abios_show(no, mask, ef);		break;
		case 1: chkrec_aec_show(no, mask, ef);			break;
		case 2: chkrec_cpu_show(no, mask, ef);			break;
		case 3: chkrec_e820_show(no, mask, ef);			break;
		case 4: chkrec_smbios_show(no, mask, ef);		break;
		case 5:	chkrec_pci_show(no, mask, ef);			break;
		case 6: chkrec_smbus_show(no, mask, ef);		break;
		case 7: chkrec_spd_show(no, mask, ef);			break;
		case 8: chkrec_rtc_show(no, mask, ef);			break;
		case 9: chkrec_tmp_show(no);					break;
		case 10: chkrec_tsc_show(no);					break;
		case 11: chkrec_intv_show(no);					break;
	}

	// remember last selection
	sel0 = sel;

	goto re_chkrec;

	// recover key help
//	scr_show_main_keyhlp();

//	t_set_color(bk);

//	return iret;
}

//=============================================================================
//  Archives : Linked-List Sort
//=============================================================================
#if 0
int linkedListSort(LINKED_LIST_T list, newCompareFunction fn, void *usr_info)
{
    LINKED_LIST_T tmpptr = list;
    int newitems[N_ITEMS];
    int i=0;

    //Logic to get all the items in the list in a array
    while(tmpptr != NULL)
    {
        newitems[i] = tmpptr->item;
        tmpptr = tmpptr->next;
        i++;
    }
    tmpptr = list;
    i = 0;
    //Sort that array
    //qsort ( list , 100, sizeof (struct LINKED_LIST_S), (fn) );
    qsort ( newitems , 100, sizeof(list->item), (fn) );

    //store the sorted items back into the list.
    while(tmpptr != NULL)
    {
        tmpptr->item = newitems[i];
        tmpptr = tmpptr->next;
        i++;
    }

   return -1;
}

int sort_fn_descend(void *ptr1,void *ptr2)
{   
   int a = *((int*)(ptr1));
   int b = *((int*) (ptr2));

  return a - b;

}


int sort_fn_ascend(const void *ptr1, const void *ptr2)
{
    int a = *((int*)(ptr1));
   int b = *((int*) (ptr2));

  return b - a;

}
#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  SCR : Screen Handler                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>

#ifdef __WATCOMC__
#include <i86.h>
#endif

#include "typedef.h"
#include "t_video.h"
#include "t_title.h"
#include "t_keyhlp.h"
#include "t_bignum.h"
#include "t_msgbox.h"
#include "t_scrcap.h"
#include "t_hlbox.h"
#include "log.h"		// debug

#include "t_indat.h"
#include "smb.h"

#include "abios.h"
#include "key.h"
#include "smbios.h"
#include "bda.h"
#include "fbuf.h"
#include "conv.h"

#include "pu.h"
#include "upd.h"
#include "rt.h"
#include "errcode.h"
#include "parinit.h"
#include "statis.h"
#include "scr.h"
#include "flmenu.h"
#include "sysmenu.h"
#include "mainmenu.h"
#include "chkrec.h"

//=============================================================================
//  scr_show_main_body
//=============================================================================
void scr_show_main_body(void)
{
	t_fill_color_screen(T_LIGHTGRAY, T_BLUE, ' ');
}

//=============================================================================
//  scr_show_title
//=============================================================================
void scr_show_title(void)
{
	t_title_t	title;

	title.sx = 0;			// sx : start_x
	title.sy = 0;			// sy : start_y
	title.fg = T_BLACK;		// fg : foreground color
	if (pu->mode == PU_MODE_ON_LINE)
		title.bg = T_LIGHTGRAY;	// bg : background color
	else
		title.bg = T_CYAN;
	title.tw = 80;			// tw : title width
	title.ax = 2;			// ax : align x, ax if reference of align type
	title.align = T_AL_LEFT;	// align : alignment type : LEFT/CENTER/RIGHT

	// title
	title.name = PU_DES_NAME " " PU_DES_VER " - " PU_DES_DESC;

	t_show_title(&title);
}

//=============================================================================
//  scr_clear_keyhlp
//=============================================================================
void scr_clear_keyhlp(void)
{
	int		i;

	t_color(T_LIGHTGRAY, T_BLUE);

	for (i=0; i<79; i++)
		t_xy_putc(i, 24, 0xDB);
}

//=============================================================================
//  scr_show_hlbox_keyhlp
//=============================================================================
char *hlbox_key[3] = { "ESC",  "\x1b\x1a", "Enter"   };
char *hlbox_name[3]= { "Back", "Select"  , "Confirm" };

void scr_show_hlbox_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = hlbox_key;
	keyhlp.name= hlbox_name;
	keyhlp.num_key = sizeof(hlbox_key)/sizeof(hlbox_key[0]);
	keyhlp.num_name= sizeof(hlbox_name)/sizeof(hlbox_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_hlbox2_keyhlp
//=============================================================================
char *hlbox2_key[3] = { "ESC",  "\x1b\x1a", "Enter"   };
char *hlbox2_name[3]= { "Quit", "Select"  , "Confirm" };

void scr_show_hlbox2_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = hlbox2_key;
	keyhlp.name= hlbox2_name;
	keyhlp.num_key = sizeof(hlbox2_key)/sizeof(hlbox2_key[0]);
	keyhlp.num_name= sizeof(hlbox2_name)/sizeof(hlbox2_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_indat_dec_keyhlp
//=============================================================================
char *indat_dec_key[4] = { "ESC",  "0~9",   "BackSpace", "Enter"   };
char *indat_dec_name[4]= { "Back", "Input", "Delete",    "Confirm" };

void scr_show_indat_dec_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = indat_dec_key;
	keyhlp.name= indat_dec_name;
	keyhlp.num_key = sizeof(indat_dec_key)/sizeof(indat_dec_key[0]);
	keyhlp.num_name= sizeof(indat_dec_name)/sizeof(indat_dec_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_any_key_keyhlp
//=============================================================================
char *any_key_key[1] = { "any key" };
char *any_key_name[1]= { "Continue" };

void scr_show_any_key_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = any_key_key;
	keyhlp.name= any_key_name;
	keyhlp.num_key = sizeof(any_key_key)/sizeof(any_key_key[0]);
	keyhlp.num_name= sizeof(any_key_name)/sizeof(any_key_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_confirm_keyhlp
//=============================================================================
char *confirm_key[1] = { "Enter"   };
char *confirm_name[1]= { "Confirm" };

void scr_show_confirm_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = confirm_key;
	keyhlp.name= confirm_name;
	keyhlp.num_key = sizeof(confirm_key)/sizeof(confirm_key[0]);
	keyhlp.num_name= sizeof(confirm_name)/sizeof(confirm_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_back_confirm_keyhlp
//=============================================================================
char *last_key[2] = { "ESC",  "Enter"   };
char *last_name[2]= { "Back", "Confirm" };

void scr_show_back_confirm_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = last_key;
	keyhlp.name= last_name;
	keyhlp.num_key = sizeof(last_key)/sizeof(last_key[0]);
	keyhlp.num_name= sizeof(last_name)/sizeof(last_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_abort_keyhlp
//=============================================================================
char *abort_key[1] = { "ESC"   };
char *abort_name[1]= { "Abort" };

void scr_show_abort_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = abort_key;
	keyhlp.name= abort_name;
	keyhlp.num_key = sizeof(abort_key)/sizeof(abort_key[0]);
	keyhlp.num_name= sizeof(abort_name)/sizeof(abort_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_pci_keyhlp
//=============================================================================
char *pci_keyhlp_key[5] = { "ESC",  "Del",   "PgUp",     "PgDn", "Enter"   };
char *pci_keyhlp_name[5]= { "Back", "Abort", "Previous", "Next", "Confirm" };

void scr_show_pci_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = pci_keyhlp_key;
	keyhlp.name= pci_keyhlp_name;
	keyhlp.num_key = sizeof(pci_keyhlp_key)/sizeof(pci_keyhlp_key[0]);
	keyhlp.num_name= sizeof(pci_keyhlp_name)/sizeof(pci_keyhlp_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_timer_keyhlp
//=============================================================================
char *timer_key[2] =	 { "ESC",  "Backspace*2" };
char *timer_key_name[2]= { "Back", "Abort"};

void scr_show_timer_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = timer_key;
	keyhlp.name= timer_key_name;
	keyhlp.num_key = sizeof(timer_key)/sizeof(timer_key[0]);
	keyhlp.num_name= sizeof(timer_key_name)/sizeof(timer_key_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_chkbox_keyhlp
//=============================================================================
char *chkbox_key[3] = { "ESC",  "Space", "Enter"   };
char *chkbox_name[3]= { "Back", "Check/Un-check"  , "Confirm" };

void scr_show_chkbox_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = chkbox_key;
	keyhlp.name= chkbox_name;
	keyhlp.num_key = sizeof(chkbox_key)/sizeof(chkbox_key[0]);
	keyhlp.num_name= sizeof(chkbox_name)/sizeof(chkbox_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_show_main_keyhlp
//=============================================================================
char *main_keyhlp_key[5] = { "ESC",  "F1",   "Space",		"\x18\x19",	"Enter" 		};
char *main_keyhlp_name[5]= { "Quit", "Help", "Main Menu",	"Select",	"Check Record"  };

void scr_show_main_keyhlp(void)
{
	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = main_keyhlp_key;
	keyhlp.name= main_keyhlp_name;
	keyhlp.num_key = sizeof(main_keyhlp_key)/sizeof(main_keyhlp_key[0]);
	keyhlp.num_name= sizeof(main_keyhlp_name)/sizeof(main_keyhlp_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  scr_goto_record
//=============================================================================
static uint8_t scr_goto_record(uint16_t *recno)
{
	t_indat_t	indat;
	uint8_t		res;

	indat.title = "Go to Record";
	indat.field	= "Record No.";
	indat.mode	= (T_INDAT_WORD | T_INDAT_UNSIGNED | T_INDAT_DEC);
	indat.unit  = "";
	indat.help  = "";

	indat.fg = T_WHITE;		indat.bg = T_MAGENTA;
	indat.fi = T_YELLOW;	indat.bi = T_BLACK;
	indat.fh = T_YELLOW;	indat.bh = T_MAGENTA;

	indat.align = T_AL_CM;
	indat.shadow = 1;
	indat.data = (uint32_t)*recno;	// default : 5 seconds

	//indat.mode|= T_INDAT_MAX_MIN;
	//indat.max	= 256;
	//indat.min	= 1;

	scr_show_indat_dec_keyhlp();
	
	res = t_show_indat(&indat);
	if (res)
	{
		return res;
	}
	else
	{
		*recno = (uint16_t)indat.data;
		return 0;
	}
}


//=============================================================================
//  scr_show_big_count
//=============================================================================
void scr_show_big_count(uint32_t num)
{
	t_big_num_t		bn;
	pu_rec_t		*rec;

	rec = &dat->rec;

	bn.num	= num;
	bn.x	= 60;
	bn.y	= 2;

	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
	{
		// without RTC battery
		bn.fg = T_LIGHTMAGENTA;
		bn.bg = T_BLUE;
	}
	else
	{
		// with RTC battery
		if (dat->par->rec_cnt < 10)
		{
			bn.fg = T_LIGHTCYAN;
			bn.bg = T_BLUE;
		}
		else
		{
			// QC
			if (rec->intv == 0)
			{	bn.fg = T_LIGHTGRAY;	bn.bg = T_BLUE; }	// na
			else if (rec->intv < 0)
			{	bn.fg = T_YELLOW;		bn.bg = T_BLUE;	}	// er
			else if (rec->intv > dat->par->intv)
			{	bn.fg = T_LIGHTRED;		bn.bg = T_BLUE;	}	// gg
			else
			{	bn.fg = T_LIGHTGREEN;	bn.bg = T_BLUE; }	// ok
		}
	}

	if (pu->mode == PU_MODE_OFF_LINE)
	{
		bn.fg = T_WHITE;
		bn.bg = T_BLUE;
	}
		
	bn.pad0  = T_BIGNUM_PAD0_DIS;
	bn.digit = 5;	// max digits
	bn.align = T_BIGNUM_ALIGN_RIGHT;

	t_show_big_dec(&bn);
	
	//t_show_big_hex(24, 10, num);
	//	t_show_big_hex(&bn);
}

//=============================================================================
//  scr_show_acl_info
//=============================================================================
void scr_show_acl_info(void)
{
	char	msg[96];
	uint8_t	bg;
	int		i, j, len, my;

	pu_raw_t	*raw;

	bg = T_BLUE;

	// region
	t_color(T_WHITE, bg);
	t_fill_region(0, 21, 79, 23, 0x20);

	// vertical seperator
	t_color(T_BLACK, bg);
	for (i=0; i<3; i++)
	{
		t_xy_putc(45, 21+i, 0xb3);
		t_xy_putc(57, 21+i, 0xb3);
	}

	//if (dat->par->tm_zero == 0 || dat->par->rec_cnt == 0)
	//	return;

	if (TST_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_PRE))
	{
		// abios : magic string
		raw = raw_find_id(RAW_ID_ACL_BIOS);
		if (raw)
		{
			// abios magic
			t_color(T_YELLOW, bg);

			len = raw->len;
		
			if (len > 129)
				len = 129;
	
			for (i=0, j=0, my=21; i<len; i++)
			{
				msg[j++] = raw->data[i];
				if (j >= 43)
				{
					msg[43] = 0;
					t_xy_puts(1, my, msg);
					my++;
					j = 0;
				}
			}
			msg[j] = 0;
			t_xy_puts(1, my, msg);
		}

		// abios : model
		raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
		if (raw)
		{
			memcpy(msg, raw->data, raw->len);
			msg[raw->len] = 0;
			t_xy_cl_puts(59, 21, T_YELLOW, bg, msg);
		}
	}

	
#if 0
	// marked : don't show current mother board

	// abios magic
	t_color(T_YELLOW, bg);
	//len = strlen(abios.magic);
	len = abios.len;

	if (len > 129)
		len = 129;
	
	for (i=0, j=0, my=21; i<len; i++)
	{
		msg[j++] = abios.magic[i];
		if (j >= 43)
		{
			msg[43] = 0;
			t_xy_puts(1, my, msg);
			my++;
			j = 0;
		}
	}
	msg[j] = 0;
	t_xy_puts(1, my, msg);
#endif
	
	
	// aec
// marked : don't show current mother board ec info
//	if (TST_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE))
//	{
//		t_xy_cl_puts(47, 21, T_LIGHTCYAN,    bg, pu->aec.chip);
//		t_xy_cl_puts(47, 22, T_LIGHTGREEN,   bg, pu->aec.board);
//		t_xy_cl_puts(47, 23, T_LIGHTMAGENTA, bg, pu->aec.ver);
//	}

	if (TST_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE))
	{
		// aec : chip
		raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
		if (raw)
		{
			memcpy(msg, raw->data, raw->len);
			msg[raw->len] = 0;
			t_xy_cl_puts(47, 21, T_LIGHTCYAN, bg, msg);
		}

		// aec : board
		raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
		if (raw)
		{
			memcpy(msg, raw->data, raw->len);
			msg[raw->len] = 0;
			t_xy_cl_puts(47, 22, T_LIGHTGREEN, bg, msg);
		}

		// aec : ver
		raw = raw_find_id(RAW_ID_ACL_EC_VER);
		if (raw)
		{
			memcpy(msg, raw->data, raw->len);
			msg[raw->len] = 0;
			t_xy_cl_puts(47, 23, T_LIGHTMAGENTA, bg, msg);
		}
	}


	// todo
	//t_xy_cl_puts(58, 21, T_LIGHTBLUE, bg, "0123456789012345678901");
	
	t_xy_cl_puts(59, 23, T_WHITE, bg, pu->file_name);

	/*
	if (pu->mode == PU_MODE_OFF_LINE)
		t_xy_cl_puts(59, 23, T_LIGHTCYAN, bg, "OFFLINE");
	else
		t_xy_cl_puts(59, 23, T_YELLOW, bg, "ONLINE");
	*/
}
	
//=============================================================================
//  scr_show_time_info
//=============================================================================
void scr_show_time_info(void)
{
	char	msg[128];
	char	str[64];

	uint8_t	bg = T_BLUE;


	t_color(T_WHITE, bg);
	t_fill_region(0, 1, 26, 7, 0x20);

	// init tim
	conv_tm_str(&dat->par->tm_init, str);
	sprintf(msg, "Ini : %s", str);
	t_xy_cl_puts(1, 1, T_LIGHTGRAY, bg, msg);

	if (dat->par->tm_zero != 0)
	{
		// zero time
		conv_tm_str(&dat->par->tm_zero, str);
		sprintf(msg, "Sta : %s", str);
		t_xy_cl_puts(1, 2, T_WHITE, bg, msg);
	}

	if (dat->par->rec_cnt != 0)
	{
		// current time
		conv_tm_str(&dat->rec.tm_rtc, str);
		sprintf(msg, "Cur : %s", str);
		t_xy_cl_puts(1, 3, T_WHITE, bg, msg);

		// elapsed time
		conv_tm_elapsed_str(dat->par->tm_zero, dat->rec.tm_rtc, str);
		sprintf(msg, "Ela : %s", str);
		t_xy_cl_puts(1, 4, T_LIGHTCYAN, bg, msg);
	}

	// intervals
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC) == 0)
	{
		if (dat->par->rec_cnt < 10)
			sprintf(msg, "%s", "Int : n/a");
		else
			sprintf(msg, "Int : %d sec", dat->par->intv);
		t_xy_cl_puts(1, 5, T_YELLOW, bg, msg);

		// tolerance
		sprintf(msg, "Tol : %d sec", dat->par->tol);
		t_xy_cl_puts(1, 6, T_LIGHTRED, bg, msg);
	}

}

//=============================================================================
//  scr_show_sta_info
//=============================================================================
void scr_show_sta_info(void)
{
	char	msg[128];
	int		i;

	uint8_t	bg = T_BLUE;

	t_color(T_BLACK, bg);
	
	for (i=0; i<7; i++)
		t_xy_putc(27, 1+i, 0xb3);	// vertical seperator
	
	for (i=0; i<7; i++)
		t_xy_putc(37, 1+i, 0xb3);	// vertical seperator

	for (i=0; i<7; i++)
		t_xy_putc(58, 1+i, 0xb3);	// vertical seperator

	t_color(T_WHITE, bg);	
	t_fill_region(38, 1, 46, 7, 0x20);

	if (dat->par->tm_zero != 0 && dat->par->rec_cnt != 0)
	{
		sprintf(msg, "TO : %5d 100.00%%", dat->par->rec_cnt);
		t_xy_cl_puts(39, 1, T_WHITE, bg, msg);

		if (dat->par->rec_cnt >= 10)
		{
			sprintf(msg, "OK : %5d %6.2f%%", dat->sta.ok, (float)(dat->sta.ok*100)/(float)dat->sta.to);
			t_xy_cl_puts(39, 2, T_LIGHTGREEN, bg, msg);
			
			sprintf(msg, "GG : %5d %6.2f%%", dat->sta.gg, (float)(dat->sta.gg*100)/(float)dat->sta.to);
			t_xy_cl_puts(39, 3, T_LIGHTRED, bg, msg);
			
			sprintf(msg, "ER : %5d %6.2f%%", dat->sta.er, (float)(dat->sta.er*100)/(float)dat->sta.to);
			t_xy_cl_puts(39, 4, T_YELLOW, bg, msg);
			
			sprintf(msg, "NA : %5d %6.2f%%", dat->sta.na, (float)(dat->sta.na*100)/(float)dat->sta.to);
			t_xy_cl_puts(39, 5, T_LIGHTGRAY, bg, msg);
		}

		// check flag
		sprintf(msg, "oo : %5d %6.2f%%", dat->sta.oo, (float)(dat->sta.oo*100)/(float)dat->sta.to);
		t_xy_cl_puts(39, 6, T_LIGHTCYAN, bg, msg);

		sprintf(msg, "xx : %5d %6.2f%%", dat->sta.xx, (float)(dat->sta.xx*100)/(float)dat->sta.to);
		t_xy_cl_puts(39, 7, T_LIGHTMAGENTA, bg, msg);
			
		sprintf(msg, "nx : %5d", dat->sta.nx);
		t_xy_cl_puts( 2, 7, T_LIGHTMAGENTA, bg, msg);
	}
}

//=============================================================================
//  scr_show_par_info
//=============================================================================
void scr_show_par_info(void)
{
	uint8_t		bg = T_BLUE;
	char		msg[16];

	// todo
	//t_xy_cl_puts(49, 1, T_LIGHTBLUE, bg, "012345678");

	// 2 3
	// 8901234567
	//  RTC_BAT
	//  AT  OFF
	//  ALARM
	//  RST_NON
	//  RST_KBC
	//  RST_92
	//  RST_SW
	//  RST_HW
	//  RST_FUL
	//  RST_TMO

	// RTC Battery
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
	{
		t_xy_cl_puts(29, 1, T_DARKGRAY, bg, "RTC_BAT");
	}
	else
	{
		t_xy_cl_puts(29, 1, T_LIGHTCYAN, bg, "RTC_BAT");
	}
	
	// AT/ATX
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
	{
		t_xy_cl_puts(29, 2, T_LIGHTGREEN, bg, "ATX");
	}
	else
	{
		t_xy_cl_puts(29, 2, T_LIGHTGREEN, bg, "AT");
	}

	// ON/OFF
	if (pu->mode == PU_MODE_ON_LINE)
	{
		t_xy_cl_puts(33, 2, T_WHITE, bg, "ON");
	}
	else
	{
		t_xy_cl_puts(33, 2, T_YELLOW, bg, "OFF");
	}

	// ALARM
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW))
	{
		t_xy_cl_puts(29, 3, T_LIGHTRED, bg, "ALM");
	}
	else
	{
		t_xy_cl_puts(29, 3, T_DARKGRAY, bg, "ALM");
	}

	// Reset Type
	switch (dat->par->rst_typ)
	{
		case PU_PAR_RST_NONE:	t_xy_cl_puts(29, 4, T_LIGHTGRAY, bg, "RST_NON");	break;
		case PU_PAR_RST_KBC:	t_xy_cl_puts(29, 4, T_LIGHTCYAN, bg, "RST_KBC");	break;
		case PU_PAR_RST_SOFT92:	t_xy_cl_puts(29, 4, T_LIGHTCYAN, bg, "RST_92");		break;
		case PU_PAR_RST_SOFT:	t_xy_cl_puts(29, 4, T_LIGHTCYAN, bg, "RST_SOF");	break;
		case PU_PAR_RST_HARD:	t_xy_cl_puts(29, 4, T_LIGHTCYAN, bg, "RST_HAR");	break;
		case PU_PAR_RST_FULL:	t_xy_cl_puts(29, 4, T_LIGHTCYAN, bg, "RST_FUL");	break;
	}

	// Reset Timeout
	if (dat->par->rst_typ != PU_PAR_RST_NONE)
	{
		t_xy_cl_puts(29, 5, T_WHITE, bg, "RST_TMO");

		sprintf(msg, "%02d:%02d", dat->par->rst_tmo/60, dat->par->rst_tmo%60);
		t_xy_cl_puts(31, 6, T_WHITE, bg, msg);
	}

	// therm ch
	if ((dat->par->therm & 0xF0) == 0x10)
	{
		// RDC-EC thermal
		// bit[3:2] = therm type
		// bit[1:0] = therm ch
		if ((dat->par->therm & 0x0C) == 0x0C)
			sprintf(msg, "%s", "T_???");
		else if ((dat->par->therm & 0x0C) == 0x04)
			sprintf(msg, "T_SYS%d", (dat->par->therm & 0x3)); 
		else if ((dat->par->therm & 0x0C) == 0)
			sprintf(msg, "T_CPU%d", (dat->par->therm & 0x3)); 
	}
	else
	{
		sprintf(msg, "%s", "T_DTS");
	}
	t_xy_cl_puts(29, 7, T_LIGHTRED, bg, msg);


}

#if 0
//=============================================================================
//  scr_show_raw_cpu
//=============================================================================
void scr_show_raw_cpu(void)
{
	pu_raw_t	*raw;
	char		msg[256];
	char		str0[32];
	int			idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	raw = raw_find_id(RAW_ID_CPU_VENDOR);
	if (raw)
	{
		memcpy(&msg[idx], raw->data, raw->len);
		idx += raw->len;
		msg[idx++] = '\n';
		msg[idx++] = '\n';
	}

	raw = raw_find_id(RAW_ID_CPU_NAME);
	if (raw)
	{
		memcpy(&msg[idx], raw->data, raw->len);
		idx += raw->len;
		msg[idx++] = '\n';
		msg[idx++] = '\n';
	}

	raw = raw_find_id(RAW_ID_CPU_CLOCK);
	if (raw)
	{
		sprintf(str0, "%d MHz", *(uint32_t *)raw->data);
		
		memcpy(&msg[idx], str0, strlen(str0));
		idx += strlen(str0);
		msg[idx++] = '\n';
		msg[idx++] = '\n';
	}

	memcpy(&msg[idx], pu->cpu.vendor, strlen(pu->cpu.vendor));
	idx += strlen(pu->cpu.vendor);
	msg[idx++] = '\n';
	
	memcpy(&msg[idx], pu->cpu.name, strlen(pu->cpu.name));
	idx += strlen(pu->cpu.name);
	msg[idx++] = '\n';

	sprintf(str0, "%d MHz", pu->cpu.clock);
	memcpy(&msg[idx], str0, strlen(str0));
	idx += strlen(str0);
	msg[idx++] = 0;
	
	msgbox_waitkey(T_WHITE, T_RED, "CPU RAW", msg, 4, 1);
}
#endif


//=============================================================================
//  scr_show_raw_abios
//=============================================================================
#if 0
void scr_show_raw_abios(void)
{
	pu_raw_t	*raw;
	char		msg[256], str[32];
	int			i, idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;
	raw = raw_find_id(RAW_ID_ACL_BIOS);
	if (raw)
	{
		
		memcpy(&msg[idx], raw->data, raw->len);
		idx += raw->len;
		for (i=0; i<idx; i++)
		{
			if (msg[i] == 0x20)
				msg[i] = '\n';
		}
		msg[idx++] = '\n';
		msg[idx++] = '\n';
		sprintf(str, "raw->id : %08X\nraw->len : %d", raw->id, raw->len);
	}

	memcpy(&msg[idx], str, strlen(str));
	idx += strlen(str);
	
	msgbox_waitkey(T_WHITE, T_RED, "ABIOS RAW", msg, 4, 1);

}
#endif

//=============================================================================
//  scr_show_raw_aec
//=============================================================================
#if 0
void scr_show_raw_aec(void)
{
	pu_raw_t	*raw;
	char		msg[256];
	char		chip[32], board[32], ver[32];
	int			idx;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
	if (raw)
	{
		memset(chip, 0, sizeof(chip));
		memcpy(chip, raw->data, raw->len);
		sprintf(&msg[idx], "\rEC Chip  : %s\n\rraw->id  : %08X\n\rraw->len : %d\n\n", chip, raw->id, raw->len);
		idx = strlen(msg);
	}

	raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
	if (raw)
	{
		memset(board, 0, sizeof(board));
		memcpy(board, raw->data, raw->len);
		sprintf(&msg[idx], "\rEC Board : %s\n\rraw->id  : %08X\n\rraw->len : %d\n\n", board, raw->id, raw->len);
		idx = strlen(msg);
	}

	raw = raw_find_id(RAW_ID_ACL_EC_VER);
	if (raw)
	{
		memset(ver, 0, sizeof(board));
		memcpy(ver, raw->data, raw->len);
		sprintf(&msg[idx], "\rEC Ver   : %s\n\rraw->id  : %08X\n\rraw->len : %d", ver, raw->id, raw->len);
		idx = strlen(msg);
	}

	msgbox_waitkey(T_WHITE, T_RED, "AEC RAW", msg, 4, 1);
}
#endif

//=============================================================================
//  scr_show_raw_e820
//=============================================================================
#if 0
void scr_show_raw_e820(void)
{
	pu_raw_t	*raw;
	char		msg[256];
	int			idx;
	uint8_t		num;
	uint32_t	kbsz;
	uint32_t	mbsz;

	memset(msg, 0, sizeof(msg));
	
	num = 0;
	kbsz = mbsz = 0;
	idx = 0;

	raw = raw_find_id(RAW_ID_MEM_E820_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(&msg[idx], "\rE820 num : %d\n\rraw->id  : %08X\n\rraw->len : %d\n\n", num, raw->id, raw->len);
		idx = strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_MEM_E820_KSZ);
	if (raw)
	{
		kbsz = *(uint32_t *)raw->data;
		sprintf(&msg[idx], "\rE820 ksz : %d KB\n\rraw->id  : %08X\n\rraw->len : %d\n\n", kbsz, raw->id, raw->len);
		idx = strlen(msg);
	}

	raw = raw_find_id(RAW_ID_MEM_E820_MSZ);
	if (raw)
	{
		mbsz = *(uint32_t *)raw->data;
		sprintf(&msg[idx], "\rE820 msz : %d MB\n\rraw->id  : %08X\n\rraw->len : %d", mbsz, raw->id, raw->len);
		idx = strlen(msg);
	}
	
	msgbox_waitkey(T_WHITE, T_RED, "E820 RAW", msg, 4, 1);
}
#endif

//=============================================================================
//  scr_show_raw_smbios
//=============================================================================
#if 0
void scr_show_raw_smbios(void)
{
	pu_raw_t	*raw;
	char		msg[384];
	int			idx;
	
	smbios_hdr_t	*hdr;
	smbios_v3hdr_t	*v3hdr;

	memset(msg, 0, sizeof(msg));
	
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_HDR);
	if (raw)
	{
		hdr = (smbios_hdr_t *)raw->data;
		sprintf(msg, "\r    Addr : 0x%08X\n\r    Len  : %d = %X\n\r    Ver  : %d.%d\n\rDMI Addr : 0x%08X\n\rDMI Size : %X = %d\n\rDMI Num  : %d",
				hdr, hdr->len, hdr->len,
				hdr->major_ver,hdr->minor_ver,
				hdr->smbios_addr,
				hdr->smbios_len, hdr->smbios_len,
				hdr->smbios_ent_num);
		idx = strlen(msg);
	}
	
	raw = raw_find_id(RAW_ID_DMI_SMBIOS_V3HDR);
	if (raw)
	{
		v3hdr = (smbios_v3hdr_t *)raw->data;
		
		sprintf(&msg[idx], "\n****** SMBIOS V3 ******\n\r    Addr : %08X\n\r    Len  : %d = 0x%X\n\r    Ver  : %d.%d\n\n\rDMI Addr : 0x%08X\n\rDMI Size : %d\n\rDoc Rev  : %d\n\rEPS Rev  : %d",
				v3hdr, v3hdr->len, v3hdr->len,
				v3hdr->major_ver, v3hdr->minor_ver,
				v3hdr->tab_addr,
				v3hdr->tab_sz,
				v3hdr->doc_rev,
				v3hdr->eps_rev);
	}

	msgbox_waitkey(T_WHITE, T_RED, "SMBIOS RAW", msg, 4, 1);
}
#endif

//=============================================================================
//  scr_show_raw_smbus
//=============================================================================
#if 0
void scr_show_raw_smbus(void)
{
	pu_raw_t	*raw;
	char		msg[256];
	int			i, idx;
	uint8_t		num, *ptr;

	memset(msg, 0, sizeof(msg));
	idx = 0;

	raw = raw_find_id(RAW_ID_SMB_DEV_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
		sprintf(&msg[idx], "\rNum of Dev : %d\n", num);
		idx = strlen(msg);
	}

	raw = raw_find_id(RAW_ID_SMB_DEV_LST);
	if (raw)
	{
		sprintf(&msg[idx], "%s", "\rDev List : ");
		idx = strlen(msg);
		ptr = raw->data;
	
		for (i=0; i<num; i++)
		{
			sprintf(&msg[idx], "%02X ", *ptr++);
			idx = strlen(msg);
		}
		msg[idx++] = '\n';
	}

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

	msgbox_waitkey(T_WHITE, T_RED, "SMBUS RAW", msg, 4, 1);
}
#endif

//=============================================================================
//  scr_show_time
//=============================================================================
void scr_show_time(void)
{
	struct tm	*utc;
	char		msg[16];
	uint8_t		bg;

	utc = localtime((const time_t*)&pu->tm.curr);

	// 5         6         7
	// 012345678901234567890123456789
	//        2017/10/21 SAT 18:06:24

	if (pu->mode == PU_MODE_ON_LINE)
		bg = T_LIGHTGRAY;
	else
		bg = T_CYAN;
	
	// now time
	t_color(T_BLACK, bg);
	sprintf(msg, "%4d/%02d/%02d", utc->tm_year+1900, utc->tm_mon+1, utc->tm_mday);
	t_xy_puts(57, 0, msg);

	sprintf(msg, "%2d:%02d:%02d", utc->tm_hour, utc->tm_min, utc->tm_sec);
	t_xy_puts(72, 0, msg);

	if (utc->tm_wday == 0 || utc->tm_wday == 6)
		t_fg_color(T_RED);
	else
		t_fg_color(T_BLACK);
	t_xy_puts(68, 0, (char *)week_str[utc->tm_wday]);

	// don't show elapsed time and reset timer at off-line mode
	if (pu->mode == PU_MODE_OFF_LINE)
		return;

	// elapsed time
	t_color(T_WHITE, T_BLUE);
	if (pu->tm.tmr < 3600)
	{
		sprintf(msg, "%02d:%02d", pu->tm.tmr/60, pu->tm.tmr%60);
		t_xy_puts(75, 1, msg);
	}
	else
	{
		sprintf(msg, "%02d:%02d:%02d", pu->tm.tmr/3600, (pu->tm.tmr/60)%60, pu->tm.tmr%60);
		t_xy_puts(72, 1, msg);
	}

	// reset timer
	if (dat->par->rst_typ != PU_PAR_RST_NONE)
	{
		sprintf(msg, "%02d:%02d", pu->tm.rst_tmr/60, pu->tm.rst_tmr%60);
		t_xy_puts(75, 23, msg);
	}	
	
}

//=============================================================================
//  scr_show_rec_field
//=============================================================================
void scr_show_rec_field(void)
{
	uint8_t		bg;

	if (pu->mode == PU_MODE_ON_LINE)
	{
		bg = T_LIGHTGRAY;
		t_color(T_BLACK, bg);
	}
	else
	{
		bg = T_CYAN;
		t_color(T_BLACK, bg);
	}

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//     No RTC                 AB AEC CP E8 SM PC SB SP TMP           TSC   INTV QC
	//       ^                                                ^             ^      ^

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//     No RTC                 AB AEC CP E8 SM PC SB SP RTC TMP       TSC   INTV QC
	//       ^                                                    ^         ^      ^

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- --- -- -- -- -- -- -- --- ------ ------ ------ -- 
	//     No Date       Time     AB AEC CP E8 SM PC SB SP RTC TMP       TSC   INTV QC
	//      1 2017/11/01 15:23:56 oo ooo oo  o  o  o  x  o ooo  65    133.49 -99999  O

	t_put_hline(0, 8, 80);
	t_xy_puts(4, 8, "No Date       Time     AB AEC CP E8 SM PC SB SP RTC TMP       TSC   INTV QC");

	switch(dat->par->sort_typ)
	{
		case PU_PAR_SORT_NO_INC:	t_xy_cl_puts( 4, 8, T_RED, bg, "No\x1e");	break;
		case PU_PAR_SORT_NO_DEC:	t_xy_cl_puts( 4, 8, T_RED, bg, "No\x1f");	break;
		case PU_PAR_SORT_INTV_INC:	t_xy_cl_puts(72, 8, T_RED, bg, "INTV\x1e");	break;
		case PU_PAR_SORT_INTV_DEC:	t_xy_cl_puts(72, 8, T_RED, bg, "INTV\x1f");	break;
		case PU_PAR_SORT_TEMP_INC:	t_xy_cl_puts(56, 8, T_RED, bg, "TMP\x1e");	break;
		case PU_PAR_SORT_TEMP_DEC:	t_xy_cl_puts(56, 8, T_RED, bg, "TMP\x1f");	break;
		case PU_PAR_SORT_TSC_INC:	t_xy_cl_puts(66, 8, T_RED, bg, "TSC\x1e");	break;
		case PU_PAR_SORT_TSC_DEC:	t_xy_cl_puts(66, 8, T_RED, bg, "TSC\x1f");	break;
	}
}

//=============================================================================
//  scr_show_rec_info
//=============================================================================
void scr_show_rec_info(int idx, int cur)
{
	uint8_t		fg, bg;
	char		str[32];
	int			i, j, mark_len = 79;
	time_t		tm0;
	int32_t		offtm;
	uint8_t		col = 0;
	uint8_t		ox;

	pu_rec_t	*rec;
	
	bg = T_BLACK;
	
	t_color(T_LIGHTGRAY, bg);
	t_fill_region(0, 8, 79, 20, 0x20);

	// CV:CPU Vendor, CN:CPU Name, E8:E820, SM:SMBIOS
	// SB:SMBus, SP:SPD, PCI BusTMP:Temperature
	// TSC:boot tsc, Time
	// ACL : ABIOS present, model
	//       AEC present, board, ver
	// CPU : CPU vendor, CPU name

	// field name
	if (pu->mode == PU_MODE_ON_LINE)
		t_color(T_BLACK, T_LIGHTGRAY);
	else
		t_color(T_BLACK, T_CYAN);

	scr_show_rec_field();

	//t_put_hline(0, 8, 80);
	//t_xy_puts(4, 8, "No RTC                 AB AEC CP E8 SM PC SB SP TMP           TSC   INTV QC");

	if (dat->par->tm_zero == 0 || dat->hdr->rec_num == 0)
		return;

	t_color(T_WHITE, bg);

	// 1st record
	if (idx == 0)
	{
		tm0 = dat->par->tm_zero;
	}
	else
	{
		rec = (pu_rec_t *)&dat->rec_pool[(dat->par->rec_cnt-1)];
		tm0 = rec->tm_rtc;
	}

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- ----- --- -- -- --- -- -- ------- --- ------ ---- -- 
	//  No    RTC                 ACL   CPU E8 SM PCI SB SP CLK     TMP TSC    INTV QC
	//  1     2017/10/17 15:23:56 OOOOO  OO  O  O   O  X  O 3453.23  65 133.49 -999  O

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- -- -- -- -- -- -- -- ------- --- ------ ------ -- 
	//     No RTC                 AB AE CP E8 SM PC SB SP CLK     TMP    TSC   INTV QC
	//      1 2017/10/19 15:23:56 OO OO OO  O  O  O  X  O 3453.23  65 133.49 -99999  O

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- -- -- -- -- -- -- -- ------- --- ------ ------ -- 
	//     No RTC                 AB AE CP E8 SM PC SB SP TMP            TSC   INTV QC
	//      1 2017/11/01 15:23:56 oo oo oo  o  o  o  x  o  65         133.49 -99999  O

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- --- -- -- -- -- -- -- ------ --- ------ ------ -- 
	//     No RTC                 AB AEC CP E8 SM PC SB SP TMP           TSC   INTV QC
	//      1 2017/11/01 15:23:56 oo ooo oo  o  o  o  x  o  65        133.49 -99999  O

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- --- -- -- -- -- -- -- --- ------ ------ ------ -- 
	//     No RTC                 AB AEC CP E8 SM PC SB SP RTC TMP       TSC   INTV QC
	//      1 2017/11/01 15:23:56 oo ooo oo  o  o  o  x  o ooo  65    133.49 -99999  O

	// 0         1         2         3         4         5         6         7         
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	//  ----- ------------------- -- --- -- -- -- -- -- -- --- ------ ------ ------ -- 
	//     No Date       Time     AB AEC CP E8 SM PC SB SP RTC TMP       TSC   INTV QC
	//      1 2017/11/01 15:23:56 oo ooo oo  o  o  o  x  o ooo  65    133.49 -99999  O

	// the last record
	if (pu->alm.cmd & 0x80)
	{
		if (dat->par->rec_cnt >= 10)
		{
			//rec = (pu_rec_t *)&dat->rec_pool[0];
			//qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_no_inc);

			rec = (pu_rec_t *)&dat->rec_pool[(dat->par->rec_cnt-1)];
			if (rec->intv <= 0 || rec->intv > dat->par->intv)
				pu->alm.cmd |= 0x1;	// alarm
		}
	}

	//msgbox_progress(T_WHITE, T_MAGENTA, "POWERUP", "Loading data.....", 4, 1);//efi ver delete
	
	// sort
	if (pu->sort_typ != dat->par->sort_typ)
	{
		rec = (pu_rec_t *)&dat->rec_pool[0];

		switch(dat->par->sort_typ)
		{
		case PU_PAR_SORT_NO_INC: 	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_no_inc);		break;
		case PU_PAR_SORT_NO_DEC: 	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_no_dec);		break;
		case PU_PAR_SORT_INTV_INC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_intv_inc);		break;
		case PU_PAR_SORT_INTV_DEC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_intv_dec);		break;
		case PU_PAR_SORT_TEMP_INC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_cpu_temp_inc);	break;
		case PU_PAR_SORT_TEMP_DEC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_cpu_temp_dec);	break;
		case PU_PAR_SORT_TSC_INC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_cpu_tsc_inc);	break;
		case PU_PAR_SORT_TSC_DEC:	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_cpu_tsc_dec);	break;
		}

		pu->sort_typ = dat->par->sort_typ;
	}
	msgbox_progress_done();

	rec = (pu_rec_t *)&dat->rec_pool[idx];

	for (i=idx, j=9; i<(idx+12); i++, j++)
	{
		if(i == (idx + cur))
		{
			// cursor
			if (pu->mode == PU_MODE_OFF_LINE)
			{
				bg = T_LIGHTGRAY;	// bg
			}
			else
			{
				bg = T_CYAN;			// bg
			}
			t_mark_hline(0, 9+cur, mark_len, bg);	// bg
			t_pen_hline (7, 9+cur, 19, T_DARKGRAY);			// fg : DATE/TIME
		}
		else
		{
			bg = T_BLACK;
			t_bg_color(bg);
		}
		
		// No : ....1
		sprintf(str, "%5d", rec->no);
		t_xy_puts(1, j, str);

		// RTC : yyyy/mm/dd hh:mm:ss
		if(i == (idx + cur))
		{
			fg = T_DARKGRAY;			// fg : DATE/TIME
		}
		else
		{
			fg = T_LIGHTGRAY;
		}
		conv_tm_str(&rec->tm_rtc, str);
		t_xy_cl_puts(7, j, fg, bg, str);
		t_color(T_WHITE, bg);

		// AB : ABIOS
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AB))
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS))
			{
				// AB : ABIOS : model
				if (rec->acl_flag & PU_REC_ACL_ABIOS_MODEL)
				{	ox = 'o';	t_xy_cl_putc(27, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(27, j, T_LIGHTMAGENTA, bg, ox);	}

				// AB : ABIOS : ver
				if (rec->acl_flag & PU_REC_ACL_ABIOS_VER)
				{	ox = 'o';	t_xy_cl_putc(28, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(28, j, T_LIGHTMAGENTA, bg, ox);	}
			}
			else
			{
				ox = '-';
				t_xy_cl_putc(27, j, T_DARKGRAY, bg, ox);
				t_xy_cl_putc(28, j, T_DARKGRAY, bg, ox);
			}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(27, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(28, j, T_DARKGRAY, bg, ox);
		}
		
		// AE : AEC
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC))
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC))
			{
				// AE : AEC : chip
				if (rec->acl_flag & PU_REC_ACL_AEC_CHIP)
				{	ox = 'o';	t_xy_cl_putc(30, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(30, j, T_LIGHTMAGENTA, bg, ox);	}

				// AE : AEC : board
				if (rec->acl_flag & PU_REC_ACL_AEC_BOARD)
				{	ox = 'o';	t_xy_cl_putc(31, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(31, j, T_LIGHTMAGENTA, bg, ox);	}

				// AE : AEC : ver
				if (rec->acl_flag & PU_REC_ACL_AEC_VER)
				{	ox = 'o';	t_xy_cl_putc(32, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(32, j, T_LIGHTMAGENTA, bg, ox);	}
			}
			else
			{
				ox = '-';
				t_xy_cl_putc(30, j, T_DARKGRAY, bg, ox);
				t_xy_cl_putc(31, j, T_DARKGRAY, bg, ox);	
				t_xy_cl_putc(32, j, T_DARKGRAY, bg, ox);	
			}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(30, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(31, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(32, j, T_DARKGRAY, bg, ox);
		}

		// CP : CPU
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_CP))
		{
			// CP : CPU vendor
			if (rec->sys_flag & PU_REC_SYS_CPU_VENDOR)
			{	ox = 'o';	t_xy_cl_putc(34, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(34, j, T_LIGHTMAGENTA, bg, ox);	}

			// CP : CPU name
			if (rec->sys_flag & PU_REC_SYS_CPU_NAME)
			{	ox = 'o';	t_xy_cl_putc(35, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(35, j, T_LIGHTMAGENTA, bg, ox);	}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(34, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(35, j, T_DARKGRAY, bg, ox);
		}

		// E8 : E820
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_E8))
		{
			if (rec->sys_flag & PU_REC_SYS_E820)
			{	ox = 'o';	t_xy_cl_putc(38, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(38, j, T_LIGHTMAGENTA, bg, ox);	}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(38, j, T_DARKGRAY, bg, ox);
		}
	
		// SM : SMBIOS
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SM))
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS))
			{
				// SMBIOS present
				if (rec->sys_flag & PU_REC_SYS_SMBIOS)
				{	ox = 'o';	t_xy_cl_putc(41, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(41, j, T_LIGHTMAGENTA, bg, ox);	}
			}
			else
			{
				// SMBIOS not present
				ox = '-';
				t_xy_cl_putc(41, j, T_DARKGRAY, bg, ox);
			}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(41, j, T_DARKGRAY, bg, ox);
		}
	
		// PC : PCI
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_PC))
		{
			if (rec->sys_flag & PU_REC_SYS_PCI_SCAN)
			{	ox = 'o';	t_xy_cl_putc(44, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(44, j, T_LIGHTMAGENTA, bg, ox);	}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(44, j, T_DARKGRAY, bg, ox);
		}
	
		// SB : SMBUS
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SB))
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB))
			{
				// SMBUS present
				if (rec->sys_flag & PU_REC_SYS_SMB_SCAN)
				{	ox = 'o';	t_xy_cl_putc(47, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(47, j, T_LIGHTMAGENTA, bg, ox);	}
			}
			else
			{
				// SMBUS not present
				ox = '-';
				t_xy_cl_putc(47, j, T_DARKGRAY, bg, ox);
			}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(47, j, T_DARKGRAY, bg, ox);
		}

		// SP : SPD
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SP))
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD))
			{
				// SPD present
				if (rec->sys_flag & PU_REC_SYS_SMB_SPD)
				{	ox = 'o';	t_xy_cl_putc(50, j, T_LIGHTCYAN, bg, ox);		}
				else
				{	ox = 'x';	t_xy_cl_putc(50, j, T_LIGHTMAGENTA, bg, ox);	}
			}
			else
			{
				// SPD not present
				ox = '-';
				t_xy_cl_putc(50, j, T_DARKGRAY, bg, ox);
			}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(50, j, T_DARKGRAY, bg, ox);
		}

		// RTC : RTC
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_RTC))
		{
			if (rec->sys_flag & PU_REC_SYS_RTC_0A)
			{	ox = 'o';	t_xy_cl_putc(52, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(52, j, T_LIGHTMAGENTA, bg, ox);	}

			if (rec->sys_flag & PU_REC_SYS_RTC_0B)
			{	ox = 'o';	t_xy_cl_putc(53, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(53, j, T_LIGHTMAGENTA, bg, ox);	}

			if (rec->sys_flag & PU_REC_SYS_RTC_0D)
			{	ox = 'o';	t_xy_cl_putc(54, j, T_LIGHTCYAN, bg, ox);		}
			else
			{	ox = 'x';	t_xy_cl_putc(54, j, T_LIGHTMAGENTA, bg, ox);	}
		}
		else
		{
			ox = '_';
			t_xy_cl_putc(52, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(53, j, T_DARKGRAY, bg, ox);
			t_xy_cl_putc(54, j, T_DARKGRAY, bg, ox);
		}
		
		// CLK
		// clock is not accuracy, remove it avoid confusing
		//sprintf(str, "%.2f", rec->cpu_clk);
		//t_xy_cl_puts(51, j, T_LIGHTGRAY, bg, str);
		//t_color(T_WHITE, bg);

		// TMP
		//if (TST_BIT_MASK(rec->rt_flag, PU_REC_RT_CPU_INTEL))
		if ((dat->par->therm & 0xF0) == 0x00)
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_INTEL))
			{
				if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_NO_TEMP)==0)
				{
					if (rec->cpu_temp & 0x80)
					{
						sprintf(str, "-%2d", 0xFF - rec->cpu_temp + 1);
						col = T_LIGHTBLUE;
					}
					else
					{
						sprintf(str, "%3d", rec->cpu_temp);
						if(i != (idx + cur))
						{
							bg = T_BLACK;
						}
						if (rec->cpu_temp < 30)
							col = T_LIGHTBLUE;		//   ~30
						else if (rec->cpu_temp < 40)
							col = T_LIGHTCYAN;		// 30~39
						else if (rec->cpu_temp < 50)
							col = T_LIGHTGREEN;		// 40~49
						else if (rec->cpu_temp < 60)
							col = T_YELLOW;			// 50~59
						else if (rec->cpu_temp < 70)
							col = T_LIGHTMAGENTA;	// 60~69
						else if (rec->cpu_temp < 80)
							col = T_LIGHTRED;		// 70~79
						else
						{
							col = T_WHITE;			// 80~
							bg = T_RED;
						}
					}
				}
				else
				{
					col = T_DARKGRAY;
					sprintf(str, "%s", "---");
				}
			}
			else
			{
				col = T_DARKGRAY;
				sprintf(str, "%s", "___");
			}
		}
		else if ((dat->par->therm & 0xF0) == 0x10)
		{
			// EC temperature
			if (rec->cpu_temp & 0x80)
			{
				sprintf(str, "-%2d", 0xFF - rec->cpu_temp + 1);
				col = T_LIGHTBLUE;
			}
			else
			{
				sprintf(str, "%3d", rec->cpu_temp);
				bg = T_BLACK;
				if (rec->cpu_temp < 30)
					col = T_LIGHTBLUE;		//   ~30
				else if (rec->cpu_temp < 40)
					col = T_LIGHTCYAN;		// 30~39
				else if (rec->cpu_temp < 50)
					col = T_LIGHTGREEN;		// 40~49
				else if (rec->cpu_temp < 60)
					col = T_YELLOW;			// 50~59
				else if (rec->cpu_temp < 70)
					col = T_LIGHTMAGENTA;	// 60~69
				else if (rec->cpu_temp < 80)
					col = T_LIGHTRED;		// 70~79
				else
				{
					col = T_WHITE;			// 80~
					bg = T_RED;
				}
			}
		}
		
		t_xy_cl_puts(56, j, col, bg, str);	// temperature

		//bg = T_BLACK;
		t_color(T_WHITE, bg);
			
		// TSC
		if (rec->cpu_tsc < 1000.0)
		{
			sprintf(str, "%6.2f", rec->cpu_tsc);
		}
		else if (rec->cpu_tsc < 10000.0)
		{
			sprintf(str, "%6.1f", rec->cpu_tsc);
		}
		else if (rec->cpu_tsc < 100000.0)
		{
			sprintf(str, "%6d", (int)rec->cpu_tsc);
		}
		else
		{
			sprintf(str, "%s", ">99999");
		}
		t_xy_puts(63, j, str);

		// INTV
		offtm = (int32_t)rec->intv;
		
		if (offtm > 99999)
		{
			sprintf(str, "%s", ">99999");
		}
		else if (offtm < -9999)
		{
			sprintf(str, "%s", "<-9999");
		}
		else if (offtm < 0)
		{
			sprintf(str, "%6d", offtm);
		}
		else
		{
			sprintf(str, "%6d", offtm);
		}
		t_xy_puts(70, j, str);

		// QC
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		{
			t_xy_cl_puts(77, j, T_LIGHTMAGENTA, bg, "--");	// 
		}
		else
		{
			if (dat->par->rec_cnt < 10)
			{
				t_xy_putc(78, j, '?');
			}
			else
			{
			
				// pass : O : offtm <= intv
				// fail : X : offtm >  intv
				// err  : E : offtm < 0
				// na   : N : offtm = 0
				if (offtm == 0)
					t_xy_cl_puts(77, j, T_LIGHTGRAY, bg, "NA");	// n/a
				else if (offtm < 0)
					t_xy_cl_puts(77, j, T_YELLOW, bg, "ER");// error
				else if (offtm > dat->par->intv)
					t_xy_cl_puts(77, j, T_LIGHTRED, bg, "GG");	// fail
				else
					t_xy_cl_puts(77, j, T_LIGHTGREEN, bg, "OK");	// OK
			}
		}
		if (dat->par->rec_cnt > 12)
			mark_len = 79;	// with scroll bar
		else
			mark_len = 80;	// without scroll bar

		// cursor
		//if (pu->mode == PU_MODE_OFF_LINE)
		//	t_mark_hline(0, 9+cur, mark_len, T_LIGHTGRAY);	// bg
		//else
		//	t_mark_hline(0, 9+cur, mark_len, T_CYAN);			// bg

		//t_pen_hline (7, 9+cur, 19, T_DARKGRAY);			// fg : DATE/TIME
		
			

		t_color(T_WHITE, bg);
	
		if ((i+1) >= dat->par->rec_cnt)
			break;

		rec++;	// next record
	}
	
}

//=============================================================================
//  scr
//=============================================================================
void scr_alarm_task(void)
{
	// according to Taiwan government google : siren
	// 
	
	// vehicle     lo_freq  hi_freq    lo_hold    hi_hold
	//---------------------------------------------------------
	// fire truck  650~750  1450~1550  1500(l>h)  3500 (h>l)
	// ambulance   650~750   900~1000   400        600
	// police      650~750  1450~1550   230(l>h)   100 (h>l)
	// engineer    650~750   900~1000   800        200
	// emergency   650~750  1450~1550  1500(l>h)  3500 (h>l) alarm
	//---------------------------------------------------------
	// (unit)         Hz       Hz       ms         ms
	
	
	if (pu->alm.cmd == 0x81)
	{
		sound(dat->par->alm_lo);
		pu->alm.tmo = bda_get_timer() + 7;	// lo_freq hold time : 400ms

		if (pu->alm.tmo >= BDA_TIMER_MAX)
		{
			// 23:59:59 --> 0:00:00
			delay(1000);
			pu->alm.tmo = bda_get_timer() + 7;	// lo_freq hold time : 400ms
		}
			
		pu->alm.cmd = 0x82;
	}
	else if (pu->alm.cmd == 0x82)
	{
		if (bda_get_timer() > pu->alm.tmo)
		{
			sound(dat->par->alm_hi);
			pu->alm.tmo = bda_get_timer() + 11;	// hi_freq hold time : 600ms
			pu->alm.cmd = 0x83;
		}
	}
	else if (pu->alm.cmd == 0x83)
	{
		if (bda_get_timer() > pu->alm.tmo)
		{
			nosound();
			sound(dat->par->alm_lo);
			pu->alm.tmo = bda_get_timer() + 7;	// lo_freq hold time : 400ms
			//pu->alm.cmd = 0;
			pu->alm.cmd = 0x84;
		}
	}
	else if (pu->alm.cmd == 0x84)
	{
		if (bda_get_timer() > pu->alm.tmo)
		{
			sound(dat->par->alm_hi);
			pu->alm.tmo = bda_get_timer() + 11;	// hi_freq hold time : 600ms
			pu->alm.cmd = 0x85;
		}
	}
	else if (pu->alm.cmd == 0x85)
	{
		if (bda_get_timer() > pu->alm.tmo)
		{
			nosound();
			pu->alm.cmd = 0;
		}
	}
}

//=============================================================================
//  scr_show_big_count_temp
//=============================================================================
void scr_show_big_count_temp(uint8_t inv)
{
	// pu->atatx_tmo
	// = 0 : stop
	// = 1 : start
	if (inv)
	{
		if (pu->atatx_tmo == 1)
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
			{
				// ATX : show AT count
				scr_show_big_count(dat->par->rec_cnt);
			}
			else
			{
				// AT : show ATX count
				scr_show_big_count(dat->par->rec_cnt*2);
			}

			pu->atatx_tmo = 2;	// recover after 3 seconds
		}
	}
	else
	{
		if (pu->atatx_tmo > 1)
		{
			pu->atatx_tmo--;
		}
		else
		{
			// tmo
			if (pu->atatx_tmo == 1)
			{
				if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
					scr_show_big_count(dat->par->rec_cnt*2);	// ATX
				else
					scr_show_big_count(dat->par->rec_cnt);		// AT

				pu->atatx_tmo = 0;
			}
		}
	}
}
//=============================================================================
//  scr_find_rec_qc
//=============================================================================
void scr_find_rec_qc(uint8_t qc_type)
{
	int			i;
	int			idx;
	uint8_t		found;
	uint8_t		round;

	pu_rec_t	*rec;

	found = 0;
	round = 0;
	
	// gg : fail
	if (qc_type == PU_REC_QC_GG)
	{

round_fail:
		idx = pu->find.fail;
		rec = (pu_rec_t *)&dat->rec_pool[idx];
	
		for (i=idx; i<dat->par->rec_cnt; i++)
		{
			if (rec->intv > dat->par->intv)
			{
				pu->find.fail = i;
				found = 1;
				break;
			}
			rec++;
		}
		
		if (found == 0)
		{
			// not found : round 1st rec
			if (round == 0)
			{
				//round = 1;
				pu->find.fail = 0;
				goto round_fail;
			}
		}
	}
	// er : error
	else if (qc_type == PU_REC_QC_ER)
	{

round_error:
		idx = pu->find.error;
		rec = (pu_rec_t *)&dat->rec_pool[idx];
	
		for (i=idx; i<dat->par->rec_cnt; i++)
		{
			if (rec->intv < 0)
			{
				pu->find.error = i;
				found = 1;
				break;
			}
			rec++;
		}
		
		if (found == 0)
		{
			// not found : round 1st rec
			if (round == 0)
			{
				//round = 1;
				pu->find.error = 0;
				goto round_error;
			}
		}
	}
	// na : n/a
	else if (qc_type == PU_REC_QC_NA)
	{

round_na:
		idx = pu->find.na;
		rec = (pu_rec_t *)&dat->rec_pool[idx];
	
		for (i=idx; i<dat->par->rec_cnt; i++)
		{
			if (rec->intv == 0)
			{
				pu->find.na = i;
				found = 1;
				break;
			}
			rec++;
		}
		
		if (found == 0)
		{
			// not found : round 1st rec
			if (round == 0)
			{
				//round = 1;
				pu->find.na = 0;
				goto round_na;
			}
		}
	}
}

//=============================================================================
//  scr_find_rec_ooxx
//=============================================================================
int scr_find_rec_ooxx(int st_idx)
{
	int			i;
	int			idx;
	uint8_t		found;
	uint8_t		amask;
	uint16_t	smask;
	pu_rec_t	*rec;

ooxx_search_again:

	found = 0;

	if (pu->find.ooxx == 0)
	{
		idx = 0;
	}
	else
	{
		// start from next record
		idx = pu->find.ooxx + 1;
		
		if (idx >= dat->par->rec_cnt)
			idx = 0;
	}

	for (i=idx; i<dat->par->rec_cnt; i++)
	{
		rec = (pu_rec_t *)&dat->rec_pool[i];

		amask = 0;
		if (dat->par->cmp & PU_PAR_CMP_AB)
		{
			if (dat->par->feat & PU_PAR_FEAT_ABIOS)
			{
				if ((rec->acl_flag & PU_REC_ACL_ABIOS_MODEL) == 0)
					amask |= PU_REC_ACL_ABIOS_MODEL;	// err
				
				if ((rec->acl_flag & PU_REC_ACL_ABIOS_VER) == 0)
					amask |= PU_REC_ACL_ABIOS_VER;		// err
			}
		}

		if (dat->par->cmp & PU_PAR_CMP_AEC)
		{
			if (dat->par->feat & PU_PAR_FEAT_AEC)
			{
				if ((rec->acl_flag & PU_REC_ACL_AEC_CHIP) == 0)
					amask |= PU_REC_ACL_AEC_CHIP;	// err
					
				if ((rec->acl_flag & PU_REC_ACL_AEC_BOARD) == 0)
					amask |= PU_REC_ACL_AEC_BOARD;	// err

				if ((rec->acl_flag & PU_REC_ACL_AEC_VER) == 0)
					amask |= PU_REC_ACL_AEC_VER;	// err
			}
		}

		smask = 0;
		if (dat->par->cmp & PU_PAR_CMP_CP)
			smask |= (PU_REC_SYS_CPU_VENDOR | PU_REC_SYS_CPU_NAME);

		if (dat->par->cmp & PU_PAR_CMP_E8)
			smask |= (PU_REC_SYS_E820);

		if ((dat->par->cmp & PU_PAR_CMP_SM) &&
			(dat->par->feat & PU_PAR_FEAT_SMBIOS))
			smask |= (PU_REC_SYS_SMBIOS);

		if (dat->par->cmp & PU_PAR_CMP_PC)
			smask |= (PU_REC_SYS_PCI_SCAN);

		if ((dat->par->cmp & PU_PAR_CMP_SB) &&
			(dat->par->feat & PU_PAR_FEAT_SMB))
			smask |= (PU_REC_SYS_SMB_SCAN);

		if ((dat->par->cmp & PU_PAR_CMP_SP) &&
			(dat->par->feat & PU_PAR_FEAT_SPD))
			smask |= (PU_REC_SYS_SMB_SPD);

		if (dat->par->cmp & PU_PAR_CMP_RTC)
			smask |= (PU_REC_SYS_RTC_0A | PU_REC_SYS_RTC_0B | PU_REC_SYS_RTC_0D);

		if ((amask != 0x00) || ((rec->sys_flag & smask) != smask))
		{
			pu->find.ooxx = i;
			found = 1;
			break;
		}
	}



	if (found == 0)
	{
		// not found
		if (pu->find.ooxx == 0)
		{
			msgbox_waitkey(T_WHITE, T_RED, "Error Flag", "Error flag not found!", 4, 1);
			return -1;
		}
		else
		{
			pu->find.ooxx = 0;
			// search again
			goto ooxx_search_again;
		}
	}
	else
	{
		// found
		if (dat->par->rec_cnt <= 12)
		{
			// 1st page
			pu->find.ooxx = 0;
		}
		else if (pu->find.ooxx >= (dat->par->rec_cnt - 12))
		{
			// last page
			//pu->find.ooxx = dat->par->rec_cnt - 12;
			//msgbox_waitkey(T_BLACK, T_LIGHTGRAY, "Search Error Check Flags", "The end of records, please go to 1st record and search again!", 4, 1);
		}
	}

	return 0;
}

//=============================================================================
//  scr_quit_main
//=============================================================================
static uint8_t scr_quit_main(void)
{
	char *hb_item[2] = { "Yes",  "No" };
	char *hb_help[2] = { "", "" };

	t_hlbox_t	hb;

	hb.title	= "Exit program ?";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = 0;

	hb.fg = T_WHITE;	hb.bg = T_RED;
	hb.fs = T_RED;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align = T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 1;		// default : No

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  scr_show_main
//=============================================================================
void scr_show_main(void)
{
	uint8_t		exit_loop;
	uint8_t		res;
	uint16_t	key;
	uint8_t		skey, akey, dirty;
	int			rec_idx;
	int			rec_max_idx;
	uint16_t	rec_no;
	int			rec_cur;
	int			sb_sta, sb_end, sb_len;
	int			j;
	uint8_t		fg, bg;
	char		msg[64];
	int			iret;
	pu_rec_t	*rec;
	//ps2_key_t	pk;

scr_show_main_begin:

	fg = T_LIGHTGRAY;
	bg = T_BLUE;

	// color, background
	t_clear_color_screen(fg, bg);

	// title
	scr_show_title();
	
	// key help
	scr_show_main_keyhlp();

	// big count
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
	{
		// ATX : *2 for digit counter of on-off equipment
		scr_show_big_count(dat->par->rec_cnt*2);
	}
	else
	{
		// AT
		scr_show_big_count(dat->par->rec_cnt);
	}

	// time info
	scr_show_time_info();
		
	// sta info
	scr_show_sta_info();
	
	// par info
	scr_show_par_info();
	
	// acl info
	scr_show_acl_info();

	// rec info, rec cursor
	if (dat->par->rec_cnt < 13)
	{
		rec_idx = 0;
		rec_cur	= dat->par->rec_cnt - 1;
	}
	else
	{
		rec_idx = dat->par->rec_cnt - 12;
		rec_cur = 11;	// 12 items / page
	}

	scr_show_rec_info(rec_idx, rec_cur);
	dirty = 1;

	rec_max_idx = dat->par->rec_cnt - 1;

	// assigned alarm frequency
	if (pu->alm.lo && pu->alm.hi)
	{
		dat->par->alm_lo = pu->alm.lo;
		dat->par->alm_hi = pu->alm.hi;
	}
	
	exit_loop = 0;
	while (exit_loop == 0)
	{
		// timer handler
		if (rt_tmr_handler() == PU_STS_TMR_UPD)
		{
			scr_show_time();
			scr_show_big_count_temp(0);
		}

		scr_alarm_task();

		key = (uint16_t)bioskey(1);//key = key_non_blk_read(&pk);

		if(key)//(key != KEY_SC_NULL)
		{
			akey = key & 0xFF;//akey = pk.ac;
			skey = (key >> 8) & 0xFF;//skey = pk.sc;
			
			switch(key)
			{
			case (SCAN_ESC << 8):
				res = scr_quit_main();
				if (res == 0 || res == 0xFA)
					exit_loop = 1;	// Yes : Enter or ESC (twice ESC)
				break;

			case ' ':
				iret = mainmenu_show();
				if (iret == 0)
					goto scr_show_main_begin;
				break;

			case CHAR_CARRIAGE_RETURN:
				chkrec_show(rec_idx+rec_cur);
				//sysmenu_show();
				break;

			case 'S':
			case 's':
				sysmenu_show();
				break;

			case 'C':
			case 'c':
				flmenu_show();
				break;

			// AT/ATX count
			case CHAR_TAB:
				pu->atatx_tmo = 1;
				scr_show_big_count_temp(1);
				break;

			case (SCAN_PAGE_UP << 8):
				if (rec_idx == 0)
					rec_cur = 0;	// cursor goto top
					
				rec_idx -= 12;
				if (rec_idx < 0)
					rec_idx = 0;
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			case (SCAN_PAGE_DOWN << 8):
				if (dat->par->rec_cnt <= 12)
				{
					rec_idx = 0;
				}
				else
				{
					if (rec_idx >= (dat->par->rec_cnt - 12))
						rec_cur = 11;	// cursor goto bottom
					
					rec_idx += 12;
					if (rec_idx > (dat->par->rec_cnt - 12))
						rec_idx = dat->par->rec_cnt - 12;
				}
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			case (SCAN_UP << 8):
			case (SCAN_LEFT << 8):
				if (key == (SCAN_UP << 8))
				{
					if (rec_cur <= 0)
						rec_idx--;
					else
						rec_cur--;
				}
				else
				{
					rec_idx -= 100;
				}

				if (rec_idx < 0)
					rec_idx = 0;
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			case (SCAN_DOWN << 8):
			case (SCAN_RIGHT << 8):
				if (key == (SCAN_DOWN << 8))
				{
					if (rec_cur >= 11)
					{
						rec_idx++;
					}
					else
					{
						rec_cur++;
						if (dat->par->rec_cnt < 12)
						{
							if (rec_cur >= dat->par->rec_cnt)
								rec_cur = dat->par->rec_cnt - 1;
						}
					}
				}
				else
				{
					rec_idx += 100;
				}

				if (dat->par->rec_cnt <= 12)
				{
					rec_idx = 0;
				}
				else
				{
					if (rec_idx > (dat->par->rec_cnt - 12))
						rec_idx = dat->par->rec_cnt - 12;
				}
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			// +1000
			case ((EFI_SCANEX_ALT << 8) | '+'):	// +=
			case ((EFI_SCANEX_ALT << 8) | '='):
				//if (akey == 0)	// ALT+=
				{
					rec_idx += 1000;

					if (dat->par->rec_cnt <= 12)
					{
						rec_idx = 0;
					}
					else
					{
						if (rec_idx > (dat->par->rec_cnt - 12))
							rec_idx = dat->par->rec_cnt - 12;
					}
				
					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
				}
				break;

			// -1000
			case ((EFI_SCANEX_ALT << 8) | '-'):	// _-
			case ((EFI_SCANEX_ALT << 8) | '_'):
				//if (akey == 0)	// ALT+-
				{
					rec_idx -= 1000;

					if (rec_idx < 0)
						rec_idx = 0;
					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
				}
				break;

			case ((EFI_SCANEX_ALT << 8) | 'G'):	
			case ((EFI_SCANEX_ALT << 8) | 'g'):	
				//if (akey == 0)	// Alt-G : goto record
				{
					// consider the case of sort_type is not no_inc
					if (dat->par->sort_typ != PU_PAR_SORT_NO_INC)
					{
						rec_no = dat->rec_pool[(rec_idx + rec_cur)].no;
					}
					else
					{
						rec_no = (uint16_t)rec_idx + 1 + (uint16_t)rec_cur;
					}

					if (scr_goto_record(&rec_no) == 0)
					{
						if (dat->par->rec_cnt == 0)
						{
							msgbox_waitkey(T_WHITE, T_RED, "Go to Record", "No records found!", 4, 1);
						}
						else if (rec_no > dat->par->rec_cnt || rec_no == 0)
						{
							sprintf(msg, "Record no. %d error! It should be 1 ~ %d.", rec_no, dat->par->rec_cnt);
							msgbox_waitkey(T_WHITE, T_RED, "Go to Record", msg, 4, 1);
						}
						else
						{
							// consider the case of sort_type is not no_inc
							if (dat->par->sort_typ != PU_PAR_SORT_NO_INC)
							{
								for (j=0; j<dat->par->rec_cnt; j++)
								{
									rec = (pu_rec_t *)&dat->rec_pool[j];
									if (rec->no == rec_no)
									{
										rec_no = (uint16_t)j + 1;
										break;
									}
								}
								if (rec_no <= 12)
								{
									if (rec_no < (rec_cur + 1))
									{
										rec_cur = rec_no - 1;
									}
								}
							}

							rec_idx = rec_no - 1;

							if (rec_idx > (dat->par->rec_cnt - 12))
							{
								rec_idx = dat->par->rec_cnt - 12;
								rec_cur = 11 - (dat->par->rec_cnt - rec_no);
							}
							else
							{
								// let record at rec_cur
								rec_idx -= rec_cur;
							}
							
							scr_show_rec_info(rec_idx, rec_cur);	// rec info
							dirty = 1;
						}
					}
					else
					{
						msgbox_waitkey(T_WHITE, T_RED, "Go to Record", "Can not go to record!", 4, 1);
					}
				}
				break;

			// Find : Fail
			case 'F':
			case 'f':
				if (dat->sta.gg == 0)
				{
					msgbox_waitkey(T_WHITE, T_RED, "Find Fail", "Fail record not found!", 4, 1);
				}
				else
				{
					scr_find_rec_qc(PU_REC_QC_GG);

					if (dat->par->rec_cnt <= 12)
					{
						rec_idx = 0;
						rec_cur = pu->find.fail - rec_idx;
					}
					else if (pu->find.fail > (dat->par->rec_cnt - 12))
					{
						rec_idx = dat->par->rec_cnt - 12;
						rec_cur = pu->find.fail - rec_idx;
					}
					else
					{
						rec_idx = pu->find.fail;
						rec_cur = 0;
					}

					pu->find.fail++;
					if (pu->find.fail >= dat->par->rec_cnt)
						pu->find.fail = 0;

					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
					
				}
				break;

			// Find : Error
			case 'E':
			case 'e':
				if (dat->sta.er == 0)
				{
					msgbox_waitkey(T_WHITE, T_RED, "Find Error", "Error record not found!", 4, 1);
				}
				else
				{
					scr_find_rec_qc(PU_REC_QC_ER);

					if (dat->par->rec_cnt <= 12)
					{
						rec_idx = 0;
						rec_cur = pu->find.error - rec_idx;
					}
					else if (pu->find.error > (dat->par->rec_cnt - 12))
					{
						rec_idx = dat->par->rec_cnt - 12;
						rec_cur = pu->find.error - rec_idx;
					}
					else
					{
						rec_idx = pu->find.error;
						rec_cur = 0;
					}

					pu->find.error++;
					if (pu->find.error >= dat->par->rec_cnt)
						pu->find.error = 0;

					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
				}
				break;

			// Find : N/A
			case 'N':
			case 'n':
				if (dat->sta.na == 0)
				{
					msgbox_waitkey(T_WHITE, T_RED, "Find N/A", "N/A record not found!", 4, 1);
				}
				else
				{
					scr_find_rec_qc(PU_REC_QC_NA);

					if (dat->par->rec_cnt <= 12)
					{
						rec_idx = 0;
						rec_cur = pu->find.na - rec_idx;
					}
					else if (pu->find.na > (dat->par->rec_cnt - 12))
					{
						rec_idx = dat->par->rec_cnt - 12;
						rec_cur = pu->find.na - rec_idx;
					}
					else
					{
						rec_idx = pu->find.na;
						rec_cur = 0;
					}

					pu->find.na++;
					if (pu->find.na >= dat->par->rec_cnt)
						pu->find.na = 0;

					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
				}
				break;

			// Find : error check flag
			case 'X':
			case 'x':
				if (scr_find_rec_ooxx(rec_idx) == 0)
				{
					// found
					if (dat->par->rec_cnt <= 12)
					{
						rec_idx = 0;
						rec_cur = pu->find.ooxx - rec_idx;
					}
					else if (pu->find.ooxx > (dat->par->rec_cnt - 12))
					{
						rec_idx = dat->par->rec_cnt - 12;
						rec_cur = pu->find.ooxx - rec_idx;
					}
					else
					{
						rec_idx = pu->find.ooxx;
						rec_cur = 0;
					}

					pu->find.ooxx++;
					if (pu->find.ooxx >= dat->par->rec_cnt)
						pu->find.ooxx = 0;

					scr_show_rec_info(rec_idx, rec_cur);	// rec info
					dirty = 1;
				}
				break;

			// view log file
			case 'L':
			case 'l':
				pu_log_select();
				break;

			case CHAR_BACKSPACE:
				msgbox_doing(T_WHITE, T_RED, "Output Error Log", "Create *.RAW and *.ERR files.....", 4, 1);
				
				pu_raw_output();		// POWERUP.DAT --> .RAW -> *.RAW
				pu_err_raw_output();	// POWERUP.ERR --> .ERR -> *.ERR
				
				msgbox_doing_done();
				break;

			case (SCAN_HOME << 8):
				if (rec_idx == 0)
					rec_cur = 0;	// cursor goto top

				rec_idx = 0;
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			case (SCAN_END << 8):
				if (rec_idx >= (dat->par->rec_cnt - 12))
					rec_cur = 11;	// cursor goto bottom
					
				rec_idx = dat->par->rec_cnt - 12;
				scr_show_rec_info(rec_idx, rec_cur);	// rec info
				dirty = 1;
				break;

			case (SCAN_DELETE << 8):
				if (dat->par->rec_cnt < 10)
				{
					if (rt_clear_rec() == 0)	// clear all records
						exit_loop = 1;
				}
				break;

			// help
			case (SCAN_F1 << 8):
				pu_help_show();
				break;

			// about
			case (SCAN_F2 << 8):
				pu_about_show();
				break;

			// view text report
			case (SCAN_F5 << 8):
				pu_trep_select();
				break;

			// select dat file
			case (SCAN_F6 << 8):
				if (pu_sel_dat() == 0)
					goto scr_show_main_begin;
				break;

			// graphic report
			case (SCAN_F7 << 8):
				pu_create_grep();
				break;

			// text report
			case (SCAN_F8 << 8):
				pu_create_trep();
				break;

			// interval statistic
			case (SCAN_F9 << 8):
				pu_statis_show();
				break;

			// sort : inc
			case (SCAN_F3 << 8):
				// inc : 0, 2, 4, 6
				// dec : 1, 3, 5, 7
				if ((dat->par->sort_typ & 0x1) == 0)
					dat->par->sort_typ += 2;	// inc->inc
				else
					dat->par->sort_typ -= 1;	// dec->inc
				dat->par->sort_typ &= 0x7;
				scr_show_rec_info(rec_idx, rec_cur);
				dirty = 1;
				break;

			// sort : dec
			case (SCAN_F4 << 8):
				// inc : 0, 2, 4, 6
				// dec : 1, 3, 5, 7
				if ((dat->par->sort_typ & 0x1) == 1)
					dat->par->sort_typ += 2;	// dec->dec
				else
					dat->par->sort_typ += 1;	// inc->dec
				dat->par->sort_typ &= 0x7;
				scr_show_rec_info(rec_idx, rec_cur);
				dirty = 1;
				break;

			case (SCAN_F10 << 8):
				t_scr_capture();
				break;
			}

			key_flush_buf();
		}

		//-----------------------------------------------------------------
		//  scroll bar
		//-----------------------------------------------------------------
		if (dirty)
		{
			dirty = 0;

			// scroll bar
			if (dat->par->rec_cnt > 12)
			{
				sb_len = 12  *12 / dat->par->rec_cnt;

				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (rec_idx == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if ((rec_idx + (12-1)) == rec_max_idx)
				{
					sb_end = (12 - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (rec_idx + 1) * 12 / dat->par->rec_cnt;
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (12-2))
					{
						sb_end = 12 - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}

				t_color(T_LIGHTGRAY, T_BLACK);

				// 12: 0~11
				for (j=0; j<12; j++)
				{
					if (j<sb_sta || j>sb_end)
						t_xy_putc(79, j+9, 0xb0);
					else
						t_xy_putc(79, j+9, 0xdb);	//0xb2
				}

				t_color(fg, bg);
			}
		}
	}

}

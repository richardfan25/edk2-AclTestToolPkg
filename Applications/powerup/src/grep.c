//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  GREP : Graphic Report                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "sound.h"
#include "conv.h"
#include "dat.h"
#include "g_fbmp.h"
#include "advlogo.h"
#include "pu.h"
#include "upd.h"

//=============================================================================
//  colors
//=============================================================================
// text
#define GREP_COL_FG			T_BLACK
#define GREP_COL_BG			T_WHITE
#define GREP_COL_CM			T_DARKGRAY	// common
#define GREP_COL_L0			T_RED		// high light 0
#define GREP_COL_L1			T_BLUE		// high light 1
#define GREP_COL_L2			T_GREEN		// high light 2
#define GREP_COL_L3			T_MAGENTA	// high light 3
#define GREP_COL_IF			T_CYAN		// info

// logo
#define GREP_COL_LOGO_FG	T_WHITE
#define GREP_COL_LOGO_BG	T_BLUE

// qc bar
#define GREP_COL_QC_OK		T_GREEN
#define GREP_COL_QC_GG		T_RED
#define GREP_COL_QC_ER		T_BROWN
#define GREP_COL_QC_NA		T_BLACK
#define GREP_COL_QC_OO		T_CYAN
#define GREP_COL_QC_XX		T_MAGENTA
#define GREP_COL_QC_NX		T_MAGENTA

// chart
#define GREP_COL_CH_TAB		T_BLACK
#define GREP_COL_CH_MID		T_MAGENTA
#define GREP_COL_CH_IDX		T_MAGENTA

#if 0
// text
#define GREP_COL_FG			T_WHITE
#define GREP_COL_BG			T_BLACK
#define GREP_COL_CM			T_LIGHTGRAY		// common
#define GREP_COL_L0			T_LIGHTRED		// high light 0
#define GREP_COL_L1			T_YELLOW		// high light 1
#define GREP_COL_L2			T_LIGHTGREEN	// high light 2
#define GREP_COL_L3			T_LIGHTMAGENTA	// high light 3
#define GREP_COL_IF			T_LIGHTCYAN		// info

// logo
#define GREP_COL_LOGO_FG	T_WHITE
#define GREP_COL_LOGO_BG	T_BLUE

// qc bar
#define GREP_COL_QC_OK		T_LIGHTGREEN
#define GREP_COL_QC_GG		T_LIGHTRED
#define GREP_COL_QC_ER		T_YELLOW
#define GREP_COL_QC_NA		T_DARKGRAY

// chart
#define GREP_COL_CH_TAB		T_LIGHTGRAY
#define GREP_COL_CH_MID		T_LIGHTMAGENTA
#define GREP_COL_CH_IDX		T_LIGHTMAGENTA
#endif

//=============================================================================
//  grep_create_file
//=============================================================================
static int grep_create_file(char *fname, int idx)
{
	char		str[256];
	int			vline_x[12] = { 12, 112, 212, 312, 412, 512, 612, 712, 812, 912, 1012, 513};
	int			i, j, m, vx;
	uint8_t		mask;
	char		opt[32];
	
	int			qcbar_sta;
	int			qcbar_len[4];
	uint8_t		qcbar_col[4];
	int			sta_cnt;
	int			recbar_h;
	uint8_t		recbar_col;

	g_fbmp_t	*fb;
	pu_rec_t	*rec;
	pu_raw_t	*raw;

	//---------------------------------------------------------
	//  canvas
	//---------------------------------------------------------
	// 1024x768x4bpp with palette
	fb = g_fbmp_init(1024, 768, 4, bmp_pal_acl_16, G_FONT_SUN8X16);
	if (!fb)
		return -1;

	// canvas background
	for (i=0; i<768; i++)
		g_fbmp_put_hline(fb, 0, i, GREP_COL_BG, 1024);

	// better way
	//	g_fbmp_clear_color(fb, GREP_COL_BG);

	//---------------------------------------------------------
	//  title
	//---------------------------------------------------------
	sprintf(str, "%s %s - %s Graphic Report", PU_DES_NAME, PU_DES_VER, PU_DES_DESC);
	g_fbmp_put_str(fb, 16, 16, GREP_COL_FG, GREP_COL_BG, str);

	// title underline
	g_fbmp_put_hline(fb, 16, 34, GREP_COL_FG, (int)strlen(str)*8);
	g_fbmp_put_hline(fb, 16, 36, GREP_COL_FG, (int)strlen(str)*8);

	//---------------------------------------------------------
	//  bios info
	//---------------------------------------------------------
	g_fbmp_put_str(fb, 16, 48, GREP_COL_FG, GREP_COL_BG, "BIOS Info");
	//g_fbmp_put_str(fb, 32, 72, GREP_COL_L1, GREP_COL_BG, pu->abios.magic);
	
	raw = raw_find_id(RAW_ID_ACL_BIOS);
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
	g_fbmp_put_str(fb, 32, 72, GREP_COL_L1, GREP_COL_BG, str);
	

	//---------------------------------------------------------
	//  ec info
	//---------------------------------------------------------
	g_fbmp_put_str(fb,  16,  96, GREP_COL_FG, GREP_COL_BG, "EC Info");

	// EC : chip
	g_fbmp_put_str(fb,  32, 112, GREP_COL_CM, GREP_COL_BG, "Chip Model  :");
	//g_fbmp_put_str(fb, 144, 112, GREP_COL_L0, GREP_COL_BG, pu->aec.chip);
	raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
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
	g_fbmp_put_str(fb, 144, 112, GREP_COL_L0, GREP_COL_BG, str);

	// EC : board
	g_fbmp_put_str(fb,  32, 128, GREP_COL_CM, GREP_COL_BG, "Board Name  :");
	//g_fbmp_put_str(fb, 144, 128, GREP_COL_L1, GREP_COL_BG, pu->aec.board);
	raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
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
	g_fbmp_put_str(fb, 144, 128, GREP_COL_L1, GREP_COL_BG, str);
	
	// EC : f/w version
	g_fbmp_put_str(fb,  32, 144, GREP_COL_CM, GREP_COL_BG, "F/W Version :");
	//g_fbmp_put_str(fb, 144, 144, GREP_COL_L2, GREP_COL_BG, pu->aec.ver);
	raw = raw_find_id(RAW_ID_ACL_EC_VER);
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
	g_fbmp_put_str(fb, 144, 144, GREP_COL_L2, GREP_COL_BG, str);

	//---------------------------------------------------------
	//  time info
	//---------------------------------------------------------
	g_fbmp_put_str(fb,  16, 176, GREP_COL_FG, GREP_COL_BG, "Time Info");

	// init
	conv_tm_str(&dat->par->tm_init, str);
	g_fbmp_put_str(fb,  32, 192, GREP_COL_CM, GREP_COL_BG, "Ini :");
	g_fbmp_put_str(fb,  80, 192, GREP_COL_IF, GREP_COL_BG, str);

	// start
	conv_tm_str(&dat->par->tm_zero, str);
	g_fbmp_put_str(fb,  32, 208, GREP_COL_CM, GREP_COL_BG, "Sta :");
	g_fbmp_put_str(fb,  80, 208, GREP_COL_IF, GREP_COL_BG, str);

	// current
	conv_tm_str(&dat->rec.tm_rtc, str);
	g_fbmp_put_str(fb,  32, 224, GREP_COL_CM, GREP_COL_BG, "Cur :");
	g_fbmp_put_str(fb,  80, 224, GREP_COL_IF, GREP_COL_BG, str);

	// end
	conv_tm_elapsed_str(dat->par->tm_zero, dat->rec.tm_rtc, str);
	g_fbmp_put_str(fb,  32, 240, GREP_COL_CM, GREP_COL_BG, "Ela :");
	g_fbmp_put_str(fb,  80, 240, GREP_COL_IF, GREP_COL_BG, str);
	

	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC) == 0)
	{
		// interval
		if (dat->par->rec_cnt < 10)
			sprintf(str, "Int : %s", "n/a");
		else
			sprintf(str, "Int : %d sec", dat->par->intv);
		g_fbmp_put_str(fb,  32, 256, GREP_COL_L3, GREP_COL_BG, str);
		
		// tolerance
		sprintf(str, "Tol : %d sec", dat->par->tol);
		g_fbmp_put_str(fb, 32, 272, GREP_COL_L1, GREP_COL_BG, str);
	}

	//---------------------------------------------------------
	//  features
	//---------------------------------------------------------
	g_fbmp_put_str(fb, 352,  96, GREP_COL_FG, GREP_COL_BG, "Features");

	// rtc
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
		sprintf(opt, "%s", "No");
	else
		sprintf(opt, "%s", "Yes");
	sprintf(str, "RTC    : %s", opt);
	g_fbmp_put_str(fb, 368, 112, GREP_COL_CM, GREP_COL_BG, str);

	// at/atx
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX))
		sprintf(opt, "%s", "ATX");
	else
		sprintf(opt, "%s", "AT");
	sprintf(str, "AT/ATX : %s", opt);
	g_fbmp_put_str(fb, 368, 128, GREP_COL_CM, GREP_COL_BG, str);

	// alarm
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW))
		sprintf(opt, "%s", "Yes");
	else
		sprintf(opt, "%s", "No");
	sprintf(str, "Alarm  : %s", opt);
	g_fbmp_put_str(fb, 368, 144, GREP_COL_CM, GREP_COL_BG, str);

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
	sprintf(str, "Reset  : %s", opt);
	g_fbmp_put_str(fb, 368, 160, GREP_COL_CM, GREP_COL_BG, str);

	//---------------------------------------------------------
	//  basic info
	//---------------------------------------------------------
	g_fbmp_put_str(fb, 352, 192, GREP_COL_FG, GREP_COL_BG, "Basic Info");

	sprintf(str, "Magic  : %8.8s", dat->hdr->magic);
	g_fbmp_put_str(fb, 368, 208, GREP_COL_CM, GREP_COL_BG, str);
	
	sprintf(str, "File   : %s", pu->file_name);
	g_fbmp_put_str(fb, 368, 224, GREP_COL_CM, GREP_COL_BG, str);

	sprintf(str, "Size   : %d bytes", dat->hdr->fsz);
	g_fbmp_put_str(fb, 368, 240, GREP_COL_CM, GREP_COL_BG, str);
		
	//---------------------------------------------------------
	//  count info
	//---------------------------------------------------------
	g_fbmp_put_str(fb, 712,  96, GREP_COL_FG, GREP_COL_BG, "Count Info");

	sprintf(str, "Total : %5d = %6.2f%%", dat->par->rec_cnt, 100.0);
	g_fbmp_put_str(fb, 728, 112, GREP_COL_FG, GREP_COL_BG, str);
	
	sprintf(str, "Pass  : %5d = %6.2f%%", dat->sta.ok,
			(float)dat->sta.ok*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 128, GREP_COL_QC_OK, GREP_COL_BG, str);

	sprintf(str, "Fail  : %5d = %6.2f%%", dat->sta.gg,
			(float)dat->sta.gg*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 144, GREP_COL_QC_GG, GREP_COL_BG, str);

	sprintf(str, "Error : %5d = %6.2f%%", dat->sta.er,
			(float)dat->sta.er*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 160, GREP_COL_QC_ER, GREP_COL_BG, str);

	sprintf(str, "N/A   : %5d = %6.2f%%", dat->sta.na,
			(float)dat->sta.na*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 176, GREP_COL_QC_NA, GREP_COL_BG, str);

	sprintf(str, "oo    : %5d = %6.2f%%", dat->sta.oo,
			(float)dat->sta.oo*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 192, GREP_COL_QC_OO, GREP_COL_BG, str);

	sprintf(str, "xx    : %5d = %6.2f%%", dat->sta.xx,
			(float)dat->sta.xx*100.0/(float)dat->par->rec_cnt);
	g_fbmp_put_str(fb, 728, 208, GREP_COL_QC_XX, GREP_COL_BG, str);

	sprintf(str, "nx    : %5d", dat->sta.nx);
	g_fbmp_put_str(fb, 728, 224, GREP_COL_QC_NX, GREP_COL_BG, str);

	//---------------------------------------------------------
	//  qc bar
	//---------------------------------------------------------
	qcbar_len[0] = (int)((float)dat->sta.ok*1000.0/(float)dat->par->rec_cnt);
	qcbar_len[1] = (int)((float)dat->sta.gg*1000.0/(float)dat->par->rec_cnt);
	qcbar_len[2] = (int)((float)dat->sta.er*1000.0/(float)dat->par->rec_cnt);
	qcbar_len[3] = (int)((float)dat->sta.na*1000.0/(float)dat->par->rec_cnt);

	qcbar_col[0] = GREP_COL_QC_OK;
	qcbar_col[1] = GREP_COL_QC_GG;
	qcbar_col[2] = GREP_COL_QC_ER;
	qcbar_col[3] = GREP_COL_QC_NA;

	for (j=0, qcbar_sta=12; j<4; j++)
	{
		for (i=qcbar_sta; i<qcbar_sta+qcbar_len[j]; i++)
		{
			// qcbar height : 8 => 304~311
			for (m=304; m<312; m++)
			{
				g_fbmp_put_pixel(fb, i, m, qcbar_col[j]);
			}
		}
		qcbar_sta += qcbar_len[j];
	}
	
	//---------------------------------------------------------
	//  chart : 1001 x 400
	//---------------------------------------------------------
	g_fbmp_put_hline(fb, 12, 750, GREP_COL_CH_TAB, 1001);	// bottom
	g_fbmp_put_hline(fb, 12, 550, GREP_COL_CH_MID, 1001);	// middle
	g_fbmp_put_hline(fb, 12, 350, GREP_COL_CH_TAB, 1001);	// top
	
	//---------------------------------------------------------
	//  chart : horizontal index : 1001 x 400
	//---------------------------------------------------------
	sta_cnt = idx * 1000;
	for (i=0; i<11; i++)
	{
		sprintf(str, "%d", sta_cnt + i*100);	// todo

		if (i == 0)
		{
			// left
			if (vline_x[i] < ((int)strlen(str)*8/2))
				vx = 1;
			else
				vx = vline_x[i] - ((int)strlen(str)*8)/2;
		}
		else if (i == 10)
		{
			// right
			if ((1024-vline_x[i]) < ((int)strlen(str)*8/2))
				vx = 1023 - (int)strlen(str)*8;
			else
				vx = vline_x[i] - ((int)strlen(str)*8)/2;
		}
		else
		{
			vx = vline_x[i] - ((int)strlen(str)*8)/2;
		}
		//g_fbmp_put_str(fb, axis_x[i], 334, GREP_COL_CH_TAB, GREP_COL_BG, str);
		//g_fbmp_put_str(fb, axis_x[i], 752, GREP_COL_CH_TAB, GREP_COL_BG, str);
		g_fbmp_put_str(fb, vx, 334, GREP_COL_CH_TAB, GREP_COL_BG, str);
		g_fbmp_put_str(fb, vx, 752, GREP_COL_CH_TAB, GREP_COL_BG, str);
	}

	//---------------------------------------------------------
	//  chart : vertical line
	//---------------------------------------------------------
	for (i=0; i<12; i++)
		g_fbmp_put_vline(fb, vline_x[i], 350, GREP_COL_CH_TAB, 401);

	//---------------------------------------------------------
	//  chart : record
	//---------------------------------------------------------
	m = 1000 + sta_cnt;
	for (j=0+sta_cnt; j<m; j++)
	{
		if (j >= dat->par->rec_cnt)
			break;

		rec = (pu_rec_t *)&dat->rec_pool[j];

		// recbar : height
		if (rec->intv < 0)
			recbar_h = 400;	// na : max
		else if (rec->intv >= (2*dat->par->intv))
			recbar_h = 400;
		else
			recbar_h = 200 * rec->intv / dat->par->intv;

		// recbar : color
		if (rec->intv == 0)
			recbar_col = GREP_COL_QC_NA;	// qc : na
		else if (rec->intv < 0)
			recbar_col = GREP_COL_QC_ER;	// qc : er
		else if (rec->intv > dat->par->intv)
			recbar_col = GREP_COL_QC_GG;	// qc : gg
		else
			recbar_col = GREP_COL_QC_OK;	// qc : ok

		// recbar : draw
		for (i=0; i<recbar_h; i++)
			g_fbmp_put_pixel(fb, 12+j-sta_cnt, 750-i, recbar_col);
	}

	//---------------------------------------------------------
	//  chart : record interval index
	//---------------------------------------------------------
	sprintf(str, "%d", dat->par->intv);

	vx = vline_x[0] + 1;
	g_fbmp_put_str(fb, vx, 534, GREP_COL_CH_IDX, GREP_COL_BG, str);

	vx = vline_x[5] + 2;
	
	g_fbmp_put_str(fb, vx, 534, GREP_COL_CH_IDX, GREP_COL_BG, str);

	vx = vline_x[10] - (int)strlen(str)*8;
	g_fbmp_put_str(fb, vx, 534, GREP_COL_CH_IDX, GREP_COL_BG, str);
	
	//---------------------------------------------------------
	//  advantech logo : 288 * 44 = (36*8) * 44
	//---------------------------------------------------------
	for (j=0; j<59; j++)
		g_fbmp_put_hline(fb, 696, j+8, GREP_COL_LOGO_BG, 320);

	for (j=0; j<44; j++)
	{
		for (i=j*36; i<(j+1)*36; i++)
		{
			mask = adv_logo[i];
			for (m=7; m>=0; m--)
			{
				if ( mask & (1<<m) )
					g_fbmp_put_pixel(fb, 712+(i-j*36)*8+(7-m), j+15, GREP_COL_LOGO_FG);
				else
					g_fbmp_put_pixel(fb, 712+(i-j*36)*8+(7-m), j+15, GREP_COL_LOGO_BG);
			}
		}
	}

	g_fbmp_flush(fb, fname);
	
	return 0;
}

//=============================================================================
//  pu_create_grep
//=============================================================================
void pu_create_grep(void)
{
	int		i, j;
	int		len;
	int		num;
	char	tname[32];
	char	gname[32];
	char	title[32];
	char	msg[128];
	char	*fname = pu->file_name;


	sound_end();

	len = (int)strlen(pu->file_name);

	for (i=0, j=0; i<len; i++)
	{
		if (fname[i] == '.' || fname[i] == '\n')
		{
			tname[j] = 0;
			break;
		}
		else
		{
			tname[j++] = fname[i];
		}
	}
	
	// num of files
	num = (dat->par->rec_cnt + 999 ) / 1000;

	sprintf(title, "%s", "Graphic Report");
	for (i=0; i<num; i++)
	{
		if (i > 0)
		{
			if (strlen(tname) >= 6)
				sprintf(gname, "%6s-%d.BMP", tname, i);
			else
				sprintf(gname, "%s-%d.BMP", tname, i);
		}
		else
		{
			sprintf(gname, "%s.BMP", tname);
		}

		sprintf(msg, "Creating graphic report %d/%d : %s", i+1, num, gname);
		
		//msgbox_waitkey(GREP_COL_FG, GREP_COL_L0, title, msg, 4, 1);
	
		msgbox_doing(T_WHITE, T_MAGENTA, title, msg, 4, 1);
		grep_create_file(gname, i);	// todo
		msgbox_doing_done();

		sound_ok();
	}

	sprintf(msg, "File : %s created!", gname);
	msgbox_waitkey(T_WHITE, T_MAGENTA, title, msg, 4, 1);
}

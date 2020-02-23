//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RT : Run-Time                                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <i86.h>

#include "typedef.h"
#include "t_video.h"
#include "t_hlbox.h"
#include "t_msgbox.h"
#include "dat.h"
#include "pu.h"
#include "reset.h"
#include "errcode.h"
#include "crc.h"

//===========================================================================
//  rt_tmr_handler
//===========================================================================
uint8_t rt_tmr_handler(void)
{
	uint8_t		res = PU_OK;
	
	pu->tm.curr = time(NULL);

	// timer
	if (pu->tm.curr != pu->tm.prev)
	{
		pu->tm.tmr++;
		res = PU_STS_TMR_UPD;
	}

	// off-line : no need to update reset timer
	if (pu->mode == PU_MODE_OFF_LINE)
	{
		pu->tm.prev = pu->tm.curr;
		return res;
	}
			
	// reset type
	if (dat->par->rst_typ != PU_PAR_RST_NONE)
	{
		if (pu->tm.rst_tmr)
		{
			if (pu->tm.curr != pu->tm.prev)
				pu->tm.rst_tmr--;

			// reset timer time-out
			if (pu->tm.rst_tmr == 0)
			{
				switch (dat->par->rst_typ)
				{
					case PU_PAR_RST_KBC:	reset_kbc();	break;
					case PU_PAR_RST_SOFT92:	reset_soft92();	break;
					case PU_PAR_RST_SOFT:	reset_soft();	break;
					case PU_PAR_RST_HARD:	reset_hard();	break;
					case PU_PAR_RST_FULL:	reset_full();	break;
				}
			}
		}
	}
	
	pu->tm.prev = pu->tm.curr;

	return res;
}

//===========================================================================
//  rt_sort_no_inc
//===========================================================================
int rt_sort_no_inc(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;

	if (ia->no >= ib->no)
		return 1;
	else
		return -1;
}

//===========================================================================
//  rt_sort_no_dec
//===========================================================================
int rt_sort_no_dec(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;

	if (ia->no >= ib->no)
		return -1;
	else
		return 1;
}

//===========================================================================
//  rt_sort_intv_inc
//===========================================================================
int rt_sort_intv_inc(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	int			val;

	val = (int)ia->intv - (int)ib->intv;

	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_sort_intv_dec
//===========================================================================
int rt_sort_intv_dec(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	int			val;

	val = (int)ib->intv - (int)ia->intv;

	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_sort_cpu_temp_inc
//===========================================================================
int rt_sort_cpu_temp_inc(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	int			val;

	val = (int)ia->cpu_temp - (int)ib->cpu_temp;

	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_sort_cpu_temp_dec
//===========================================================================
int rt_sort_cpu_temp_dec(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	int			val;

	val = (int)ib->cpu_temp - (int)ia->cpu_temp;

	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_sort_cpu_tsc_inc
//===========================================================================
int rt_sort_cpu_tsc_inc(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	float		val;

	val = (float)ia->cpu_tsc - (float)ib->cpu_tsc;

	if (val > 0.0)
		return 1;
	else if (val < 0.0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_sort_cpu_tsc_dec
//===========================================================================
int rt_sort_cpu_tsc_dec(const void *a, const void *b)
{
	pu_rec_t	*ia = (pu_rec_t *)a;
	pu_rec_t	*ib = (pu_rec_t *)b;
	float		val;

	val = (float)ib->cpu_tsc - (float)ia->cpu_tsc;

	if (val > 0.0)
		return 1;
	else if (val < 0.0)
		return -1;
	else
		return 0;
}

//===========================================================================
//  rt_clear_rec
//===========================================================================
uint8_t rt_clear_rec(void)
{
	FILE		*fp;
	pu_hdr_t	hdr;
	pu_par_t	par;
	t_hlbox_t	hb;
	uint8_t		res;
	uint8_t		*xraw;

	char *hb_item[2] = { "Yes", "No" };
	char *hb_help[2] = { "",      ""     };

	hb.title = " Delete all of records, are you sure ? ";
	hb.item = hb_item;
	hb.help = hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;
	hb.bg = T_RED;
	hb.fs = T_BLUE;
	hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;
	hb.bh = T_RED;

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

	hb.sel = 1;		// default : no

	res = t_show_hlbox(&hb);

	if (hb.sel == 0)
	{
		fp = fopen("POWERUP.DAT", "rb+");
		if (fp)
		{
			rewind(fp);
			fread(&hdr, sizeof(pu_hdr_t), 1, fp);
			fread(&par, sizeof(pu_par_t), 1, fp);

			par.tm_zero	= 0;	// 1st time
			par.rec_cnt	= 0;	//
			hdr.rec_num = 0;

			// re-crc : par
			xraw = (uint8_t *)&par;
			hdr.par_crc = crc_crc32(xraw, hdr.par_sz);

			// re-crc : hdr
			xraw = (uint8_t *)&hdr;
			hdr.hdr_crc = 0;
			hdr.hdr_crc = crc_crc32(xraw, hdr.hdr_sz);

			fseek(fp, hdr.hdr_off, SEEK_SET);
			fwrite(&hdr, sizeof(pu_hdr_t), 1, fp);

			fseek(fp, hdr.par_off, SEEK_SET);
			fwrite(&par, sizeof(pu_par_t), 1, fp);
			

			fseek(fp, 0L, SEEK_END);
			fflush(fp);
			fclose(fp);
		}
	}

	return hb.sel;
}

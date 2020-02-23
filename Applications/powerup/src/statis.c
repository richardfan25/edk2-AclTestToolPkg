//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  STATIS : Statistic
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "t_scrcap.h"
#include "statis.h"
#include "dat.h"
#include "pu.h"
#include "key.h"
#include "strlst.h"
#include "rt.h"

//=============================================================================
//  pu_statis_cnt_t
//=============================================================================
pu_statis_cnt_t *pu_statis_add(int intv)
{
	pu_statis_cnt_t		*ps;

	ps = (pu_statis_cnt_t *)malloc(sizeof(pu_statis_cnt_t));
	if (!ps)
		return NULL;

	memset(ps, 0, sizeof(pu_statis_cnt_t));

	// intv
	ps->intv = intv;

	// cnt
	ps->cnt	= 1;

	// qc
	if (intv == 0)
		ps->qc = PU_REC_QC_NA;	// n/a
	else if (intv < 0)
		ps->qc = PU_REC_QC_ER;	// error
	else if (intv > dat->par->intv)
		ps->qc = PU_REC_QC_GG;	// fail
	else
		ps->qc = PU_REC_QC_OK;	// ok
	

	if (pu->stt.num == 0)
	{
		pu->stt.head = ps;
	}
	else
	{
		pu->stt.curr->next = ps;

		ps->prev = pu->stt.curr;
	}
	pu->stt.num++;
	pu->stt.curr = ps;

	return ps;
}

//=============================================================================
//  pu_statis_find
//=============================================================================
pu_statis_cnt_t	*pu_statis_find(int intv)
{
	pu_statis_cnt_t	*ps;

	uint8_t	found = 0;


	ps = pu->stt.head;
	while (ps)
	{
		if (ps->intv == intv)
		{
			found = 1;
			ps->cnt++;
			break;
		}
		ps = ps->next;
	}

	if (found)
		return ps;

	return NULL;
}

//=============================================================================
//  pu_statis_swap
//=============================================================================
void pu_statis_swap(pu_statis_cnt_t *a, pu_statis_cnt_t *b)
{
	pu_statis_cnt_t *ap, *an;
	pu_statis_cnt_t *bp, *bn;

	ap = a->prev;
	an = a->next;
	bp = b->prev;
	bn = b->next;

	// (ap)<--[a]-->(an)
	// (bp)<--[b]-->(bn)
	
	// swap
	ap->next = b;
	an->prev = b;
	bp->next = a;
	bn->prev = a;

	a->prev = bp;
	a->next = bn;
	b->prev = ap;
	b->next = an;
}

//=============================================================================
//  pu_statis_exit
//=============================================================================
void pu_statis_exit(void)
{
	pu_statis_cnt_t		*ps, *p;

	int		i;

	if (pu->stt.num)
	{
		ps = pu->stt.head;
		for (i=0; i<pu->stt.num; i++)
		{
			p = ps->next;
			free(ps);

			ps = p;
		}
	}
	memset(&pu->stt, 0, sizeof(pu_statis_t));
}

//=============================================================================
//  pu_statis_init
//=============================================================================
void pu_statis_init(void)
{
	int					i;//, j;
	pu_statis_cnt_t		*ps;//, *pt;
	pu_rec_t			*rec;
	double				var;
//	uint8_t				swap = 0;
	pu_rec_t			*pool;

	
	if (pu->stt.head)
		pu_statis_exit();

	memset(&pu->stt, 0, sizeof(pu_statis_t));

	// create tmp pool
	pool = (pu_rec_t *)malloc(dat->par->rec_cnt * sizeof(pu_rec_t));
	if (!pool)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Interval Statistic", "Can not create record pool, due to malloc err!", 4, 1);
		return;
	}

	memcpy(pool, dat->rec_pool, sizeof(pu_rec_t)*dat->par->rec_cnt);
	
	//rec = (pu_rec_t *)&dat->rec_pool[0];
	rec = pool;
	qsort(rec, dat->par->rec_cnt, sizeof(pu_rec_t), rt_sort_intv_inc);
		
	// count
	for (i=0; i<dat->par->rec_cnt; i++)
	{
		//rec = (pu_rec_t *)&dat->rec_pool[i];
		rec = &pool[i];
		ps = pu_statis_find(rec->intv);
		if (!ps)
		{
			// not found intv
			pu_statis_add(rec->intv);	// todo : check ret ?
		}
	}

#if 0
	// bubble sort : intv increase
	for (i=0; i<(pu->stt.num-1); i++)
	{
		swap = 0;
		ps = pu->stt.head;

		for (j=0; j<(pu->stt.num-i-1); j++, ps=ps->next)
		{
			pt = ps->next;

			if (ps->intv > pt->intv)
			{
				pu_statis_swap(ps, pt);
				swap = 1;
			}
		}
		if (!swap)
			break;
	}
	
	// update head : head may be changed
	ps = pu->stt.head;
	while (ps->prev != NULL)
		ps = ps->prev;
	pu->stt.head = ps;
	
#endif

	// sum
	ps = pu->stt.head;
	for (i=0; i<pu->stt.num; i++)
	{
		pu->stt.sum.total += (ps->intv * ps->cnt);
		pu->stt.sum.total_cnt += ps->cnt;

		if ((ps->intv <= dat->par->intv) && 
			(ps->intv > 0))
		{
			// pass
			pu->stt.sum.pass_cnt += ps->cnt;
			pu->stt.sum.pass += (ps->intv * ps->cnt);
		}
		else
		{
			// fail
			pu->stt.sum.fail_cnt += ps->cnt;
			pu->stt.sum.fail += (ps->intv * ps->cnt);
		}
		
		ps = ps->next;
	}
	
	// avr : average
	if (dat->par->rec_cnt > 0)
		pu->stt.avr.total	= (double)pu->stt.sum.total / (double)dat->par->rec_cnt;
	
	if (pu->stt.sum.pass_cnt > 0)
		pu->stt.avr.pass	= (double)pu->stt.sum.pass	/ (double)pu->stt.sum.pass_cnt;
	else
		pu->stt.avr.pass	= 0.0;
	
	if (pu->stt.sum.fail_cnt > 0)
		pu->stt.avr.fail	= (double)pu->stt.sum.fail	/ (double)pu->stt.sum.fail_cnt;
	else
		pu->stt.avr.fail	= 0.0;

	// var
	pu->stt.avr.tvar = 0.0;
	for (i=0; i<dat->par->rec_cnt; i++)
	{
		//rec = (pu_rec_t *)&dat->rec_pool[i];
		rec = &pool[i];

		var = (double)rec->intv - pu->stt.avr.total;
		var *= var;
		pu->stt.avr.tvar += var;
	}

	pu->stt.avr.tvar /= (double)dat->par->rec_cnt;

	// standard deviation
	pu->stt.avr.stdv = sqrt(pu->stt.avr.tvar);
	
	free(pool);
}

//=============================================================================
//  pu_statis_show
//=============================================================================
void pu_statis_show(void)
{
	str_list_t		strlst;
	str_list_str_t	*s;

	pu_statis_cnt_t	*ps;

	char		str[128];
	uint8_t		loops;
	int			i, j, k, len;
	int			sx, sy;
	int			mw, mh;
	uint8_t		fg, bg;
	uint16_t		key, dirty, acol;

	int			page_max_idx;
	int			page_st_idx;
	int			sb_sta, sb_end, sb_len;

	if (dat->par->rec_cnt < 10)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Interval Statistic", "Record count should be >= 10!", 4, 1);
		return;
	}

	memset(&strlst, 0, sizeof(str_list_t));

	pu_statis_init();

	// 0         1         2         3         4         5         6         7
	// 01234567890123456789012345678901234567890123456789012345678901234567890
	// |  No       Interval  :  Count     Rate |
	// | xxx   xxxxxxxxxxxx  :  xxxxx   xxxxxx |
	// |       Total Average :  33.53 seconds  |
	// |        Pass Average :  32.96 seconds  |
	// |        Fail Average : 196.17 seconds  |
	// |            Variance :  xxxxxxxxx.xxxx |
	// |  Standard Deviation :  xxxx.xxxx      |

	strcpy(str, "  No       Interval     Count      Rate");
	str_list_add(str, &strlst);

	ps = pu->stt.head;
	for (i=0; i<pu->stt.num; i++)
	{
		if (ps->qc == PU_REC_QC_NA)
			acol = T_ARG_DARKGRAY;	// n/a
		else if (ps->qc == PU_REC_QC_ER)
			acol = T_ARG_RED;
		else if (ps->qc == PU_REC_QC_GG)
			acol = T_ARG_RED;
		else
			acol = T_ARG_BLACK;
	
		sprintf(str, "\b%c %3d   %12d  :  %5d   %6.2f%% ",
				acol, i+1, ps->intv, ps->cnt,
				(float)ps->cnt*100.0/(float)dat->par->rec_cnt);

		str_list_add(str, &strlst);

		ps = ps->next;
	}
	
	sprintf(str, "%s", "  ");
	str_list_add(str, &strlst);

	sprintf(str, "\b%c       Total Average : %.2f ", T_ARG_MAGENTA, pu->stt.avr.total);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c        Pass Average : %.2f ", T_ARG_MAGENTA, pu->stt.avr.pass);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c        Fail Average : %.2f ", T_ARG_MAGENTA, pu->stt.avr.fail);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c            Variance : %.2f ", T_ARG_BLUE, pu->stt.avr.tvar);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c  Standard Deviation : %.2f ", T_ARG_BLUE, pu->stt.avr.stdv);
	str_list_add(str, &strlst);

	//-------------------------------------------------------------------------
	//  show
	//-------------------------------------------------------------------------

	mw = strlst.maxlen + 4;	// 2:border, 2:space
	mh = 2 + 2 + 16;			// 2:border, 2:title, 16:item

	sx = (80 - mw) / 2;	// align center
	sy = (25 - mh) / 2;

	fg = T_BLACK;
	bg = T_LIGHTGRAY;

	//page_no = 0;
	//page_no_max = strlst.num/16;

	page_max_idx = strlst.num - 1;
	page_st_idx = 0;

	t_copy_region(sx, sy, sx+mw, sy+mh);
	t_color(fg, bg);
	t_put_win_single_double(sx, sy, mw, mh);
	
	// shadow
	for (i=sx+1; i<=sx+mw; i++)
		t_shadow_char(i, sy+mh);	// shadow : bottom border

	for (i=sy+1; i<=sy+mh; i++)
		t_shadow_char(sx+mw+0, i);	// shadow : right border
	
	sprintf(str, "%s", "Interval Statistics");
	t_xy_puts(sx+(mw-(int)strlen(str))/2, sy+1, str);

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
			case CHAR_CARRIAGE_RETURN:
				loops = 0;
				break;

			case (SCAN_PAGE_UP << 8):
			case (SCAN_UP << 8):

				if (key == (SCAN_PAGE_UP << 8))
					page_st_idx -= 16;
				else if (key == (SCAN_UP << 8))
					page_st_idx--;

				if (page_st_idx < 0)
					page_st_idx = 0;
			
				//page_no--;
				//if (page_no < 0)
				//	page_no = 0;
				dirty = 1;
				break;

			case (SCAN_PAGE_DOWN << 8):
			case (SCAN_DOWN << 8):

				if (key == (SCAN_PAGE_DOWN << 8))
					page_st_idx += 16;
				else if (key == (SCAN_DOWN << 8))
					page_st_idx++;

				if (page_max_idx < 16)
				{
					page_st_idx = 0;
				}
				else
				{
					if (page_st_idx > (page_max_idx-15))
						page_st_idx = page_max_idx - 15;
				}
				//page_no++;
				//if (page_no > page_no_max)
				//	page_no = page_no_max;
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
			
			//if (page_no)
			//{
			//	for (i=0; i<page_no*16; i++)
			//		s = s->next;
			//}

			//if (page_no < page_no_max)
			//{
			//	page_item_max = 16;
			//}
			//else
			//{
			//	page_item_max = strlst.num%16;
			//	if (page_item_max == 0)
			//		page_item_max = 16;
			//}

			// show item
			/*
			for (i=0; i<16; i++)
			{
				if (i < page_item_max)
				{
					t_xy_puts(sx+1, sy+3+i, s->str);
					s = s->next;
				}
				else
				{
					t_put_hline(sx+1, sy+3+i, strlst.maxlen+2);
				}
			}
			*/
			for (i=0; i<16; i++)
				t_put_hline(sx+1, sy+3+i, strlst.maxlen+2);

			for (i=0; i<16; i++)
			{
				if ((page_st_idx+i) > page_max_idx)
				{
					t_put_hline(sx+1, sy+3+i, strlst.maxlen+2);
				}
				else
				{
					len = (int)strlen(s->str);
					for (j=0,k=0; j<len; j++)
					{
						if (s->str[0] == '\n')
							break;

						if (s->str[j] == '\b')
						{
							j++;
							t_fg_color(s->str[j] & 0x0F);
							continue;
						}
						t_xy_putc(sx+2+k, sy+3+i, s->str[j]);
						k++;
					}
					s = s->next;
				}
			}

			// scroll bar
			if (strlst.num > 16)
			{
				sb_len = 16 * 16 / strlst.num;
				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (page_st_idx == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if ((page_st_idx + (16-1)) == page_max_idx)
				{
					sb_end = (16 - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (page_st_idx + 1) * 16 / strlst.num;
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (16-2))
					{
						sb_end = 16 - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}
				
				t_color(fg, bg);

				// 16: 0~15
				for (i=0; i<16; i++)
				{
					if (i<sb_sta || i>sb_end)
						t_xy_putc(sx+mw-2, i+sy+3, 0xb0);
					else
						t_xy_putc(sx+mw-2, i+sy+3, 0xdb);	// 0xb2
				}
			}
			
			dirty = 0;
		}
	}
	
	str_list_del_all(&strlst);

	pu_statis_exit();

	t_paste_region(-1, -1);
}

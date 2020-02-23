//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_SVLBOX : Scroll Vertical List Box                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <bios.h>
//#include <conio.h>

#ifdef __BORLANDC__
#include <dos.h>
#endif

#ifdef __WATCOMC__
#include <i86.h>
#endif

#include "typedef.h"
#include "t_video.h"
#include "t_svlbox.h"
#include "t_scrcap.h"
#include "t_keyhlp.h"
#include "key.h"

//=============================================================================
//  t_show_svlbox_keyhelp
//=============================================================================
void t_show_svlbox_keyhelp(void)
{
	char *svlbox_key[3] = { "ESC \x1b", "\x18\x19 PgUp PgDn Home End Space",	 "Enter"   };
	char *svlbox_name[3]= { "Quit", 	"Select"  , 							"Confirm" };

	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = svlbox_key;
	keyhlp.name= svlbox_name;
	keyhlp.num_key = sizeof(svlbox_key)/sizeof(svlbox_key[0]);
	keyhlp.num_name= sizeof(svlbox_name)/sizeof(svlbox_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  t_put_svlbox
//=============================================================================
static void t_put_svlbox(t_svlbox_t *svb)
{
	int 	i, j;
	int		sx = svb->sx;
	int		sy = svb->sy;
	int		mw = svb->mw;
	int		mh = svb->mh;
	int		tlen = svb->tlen;
	int		hlen = svb->hlen;
	uint8_t	fg = svb->fg;
	uint8_t bg = svb->bg;

	t_color(fg, bg);

	// frame
	if (svb->flag & T_SVLBOX_FL_FRAME)
	{
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
				else if (j==sy+2 && tlen && (svb->flag & T_SVLBOX_FL_TITLE))
				{
					if (i==sx)				t_putc(0xc6);	// left-middle
					else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
					else					t_putc(0xcd);	// --
				}
				// help seperator
				else if (j==sy+mh-3 && hlen && (svb->flag & T_SVLBOX_FL_HELP))
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
	}
	else
	{
		t_xy_put_rect_solid(sx, sy, sx+mw-1, sy+mh-1);
	}

	// shadow
	if (svb->flag & T_SVLBOX_FL_SHADOW)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);
		
		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw, i);
	}
}

//=============================================================================
//  t_show_svlbox
//=============================================================================
uint8_t t_show_svlbox(t_svlbox_t *svb)
{
	int		sx = 0, sy = 0, ex = 0, ey = 0, oy;
	int		tx = 0, ty = 0, mw = 0, mh = 0;
	int		wy;
	int		tlen = 0, ilen = 0, hlen = 0, len, dlen, off;
	int		i = 0, idx = 0, idx0 = 0;
	
	int		si = 0;		// si:start item idx = top item of box
	int		ei = 0;		// ei:end item idx   = bottom item of box

	char	*item;
	char	*help;
	uint16_t	scan;
	uint8_t	align;
	uint8_t	bk;
	uint8_t	sfm, sbar, sfield;	// with/without frame, scroll bar
	int		sb_sta, sb_end, sb_len;
	int		j;
	char	buf[32];
	uint8_t n, k, i_bak[256];

	bk = t_get_color();
	
	svb->err = T_SVLBOX_ERR_NONE;

	// no item found
	if ((svb->num_item == 0) || (svb->item == NULL))
	{
		svb->err = T_SVLBOX_ERR_NO_ITEM;
		goto err_svlbox;
	}

	// mw : title
	if (svb->flag & T_SVLBOX_FL_TITLE)
	{
		if (svb->title == NULL)
		{
			tlen = 0;
			svb->flag &= ~T_SVLBOX_FL_TITLE;
		}
		else
		{
			tlen = (int)strlen(svb->title);
		}
	}
	else
	{
		tlen = 0;
	}

	// mw : ind
	if (svb->flag & T_SVLBOX_FL_IND)
	{
		sprintf(buf, "%d/%d", svb->num_item, svb->num_item);
		dlen = (int)strlen(buf);	// 1=gap
	}
	else
	{
		dlen = 0;
	}
	mw = tlen + dlen;

	// mw : title : left/right margin
	if (mw)
		mw += (svb->lm + svb->rm);

	// mw : field
	if (svb->field)
	{
		len = (int)strlen(svb->field) + svb->lm + svb->rm;
		if (len > mw)
			mw = len;

		svb->flag |= T_SVLBOX_FL_FIELD;
		sfield = 1;
	}
	else
	{
		svb->flag &= ~T_SVLBOX_FL_FIELD;
		sfield = 0;
	}
	
	// mw : item
	ilen = 0;
	for (i=0; i<svb->num_item; i++)
	{
		len = (int)strlen(svb->item[i]);
		if (len > ilen)
			ilen = len;
	}

	// sbar : scroll bar, at tail of item
	if (svb->wh >= svb->num_item)
	{
		svb->wh = svb->num_item;
		sbar = 0;	// without scroll bar
	}
	else
	{
		sbar = 1;	// with scroll bar
	}

	// mw : item : left/right margin
	ilen += (svb->lm + svb->rm);

	// mw : sbar
	ilen += sbar;

	// mw : item, hm
	if (ilen > mw)
		mw = ilen;
	
	// mw : help, max length of help
	hlen = 0;
	if (svb->flag & T_SVLBOX_FL_HELP)
	{
		if (svb->help == NULL)
		{
			svb->num_help = 0;
			svb->flag &= ~T_SVLBOX_FL_HELP;
		}
		else
		{
			if (svb->num_help > 0)
			{
				for (i=0; i<svb->num_help; i++)
				{
					help = svb->help[i];
					len = (int)strlen(help);
					if (len > hlen)
						hlen = len;
				}
				// help : left/right margin
				hlen += (svb->lm + svb->rm);
			}
		}
	}
	else
	{
		hlen = 0;
	}

	if (hlen > mw)
		mw = hlen;
	
	if (svb->flag & T_SVLBOX_FL_FRAME)
	{
		mw += 2; // 2*frame left/right
		sfm = 1;
	}
	else
	{
		sfm = 0;
	}

	// title : center align will eat ind space
	if ((svb->flag & T_SVLBOX_FL_TITLE) && (dlen > 0))
	{
		if (svb->ta == T_AL_CENTER)
		{
			off = (mw/2) - ((tlen+1)/2);
			off -= svb->rm;			// right margin
			off -= sfm;				// right frame
		
			if (off < (dlen+1))		// 1: gap between title and indicator
			{
				off = (dlen+1) - off;
				mw += (off + 2);
			}
		}
	}

	if (mw > 79)
	{
		svb->err = T_SVLBOX_ERR_WIDTH;
		goto err_svlbox;
	}

	// mh : wh
	mh = svb->wh;

	// si, ei
	if (svb->sel < svb->num_item)
	{
		//si = 0;			// default 1st item
		//si = svb->sel;

		si = svb->si;
		ei = si + mh - 1;
	}
	

	if (svb->flag & T_SVLBOX_FL_FRAME)
		mh += 2;	// 2*frame top/bottom

	if (svb->tm)
		mh += svb->tm;	// top margin

	if (svb->bm)
		mh += svb->bm;	// bottom margin

	if (svb->flag & T_SVLBOX_FL_TITLE)
	{
		mh += 1;
		if (svb->flag & T_SVLBOX_FL_FRAME)
			mh += 1;	// title seperator
	}

	if (svb->flag & T_SVLBOX_FL_FIELD)
	{
		mh += 1;
	}

	if (svb->flag & T_SVLBOX_FL_HELP)
	{
		if (hlen)
			mh += 1;
		if (svb->flag & T_SVLBOX_FL_FRAME)
			mh += 1;	// help seperator
	}

	if (mh > 24)
	{
		svb->err = T_SVLBOX_ERR_HEIGHT;
		goto err_svlbox;
	}

	// alignment
	align = svb->align;

	if (align == T_AL_NONE)
	{
		sx = svb->sx;
		sy = svb->sy;
	}
	else
	{
		// horizontal
		if (align & T_AL_MASK_L)
			sx = svb->ho;
		else if (align & T_AL_MASK_R)
			sx = 80 - mw - svb->ho;
		else if (align & T_AL_MASK_C)
			sx = (80 - mw) / 2;

		// vertical
		if (align & T_AL_MASK_T)
			sy = svb->vo;
		else if (align & T_AL_MASK_B)
			sy = 25 - mh - svb->vo;
		else if (align & T_AL_MASK_M)
			sy = (25 - mh) / 2;
	}

	svb->sx = sx;
	svb->sy = sy;
	
	// position
	ex = sx + mw - 1;
	ey = sy + mh - 1;

	// out of border
	if (sx < 1)
	{
		sx = 1;
		ex = sx + mw - 1;
	}
	if (ex > 79)
	{
		sx = (79 - mw);
		ex = sx + mw - 1;
	}

	// out of border
	if (sy < 1)
	{
		sy = 1;
		ey = sy + mh - 1;
	}
	if (ey > 23)
	{
		sy = (24 - mh);
		ey = sy + mh - 1;
	}
	
	// with shadow
	if (svb->flag & T_SVLBOX_FL_SHADOW)
	{
		ex += 1;
		ey += 1;
	}

	idx = svb->sel;
	idx0 = -1;

	t_copy_region(sx, sy, ex, ey);

	svb->sx = sx;
	svb->sy = sy;
	svb->mw = mw;
	svb->mh = mh;
	svb->tlen = tlen;
	svb->hlen = hlen;

	if (svb->flag & T_SVLBOX_FL_KEYHELP)
	{
		t_copy_region(0, 24, 79, 24);
		t_show_svlbox_keyhelp();
	}

	// offset y
	if ((svb->flag & T_SVLBOX_FL_TITLE) && (tlen > 0))
	{
		if (svb->flag & T_SVLBOX_FL_FRAME)
			oy = 3;
		else
			oy = 1;
	}
	else
	{
		if (svb->flag & T_SVLBOX_FL_FRAME)
			oy = 1;
		else
			oy = 0;
	}

	key_flush_buf();
	
	// debug
	//sprintf(str, "svb->ind = %d", svb->ind);
	//t_xy_puts(0, 0, str);

	while (1)
	{
		if (idx != idx0)
		{
			t_put_svlbox(svb);

			if (svb->flag & T_SVLBOX_FL_FRAME)
				ty = sy + 1;
			else
				ty = sy;
	
			// title
			if ((svb->flag & T_SVLBOX_FL_TITLE) && (tlen > 0))
			{
				// title : align
				switch (svb->ta)
				{
				case T_AL_LEFT:		tx = sx+sfm+svb->lm;						break;
				case T_AL_CENTER:	tx = sx+(mw-tlen)/2+(svb->lm+svb->rm)/2;	break;
				case T_AL_RIGHT:	tx = ex-sfm-tlen-dlen-svb->rm;				break;
				}

				t_xy_cl_putcs(tx, ty, svb->ft, svb->bt, svb->title);
				
				// ind : indicator : 1/n, 2/n...n/n
				if (svb->flag & T_SVLBOX_FL_IND)
				{
					sprintf(buf, "%d/%d", idx+1, svb->num_item);
					// +1: gap
					t_xy_cl_putcs(ex-sfm-dlen-svb->rm+1, ty, svb->fi, svb->bi, buf);
					t_color(svb->fg, svb->bg);
				}
				
				//t_xy_puts(sx+(mw-tlen)/2, ty, svb->title);
				ty += 1;
				
				if (svb->flag & T_SVLBOX_FL_FRAME)
					ty += 1;
			}
			
			if (svb->tm)
				ty += svb->tm;

			if (svb->flag & T_SVLBOX_FL_FIELD)
			{
				t_xy_cl_putcs(sx+sfm+svb->lm, ty, svb->ff, svb->bf, svb->field);
				ty += 1;
			}

			wy = ty;

			// item
			for (i=si; i<=ei; i++)
			{
				item = svb->item[i];
				switch (svb->ia)
				{
				case T_AL_LEFT:		tx = sx+sfm+svb->lm;					break;
				case T_AL_CENTER:	tx = sx+(mw-(int)strlen(item))/2+(svb->lm+svb->rm)/2;	break;
				case T_AL_RIGHT:	tx = ex-sfm-(int)strlen(item)-svb->rm;		break;
				}
				
				if(i == idx)
				{
					// svlbox bar
					t_mark_hline(sx+sfm, sy+oy+idx-si+sfield, mw-2*sfm-sbar, svb->bs);
					t_pen_hline( sx+sfm, sy+oy+idx-si+sfield, mw-2*sfm-sbar, svb->fs);

					//this loop to delete color character, because idx item color is fix
					memset(i_bak, '\0', 256);
					for(n=0, k=0; n<255; n++)
					{
						if(item[n] == '\0')
						{
							break;
						}
						else if((item[n] == 0x08) || 	// \b : bg
							(item[n] == 0x0c))			// \f : fg
						{
							n++;
						}
						else
						{
							i_bak[k++] = item[n];
						}
					}

					item = (char *)i_bak;
				}
				else
				{
					t_color(svb->fg, svb->bg);
				}
				t_xy_putcs(tx, ty, item);//t_xy_cl_putcs(tx, ty, svb->fs, svb->bs, item);

				ty += 1;
			}

			if (svb->tm > 0)
				ty += svb->tm;

			if (svb->flag & T_SVLBOX_FL_FRAME)
				ty += 1;

			// help
			if ((svb->flag & T_SVLBOX_FL_HELP) && (hlen > 0))
			{
				if (idx < svb->num_help)
				{
					help = svb->help[idx];
					if(help[0])
					{
						switch (svb->ha)
						{
							case T_AL_LEFT:		tx = sx+sfm+svb->lm;					break;
							case T_AL_CENTER:	tx = sx+(mw-(int)strlen(help))/2+(svb->lm+svb->rm)/2;	break;
							case T_AL_RIGHT:	tx = ex-sfm-(int)strlen(help)-svb->rm;		break;
						}
						//t_xy_cl_puts(sx+(mw-strlen(help))/2, ty, svb->fh, svb->bh, help);
						t_xy_cl_putcs(tx, ty, svb->fh, svb->bh, help);
					}
				}
			}
			
			//-----------------------------------------------------------------
			//  scroll bar
			//-----------------------------------------------------------------
			if (sbar)
			{
				sb_len = svb->wh * svb->wh / svb->num_item;
				
				if (sb_len == 0)
					sb_len = 1;		// bar indicator length = 1 at least

				// top
				if (si == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if ((si + svb->wh) >= svb->num_item)
				{
					sb_end = svb->wh - 1;
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					//sb_sta = (idx + 1) * svb->wh / svb->num_item;
					sb_sta = (si + 1) * svb->wh / svb->num_item;

					// not top of bar
					if (sb_sta == 0)
						sb_sta = 1;

					sb_end = sb_sta + sb_len - 1;
					
					// not bottom iof bar
					if (sb_end >= (svb->wh-1))
					{
						sb_end = svb->wh - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}
				
				// color ?
				t_color(svb->fg, svb->bg);
			
				for (j=0; j<svb->wh; j++)
				{
					if ((j<sb_sta) || (j>sb_end))
						t_xy_putc(ex-sfm-1, j+wy, 0xb0);
					else
						t_xy_putc(ex-sfm-1, j+wy, 0xb2);
				}
			}

			idx0 = idx;
		}

		// key handler
		scan = (uint16_t)bioskey(1);//scan = key_non_blk_read_sc();

		if (scan)//(scan != KEY_SC_NULL)
		{
			if (scan == (SCAN_UP << 8))
			{
				idx--;
				if (idx < 0)
				{
					if (sbar)
					{
						// no round down
						si = 0;
						ei = svb->wh - 1;
						idx = si;
					}
					else
					{
						// round down
						ei = svb->num_item - 1;
						si = ei - svb->wh + 1;
						idx = ei;
					}
				}
				else if (idx < si)
				{
					si--;
					ei--;
				}
			}
			else if (scan == (SCAN_DOWN << 8))
			{
				idx++;
				if (idx >= svb->num_item)
				{
					if (sbar)
					{
						// no round up
						ei = svb->num_item - 1;
						si = ei - svb->wh + 1;
						idx = ei;
					}
					else
					{
						// round up
						si = 0;
						ei = svb->wh - 1;
						idx = 0;
					}
				}
				else if (idx > ei)
				{
					si++;
					ei++;
				}
			}
			else if (scan ==(SCAN_PAGE_UP << 8))
			{
				idx -= svb->wh;
				if (idx < 0)
				{
					si = 0;
					ei = svb->wh - 1;
					idx = 0;
				}
				else
				{
					si -= svb->wh;
					if (si < 0)
						si = 0;
					ei = si + svb->wh - 1;
				}
			}
			else if (scan == (SCAN_PAGE_DOWN << 8) || scan == ' ')
			{
				idx += svb->wh;
				if (idx >= svb->num_item)
				{
					ei = svb->num_item - 1;
					si = ei - svb->wh + 1;
					idx = ei;
				}
				else if (idx > ei)
				{
					ei += svb->wh;
					if (ei >= svb->num_item)
						ei = svb->num_item - 1;
					si = ei - svb->wh + 1;
				}
			}
			else if (scan == (SCAN_HOME << 8))
			{
				si = 0;
				ei = svb->wh - 1;
				idx = 0;
			}
			else if (scan == (SCAN_END << 8))
			{
				ei = svb->num_item - 1;
				si = ei - svb->wh + 1;
				idx = ei;
			}
			else if (scan == CHAR_CARRIAGE_RETURN)
			{
				svb->si = si;		// saved si
				break;
			}
			else if (scan == (SCAN_F10 << 8))
			{
				t_scr_capture();
			}
			else if (scan == (SCAN_ESC << 8) || scan == (SCAN_LEFT << 8))
			{
				idx = 0xFA;			// abort, restore select
				break;
			}
		}
	}

err_svlbox:

	// keyhelp
	if (svb->flag & T_SVLBOX_FL_KEYHELP)
		t_paste_region(-1, -1);

	// svlbox
	t_paste_region(-1, -1);

	t_set_color(bk);

	if (svb->err != T_SVLBOX_ERR_NONE)
	{
		return svb->err;
	}
	else
	{
		svb->sel = (uint8_t)idx;
		return svb->sel;
	}
}

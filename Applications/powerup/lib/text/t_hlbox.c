//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_HLBOX : Horizontal List Box                                     *
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
#include "t_hlbox.h"
#include "t_scrcap.h"
#include "t_keyhlp.h"
#include "key.h"

//=============================================================================
//  t_show_hlbox_keyhelp
//=============================================================================
void t_show_hlbox_keyhelp(void)
{
	char *hlbox_key[3] = { "ESC",  "\x1b\x1a", "Enter"   };
	char *hlbox_name[3]= { "Quit", "Select"  , "Confirm" };

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
//  t_put_hlbox
//=============================================================================
static void t_put_hlbox(t_hlbox_t *hb)
{
	int 	i, j;
	int		sx = hb->sx;
	int		sy = hb->sy;
	int		mw = hb->mw;
	int		mh = hb->mh;
	int		tlen = hb->tlen;
	int		hlen = hb->hlen;
	uint8_t	fg = hb->fg;
	uint8_t bg = hb->bg;


	t_color(fg, bg);

	if (hb->flag & T_HLBOX_FL_FRAME)
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
				else if (j==sy+2 && tlen && (hb->flag & T_HLBOX_FL_TITLE))
				{
					if (i==sx)				t_putc(0xc6);	// left-middle
					else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
					else					t_putc(0xcd);	// --
				}
				// help seperator
				else if (j==sy+mh-3 && hlen && (hb->flag & T_HLBOX_FL_HELP))
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
	if (hb->flag & T_HLBOX_FL_SHADOW)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);
		
		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw, i);
	}
}

//=============================================================================
//  t_show_hlbox
//=============================================================================
uint8_t t_show_hlbox(t_hlbox_t *hb)
{
	int		sx = 0, sy = 0, ex = 0, ey = 0, ty;
	int		mx = 0, my = 0, mw = 0, mh = 0;
	int		tlen = 0, alen = 0, hlen = 0, len;
	int		i = 0, idx = 0, idx0 = 0;
	int		*ilen = NULL;

	char	*item;
	char	*help;
	uint16_t	sc;
	uint8_t	align;
	uint8_t	bk;

	bk = t_get_color();
	
	hb->err = T_HLBOX_ERR_NONE;

	// no item found
	if (hb->num_item == 0)
	{
		hb->err = T_HLBOX_ERR_NO_ITEM;
		goto err_hlbox;
	}

	// string length : title
	tlen = (int)strlen(hb->title);

	ilen = (int *)malloc(hb->num_item * sizeof(int));
	if (!ilen)
	{
		hb->err = T_HLBOX_ERR_MALLOC;		// malloc err
		goto err_hlbox;
	}
	
	// total length of item
	alen = 0;
	for (i=0; i<hb->num_item; i++)
	{
		item = hb->item[i];
		ilen[i] = (int)strlen(item);
		alen += ilen[i];
	}

	// max length of help
	hlen = 0;
	for (i=0; i<hb->num_help; i++)
	{
		help = hb->help[i];
		len = (int)strlen(help);
		if (len > hlen)
			hlen = len;
	}

	// all of item name length
	// gap between items
	// left/right margin
	mw = alen + (hb->num_item - 1) * hb->gap + hb->hm * 2;

	// title length
	if (mw < tlen)
		mw = tlen;

	// help length
	if (mw < hlen)
		mw = hlen;

	if (hb->flag & T_HLBOX_FL_FRAME)
		mw += 2;

	if (mw > 79)
	{
		hb->err = T_HLBOX_ERR_WIDTH;
		goto err_hlbox;
	}

	// mh
	mh = 0;

	if (hb->flag & T_HLBOX_FL_TITLE)
	{
		mh += 1;
		if (hb->flag & T_HLBOX_FL_FRAME)
			mh += 1;
	}

	if (hb->flag & T_HLBOX_FL_HELP)
	{
		mh += 1;
		if (hb->flag & T_HLBOX_FL_FRAME)
			mh += 1;
	}

	mh += 1;	// hlbox button
	mh += hb->vm * 2;

	if (hb->flag & T_HLBOX_FL_FRAME)
		mh += 2;		// top/bottom

	if (mh > 24)
	{
		hb->err = T_HLBOX_ERR_HEIGHT;
		goto err_hlbox;
	}

	// alignment
	align = hb->align;

	if (align == T_AL_NONE)
	{
		sx = hb->sx;
		sy = hb->sy;
	}
	else
	{
		// hori
		if (align & T_AL_MASK_L)
			sx = hb->ha;
		else if (align & T_AL_MASK_R)
			sx = 80 - mw - hb->ha;
		else if (align & T_AL_MASK_C)
			sx = (80 - mw) / 2;

		// vert
		if (align & T_AL_MASK_T)
			sy = hb->va;
		else if (align & T_AL_MASK_B)
			sy = 25 - mh - hb->va;
		else if (align & T_AL_MASK_M)
			sy = (25 - mh) / 2;
	}

	hb->sx = sx;
	hb->sy = sy;

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
		sy = (23 - mh);
		ey = sy + mh - 1;
	}

	// with shadow
	if (hb->flag & T_HLBOX_FL_SHADOW)
	{
		ex += 1;
		ey += 1;
	}

	idx = hb->sel;
	idx0 = -1;

	t_copy_region(sx, sy, ex, ey);

	hb->sx = sx;
	hb->sy = sy;
	hb->mw = mw;
	hb->mh = mh;
	hb->tlen = tlen;
	hb->hlen = hlen;

	if (hb->flag & T_HLBOX_FL_KEYHELP)
	{
		t_copy_region(0, 24, 79, 24);
		t_show_hlbox_keyhelp();
	}
	
	key_flush_buf();

	while (1)
	{
		if (idx != idx0)
		{
			t_put_hlbox(hb);

			if (hb->flag & T_HLBOX_FL_FRAME)
				ty = sy + 1;
			else
				ty = sy;

			// title
			if ((hb->flag & T_HLBOX_FL_TITLE) && (tlen > 0))
			{
				t_xy_puts(sx+(mw-tlen)/2, ty, hb->title);
				ty += 1;
				if (hb->flag & T_HLBOX_FL_FRAME)
					ty += 1;
			}

			if (hb->vm)
				ty += hb->vm;

			//mx = sx + 4;
			//mx = sx + hb->hm;
			//mx = sx + (mw - alen - (hb->num_item-1)*hb->gap - hb->hm*2) / 2;
			//mx = sx + (mw - alen - (hb->num_item-1)*hb->gap)/2;
			//mx = sx + mw/2 - (alen + (hb->num_item-1)*hb->gap)/2;
			
			//mx = mw - alen - (hb->num_item-1)*hb->gap

			//if (hb->flag & T_HLBOX_FL_FRAME)
			//	mx += 1;

			mx = sx + (mw - alen - (hb->num_item-1)*hb->gap) / 2;
			my = ty;

			// item
			for (i=0; i<hb->num_item; i++)
			{
				item = hb->item[i];
				if (i == idx)
				{
					//t_mark_hline(mx-1, my, ilen[i]+2, hb->bs);
					//t_pen_hline( mx-1, my, ilen[i]+2, hb->fs);
					t_mark_hline(mx-2, my, ilen[i]+4, hb->bs);
					t_pen_hline( mx-2, my, ilen[i]+4, hb->fs);
					if (tlen)
						t_xy_puts(mx, my, item);
					else
						t_xy_puts(mx, my, item);
				}
				else
				{
					if (tlen)
						t_xy_cl_puts(mx, my, hb->fg, hb->bg, item);	// with title
					else
						t_xy_cl_puts(mx, my, hb->fg, hb->bg, item);	// without title
				}

				mx += (ilen[i] + hb->gap);	// 4:gap between items
			}

			ty += 1;

			if (hb->vm)
				ty += hb->vm;

			if (hb->flag & T_HLBOX_FL_FRAME)
				ty += 1;

			// help
			if ((hb->flag & T_HLBOX_FL_HELP) && (hlen > 0))
			{
				if (idx < hb->num_help)
				{
					help = hb->help[idx];
					if (help[0])
						t_xy_cl_puts(sx+(mw-(int)strlen(help))/2, ty, hb->fh, hb->bh, help);
				}
			}

			idx0 = idx;
		}

		sc = (uint16_t)bioskey(1);
		if(sc)
		{
			//sc = key_blk_read_sc();

			if (sc == (SCAN_LEFT << 8))
			{
				idx--;
				if (idx < 0)
					idx = hb->num_item - 1;
			}
			else if (sc == (SCAN_RIGHT << 8))
			{
				idx++;
				if (idx > (hb->num_item-1))
					idx = 0;
			}
			else if (sc == CHAR_CARRIAGE_RETURN)
			{
				break;
			}
			else if (sc == (SCAN_F10 << 8))
			{
				t_scr_capture();
			}
			else if (sc == (SCAN_ESC << 8))
			{
				idx = 0xFA;		// abort, restore select
				break;
			}
		}
	}

err_hlbox:

	// key help
	if (hb->flag & T_HLBOX_FL_KEYHELP)
		t_paste_region(-1, -1);

	// horizontal box
	t_paste_region(-1, -1);

	t_set_color(bk);

	if (ilen)
		free(ilen);

	if (hb->err != T_HLBOX_ERR_NONE)
	{
		return hb->err;
	}
	else
	{
		hb->sel = (uint8_t)idx;
		return hb->sel;
	}
}

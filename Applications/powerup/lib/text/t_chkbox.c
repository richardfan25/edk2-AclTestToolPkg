//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_CHKBOX : Check Box                                              *
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
#include "t_chkbox.h"
#include "t_scrcap.h"
#include "t_keyhlp.h"
#include "key.h"

//=============================================================================
//  t_put_chkbox_keyhelp
//=============================================================================
void t_put_chkbox_keyhelp(void)
{
	char *chkbox_key[4] = { "ESC",  "\x18\x19",	"Space",			"Enter"   };
	char *chkbox_name[4]= { "Back", "Select",	"Check/Un-check"  , "Confirm" };

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
//  t_put_chkbox
//=============================================================================
static void t_put_chkbox(t_chkbox_t *cb)
{
	int 	i, j;
	int		sx = cb->sx;
	int		sy = cb->sy;
	int		mw = cb->mw;
	int		mh = cb->mh;
	int		tlen = cb->tlen;
	uint8_t	fg = cb->fg;
	uint8_t bg = cb->bg;

	t_color(fg, bg);

	if (cb->flag & T_CHKBOX_FL_FRAME)
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
				else if (j==sy+2 && tlen && (cb->flag & T_CHKBOX_FL_TITLE))
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
	if (cb->flag & T_CHKBOX_FL_SHADOW)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);
		
		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw, i);
	}
}

//=============================================================================
//  t_show_chkbox
//=============================================================================
uint8_t t_show_chkbox(t_chkbox_t *cb)
{
	int		sx = 0, sy = 0, ex = 0, ey = 0, oy;
	int		tx = 0, ty = 0, mw = 0, mh = 0;
	int		tlen, len_max, len;
	int		i, idx, idx0;

	char		*item;
	uint16_t		sc;
	uint8_t		align;
	uint8_t		dirty;
	uint16_t	mask;
	uint16_t	sel, sw;
	uint8_t		bk;

	bk = t_get_color();

	cb->err = T_CHKBOX_ERR_NONE;

	// no item found
	if (cb->num_item == 0)
	{
		cb->err = T_CHKBOX_ERR_NO_ITEM;
		goto err_chkbox;
	}

	// string length : title
	if (cb->flag & T_CHKBOX_FL_TITLE)
		tlen = (int)strlen(cb->title);
	else
		tlen = 0;

	mw = tlen;

	// max length of item
	len_max = 0;
	for (i=0; i<cb->num_item; i++)
	{
		item = cb->item[i];
		len = (int)strlen(item);
		if (len > len_max)
			len_max = len;
	}
	
	if (len_max > mw)
		mw = len_max;

	mw += (cb->hm * 2);	// 2*left/right hm

	if (cb->flag & T_CHKBOX_FL_FRAME)
		mw += 2;	// 2*left/right frame

	mw += 1;		// 1*check mark
	mw += cb->gap;	// 1*gap
	
	if (mw > 79)
	{
		cb->err = T_CHKBOX_ERR_WIDTH;
		goto err_chkbox;
	}

	mh = cb->num_item;
	
	if (cb->flag & T_CHKBOX_FL_FRAME)
		mh += 2;	// 2*top/bottom

	if (cb->vm)
		mh += (cb->vm*2);	// v margin

	if (cb->flag & T_CHKBOX_FL_TITLE)
	{
		mh += 1;
		if (cb->flag & T_CHKBOX_FL_FRAME)
			mh += 1;
	}

	if (mh > 24)
	{
		cb->err = T_CHKBOX_ERR_HEIGHT;
		goto err_chkbox;
	}

	// alignment
	align = cb->align;

	if (align == T_AL_NONE)
	{
		sx = cb->sx;
		sy = cb->sy;
	}
	else
	{
		// hori
		if (align & T_AL_MASK_L)
			sx = cb->ha;
		else if (align & T_AL_MASK_R)
			sx = 80 - mw - cb->ha;
		else if (align & T_AL_MASK_C)
			sx = (80 - mw) / 2;

		// vert
		if (align & T_AL_MASK_T)
			sy = cb->va;
		else if (align & T_AL_MASK_B)
			sy = 25 - mh - cb->va;
		else if (align & T_AL_MASK_M)
			sy = (25 - mh) / 2;
	}

	cb->sx = sx;
	cb->sy = sy;
	
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
	if (cb->flag & T_CHKBOX_FL_SHADOW)
	{
		ex += 1;
		ey += 1;
	}

	idx = 0;
	idx0 = -1;
	dirty = 1;
	
	sel = cb->sel;
	sw  = cb->sw;

	t_copy_region(sx, sy, ex, ey);

	cb->sx = sx;
	cb->sy = sy;
	cb->mw = mw;
	cb->mh = mh;
	cb->tlen = tlen;

	if (cb->flag & T_CHKBOX_FL_KEYHELP)
	{
		t_copy_region(0, 24, 79, 24);
		t_put_chkbox_keyhelp();
	}

	// offset y
	if ((cb->flag & T_CHKBOX_FL_TITLE) && (tlen > 0))
	{
		if (cb->flag & T_CHKBOX_FL_FRAME)
			oy = 3;
		else
			oy = 1;
	}
	else
	{
		if (cb->flag & T_CHKBOX_FL_FRAME)
			oy = 1;
		else
			oy = 0;
	}

	key_flush_buf();
	
	while (1)
	{
		if ((idx != idx0) || dirty)
		{
			dirty = 0;

			t_put_chkbox(cb);

			if (cb->flag & T_CHKBOX_FL_FRAME)
				ty = sy + 1;
			else
				ty = sy;
			
			// title
			if ((cb->flag & T_CHKBOX_FL_TITLE) && (tlen > 0))
			{
				// title : align
				switch ((cb->tialign>>4) & 0x7)
				{
				case T_AL_LEFT:		t_xy_puts(sx+1, ty, cb->title);				break;
				case T_AL_CENTER:	t_xy_puts(sx+(mw-tlen)/2, ty, cb->title);	break;
				case T_AL_RIGHT:	t_xy_puts(ex-tlen, ty, cb->title);			break;
				}
				
				t_xy_puts(sx+(mw-tlen)/2, ty, cb->title);
				ty += 1;

				if (cb->flag & T_CHKBOX_FL_FRAME)
					ty += 1;
			}
			
			if (cb->vm > 0)
				ty += cb->vm;

			// item
			for (i=0; i<cb->num_item; i++)
			{
				if(i == idx)
				{
					t_mark_hline(sx+1, sy+oy+idx, mw-2, cb->bs);
					t_pen_hline( sx+1, sy+oy+idx, mw-2, cb->fs);
				}
				else
				{
					t_color(cb->fg, cb->bg);
				}
				
				item = cb->item[i];
				switch (cb->tialign & 0x7)
				{
					case T_AL_LEFT:
						tx = sx+cb->hm+1+cb->gap;
						if (cb->flag & T_CHKBOX_FL_FRAME)
							tx += 1;
						break;

					case T_AL_CENTER:
						tx = sx+(mw-(int)strlen(item))/2;
						break;

					case T_AL_RIGHT:
						tx = ex-(int)strlen(item)-cb->hm;
						if (cb->flag & T_CHKBOX_FL_FRAME)
							tx -= 1;
						break;
				}
				
				if(i == idx)
				{
					t_xy_puts(tx, ty, item);
				}
				else
				{
					// sw
					if (sw & (1<<i))
						t_xy_cl_puts(tx, ty, cb->fg, cb->bg, item);			// sw=on
					else
						t_xy_cl_puts(tx, ty, cb->fg ^ 0x8, cb->bg, item);	// sw=off
				}
				
				t_fg_color(cb->fs);
				mask = (1<<i);
				
				tx = sx+cb->hm;
				if (cb->flag & T_CHKBOX_FL_FRAME)
					tx += 1;

				if (sel & mask)
				{
					t_xy_putc(tx, ty, 0xFB);	// check
				}
				else
				{
					t_xy_putc(tx, ty, '_');
				}
				t_fg_color(cb->fg);

				ty += 1;
			}

			if (cb->vm > 0)
				ty += cb->vm;

			//t_mark_hline(sx+1, sy+oy+idx, mw-2, cb->bs);
			//t_pen_hline( sx+1, sy+oy+idx, mw-2, cb->fs);

/*			
			if (tlen)
			{
				t_mark_hline(sx+1, sy+3+idx, mw-2, cb->bs);
				t_pen_hline( sx+1, sy+3+idx, mw-2, cb->fs);
			}
			else
			{
				t_mark_hline(sx+1, sy+1+idx, mw-2, cb->bs);
				t_pen_hline( sx+1, sy+1+idx, mw-2, cb->fs);
			}
	*/
			
			idx0 = idx;
		}

		sc = (uint16_t)bioskey(1);//sc = key_blk_read_sc();
		if (sc)
		{
			//sc = key_blk_read_sc();
			if (sc == (SCAN_UP << 8))
			{
				idx--;
				if (idx < 0)
					idx = cb->num_item - 1;
			}
			else if (sc == (SCAN_DOWN<< 8))
			{
				idx++;
				if (idx > (cb->num_item-1))
					idx = 0;
			}
			else if (sc == ' ')
			{
				// toggle
				if (sw & (1<<idx))
				{
					// can be toggled if sw = on
					sel ^= (1<<idx);
					dirty = 1;
				}
			}
			else if (sc == CHAR_CARRIAGE_RETURN)
			{
				cb->sel = sel;
				break;
			}
			else if (sc == (SCAN_F10 << 8))
			{
				t_scr_capture();
			}
			else if (sc == (SCAN_ESC << 8))
			{
				cb->err = 0xFA;	// abort, restore select
				break;
			}
		}
	}

err_chkbox:

	// key help
	if (cb->flag & T_CHKBOX_FL_KEYHELP)
		t_paste_region(-1, -1);

	// check box
	t_paste_region(-1, -1);

	t_set_color(bk);

	if (cb->err)
	{
		return cb->err;
	}
	else
	{
		return 0;
	}
}

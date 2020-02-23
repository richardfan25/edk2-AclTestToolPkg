//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_VLBOX : Vertical List Box                                       *
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
#include "t_vlbox.h"
#include "t_scrcap.h"
#include "t_keyhlp.h"
#include "key.h"

//=============================================================================
//  t_show_vlbox_keyhelp
//=============================================================================
void t_show_vlbox_keyhelp(void)
{
	char *vlbox_key[3] = { "ESC \x1b",  "\x18\x19", "Enter"   };
	char *vlbox_name[3]= { "Quit", "Select"  , "Confirm" };

	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key = vlbox_key;
	keyhlp.name= vlbox_name;
	keyhlp.num_key = sizeof(vlbox_key)/sizeof(vlbox_key[0]);
	keyhlp.num_name= sizeof(vlbox_name)/sizeof(vlbox_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  t_put_vlbox
//=============================================================================
static void t_put_vlbox(t_vlbox_t *vb)
{
	int 	i, j;
	int		sx = vb->sx;
	int		sy = vb->sy;
	int		mw = vb->mw;
	int		mh = vb->mh;
	int		tlen = vb->tlen;
	int		hlen = vb->hlen;
	uint8_t	fg = vb->fg;
	uint8_t bg = vb->bg;

	t_color(fg, bg);

	// frame
	if (vb->flag & T_VLBOX_FL_FRAME)
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
				else if (j==sy+2 && tlen && (vb->flag & T_VLBOX_FL_TITLE))
				{
					if (i==sx)				t_putc(0xc6);	// left-middle
					else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
					else					t_putc(0xcd);	// --
		
				}
				// help seperator
				else if (j==sy+mh-3 && hlen && (vb->flag & T_VLBOX_FL_HELP))
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
	if (vb->flag & T_VLBOX_FL_SHADOW)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);
		
		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw, i);
	}
}

//=============================================================================
//  t_show_vlbox
//=============================================================================
uint8_t t_show_vlbox(t_vlbox_t *vb)
{
	int		sx = 0, sy = 0, ex = 0, ey = 0, oy;
	int		tx = 0, ty = 0, mw = 0, mh = 0, mo;
	int		tlen = 0, ilen = 0, hlen = 0, len;
	int		i = 0, idx = 0, idx0 = 0;

	char	*item;
	char	*help = NULL;
	uint16_t	scan;
	uint8_t	align;
	uint8_t	bk;
	uint8_t j, k, i_bak[256];

	bk = t_get_color();
	
	vb->err = T_VLBOX_ERR_NONE;

	// no item found
	if (vb->num_item == 0)
	{
		vb->err = T_VLBOX_ERR_NO_ITEM;
		goto err_vlbox;
	}

	// string length : title
	if (vb->flag & T_VLBOX_FL_TITLE)
		tlen = (int)strlen(vb->title);
	else
		tlen = 0;
	mw = tlen;

	// max length of item
	ilen = 0;
	for (i=0; i<vb->num_item; i++)
	{
		len = (int)strlen(vb->item[i]);
		if (len > ilen)
			ilen = len;
	}
	
	//ilen += 2;	// item left/right space
	ilen += (vb->hm*2);

	if (ilen > mw)
		mw = ilen;
	
	// max length of help
	hlen = 0;
	if (vb->flag & T_VLBOX_FL_HELP)
	{
		if (vb->num_help > 0)
		{
			for (i=0; i<vb->num_help; i++)
			{
				len = (int)strlen(help);
				help = vb->help[i];
				if (len > hlen)
					hlen = len;
			}

			hlen += (vb->hm*2);	// h margin
		}
	}

	if (hlen > mw)
		mw = hlen;

	if (vb->flag & T_VLBOX_FL_FRAME)
	mw += 2; // 2*frame left/right

	if (mw > 79)
	{
		vb->err = T_VLBOX_ERR_WIDTH;
		mo = mw - 79;
		mw = 79;

		// shorten the length of item name 
		for (i=0; i<vb->num_item; i++)
		{
			len = (int)strlen(vb->item[i]);
			vb->item[i][(len-mo)] = 0;
		}
	
		// don't return error
		//goto err_vlbox;
	}

	mh = vb->num_item;

	if (vb->flag & T_VLBOX_FL_FRAME)
		mh += 2;	// 2*frame top/bottom

	if (vb->vm)
		mh += (vb->vm*2);	// v margin

	if (vb->flag & T_VLBOX_FL_TITLE)
	{
		mh += 1;
		if (vb->flag & T_VLBOX_FL_FRAME)
			mh += 1;
	}

	if (vb->flag & T_VLBOX_FL_HELP)
	{
		if (hlen > 0)
		{
			mh += 1;
			if (vb->flag & T_VLBOX_FL_FRAME)
				mh += 1;
		}
	}

	if (mh > 24)
	{
		vb->err = T_VLBOX_ERR_HEIGHT;
		goto err_vlbox;
	}

	// alignment
	align = vb->align;

	if (align == T_AL_NONE)
	{
		sx = vb->sx;
		sy = vb->sy;
	}
	else
	{
		// horizontal
		if (align & T_AL_MASK_L)
			sx = vb->ha;
		else if (align & T_AL_MASK_R)
			sx = 80 - mw - vb->ha;
		else if (align & T_AL_MASK_C)
			sx = (80 - mw) / 2;

		// vertical
		if (align & T_AL_MASK_T)
			sy = vb->va;
		else if (align & T_AL_MASK_B)
			sy = 25 - mh - vb->va;
		else if (align & T_AL_MASK_M)
			sy = (25 - mh) / 2;
	}

	vb->sx = sx;
	vb->sy = sy;
	
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
	if (ey > 22)
	{
		sy = (23 - mh);
		ey = sy + mh - 1;
	}
	
	// with shadow
	if (vb->flag & T_VLBOX_FL_SHADOW)
	{
		ex += 1;
		ey += 1;
	}

	idx = vb->sel;
	idx0 = -1;

	t_copy_region(sx, sy, ex, ey);

	vb->sx = sx;
	vb->sy = sy;
	vb->mw = mw;
	vb->mh = mh;
	vb->tlen = tlen;
	vb->hlen = hlen;

	if (vb->flag & T_VLBOX_FL_KEYHELP)
	{
		t_copy_region(0, 24, 79, 24);
		t_show_vlbox_keyhelp();
	}

	// offset y
	if ((vb->flag & T_VLBOX_FL_TITLE) && (tlen > 0))
	{
		if (vb->flag & T_VLBOX_FL_FRAME)
			oy = 3;
		else
			oy = 1;
	}
	else
	{
		if (vb->flag & T_VLBOX_FL_FRAME)
			oy = 1;
		else
			oy = 0;
	}
						
	key_flush_buf();
	
	while (1)
	{
		if (idx != idx0)
		{
			t_put_vlbox(vb);

			if (vb->flag & T_VLBOX_FL_FRAME)
				ty = sy + 1;
			else
				ty = sy;
	
			// title
			if ((vb->flag & T_VLBOX_FL_TITLE) && (tlen > 0))
			{
				// title : align
				switch ((vb->tialign>>4) & 0x7)
				{
				case T_AL_LEFT:		t_xy_putcs(sx+1, ty, vb->title);			break;
				case T_AL_CENTER:	t_xy_putcs(sx+(mw-tlen)/2, ty, vb->title);	break;
				case T_AL_RIGHT:	t_xy_putcs(ex-tlen, ty, vb->title);			break;
				}
				//t_xy_puts(sx+(mw-tlen)/2, ty, vb->title);
				ty += 1;
				
				if (vb->flag & T_VLBOX_FL_FRAME)
					ty += 1;
			}

			if (vb->vm > 0)
				ty += vb->vm;

			// item
			for (i=0; i<vb->num_item; i++)
			{
				item = vb->item[i];
				switch (vb->tialign & 0x7)
				{
				case T_AL_LEFT:		tx = sx+1+vb->hm;					break;
				case T_AL_CENTER:	tx = sx+(mw-(int)strlen(item))/2+vb->hm;	break;
				case T_AL_RIGHT:	tx = ex-(int)strlen(item)-vb->hm;		break;
				}
				
				if(i == idx)
				{
					// vlbox bar
					t_mark_hline(sx+1, sy+oy+idx, mw-2, vb->bs);
					t_pen_hline( sx+1, sy+oy+idx, mw-2, vb->fs);

					//this loop to delete color character, because idx item color is fix
					memset(i_bak, '\0', 256);
					for(j=0, k=0;j<255;j++)
					{
						if(item[j] == '\0')
						{
							break;
						}
						else if((item[j] == 0x08) || 	// \b : bg
							(item[j] == 0x0c))			// \f : fg
						{
							j++;
						}
						else
						{
							i_bak[k++] = item[j];
						}
					}

					item = (char *)i_bak;
				}
				else
				{
					t_color(vb->fg, vb->bg);
				}
				t_xy_putcs(tx, ty, item);

				ty += 1;
			}

			if (vb->vm > 0)
				ty += vb->vm;

			if (vb->flag & T_VLBOX_FL_FRAME)
				ty += 1;

			// help
			if ((vb->flag & T_VLBOX_FL_HELP) && (hlen > 0))
			{
				if (idx < vb->num_help)
				{
					help = vb->help[idx];
					if (help[0])
						t_xy_cl_puts(sx+(mw-(int)strlen(help))/2, ty, vb->fh, vb->bh, help);
				}
			}

			idx0 = idx;
		}

		// key handler
		scan = (uint16_t)bioskey(1);//scan = key_non_blk_read_sc();
		if(scan)//if (scan != KEY_SC_NULL)
		{
			if (scan == (SCAN_UP << 8))
			{
				idx--;
				if (idx < 0)
					idx = vb->num_item - 1;
			}
			else if (scan == (SCAN_DOWN << 8))
			{
				idx++;
				if (idx > (vb->num_item-1))
					idx = 0;
			}
			else if (scan == CHAR_CARRIAGE_RETURN)
			{
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

err_vlbox:

	// keyhelp
	if (vb->flag & T_VLBOX_FL_KEYHELP)
		t_paste_region(-1, -1);

	// vlbox
	t_paste_region(-1, -1);

	t_set_color(bk);

	if (vb->err != T_VLBOX_ERR_NONE)
	{
		return vb->err;
	}
	else
	{
		vb->sel = (uint8_t)idx;
		return vb->sel;
	}
}

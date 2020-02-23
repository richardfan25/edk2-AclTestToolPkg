//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_HEXTAB : Hex Table                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_hextab.h"

//=============================================================================
//  t_show_hextab
//=============================================================================
uint8_t t_show_hextab(t_hextab_t *ht)
{
	int		sx = 0, sy = 0;
	int		ix = 0, iy = 0;
	int		mw, mh;
	int		i, j, len, lenz, idx;
	uint8_t	c;
	uint8_t	*buf;
	uint8_t	fg, bg;
	char	msg[32];
	int		tlen;
	uint8_t	bk;

	bk = t_get_color();

	// len
	if (ht->len == 0)
		return 0;	// todo
	else if (ht->len > 256)
		len = 256;
	else
		len = (int)ht->len;

	tlen = (int)strlen(ht->title);

	// with title : force frame enable
	//if (tlen > 0)
	//	ht->flag |= T_HEX_FL_FRM;

	// mh
	mh = (len + 15) / 16;

	// mh : frame => top, bottom
	if (ht->flag & T_HEX_FL_FRM)
		mh += 2;

	// mh : h-index
	if (ht->flag & T_HEX_FL_IDX)
		mh += 1;
	
	// mh : with title
	if (tlen > 0)
	{
		mh++;	// title
		if (ht->flag & T_HEX_FL_FRM)
			mh++;	// separator
	}

	// mw
	if (ht->flag & T_HEX_FL_FRM)
		mw = 2;			// border : left/right
	else
		mw = 0;

	mw += 1;		// 1=gap
	mw += 16 * 3;	// hex : 16 bytes  (2=hex, 1=gap)
	mw += 2;		// [7]-[8] gap
	
	if (ht->flag & T_HEX_FL_ASC)
		mw += 16;		// asc : 16 chars
	else
		mw -= 1;
	
	if ((ht->flag & T_HEX_FL_IDX) == 0)
		mw -= 3;

	// v-index
	switch (ht->flag & T_HEX_FL_IDX_MASK)
	{
	case T_HEX_FL_IDX_BYTE:		mw += 2;	break;
	case T_HEX_FL_IDX_WORD:		mw += 4;	break;
	case T_HEX_FL_IDX_DWORD:	mw += 8;	break;
	}
	
	if (ht->align == T_AL_NONE)
	{
		sx = ht->sx;
		sy = ht->sy;
	}
	else
	{
		// hori
		if (ht->align & T_AL_MASK_L)
			sx = ht->hm;
		else if (ht->align & T_AL_MASK_R)
			sx = 80 - mw - ht->hm;
		else if (ht->align & T_AL_MASK_C)
			sx = (80 - mw) / 2;

		// vert
		if (ht->align & T_AL_MASK_T)
			sy = ht->vm;
		else if (ht->align & T_AL_MASK_B)
			sy = 25 - mh - ht->vm;
		else if (ht->align & T_AL_MASK_M)
			sy = (25 - mh) / 2;
	}

	buf = ht->data;

	// drawing 
	fg = ht->fg;
	bg = ht->bg;
	
	t_color(fg, bg);

	if (ht->flag & T_HEX_FL_FRM)
	{
		if (tlen > 0)
			t_put_win_double(sx, sy, mw, mh);
		else
			t_put_rect_double_solid(sx, sy, mw, mh);

		// frame shadow
		if (ht->flag & T_HEX_FL_SHADOW)
		{
			for (i=sx+1; i<=sx+mw; i++)
				t_shadow_char(i, sy+mh);	// shadow : bottom border

			for (i=sy+1; i<=sy+mh; i++)
				t_shadow_char(sx+mw+0, i);	// shadow : right border
		}
	}
	else
	{t_color(EFI_LIGHTGRAY,EFI_BLACK);Print(L"sx=%d sy=%d",sx,sy);bioskey(0);
		t_put_rect_solid(sx, sy, mw+1, mh);
	}
		
	// title
	if (tlen > 0)
	{
		ix = sx + (mw - tlen)/2;
		if (ht->flag & T_HEX_FL_FRM)
			iy = sy + 1;
		else
			iy = sy;
		t_xy_cl_puts(ix, iy, ht->ft, ht->bt, ht->title);
	}

	// index
	if (ht->flag & T_HEX_FL_IDX)
	{
		t_fg_color(ht->fi);	// index color

		// h-index : hex
		ix = sx + 3;
		iy = sy;
		if (ht->flag & T_HEX_FL_FRM)
		{
			iy += 1;
			if (tlen > 0)
				iy += 2;
		}
		else
		{
			if (tlen > 0)
				iy += 1;
		}

		// v-index
		switch (ht->flag & T_HEX_FL_IDX_MASK)
		{
		case T_HEX_FL_IDX_BYTE:		ix += 2;	break;
		case T_HEX_FL_IDX_WORD:		ix += 4;	break;
		case T_HEX_FL_IDX_DWORD:	ix += 8;	break;
		}

		for (i=0; i<16; i++)
		{
			if (i < 10)
				t_xy_putc(ix, iy, '0'+(char)i);
			else
				t_xy_putc(ix, iy, 'A'-10+(char)i);
		
			ix += 3;

			if (i==7)
				ix += 2;
		}
	}

	// with asc
	if (ht->flag & T_HEX_FL_ASC)
	{
		// hex-asc gap
		ix -= 1;

		if (ht->flag & T_HEX_FL_IDX)
		{
			// h-index : asc
			for (i=0; i<16; i++)
			{
				if (i < 10)
					t_xy_putc(ix, iy, '0'+(char)i);
				else
					t_xy_putc(ix, iy, 'A'-10+(char)i);

				ix++;
			}
		}
	}

	// data
	for (i=0; i<ht->len; i+=16, len-=16)
	{
		ix = sx + 1;
		
		iy = sy + i/16;
		if ((ht->flag & T_HEX_FL_FRM))
		{
			iy += 1;	// top-frm
			if (tlen > 0)
				iy += 2;

			if (ht->flag & T_HEX_FL_IDX)
				iy += 1;
		}
		else
		{
			if (tlen > 0)
				iy += 1;

			if (ht->flag & T_HEX_FL_IDX)
				iy += 1;
		}
		
		// index : 0x00, 0x10...0xF0
		if (ht->flag & T_HEX_FL_IDX)
		{
			switch (ht->flag & T_HEX_FL_IDX_MASK)
			{
			case T_HEX_FL_IDX_BYTE:		sprintf(msg, "%02X ", (uint8_t)i+ht->stidx);	break;
			case T_HEX_FL_IDX_WORD:		sprintf(msg, "%04X ", (uint8_t)i+ht->stidx);	break;
			case T_HEX_FL_IDX_DWORD:	sprintf(msg, "%08X ", (uint8_t)i+ht->stidx);	break;
			}
			t_xy_cl_puts(ix, iy, ht->fi, ht->bi, msg);
			ix += (int)strlen(msg);
		}

		if (len < 16)
			lenz = len;	// less 16 bytes, last line
		else
			lenz = 16;	// 16 bytes per/line

		// hex data
		for (j=0; j<lenz; j++)
		{
			c = buf[i+j];
			sprintf(msg, "%02X", c);

			if (c == 0x00)
				t_xy_cl_puts(ix, iy, T_LIGHTGRAY, bg, msg);
			else if (c == 0xFF)
				t_xy_cl_puts(ix, iy, T_WHITE, bg, msg);
			else
				t_xy_cl_puts(ix, iy, T_YELLOW, bg, msg);

			ix += 3;
			
			if (j==7)
			{
				t_xy_cl_puts(ix, iy, T_LIGHTGRAY, bg, "- ");
				ix += 2;
			}
		}

		if (ht->flag & T_HEX_FL_ASC)
		{
			if (lenz < 16)
				ix += ((16-lenz) * 3);

			if (lenz < 8)
				ix += 2;	// [7]-[8] separator
		
			// separator
			t_xy_cl_puts(ix, iy, T_LIGHTGRAY, bg, "  ");

			// asc data
			for (j=0, idx=0; j<lenz; j++)
			{
				c = buf[i+j];
				if (c < 0x20 || c > 0x7E)
					msg[idx++] = '.';
				else
					msg[idx++] = c;
			}
			msg[lenz] = 0;
		
			t_xy_cl_puts(ix, iy, T_LIGHTGRAY, bg, msg);
		}
	}

	t_set_color(bk);

	return 0;
}

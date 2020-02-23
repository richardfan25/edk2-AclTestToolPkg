//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_INDAT : Input Data                                              *
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
#include "t_indat.h"
#include "t_msgbox.h"
#include "t_keyhlp.h"
#include "t_scrcap.h"
#include "key.h"
#include "video.h"

//=============================================================================
//  t_show_indat_dec_keyhelp
//=============================================================================
void t_show_indat_dec_keyhelp(void)
{
	char *indat_dec_key[4] = { "ESC",  "0~9",   "BackSpace", "Enter"   };
	char *indat_dec_name[4]= { "Back", "Input", "Delete",    "Confirm" };

	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 79;//80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key		= indat_dec_key;
	keyhlp.name		= indat_dec_name;
	keyhlp.num_key	= sizeof(indat_dec_key)/sizeof(indat_dec_key[0]);
	keyhlp.num_name	= sizeof(indat_dec_name)/sizeof(indat_dec_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  t_show_indat_hex_keyhelp
//=============================================================================
void t_show_indat_hex_keyhelp(void)
{
	char *indat_hex_key[4] = { "ESC",  "0~9A~F",   "BackSpace", "Enter"   };
	char *indat_hex_name[4]= { "Back", "Input", "Delete",    "Confirm" };

	t_keyhlp_t	keyhlp;

	keyhlp.sx = 0;			// sx : start_x
	keyhlp.sy = 24;			// sy : start_y
	keyhlp.kw = 80;

	keyhlp.fg = T_BLACK;		// fg : foreground color
	keyhlp.bg = T_LIGHTGRAY;	// bg : background color
	keyhlp.kg = T_RED;			// kg : key name color

	keyhlp.key		= indat_hex_key;
	keyhlp.name		= indat_hex_name;
	keyhlp.num_key	= sizeof(indat_hex_key)/sizeof(indat_hex_key[0]);
	keyhlp.num_name	= sizeof(indat_hex_name)/sizeof(indat_hex_name[0]);

	t_show_keyhlp(&keyhlp);
}

//=============================================================================
//  t_put_hlbox
//=============================================================================
static void t_put_indat(t_indat_t *indat)
{
	int		sx	 = indat->sx;
	int		sy	 = indat->sy;
	int		mw	 = indat->mw;
	int		mh	 = indat->mh;
	int		tlen = indat->tlen;
	int		hlen = indat->hlen;
	uint8_t	fg	 = indat->fg;
	uint8_t bg	 = indat->bg;
	int 	i, j;
	
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
			else if (j==sy+2 && tlen)
			{
				if (i==sx)				t_putc(0xc6);	// left-middle
				else if (i==sx+mw-1) 	t_putc(0xb5);	// right-middle
				else					t_putc(0xcd);	// --
	
			}
			// help seperator
			else if (j==sy+mh-3 && hlen)
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

	// shadow
	if (indat->shadow)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);
		
		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw, i);
	}
}

//=============================================================================
//  t_show_indat
//=============================================================================
uint8_t t_show_indat(t_indat_t *indat)
{
	int		sx = 0, sy = 0, ex = 0, ey = 0, ix = 0, iy = 0;
	int		mx, my, mw, mh;
	int		tlen, flen, dlen, ulen, hlen;
	int		i;

	uint16_t	key;
	uint8_t		sc, ac;
	uint8_t		align;

	uint8_t		ibuf[16];
	uint8_t		idx;
	uint8_t		keyin;
	uint8_t		dsz;
	uint32_t	idata = 0, idata_max = 0;
	uint8_t		bk;
	char		str[64];
	//ps2_key_t	ps2;

	bk = t_get_color();
	
	indat->err = 0;

	// string length
	tlen = (int)strlen(indat->title);
	flen = (int)strlen(indat->field);
	ulen = (int)strlen(indat->unit);
	hlen = (int)strlen(indat->help);


	// digits of data length
	//      byte   word   dword
	// dec  3      5       10  (unsigned)
	// dec  4      6       11  (signed)
	// hex  2      4       8
	dlen = 0;
	if (indat->mode & T_INDAT_HEX)
	{
		// hex
		switch (indat->mode & T_INDAT_SZ_MASK)
		{
			case 1:	dlen = 2;	break;
			case 2: dlen = 4;	break;
			case 4: dlen = 8;	break;
		}
	}
	else
	{
		// dec
		if (indat->mode & T_INDAT_SIGNED)
		{
			// dec + signed
			switch (indat->mode & T_INDAT_SZ_MASK)
			{
				case 1:	dlen = 4;	break;
				case 2: dlen = 6;	break;
				case 4: dlen = 11;	break;
			}
		}
		else
		{
			// dec + unsigned
			switch (indat->mode & T_INDAT_SZ_MASK)
			{
				case 1:	dlen = 3;	break;
				case 2: dlen = 5;	break;
				case 4: dlen = 10;	break;
			}
		}
	}

	// window width
	// +---------------------------+
	// |       <title>             |
	// +---------------------------+
	// | <field> ________ <unit>   |
	// +---------------------------+
	// |       <help>              |
	// +---------------------------+
	mw = dlen;
	if (flen)
		mw += (flen + 1);
	if (ulen)
		mw += (ulen + 1);

	if (tlen > mw)
		mw = tlen;
	if (hlen > mw)
		mw = hlen;

	mw += 8;	// 2*border, 6*space
	if (mw > 79)
	{
		indat->err = 0xFD;
		goto err_indat;
	}

	// height
	mh = 0;
	
	if (tlen)
		mh += 2;	// with title

	if (hlen)
		mh += 2;	// with help

	mh += (2 + 3);	// 2*top/bottom, 2*empty, 1*field
	
	if (mh > 24)
	{
		indat->err = 0xFC;
		goto err_indat;
	}


	// alignment
	align =indat->align;
	// 0----19----39----59----79
	if (align & T_AL_MASK_L)
		sx = 19 - (mw>>1);
	else if (align & T_AL_MASK_C)
		sx = 39 - (mw>>1);
	else if (align & T_AL_MASK_R)
		sx = 59 - (mw>>1);

	// 0----6----12----18----24
	if (align & T_AL_MASK_T)
		sy = 6 - (mh>>1);
	else if (align & T_AL_MASK_M)
		sy = 12 - (mh>>1);
	else if (align & T_AL_MASK_B)
		sy = 18 - (mh>>1);

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
	if (indat->shadow)
	{
		ex += 1;
		ey += 1;
	}

	t_copy_region(sx, sy, ex, ey);

	indat->sx = sx;
	indat->sy = sy;
	indat->mw = mw;
	indat->mh = mh;
	indat->tlen = tlen;
	indat->hlen = hlen;

	// output
	t_put_indat(indat);
	
	t_color(indat->fg, indat->bg);
	
	my = sy + 1;
	
	// title
	if (tlen)
	{
		t_xy_puts(sx+(mw-tlen)/2, my++, indat->title);
		my += 2;
	}
	else
	{
		// no title
		my += 1;
	}

	// field position
	if (flen && ulen == 0)
		mx = sx + (mw-flen-1-dlen)/2;
	else if (flen == 0 && ulen)
		mx = sx + (mw-dlen-1-ulen)/2;
	else if (flen == 0 && ulen == 0)
		mx = sx + (mw-dlen)/2;
	else
		mx = sx + (mw-flen-1-dlen-1-ulen)/2;

	// field
	if (flen)
	{
		t_xy_puts(mx, my, indat->field);
		mx += (flen + 1);
	}

	// input field
	ix = mx;
	iy = my;
	for (i=0; i<dlen; i++)
		t_xy_cl_putc(mx+i, my, indat->fi, indat->bi, ' ');

	// show default value
	if (indat->mode & T_INDAT_HEX)
	{
		sprintf(ibuf, "%lX", indat->data);
	}
	else
	{
		if (indat->mode & T_INDAT_SIGNED)
			sprintf(ibuf, "%d", indat->data);
		else
			sprintf(ibuf, "%lu", indat->data);
	}
	t_xy_cl_puts(ix, iy, indat->fi, indat->bi, ibuf);


	mx += (dlen + 1);
	
	// unit
	t_color(indat->fg, indat->bg);
	if (ulen)
		t_xy_puts(mx, my, indat->unit);

	// help
	if (hlen)
	{
		my += 3;
		t_xy_cl_puts(sx+(mw-hlen)/2, my, indat->fh, indat->bh, indat->help);
	}


	if (indat->keyhelp)
	{
		t_copy_region(0, 24, 79, 24);

		if (indat->mode & T_INDAT_HEX)
			t_show_indat_hex_keyhelp();
		else
			t_show_indat_dec_keyhelp();
	}

	keyin = 0;
	idx = 0;

	memset(ibuf, 0, sizeof(ibuf));

	// cursor
	video_set_cur_pos(ix, iy);
	video_show_cur();
	
	key_flush_buf();

	while (1)
	{
		key = (uint16_t)bioskey(1);
		if(key)
		{
			//key_blk_read(&ps2);
			
			sc = (key >> 8);//sc = ps2.sc;
			ac = key & 0xFF;//ac = ps2.ac;

			// hex, dec
			if (((indat->mode & T_INDAT_HEX) && 
				 (ac >= '0' && ac <= '9') || (ac >= 'A' && ac <= 'F') || (ac >= 'a' && ac <= 'f')) ||
				((indat->mode & T_INDAT_HEX)==0 && (ac >= '0' && ac <= '9')))
			{
				if (idx < dlen)	
				{
					if (ac >= 'a' && ac <= 'f')
						ac &= 0xDF;	// upper case

					// clear input field when 1st char is coming.
					if (idx == 0)
					{
						for (i=ix; i<ix+dlen; i++)
							t_xy_cl_putc(i, iy, indat->fi, indat->bi, ' ');
					}

					ibuf[idx++] = ac;
					t_xy_cl_puts(ix, iy, indat->fi, indat->bi, ibuf);

					video_set_cur_pos(ix+idx, iy);
				}
				else
				{
					// field full : clear
					#if 0
					for (i=ix; i<ix+dlen; i++)
						t_xy_cl_putc(i, iy, indat->fi, indat->bi, ' ');

					video_set_cur_pos(ix, iy);

					for(idx=0; idx<16; idx++)
						ibuf[idx] = 0;
					idx = 0;
					#endif
				}
				keyin = 1;
			}
			else if ((indat->mode & T_INDAT_HEX)==0 &&
					 (indat->mode & T_INDAT_SIGNED) &&
					 idx == 0 && ac == '-')
			{
				// minus symbol
				ibuf[idx++] = '-';
				t_xy_cl_puts(ix, iy, indat->fi, indat->bi, ibuf);

				video_set_cur_pos(ix+idx, iy);
			}
			else if (key == CHAR_BACKSPACE)
			{
				if (idx > 0)
				{
					ibuf[--idx] = 0;
					t_xy_cl_putc(ix+idx, iy, indat->fi, indat->bi, ' ');
					video_set_cur_pos(ix+idx, iy);
				}
			}
			else if (key == CHAR_CARRIAGE_RETURN)
			{
				if (keyin)
				{
					if (indat->mode & T_INDAT_HEX)
					{
						sscanf(ibuf, "%lX", &idata);
					}
					else
					{
						if (indat->mode & T_INDAT_SIGNED)
							sscanf(ibuf, "%d", &idata);
						else
							sscanf(ibuf, "%lu", &idata);
					}

					dsz = (indat->mode & T_INDAT_SZ_MASK);
					
					if (dsz == 1)
						idata_max = 255;
					else if (dsz == 2)
						idata_max = 65535;
					else
						idata_max = 4294967295;

					if (indat->mode & T_INDAT_MAX_MIN)
					{
						if (idata > indat->max)
						{
							video_hide_cur();
							sprintf(str, "Input data maximum : %d", indat->max);
							msgbox_waitkey(T_WHITE, T_RED, "Error", str, 4, 1);
							video_show_cur();

							goto clear_indat;
						}
						else if (idata < indat->min)
						{
							video_hide_cur();
							sprintf(str, "Input data minimum : %d", indat->min);
							msgbox_waitkey(T_WHITE, T_RED, "Error", str, 4, 1);
							video_show_cur();

							goto clear_indat;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (idata <= idata_max)
						{
							break;
						}
						else
						{
clear_indat:
							idx = 0;
							memset(ibuf, 0, sizeof(ibuf));
							for (i=ix; i<ix+dlen; i++)
								t_xy_cl_putc(i, iy, indat->fi, indat->bi, ' ');

							video_set_cur_pos(ix, iy);
						}
					}
				}
				else
				{
					idata = indat->data;
					break;
				}
			}
			else if (sc == SCAN_F10)
			{
				t_scr_capture();
			}
			else if (sc == SCAN_ESC)
			{
				//idata = indat->data;	// abort, restore data
				indat->err = 0xFA;	// ESC abort
				break;
			}
		}
	}

	video_hide_cur();

err_indat:

	// key help
	if (indat->keyhelp)
		t_paste_region(-1, -1);

	// indat
	t_paste_region(-1, -1);

	t_set_color(bk);

	if (indat->err)
	{
		return indat->err;
	}
	else
	{
		indat->data = idata;
		return 0;
	}
}

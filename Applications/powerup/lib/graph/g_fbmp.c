//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  GRAPH - G_FBMP : File BMP                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdlib.h>
#include <string.h>

#include "../image/bmp.h"
#include "g_fbmp.h"
#include "g_fn0816.h"
#include "g_fnvinl.h"
#include "g_fnvamd.h"

//=============================================================================
//  bmp_pal_text16
//=============================================================================
bmp_col_t	bmp_pal_text_16[16] =	
{
	// blue  green  red  alpha
	{ 0x00, 0x00, 0x00, 0x00 },		// 0 : black
	{ 0xB5, 0x00, 0x00, 0x00 },		// 1 : blue
	{ 0x00, 0xB5, 0x00, 0x00 },		// 2 : green
	{ 0xB5, 0xB5, 0x00, 0x00 },		// 3 : cyan
	{ 0x00, 0x00, 0xB5, 0x00 },		// 4 : red
	{ 0xB5, 0x00, 0xB5, 0x00 },		// 5 : magenta
	{ 0x00, 0xB5, 0xB5, 0x00 },		// 6 : brown
	{ 0xB5, 0xB5, 0xB5, 0x00 },		// 7 : lightgray
	{ 0x5B, 0x5B, 0x5B, 0x00 },		// 8 : darkgray
	{ 0xFF, 0x00, 0x00, 0x00 },		// 9 : light-blue
	{ 0x00, 0xFF, 0x00, 0x00 },		// 10: light-green
	{ 0xFF, 0xFF, 0x00, 0x00 },		// 11: light-cyan
	{ 0x00, 0x00, 0xFF, 0x00 },		// 12: light-red
	{ 0xFF, 0x00, 0xFF, 0x00 },		// 13: light-magenta
	{ 0x00, 0xFF, 0xFF, 0x00 },		// 14: yellow
	{ 0xFF, 0xFF, 0xFF, 0x00 }		// 15: white
};

//=============================================================================
//  bmp_pal_acl_16
//=============================================================================
bmp_col_t	bmp_pal_acl_16[16] =	
{
	// blue  green  red  alpha
	{ 0x00, 0x00, 0x00, 0x00 },		// 0 : black
	{ 0x80, 0x42, 0x00, 0x00 },		// 1 : blue : advantech blue
	{ 0x00, 0xB5, 0x00, 0x00 },		// 2 : green
	{ 0xB5, 0xB5, 0x00, 0x00 },		// 3 : cyan
	{ 0x00, 0x00, 0xB5, 0x00 },		// 4 : red
	{ 0xB5, 0x00, 0xB5, 0x00 },		// 5 : magenta
	{ 0x00, 0xB5, 0xB5, 0x00 },		// 6 : brown
	{ 0xB5, 0xB5, 0xB5, 0x00 },		// 7 : lightgray
	{ 0x5B, 0x5B, 0x5B, 0x00 },		// 8 : darkgray
	{ 0xFF, 0x00, 0x00, 0x00 },		// 9 : light-blue
	{ 0x00, 0xFF, 0x00, 0x00 },		// 10: light-green
	{ 0xFF, 0xFF, 0x00, 0x00 },		// 11: light-cyan
	{ 0x00, 0x00, 0xFF, 0x00 },		// 12: light-red
	{ 0xFF, 0x00, 0xFF, 0x00 },		// 13: light-magenta
	{ 0x00, 0xFF, 0xFF, 0x00 },		// 14: yellow
	{ 0xFF, 0xFF, 0xFF, 0x00 }		// 15: white
};

//=============================================================================
//  g_font_sun8x16
//=============================================================================
g_font_t g_font[3] =
{
	{ 8, 16, fontdata_sun8x16,	G_FN0816_SZ },
	{ 8, 16, fontdata_fnvinl,	G_FNVINL_SZ	},
	{ 8, 16, fontdata_fnvamd,	G_FNVAMD_SZ	}
};

//=============================================================================
//  g_fbmp_init
//=============================================================================
g_fbmp_t *g_fbmp_init(int width, int height, uint16_t bpp, bmp_col_t *pal, uint8_t fnt_idx)
{
	g_fbmp_t	*fb;

	if (!((bpp == 4) || (bpp == 8)))
		return NULL;

	// todo : w, h max size ?

	fb = (g_fbmp_t *)malloc(sizeof(g_fbmp_t));
	if (!fb)
		return NULL;

	fb->width	= width;
	fb->height	= height;
	fb->bpp		= bpp;
	fb->linesz	= ((bpp * (uint16_t)width + 31) >> 5) << 2;
	
	fb->imgsz	= (fb->width * fb->height * fb->bpp) >> 3;

	// img buf
	fb->img = (uint8_t *)malloc(sizeof(uint8_t) * fb->imgsz);
	if (!fb->img)
	{
		free(fb);
		return NULL;
	}

	// img buf clear
	memset(fb->img, 0, fb->imgsz);
	
	// pal : palette
	fb->pal.num	= (1 << fb->bpp);
	fb->pal.sz	= fb->pal.num * sizeof(bmp_col_t);
	fb->pal.col	= (bmp_col_t *)malloc(sizeof(bmp_col_t) * fb->pal.num);
	if (!fb->pal.col)
	{
		free(fb->img);
		free(fb);
		return NULL;
	}
	memcpy(fb->pal.col, pal, fb->pal.sz);

	// hdr : bmp header
	fb->hdr.tag[0]		= 'B';
	fb->hdr.tag[1]		= 'M';
	fb->hdr.fsz			= sizeof(bmp_hdr_t) + fb->pal.sz + fb->imgsz;
	fb->hdr.reserved	= 0;
	fb->hdr.pxoff		= sizeof(bmp_hdr_t) + fb->pal.sz;
	// hdr : bmp dib
	fb->hdr.hsz			= 40;	// bmp dib size
	fb->hdr.width		= width;
	fb->hdr.height		= height;
	fb->hdr.planes		= 1;
	fb->hdr.bpp			= bpp;
	fb->hdr.compress	= 0;	// non-compressed
	fb->hdr.isz			= fb->imgsz;
	fb->hdr.xpixmeter	= 0;
	fb->hdr.ypixmeter	= 0;
	fb->hdr.palcolors	= 0;
	fb->hdr.colorimportant	= 0;

	// font
	fb->fnt.sz		= g_font[fnt_idx].sz;
	fb->fnt.data	= g_font[fnt_idx].data;
	fb->fnt.width	= g_font[fnt_idx].width;
	fb->fnt.height	= g_font[fnt_idx].height;

	return fb;
}

//=============================================================================
//  g_fbmp_exit
//=============================================================================
void g_fbmp_exit(g_fbmp_t *fb)
{
	if (fb)
	{
		// img
		if (fb->img)
			free(fb->img);

		// pal
		if (fb->pal.col)
			free(fb->pal.col);
		
		free(fb);
	}
}

//=============================================================================
//  g_fbmp_flush
//=============================================================================
int g_fbmp_flush(g_fbmp_t *fb, char *fname)
{
	FILE	*fp;

	if (!fb)
		return -1;

	if (fb->pal.sz != 0)
	{
		if (!fb->pal.col)
			return -1;
	}

	if (!fb->img)
		return -1;

	fp = fopen(fname, "wb");
	if (!fp)
		return -1;

	fwrite(&fb->hdr, sizeof(bmp_hdr_t), 1, fp);

	if (fb->pal.sz != 0)
	{
		fwrite(fb->pal.col, sizeof(bmp_col_t), fb->pal.num, fp);
	}

	fwrite(fb->img, sizeof(uint8_t), fb->imgsz, fp);

	fflush(fp);
	fclose(fp);

	return 0;
}

//=============================================================================
//  g_fbmp_clear
//=============================================================================
int g_fbmp_clear(g_fbmp_t *fb)
{
	if (fb)
		memset(fb->img, 0, fb->imgsz);

	return 0;
}

//=============================================================================
//  g_fbmp_clear_color
//=============================================================================
int g_fbmp_clear_color(g_fbmp_t *fb, uint8_t color)
{
	uint8_t		data;

	data = color;
	data <<= 4;
	data |= color;

	if (fb)
		memset(fb->img, data, fb->imgsz);

	return 0;
}

//=============================================================================
//  g_fbmp_put_pixel
//=============================================================================
void g_fbmp_put_pixel(g_fbmp_t *fb, int x, int y, uint8_t color)
{
	uint8_t		*img = fb->img;
	uint8_t		px;

	img	+= ((fb->height - 1 - y) * fb->linesz + (x >> 1));

	px = *img;

	// 4-bpp
	if (x & 0x1)
	{
		px &= 0xF0;
	}
	else
	{
		px &= 0x0F;
		color <<= 4;
	}
	px |= color;

	*img = px;
}

//=============================================================================
//  g_fbmp_put_hline
//=============================================================================
int g_fbmp_put_hline(g_fbmp_t *fb, int x, int y, uint8_t color, int len)
{
	uint8_t		*img = fb->img;
	uint8_t		px;
	uint8_t		pxc, mbc, lbc;
	int			st, en, mid, hmid;
	int			i;
	
	if (len < 2)
		return 0;

	mbc = (color << 4);	// msb color @ lower  y-pos
	lbc = color & 0xF;	// lsb color @ higher y-pos
	pxc = (mbc | lbc);
	
	st = en = mid = hmid = 0;

	// x  len   st  en
	// ----------------
	// 0  1024  0   0
	// 0  1023  0   1
	// 1  1022  1   1
	// 1  1023  1   0
	// ----------------
	if (x & 0x1)
	{
		st = 1;	// odd
		if (len & 0x1)
			en = 1;
		else
			en = 0;
	}
	else
	{
		st = 0;
		if (len & 0x1)
			en = 1;
		else
			en = 0;
	}


	mid		= len - st - en;
	hmid	= mid >> 1;

	img	+= ((fb->height - 1 - y) * fb->linesz + (x >> 1));

	// head px
	if (st)
	{
		px = *img & 0xF0;
		px |= lbc;
		*img++ = px;
	}

	// mid px
	for (i=0; i<hmid; i++)
		*img++ = pxc;

	// en px
	if (en)
	{
		px = *img & 0x0F;
		px |= mbc;
		*img++ = px;
	}

	return 0;
}

//=============================================================================
//  g_fbmp_put_vline
//=============================================================================
int g_fbmp_put_vline(g_fbmp_t *fb, int x, int y, uint8_t color, int len)
{
	uint8_t		*img = fb->img;
	uint8_t		px;
	uint8_t		mbc, lbc;
	int			i;
	int			ylen;

	if (len < 2)
		return 0;

	mbc 	= (color << 4);	// msb color
	lbc		= color & 0xF;	// lsb color
	ylen	= y + len;

	img	+= (fb->height - 1 - y) * fb->linesz + (x >> 1);

	// 16-color, (0,0)->0xF0, (0,1)->0x0F
	if (x & 0x1)
	{
		for (i=y; i<ylen; i++)
		{
			px = *img & 0xF0;
			px |= lbc;
			*img = px;
			img -= fb->linesz;	// next line
		}
	}
	else
	{
		for (i=y; i<ylen; i++)
		{
			px = *img & 0x0F;
			px |= mbc;
			*img = px;
			img -= fb->linesz;	// next line
		}
	}

	return 0;
}

//=============================================================================
//  g_fbmp_put_char
//=============================================================================
void g_fbmp_put_char(g_fbmp_t *fb, int x, int y, uint8_t fg, uint8_t bg, char c)
{
	uint32_t	i;
	uint32_t	cidx;
	int			j, xx, yy;
	uint8_t		fontdata;

	cidx = (uint32_t)c;
	cidx <<= 4;			// *16, font size = 16 bytes

	for (i=cidx, yy=y; i<(cidx+16); i++, yy++)
	{
		//fontdata = fontdata_sun8x16[i];
		fontdata = fb->fnt.data[i];
	
		for (j=7, xx=x; j>=0; j--, xx++)
		{
			if ((fontdata>>j) & 0x1)
				g_fbmp_put_pixel(fb, xx, yy, fg);	// fg : bit=1
			else
				g_fbmp_put_pixel(fb, xx, yy, bg);	// bg : bit=0
		}
	
	}
}

//=============================================================================
//  g_fbmp_put_str
//=============================================================================
void g_fbmp_put_str(g_fbmp_t *fb, int x, int y, uint8_t fg, uint8_t bg, char *s)
{
	int		len;
	int		i;
	int		xx;
	char	*str = s;

	len = (int)strlen(s);

	// font width ?
	for (i=0, xx=x; i<len; i++, xx+=8)
	{
		g_fbmp_put_char(fb, xx, y, fg, bg, *str++);
	}
}

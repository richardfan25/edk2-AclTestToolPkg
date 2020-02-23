//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  GRAPH - G_FBMP : File BMP                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __GRAPH_G_FBMP_H
#define __GRAPH_G_FBMP_H


#include "typedef.h"
#include "../image/bmp.h"

//=============================================================================
//  g_font
//=============================================================================
#define G_FONT_SUN8X16		0
#define G_FONT_VBIOS_INTEL	1
#define G_FONT_VBIOS_AMD	2

//=============================================================================
//  g_font_t
//=============================================================================
typedef struct _g_font_t
{
	uint8_t		width;
	uint8_t		height;

	uint8_t		*data;
	uint32_t	sz;

} g_font_t;

//=============================================================================
//  g_fbmp_t
//=============================================================================
typedef struct _g_fbmp_t
{
	uint8_t		*img;
	uint32_t	imgsz;

	bmp_hdr_t	hdr;
	bmp_pal_t	pal;

	int			width;
	int			height;
	uint16_t	bpp;
	uint16_t	linesz;

	g_font_t	fnt;

} g_fbmp_t;

//=============================================================================
//  extern
//=============================================================================
extern bmp_col_t	bmp_pal_text_16[16];
extern bmp_col_t	bmp_pal_acl_16[16];

//=============================================================================
//  functions
//=============================================================================
//g_fbmp_t*	g_fbmp_init(int width, int height, uint16_t bpp, bmp_col_t *pal);
//g_fbmp_t *g_fbmp_init(int width, int height, uint16_t bpp, bmp_col_t *pal, g_font_t *fnt);
g_fbmp_t *g_fbmp_init(int width, int height, uint16_t bpp, bmp_col_t *pal, uint8_t fnt_idx);

void		g_fbmp_exit(g_fbmp_t *fb);
int			g_fbmp_flush(g_fbmp_t *fb, char *fname);
int			g_fbmp_clear(g_fbmp_t *fb);
int		g_fbmp_clear_color(g_fbmp_t *fb, uint8_t color);

void	g_fbmp_put_pixel(g_fbmp_t *fb, int x, int y, uint8_t color);
int		g_fbmp_put_hline(g_fbmp_t *fb, int x, int y, uint8_t color, int len);
int		g_fbmp_put_vline(g_fbmp_t *fb, int x, int y, uint8_t color, int len);
void	g_fbmp_put_char(g_fbmp_t *fb, int x, int y, uint8_t fg, uint8_t bg, char c);
void	g_fbmp_put_str(g_fbmp_t *fb, int x, int y, uint8_t fg, uint8_t bg, char *s);

#endif

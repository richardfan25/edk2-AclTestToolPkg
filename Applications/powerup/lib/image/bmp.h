//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  IMAGE - BMP (BitMap)                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//  BMP header  : 54 bytes

//  BMP palette : depends on bpp
//  - RGB mode (without palette)
//  - index mode
//    8 bpp = 256 colors = 256 * 4 = 1024 bytes
//    4 bpp =  16 colors =  16 * 4 =   64 bytes
//    1 bpp =   2 colors =   2 * 4 =    8 bytes

//  BMP image data
//  - Size of scan line must be 4 bytes alignment
//  - uncompress BMP format (BMP compressed : RLE)
//  - The image data layout : (assume image height = N)
//    (scan line N-1)
//    (scan line N-2)
//    (scan line N-3)
//    ..............
//    (scan line 2)
//    (scan line 1)
//    (scan line 0)
//
//*****************************************************************************
#ifndef __IMAGE_BMP_H
#define __IMAGE_BMP_H

#include <stdio.h>

#include "typedef.h"

//=============================================================================
//  BMP : bitmap info
//=============================================================================
// bpp :
//     = 24 : true color
//     = 16 : high color (RGB565, RGB555 not defined in spec)
//     =  8 : 256 colors
//     =  4 : 16 colors
//     =  1 : 2 colors
//
// compress :
//     =  0  BI_RGB  : non-compressed
//     =  1  BI_RLE8 : RLE 8bit/px
//     =  2  BI_RLE4 : RLE 4bit/px
//     =  3  BI_BITFIELDS : RLE 8bit/px
//     =  4  BI_JPEG : RLE 8bit/px
//     =  5  BI_PNG  : PNG
//     =  6  BI_ALPHABITFILEDS : bit-field
//=============================================================================
//  define
//=============================================================================
#define BMP_HDR_SZ			54			// header size
#define	BMP_HDR_OFF			0x00		// header 
#define BMP_PAL_OFF			BMP_HDR_SZ	// header...palette...pixel data

//=============================================================================
//  error code
//=============================================================================
#define	BMP_OK				0
#define BMP_ERR_INST		1
#define BMP_ERR_BUF			2
#define BMP_ERR_TAG  		3
#define BMP_ERR_FILE_SIZE	4
#define BMP_ERR_COMPRESS	5
#define BMP_ERR_BPP			6
#define BMP_ERR_PAL			7

#pragma pack(1)
//=============================================================================
//  bmp_hdr_t : BMP header
//=============================================================================
typedef struct _bmp_hdr_t
{
	uint8_t		tag[2];			// 00 : "BM"
	uint32_t	fsz;			// 02 : file byte size
	uint32_t	reserved;		// 06 : n/a
	uint32_t	pxoff;			// 0a : pixel offset in file

	uint32_t	hsz;			// 0e : DIB header size, 40 bytes
	uint32_t	width;			// 12 : image width  (px)
	uint32_t	height;			// 16 : image height (px)
	uint16_t	planes;			// 1a : num of plane, = 1
	uint16_t	bpp;			// 1c : bit per pixel ()
	uint32_t	compress;		// 1e : compressed method
	uint32_t	isz;			// 22 : image size (byte)
	int32_t		xpixmeter;		// 26 : horizontal res. (px/meter)
	int32_t		ypixmeter;		// 2a : vertical res. (px/meter)
	uint32_t	palcolors;		// 2e : num of colors in palette
	uint32_t	colorimportant;	// 32 : num of important color in used, generally ignored

} bmp_hdr_t;	// sizeof = 54

//=============================================================================
//  bmp_col_t : BMP color
//=============================================================================
typedef struct _bmp_col_t
{
	uint8_t	b;	// blue
	uint8_t	g;	// green
	uint8_t	r;	// red
	uint8_t	a;	// alpha

} bmp_col_t;

//=============================================================================
//  bmp_pal_t : BMP palette
//=============================================================================
typedef struct _bmp_pal_t
{
	bmp_col_t	*col;

	uint16_t	num;
	uint16_t	sz;	// palette size : 4*N (N-colors)

} bmp_pal_t;

//=============================================================================
//  bmp_file_t
//=============================================================================
typedef struct _bmp_file_t
{
	uint8_t		*buf;
	
	bmp_hdr_t	*hdr;
	bmp_pal_t	*pal;

	uint32_t	xsz;	// pixel size
	uint32_t	psz;	// pal size
	uint32_t	rsz;	// row size
	uint32_t	fsz;	// file size

} bmp_file_t;
#pragma pack()

//=============================================================================
//  functions
//=============================================================================
bmp_file_t	*bmp_init(char *fname, uint8_t *res);
void		bmp_exit(bmp_file_t *bmp);

#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  IMAGE - BMP (BitMap)                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdlib.h>
#include <sys\stat.h>

#include "bmp.h"
#include "file.h"

//=============================================================================
//  bmp_init
//=============================================================================
int bmp_init(char *fname, bmp_file_t *bf)
{
	struct stat	st;
	bmp_hdr_t	*hdr;
	bmp_dib_t	*dib;

	int			res, res2;
	uint32_t	data32;
	uint32_t	palsz;

	if (stat(fname, &st))
	{
		res = BMP_ERR_STAT;
		goto err_bmp_init;
	}

	// file info
	bf = (bmp_file_t *)malloc(sizeof(bmp_file_t));
	if (!bf)
	{
		res = BMP_ERR_MALLOC_FILE;
		goto err_bmp_init;
	}

	memset(bf, 0, sizeof(bmp_file_t));

	// file open
	bf->fp = fopen(fname, "rb");
	if (!bf->fp)
	{
		res = BMP_ERR_FILE_OPEN;
		goto err_bmp_init;
	}
	
	// hdr : reading
	res2 = file_read_blk_malloc(bf->fp, BMP_OFF_HDR, (uint8_t*)bf->hdr, sizeof(bmp_hdr_t));
	if (res2 == FILE_ERR_MALLOC)
	{
		res = BMP_ERR_HDR_MALLOC;
		goto err_bmp_init;
	}
	else if (res2 == FILE_ERR_READ)
	{
		res = BMP_ERR_HDR_READ;
		goto err_bmp_init;
	}

	hdr = bf->hdr;

	// dib : reading size of dib
	if (file_read_blk(bf->fp, BMP_OFF_DIB, (uint8_t*)&data32, sizeof(uint32_t)) != FILE_OK)
	{
		res = BMP_ERR_DIB_READ;
		goto err_bmp_init;
	}

	// dib : size not equal 40
	if (data32 != BMP_SZ_DIB)
	{
		res = BMP_ERR_DIB_SZ;
		goto err_bmp_init;
	}

	// dib : reading
	res2 = file_read_blk_malloc(bf->fp, BMP_OFF_DIB, (uint8_t*)bf->dib, sizeof(bmp_dib_t));
	if (res2 == FILE_ERR_MALLOC)
	{
		res = BMP_ERR_DIB_MALLOC;
		goto err_bmp_init;
	}
	else if (res2 == FILE_ERR_READ)
	{
		res = BMP_ERR_DIB_READ;
		goto err_bmp_init;
	}


	// checking : file size
	if ((uint32_t)hdr->fsz != (uint32_t)st.st_size)
	{
		res = BMP_ERR_FILE_SIZE;
		goto err_bmp_init;
	}

	// checking : BMP tag
	if (!((hdr->tag[0] == 'B') && (hdr->tag[1] == 'M')))
	{
		res = BMP_ERR_TAG;
		goto err_bmp_init;
	}

	// checking : compress
	if (hdr->compress)
	{
		res = BMP_ERR_COMPRESS;	// can't support compressed BMP
		goto err_bmp_init;
	}

	// checking : bpp
	if (!(hdr->bpp==1 || hdr->bpp==4 || hdr->bpp==8 || hdr->bpp==16 || hdr->bpp==24 || hdr->bpp==32))
	{
		res = BMP_ERR_BPP;
		goto err_bmp_init;
	}

	// checking : image width : ?
	if (hdr->width > 2048)
	{
		res = BMP_ERR_IMG_WIDTH;
		goto err_bmp_init;
	}

	// checking : palette
	if (hdr->bpp == 8 || hdr->bpp == 4 || hdr->bpp == 1)
	{
		// no palette
		if (hdr->pxoff == BMP_OFF_PX)
		{
			res = BMP_ERR_PAL;
			goto err_bmp_init;
		}

		// palette size err
		bf->palsz = hdr->pxoff - BMP_OFF_PX;
		if (bf->palsz != (1<<(hdr->bpp+2)))
		{
			res = BMP_ERR_PAL_SZ;
			goto err_bmp_init;
		}

		// palette : reading
		res2 = file_read_blk_malloc(bf->fp, BMP_OFF_PAL, (uint8_t*)bf->pal, (size_t)bf->palsz);
		if (res2 == FILE_ERR_MALLOC)
		{
			res = BMP_ERR_PAL_MALLOC;
			goto err_bmp_init;
		}
		else if (res2 == FILE_ERR_READ)
		{
			res = BMP_ERR_PAL_READ;
			goto err_bmp_init;
		}
	
	}

	return BMP_OK;

err_bmp_init:

	if (bf->dib)
		free(bf->dib);

	if (bf->hdr)
		free(bf->hdr);

	if (bf)
		free(bf);

	if (bf->fp)
		fclose(bf->fp);

	return res;
}



//===========================================================================
//  show_bmp_file
//===========================================================================
int show_bmp_file(char *filename, int x, int y, int cw, int ch)
{
	FILE	*fp;
	struct	bmp_header	bh;
	u8		bmp_palette[1024];

	u32		scanline_sz;
	u32		off;
	u32		*px32;
	u16		*px16;
	u8		*scanline_buf;
	u8		*px8;
	u32		color;
	u16		pr, pb, pg;
	int		i, j, k, px_byte;
	int		px;

	img_sx = x;
	img_sy = y;

	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "fopen %s err!\n", filename);
		return -1;
	}

	// BMP header
	rewind(fp);
	fread(&bh, 1, sizeof(struct bmp_header), fp);

	// BMP file check : file size
	fseek(fp, 0L, SEEK_END);
	if ((u32)bh.fsize != ftell(fp)) {
		fprintf(stderr, "%s file size error, %lu != %lu\n", bh.fsize, ftell(fp));
		fclose(fp);
		return -1;
	}
	// BMP file check : tag
	if (bh.type[0] != 'B' || bh.type[1] != 'M') {
		fprintf(stderr, "%s is not BMP file\n", filename);
		fclose(fp);
		return -1;
	}
	// BMP file check : compress
	if (bh.bitcount != 16) {	// don't check compress if bpp = 16
		if (bh.compress != 0) {
			fprintf(stderr, "Can not support compressed BMP : %s\n", filename);
			fclose(fp);
			return -1;
		}
	}
	// BMP file check : bpp
	if (!(bh.bitcount == 1 || bh.bitcount == 4 || bh.bitcount == 8 || bh.bitcount == 16 || bh.bitcount == 24 || bh.bitcount == 32)) {
		fprintf(stderr, "Can not support %s bpp : %s\n", bh.bitcount, filename);
		fclose(fp);
		return -1;
	}
	// BMP file check : image width
	if (bh.width > 2048) {
		fprintf(stderr, "%s BMP image width is too big, should be less than 2048. width = %lu\n",
				filename, bh.width);
		fclose(fp);
		return -1;
	}

	if (bh.bitcount == 8 || bh.bitcount == 4 ||	bh.bitcount == 1) {
		fseek(fp, 54L, SEEK_SET);
		if (bh.colorimportant == 0)
			fread(bmp_palette, 256*4, sizeof(u8), fp);
		else
			fread(bmp_palette, bh.colorimportant*4, sizeof(u8), fp);

		for (j=0, i=0; j<(1<<bh.bitcount); j++) {
			img_palette[j] = bmp_palette[i+2];	// red
			img_palette[j] <<= 8;
			img_palette[j] |= bmp_palette[i+1];	// green
			img_palette[j] <<= 8;
			img_palette[j] |= bmp_palette[i];	// blue
			i += 4;
		}
	}

	// point to image data
	fseek(fp, bh.offbits, SEEK_SET);
	off = bh.offbits;

	// scanline_sz = (bpp * width + 31) / 32 * 4;
	scanline_sz = (((bh.bitcount * bh.width + 31) >> 5) << 2);

	scanline_buf = (u8*)malloc(scanline_sz);
	if (!scanline_buf) {
		fprintf(stderr, "malloc err! size=%lu\n", scanline_sz);
		fclose(fp);
		return -1;
	}

	// init video before calling this function
	// video_init( vmode );

	// output video mode : 24bpp
	switch(bh.bitcount) {
		case 32:	// True-color with alpha channel (RGBA8888)
		case 24:	// True-color (RGB888)
			px_byte = (bh.bitcount >> 3);
			for (i=ch-1; i>=0; i--) {
				fseek(fp, off, SEEK_SET);
				fread(scanline_buf, scanline_sz, sizeof(u8), fp);
				for (j=0, k=0; j<cw; j++) {
					px32 = (u32 *)&scanline_buf[ k ];

					put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, (*px32 & 0xFFFFFF));

					// 3 or 4 bytes/pixel
					k += px_byte;	// 3=24bpp, 4=32bpp
				}
				off += scanline_sz;
			}

			break;

		case 16:	// Hi-color (RGB565)
			px_byte = (bh.bitcount >> 3);
			for (i=ch-1; i>=0; i--) {
				fseek(fp, off, SEEK_SET);
				fread(scanline_buf, scanline_sz, sizeof(u8), fp);

				for (j=0, k=0; j<cw; j++) {
					px16 = (u16 *)&scanline_buf[k];
					pr = (((*px16>>11) & 0x1F)<<3);
					pg = (((*px16>>5) & 0x3F)<<2);
					pb = ((*px16 & 0x1F)<<3);
					// RGB565 : 11, 5
					color = pr;
					color <<= 8;
					color |= pg;
					color <<= 8;
					color |= pb;

					put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, (color&0xFFFFFF) );

					k += px_byte;	// 2=16bpp
				}
				off += scanline_sz;
			}
			break;

		
		case 8:	// 256-color : 
			//for (i=0; i<bh.height; i++, --y) {
			//for (i=bh.height-1; i>=0; i--) {
			for (i=ch-1; i>=0; i--) {
				fseek(fp, off, SEEK_SET);
				fread(scanline_buf, scanline_sz, sizeof(u8), fp);
				px8 = scanline_buf;
				for (j=0, k=0; j<cw; j++) {
					// 1 byte/pixel
					put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, img_palette[ *px8++ ]);
				}
				off += scanline_sz;
			}
			break;

		// 16-color
		case 4:
			for (i=ch-1; i>=0; i--) {
				fseek(fp, off, SEEK_SET);
				fread(scanline_buf, scanline_sz, sizeof(u8), fp);
				px8 = scanline_buf;
				for (j=0; j<cw; j++) {
					// 4 bits/pixel
					// px = (*px8 >> 4) & 0xF
					// px = (*px8 & 0xF)
					if (j&1) {
						put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, img_palette[ (px&0xF) ]);
					} else {
						px = *px8++;
						put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, img_palette[ ((px>>4)&0xF) ]);
					}
				}
				off += scanline_sz;
			}
			break;

		case 1:	// 2-color
			for (i=ch-1; i>=0; i--) {
				fseek(fp, off, SEEK_SET);
				fread(scanline_buf, scanline_sz, sizeof(u8), fp);
				px8 = scanline_buf;

				for (j=0; j<cw; j++) {
					// 1 bit/pixel
					// px = (*px8 >> 7) & 0x1, (*px8 >> 6) & 0x1.... (*px8 & 0x1)
					k = 7 - (j & 7);
					if (k == 7)
						px = *px8++;
					put_pixel_24bpp((u32)j+img_sx, (u32)i+img_sy, img_palette[ ((px>>k)&1) ]);
				}
				off += scanline_sz;
			}

			break;
	}

	if (scanline_buf)
		free(scanline_buf);

	fclose(fp);

	return 0;
}

#endif
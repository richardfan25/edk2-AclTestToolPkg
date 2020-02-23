//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - FILE                                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "file.h"

#if 0
//===========================================================================
//  file_read_blk
//===========================================================================
int file_read_blk(FILE *fp, uint32_t fpos, uint8_t *blk, uint32_t blksz)
{
	uint32_t	rsz;

	fseek(fp, (long)fpos, SEEK_SET);
	rsz = fread(blk, sizeof(uint8_t), blksz, fp);

	if (rsz != blksz)
		return FILE_ERR_READ_BLK;

	return FILE_OK;
}

//===========================================================================
//  file_read_blk_malloc
//===========================================================================
int file_read_blk_malloc(FILE *fp, uint32_t fpos, uint8_t *blk, uint32_t blksz)
{
	uint32_t	rsz;

	blk = (uint8_t *)malloc(sizeof(uint8_t) * blksz);
	if (!buf)
		return FILE_ERR_BLK;

	fseek(fp, (long)fpos, SEEK_SET);
	rsz = fread(blk, sizeof(uint8_t), blksz, fp);

	if (rsz != blksz)
	{
		free(blk);
		return FILE_ERR_READ_BLK;
	}

	return FILE_OK;
}
#endif

//===========================================================================
//  file_read_to_buf
//===========================================================================
file_t *file_read_to_buf(char *fname, uint8_t *res)
{
	FILE		*fp;
	uint32_t	len;
	file_t		*f;

	f = (file_t *)malloc(sizeof(file_t));
	if (!f)
	{
		*res = FILE_ERR_ENT;
		return NULL;
	}

	fp = fopen(fname, "rb");
	if (!fp)
	{
		*res = FILE_ERR_OPEN;
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	f->sz = ftell(fp);

	f->buf = (uint8_t *)malloc(sizeof(uint8_t) * f->sz);
	if (!f->buf)
	{
		fclose(fp);
		*res = FILE_ERR_BUF;
		return NULL;
	}

	rewind(fp);
	fread(f->buf, sizeof(uint8_t), f->sz, fp);
	fclose(fp);

	len = (uint32_t)strlen(fname);
	f->name = (uint8_t *)malloc(sizeof(uint8_t) * (len + 1));
	if (!f->name)
	{
		free(f->buf);
		*res = FILE_ERR_NAME;
		return NULL;
	}

	sprintf(f->name, "%s", fname);

	*res = FILE_OK;
	return f;
}

//=============================================================================
//  buf_write_to_file
//=============================================================================
uint8_t buf_write_to_file(uint8_t *buf, uint32_t buflen, char *fname)
{
	FILE	*fp;

	fp = fopen(fname, "wb");
	if (fp)
	{
		fwrite(buf, sizeof(uint8_t), buflen, fp);
		fclose(fp);
		free(buf);
	}
	else
	{
		//free(buf);
		return FILE_ERR_OPEN;
	}

	return FILE_OK;
}

//===========================================================================
//  file_exit
//===========================================================================
uint8_t file_exit(file_t *f)
{
	if (f)
	{
		if (f->name)
			free(f->name);

		if (f->buf)
			free(f->buf);

		free(f);
	}
	else
	{
		return FILE_ERR_EXIT;
	}

	return FILE_OK;
}

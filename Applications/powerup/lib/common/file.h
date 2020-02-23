//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - FILE                                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __COMMON_FILE_H
#define __COMMON_FILE_H

#include "typedef.h"

//=============================================================================
//  error code
//=============================================================================
#define	FILE_OK				0
#define FILE_ERR_ENT		1
#define FILE_ERR_OPEN		2
#define FILE_ERR_BUF		3
#define FILE_ERR_NAME		4
#define FILE_ERR_EXIT		5
#define FILE_ERR_BLK		6
#define FILE_ERR_READ_BLK	7

//===========================================================================
//  FILE
//===========================================================================
typedef struct _file_t
{
	char		*name;
	uint8_t		*buf;
	uint32_t	sz;

} file_t;

//=============================================================================
//  functions
//=============================================================================
int		file_read_blk(FILE *fp, uint32_t fpos, uint8_t *blk, uint32_t blksz);
int		file_read_blk_malloc(FILE *fp, uint32_t fpos, uint8_t *blk, uint32_t blksz);

//uint8_t file_read_to_buf(char *fname, file_t *f);
file_t *file_read_to_buf(char *fname, uint8_t *res);

uint8_t buf_write_to_file(uint8_t *buf, uint32_t buflen, char *fname);
uint8_t file_exit(file_t *f);

#endif

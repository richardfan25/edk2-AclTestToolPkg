//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  DATLST - DAT File List                                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __DATLST_H
#define __DATLST_H

#include "typedef.h"

//=============================================================================
//  dat_finfo_t
//=============================================================================
typedef struct _dat_finfo_t
{
	char		name[16];	// file name + ext_name
	uint32_t	num;		// num : record number, depends on file type
	uint32_t	size;		// file size in bytes
	time_t		mtime;		// modified time

	struct _dat_finfo_t	*prev;
	struct _dat_finfo_t	*next;

} dat_finfo_t;

//=============================================================================
//  dat_flst_t
//=============================================================================
typedef struct _dat_flst_t
{
	uint32_t	num;

	dat_finfo_t	*head;
	dat_finfo_t	*curr;

} dat_flst_t;

//=============================================================================
//  functions
//=============================================================================
int datlst_show(char *file_sel, char *filter);

#endif

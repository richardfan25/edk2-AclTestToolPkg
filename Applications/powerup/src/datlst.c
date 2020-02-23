//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  DATLST - DAT File List                                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>
#include <time.h>

#include "typedef.h"
#include "t_video.h"
#include "t_scrcap.h"
#include "datlst.h"
#include "key.h"
#include "dat.h"
#include "log.h"
#include "pu.h"

//=============================================================================
//  variables
//=============================================================================
dat_flst_t	dflst;

//=============================================================================
//  dat_add_finfo
//=============================================================================
dat_finfo_t *dat_add_info(void)
{
	dat_finfo_t		*df;

	df = (dat_finfo_t *)malloc(sizeof(dat_finfo_t));
	if (!df)
		return NULL;

	memset(df, 0, sizeof(dat_finfo_t));

	return df;
}

//=============================================================================
//  dat_del_info
//=============================================================================
void dat_del_info(void)
{
	dat_finfo_t		*df, *d;

	int		i;

	if (dflst.num)
	{
		df = dflst.head;
		for (i=0; (uint32_t)i<dflst.num; i++)
		{
			d = df->next;
			free(df);

			df = d;
		}
	}
}

//===========================================================================
//  dat_sort_inc_name
//===========================================================================
int dat_sort_inc_name(const void *a, const void *b)
{
	dat_finfo_t	*ia = (dat_finfo_t *)a;
	dat_finfo_t	*ib = (dat_finfo_t *)b;

	return strcmp(ia->name, ib->name);
}

//=============================================================================
//  dat_sort_info
//=============================================================================
void dat_sort_info(void)
{
	int		i;
	
	dat_finfo_t	*dfi;
	dat_finfo_t	*dftmp;

	dftmp = (dat_finfo_t *)malloc(sizeof(dat_finfo_t)*dflst.num );
	if (!dftmp)
		return;

	dfi = dflst.head;
	
	for (i=0; (uint32_t)i<dflst.num; i++)
	{
		memcpy(&dftmp[i], dfi, sizeof(dat_finfo_t));
		dfi = dfi->next;
	}
	
	dfi = dftmp;
	qsort(dfi, dflst.num, sizeof(dat_finfo_t), dat_sort_inc_name);

	dfi = dflst.head;
	for (i=0; (uint32_t)i<dflst.num; i++)
	{
		memcpy(dfi, &dftmp[i], 28);	// exclude prev/next pointer
		dfi = dfi->next;
	}

	free(dftmp);
}
	
//=============================================================================
//  datlst_show
//=============================================================================
int datlst_show(char *file_sel, char *filter)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = NULL;//struct _finddata_t  fi;

	EFI_FILE_PROTOCOL *RootFS, *FileHandle;//long	hdl;
	EFI_DEVICE_PATH_PROTOCOL *dpath = NULL;// int		rc;
	EFI_HANDLE *dhandle = NULL;
	EFI_FILE_INFO *FileInfo;
	EFI_STATUS status;
	UINTN info_size, handlesize = 0;
	CONST CHAR16 *path;
	CHAR16 *tmp = NULL, *name = NULL;
	
	struct tm	*utc;

	int		sx, sy, mw, mh;
	uint8_t	fg, bg, fm;
	char		title[64];

	int		page_no, page_no_max;
	int		page_idx, page_idx_max;
	char		item[128];
	uint16_t	dirty, key;
	int		i, ret;
	uint8_t	filter_dat = 0;
	uint16_t	reccnt = 0;

	dat_finfo_t	*dfi, *dfj = NULL;

	// init
	memset(&dflst, 0, sizeof(dat_flst_t));
	
	status = gBS->LocateHandle(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handlesize, dhandle);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		dhandle = AllocateZeroPool(handlesize);
		status = gBS->LocateHandle(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handlesize, dhandle);
	}
	if(EFI_ERROR(status))
	{
		return -3;
	}
	
	for(i=0; i<(handlesize / sizeof(EFI_HANDLE)); i++)
	{
		dpath = DevicePathFromHandle(dhandle[i]);
		
		if(gEfiShellProtocol != NULL)
		{
			name = gEfiShellProtocol->GetFilePathFromDevicePath((CONST EFI_DEVICE_PATH_PROTOCOL *)dpath);
		}
		else if(mEfiShellEnvironment2 != NULL)
		{
			mEfiShellEnvironment2->GetFsName(dpath, FALSE, &name);
		}
		
		if(StrStr(ShellGetCurrentDir(NULL), name))
		{
			status = gBS->HandleProtocol(dhandle[i], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&SimpleFileSystem);
			FreePool(dhandle);
			if(EFI_ERROR(status))
			{
				return -4;
			}
			break;
		}
	}
	
	status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &RootFS);
	if(EFI_ERROR(status))
	{
		return -5;
	}

	// borrow item for strstr() due to modifying pointer of filter
	sprintf(item, "%s", filter);
	if (strstr(item, ".DAT"))
		filter_dat = 1;

	path = ShellGetCurrentDir(NULL);//hdl = _findfirst("*.DAT", &fi);
	if(path && StrStr(path, L":"))
	{
		tmp = StrStr(path, L":");
		tmp++;
		if(*tmp == L'\\')//hdl = _findfirst(filter, &fi);
		{
			tmp = StrStr(path, L"\\");
		}
		else if(*tmp == '\0')
		{
			tmp = L"\\";
		}
	}
	status = RootFS->Open(RootFS, &FileHandle, tmp, EFI_FILE_MODE_READ, 0);
	
	FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &info_size, NULL);
	gBS->AllocatePool(EfiBootServicesData, info_size, &FileInfo);
	
	//rc = hdl;
	while(1)//(rc != -1) 
	{
		//get fileinfo
		status = FileHandle->Read(FileHandle, &info_size, FileInfo);
		if(status == EFI_BUFFER_TOO_SMALL)
		{
			FreePool(FileInfo);
			gBS->AllocatePool(EfiBootServicesData, info_size, &FileInfo);
			status = FileHandle->Read(FileHandle, &info_size, FileInfo);
		}
		if(EFI_ERROR(status))
		{
			return -6;
		}

		//if end of list, info size is 0
		if(info_size == 0)
		{
			break;
		}

		UnicodeStrToAsciiStr(FileInfo->FileName, item);
		if(strstr(item, strstr(filter, ".")) == NULL)
		{
			goto datlst_find_next;
		}
		
		// filter : no < 1000
		if (dflst.num >= 1000)
			break;

		// filter : size < 9999999 = 9765.6 KiB = 9.536 MiB
		if (FileInfo->FileSize > 9999999)
			goto datlst_find_next;

		if (filter_dat)
		{
			// filter : size > 25052 (416 + 24*1024)
			if (FileInfo->FileSize < 25052)
				goto datlst_find_next;
			
			// filter : records : reading : todo.....
			if (dat_verify(item, &reccnt) < 0)
				goto datlst_find_next;
		}

		dfi = dat_add_info();
		if (!dfi)
			break;	// todo ?

		sprintf(dfi->name, "%s", item);	// name
		dfi->size	= (uint32_t)FileInfo->FileSize;	// size
		dfi->num	= (uint32_t)reccnt;
		dfi->mtime = Efi2Time(&FileInfo->ModificationTime);//fi.time_write;
		
		//if (dflst.head == NULL)
		//	dflst.head = dfi;
		
		if (dflst.num == 0)
		{
			dflst.head = dfi;	// 1st item
		}
		else
		{
			dfj->next = dfi;	// 2nd~ item
			dfi->prev = dfj;
		}
		dfj = dfi;
		dflst.num++;

datlst_find_next:

		NULL;//rc = _findnext(hdl, &fi);
	
		//if (rc == -1)
		//	break;
	}
	FreePool(FileInfo);
	FileHandle->Close(FileHandle);//_findclose(hdl);
	RootFS->Close(RootFS);

	if (dflst.num == 0)
	{
		return -2;
	}

	// sorting
	dat_sort_info();


	// 0         1         2         3         4         5         6         7
	// 01234567890123456789012345678901234567890123456789012345678901234567890
	// | No   Name          Records  Size     Date        Time     |
	// | xxx  FILENAME.EXT  xxxxx    xxxxxxx  YYYY/MM/DD  hh:mm:ss |

	mw = 61;
	mh = 2 + 2 + 1 + 16;	// 2,2:border, title, 1:index, 16:item
	
	sx = (80 - mw) / 2;
	sy = (25 - mh) / 2;

	fg = T_BLACK;
	bg = T_LIGHTGRAY;
	fm = T_GREEN;		// active bg

	// save screen
	t_copy_region(sx, sy, sx+mw, sy+mh);

	// draw
	t_color(fg, bg);
	t_put_win_single_double(sx, sy, mw, mh);

	// shadow
	for (i=sx+1; i<=sx+mw; i++)
		t_shadow_char(i, sy+mh);	// shadow : bottom border

	for (i=sy+1; i<=sy+mh; i++)
		t_shadow_char(sx+mw+0, i);	// shadow : right border

	// title
	sprintf(title, "%s Files : %d", filter, dflst.num);
		
	t_xy_puts(sx+(mw-(int)strlen(title))/2, sy+1, title);

	// index
	t_xy_puts(sx+3, sy+3, "No          Name    Rec       Size  Date        Time");

	// item
	page_no = 0;
	page_no_max = ((dflst.num + 15)) / 16 - 1;	// 16 items/page
	page_idx = 0;

	dfi = dflst.head;
	dirty = 1;

	// 0         1         2         3         4         5         6         7
	// 01234567890123456789012345678901234567890123456789012345678901234567890
	// |  No          Name    Rec       Size  Date        Time     |
	// | xxx  FILENAME.EXT  xxxxx    xxxxxxx  YYYY/MM/DD  hh:mm:ss |
	while (1)
	{
		if (dirty)
		{
			// page max
			if (page_no == page_no_max)
			{
				page_idx_max = (dflst.num - 1) & 0xF;
			}
			else
			{
				page_idx_max = 15;
			}

			// dfi : point to head of page
			dfi = dflst.head;
			if (page_no > 0)
			{
				for (i=0; i<page_no*16; i++)
					dfi = dfi->next;
			}

			for (i=0; i<=page_idx_max; i++)
			{
				utc = localtime(&dfi->mtime);

				sprintf(item, " %3d  %12s  %5d    %7ld  %04d/%02d/%02d  %02d:%02d:%02d ",
					page_no*16+i+1,	dfi->name, dfi->num, dfi->size,
					utc->tm_year+1900, utc->tm_mon+1, utc->tm_mday,
					utc->tm_hour, utc->tm_min, utc->tm_sec);

				if(i == page_idx)
				{
					t_xy_cl_puts(sx+1, sy+4+i, fg, fm, item);
				}
				else
				{
					t_xy_cl_puts(sx+1, sy+4+i, fg, bg, item);
				}

				dfi = dfi->next;
			}
			
			// last page : for tail of item list
			if (page_idx_max < 15)
			{
				for (i=page_idx_max+1; i<=15; i++)
					t_mark_hline(sx+1, sy+4+i, 59, bg);//t_put_hline(sx+1, sy+4+i, 59);
			}

			// bar
			//t_mark_hline(sx+1, sy+4+page_idx, 59, fm);
			
			dirty = 0;
		}

		key = (uint16_t)bioskey(1);
		if(key)
		{
			//key = key_blk_read_sc();

			if (key == (SCAN_ESC << 8))
			{
				ret = -1;
				break;
			}
			else if (key == (SCAN_UP << 8))
			{
				if (page_no == 0)
				{
					// 1st page
					page_idx--;

					//if (page_idx < 0)
					//	page_idx = 0;
				
					// round down
					if (page_idx < 0)
					{
						page_idx_max = (dflst.num - 1) & 0xF;
						page_idx = page_idx_max;
						page_no = page_no_max;
					}
				}
				else
				{
					// not 1st page
					page_idx--;
					if (page_idx < 0)
					{
						page_no--;
						page_idx = 15;
					}
				}
				dirty = 1;
			}
			else if (key == (SCAN_DOWN << 8))
			{
				if (page_no < page_no_max)
				{
					// not last page
					page_idx++;
					if (page_idx > 15)
					{
						// next page
						page_no++;
						page_idx = 0;
					}
				}
				else
				{
					// last page
					page_idx++;
					page_idx_max = (dflst.num - 1) & 0xF;
					//if (page_idx > page_idx_max)
					//	page_idx = page_idx_max;
				
					// round up
					if (page_idx > page_idx_max)
					{
						page_idx = 0;
						page_no = 0;
					}
				}
				dirty = 1;
			}
			else if (key == (SCAN_PAGE_UP << 8))
			{
				if (page_no == 0)
					page_idx = 0;	// 1st page
				else
					page_no--;		// 2nd page ~

				dirty = 1;
			}
			else if (key == (SCAN_PAGE_DOWN << 8))
			{
				if (page_no < page_no_max)
				{
					// not last page
					page_no++;
					if (page_no == page_no_max)
					{
						page_idx_max = (dflst.num - 1) & 0xF;
						if (page_idx > page_idx_max)
							page_idx = page_idx_max;
					}
				}
				else
				{
					// last page
					page_idx_max = (dflst.num - 1) & 0xF;
					page_idx = page_idx_max;
				}

				dirty = 1;
			}
			else if (key == CHAR_CARRIAGE_RETURN)
			{
				ret = (page_no*16 + page_idx);
				
				dfi = dflst.head;
				if (ret > 0)
				{
					for (i=0; i<ret; i++)
						dfi = dfi->next;
				}
				sprintf(file_sel, "%s", dfi->name);

				break;
			}
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
			}
		}
	}

	dat_del_info();

	// restore screen
	t_paste_region(-1, -1);

	return ret;
}

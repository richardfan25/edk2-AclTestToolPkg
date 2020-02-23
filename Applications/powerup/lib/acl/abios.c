//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ACL - ABIOS (Advantech BIOS)                                             *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>
//#include <bios.h>
#include <string.h>
//#include <conio.h>

#include "typedef.h"
#include "x86io.h"
#include "abios.h"
#include "t_video.h"
#include "t_scrcap.h"
#include "key.h"

//=============================================================================
//  variable
//=============================================================================
abios_t	abios;

void	(*abios_getch_update)(void) = NULL;

static EFI_GUID gEFIBiosStringVariableGuid = {0xAAAA0056, 0x3341, 0x44B5, {0x9C, 0x9C, 0x6D, 0x76, 0xF7, 0x67, 0x38, 0xBB}};

//=============================================================================
//  EfiLibGetSystemConfigurationTable
//=============================================================================
EFI_STATUS EfiGetRuntimeVariableServices(IN EFI_GUID *TableGuid, OUT uint8_t **info, UINTN *size)
{
	EFI_STATUS status = 0;

 	status = gRT->GetVariable(L"BIOSString", TableGuid, NULL, size, (VOID *)info);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		*info = AllocatePool(*size);
		if(info != NULL)
		{
			status = gRT->GetVariable(L"BIOSString", TableGuid, NULL, size, (VOID *)info);
		}
	}
	
	return status;
}

//=============================================================================
//  abios_init
//=============================================================================
int abios_init(void)
{
	#ifdef __BORLANDC__
	// find tag in E0000~FFFFF
	char far	*addr = (char far *)0xF0000000;	// F000:0000 = 0x000F0000
	#endif
	
	#ifdef __WATCOMC__
	// find tag in E0000~FFFFF
	char		*addr = (char *)0xF0000;	// F000:0000 = 0x000F0000;
	#endif
	
	int			i, j;
	char		tag[4];
	uint8_t	flag;
	uint16_t	off;
	char		buf[256];
	char		*pch;
	char		*delim = " ";
	char		*ptr[16];

	UINTN size = 0;
	EFI_STATUS status;

	memset(&abios, 0, sizeof(abios_t));
	flag = 0;

	status = EfiGetRuntimeVariableServices(&gEFIBiosStringVariableGuid, (uint8_t**)buf, &size);
	if(status == 0)
	{
		abios.st_addr = buf;
		abios.en_addr = abios.st_addr + size -1;
		abios.segment = ((uint64_t)abios.st_addr >> 16) & 0xFFFF;
		abios.offset  = (uint64_t)abios.st_addr & 0xFFFF;
		flag = 2;
	}
	else
	{
		for (off = 0; off < 65533; off++)
		{
			// reading tag : ????
			if (off == 0)
			{
				for (j=0; j<4; j++)
					tag[j] = *addr++;
			}
			else
			{
				for (j=1; j<4; j++)
					tag[j-1] = tag[j];
				tag[3] = *addr++;
			}

			// checking tag : ****
			if (tag[0] == '*' && tag[1] == '*' && tag[2] == '*' && tag[3] == '*')
			{
				if (flag == 0)
				{
					// st_addr : flag=0
					abios.st_addr = (addr - 4);
					abios.segment = 0xF000;
					abios.offset  = off;
					flag = 1;
				}
				else if (flag == 1)
				{
					// en_addr : flag=1
					abios.en_addr = (addr - 1);	// point to last *
					flag = 2;
					flag |= 0xF0;	// 0xF000 segment
					break;
				}
			}
		}

		if (flag != 0xF2)
		{
			#ifdef __BORLANDC__
			// find tag in E0000
			addr = (char far *)0xE0000000;	// E000:0000 = 0x000E0000
			#endif

			#ifdef __WATCOMC__
			// find tag in E0000~FFFFF
			addr = (char *)0xE0000;	// E000:0000 = 0x000E0000
			#endif

			flag = 0;

			for (off = 0; off < 65533; off++)
			{
				// reading tag : ????
				if (off == 0)
				{
					for (j=0; j<4; j++)
						tag[j] = *addr++;
				}
				else
				{
					for (j=1; j<4; j++)
						tag[j-1] = tag[j];
					tag[3] = *addr++;
				}

				// checking tag : ****
				if (tag[0] == '*' && tag[1] == '*' && tag[2] == '*' && tag[3] == '*')
				{
					if (flag == 0)
					{
						// st_addr : flag=0
						abios.st_addr = (addr - 4);
						abios.segment = 0xE000;
						abios.offset  = off;
						flag = 1;
					}
					else if (flag == 1)
					{
						// en_addr : flag=1
						abios.en_addr = (addr - 1);	// point to last *
						flag = 2;
						flag |= 0xE0;	// 0xE000 segment
						break;
					}
				}
			}

		}//flag==0
	}

	// found bios tag
	if ((flag & 0x03) == 2)
	{
		abios.len = (int)(abios.en_addr - abios.st_addr) + 1;
		
		abios.magic = (char *)malloc(abios.len*sizeof(char));
		if (!abios.magic)
			return -2;	// malloc err!
		//memset(abios.magic, 0, sizeof(char)*abios.len);
		
		// bios tag must be shorter than 256 bytes
		//if (abios.len < 256)
		//	memcpy(abios.magic, abios.st_addr, abios.len);

		memcpy(abios.magic, abios.st_addr, abios.len);
		abios.magic[abios.len] = 0;

		sprintf(buf, "%s", abios.magic);

		abios.addr32	= abios.segment;
		abios.addr32	<<= 4;
		abios.addr32	+= abios.offset;

		i = 0;
		pch = strtok(buf, delim);
		ptr[i++] = pch;
		while (pch != NULL)
		{
			pch = strtok(NULL, delim);
			ptr[i++] = pch;
		}

		for (j=0; j<i; j++)
		{
			if (strcmp(ptr[j], "BIOS") == 0)
			{
				// **** PCM-9366 BIOS ....
				// **** Advatench-Innocore DPX-S435 BIOS ....
				if (j>0)
				{
					// **** ... <model_name> BIOS <version> .... ****
					
					// previous token is model name
					if (strlen(ptr[j-1]) > 31)
						sprintf(abios.model, "%31s", ptr[j-1]);
					else
						sprintf(abios.model, "%s", ptr[j-1]);
				
					if (strlen(ptr[j+1]) > 31)
						sprintf(abios.ver, "%31s", ptr[j+1]);
					else
						sprintf(abios.ver, "%s", ptr[j+1]);
				}
				break;
			}
		}
		
		return 0;
	}
	else
	{
		sprintf(abios.model, "%s", "NON-ACL-MB");
		sprintf(abios.ver, "%s", "NON-BIOS-VER");
		
		return -1;	// BIOS version not found!
	}
}

//=============================================================================
//  abios_exit
//=============================================================================
void abios_exit(void)
{
	if (abios.magic)
		free(abios.magic);
}

//===========================================================================
//  abios_delay
//===========================================================================
uint8_t abios_delay(uint32_t ms)
{
	uint32_t	cnt;

	// unit : 10ms
	cnt = (ms + 9) / 10;
	
	while (1)
	{
		if(bioskey(1))
		{
			#ifdef __BORLANDC__
			bioskey(0);
			#endif

			#ifdef __WATCOMC__
			//_bios_keybrd(0);
			#endif

			break;
		}

		delay(10);
		
		if (--cnt == 0)
			break;
	}

	if (cnt)
		return 0xFF;

	return 0;
}

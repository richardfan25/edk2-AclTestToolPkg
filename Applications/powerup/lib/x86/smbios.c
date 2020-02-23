//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SMBIOS (System Mangement BIOS)                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "smbios.h"
//=============================================================================
//  variables
//=============================================================================
smbios_t		smbios;

//=============================================================================
//  smbios_init
//=============================================================================
uint8_t smbios_init(void)
{
	#ifdef __BORLANDC__
	char far		*addr = (char far *)0xF0000000;	// F000:0000 = 0x000F0000
	#endif
	
	#ifdef __WATCOMC__
	//uint8_t			*addr = (uint8_t *)0xF0000;	// F000:0000 = 0x000F0000;
	#endif

	//uint32_t		off;
	uint8_t			found;
	//uint8_t			sum, len;
	//int				i;
	
	SMBIOS_TABLE_ENTRY_POINT *gSmbiosTable = NULL;
	SMBIOS_TABLE_3_0_ENTRY_POINT *gSmbios3Table = NULL;
	EFI_STATUS status;

	smbios.hdr = NULL;
	smbios.v3hdr = NULL;
#if 0
	found	= 0;

	// non-UEFI :
	//	16 bytes alignment : 0xF0000~0xFFFFF

	// UEFI :
	//  look for EFI configuration table for SMBIOS GUID {EB9D2D31-2D88-11D3-9A16-0090273FC14D}
	//  see section 4.6 of the UEFI spec for details. section 2.3
	
	// SMBIOS 2.1 UUID {EB9D2D31-2D88-11D3-9A16-0090273FC14D}
	// SMBIOS 3.0 UUID {F2FD1544-9794-4A2C-992E-E5BBCF20E394}
	for (off=0; off<65536; off+=16)
	{
		if (memcmp(&addr[off], "_SM_", 4) == 0)
		{
			if (memcmp(&addr[off+16], "_DMI_", 5) == 0)
			{
				found = 1;
				smbios.hdr = (smbios_hdr_t *)&addr[off];
			}
			else
			{
				// checksum
				len = addr[off+5];
				sum = 0;
				for (i=0; i<len; i++)
					sum += addr[off+i];
				if (sum == 0)
				{
					found = 1;	// checksum ok
					smbios.hdr = (smbios_hdr_t *)&addr[off];
				}
			}
		}
		else if (memcmp(&addr[off], "_SM3_", 5) == 0)
		{
			smbios.v3hdr = (smbios_v3hdr_t *)&addr[off];
			if (found)
				break;
		}
	}
#endif
	found = 0;
	status = EfiGetSystemConfigurationTable(&gEfiSmbiosTableGuid, (VOID**)&gSmbiosTable);
	if(status == EFI_SUCCESS && gSmbiosTable != NULL)
	{
		if(CompareMem (gSmbiosTable->AnchorString, "_SM_", 4) == 0)
		{
			smbios.hdr = (smbios_hdr_t *)gSmbiosTable;
			found = 1;
		}
	}
	
	status = EfiGetSystemConfigurationTable(&gEfiSmbios3TableGuid, (VOID**)&gSmbios3Table);
	if(status == EFI_SUCCESS || gSmbios3Table != NULL)
	{
		if(CompareMem (gSmbios3Table->AnchorString, "_SM3_", 5) == 0)
		{
			smbios.v3hdr = (smbios_v3hdr_t *)gSmbios3Table;
		}
	}

	if (!found)
	{
		return SMBIOS_ERR_NOT_FOUND;
	}

	return SMBIOS_OK;
}
	
//=============================================================================
//  smbios_exit
//=============================================================================
uint8_t smbios_exit(void)
{
	return SMBIOS_OK;
}

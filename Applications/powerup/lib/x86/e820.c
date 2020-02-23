//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - E820                                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include "typedef.h"

#ifdef __WATCOMC__
#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>
//#include <i86.h>
#include <string.h>
#endif

#include "dpmi.h"
#include "e820.h"

//===========================================================================
//  variable
//===========================================================================
e820_t		e820;

char	e820_type_name[8][5] =
{
	"UDEF",	// 0 undefined
	"MEM ",	// 1 memory
	"RSVD",	// 2 reserved
	"ACPI",	// 3 acpi
	"NVS ",	// 4 nvram
	"UNUS",	// 5 unused
	"DIS ",	// 6 disabled
	"ERR "	// 7 error (not defined in ACPI spec)
};

//===========================================================================
//  e820_get_info
//===========================================================================
#ifdef __BORLANDC__

#pragma inline
static uint32_t e820_get_info(uint32_t index, e820_info_t *e820)
{
	uint32_t	a, b, c, d, e;
	uint32_t	res;

    asm {
        .386
        push	eax
        push	ebx
        push	ecx
        push	edx
		push	esi
		push	edi
		push	ds
		push	es
		pushf

        mov     eax, 0E820h
        mov		ebx, index
        mov     edx, 0534D4150h
        mov     ecx, 14h	// 20 bytes
        int     15h

		mov		res, eax
		
        mov     eax, es:[di]
        mov     a, eax
        inc     di
        inc     di
        inc     di
        inc     di
        mov     eax, es:[di]
        mov     b, eax
        inc     di
        inc     di
        inc     di
        inc     di

        mov     eax, es:[di]
        mov     c, eax
        inc     di
        inc     di
        inc     di
        inc     di
        
        mov     eax, es:[di]
        mov     d, eax
        inc     di
        inc     di
        inc     di
        inc     di

        mov     eax, es:[di]
        mov     e, eax
        inc     di
        inc     di
        inc     di
        inc     di

		popf
		pop		es
		pop		ds
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
    };

	memset(e820, 0, sizeof(e820_info_t));

	if (res == 0x534d4150)
	{
		e820->base_lsb	= a;
		e820->base_msb	= b;
		e820->len_lsb	= c;
		e820->len_msb	= d;
		e820->type		= e;
	}

	return res;
}
#endif

#if 0
#ifdef __WATCOMC__
extern uint32_t far e820_get_info(uint32_t index, e820_info_t *e820);
#pragma aux e820_get_info parm [ebx][es di] = \
	"mov eax, 0E820h"		\
	"mov edx, 0534D4150h"	\
	"mov ecx, 14h"			\
	"int 15h"				\
	modify [eax];
#endif
#endif

EFI_STATUS memory_map(EFI_MEMORY_DESCRIPTOR **map_buf, UINTN *map_size, UINTN *map_key, UINTN *desc_size, UINT32 *desc_version)
{
	EFI_STATUS err = EFI_SUCCESS;

	*map_size = sizeof(**map_buf) * 31;

get_map:

	*map_size += sizeof(**map_buf);
	err = gBS->AllocatePool(EfiLoaderData, *map_size, (void **)map_buf);

	if (err != EFI_SUCCESS)
	{

		Print(L"ERROR: Failed to allocate pool for memory map");
		return err;
	}

	err = gBS->GetMemoryMap(map_size, *map_buf, map_key, desc_size, desc_version);

	if (err != EFI_SUCCESS)
	{
		if (err == EFI_BUFFER_TOO_SMALL)
		{
			gBS->FreePool((void *)*map_buf);
			goto get_map;
		}
		Print(L"ERROR: Failed to get memory map");
	}

	return err;
}

#ifdef __WATCOMC__
uint32_t e820_get_info(uint32_t index, e820_info_t *e820)
{
	//__dpmi_regs		regs;

	//memset(&regs, 0, sizeof(regs));

	//regs.d.eax = 0xE820;
	//regs.d.ebx = index;
	//regs.d.ecx = 0x14;	// 20 bytes
	//regs.d.edx = 0x534D4150;

	//if (real_mode_int_es_di(e820, sizeof(e820_info_t), &regs, 0x15))
	//	return regs.d.eax;
	//return regs.d.eax;
	static EFI_MEMORY_DESCRIPTOR *buf = NULL;
	static EFI_MEMORY_DESCRIPTOR *desc = NULL;
	EFI_STATUS err = EFI_SUCCESS;

	static uint8_t flag = 0;
	static UINTN size = 0, desc_size = 0;
	UINTN map_key, mapping_size, page_size = 4096;
	UINT32 desc_version;

	if(flag == 0)
	{
		err = memory_map(&buf, &size, &map_key, &desc_size, &desc_version);
		if(err == EFI_SUCCESS)
		{
			flag = 1;
			desc = buf;
		}
	}
	
	if((UINT8 *)desc <  (UINT8 *)buf + size)
	{
		mapping_size =(UINTN) desc->NumberOfPages * page_size;
		//Print(L"[#%02d] Type: %d  Attr: 0x%x\n", index, desc->Type, desc->Attribute);
		//Print(L"      Phys: %016llx-%016llx\n", desc->PhysicalStart, desc->PhysicalStart + mapping_size -1 );

		switch(desc->Type)
		{
			case 1:		//EfiLoaderCode
			case 2:		//EfiLoaderData
			case 3:		//EfiBootServicesCode
			case 4:		//EfiBootServicesData
			case 5:		//EfiRuntimeServicesCode
			case 6:		//EfiRuntimeServicesData
			case 7:		//EfiConventionalMemory
			case 9:		//EfiACPIReclaimMemory
			case 10:		//EfiACPIMemoryNVS
			case 13:		//EfiPalCode
			case 14:		//EfiPersistentMemory
			{
				e820->type = 1;
				break;
			}
			case 0:		//EfiReservedMemoryType
			case 11:		//EfiMemoryMappedIO
			case 12:		//EfiMemoryMappedIOPortSpace
			{
				e820->type = 2;
				break;
			}
			case 8:		//EfiUnusableMemory
			{
				e820->type = 5;
				break;
			}
			default:
			{
				e820->type = 0;
				break;
			}
		}
		e820->base_lsb = desc->PhysicalStart & 0xFFFFFFFF;
		e820->base_msb = desc->PhysicalStart >> 32;
		e820->len_lsb = mapping_size & 0xFFFFFFFF;
		e820->len_msb = mapping_size >> 32;
		
		desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)desc + desc_size);

		return 0x534d4150;
	}
	else
	{
		gBS->FreePool (buf);
		flag = 0;
	}
	return (uint32_t)err;
}
#endif

/*
uint32_t e820_get_info(uint32_t index, e820_info_t *e820)
{
	union	REGS 	regs;
	struct	SREGS	sregs;

	regs.d.eax = 0xE820;
	regs.d.ebx = index;
	regs.d.ecx = 0x14;	// 20 bytes
	regs.d.edx = 0x534D4150;
	
	sregs.es = FP_SEG(e820);
	reg.d.di = FP_OFF(e820);
	int386x(0x15, &regs, &regs, &sregs);

	return regs.d.eax;
}
*/

//=============================================================================
//  e820_add_info
//=============================================================================
static e820_info_t *e820_add_info(void)
{
	e820_info_t	*info;

	info = (e820_info_t *)malloc(sizeof(e820_info_t));
	if (!info)
		return NULL;

	memset(info, 0, sizeof(e820_info_t));

	return info;
}

//=============================================================================
//  e820_get_type_name
//=============================================================================
char *e820_get_type_name(uint8_t id)
{
	return e820_type_name[id&0x7];
}

//=============================================================================
//  e820_init
//=============================================================================
uint8_t e820_init(void)
{
	e820_info_t		e820_item;
	e820_info_t		*e = NULL, *pe = NULL;

	uint32_t		i, res;
	uint32_t		kbsz;
	uint8_t			t;
	
	memset(&e820, 0, sizeof(e820_t));
	
	for (i=0; i<E820_ITEM_MAX; i++)
	{
		res = e820_get_info(i, &e820_item);
		if (res != 0x534d4150)
			break;

		e = e820_add_info();
		if (!e)
		{
			continue;
			//return E820_ERR_MALLOC;
		}

		if (e820.num_item == 0)
		{
			e820.item = e;	// 1st device
		}
		else
		{
			pe->next = e;
			e->prev = pe;
		}
		pe = e;
		e820.num_item++;

		// RAM unit : KB, (max = 4TB-1)
		kbsz = (e820_item.len_lsb >> 10);
		if (e820_item.len_msb)
			kbsz += (e820_item.len_msb << 22);

		e820_item.kbsz = kbsz;
		memcpy(e, &e820_item, E820_ITEM_SZ);

		if (e820_item.type == E820_TYPE_RAM)
			e820.kbsz += e820_item.kbsz;

		t = e820_item.type & 0x7;
		e820.tynum[t]++;
		e820.tysz[t] += e820_item.kbsz;
	}
	e820.mbsz = e820.kbsz >> 10;

	return E820_OK;
}

//=============================================================================
//  e820_exit
//=============================================================================
void e820_exit(void)
{
	uint32_t		i;
	e820_info_t	*e, *pe;
	
	if (e820.num_item)
	{
		pe = e820.item;
		for (i=0; i<e820.num_item; i++)
		{
			e = pe;
			pe = e->next;
			if (e)
				free(e);
		}
	}
}

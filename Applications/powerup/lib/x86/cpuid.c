//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - CPUID (CPUID instruction for identification)                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "cpuid.h"

//=============================================================================
//  cpuid_get_ex
//=============================================================================
static void cpuid_get_ex(uint32_t* obuf, uint32_t id, uint32_t subid)
{
	if (obuf == NULL)
		return;

	obuf[0] = id;		// eax
	obuf[2] = subid;   // ecx

	#ifdef __BORLANDC__
	_asm {
		mov di, obuf;
		db 0x66; mov cx, [di+8];	// mov ecx, DWORD PTR [di+8]
		db 0x66; mov ax, [di];		// mov eax, DWORD PTR [di]
		db 0x66; xor dx, dx;		// xor edx, edx
		db 0x66; xor bx, bx;		// xor ebx, ebx

		db 0x0f; db 0xa2;			// cpuid

		db 0x66; mov [di], ax;		// mov DWORD PTR [di], eax
		db 0x66; mov [di+4], bx;	// mov DWORD PTR [di+4], ebx
		db 0x66; mov [di+8], cx;	// mov DWORD PTR [di+8], ecx
		db 0x66; mov [di+12], dx;	// mov DWORD PTR [di+12], edx
	}
	#endif

	#ifdef __WATCOMC__
	/*__asm
	{
		mov	edi, obuf
		mov ecx, [edi+8]
		mov	eax, [edi]
		xor edx, edx
		xor	ebx, ebx
		cpuid
		mov	[edi], eax
		mov	[edi+4], ebx
		mov	[edi+8], ecx
		mov	[edi+12], edx		
	}*/
	AsmCpuid(id, &obuf[0], &obuf[1], &obuf[2], &obuf[3]);
	#endif
}

//=============================================================================
//  cpuid_get_edx
//=============================================================================
void cpuid_get_edx(uint32_t* obuf)
{
	if (obuf == NULL)
		return;

	#ifdef __BORLANDC__
	_asm {
		mov di, obuf;
		db 0x66; mov [di], dx;		// mov DWORD PTR [di], eax
	}
	#endif

	#ifdef __WATCOMC__
	/*__asm
	{
		mov	edi, obuf
		mov	[edi], edx
	}*/
	#endif
}

//=============================================================================
//  cpuid_get_id
//=============================================================================
void cpuid_get_id(uint32_t* obuf, uint32_t id)
{
	cpuid_get_ex(obuf, id, 0);
}

//=============================================================================
//  cpuid_get_vendor
//=============================================================================
int cpuid_get_vendor(char *str)
{
	uint32_t buf[4];

	if (str == NULL)
		return -1;

	// EAX=0: Vendor-ID and Largest Standard Function
	cpuid_get_id(buf, 0);

	*(uint32_t*)&str[0] = buf[1];	// ebx:
	*(uint32_t*)&str[4] = buf[3];	// edx:
	*(uint32_t*)&str[8] = buf[2];	// ecx:

	str[12] = '\0';

	return 0;
}

//=============================================================================
//  cpuid_get_brand
//=============================================================================
int cpuid_get_brand(char *str)
{
	uint32_t buf[4];
	char		c, st;
	char		brand[64], *ptr;
	int			i;

	if (str == NULL)
		return -1;

	// Function 0x80000000: Largest Extended Function Number
	cpuid_get_id(buf, 0x80000000UL);

	if (buf[0] < 0x80000004UL)
		return -1;

	// Function 80000002h,80000003h,80000004h: Processor Brand String
	cpuid_get_id((uint32_t*)&brand[0], 0x80000002UL);
	cpuid_get_id((uint32_t*)&brand[16], 0x80000003UL);
	cpuid_get_id((uint32_t*)&brand[32], 0x80000004UL);

	ptr = brand;
	st	= 0;
	i	= 0;
	do
	{
		c = *ptr++;
		if (st == 0)
		{
			// delete space from head of brand
			if (c > 0x20 && c < 0x7E)
			{
				st = 1;
				str[i++] = c;
			}
		}
		else
		{
			str[i++] = c;
		}

	} while (c != 0);

	return 0;
}

//=============================================================================
//  cpuid_get_clock
//=============================================================================
// name : Intel(R) Pentium(R) CPU N4200 @ 1.1GHz
uint32_t cpuid_get_clock(char *name)
{
	char	*pch;
	char	*delim = " ";
	char	*ptr[16];
	char	buf[16];
	int		i, j, idx = 0;
	int		found;
	double	mhz = 0;
	char	c;

	i = 0;
	found = 0;

	while (1)
	{
		if (i==0)
			pch = strtok(name, delim);
		else
			pch = strtok(NULL, delim);

		if (pch == NULL)
			break;

		ptr[i++] = pch;
		if (*pch == '@')
		{
			// @ 1.1GHz
			idx = i;	// next i : 1.1GHz
		}

		if (i >= 16)
			break;
	}

	if (i > idx)
		found = 1;

#if 0
	pch = strtok(name, delim);
	ptr[i++] = pch;

	while (pch != NULL)
	{
		pch = strtok(NULL, delim);
		ptr[i++] = pch;

		if (*pch == '@')
		{
			idx = i;	// next : 1.1GHz
			found = 1;
		}

		if (i>=16)
			break;
	}
#endif

	if (found)
	{
		for (i=0, j=0; ; i++)
		{
			c = ptr[idx][i];
			
			if (c >= '0' && c <= '9')
			{
				buf[j++] = c;
			}
			else if (c == '.')
			{
				buf[j++] = c;
			}
			else
			{
				// should be 1100MHz, 800MHz, 1.10GHz..
				break;
			}
		}
		buf[j] = 0;

		mhz = atof(buf);

		if (strstr(ptr[idx], "GHz"))
		{
			mhz *= 1000.0;	// GHz -> MHz
		}
	}

	if (found)
		return (uint32_t)mhz;
	else
		return 0;
}

//=============================================================================
//  cpuid_get_info
//=============================================================================
int cpuid_get_info(cpuid_info_t *info)
{
	uint32_t reg[4];

	//cpuid_get_edx(&reg[0]);

	if (info == NULL)
		return -1;

	memset(info, 0, sizeof(cpuid_info_t));

	//info->rst_edx = reg[0];

	// EAX=0: Vendor-ID and Largest Standard Function
	cpuid_get_id(reg, 0x00);
	*(uint32_t*)&info->vid[0] = reg[1];	// ebx:
	*(uint32_t*)&info->vid[4] = reg[3];	// edx:
	*(uint32_t*)&info->vid[8] = reg[2];	// ecx:

	// EAX=1: Processor Signature
	cpuid_get_id(reg, 0x01);

	info->stepping = reg[0] & 0xF;
	reg[0] >>= 4;

	info->std_model = reg[0] & 0xF;
	reg[0] >>= 4;
	
	info->std_family = reg[0] & 0xF;
	reg[0] >>= 4;

	info->cpu_type = reg[0] & 0x3;
	reg[0] >>= 4;

	info->ext_model = reg[0] & 0xF;
	reg[0] >>= 4;

	info->ext_family = reg[0] & 0xFF;
	
	// Feature Flags
	info->feat_ecx = reg[2];
	info->feat_edx = reg[3];

	// family code
	info->family = info->ext_family;
	info->family += info->std_family;

	// model code
	info->model = info->ext_model;
	info->model <<= 4;
	info->model += info->std_model;

	// EAX=0x80000000 : Largest Extended Function Number
	cpuid_get_id(reg, 0x80000000UL);

	// EAX=80000000h: extend vendor_id (AMD)
	*(uint32_t*)&info->evid[0] = reg[1];	// ebx:
	*(uint32_t*)&info->evid[4] = reg[3];	// edx:
	*(uint32_t*)&info->evid[8] = reg[2];	// ecx:

	if (reg[0] >= 0x80000004UL)
	{
		// EAX=80000002h
		// EAX=80000003h
		// EAX=80000004h
		// Processor Brand String
		cpuid_get_id((uint32_t*)&info->name[0], 0x80000002UL);
		cpuid_get_id((uint32_t*)&info->name[16], 0x80000003UL);
		cpuid_get_id((uint32_t*)&info->name[32], 0x80000004UL);
	}

	return 0;
}

#if 0
//=============================================================================
//  main
//=============================================================================
int main(void)
{
	int				ret;
	cpuid_info_t	cpuinfo;
	
	ret = cpuid_get_info( &cpuinfo );
	if (ret)
	{
		fprintf(stderr, "cpuid_get_info() : err!\n");
		return -1;
	}
	
	printf("cpuid >>\n");
	printf("    vid : %s\n", cpuinfo.vid);
	printf("rst_edx : 0x%08X\n", cpuinfo.rst_edx);
	printf("ext_fam : 0x%X\n", cpuinfo.ext_family);
	printf("ext_mod : 0x%X\n", cpuinfo.ext_model);
	printf("cpu_typ : 0x%X\n", cpuinfo.cpu_type);
	printf("std_fam : 0x%X\n", cpuinfo.std_family);
	printf("std_mod : 0x%X\n", cpuinfo.std_model);
	printf("steppin : 0x%X\n", cpuinfo.stepping);
	printf("\n");
	printf(" family : 0x%X\n", cpuinfo.family);
	printf("  model : 0x%X\n", cpuinfo.model);
	printf("\n");
	printf("   name : [%s]\n", cpuinfo.name);
	printf("   evid : [%s]\n", cpuinfo.evid);
	printf("\n");
	printf("feat_cx : 0x%08X\n", cpuinfo.feat_ecx);
	printf("feat_dx : 0x%08X\n", cpuinfo.feat_edx);

	return 0;
}
#endif
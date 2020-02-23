//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  MSR - Model-Specific Registers                                           *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_MSR_H
#define __X86_MSR_H

#include "typedef.h"

//=============================================================================
//  MSR
//=============================================================================
// index
// 00000000
// 00000001

// 0x198    IA32_PERF_STS
// 0x199    IA32_PERF_CTL
// 0x19A    IA32_THERM_CTL
// 0x19B    IA32_THERM_INTERRUPT
// 0x19C    IA32_THERM2_STATUS
// 0x19D    IA32_THERM2_CTL
//......
// 0x1A0    IA32_MISC_ENABLE
//......
// 0x1A2    IA32_TEMPERATURE_TARGET
//......
// 0x1A2    IA32_MISC_PWR_MGMT

//#############################################################################
//  __BORLANDC__
//#############################################################################
#ifdef __BORLANDC__

//=============================================================================
//  rdmsr_lsb
//=============================================================================
const uint8_t g_rdmsr_lsb[] =
{									//
	0x66, 0x55, 0x66,				// push esp
	0x8B, 0xEC,						// mov ebp, esp
	0x67, 0x66, 0x8B, 0x4D, 0x08,	// mov ecx, [bp + 8]
	0x0F, 0x32,						// rdmsr
	
	0x66, 0x8B, 0xD0,				// mov edx, eax
	0x66, 0xC1, 0xE8, 0x10,			// shr eax, 16
	0x66, 0x92,						// xchg edx, eax
	
	0x66, 0x5D,						// pop ebp
	0xCB							// retf
};
static uint32_t far cdecl (*rdmsr_lsb)(uint32_t idx) =
	(uint32_t far (*)(uint32_t))g_rdmsr_lsb;

//=============================================================================
//  rdmsr_msb
//=============================================================================
const uint8_t g_rdmsr_msb[] =
{									/* BITS 16 */
	0x66, 0x55, 0x66,				// push esp
	0x8B, 0xEC,						// mov ebp, esp
	0x67, 0x66, 0x8B, 0x4D, 0x08,	// mov ecx, [bp + 8]
	0x0F, 0x32,						// rdmsr
	
	0x66, 0x8B, 0xC2,				// mov eax, edx
	0x66, 0xC1, 0xE8, 0x10,			// shr eax, 16
	0x66, 0x92,						// xchg edx, eax
	
	0x66, 0x5D,						// pop ebp
	0xCB							// retf
};
static uint32_t far cdecl (*rdmsr_msb)(uint32_t idx) =
	(uint32_t far (*)(uint32_t))g_rdmsr_msb;
	
//=============================================================================
//  wrmsr
//=============================================================================
const uint8_t g_wrmsr[] =
{									// BITS 16
	0x66, 0x55, 0x66,				// push esp
	0x8B, 0xEC,						// mov ebp, esp
	0x67, 0x66, 0x8B, 0x4D, 0x08,	// mov ecx, [bp + 8]
	0x0F, 0x32,						// rdmsr
	
	0x66, 0x8B, 0xD0,				// mov edx, eax
	0x66, 0xC1, 0xE8, 0x10,			// shr eax, 16
	0x66, 0x92,						// xchg edx, eax
	
	0x66, 0x5D,						// pop ebp
	0xCB							// retf
};
static uint32_t far cdecl (*wrmsr)(uint32_t idx, uint32_t lsb, uint32_t msb) =
	(uint32_t far (*)(uint32_t, uint32_t, uint32_t))g_wrmsr;

#endif

//#############################################################################
//  __WATCOMC__
//#############################################################################
#ifdef __WATCOMC__

uint32_t rdmsr_lsb(uint32_t idx);
uint32_t rdmsr_msb(uint32_t idx);
uint64_t rdmsr(uint32_t idx);
uint32_t wrmsr(uint32_t idx, uint32_t lsb, uint32_t msb);

#endif

#endif

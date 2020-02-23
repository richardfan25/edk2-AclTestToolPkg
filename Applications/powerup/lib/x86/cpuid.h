//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - CPUID (CPUID instruction for identification)                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  EAX = 00h : Vendor ID
//=============================================================================
// EAX : Largest standard function number supported

// EBX~EDX : Vendor ID
// EBX = 0x756E6547 = uneG
// ECX = 0x49656E69 = Ieni
// EDX = 0x6C65746E = letn  ==> GenuineIntel

//=============================================================================
//  EAX = 01h : Processor Signature, Feature Flags
//=============================================================================
// EAX : Processor Signature
// [31:28] Reserved
// [27:20] Extended Family
// [19:16] Extended Model
// [15:14] Reserved
// [13:12] Type
// [11:8]  Family Code
// [7:4]   Model Number
// [3:0]   Stepping ID

// EBX : Misc Info
// EBX[31:24] APIC ID
// EBX[23:16] Count
// EBX[15:8]  Chunks
// EBX[7:0]   Brand ID

// ECX : Feature Flags
// [31:28] RSVD
// [27] OSXSAVE  OS-Enabled Extension State Management
// [26] XSAVE    XSAVE/XSTOR States
// [25] AES      AES Instruction
// [24] RSVD
// [23] POPCNT   POPCNT Instruction
// [22] MOVBE    MOVBE Instruction
// [21] x2APIC   Extended xAPIC Support
// [20] SSE4.2   Streaming SIMD Extensions 4.2
// [19] SSE4.1   Streaming SIMD Extensions 4.1
// [18] DCA      Direct Cache Access
// [17:16] RSVD
// [15] PDCM     Perform and Debug Capability
// [14] xTPR     xTPR Update Control
// [13] CX16     CMPXCHG16B
// [12:11] RSVD
// [10] CNXT-ID  L1 Context ID
// [9]  SSSE3    Supplemental Streaming SIMD Extension 3
// [8]  TM2      Thermal Monitor 2
// [7]  EST      Enhanced Intel SpeedStep Techonology
// [6]  SMX      Safer Mode Extensions
// [5]  VMX      Virtual Machine Extensions
// [4]  DS-CPL   CPL Qualified Debug Store
// [3]  MONITOR  MONITOR/MWAIT
// [2]  DTES64   64-Bit Debug Store
// [1]  PCLMULDQ PCLMULDQ Instruction
// [0]  SSE3     Streaming SIMD Extension 3

// EDX : Feature Flags
// [31] PBE  Pending Break Enable
// [30] RSVD
// [29] TM    Thermal Monitor
// [28] HIT   Multi-Threading
// [27] SS    Self-Snoop
// [26] SSE2  Streaming SIMD Extension 2
// [25] SSE   Streaming SIMD Extension
// [24] FXSR  FXSAVE and FXSTOR Instructions
// [23] MMX   MMX techonology
// [22] ACPI  Thermal Monitor and Software Controlled Clock Facilities
// [21] DS    Debug Store
// [20] RSVD
// [19] CLFSH CLFLUSH Instruction
// [18] PSN   Processor serial number is present and enabled
// [17] PSE36 36-bit Page Size Extension
// [16] PAT   Page Attribute Table
// [15] CMOV  Conditional Move Instruction
// [14] MCA   Machine-Check Architecture
// [13] PGE   Page Global Enable
// [12] MTRR  Memory Type Range Registers
// [11] SEP   Fast System Call
// [10] RSVD
// [9]  APIC  On-Chip APIC Hardware
// [8]  CX8   CMPXCHG8 Instruction
// [7]  MCE   Machine-Check Extension
// [6]  PAE   Physical Address Extension
// [5]  MSR   Model Specific Registers
// [4]  TSC   Time Stamp Counter
// [3]  PSE   Page Size Extension
// [2]  DE    Debugging Extension
// [1]  VME   Virtual Mode Extension
// [0]  FPU   Floating-Point Unit On-Chip



//=============================================================================
//  EAX = 02h : Cache and TLB Descriptors
//=============================================================================

//=============================================================================
//  EAX = 03h : Processor Serial Number
//=============================================================================

//=============================================================================
//  EAX = 04h : Deterministic Cache Parameter
//=============================================================================

//=============================================================================
//  EAX = 05h : MONITOR/MWAIT Parameter
//=============================================================================

//=============================================================================
//  EAX = 06h : Digital Thermal Sensor(DTS) and Power Management Parameters 
//=============================================================================

//=============================================================================
//  EAX = 07h : Reserved
//  EAX = 08h : Reserved
//=============================================================================

//=============================================================================
//  EAX = 09h : Direct Cache Access(DCA) Parameters
//=============================================================================

//=============================================================================
//  EAX = 0Ah : Architectural Performance Monitor Features
//=============================================================================

//=============================================================================
//  EAX = 0Bh : x2APIC Features / Processor Topology
//=============================================================================

//=============================================================================
//  EAX = 0Ch : Reserved
//=============================================================================

//=============================================================================
//  EAX = 0Dh : XSAVE Features
//=============================================================================

//=============================================================================
//  EAX = 80000000h : Largest Extended Function
//=============================================================================

//=============================================================================
//  EAX = 80000001h : Extended Feature Bits
//=============================================================================

//=============================================================================
//  EAX = 80000002h : Processor Name/Brand String
//  EAX = 80000003h : 
//  EAX = 80000004h : 
//=============================================================================

//=============================================================================
//  EAX = 80000005h : Reserved (Intel)
//  EAX = 80000005h : L1 TLB Identifier (AMD)
//=============================================================================

//=============================================================================
//  EAX = 80000006h : Extended L2 Cache Features (Intel)
//  EAX = 80000006h : L2/L3 Identifier (AMD)
//=============================================================================

//=============================================================================
//  EAX = 80000007h : Advanced Power Management
//=============================================================================

//=============================================================================
//  EAX = 80000008h : Virtual and Physical Address Sizes (Intel)
//  EAX = 80000008h : Long Mode Address Size Identifier (AMD)
//=============================================================================

//=============================================================================
//  EAX = 8000000Ah : SVM Revision and Feature Identification (AMD)
//  EAX = 80000019h : L1/L2 TLB 1G Identifier (AMD)
//  EAX = 8000001Ah : Performance Optimization Identifier (AMD)
//  EAX = 8000001Bh : Instruction Based Sampling Identifier (AMD)
//  EAX = 8000001Dh : Cache Properties DC/IC (AMD)
//  EAX = 8000001Eh : Extended APIC ID, Core Identifier (AMD)
//  EAX = 8000001Fh : AMD Secure Encryption (AMD)
//=============================================================================

#ifndef __X86_CPUID_H
#define __X86_CPUID_H

#include "typedef.h"

//=============================================================================
//  define
//=============================================================================
// CPUID Capability Mask
#define CPUID_CAP0_INTEL	0x00002E7F	// 00h~
#define CPUID_CAP1_INTEL	0x000001FF	// 80000000h~
#define CPUID_CAP0_AMD		0x00002E7F	// 00h~
#define CPUID_CAP1_AMD		0xEE0003FF	// 80000000h~

#pragma pack(1)

//=============================================================================
//  typedef : x86_reg_t
//=============================================================================
typedef struct _x86_reg_t
{
	uint32_t	eax;
	uint32_t	ebx;
	uint32_t	ecx;
	uint32_t	edx;

} x86_reg_t;

//=============================================================================
//  cpuid_info_t
//=============================================================================
typedef struct _cpuid_info_t
{
	uint32_t	rst_edx;

	// cpu signature
	char		vid[13];
	char		evid[13];
	char		name[49];

	uint8_t		ext_family;
	uint8_t		ext_model;
	uint8_t		cpu_type;
	uint8_t		std_family;
	uint8_t		std_model;
	uint8_t		stepping;

	uint16_t	family;
	uint16_t	model;

	uint32_t	feat_ecx;	// Feature Flags - ECX
	uint32_t	feat_edx;	// Feature Flags - EDX

} cpuid_info_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
void	 cpuid_get_edx(uint32_t* obuf);
void	 cpuid_get_id(uint32_t* pdwout4, uint32_t id);
int		 cpuid_get_vendor(char* pvendor);
int		 cpuid_get_brand(char* pbrand);
uint32_t cpuid_get_clock(char *name);

int		 cpuid_get_info(cpuid_info_t *info);

#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - DPMI (DOS Protected Mode Interface)                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_DPMI_H
#define __X86_DPMI_H

#ifdef __WATCOMC__

#pragma pack(1)

#define	dosmemget(off,n,buf)		memcpy(buf, (void *)(off), n)
#define	dosmemput(buf,n,off)		memcpy((void *)(off), buf, n)
#define	__djgpp_conventional_base	0

/* same layout as DJGPP */
typedef union
{
	struct
	{
		uint32_t edi;
		uint32_t esi;
		uint32_t ebp;
		uint32_t res;
		uint32_t ebx;
		uint32_t edx;
		uint32_t ecx;
		uint32_t eax;
	} d;
	struct
	{
		uint16_t di, res_di;
		uint16_t si, res_si;
		uint16_t bp, res_bp;
		uint16_t res[2];
		uint16_t bx, res_bx;
		uint16_t dx, res_dx;
		uint16_t cx, res_cx;
		uint16_t ax, res_ax;
		uint16_t flags, es, ds, fs, gs;
		uint16_t ip, cs, sp, ss;
	} x;
	struct
	{
		uint8_t res[16];
		uint8_t bl, bh, res_bx[2];
		uint8_t dl, dh, res_dx[2];
		uint8_t cl, ch, res_cx[2];
		uint8_t al, ah, res_ax[2];
	} h;
} __dpmi_regs;

typedef struct
{
	unsigned long size, address;
} __dpmi_meminfo;

#pragma pack()

//=============================================================================
//  functions
//=============================================================================
int real_mode_int_es_di(void *buf, unsigned buf_size, __dpmi_regs *regs, unsigned int_num);
int real_mode_int_es_di2(void *buf, unsigned buf_size, __dpmi_regs *regs, unsigned int_num);
//int real_mode_int_es_bp(void *buf, unsigned buf_size, __dpmi_regs *regs, unsigned int_num);

int dpmi_physical_address_mapping(__dpmi_meminfo *info);

#endif

#endif

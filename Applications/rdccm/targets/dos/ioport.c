#include "ioport.h"

#if defined(__TURBOC__) || (defined(__WATCOMC__) && !defined(__386__))
/*
 * These functions are _code_, but are stored in the _data_ segment.
 * Declare them 'far' and end them with 'retf' instead of 'ret'.
 *
 * For 16-bit Watcom C, use 'cdecl' to force usage of normal, stack
 * calling convention instead of Watcom register calling convention.
 */
static const unsigned char g_inportl[] =
{
	0x55,					/* push bp */
	0x8B, 0xEC,				/*  mov bp,sp */
	0x8B, 0x56, 0x06,		/*  mov dx,[bp + 6] */
	0x66, 0xED,				/*  in eax,dx */
	0x8B, 0xD0,				/*  mov dx,ax */
	0x66, 0xC1, 0xE8, 0x10,	/*  shr eax,16 */
	0x92,					/*  xchg dx,ax */
	0x5D,					/* pop bp */
	0xCB					/* retf */
};
static unsigned long far cdecl (*inportd)(unsigned port) = (unsigned long far cdecl(*)(unsigned))g_inportl;

static const unsigned char g_outportl[] =
{
	0x55,					/* push bp */
	0x8B, 0xEC,				/*  mov bp,sp */
	0x8B, 0x56, 0x06,		/*  mov dx,[bp + 6] */
	0x66, 0x8B, 0x46, 0x08,	/*  mov eax,[bp + 8] */
	0x66, 0xEF,				/*  out dx,eax */
	0x5D,					/* pop bp */
	0xCB					/* retf */
};
static void far cdecl (*outportd)(unsigned port, unsigned long val) = (void far cdecl(*)(unsigned, unsigned long))g_outportl;

unsigned long inpd(unsigned port)
{
	return inportd(port);
}

void outpd(unsigned port, unsigned long val)
{
	outportd(port, val);
}
#endif /* 16-bit */

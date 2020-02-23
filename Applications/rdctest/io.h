//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - IO (I/O Port)                                                      *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_IO_H
#define __X86_IO_H

//#############################################################################
//  __BORLANDC__
//#############################################################################
#ifdef __BORLANDC__

#include <dos.h>

#define inb		inportb
#define	outb	outportb

#define	inw		inport
#define	outw	outport

#endif

//#############################################################################
//  __WATCOMC__
//#############################################################################
#ifdef __WATCOMC__
#include <conio.h>

#define inb(P)		inp(P)
#define outb(P, V)	outp(P, V)

#define inw(P)		inpw(P)
#define outw(P, V)	outpw(P, V)

#endif

//#############################################################################
//  _UEFI_
//#############################################################################
#ifdef _UEFI_

#include <Library/IoLib.h>

#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)

#endif // _UEFI_


#endif


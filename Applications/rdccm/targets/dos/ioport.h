#ifndef _IOPORT_LIB_
#define _IOPORT_LIB_
/*
 *----------------------------------------------------------------------------
 * DOS IO Port Access
 *----------------------------------------------------------------------------
 * Support compiler:
 *	DJGPP, Turbo C 3.0, or Watcom C
 *
 *----------------------------------------------------------------------------
 */

#if defined(__DJGPP__)
#include <dos.h>
#define	inp(p)			inportb(p)
#define	inpw(p)			inportw(p)
#define	inpd(p)			inportl(p)
#define	outp(p,v)		outportb(p,v)
#define	outpw(p,v)		outportw(p,v)
#define	outpd(p,v)		outportl(p,v)
#elif defined(__WATCOMC__)
#include <conio.h>
#elif defined(__TURBOC__)
#include <dos.h>
//#include <conio.h>
//#define	inp(p)			inportb(p)
//#define	inpw(p)			inportw(p)
//#define	outp(p,v)		outportb(p,v)
//#define	outpw(p,v)		outportw(p,v)
#else
#error Unsupported compiler
#endif

#if defined(__TURBOC__) || (defined(__WATCOMC__) && !defined(__386__))
unsigned long inpd(unsigned port);
void outpd(unsigned port, unsigned long val);
#endif /* 16-bit */

#endif /* _IOPORT_LIB_ */

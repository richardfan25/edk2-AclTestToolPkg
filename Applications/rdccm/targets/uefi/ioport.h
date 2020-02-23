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
#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define	inp(p)			IoRead8(p)
#define	inpw(p)			IoRead16(p)
#define	inpd(p)			IoRead32(p)
#define	outp(p,v)		IoWrite8(p,v)
#define	outpw(p,v)		IoWrite16(p,v)
#define	outpd(p,v)		IoWrite32(p,v)



#endif /* _IOPORT_LIB_ */

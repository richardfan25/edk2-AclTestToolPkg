#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)

// The following patches using for some files that support to compiler on linux or dos
#define sys_open_ioport(a)
#define sys_close_ioport(a)

#endif //_GLOBAL_H_


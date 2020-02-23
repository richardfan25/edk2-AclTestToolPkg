#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <Library/IoLib.h>
#include <Include/unistd.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)

#define clrscr()					gST->ConOut->ClearScreen(gST->ConOut)
#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)

/*----Dos Color to EFI Color Start----*/
// text color
#define BLACK               EFI_BLACK   
#define GREEN               EFI_GREEN    
#define RED                 EFI_RED         
#define LIGHTGRAY           EFI_LIGHTGRAY   
#define YELLOW              EFI_YELLOW

/*----EFI Color End----*/

// The following patches using for some files that support to compiler on linux or dos
#define sys_open_ioport(a)
#define sys_close_ioport(a)



static uint8_t read8(const void *addr)
{
	return *(volatile uint8_t *)addr;
}

static uint16_t read16(const void *addr)
{
	return *(volatile uint16_t *)addr;
}

static uint32_t read32(const void *addr)
{
	return *(volatile uint32_t *)addr;
}

static void write8(void *addr, uint8_t val)
{
	*(volatile uint8_t *)addr = val;
}

static void write16(void *addr, uint16_t val)
{
	*(volatile uint16_t *)addr = val;
}

static void write32(void *addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}



#endif //_GLOBAL_H_


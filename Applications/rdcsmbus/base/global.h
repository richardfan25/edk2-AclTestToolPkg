#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "Protocol/SimpleFileSystem.h"
#include "Guid/FileInfo.h"

#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)

// The following patches using for some files that support to compiler on linux or dos
#define sys_open_ioport(a)
#define sys_close_ioport(a)


#define clrscr()					gST->ConOut->ClearScreen(gST->ConOut)
#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)

#define EFI_SCANEX_ALT				0x0040
#define EFI_SCANEX_CTRL				0x0080





extern UINTN ConsoleCurrRow;
extern UINTN ConsoleWidth;
extern UINTN ConsoleHeight;

extern EFI_HANDLE				gImageHandle;
extern EFI_SYSTEM_TABLE			*gST;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_RUNTIME_SERVICES 	*gRT;
extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInEx;

EFI_STATUS console_init(void);
void console_exit(void);
void console_color(UINTN txt_color, UINTN bg_color);
int bioskey(uint8_t cmd);
void kdebug(uint8_t code1, uint8_t code2);
char *GetCurrentTime(void);
uint8_t CheckKeyIsESC(void);
void InitFileSystem(void);
void WriteLogFile(uint16_t *name, uint8_t *content);

#endif //_GLOBAL_H_


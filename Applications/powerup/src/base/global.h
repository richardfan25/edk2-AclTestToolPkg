/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  global.H
* Purpose : global variable
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <msr.h>

#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Guid/FileInfo.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>

#include <protocol/GraphicsOutput.h>
#include <protocol/UgaDraw.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

#include <Library/BaseMemoryLib.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/SmBios.h>

#include <Guid/FileSystemInfo.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePathFromText.h>

#include <sys/time.h>
//#define KKDEBUG

// The following patches using for some files that support to compiler on linux or dos
#define sys_open_ioport(a)
#define sys_close_ioport(a)

//#define NULL 0
#define clrscr()					gST->ConOut->ClearScreen(gST->ConOut)
#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)
#define delay(a)					usleep(a * 1000)
#define EFI_SCANEX_ALT				0x0040
#define EFI_SCANEX_CTRL				0x0080

/*----Dos Color to EFI Color Start----*/
// text color
#define BLACK               EFI_BLACK       
#define BLUE                EFI_BLUE        
#define GREEN               EFI_GREEN       
#define CYAN                EFI_CYAN        
#define RED                 EFI_RED         
#define MAGENTA             EFI_MAGENTA     
#define BROWN               EFI_BROWN       
#define LIGHTGRAY           EFI_LIGHTGRAY   
#define BRIGHT              EFI_BRIGHT      
#define DARKGRAY            EFI_DARKGRAY    
#define LIGHTBLUE           EFI_LIGHTBLUE   
#define LIGHTGREEN          EFI_LIGHTGREEN  
#define LIGHTCYAN           EFI_LIGHTCYAN   
#define LIGHTRED            EFI_LIGHTRED    
#define LIGHTMAGENTA        EFI_LIGHTMAGENTA
#define YELLOW              EFI_YELLOW      
#define WHITE               EFI_WHITE       

/*----EFI Color End----*/

/*----EFI IO Start----*/
#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)
/*----EFI IO End----*/

#define __WATCOMC__ 1

extern UINTN ConsoleCurrRow;
extern UINTN ConsoleWidth;
extern UINTN ConsoleHeight;

extern UINTN mode_bak;
extern INT32 attr_bak;

extern EFI_HANDLE				gImageHandle;
extern EFI_SYSTEM_TABLE			*gST;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_RUNTIME_SERVICES 	*gRT;
extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInEx;
extern EFI_SHELL_ENVIRONMENT2	*mEfiShellEnvironment2;


int bioskey(uint8_t cmd);
void kdebug(uint8_t code1, uint8_t code2);
void AsciiToUnicodeString(uint8_t *origin, uint16_t *str);
void ByteArrayToOneNumber(uint8_t *origin, uint16_t len, void *one);
int GetKey(void);
uint8_t CheckESC(int key);
uint8_t WaitReturnOrRefreshEvent(void);
void sound(uint16_t freq);
void nosound(void);
#endif //_GLOBAL_H


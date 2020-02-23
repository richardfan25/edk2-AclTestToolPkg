/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  global.H
* Purpose : global variable
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdint.h>
#include <stdlib.h>
#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define clrscr()					gST->ConOut->ClearScreen(gST->ConOut)
#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)
#define delay(a)					usleep(a * 1000)
#define EFI_SCANEX_ALT				0x0040
#define EFI_SCANEX_CTRL				0x0080

/*----Dos Color to EFI Color Start----*/
// text color
#define BLACK               EFI_BLACK      
#define RED                 EFI_RED         
#define LIGHTGRAY           EFI_LIGHTGRAY   
#define YELLOW              EFI_YELLOW

/*----EFI Color End----*/


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
uint8_t CheckESC(int key);
#endif //_GLOBAL_H


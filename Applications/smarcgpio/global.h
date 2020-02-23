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

#include <Protocol/PciRootBridgeIo.h>

//#define NULL 0

#define BIT(X)  (1 << (X))
#define SetFlag(reg, bit)		((reg) |= (bit))
#define ClrFlag(reg, bit) 		((reg) &= (~(bit)))
#define mIsFlag1(reg, bit)   	(((reg) & (bit)) != 0)
#define mIsFlag0(reg, bit)   	(((reg) & (bit)) == 0)

#define OBF_BIT         		BIT(0)
#define IBF_BIT         		BIT(1)

//EC ACPI Ram commands
#define EC_CMD_READ_ACPI   			0x80
#define EC_CMD_WRITE_ACPI    		0x81
//EC HW Ram commands
#define EC_CMD_READ_RAM				0x88
#define EC_CMD_WRITE_RAM			0x89
//Advantech io channel command
#define PMC_CMD_READ_MBX    		0xA0
#define PMC_CMD_WRITE_MBX   		0x50
//ACPI offset
#define ACPI_OFFSET_MBX  			0xA0

#define inp(addr)               IoRead8(addr)
#define inpw(addr)              IoRead16(addr)
#define inpd(addr)              IoRead32(addr)
#define outp(addr,data)			IoWrite8(addr,data)
#define outpw(addr,data)        IoWrite16(addr,data)
#define outpd(addr,data)        IoWrite32(addr,data)

#define clrscr()					gST->ConOut->ClearScreen(gST->ConOut)
#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)
#define delay(a)					usleep(a * 1000)
#define EFI_SCANEX_ALT				0x0040
#define EFI_SCANEX_CTRL				0x0080
typedef enum 
{
    LowActive,      //0
    HighActive,     //1
    OriginActive,   //2
} _GPIO_Active;

typedef enum 
{
    _MBEr_Fail = 0,           //0, fail
    _err_noerror = 1,         //1
    _MBEr_Success = 1,        //1, success
    _MBEr_Undefined_Item,   //2, undefined item
    _MBEr_ID_Undefined,     //3, undefined device id
    _MBEr_Pin_Type_Err,     //4, device type error

    _err_ibc_timeout = 0x10,  //0x10
    _err_obf_timeout,       //0x11
    _err_gpio_index,        //0x12
    _err_gpio_read,         //0x13
    _err_auth,              //0x14
    _err_dynatable_index,   //0x15
    _err_deviceid,          //0x16
    _err_SMBChanelSetFail,  //0x17
    _err_MBoxTimeOut,       //0x18
    _err_MBoxAccessFail,    //0x19
} _pmerror_code;

/*----MBX_CMD_VERSION Start----*/
#define ProjectNameLength   0x08
#define VersionTotalLength  0x11
/*----MBX_CMD_VERSION End----*/
extern uint8_t project_name[ProjectNameLength + 1];
extern uint8_t version[VersionTotalLength - ProjectNameLength];
typedef enum
{
    _mid_main,  //0
    _mid_rd,
    _mid_loaddefault,
    _mid_rd_dyamictbl,
    _mid_rd_tmlprotbl,
    _mid_rd_tmesrctbl,
    _mid_rd_smartfantbl,
    _mid_hwm,
    _mid_thmlProThrottle,
    _mid_thmlProShutdown,
    _mid_wdIRQNMI,
    _mid_wdCldRst,
    _mid_wdWrmRst,
    _mid_wdWDPin,
    _mid_sFanTest,
    _mid_gpiotest,
    _mid_lvdsonoff,
    _mid_lvdslvl,
    _mid_eeprom,
    _mid_smbreadwrite,
    _mid_slvadrscan,
    _mid_auto,
    _mid_testcode,
    _mid_exit,
} _menuID;
extern int menuSelect;
extern int doneFlag;
/*----doneFlag Start----*/
#define rdEnter         BIT(0)
#define hwmDone         BIT(1)
#define tmlProThdone    BIT(2)
#define tmlProShut      BIT(3)
#define wdIRQNMIdone    BIT(4)
#define wdIRQCldRstDone BIT(5)
#define wdIRQWrmRstDone BIT(6)
#define wdWDPinDone     BIT(7)
#define sfanDone        BIT(8)
#define gpioDone        BIT(9)
#define lvdsOnOffDone   BIT(10)
#define lvdsLvlDone     BIT(11)
#define eepromDone      BIT(12)
#define smbrwDone       BIT(13)
#define scanAdrDone     BIT(14)
/*----doneFlag End----*/

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
#endif //_GLOBAL_H


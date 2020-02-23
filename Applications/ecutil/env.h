#ifndef __ENV_H__
#define __ENV_H__

#ifndef _LINUX_					// define in makefile
#define _UEFI_					// define uefi if it doesn't build with makefile on linux 
#endif

// Include
#ifdef _UEFI_
#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#else
#include <sys/io.h>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#endif

//Typedef
#ifdef _LINUX_
#define UINT8				uint8_t
#define UINT16				uint16_t
#define UINT32				uint32_t
#define IN
#define OUT
#endif

//Constant
#ifdef _LINUX_
#define ENV_STRING			"LINUX"
#define CHAR_SLASH			'/'
#define ERR_EXIT			-1
#define PROGRESS_BUS_STRING	"\033[?25l%s[%-s] %d%%\033[?25h\033[0m\r"
#else
#define ENV_STRING			"UEFI"
#define CHAR_SLASH			'\\'
#define ERR_EXIT			0
#define PROGRESS_BUS_STRING	"%s[%-s] %d%%\r"
#endif

//Function
#ifdef _LINUX_				// only for linux function
#define mCHECK_PERMISSION()	(if(getuid() != 0){\
								printf("ERROR: root permission needed.\n\n");\
								return -1;\
							})
	
#define mOPEN_EC_PORT(addr)	(if(ioperm(addr, 1, 1)){\
								perror("ioperm");\
								return -1;\
							})
								
#define mINB(a)				inb(a)
#define mOUTB(addr,data)	outb(data,addr)
#else						// only for uefi function
#define mCHECK_PERMISSION()
#define mOPEN_EC_PORT(addr)	
#define mINB(a)				IoRead8(a)
#define mOUTB(addr,data)	IoWrite8(addr,data)
#endif


#endif // __ENV_H__

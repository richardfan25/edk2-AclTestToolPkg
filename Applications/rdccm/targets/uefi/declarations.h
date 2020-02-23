#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define _UEFI
#include <Include/unistd.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

/* #define uint8_t		UINT8;
#define uint16_t	UINT16;
#define uint32_t	UINT32;
#define int8_t		INT8;
#define int16_t		INT16;
#define int32_t		INT32;
#define uint64_t	UINT64;
#define int64_t		INT64;
 */

//#define _DEBUG
#ifdef _DEBUG
#define DPRINTF		printf
#else
#define DPRINTF(...)
#endif

#endif /* _DECLARATIONS_H_ */

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - TYPEDEF (Type definition)                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TYPEDEF_H
#define __TYPEDEF_H

//===========================================================================
//  Macro
//===========================================================================
// bit-mask
#define	SET_BIT_MASK(x, bm)		(x |= bm)
#define CLR_BIT_MASK(x, bm)		(x &= ~bm)
#define TST_BIT_MASK(x, bm)		(x & bm)

// bit-pos
#define	SET_BIT_POS(x, bp)		(x |= (1<<bp))
#define CLR_BIT_POS(x, bp)		(x &= ~(1<<bp))
#define TST_BIT_POS(x, bp)		(x & (1<<bp))

//===========================================================================
//  typedef
//===========================================================================
//---------------------------------------------
//  __BORDLANDC__
//---------------------------------------------
#ifdef __BORLANDC__
// Borland C/C++ / Turbo C/C++ for DOS
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;

typedef char            s8;
typedef short           s16;
typedef long            s32;

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;

typedef char            int8_t;
typedef short           int16_t;
typedef long            int32_t;

#endif

//---------------------------------------------
//  __WATCOMC__
//---------------------------------------------
#ifdef __WATCOMC__
#include <stdint.h>
typedef unsigned __int64	uint64_t;
#endif

//---------------------------------------------
//  __DJGPP__ : DJGPP C/C++
//---------------------------------------------


//---------------------------------------------
//  __GNUC__ : GNU C/C++
//---------------------------------------------


//---------------------------------------------
//  __INTEL_COMPILER : Intel C Compiler
//---------------------------------------------


//---------------------------------------------
//  __MSC_VER : Microsoft Visual C/C++
//---------------------------------------------


#endif

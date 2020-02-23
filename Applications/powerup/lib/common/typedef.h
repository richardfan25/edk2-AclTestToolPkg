//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - TYPEDEF (Type definition)                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TYPEDEF_H
#define __TYPEDEF_H

#include <stdint.h>
#include "../../src/base/global.h"
//===========================================================================
//  Macro
//===========================================================================
// BIT :
#define BIT(n)					(1 << (n))

// bm  : bit mask       : (1<<bp)
// bp  : bit posistion  : (0 ~ 31)
// gm  : group mask     : (3<<gp)
// gp  : group position : (0 ~ 31)
// gv  : group value      : (0~)
// gmv : group mask value : shifted group value
// pos : position
// var : variables

// bm
#define	SET_BIT_MASK(x, bm)		(x |= bm)
#define CLR_BIT_MASK(x, bm)		(x &= ~bm)
#define TST_BIT_MASK(x, bm)		(x & bm)

// bp
#define	SET_BIT_POS(x, bp)		(x |= (1<<bp))
#define CLR_BIT_POS(x, bp)		(x &= ~(1<<bp))
#define TST_BIT_POS(x, bp)		(x & (1<<bp))

// VAR : BM (bit mask) : 1-bit
#define SET_VAR_BM(var, bm)		(var |= (bm))
#define CLR_VAR_BM(var, bm)		(var &= ~(bm))
#define TST_VAR_BM(var, bm)		(var & (bm))

// VAR : GM (group mask) : more than 1-bit
#define CLR_VAR_GM(var, gm)		(var &= ~(gm))
#define SET_VAR_GM(var, gm, gmv) \
		do {	\
			CLR_VAR_GM(var, gm); \
			var |= gmv;	\
		} while(0)
#define TST_VAR_GM(var, gm)		(var & gm)

// VAR : GV(group value) : more than 1-bit
#define GET_VAR_GV(var, gm, gp)	((var & gm) >> gp)

#define SET_VAR_GV(var, gm, gp, gv) \
		do {	\
			CLR_VAR_GM(var, gm);	\
			var	|= (gv << gp);		\
		} while (0)
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

#ifndef __A9610_COMMON_H
#define __A9610_COMMON_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

//=============================================================================
//  Common Definition
//=============================================================================
#define __TRUE		0
#define __FALSE		1

//=============================================================================
//  bitwise operations
//=============================================================================
// REG : byte size
#define SET_REG_BM8(bm, reg)            outp(reg, inp(reg) | (bm))
#define CLR_REG_BM8(bm, reg)            outp(reg, inp(reg) & ~(bm))
#define TST_REG_BM8(bm, reg)            ((inp(reg) & (bm)) != 0) 

#define GET_REG_GM8(gm, gp, reg)        ((inp(reg) & gm) >> gp)
#define SET_REG_GM8(gm, gp, gv, reg)    outp(reg, ((inp(reg) & ~(gm)) | (gv << gp)))

// REG : word size
#define SET_REG_BM16(bm, reg)           outpw(reg, inpw(reg) | (bm))
#define CLR_REG_BM16(bm, reg)           outpw(reg, inpw(reg) & ~(bm))
#define TST_REG_BM16(bm, reg)           ((inpw(reg) & (bm)) != 0)

#define GET_REG_GM16(gm, gp, reg)       ((inpw(reg) & gm) >> gp)
#define SET_REG_GM16(gm, gp, gv, reg)   outpw(reg, ((inpw(reg) & ~(gm)) | (gv << gp)))

// REG : dword size
#define SET_REG_BM32(bm, reg)           outpd(reg, inpd(reg) | (bm))
#define CLR_REG_BM32(bm, reg)           outpd(reg, inpd(reg) & ~(bm))
#define TST_REG_BM32(bm, reg)           ((inpd(reg) & (bm)) != 0)

#define GET_REG_GM32(gm, gp, reg)		((inpd(reg) & gm) >> gp)
#define SET_REG_GM32(gm, gp, gv, reg)	outpd(reg, ((inpd(reg) & ~(gm)) | (gv << gp)))


// VAR : bit mask
#define SET_VAR_BM(bm, var)      (var |= (bm))
#define CLR_VAR_BM(bm, var)      (var &= ~(bm))
#define TST_VAR_BM(bm, var)      (var & (bm))

// VAR : group mask ( > 1 bit)
#define CLR_VAR_GM(gm, var)      (var &= ~(gm))
#define SET_VAR_GM(gm, gv, var)	 \
		do {				 \
			CLR_VAR_GM(gm, var); \
			var |= gv;		 \
		} while(0)
#define TST_VAR_GM(gm, var)      (var & gm) 

    
// BIT :
#define BIT(num)   (1 << num)           // num ranage:
                                        //   byte reg: 0~7
                                        //   word reg: 0~15
                                        //   long reg: 0~31
#endif

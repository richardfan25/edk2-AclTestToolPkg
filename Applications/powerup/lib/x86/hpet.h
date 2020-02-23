//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - HPET (High Precision Event Timer)                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

#ifndef __X86_HPET_H
#define __X86_HPET_H

#include "typedef.h"

//===========================================================================
//  define
//===========================================================================
//  HPET Registers : 1KB
//---------------------------------------------------------------------------
//  offset  len.  attr.  registers
//---------------------------------------------------------------------------
//  00h     8     RO     General Capabilities and ID Register
//  08h~0Fh              Reserved
//  10h     8     RW     General Configuration Register
//  18h~1Fh              Reserved
//  20h     8     RWC    General Interrupt Status Register
//  28h~EFh              Reserved
//  F0h     8     RW     Main Counter Value Register
//  F8h~FFh              Reserved
//
//  100h~3FFh : see the reference
//===========================================================================
#define HPET_BASE			0xFED00000
#define HPET_REG_UNIT		(HPET_BASE + 0x04)	// uint32_t : fs = femto second = 10^-15 second
#define HPET_REG_EN			(HPET_BASE + 0x10)
#define HPET_REG_CNT_LSB	(HPET_BASE + 0xF0)
#define HPET_REG_CNT_MSB	(HPET_BASE + 0xF4)
#define HPET_REG_CNT		HPET_REG_CNT_LSB

//#define HPET_1S_CNT		0xDA7A64
//#define HPET_1MS_CNT		0x37EE
//#define HPET_1US_CNT		0xE

//=============================================================================
//  status code
//=============================================================================
#define 	HPET_OK		0
#define		HPET_ERR	1

#pragma pack(1)
//=============================================================================
//  typedef hpet_t
//=============================================================================
typedef struct _hpet_t
{
	uint32_t	*reg_en;
	uint32_t	*reg_unit;

	uint32_t	*reg_cnt_lsb;
	uint32_t	*reg_cnt_msb;
	uint64_t	*reg_cnt;

	uint32_t	cnt_1s;
	uint32_t	cnt_1ms;
	uint32_t	cnt_1us;

	uint32_t	tmout32;
	uint64_t	tmout;

} hpet_t;

#pragma pack()

//=============================================================================
//  extern
//=============================================================================
extern hpet_t	hpet;

//=============================================================================
//  functions
//=============================================================================
void		hpet_start(void);
void		hpet_stop(void);

void		hpet_set_tmout(uint32_t ms);
void		hpet_set_tmout32(uint32_t ms);

uint8_t		hpet_is_tmout(void);
uint8_t		hpet_is_tmout32(void);

void		hpet_delay_ms(uint32_t ms);
void		hpet_delay_us(uint32_t us);

uint64_t	hpet_get_cnt(void);
uint32_t	hpet_get_cnt32(void);

uint8_t		hpet_init(void);
void		hpet_exit(void);

#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - HPET (High Precision Event Timer)                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
// Reference:
// https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/software-developers-hpet-spec-1-0a.pdf
//*****************************************************************************
#ifdef __WATCOMC__

#include "typedef.h"
#include "hpet.h"
#include "bda.h"

//=============================================================================
//  variables
//=============================================================================
hpet_t		hpet;

//=============================================================================
//  hpet_start
//=============================================================================
void hpet_start(void)
{
	*hpet.reg_en = 1;	// enable HPET
}

//=============================================================================
//  hpet_stop
//=============================================================================
void hpet_stop(void)
{
	*hpet.reg_en = 0;	// disable HPET
}

//=============================================================================
//  hpet_set_tmout
//=============================================================================
void hpet_set_tmout(uint32_t ms)
{
	hpet.tmout = (uint64_t)(ms * hpet.cnt_1ms);
}

//=============================================================================
//  hpet_set_tmout32
//=============================================================================
void hpet_set_tmout32(uint32_t ms)
{
	hpet.tmout32 = (uint32_t)(ms * hpet.cnt_1ms);
}

//=============================================================================
//  hpet_is_tmout
//=============================================================================
uint8_t hpet_is_tmout(void)
{
	if (*hpet.reg_cnt > hpet.tmout)
		return 1;

	return 0;
}

//=============================================================================
//  hpet_is_tmout32
//=============================================================================
uint8_t hpet_is_tmout32(void)
{
	if (*hpet.reg_cnt_lsb > hpet.tmout32)
		return 1;

	return 0;
}

//=============================================================================
//  hpet_delay_ms
//=============================================================================
void hpet_delay_ms(uint32_t ms)
{
	uint32_t	tmout;
	
	hpet_stop();
	*hpet.reg_cnt_lsb = 0;
	tmout = (uint32_t)(ms * hpet.cnt_1ms);
	hpet_start();

	do
	{
		if (*hpet.reg_cnt_lsb > tmout)
			break;

	} while (1);
}

//=============================================================================
//  hpet_delay_ms
//=============================================================================
/*
void hpet_delay_ms(uint32_t ms)
{
	hpet_stop();
	hpet.tmout = ms * HPET_1MS_CNT;
	hpet_start();

	while (1)
	{
		if (*hpet.reg_cnt_lsb > hpet.tmout)
			break;
	}
	hpet_stop();
}
*/

//=============================================================================
//  hpet_delay_us
//=============================================================================
void hpet_delay_us(uint32_t us)
{
	uint32_t	tmout;

	hpet_stop();
	*hpet.reg_cnt_lsb = 0;
	tmout = (uint32_t)(us * hpet.cnt_1us);
	hpet_start();

	do
	{
		if (*hpet.reg_cnt_lsb > tmout)
			break;

	} while (1);
}

//=============================================================================
//  hpet_delay_us
//=============================================================================
/*
void hpet_delay_us(uint32_t us)
{
	hpet_stop();
	hpet.tmout = us * HPET_1US_CNT;
	hpet_start();

	while (1)
	{
		if (*hpet.reg_cnt_lsb > hpet.tmout)
			break;
	}
	hpet_stop();
}
*/

//=============================================================================
//  hpet_get_cnt
//=============================================================================
uint64_t hpet_get_cnt(void)
{
	return *hpet.reg_cnt;
}

//=============================================================================
//  hpet_get_cnt32
//=============================================================================
uint32_t hpet_get_cnt32(void)
{
	return *hpet.reg_cnt_lsb;
}

//=============================================================================
//  hpet_init
//=============================================================================
uint8_t hpet_init(void)
{
	uint64_t	dat64;
	uint32_t	uni;

	hpet.reg_en		= (uint32_t *)(HPET_REG_EN);
	hpet.reg_unit	= (uint32_t *)(HPET_REG_UNIT);
	hpet.reg_cnt_lsb= (uint32_t *)(HPET_REG_CNT_LSB);
	hpet.reg_cnt_msb= (uint32_t *)(HPET_REG_CNT_MSB);
	hpet.reg_cnt	= (uint64_t *)(HPET_REG_CNT);

	uni = *hpet.reg_unit;	// fs (10^-15)
	uni /= 1000;			// fs -> ps (10^-12)

	dat64 = (uint64_t)1000000000000;	// 1s = 10^12 ps
	dat64 /= uni;						// cnt/s = 1s / uni

	hpet.cnt_1s = (uint32_t)dat64;		// cnt/s
	hpet.cnt_1ms = hpet.cnt_1s / 1000;	// cnt/ms (10^-3)
	hpet.cnt_1us = hpet.cnt_1ms / 1000;	// cnt/us (10^-6)

	// disable HPET
	*hpet.reg_en = 0;

	// clear HPET
	*hpet.reg_cnt_lsb = 0;
	*hpet.reg_cnt_msb = 0;

	// enable HPET
	*hpet.reg_en = 1;

		return HPET_OK;
}

//=============================================================================
//  hpet_exit : __WATCOMC__
//=============================================================================
void hpet_exit(void)
{
	*hpet.reg_en = 0;	// disable HPET
}

#endif

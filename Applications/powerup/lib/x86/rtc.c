//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - RTC (Real Time Clock)                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <dos.h>
#include "typedef.h"


#include "x86io.h"

//===========================================================================
//  __BORLANDC__
//===========================================================================
#ifdef __BORLANDC__

#include "bcdhex.h"
//===========================================================================
//  Get RTC time
//===========================================================================
void tmr_get_rtc_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *daylight)
{
	union	REGS 	reg;

	reg.h.ah = 0x02;
	int86(0x1A, &reg, &reg);
	*hour = bcd2hex(reg.h.ch);
	*min  = bcd2hex(reg.h.cl);
	*sec  = bcd2hex(reg.h.dh);
	*daylight = reg.h.dl;
}
//===========================================================================
//  Set RTC time
//===========================================================================
void tmr_set_rtc_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t daylight)
{
	union	REGS 	reg;

	reg.h.ah = 0x03;
	reg.h.ch = hex2bcd(hour);
	reg.h.cl = hex2bcd(min);
	reg.h.dh = hex2bcd(sec);
	reg.h.dl = daylight;
	int86(0x1A, &reg, &reg);
}
//===========================================================================
//  Get RTC date
//===========================================================================
void tmr_get_rtc_date(uint8_t *cent, uint8_t *year, uint8_t *mon, uint8_t *day)
{
	union	REGS 	reg;
	uint32_t		tmr;

	reg.h.ah = 0x04;
	int86(0x1A, &reg, &reg);
	*cent = bcd2hex(reg.h.ch);
	*year = bcd2hex(reg.h.cl);
	*mon  = bcd2hex(reg.h.dh);
	*day  = bcd2hex(reg.h.dl);
}
//===========================================================================
//  Set RTC date
//===========================================================================
void tmr_set_rtc_date(uint8_t cent, uint8_t year, uint8_t mon, uint8_t day)
{
	union	REGS 	reg;

	reg.h.ah = 0x05;
	reg.h.ch = hex2bcd(cent);
	reg.h.cl = hex2bcd(year);
	reg.h.dh = hex2bcd(mon);
	reg.h.dl = hex2bcd(day);
	int86(0x1A, &reg, &reg);
}
#endif

//===========================================================================
//  rtc_start_clock
//===========================================================================
void rtc_start_clock(void)
{
	uint8_t	value;

	outb(0x70, 0x0B);
	value = inb(0x71);
	value &= ~0x80;
	outb(0x71, value);
}

//===========================================================================
//  rtc_stop_clock
//===========================================================================
void rtc_stop_clock(void)
{
	uint8_t	value;

	outb(0x70, 0x0B);
	value = inb(0x71);
	value |= 0x80;
	outb(0x71, value);
}

//===========================================================================
//  rtc_read_reg
//===========================================================================
uint8_t rtc_read_reg(uint8_t reg)
{
	outb(0x70, reg);

	return inb(0x71);
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - RTC (Real Time Clock)                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_RTC_H
#define __X86_RTC_H

//=============================================================================
//  functions
//=============================================================================

#ifdef __BORLANDC__
void tmr_get_rtc_time(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *daylight);
void tmr_set_rtc_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t daylight);
void tmr_get_rtc_date(uint8_t *cent, uint8_t *year, uint8_t *mon, uint8_t *day);
void tmr_set_rtc_date(uint8_t cent, uint8_t year, uint8_t mon, uint8_t day);
#endif

void rtc_start_clock(void);
void rtc_stop_clock(void);
uint8_t rtc_read_reg(uint8_t reg);

#endif

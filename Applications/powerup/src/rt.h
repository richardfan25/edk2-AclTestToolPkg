//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  RT : Run-Time                                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __RT_H
#define __RT_H

#include "pu.h"

//=============================================================================
//  extern
//=============================================================================
extern const char week_str[7][4];

//=============================================================================
//  functions
//=============================================================================
uint8_t rt_tmr_handler(void);
int		rt_sort_no_inc(const void *a, const void *b);
int		rt_sort_no_dec(const void *a, const void *b);
int		rt_sort_intv_inc(const void *a, const void *b);
int		rt_sort_intv_dec(const void *a, const void *b);
int		rt_sort_cpu_temp_inc(const void *a, const void *b);
int		rt_sort_cpu_temp_dec(const void *a, const void *b);
int		rt_sort_cpu_tsc_inc(const void *a, const void *b);
int		rt_sort_cpu_tsc_dec(const void *a, const void *b);
uint8_t rt_clear_rec(void);

#endif

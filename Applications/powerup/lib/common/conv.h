//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - CONV (Convert)                                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __COMMON_CONV_H
#define __COMMON_CONV_H

#include <time.h>

#include "typedef.h"
//=============================================================================
//  extern
//=============================================================================
extern const char	week_str[7][4];

//=============================================================================
//  functions
//=============================================================================
uint8_t  bcd2hex(uint8_t n);
uint8_t  hex2bcd(uint8_t n);

uint16_t swap16(uint16_t data);
uint32_t swap32(uint32_t data);

int str2hex8(char *str, uint8_t *data8);
int str2hex16(char *str, uint16_t *data16);
int str2hex32(char *str, uint32_t *data32);

void conv_tm_str(time_t *now, char *str);
void conv_tm_week_str(time_t *tm, char *str);
void conv_tm_elapsed_str(time_t tm0, time_t tm1, char *str);

#endif

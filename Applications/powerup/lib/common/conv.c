//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - CONV (Convert)                                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "typedef.h"

const char	week_str[7][4] = { "SUN", "MON", "TUE", "WED","THU", "FRI", "SAT" };
//===========================================================================
//  bcd2hex
//===========================================================================
uint8_t bcd2hex(uint8_t n)
{
	return	((((n >> 4) & 0x0F) * 10) + (n & 0x0F));
}

//===========================================================================
//  hex2bcd
//===========================================================================
uint8_t hex2bcd(uint8_t n)
{
	return (((n / 10) << 4) | (n % 10));
}

//===========================================================================
//  swap16
//===========================================================================
uint16_t swap16(uint16_t data)
{
	uint16_t    lsb = (data >> 8) & 0x00FF;
	uint16_t    msb = (data << 8) & 0xFF00;

	return (msb | lsb);
}

//===========================================================================
//  swap32
//===========================================================================
uint32_t swap32(uint32_t data)
{
	uint32_t    b0 = (data >> 24) & 0x000000FF;
	uint32_t    b1 = (data >> 8)  & 0x0000FF00;
	uint32_t    b2 = (data << 8)  & 0x00FF0000;
	uint32_t    b3 = (data << 24) & 0xFF000000;

	return (b3 | b2 | b1 | b0);
}

//===========================================================================
//  str2hex8
//===========================================================================
int str2hex8(char *str, uint8_t *data8)
{
	int		len = (int)strlen(str);
	uint8_t	res = 0;
	int		i;
	char	c;

	if (len > 2 || len < 1)
		return -1;

	for (i=0; i<len; i++)
	{
		if (!((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'F') ||
			(c >= 'a' && c <= 'f')))
			return -1;
	}
	
	for (i=0; i<len; i++)
	{
		c = str[i];
		res <<= 4;

		if (c >= '0' && c <= '9')
			res |= (c & 0xf);
		else if (c >= 'A' && c <= 'F')
			res |= (c - 'A' + 10);
		else if (c >= 'a' && c <= 'f')
			res |= (c - 'a' + 10);
	}

	*data8 = res;

	return 0;
}

//===========================================================================
//  str2hex16
//===========================================================================
int str2hex16(char *str, uint16_t *data16)
{
	int			len = (int)strlen(str);
	uint16_t	res = 0;
	int			i;
	char		c;

	if (len > 4 || len < 1)
		return -1;

	for (i=0; i<len; i++)
	{
		if (!((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'F') ||
			(c >= 'a' && c <= 'f')))
			return -1;
	}
	
	for (i=0; i<len; i++)
	{
		c = str[i];
		res <<= 4;

		if (c >= '0' && c <= '9')
			res |= (c & 0xf);
		else if (c >= 'A' && c <= 'F')
			res |= (c - 'A' + 10);
		else if (c >= 'a' && c <= 'f')
			res |= (c - 'a' + 10);
	}

	*data16 = res;
	
	return 0;
}

//===========================================================================
//  str2hex32
//===========================================================================
int str2hex32(char *str, uint32_t *data32)
{
	int			len = (int)strlen(str);
	uint32_t	res = 0;
	int			i;
	char		c;

	if (len > 8 || len < 1)
		return -1;

	for (i=0; i<len; i++)
	{
		if (!((c >= '0' && c <= '9') ||
			(c >= 'A' && c <= 'F') ||
			(c >= 'a' && c <= 'f')))
			return -1;
	}
	
	for (i=0; i<len; i++)
	{
		c = str[i];
		res <<= 4;

		if (c >= '0' && c <= '9')
			res |= (c & 0xf);
		else if (c >= 'A' && c <= 'F')
			res |= (c - 'A' + 10);
		else if (c >= 'a' && c <= 'f')
			res |= (c - 'a' + 10);
	}

	*data32 = res;
	
	return 0;
}

//=============================================================================
//  conv_tm_str
//=============================================================================
void conv_tm_str(time_t *tm, char *str)
{
	struct 	tm	*utc;

	utc = localtime(tm);
	sprintf(str, "%04d/%02d/%02d %02d:%02d:%02d",
				utc->tm_year+1900,	utc->tm_mon+1,	utc->tm_mday,
				utc->tm_hour, 		utc->tm_min,	utc->tm_sec);
}

//=============================================================================
//  conv_tm_week_str
//=============================================================================
void conv_tm_week_str(time_t *tm, char *str)
{
	struct 	tm	*utc;

	utc = localtime(tm);
	sprintf(str, "%04d/%02d/%02d %s %02d:%02d:%02d",
				utc->tm_year+1900,	utc->tm_mon+1,	utc->tm_mday, week_str[utc->tm_wday],
				utc->tm_hour, 		utc->tm_min,	utc->tm_sec);
}

//=============================================================================
//  conv_tm_elapsed_str
//=============================================================================
void conv_tm_elapsed_str(time_t tm0, time_t tm1, char *str)
{
	int32_t		elapsed_sec;

	elapsed_sec = (int32_t)tm1 - (int32_t)tm0;

	if (elapsed_sec < 0)
	{
		elapsed_sec *= (-1);
		sprintf(str, "-%5lddays -%2ld:%02ld:%02ld",
						elapsed_sec/3600/24,
						(elapsed_sec/3600)%24, (elapsed_sec/60)%60, elapsed_sec%60);
	}
	else
	{
		sprintf(str, "%5ld days %2ld:%02ld:%02ld",
						elapsed_sec/3600/24,
						(elapsed_sec/3600)%24, (elapsed_sec/60)%60, elapsed_sec%60);
	}
}

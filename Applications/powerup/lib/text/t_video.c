//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_VIDEO (Text Video)                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//  dependancy : VGA.C, VGA.H
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#include <bios.h>
//#include <conio.h>
#include <time.h>

//#include "typedef.h"
#include "t_video.h"
#include "video.h"

#ifdef __BORLANDC__
#define T_VIDEO_BASE()	tv.buf=(uint8_t far *)T_BASE
#endif

#ifdef __WATCOMC__
#define T_VIDEO_BASE()	tv.buf=(uint8_t *)T_BASE
#endif
//=============================================================================
//  variables
//=============================================================================
t_video_info_t		tv;
uint8_t *gTVBASE = NULL;

//=============================================================================
//  t_color
//=============================================================================
void t_color(uint8_t fg, uint8_t bg)
{
	tv.color = (bg << 4) + fg;
	gST->ConOut->SetAttribute(gST->ConOut, EFI_TEXT_ATTR(fg, bg));
}

//=============================================================================
//  t_fg_color
//=============================================================================
void t_fg_color(uint8_t fg)
{
	tv.color &= ~0x0F;
	tv.color |= (fg & 0x0F);
	t_color(fg, tv.color >> 4);
}

//=============================================================================
//  t_bg_color
//=============================================================================
void t_bg_color(uint8_t bg)
{
	tv.color &= ~0xF0;
	tv.color |= (bg<<4);
	t_color(tv.color & 0x0F, bg);
}

//=============================================================================
//  t_bl_color
//=============================================================================
void t_bl_color(uint8_t blink)
{
	if (blink)
		tv.color |= T_MASK_BL;		// bit7 = blink color
	else
		tv.color &= ~T_MASK_BL;
}

//=============================================================================
//  t_get_color
//=============================================================================
uint8_t t_get_color(void)
{
	return tv.color;
}

//=============================================================================
//  t_set_color
//=============================================================================
void t_set_color(uint8_t color)
{
	tv.color = color;
}

//=============================================================================
//  t_xy_get_color
//=============================================================================
uint8_t t_xy_get_color(int x, int y)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();
	
	tv.buf += (((y*5)<<5) + (x<<1));
	tv.buf++;
	
	return (*tv.buf);//tv.color;
}

//=============================================================================
//  t_gotoxy
//=============================================================================
void t_gotoxy(int x, int y)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	tv.x = x;
	tv.y = y;

	//video_set_cur_pos(x, y);
	
	gST->ConOut->SetCursorPosition(gST->ConOut, x, y);
}
//=============================================================================
//  t_clear_screen
//=============================================================================
void t_clear_screen(void)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	for (i=0; i<T_CHAR_SZ; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = tv.color;
	}
	
	gST->ConOut->ClearScreen(gST->ConOut);
}
//=============================================================================
//  t_clear_color_screen
//=============================================================================
void t_clear_color_screen(uint8_t fg, uint8_t bg)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.color = (bg << 4) + fg;

	for (i=0; i<T_CHAR_SZ; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = tv.color;
	}
	
	t_color(T_LIGHTGRAY, T_LIGHTGRAY);
	gST->ConOut->ClearScreen(gST->ConOut);
	tv.buf[3999] = tv.color;

	t_color(fg, bg);
	for (i=0; i<T_CHAR_SZ-1; i++)
	{
		Print(L" ");
	}
}
//=============================================================================
//  t_fill_screen
//=============================================================================
void t_fill_screen(uint8_t c)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	for (i=0; i<T_CHAR_SZ; i++)
	{
		*tv.buf++ = c;
		*tv.buf++ = tv.color;
	}
	
	t_color(tv.color & 0x0F, tv.color >> 4);
	for (i=0; i<T_CHAR_SZ-1; i++)
	{
		Print(L"%c", c);
	}
}
//=============================================================================
//  t_fill_color_screen
//=============================================================================
void t_fill_color_screen(uint8_t fg, uint8_t bg, uint8_t c)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();
	
	tv.color = (bg << 4) + fg;

	for (i=0; i<T_CHAR_SZ; i++)
	{
		*tv.buf++ = c;
		*tv.buf++ = tv.color;
	}
	
	t_color(T_LIGHTGRAY, T_LIGHTGRAY);
	gST->ConOut->ClearScreen(gST->ConOut);
	tv.buf[3999] = tv.color;
	
	t_color(fg, bg);
	for (i=0; i<T_CHAR_SZ-80; i++)
	{
		Print(L"%c", c);
	}
}
//=============================================================================
//  t_fill_region
//=============================================================================
void t_fill_region(int sx, int sy, int ex, int ey, uint8_t c)
{
	int		i, j;

	for (j=sy; j<=ey; j++)
	{
		tv.buf = gTVBASE;//T_VIDEO_BASE();
		
		tv.buf += (((j*5)<<5) + (sx<<1));
		
		for (i=sx; i<=ex; i++)
		{
			*tv.buf++ = c;
			*tv.buf++ = tv.color;
		}
	}
	
	t_color(tv.color & 0x0F, tv.color >> 4);
	for (j=sy; j<=ey; j++)
	{
		for (i=sx; i<=ex; i++)
		{
			t_gotoxy(i, j);
			Print(L"%c", c);
		}
	}
}

//=============================================================================
//  t_print_unicode
//=============================================================================
void t_print_unicode(char c)
{
	switch((uint8_t)c)
	{
		case 0x18:
		{
			Print(L"%c", 0x2191);
			break;
		}
		case 0x19:
		{
			Print(L"%c", 0x2193);
			break;
		}
		case 0x1A:
		{
			Print(L"%c", 0x2192);
			break;
		}
		case 0x1B:
		{
			Print(L"%c", 0x2190);
			break;
		}
		case 0x1E:
		{
			Print(L"%c", 0x25B2);
			break;
		}
		case 0xB0:
		case 0xB2:
		{
			Print(L"%c", 0x2591);
			break;
		}
		case 0xB3:
		case 0xBA:
		{
			Print(L"%c", 0x2502);
			break;
		}
		case 0xB4:
		case 0xB5:
		case 0xB6:
		case 0xB9:
		{
			Print(L"%c", 0x2561);
			break;
		}
		case 0xBB:
		case 0xBF:
		{
			Print(L"%c", 0x2510);
			break;
		}
		case 0xBC:
		case 0xD9:
		{
			Print(L"%c", 0x2518);
			break;
		}
		case 0xC0:
		case 0xC8:
		{
			Print(L"%c", 0x2514);
			break;
		}
		case 0xC3:
		case 0xC6:
		case 0xC7:
		case 0xCC:
		{
			Print(L"%c", 0x255E);
			break;
		}
		case 0xC4:
		{
			Print(L"%c", 0x2500);
			break;
		}
		case 0xC9:
		case 0xDA:
		{
			Print(L"%c", 0x250C);
			break;
		}
		case 0xCD:
		{
			Print(L"%c", 0x2550);
			break;
		}
		case 0xDB:
		{
			Print(L"%c", 0x2588);
			break;
		}
		case 0xF8:
		{
			Print(L"%c", 0x00B0);
			break;
		}
		case 0xFB:
		{
			Print(L"%c", 'V');
			break;
		}
		default:
		{
			Print(L"%c", c);
			break;
		}
	}
}

//=============================================================================
//  t_paste_region_cl_putc
//=============================================================================
void t_paste_region_cl_putc(uint8_t fg, uint8_t bg, char c)
{
	t_color(fg, bg);
	t_print_unicode(c);
}

//=============================================================================
//  t_putc
//=============================================================================
void t_putc(char c)
{
	*tv.buf++ = c;
	*tv.buf++ = tv.color;
	
	t_color(tv.color & 0x0F, tv.color >> 4);
	t_print_unicode(c);
}

//=============================================================================
//  t_puts
//=============================================================================
void t_puts(char *s)
{
	int		i;
	int		len;
	char	*ps;
	
	uint16_t s16[80] = {'\0'};

	len = (int)strlen(s);
	ps = s;

	for (i=0; i<len; i++)
	{
		if(*ps != '\0')
		{
			switch((uint8_t)*ps)
			{
				case 0x18:
				{
					s16[i] = 0x2191;
					break;
				}
				case 0x19:
				{
					s16[i] = 0x2193;
					break;
				}
				case 0x1B:
				{
					s16[i] = 0x2190;
					break;
				}
				case 0x1A:
				{
					s16[i] = 0x2192;
					break;
				}
				case 0x1E:
				{
					s16[i] = 0x25B2;
					break;
				}
				case 0x1F:
				{
					s16[i] = 0x25BC;
					break;
				}
				case 0xB2:
				{
					s16[i] = 0x2591;
					break;
				}
				case 0xF8:
				{
					s16[i] = 0x00B0;
					break;
				}
				default:
				{
					s16[i] = (uint16_t)*ps;
					break;
				}
			}
		}
		
		*tv.buf++ = *ps++;
		*tv.buf++ = tv.color;
	}
	s16[i] = '\0';
	
	t_color(tv.color & 0x0F, tv.color >> 4);
	Print(L"%s", s16);
}

//=============================================================================
//  t_putcs
//=============================================================================
void t_putcs(char *cs)
{
	int			i;
	int			len;
	uint8_t		col;
	char		c;
	
	len = (int)strlen(cs);

	// do not change current color
	col = tv.color;
	t_color(col & 0x0F, col >> 4);

	for (i=0; i<len;)
	{
		c = cs[i++];

		if (c == 0x08)	// \b : bg
		{
			c = cs[i++];

			col &= 0x0F;
			col |= ((c & 0xF) << 4);
			t_color(col & 0x0F, col >> 4);
			continue;
		}
		else if (c == 0x0C)	// \f : fg
		{
			c = cs[i++];
	
			col &= 0xF0;
			col |= (c & 0xF);
			t_color(col & 0x0F, col >> 4);
			continue;
		}
		*tv.buf++ = c;
		*tv.buf++ = col;

		t_print_unicode(c);
	}
}

//=============================================================================
//  t_putd : 32-bits
//=============================================================================
void t_putd(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	uint32_t	div;		// divisor
	uint8_t		digit;
	uint8_t		digit0;
	uint8_t		dcnt;
	uint8_t		len;
	uint8_t		out;

	t_color(tv.color & 0x0F, tv.color >> 4);
	
	// align = 0 : right aligned with padding
	// align = 1 : left aligned without padding
	dcnt = 10;
	out = 0;
	len = 0;
	digit0 = 1;
	for (div=1000000000; div>0; div/=10, dcnt--)
	{
		// get each digit
		digit = (uint8_t)(data / div);

		if (digit)
			digit0 = 0;	// reach the highest digit of data
		else if (div == 1)
			digit0 = 0;	// only 1-digit

		// not yet reach high + left align
		if (digit0 && align)
			continue;

		// padlen = 2, data = 8,  output = "08"  or " 8"
		// padlen = 1, data = 8,  output = "8"   or "8"
		// padlen = 3, data = 14, output = "014" or " 14"
		if (dcnt <= padlen)
			out = 1;

		// if digit <> 0, set output flag
		// if data=0, we want to output '0' instead of "00000"
		// if div==1, forced output, if only one digit (data=0~9)
		if (digit || div == 1)
			out = 1;

		// output
		if (out)
		{
			if (out > 1)
				digit = (uint8_t)(data / div);	// get digit

			if (digit0)
			{
				// not yet reach highest digit of data
				if (pad0)
				{
					*tv.buf++ = '0';	// padding 0
					Print(L"0");
				}
				else
				{
					*tv.buf++ = ' ';	// padding space
					Print(L" ");
				}
			}
			else
			{
				*tv.buf++ = (digit + '0');
				Print(L"%d", digit);
			}
				
			*tv.buf++ = tv.color;
			
			len++;

			data -= (digit*div);
			out++;
		}
	}

	// left-align
	if (align && (padlen > len))
	{
		for (div=len; div<padlen; div++)
		{
			*tv.buf++ = ' ';	// padding space
			*tv.buf++ = tv.color;
			Print(L" ");
		}
	}
	
}

//=============================================================================
//  t_putx : 32-bits
//=============================================================================
void t_putx(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	int			off;
	uint8_t		digit;
	uint8_t		digit0;
	uint8_t		dcnt;
	uint8_t		out;
	uint8_t		len;

	t_color(tv.color & 0x0F, tv.color >> 4);
	
	digit0 = 1;
	dcnt = 8;
	out = 0;
	len = 0;
	
	for (off=28; off >=0; off -=4, dcnt--)
	{
		// get digit, 4-bits
		digit = (data >> off) & 0xF;

		if (digit)
			digit0 = 0;	// reach the highest digit of data
		else if (off == 0)
			digit0 = 0;	// only 1-digit
		else if (dcnt <= padlen)
			digit0 = 0;

		// not yet reach high + left align
		if (digit0 && align)
			continue;

		// padlen = 2, data = 8,  output = "08"  or " 8"
		// padlen = 1, data = 8,  output = "8"   or "8"
		// padlen = 3, data = 14, output = "014" or " 14"
		if (dcnt <= padlen)
			out = 1;

		// if digit <> 0, set output flag
		// if data=0, we want to output '0' instead of "00000"
		if (digit || off == 0)
			out = 1;

		// output
		if (out)
		{
			if (digit0)
			{
				// not yet reach highest digit of data
				if (pad0)
				{
					*tv.buf++ = '0';	// padding 0
					Print(L"0");
				}
				else
				{
					*tv.buf++ = ' ';	// padding space
					Print(L" ");
				}
			}
			else
			{
				if (digit > 9)
					*tv.buf++ = (digit + '7');
				else
					*tv.buf++ = (digit + '0');
				Print(L"%d", digit);
			}
				
			*tv.buf++ = tv.color;
			len++;

			out++;
		}
	}

	// left-align
	if (align && (padlen > len))
	{
		for (out=len; out<padlen; out++)
		{
			*tv.buf++ = ' ';	// padding space
			*tv.buf++ = tv.color;
			Print(L" ");
		}
	}
	
}

//=============================================================================
//  t_putb : 32-bits
//=============================================================================
void t_putb(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	int			off;
	uint8_t		digit;
	uint8_t		digit0;
	uint8_t		dcnt;
	uint8_t		out;
	uint8_t		len;

	t_color(tv.color & 0x0F, tv.color >> 4);
	
	digit0 = 1;
	dcnt = 32;
	out = 0;
	len = 0;
	
	for (off=31; off >=0; off --, dcnt--)
	{
		// get digit, 4-bits
		digit = (data >> off) & 0x1;

		if (digit)
			digit0 = 0;	// reach the highest digit of data
		else if (off == 0)
			digit0 = 0;	// only 1-digit

		// not yet reach high + left align
		if (digit0 && align)
			continue;

		// padlen = 2, data = 8,  output = "08"  or " 8"
		// padlen = 1, data = 8,  output = "8"   or "8"
		// padlen = 3, data = 14, output = "014" or " 14"
		if (dcnt <= padlen)
			out = 1;

		// if digit <> 0, set output flag
		// if data=0, we want to output '0' instead of "00000"
		if (digit || off == 0)
			out = 1;

		// output
		if (out)
		{
			if (digit0)
			{
				// not yet reach highest digit of data
				if (pad0)
				{
					*tv.buf++ = '0';	// padding 0
					Print(L"0");
				}
				else
				{
					*tv.buf++ = ' ';	// padding space
					Print(L" ");
				}
			}
			else
			{
				if (digit)
					*tv.buf++ = '1';
				else
					*tv.buf++ = '0';
				Print(L"%d", digit);
			}
				
			*tv.buf++ = tv.color;
			len++;

			out++;
		}
	}

	// left-align
	if (align && (padlen > len))
	{
		for (out=len; out<padlen; out++)
		{
			*tv.buf++ = ' ';	// padding space
			*tv.buf++ = tv.color;
			Print(L" ");
		}
	}
	
}

//=============================================================================
//  t_printf
//=============================================================================
void t_printf(const char *fmt, ...)
{
	va_list		args;
	char		*ptr;
	uint8_t		pad0;
	uint8_t		maxlen;
	uint8_t		align;
	char		c;

	align = 0;

	va_start(args, fmt);
	ptr = (char *)fmt;

	while (*ptr)
	{

		switch (*ptr) {

		case '\\':
			if (*++ptr)
				ptr++;
			continue;

		case '%':
			// %-06X
			if (*++ptr == '-')
			{
				align = 1;	// left-alignment
			}
			else
			{
				ptr--;
				align = 0;
			}

			if (*++ptr == '0')
			{
				pad0 = 1;
			}
			else
			{
				pad0 = 0;
				ptr--;
			}

			c = *++ptr;
			if ((c>='1') && (c<='9'))
			{
				maxlen = (c & 0x0F);	// 1~9
			}
			else
			{
				ptr--;
				maxlen = 0;
			}

			// switch-2
			switch(*++ptr)
			{

			case NULL:
				continue;

			case 'c':
				t_putc(va_arg(args, char));
				break;

			case 's':
				t_puts(va_arg(args, char*));
				break;

			case 'd':
				t_putd(va_arg(args, unsigned int), pad0, maxlen, align);
				break;

			case 'l':
				// switch-3
				switch(*++ptr)
				{
				case 'u':
					t_putd(va_arg(args, unsigned long), pad0, maxlen, align);
					break;

				case 'x':
					t_putx(va_arg(args, unsigned long), pad0, maxlen, 0);
					break;

				case 'X':
					t_putx(va_arg(args, unsigned long), pad0, maxlen, 1);
					break;

				case 'b':
					t_putb(va_arg(args, unsigned long), pad0, maxlen, align);
					break;

				}//end-switch-3

				break;

			case 'x':
				t_putx(va_arg(args, unsigned int), pad0, maxlen, 0);
				break;

			case 'X':
				t_putx(va_arg(args, unsigned int), pad0, maxlen, 1);
				break;

			case 'b':
				t_putb(va_arg(args, unsigned int), pad0, maxlen, align);
				break;

			}//end-switch-2

			ptr++;
			break;

		default:
			t_putc(*ptr++);
			break;

		}//end-switch

	}//end-while
	
	va_end(args);
}

//=============================================================================
//  t_xy_putc
//=============================================================================
void t_xy_putc(int x, int y, char c)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	*tv.buf++ = c;
	*tv.buf++ = tv.color;
	
	t_gotoxy(x, y);
	t_putc(c);
}

//=============================================================================
//  t_xy_puts
//=============================================================================
void t_xy_puts(int x, int y, char *s)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	t_gotoxy(x, y);
	t_puts(s);
}

//=============================================================================
//  t_xy_putcs
//=============================================================================
void t_xy_putcs(int x, int y, char *cs)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	t_gotoxy(x, y);
	t_putcs(cs);
}

//=============================================================================
//  t_xy_putd
//=============================================================================
void t_xy_putd(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	t_gotoxy(x, y);
	t_putd(data, pad0, padlen, align);
}

//=============================================================================
//  t_xy_putx
//=============================================================================
void t_xy_putx(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	t_gotoxy(x, y);
	t_putx(data, pad0, padlen, align);
}

//=============================================================================
//  t_xy_putb
//=============================================================================
void t_xy_putb(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align)
{
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	t_gotoxy(x, y);
	t_putb(data, pad0, padlen, align);
}

//=============================================================================
//  t_xy_cl_putc
//=============================================================================
void t_xy_cl_putc(int x, int y, uint8_t fg, uint8_t bg, char c)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
	
	*tv.buf++ = c;
	*tv.buf++ = tv.color;
	
	t_gotoxy(x, y);
	t_putc(c);
}

//=============================================================================
//  t_xy_cl_puts
//=============================================================================
void t_xy_cl_puts(int x, int y, uint8_t fg, uint8_t bg, char *s)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	t_gotoxy(x, y);
	t_puts(s);
}

//=============================================================================
//  t_xy_cl_putcs
//=============================================================================
void t_xy_cl_putcs(int x, int y, uint8_t fg, uint8_t bg, char *s)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	t_gotoxy(x, y);
	t_putcs(s);
}

//=============================================================================
//  t_xy_cl_putd
//=============================================================================
void t_xy_cl_putd(int x, int y, uint8_t fg, uint8_t bg, uint32_t data)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	t_gotoxy(x, y);
	t_putd(data, 0, 0, 1);	// no pad0, no padlen, left align
}

//=============================================================================
//  t_xy_cl_putx
//=============================================================================
void t_xy_cl_putx(int x, int y, uint8_t fg, uint8_t bg, uint32_t data)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	t_gotoxy(x, y);
	t_putx(data, 0, 0, 1);	// no pad0, no padlen, left align
}

//=============================================================================
//  t_xy_cl_putb
//=============================================================================
void t_xy_cl_putb(int x, int y, uint8_t fg, uint8_t bg, uint32_t data)
{
	tv.color = (bg << 4) + fg;
	
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	t_gotoxy(x, y);
	t_putb(data, 0, 0, 1);	// no pad0, no padlen, left align
}

//=============================================================================
//  t_put_hline
//=============================================================================
void t_put_hline(int x, int y, int len)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	for (i=0; i<len; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = tv.color;
	}
	
	t_gotoxy(x, y);
	t_color(tv.color & 0x0F, tv.color >> 4);
	
	for (i=0; i<len; i++)
	{
		Print(L" ");
	}
}
//=============================================================================
//  t_put_vline
//=============================================================================
void t_put_vline(int x, int y, int len)
{
	int		i;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
		
	for (i=0; i<len; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = tv.color;

		tv.buf += (T_LINE_SZ - 2);
	}
	
	t_gotoxy(x, y);
	t_color(tv.color & 0x0F, tv.color >> 4);
		
	for (i=0; i<len; i++)
	{
		t_gotoxy(x, y + i);
		Print(L" ");
	}
}

//=============================================================================
//  t_cl_put_hline
//=============================================================================
void t_cl_put_hline(int x, int y, uint8_t fg, uint8_t bg, int len)
{
	int		i;
	uint8_t	color = (bg << 4) + fg;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));

	for (i=0; i<len; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = color;
	}
	
	t_gotoxy(x, y);
	t_color(color & 0x0F, color >> 4);
		
	for (i=0; i<len; i++)
	{
		Print(L" ");
	}
}

//=============================================================================
//  t_cl_put_vline
//=============================================================================
void t_cl_put_vline(int x, int y, uint8_t fg, uint8_t bg, int len)
{
	int		i;
	uint8_t	color = (bg << 4) + fg;

	tv.buf = gTVBASE;//T_VIDEO_BASE();

	tv.buf += (((y*5)<<5) + (x<<1));
		
	for (i=0; i<len; i++)
	{
		*tv.buf++ = ' ';
		*tv.buf++ = color;

		tv.buf += (T_LINE_SZ - 2);
	}
	
	t_gotoxy(x, y);
	t_color(color & 0x0F, color >> 4);
		
	for (i=0; i<len; i++)
	{
		t_gotoxy(x, y + i);
		Print(L" ");
	}
}

//=============================================================================
//  t_mark_hline
//=============================================================================
void t_mark_hline(int x, int y, int len, uint8_t mark)
{
	int			i;
	uint8_t		color;

	t_gotoxy(x, y);

	t_bg_color(mark);
	t_color(tv.color & 0x0F, tv.color >> 4);

	for (i=0; i<len; i++)
	{
		color = *++tv.buf;
		color &= 0x0F;
		color |= (mark<<4);
		*tv.buf++ = color;
		Print(L" ");
	}
}

//=============================================================================
//  t_mark_vline
//=============================================================================
void t_mark_vline(int x, int y, int len, uint8_t mark)
{
	int			i;
	uint8_t		color;

	t_bg_color(mark);
	t_color(tv.color & 0x0F, tv.color >> 4);

	for (i=0; i<len; i++)
	{
		t_gotoxy(x, y+i);

		color = *++tv.buf;
		color &= 0x0F;
		color |= (mark<<4);
		*tv.buf++ = color;
		Print(L" ");
	}
}

//=============================================================================
//  t_pen_hline
//=============================================================================
void t_pen_hline(int x, int y, int len, uint8_t pen)
{
	int			i;
	uint8_t		color;

	t_gotoxy(x, y);

	t_fg_color(pen);
	t_color(tv.color & 0x0F, tv.color >> 4);

	for (i=0; i<len; i++)
	{
		color = *++tv.buf;
		color &= 0xF0;
		color |= pen;
		*tv.buf++ = color;
		Print(L" ");
	}
}

//=============================================================================
//  t_pen_vline
//=============================================================================
void t_pen_vline(int x, int y, int len, uint8_t pen)
{
	int			i;
	uint8_t		color;

	t_fg_color(pen);
	t_color(tv.color & 0x0F, tv.color >> 4);

	for (i=0; i<len; i++)
	{
		t_gotoxy(x, y+i);

		color = *++tv.buf;
		color &= 0xF0;
		color |= pen;
		*tv.buf++ = color;
		Print(L" ");
	}
}

//=============================================================================
//  t_shadow_char
//=============================================================================
void t_shadow_char(int x, int y)
{
	t_gotoxy(x, y);
	*++tv.buf = T_DARKGRAY;
	gST->ConOut->SetAttribute(gST->ConOut, EFI_TEXT_ATTR(*tv.buf & 0x0F, *tv.buf >> 4));
	t_print_unicode(*(tv.buf-1));
	t_color(tv.color & 0x0F, tv.color >> 4);
}

//=============================================================================
//  t_put_rect
//=============================================================================
void t_put_rect(int sx, int sy, int w, int h)
{
	t_put_hline(sx,     sy,     w);
	t_put_hline(sx,     sy+h-1, w);
	t_put_vline(sx,     sy,     h);
	t_put_vline(sx+w-1, sy,     h);
}

//=============================================================================
//  t_cl_put_rect
//=============================================================================
void t_cl_put_rect(int sx, int sy, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect(sx, sy, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect
//=============================================================================
void t_xy_put_rect(int sx, int sy, int ex, int ey)
{
	int		w, h;
	
	w = ex - sx + 1;
	h = ey - sy + 1;

	t_put_rect(sx, sy, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect
//=============================================================================
void t_xy_cl_put_rect(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	int			w, h;
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;
	
	w = ex - sx + 1;
	h = ey - sy + 1;

	t_set_color(col);
	t_put_rect(sx, sy, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_rect_solid
//=============================================================================
void t_put_rect_solid(int sx, int sy, int w, int h)
{
	int		i;

	for (i=0; i<h; i++)
		t_put_hline(sx, sy+i, w);
}

//=============================================================================
//  t_cl_put_rect_solid
//=============================================================================
void t_cl_put_rect_solid(int sx, int sy, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect_solid(sx, sy, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect_solid
//=============================================================================
void t_xy_put_rect_solid(int sx, int sy, int ex, int ey)
{
	int		w, h;

	w = ex - sx + 1;
	h = ey - sy + 1;

	t_put_rect_solid(sx, sy, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect_solid
//=============================================================================
void t_xy_cl_put_rect_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	int		w, h;

	w = ex - sx + 1;
	h = ey - sy + 1;

	t_set_color(col);
	t_put_rect_solid(sx, sy, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_rect_single
//=============================================================================
void t_put_rect_single(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xda);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc0);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
			}
			else if (i==x)
			{
				t_putc(0xb3);		// |
			}
			else if (i==(x+w-1))
			{
				t_gotoxy(x+w-1, j);
				t_putc(0xb3);		// |
			}
		}
	}
}

//=============================================================================
//  t_cl_put_rect_single
//=============================================================================
void t_cl_put_rect_single(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect_single(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect_single
//=============================================================================
void t_xy_put_rect_single(int sx, int sy, int ex, int ey)
{
	int		w, h;

	w = ex - sx + 1;
	h = ey - sy + 1;
	t_put_rect_single(sx, sy, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect_single
//=============================================================================
void t_xy_cl_put_rect_single(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_xy_put_rect_single(sx, sy, ex, ey);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_rect_single_solid
//=============================================================================
void t_put_rect_single_solid(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xda);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc0);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
	
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xb3);	// |
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_rect_single_solid
//=============================================================================
void t_cl_put_rect_single_solid(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect_single_solid(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect_single_solid
//=============================================================================
void t_xy_put_rect_single_solid(int sx, int sy, int ex, int ey)
{
	int		w, h;

	w = ex - sx + 1;
	h = ey - sy + 1;

	t_put_rect_single_solid(sx, sy, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect_single_solid
//=============================================================================
void t_xy_cl_put_rect_single_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_xy_put_rect_single_solid(sx, sy, ex, ey);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_rect_double
//=============================================================================
void t_put_rect_double(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xc9);	// left-top
				else if (i==(x+w-1))	t_putc(0xbb);	// right-top
				else					t_putc(0xcd);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc8);	// left-bottom
				else if (i==(x+w-1))	t_putc(0xbc);	// right-bottom
				else					t_putc(0xcd);	// --
			}
			else if (i==x)
			{
				t_putc(0xba);		// |
			}
			else if (i==(x+w-1))
			{
				t_gotoxy(x+w-1, j);
				t_putc(0xba);		// |
			}
		}
	}
}

//=============================================================================
//  t_cl_put_rect_double
//=============================================================================
void t_cl_put_rect_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect_double(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect_double
//=============================================================================
void t_xy_put_rect_double(int sx, int sy, int ex, int ey)
{
	int		w, h;

	w = ex - sx + 1;
	h = ey - sy + 1;

	t_put_rect_double(sx, sy, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect_double
//=============================================================================
void t_xy_cl_put_rect_double(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_xy_put_rect_double(sx, sy, ex, ey);
	t_set_color(bkcol);
}


//=============================================================================
//  t_put_rect_double_solid
//=============================================================================
void t_put_rect_double_solid(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xc9);	// left-top
				else if (i==(x+w-1))	t_putc(0xbb);	// right-top
				else					t_putc(0xcd);	// ==
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc8);	// left-bottom
				else if (i==(x+w-1))	t_putc(0xbc);	// right-bottom
				else					t_putc(0xcd);	// ==
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xba);	// ||
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_rect_double_solid
//=============================================================================
void t_cl_put_rect_double_solid(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_rect_double_solid(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_xy_put_rect_double_solid
//=============================================================================
void t_xy_put_rect_double_solid(int sx, int sy, int ex, int ey)
{
	int		x, y, w, h;

	x = sx;
	y = sy;
	w = ex - sx + 1;
	h = ey - sy + 1;

	t_put_rect_double_solid(x, y, w, h);
}

//=============================================================================
//  t_xy_cl_put_rect_double_solid
//=============================================================================
void t_xy_cl_put_rect_double_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_xy_put_rect_double_solid(sx, sy, ex, ey);
	t_set_color(bkcol);	
}

//=============================================================================
//  t_put_win_single
//=============================================================================
void t_put_win_single(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xda);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc0);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
	
			}
			else if (j==(y+2))
			{
				if (i==x)				t_putc(0xc3);	// left-middle
				else if (i==(x+w-1)) 	t_putc(0xb4);	// right-middle
				else					t_putc(0xc4);	// --
	
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xb3);	// |
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_win_single
//=============================================================================
void t_cl_put_win_single(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_win_single(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_win_single_double
//=============================================================================
void t_put_win_single_double(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xda);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbf);	// right-top
				else					t_putc(0xc4);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc0);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xd9);	// right-bottom
				else					t_putc(0xc4);	// --
	
			}
			else if (j==(y+2))
			{
				if (i==x)				t_putc(0xc6);	// left-middle
				else if (i==(x+w-1)) 	t_putc(0xb5);	// right-middle
				else					t_putc(0xcd);	// --
	
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xb3);	// |
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_win_single_double
//=============================================================================
void t_cl_put_win_single_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_win_single_double(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_win_double
//=============================================================================
void t_put_win_double(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xc9);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbb);	// right-top
				else					t_putc(0xcd);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc8);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xbc);	// right-bottom
				else					t_putc(0xcd);	// --
	
			}
			else if (j==(y+2))
			{
				if (i==x)				t_putc(0xcc);	// left-middle
				else if (i==(x+w-1)) 	t_putc(0xb9);	// right-middle
				else					t_putc(0xcd);	// --
	
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xba);	// |
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_win_double
//=============================================================================
void t_cl_put_win_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_win_double(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_put_win_double_single
//=============================================================================
void t_put_win_double_single(int x, int y, int w, int h)
{
	int 	i, j;

	for (j=y; j<(y+h); j++)
	{
		t_gotoxy(x, j);

		for (i=x; i<(x+w); i++)
		{
			if (j==y)
			{
				if (i==x)				t_putc(0xc9);	// left-top
				else if (i==(x+w-1)) 	t_putc(0xbb);	// right-top
				else					t_putc(0xcd);	// --
			}
			else if (j==(y+h-1))
			{
				if (i==x)				t_putc(0xc8);	// left-bottom
				else if (i==(x+w-1)) 	t_putc(0xbc);	// right-bottom
				else					t_putc(0xcd);	// --
	
			}
			else if (j==(y+2))
			{
				if (i==x)				t_putc(0xc7);	// left-middle
				else if (i==(x+w-1)) 	t_putc(0xb6);	// right-middle
				else					t_putc(0xc4);	// --
	
			}
			else
			{
				if (i==x || i== (x+w-1))	t_putc(0xba);	// |
				else						t_putc(0x20);
			}
		}
	}
}

//=============================================================================
//  t_cl_put_win_double_single
//=============================================================================
void t_cl_put_win_double_single(int x, int y, uint8_t fg, uint8_t bg, int w, int h)
{
	uint8_t		bkcol = t_get_color();
	uint8_t		col	= (bg << 4) + fg;

	t_set_color(col);
	t_put_win_double_single(x, y, w, h);
	t_set_color(bkcol);
}

//=============================================================================
//  t_copy_region
//=============================================================================
void t_copy_region(int sx, int sy, int ex, int ey)
{
	int			idx;
	int			bsz;
	int			w, h;
	uint8_t		*buf;
	uint8_t		*pb;
	
	idx = tv.clip_idx;
	
	if (idx > 14)
		return;

	w = ex - sx + 1;
	h = ey - sy + 1;
	
	tv.clip[idx].x = sx;
	tv.clip[idx].y = sy;
	tv.clip[idx].w = w;
	tv.clip[idx].h = h;
	
	bsz = 2 * w * h;
	
	buf = (char*)malloc( bsz );

	if (!buf)
	{
		tv.clip[idx].buf = NULL;
	}
	else
	{
		pb = buf;
		t_gotoxy(sx, sy);

		//fprintf(stderr, "(%d, %d) w=%d, h=%d, idx=%d, bsz=%d", sx, sy, w, h, idx, bsz);
		w = 0;
		h = sy;
		while (bsz > 0)
		{
			*pb++ = *tv.buf++;
			*pb++ = *tv.buf++;
			w++;
			if (w >= tv.clip[idx].w)
			{
				h++;
				w = 0;
				t_gotoxy(sx, h);
			}
			bsz -= 2;
		}
		tv.clip[idx].buf = buf;
		tv.clip_idx++;
		//fprintf(stderr, "clip_idx=%d", tv.clip_idx);
	}
}

//=============================================================================
//  t_paste_region
//=============================================================================
void t_paste_region(int x, int y)
{
	int		idx;
	uint8_t	*buf;
	int		px, py;
	int		bsz;
	int		w, h;

	//fprintf(stderr, "Clip_idx=%d", tv.clip_idx);

	idx = tv.clip_idx;
	if (idx == 0)
		return;
	--idx;

	buf = tv.clip[idx].buf;
	w = tv.clip[idx].w;
	h = tv.clip[idx].h;
	
	if ( buf )
	{
		if (x == -1 || y == -1)
		{
			// original position
			px = tv.clip[idx].x;
			py = tv.clip[idx].y;
		}
		else
		{
			// new position
			px = x;
			py = y;
		}

		t_gotoxy(px, py);
		bsz = 2 * w * h;
		//fprintf(stderr, "[%d, %d] w=%d, h=%d, idx=%d, bsz=%d\n", px, py, w, h, idx, bsz);
		
		w = 0;
		h = py;

		while (bsz > 0)
		{
			*tv.buf++ = *buf++;
			*tv.buf++ = *buf++;
			if((h < 24) || ((h == 24) && (w < 79)))
			{
				t_paste_region_cl_putc(*(buf - 1) & 0x0F, (*(buf - 1) >> 4) & 0x0F, *(buf - 2));
			}
			w++;
			if (w >= tv.clip[idx].w)
			{
				h++;
				w = 0;
				t_gotoxy(px, h);
			}
			bsz -= 2;
		}

		free(tv.clip[idx].buf);
		tv.clip[idx].buf = NULL;
		tv.clip_idx--;
	}
}

//=============================================================================
//  t_capture_screen
//=============================================================================
void t_capture_screen(void)
{
	FILE		*fp;
	char		fname[16];
	struct		tm *utc;
	time_t		now;
	
	#ifdef __BORLANDC__
	uint8_t far	*tvbuf = (uint8_t far *)T_BASE;
	#endif
	#ifdef __WATCOMC__
	uint8_t *tvbuf = gTVBASE;//(uint8_t *)T_BASE;
	#endif
	int			i;
	uint8_t		vbuf[T_BUF_SZ];

	for (i=0; i<T_BUF_SZ; i++)
		vbuf[i] = *(tvbuf+i) & 0xFF;
	
	// fname
	now = time(NULL);
	utc = localtime(&now);
	sprintf(fname, "%02d%02d%02d%02d.BIN", utc->tm_mday, utc->tm_hour, utc->tm_min, utc->tm_sec);

	// fopen
	fp = fopen(fname, "wb");
	if (fp)
	{
		fwrite(vbuf, sizeof(uint8_t), T_BUF_SZ, fp);
		fclose(fp);
	}
}

//=============================================================================
//  t_init
//=============================================================================
EFI_STATUS t_init(void)
{
	EFI_STATUS status;
	UINTN 	mode_num;
	UINTN 	col;
	UINTN 	row;
	
	memset(&tv, 0, sizeof(t_video_info_t));

	gTVBASE = (uint8_t *)malloc(sizeof(uint8_t) * 5000);
	tv.buf = gTVBASE;//T_VIDEO_BASE();

	// default color
	tv.color	= (T_BLACK << 4) + T_LIGHTGRAY;

	tv.update_func	= NULL;

	//video_set_mode(0x03);	// VGA mode 3 : 80x25 text mode

	//video_hide_cur();

	//-------------------------EFI-------------------------//
	// open extension console input
	status = gBS->OpenProtocol(
			gST->ConsoleInHandle,
			&gEfiSimpleTextInputExProtocolGuid,
			(VOID **) &ConInEx,
			gImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if(EFI_ERROR(status))
		return status;
	
	// backup simple console setting
	mode_bak = gST->ConOut->Mode->Mode;
	attr_bak = gST->ConOut->Mode->Attribute;
	status = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &col, &row);
	
	if(EFI_ERROR(status))
		return status;
	
	// search 80X25 mode.
	for(mode_num = 0; mode_num < gST->ConOut->Mode->MaxMode; mode_num++)
	{
		status = gST->ConOut->QueryMode(gST->ConOut, mode_num, &col, &row);
		
		if(EFI_ERROR(status))
			continue;
		
		if(col == ConsoleWidth && row == ConsoleHeight)
			break;
	}
	// change global X/Y edge if 80x25 doesn't find.
	if(mode_num >= gST->ConOut->Mode->MaxMode)
	{
		ConsoleWidth	= col;
		ConsoleHeight	= row;
	}
	else
	{
		status = gST->ConOut->SetMode(gST->ConOut, mode_num);
		if(EFI_ERROR(status))
			return status;
	}
	// clear screen
	gST->ConOut->ClearScreen(gST->ConOut);
	// hide cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);

	return EFI_SUCCESS;
}

//=============================================================================
//  t_exit
//=============================================================================
void t_exit(void)
{
	int		i;

	for (i=15; i>=0; i--)
	{
		if (tv.clip[i].buf)
		{
			free(tv.clip[i].buf);

			if (i)
				tv.clip_idx = (uint8_t)i - 1;
		}
	}
	
	t_color( T_LIGHTGRAY, T_BLACK );

	t_clear_screen();

	//video_set_mode(0x03);	// VGA mode 3 : 80x25 text mode
	//video_show_cur();

	free(gTVBASE);
	//-------------------------EFI-------------------------//
	// backup simple console setting
	gST->ConOut->SetMode(gST->ConOut, mode_bak);
	gST->ConOut->SetAttribute(gST->ConOut, attr_bak);
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}

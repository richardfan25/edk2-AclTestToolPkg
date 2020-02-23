//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  FBUF - File Buffer                                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "typedef.h"
#include "fbuf.h"

//=============================================================================
//  fbuf_flush
//=============================================================================
void fbuf_flush(fbuf_t *fb)
{
	// flush buffer
	fwrite(fb->head, sizeof(uint8_t), fb->sz_used, fb->fp);

	fb->pos = fb->head;
	fb->sz_used = 0;
}

//=============================================================================
//  fbuf_free_space
//=============================================================================
int fbuf_free_space(fbuf_t *fb)
{
	return (fb->sz_full - fb->sz_used);
}

//=============================================================================
//  fbuf_putc
//=============================================================================
void fbuf_putc(fbuf_t *fb, uint8_t c)
{
	if (fbuf_free_space(fb) < 1)
		fbuf_flush(fb);

	*fb->pos++ = c;
	fb->sz_used++;
}

//===========================================================================
//  fbuf_puts
//===========================================================================
void fbuf_puts(fbuf_t *fb, uint8_t *data)
{
	int	len;
	
	len = (int)strlen(data);

	if (fbuf_free_space(fb) < len)
		fbuf_flush(fb);

	memcpy(fb->pos, data, len);

	fb->pos += len;
	fb->sz_used += len;
}

//===========================================================================
//  fbuf_putd
//===========================================================================
void fbuf_putd(fbuf_t *fb, uint32_t d, uint8_t pad0, uint8_t maxlen, uint8_t align)
{
	int			digit[10];
	int			i, j, k;
	uint8_t		hide;
	uint32_t	data;

	hide = 1;

	for (i=0, data = d; i<10; i++)
	{
		digit[i] = data % 10;
		data /= 10;
	}
		
	for (i=9; i>=0; i--)
	{
		if (i==0)
			break;

		if (digit[i])
			break;
	}

	// i : num of digits - 1 (12345 => i=4)
	// k : max len or i
	k = i;
	if (maxlen > (i+1))
		k = maxlen - 1;

	if (k > 9)
		k = 9;
			
	if (align == 0)
	{
		for (j=k; j>=0; j--)
		{
			if (j > i)
			{
				if (pad0)
					fbuf_putc(fb, '0');
				else
					fbuf_putc(fb, ' ');
				continue;
			}				
			fbuf_putc(fb, (uint8_t)digit[j] + '0');
		}
	}
	else
	{
		for (j=k; j>=0; j--)
		{
			// left-align
			if (hide)
			{
				if (digit[j]==0)
					continue;
				else
					hide = 0;
			}
			fbuf_putc(fb, (uint8_t)digit[j] + '0');
		}
	}

	if (align == 1)
	{
		if (k != i)
		{
			for (j=0; j<=(k-i); j++)
			{
				fbuf_putc(fb, ' ');
			}
		}
	}				
}

//=============================================================================
//  fbuf_putx
//=============================================================================
void fbuf_putx(fbuf_t *fb, uint32_t x, uint8_t capital, uint8_t pad0, uint8_t maxlen)
{
	char		hex[8];
	char		c;
	int			i, j, k;
	uint32_t	data;

	for (i=0, data = x; i<8; i++)
	{
		hex[i] = data & 0xF;
		data >>= 4;
	}

	for (i=7; i>=0; i--)
	{
		if (i==0)
			break;

		if (hex[i])
			break;
	}

	k = i;

	if (maxlen > (i+1))
		k = maxlen - 1;

	for (j=k; j>=0; j--)
	{
		if (j > i)
		{
			if (pad0)
				fbuf_putc(fb, '0');
			else
				fbuf_putc(fb, ' ');
			continue;
		}
			
		if (hex[j] >= 10)
		{
			if (capital)
				c = hex[j] - 10 + 'A';
			else
				c = hex[j] - 10 + 'a';
		}
		else
		{
			c = hex[j] + '0';
		}

		fbuf_putc(fb, c);
	}
}
//=============================================================================
//  fbuf_putb
//=============================================================================
void fbuf_putb(fbuf_t *fb, uint32_t b, uint8_t pad0, uint8_t maxlen)
{
	char	bin[32];
	char	c;
	int		i, j, k;

	for (i=0; i<32; i++)
	{
		if (b & (1<<i))
			bin[i] = 1;
		else
			bin[i] = 0;
	}

	for (i=31; i>=0; i--)
	{
		if (i==0)
			break;

		if (bin[i])
			break;
	}

	k = i;

	if (maxlen > (i+1))
		k = maxlen - 1;
	
	for (j=k; j>=0; j--)
	{
		if (j > i)
		{
			if (pad0)
				fbuf_putc(fb, '0');
			else
				fbuf_putc(fb, ' ');
			continue;
		}

		if (bin[j])
			c = '1';
		else
			c = '0';

		fbuf_putc(fb, c);
	}
}

//===========================================================================
//  fbuf_printf
//===========================================================================
void fbuf_printf(fbuf_t *fb, const char *fmt, ...)
{
	va_list		args;
	char		*ptr;
	uint8_t		pad0;
	uint8_t		maxlen;
	uint8_t		align;
	char		c, d;

	va_start(args, fmt);
	ptr = (char *)fmt;

	while (*ptr)
	{
		switch (*ptr)
		{
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


					// %10d
					d = *++ptr;
					if ((d>='0') && (d<='9'))
					{
						maxlen *= 10;
						maxlen += (d & 0x0F);
					}
					else
					{
						ptr--;
					}
				}
				else
				{
					ptr--;
					maxlen = 0;
				}

				//-------------------------
				//  2nd level switch
				//-------------------------
				switch(*++ptr)
				{
					case NULL:
						continue;

					case '%':
						fbuf_putc(fb, '%');
						break;

					case 'c':
						fbuf_putc(fb, va_arg(args, char));
						break;

					case 's':
						fbuf_puts(fb, va_arg(args, char*));
						break;

					case 'd':
						fbuf_putd(fb, va_arg(args, unsigned int), pad0, maxlen, align);
						break;

					case 'x':
						fbuf_putx(fb, va_arg(args, unsigned int), 0, pad0, maxlen);
						break;

					case 'X':
						fbuf_putx(fb, va_arg(args, unsigned int), 1, pad0, maxlen);
						break;

					case 'b':
						fbuf_putb(fb, va_arg(args, unsigned int), pad0, maxlen);
						break;

				} //end-switch-2
				
				ptr++;
				break;

			default:
				fbuf_putc(fb, *ptr++);
				break;

		}// end-switch-1

	}// while

	va_end(args);
}

//=============================================================================
//  fbuf_init
//=============================================================================
fbuf_t	*fbuf_init(char *filename, uint8_t *res)
{
	fbuf_t	*fb;

	fb = (fbuf_t *)malloc(sizeof(fbuf_t));
	if (!fb)
	{
		*res = FBUF_ERR_MALLOC;
		return NULL;	//FBUF_ERR_MALLOC;
	}

	fb->fp = fopen(filename, "wb");
	if (!fb->fp)
	{
		free(fb);
		*res = FBUF_ERR_FOPEN;
		return NULL;	//FBUF_ERR_FOPEN;
	}

	// buffer : 64KB
	fb->sz_full = FBUF_BUF_SZ;

	fb->head = (uint8_t *)malloc(fb->sz_full * sizeof(uint8_t));
	if (!fb->head)
	{
		fclose(fb->fp);
		free(fb);
		
		*res = FBUF_ERR_BUF_MALLOC;
		return NULL;	//FBUF_ERR_BUF_MALLOC;
	}

	fb->pos = fb->head;
	fb->sz_used = 0;

	*res = FBUF_OK;

	return fb;
}

//=============================================================================
//  fbuf_exit
//=============================================================================
void fbuf_exit(fbuf_t *fb)
{
	if (fb)
	{
		//if (fb->sz_used)
		//	fbuf_flush(fb);
		
		if (fb->fp)
		{
			if (fb->sz_used)
				fbuf_flush(fb);

			fclose(fb->fp);
		}
	
		if (fb->head)
			free(fb->head);

		free(fb);
	}
}

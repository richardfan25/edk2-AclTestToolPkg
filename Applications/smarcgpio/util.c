#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util.h"

//=========================================================================================
//  crc16
//=========================================================================================
uint16_t crc16(uint8_t *ptr, int count)
{
	uint16_t		crc, i;

	crc = 0;

	while (--count >= 0)
	{
		crc = crc ^ (uint16_t)*ptr++ << 8;
		for (i=0; i<8; ++i)
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return crc & 0xFFFF;
}
/*==============================================================*/
uint8_t dec_to_str(uint16_t dec, uint8_t *str, uint8_t pad_len)
{
	uint16_t	div;		// divisor
	uint8_t		digit, out;	// each digit, output flag
	uint8_t		*buf = str;
	uint8_t		len, dcnt;

	// max unsigned integer = 65535, so divior=10000 at starting
	out = 0;
	len = 0;
	dcnt = 5;
	for (div=10000; div>0; div/=10, dcnt--)
	{
		// get each digit 6,5,5,3,5
		digit = (uint8_t)(dec / div);

		// pad_len = 2, dec = 8,  output = "08"
		// pad_len = 1, dec = 8,  output = "8"
		// pad_len = 3, dec = 14, output = "014"
		if (dcnt <= pad_len)
			out = 1;

		// if digit <> 0, set output flag
		// if dec=0, we want to output '0' instead of "00000"
		// if div==1, forced output, if only one digit (dec=0~9)
		if (digit || div == 1)
			out = 1;

		if (out)
		{
			if (out > 1)
				digit = (uint8_t)(dec / div);	// get digit

			*buf++ = (digit + '0');
			len++;

			dec -= (digit*div);
			out++;
		}
	}

	return len;
}

/*==============================================================*/
uint16_t str_to_dec(uint8_t type, char *str)
{
	uint8_t len = 0;
	uint8_t idx;
	uint16_t digi;
	uint16_t digi_2 = 1;
	uint16_t result;
	uint8_t	num_buf[5];
	char num;
	
	while(*(str + len) != '\0') len++;
	
	if(len == 0)
		return 0xFFFF;
	
	if(type == 0)
	{
		if(len > 5)
			return 0xFFFF;
		for(idx = 0; idx < len; idx++)
		{
			num = *(str + idx);
			if(num < '0' || num > '9')
				return 0xFFFF;
			*(num_buf + idx) = (uint8_t)(num - '0');
		}
		digi = 10;
	}
	else 
	{
		if(len > 4)
			return 0xFFFF;
		for(idx = 0; idx < len; idx++)
		{
			num = *(str + idx);
			if(num >= '0' && num <= '9')
				*(num_buf + idx) = (uint8_t)(num - '0');
			else if(num >= 'A' && num <= 'F')
				*(num_buf + idx) = (uint8_t)(num - 'A') + 10;
			else if(num >= 'a' && num <= 'f')
				*(num_buf + idx) = (uint8_t)(num - 'a') + 10;
			else
				return 0xFFFF;
		}
		digi = 16;
	}

	result = 0;
	while(len > 0)
	{
		len--;
		result = result + *(num_buf + len) * digi_2;
		digi_2 = digi_2 * digi;
	}
	return result;
}

//=============================================================================
void print_title(char *title)
{
	uint8_t	len = 0;
	char	*str;
	uint8_t	i;
	
	while(1)
	{
		if(*(title + len) == '\0')
			break;
		len++;
	}
	if(len < 1)
		return;
	// #==============#
	// #  Your Title  #
	// #==============#
	str = (char *)malloc(len+6+1);
	*str = '#';
	for(i = 1; i < len+5; i++)
		*(str + i) = '=';
	*(str + i++) = '#';
	*(str + i) = '\0';
	
	fprintf(stderr, "%s\n", str);
	fprintf(stderr, "#  %s  #\n", title);
	fprintf(stderr, "%s\n", str);
}

//=============================================================================
uint16_t get_number_stdin(CHAR8 *msg, uint8_t type, uint8_t digi)
{
	int	  key;
	CHAR8 ch;
	CHAR8 *num_buf;
	uint8_t idx = 0;
	INT32 col;
	INT32 row;
	
	if(digi == 0)
		digi = 1;
	else if(type == 0 && digi > 5)
		digi = 5;
	else if(type != 0 && digi > 4)
		digi = 4;
	
	num_buf = (CHAR8 *)malloc(digi + 1);
	*num_buf = ' ';
	*(num_buf + 1) = '\0';
	if(type == 0)
		fprintf(stderr,"\r%s ", msg);
	else
		fprintf(stderr,"\r%s 0x", msg);
	col = gST->ConOut->Mode->CursorColumn;
	row = gST->ConOut->Mode->CursorRow;
	
	while(1)
	{
		
		while((key = bioskey(1)) == 0);

		ch = (CHAR8) (key & 0xFF);
		
		// 1. check valid key
		if(ch == 0x08)					// 0x08: BACKSPACE key
		{
			if(idx != 0)
			{
				*(num_buf + idx) = '\0';
				idx--;
				*(num_buf + idx) = ' ';
				col--;
				gotoxy(col, row);
				fprintf(stderr," ");
				gotoxy(col, row);
			}
			continue;
		}
		else if(ch == 0x0D)				// 0x0D: Enter key
		{
			break;
		}
		if(ch < '0' || idx == digi)
			continue;

		if(ch > '9')
		{
			if(type == 0 || ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'f')
				continue;
			
			if(ch >= 'a')
				ch = ch - 0x20;			// convert to upper case
		}

		// 2. push num to buf
		*(num_buf + idx) = ch;
		idx++;
		*(num_buf + idx) = '\0';
		fprintf(stderr,"%c", ch);
		col++;
	}
	fprintf(stderr,"\n");
	return str_to_dec(type, num_buf);
}



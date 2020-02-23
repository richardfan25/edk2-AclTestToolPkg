#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util.h"

uint32_t str_to_dec(uint8_t type, char *str)
{
	uint8_t len = 0;
	uint8_t idx;
	uint32_t digi;
	uint32_t digi_2 = 1;
	uint32_t result;
	uint8_t	num_buf[10];
	char num;
	
	while(*(str + len) != '\0') len++;
	
	if(len == 0)
		return 0xFFFFFFFF;
	
	if(type == 0)
	{
		if(len > 10)
			return 0xFFFFFFFF;
		for(idx = 0; idx < len; idx++)
		{
			num = *(str + idx);
			if(num < '0' || num > '9')
				return 0xFFFFFFFF;
			*(num_buf + idx) = (uint8_t)(num - '0');
		}
		digi = 10;
	}
	else 
	{
		if(len > 4)
			return 0xFFFFFFFF;
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
				return 0xFFFFFFFF;
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

uint32_t get_number_stdin(uint16_t *msg, uint8_t type, uint8_t digi)
{
	int	  key;
	CHAR8 ch;
	CHAR8 *num_buf;
	uint8_t idx = 0;
	INT32 col;
	INT32 row;
	
	if(digi == 0)
		digi = 1;
	else if(type == 0 && digi > 10)
		digi = 10;
	else if(type != 0 && digi > 4)
		digi = 4;
	
	num_buf = (CHAR8 *)malloc(digi + 1);
	*num_buf = ' ';
	*(num_buf + 1) = '\0';
	if(type == 0)
		Print(L"%s ", msg);
	else
		Print(L"%s 0x", msg);
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
				Print(L" ");
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
		Print(L"%c", ch);
		col++;
	}
	Print(L"\n");
	return str_to_dec(type, num_buf);
}



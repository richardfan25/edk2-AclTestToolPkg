#include <stdio.h>
#include <stdlib.h>

#include "x86hw.h"
#include "util.h"
#include "spd.h"

#include "dio.h"
#include "hwm.h"
#include "lm95245.h"
#include "pca9555.h"

#include "pchsmbtest.h"

//=============================================================================
//  variables
//=============================================================================
SmbusVariableConfig *gSmbusConfig = NULL;
SMB_BUS *gSmbus = NULL;



//=============================================================================
void SmbusTestConfigDefault(void)
{
	gSmbusConfig->eeprom->ch = 0;
	gSmbusConfig->eeprom->freq = 0;
	gSmbusConfig->eeprom->type= 0;
	gSmbusConfig->eeprom->addr = 0x00;
	gSmbusConfig->eeprom->spos = 0x00;
	gSmbusConfig->eeprom->rwlen = 256;
	gSmbusConfig->eeprom->wsval = 0x00;
	
	gSmbusConfig->thermal->ch = 0;
	gSmbusConfig->thermal->freq= 0;
	gSmbusConfig->thermal->addr = 0x00;
	gSmbusConfig->thermal->wsval= 85;
	
	gSmbusConfig->dio->ch = 0;
	gSmbusConfig->dio->freq= 0;
	gSmbusConfig->dio->addr = 0x00;

	gSmbusConfig->dev->ch = 0;
	gSmbusConfig->dev->freq= 0;
	gSmbusConfig->dev->addr = 0x00;
	gSmbusConfig->dev->cmd = 0x00;
	gSmbusConfig->dev->rwlen = 1;
	gSmbusConfig->dev->wsval = 0x00;
	
	gSmbusConfig->battery->ch = 0;
	gSmbusConfig->battery->freq= 0;
	gSmbusConfig->battery->addr = 0x00;

	gSmbusConfig->spd->ch = 0;
	gSmbusConfig->spd->freq = 0;
	gSmbusConfig->spd->source_addr = 0x00;
	gSmbusConfig->spd->destination_addr = 0x00;
}

void SmbusEepromConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"EEPROM Configuration Page \n\n");
	
	Print(L"Channel(0:Smb0 1:Smb1)   : %d\n", gSmbusConfig->eeprom->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->eeprom->freq);
	Print(L"Type(0:Byte 1:Word)      : %d\n", gSmbusConfig->eeprom->type);
	Print(L"Address(0x00~0xFF)       : %X\n", gSmbusConfig->eeprom->addr);

	if(gSmbusConfig->eeprom->type)
	{
		Print(L"Position(0x0000~0xFFFF)  : %X\n", gSmbusConfig->eeprom->spos);
	}
	else
	{
		Print(L"Position(0x00~0xFF)      : %X\n", gSmbusConfig->eeprom->spos);
	}
	Print(L"Length(0~256)            : %d\n", gSmbusConfig->eeprom->rwlen);
	Print(L"Value(0x00~0xFF)         : %X\n", gSmbusConfig->eeprom->wsval);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 5)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(27 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					//case 1:
					case 1:
					case 4:
					{
						value = value / 10;
						break;
					}
					case 2:
					case 3:
					case 5:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
				//case 1:
				case 1:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 4:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(value > 256)
							{
								value = 256;
							}
						}
					}
					break;
				}
				case 2:
				case 3:
				case 5:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->eeprom->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->eeprom->ch);
					break;
				}
				/*case 1:
				{
					gSmbusConfig->eeprom->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->eeprom->freq);
					break;
				}*/
				case 1:
				{
					gSmbusConfig->eeprom->type = (uint8_t)value;
					Print(L"%d", gSmbusConfig->eeprom->type);
					
					gotoxy(0, 5);
					if(gSmbusConfig->eeprom->type)
					{
						Print(L"Position(0x0000~0xFFFF)  : %X\n", gSmbusConfig->eeprom->spos);
					}
					else
					{
						Print(L"Position(0x00~0xFF)      : %X\n", gSmbusConfig->eeprom->spos);
					}
					break;
				}
				case 2:
				{
					gSmbusConfig->eeprom->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->eeprom->addr);
					break;
				}
				case 3:
				{
					gSmbusConfig->eeprom->spos = value;
					Print(L"%-4X", gSmbusConfig->eeprom->spos);
					break;
				}
				case 4:
				{
					gSmbusConfig->eeprom->rwlen = value;
					Print(L"%-3d", gSmbusConfig->eeprom->rwlen);
					break;
				}
				case 5:
				{
					gSmbusConfig->eeprom->wsval = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->eeprom->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->eeprom->ch;
					Print(L"%d", value);
					break;
				}
				/*case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->eeprom->freq;
					Print(L"%d", value);
					break;
				}*/
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->eeprom->type;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%X", gSmbusConfig->eeprom->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->eeprom->addr;
					Print(L"%-2X", value);
					break;
				}
				case 3:
				{
					sprintf(buf, "%X", gSmbusConfig->eeprom->spos);
					len = (uint8_t)strlen(buf);
					
					if(gSmbusConfig->eeprom->type)
					{
						t_len = 4;
					}
					else
					{
						t_len = 2;
					}
					
					value = gSmbusConfig->eeprom->spos;
					Print(L"%-4X", value);
					break;
				}
				case 4:
				{
					if(gSmbusConfig->eeprom->rwlen > 100)
					{
						len = 3;
					}
					else if(gSmbusConfig->eeprom->rwlen > 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gSmbusConfig->eeprom->rwlen;
					Print(L"%-3d", value);
					break;
				}
				case 5:
				{
					sprintf(buf, "%X", gSmbusConfig->eeprom->wsval);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->eeprom->wsval;
					Print(L"%-2X", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->eeprom->ch = (uint8_t)value;
			break;
		}
		/*case 1:
		{
			gSmbusConfig->eeprom->freq = (uint8_t)value;
			break;
		}*/
		case 1:
		{
			gSmbusConfig->eeprom->type = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->eeprom->addr = (uint8_t)value;
			break;
		}
		case 3:
		{
			gSmbusConfig->eeprom->spos = value;
			break;
		}
		case 4:
		{
			gSmbusConfig->eeprom->rwlen = value;
			break;
		}
		case 5:
		{
			gSmbusConfig->eeprom->wsval = (uint8_t)value;
			break;
		}
	}
}

void SmbusThermalConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Thermal IC Configuration Page \n\n");
	
	Print(L"Channel(0:Smb0 1:Smb1)   : %d\n", gSmbusConfig->thermal->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->thermal->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gSmbusConfig->thermal->addr);
	Print(L"Value(0~127)             : %d\n", gSmbusConfig->thermal->wsval);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 2)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(27 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					//case 1:
					case 2:
					{
						value = value / 10;
						break;
					}
					case 1:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
				//case 1:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 1:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
				case 2:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(value > 127)
							{
								value = 127;
							}
						}
					}
					break;
				}
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->thermal->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->thermal->ch);
					break;
				}
				/*case 1:
				{
					gSmbusConfig->thermal->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->thermal->freq);
					break;
				}*/
				case 1:
				{
					gSmbusConfig->thermal->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->thermal->addr);
					break;
				}
				case 2:
				{
					gSmbusConfig->thermal->wsval = (uint8_t)value;
					Print(L"%-3d", gSmbusConfig->thermal->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->thermal->ch;
					Print(L"%d", value);
					break;
				}
				/*case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->thermal->freq;
					Print(L"%d", value);
					break;
				}*/
				case 1:
				{
					sprintf(buf, "%x", gSmbusConfig->thermal->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->thermal->addr;
					Print(L"%-2X", value);
					break;
				}
				case 2:
				{
					if(gSmbusConfig->thermal->wsval > 100)
					{
						len = 3;
					}
					else if(gSmbusConfig->thermal->wsval > 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gSmbusConfig->thermal->wsval;
					Print(L"%-3d", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->thermal->ch = (uint8_t)value;
			break;
		}
		/*case 1:
		{
			gSmbusConfig->thermal->freq = (uint8_t)value;
			break;
		}*/
		case 1:
		{
			gSmbusConfig->thermal->addr = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->thermal->wsval = (uint8_t)value;
			break;
		}
	}
}

void SmbusDIOBoardConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"DIO Board Configuration Page \n\n");
	
	Print(L"Channel(0:Smb0 1:Smb1)   : %d\n", gSmbusConfig->dio->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->dio->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gSmbusConfig->dio->addr);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 1)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(27 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					//case 1:
					{
						value = value / 10;
						break;
					}
					case 1:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
				//case 1:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 1:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->dio->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->dio->ch);
					break;
				}
				/*case 1:
				{
					gSmbusConfig->dio->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->dio->freq);
					break;
				}*/
				case 1:
				{
					gSmbusConfig->dio->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->dio->addr);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->dio->ch;
					Print(L"%d", value);
					break;
				}
				/*case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->dio->freq;
					Print(L"%d", value);
					break;
				}*/
				case 1:
				{
					sprintf(buf, "%X", gSmbusConfig->dio->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->dio->addr;
					Print(L"%-2X", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->dio->ch = (uint8_t)value;
			break;
		}
		/*case 1:
		{
			gSmbusConfig->dio->freq = (uint8_t)value;
			break;
		}*/
		case 1:
		{
			gSmbusConfig->dio->addr = (uint8_t)value;
			break;
		}
	}
}

void SmbusSmartBatteryConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Smart Battery Configuration Page \n\n");
	
	Print(L"Channel(0:Smb0 1:Smb1)   : %d\n", gSmbusConfig->battery->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->battery->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gSmbusConfig->battery->addr);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 1)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(27 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					//case 1:
					{
						value = value / 10;
						break;
					}
					case 1:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
				//case 1:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 1:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->battery->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->battery->ch);
					break;
				}
				/*case 1:
				{
					gSmbusConfig->battery->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->battery->freq);
					break;
				}*/
				case 1:
				{
					gSmbusConfig->battery->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->battery->addr);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->battery->ch;
					Print(L"%d", value);
					break;
				}
				/*case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->battery->freq;
					Print(L"%d", value);
					break;
				}*/
				case 1:
				{
					sprintf(buf, "%X", gSmbusConfig->battery->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->battery->addr;
					Print(L"%-2X", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->battery->ch = (uint8_t)value;
			break;
		}
		/*case 1:
		{
			gSmbusConfig->battery->freq = (uint8_t)value;
			break;
		}*/
		case 1:
		{
			gSmbusConfig->battery->addr = (uint8_t)value;
			break;
		}
	}
}

void SmbusDeviceConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Device Configuration Page \n\n");
	
	Print(L"Channel                  : %d\n", gSmbusConfig->dev->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->dev->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gSmbusConfig->dev->addr);
	Print(L"Command(0x00~0xFF)       : %X\n", gSmbusConfig->dev->cmd);
	//Print(L"Length(0~256)            : %d\n", gSmbusConfig->dev->rwlen);
	Print(L"Value(0x00~0xFF)         : %X\n", gSmbusConfig->dev->wsval);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 3)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(27 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					//case 1:
					//case 4:
					{
						value = value / 10;
						break;
					}
					case 1:
					case 2:
					case 3:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
				//case 1:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 1:
				case 2:
				case 3:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
				/*case 4:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(value > 256)
							{
								value = 256;
							}
						}
					}
					break;
				}*/
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->dev->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->dev->ch);
					break;
				}
				/*case 1:
				{
					gSmbusConfig->dev->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->dev->freq);
					break;
				}*/
				case 1:
				{
					gSmbusConfig->dev->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->dev->addr);
					break;
				}
				case 2:
				{
					gSmbusConfig->dev->cmd = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->dev->cmd);
					break;
				}
				/*case 4:
				{
					gSmbusConfig->dev->rwlen = value;
					Print(L"%-3d", gSmbusConfig->dev->rwlen);
					break;
				}*/
				case 3:
				{
					gSmbusConfig->dev->wsval= (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->dev->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->dev->ch;
					Print(L"%d", value);
					break;
				}
				/*case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->dev->freq;
					Print(L"%d", value);
					break;
				}*/
				case 1:
				{
					sprintf(buf, "%X", gSmbusConfig->dev->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->dev->addr;
					Print(L"%-2X", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%X", gSmbusConfig->dev->cmd);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->dev->cmd;
					Print(L"%-2X", value);
					break;
				}
				/*case 4:
				{
					if(gSmbusConfig->dev->rwlen > 100)
					{
						len = 3;
					}
					else if(gSmbusConfig->dev->rwlen > 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gSmbusConfig->dev->rwlen;
					Print(L"%-3d", value);
					break;
				}*/
				case 3:
				{
					sprintf(buf, "%X", gSmbusConfig->dev->wsval);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->dev->wsval;
					Print(L"%-2X", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->dev->ch = (uint8_t)value;
			break;
		}
		/*case 1:
		{
			gSmbusConfig->dev->freq = (uint8_t)value;
			break;
		}*/
		case 1:
		{
			gSmbusConfig->dev->addr = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->dev->cmd = (uint8_t)value;
			break;
		}
		/*case 4:
		{
			gSmbusConfig->dev->rwlen = (uint8_t)value;
			break;
		}*/
		case 3:
		{
			gSmbusConfig->dev->wsval = (uint8_t)value;
			break;
		}
	}
}

void SmbusSPDConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Device Configuration Page \n\n");
	
	Print(L"Channel                        : %d\n", gSmbusConfig->spd->ch);
	//Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->spd->freq);
	Print(L"Source Address(0x00~0xFF)      : %X\n", gSmbusConfig->spd->source_addr);
	Print(L"Destination Address(0x00~0xFF) : %X\n", gSmbusConfig->spd->destination_addr);
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_UP << 8))
		{
			if(item > 0)
			{
				item--;
			}
		}
		else if(key == (SCAN_DOWN << 8))
		{
			if(item < 2)
			{
				item++;
			}
		}
		else if(key == (SCAN_ESC << 8))	//return
		{
			goto cfg_end;
		}
		else if(key == CHAR_BACKSPACE)	//delete number
		{
			if(len > 0)
			{
				len--;
				gotoxy(33 + len, item + 2);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					{
						value = value / 10;
						break;
					}
					case 1:
					case 2:
					{
						value = value >> 4;
						break;
					}
				}
			}
		}
		else if(((key >= '0') && (key <= '9')) || 
				((key >= 'A') && (key <= 'F')) || 
				((key >= 'a') && (key <= 'f')))
		{
			gotoxy(33 + len, item + 2);
			
			switch(item)
			{
				case 0:
				{
					if((key >= '0') && (key <= '1'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = key - '0';
						}
					}
					break;
				}
				case 1:
				case 2:
				{
					if(len != t_len)
					{
						len++;
						Print(L"%c", key);
						
						if((key >= '0') && (key <= '9'))
						{
							value = (value << 4) | (key - '0');
						}
						else if((key >= 'A') && (key <= 'F'))
						{
							value = (value << 4) | (key - 'A' + 10);
						}
						else if((key >= 'a') && (key <= 'f'))
						{
							value = (value << 4) | (key - 'a' + 10);
						}
					}
					break;
				}
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(33, item_bak + 2);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gSmbusConfig->spd->ch = (uint8_t)value;
					Print(L"%d", gSmbusConfig->spd->ch);
					break;
				}
				case 1:
				{
					gSmbusConfig->spd->source_addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->spd->source_addr);
					break;
				}
				case 2:
				{
					gSmbusConfig->spd->destination_addr= (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->spd->destination_addr);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(33, item + 2);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->spd->ch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					sprintf(buf, "%X", gSmbusConfig->spd->source_addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->spd->source_addr;
					Print(L"%-2X", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%X", gSmbusConfig->spd->destination_addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->spd->destination_addr;
					Print(L"%-2X", value);
					break;
				}
			}
			
			item_bak = item;
		}
	}
	
cfg_end:
	console_color(LIGHTGRAY, BLACK);
	switch(item)
	{
		case 0:
		{
			gSmbusConfig->spd->ch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gSmbusConfig->spd->source_addr = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->spd->destination_addr= (uint8_t)value;
			break;
		}
	}
}

uint8_t SmbusTransaction(SMBUS_RW *tmp)
{
	uint16_t retry;
	uint16_t baddr;
	uint8_t i, value, status, err;
	uint8_t ch = tmp->ch;

	err = SMB_OK;
	baddr = gSmbus->base[ch];	//Intel pci->bar4 , AMD ch0:0xB00 ch1:0xB20
	
	//wait busy
	retry = 35000;//35ms	//SMB_WAIT_RETRIES;
	while ( inp(baddr+SMB_HST_STS) & 0x01 )
	{
		usleep(1);
		retry--;
		if(retry == 0)
		{
			//Print(L"Smbus Busy \n");
			err = SMB_ERR_WAIT_FREE_TMOUT;
			goto exit_trans;
		}
	}

	//clean all status reg
	outp( baddr+SMB_HST_STS, 0xFE);
	usleep(10);

	//set smb protocol
	value = inp(baddr+SMB_HST_CNT);
	value &= 0xE3;
	value |= (tmp->mode & 0x07) << 2;
	
	//set Packet Error Check Register
	value &= 0x7F;
	value |= (tmp->pec << 7);	// 0:disable 1:enable
	
	outp( baddr+SMB_HST_CNT, value);

	//set tx slave address
	outp( baddr+SMB_HST_ADD, tmp->addr);

	//set tx command
	outp( baddr+SMB_HST_CMD, tmp->cmd);

	//according SMB_CMD to set tx data
	switch(tmp->mode & SMB_CMD_MASK)
	{
		case SMB_CMD_QUICK:
		{
			break;
		}
		case SMB_CMD_BYTE:
		{
			break;
		}
		case SMB_CMD_BYTE_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp( baddr+SMB_HST_DAT0, *tmp->wbuf);
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp( baddr+SMB_HST_DAT0, *tmp->wbuf);
				outp( baddr+SMB_HST_DAT1, *(tmp->wbuf+1) );
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			outp( baddr+SMB_HST_DAT0, *tmp->wbuf);
			outp( baddr+SMB_HST_DAT1, *(tmp->wbuf+1) );
			break;
		}
		case SMB_CMD_BLOCK:
		{
			if(gSmbus->vendor == 0x8086)	//Intel
			{
				//enable 32 byte buffer
				value = inp(baddr+SMB_HST_AUXCTL);
				value |= (1 << 1);	// 0:disable 1:enable
				outp( baddr+SMB_HST_AUXCTL, value);
			}
			
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_HST_DAT0, *(tmp->blen));
				for(i=0; i<*(tmp->blen); i++)
				{
					outp(baddr + SMB_HST_BLKDAT, *(tmp->wbuf + i));
				}
			}
			break;
		}
		case SMB_CMD_I2C_READ:
		{
			err = SMB_ERR_PROTOCOL;
			break;
		}
		case SMB_CMD_BLOCK_PROC:
		{
			if(gSmbus->vendor == 0x8086)	//Intel
			{
				//enable 32 byte buffer
				value = inp(baddr+SMB_HST_AUXCTL);
				value |= (1 << 1);	// 0:disable 1:enable
				outp( baddr+SMB_HST_AUXCTL, value);
			}
			
			outp(baddr + SMB_HST_DAT0, tmp->wlen);
			for(i=0; i<tmp->wlen; i++)
			{
				outp(baddr + SMB_HST_BLKDAT, *(tmp->wbuf + i));
			}
			break;
		}
		default :
		{
			err = SMB_ERR_PROTOCOL;
			break;
		}
	}

	if(err != SMB_OK)
	{
		goto exit_trans;
	}

	//start transaction
	value = inp(baddr+SMB_HST_CNT);
	value |= (1 << 6);		//start=1
	outp( baddr+SMB_HST_CNT, value );
	usleep(1 * 1000);

	//wait busy
	retry = 35000;//35ms	//SMB_WAIT_RETRIES;
	while ( inp(baddr+SMB_HST_STS) & 0x01 )
	{
		usleep(1);
		retry--;
		if(retry == 0)
		{
			//Print(L"Smbus Busy \n");
			err = SMB_ERR_WAIT_FREE_TMOUT;
			goto exit_trans;
		}
	}

	//check status
	status = inp(baddr+SMB_HST_STS);
	if(status & 0x04)
	{
		//D_Print(L"Smbus Not Ack \n");
		err = SMB_ERR_NACK;
	}
	else if(status & 0x08)
	{
		//D_Print(L"Smbus Arbitration Lost \n");
		err = SMB_ERR_ARBI;
	}
	else if(status & 0x10)
	{
		//D_Print(L"Smbus Transaction Fail \n");
		err = SMB_ERR_FAILED;
	}

	if(err != SMB_OK)
	{
		goto exit_trans;
	}

	//read data from buffer
	switch(tmp->mode & SMB_CMD_MASK)
	{
		case SMB_CMD_QUICK:
		{
			break;
		}
		case SMB_CMD_BYTE:
		case SMB_CMD_BYTE_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->rbuf = inp( baddr+SMB_HST_DAT0 );
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->rbuf = inp( baddr+SMB_HST_DAT0 );
				*(tmp->rbuf + 1) = inp( baddr+SMB_HST_DAT1 );
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			*tmp->rbuf = inp( baddr+SMB_HST_DAT0 );
			*(tmp->rbuf + 1) = inp( baddr+SMB_HST_DAT1 );
			break;
		}
		case SMB_CMD_BLOCK:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->blen = inp(baddr + SMB_HST_DAT0);
				if(*tmp->blen > tmp->rlen)
				{
					err = SMB_ERR_BUF;
					goto exit_trans;
				}
				
				for(i=0; i<*tmp->blen; i++)
				{
					*(tmp->rbuf + i) = inp(baddr + SMB_HST_BLKDAT);
				}
			}
			break;
		}
		case SMB_CMD_I2C_READ:
		{
			break;
		}
		case SMB_CMD_BLOCK_PROC:
		{
			*tmp->blen = inp(baddr + SMB_HST_DAT0);
			if(*tmp->blen > tmp->rlen)
			{
				err = SMB_ERR_BUF;
				goto exit_trans;
			}
			
			for(i=0; i<*tmp->blen; i++)
			{
				*(tmp->rbuf + i) = inp(baddr + SMB_HST_BLKDAT);
			}
			break;
		}
		default:
		{
			err = SMB_ERR_PARAMETERS;
			goto exit_trans;
		}
	}

exit_trans:
	//disable 32 byte buffer
	value = inp(baddr+SMB_HST_AUXCTL);
	value &= ~(1 << 1);	// 0:disable 1:enable
	outp( baddr+SMB_HST_AUXCTL, value);
	
	//clear all status
	outp( baddr+SMB_HST_STS, 0xFE);

	return err;
}

uint8_t SmbusQuickWrite(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_QUICK;
	smb_data.cmd = cmd;
	smb_data.addr = addr;
	smb_data.wlen = 1;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReceiveByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BYTE;
	smb_data.cmd = cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 1;
	smb_data.rbuf = rbuf;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BYTE_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 1;
	smb_data.rbuf = rbuf;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BYTE_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 1;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadWord(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_WORD_DATA;
	smb_data.cmd= cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 2;
	smb_data.rbuf = rbuf;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteWord(uint8_t ch,uint8_t cmd, uint8_t addr, uint8_t *wbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_WORD_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 2;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadBlock(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf, uint8_t *blen)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BLOCK;
	smb_data.cmd= cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 32;
	smb_data.rbuf = rbuf;
	smb_data.blen = blen;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteBlock(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf, uint8_t *blen)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BLOCK;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 32;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = blen;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

int PCHSmbusCheckSPD(uint8_t ch, uint8_t addr, uint8_t *data)
{
	uint8_t		wbuf[2];
	uint8_t		spd[4];
	uint8_t		ee_used;
	uint8_t		ee_total;
	int			i;
	uint16_t		crc;

	// spd : first 4 bytes of spd for checking spd integrity
	for (i=0; i<4; i++)
	{
		SmbusReadByte(ch, (uint8_t)i, addr, &spd[i]);
		data[i] = spd[i];
	}
	
		
	if ((spd[2] != SPD_RAM_TYPE_DDR4) &&
		(spd[2] != SPD_RAM_TYPE_DDR3) &&
		(spd[2] != SPD_RAM_TYPE_DDR2))
	{
		// set page 0 of ddr4 before checking reading spd
		SmbusQuickWrite(ch, 0, 0x6C, wbuf);
		
		for (i=0; i<4; i++)
		{
			SmbusReadByte(ch, (uint8_t)i, addr, &spd[i]);
			data[i] = spd[i];
		}
	}

	// dram type
	if (!(spd[2] == SPD_RAM_TYPE_DDR2 ||
		spd[2] == SPD_RAM_TYPE_DDR3 ||
		spd[2] == SPD_RAM_TYPE_DDR4) )
	{
		return -1;	// non-spd
	}

	// SPD revision : DDR3, DDR4
	if (spd[2] == SPD_RAM_TYPE_DDR3 || spd[2] == SPD_RAM_TYPE_DDR4)
	{
		// 0xFF : undefined
		if (spd[1] == 0xFF)
			return -2;	// non-spd

		// SPD module type
		// 01h : SPD_MOD_TYPE_RDIMM
		// 0Dh : SPD_MOD_TYPE_32SODIMM
		if ((spd[3] < SPD_MOD_TYPE_RDIMM) ||
			(spd[3] > SPD_MOD_TYPE_32SODIMM))
			return -3;

		// ee_used
		ee_used	 = spd[0] & 0x0F;			// bit[3:0]
		ee_total = (spd[0] >> 4) & 0x7;		// bit[6:4]

		if (spd[2] == SPD_RAM_TYPE_DDR3)
		{
			// undef/128/176/256
			if (ee_used == 0 || ee_used > 3)
				return -4;

			// 256 bytes
			if (ee_total != 1)
				return -5;
		}
		else if (spd[2] == SPD_RAM_TYPE_DDR4)
		{
			// 0/128/256/384/512
			if (ee_used > 4)
				return -6;
	
			// 384/512
			if (ee_total < 2 || ee_total > 3)
				return -7;
		}
	}
	else if (spd[2] == SPD_RAM_TYPE_DDR2)
	{
		// 0x80:128 bytes, 0xFF:256 bytes
		if (!(spd[0] == 0x80 || spd[0] == 0xFF))
			return -8;
		
		// 0x07:24C01=128, 0x08:24C02=256
		if (!(spd[1] == 0x07 || spd[1] == 0x08))
			return -9;

		if (!(spd[3] == 0x0F || spd[3] == 0x0E || spd[3] == 0x0D || spd[3] == 0x0C))
			return -10;
	}
	
	for(i=4; i<128; i++)
	{
		SmbusReadByte(ch, (uint8_t)i, addr, spd);
		data[i] = spd[0];
	}
#if 0	//debug
	for(i=0;i<128;i++)
	{
		Print(L"%02x  ",data[i]);
	}
#endif
	if(data[0] & 0x80)
	{
		crc = crc16(data, 117);
		//Print(L"crc117=%x \n", crc);
		if((((uint16_t)data[127] << 8) | data[126]) != crc)
		{
			//Print(L"NO");
			return -11;
		}
	}
	else
	{
		crc = crc16(data, 126);
		//Print(L"crc126=%x \n", crc);
		if((((uint16_t)data[127] << 8) | data[126]) != crc)
		{
			//Print(L"NO");
			return -12;
		}
	}
	
	return 0;
}

void PCHSmbusScanDevice(void)
{
	uint8_t ch, status, rdata, count;
	uint16_t i;
	
	for(ch=0; ch<2; ch++)
	{
		count = 0;
		
		//if channel not found, check next
		if(gSmbus->found[ch] == 0)
		{
			continue;
		}

		//scan
		for(i=0x00; i<0xFF; i+=2)
		{
			status = SmbusReadByte(ch, 0, (uint8_t)i, &rdata);
			if(status == SMB_OK)
			{
				gSmbus->dev_addr[ch][count] = (uint8_t)i;
				count++;
			}
			usleep(1000);
		}
		
		gSmbus->dev_num[ch] = count;
	}
}

void PCHSmbusListDeviceAddress(void)
{
	uint8_t ch;
	uint16_t i;
	
	clrscr();
	Print(L"List SMBus Device Address \n\n");

	//scan smbus
	PCHSmbusScanDevice();
	
	for(ch=0; ch<2; ch++)
	{
		if(gSmbus->found[ch] == 0)
		{
			continue;
		}

		Print(L"Scan Smbus %d... \n\n", (uint16_t)ch);
		if(gSmbus->dev_num == 0)
		{
			Print(L"No Device \n\n");
			continue;
		}
		
		for(i=0; i<gSmbus->dev_num[ch]; i++)
		{
			Print(L"Device %d Address : 0x%02X \n", i, gSmbus->dev_addr[ch][i]);
		}
		Print(L"\n\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusReadEepromTest(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, rbuf[2], spd[128];
	uint16_t i, len, eeprom_saddr = 0;
	
	clrscr();
	Print(L"SMBus - Read EEPROM Test. \n\n");
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD !! \n", gSmbusConfig->eeprom->addr);
		console_color(LIGHTGRAY, BLACK);
		eeprom_type = 0;
	}
	else
	{
		eeprom_type = gSmbusConfig->eeprom->type;
	}
	
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;
	
	if(eeprom_type)
	{
		status = SmbusWriteByte(eeprom_ch, (eeprom_saddr >> 8), eeprom_addr, (uint8_t *)&eeprom_saddr);
	}

	//start to read
	for(i=0; i<len; i++)
	{
		//read data
		if(eeprom_type)
		{
			status = SmbusReceiveByte(eeprom_ch, 0x00, eeprom_addr, rbuf);
		}
		else
		{
			status = SmbusReadByte(eeprom_ch, (uint8_t)(eeprom_saddr + i), eeprom_addr, rbuf);
		}
		if(status != SMB_OK)
		{
			Print(L"Read Data Fail!! \n");
			goto sret_end;
		}

		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[0]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}


sret_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusWriteEepromTest(uint8_t option)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, wbuf[2], rbuf[2], spd[128];
	uint16_t i, len, eeprom_saddr = 0;

	clrscr();
	if(option)
	{
		Print(L"SMBus - Write EEPROM Test With Auto Increase Data. \n\n");
	}
	else
	{
		Print(L"SMBus - Write EEPROM Test. \n\n");
	}
	Print(L"This function can't workable when 'SPD write disabled' \n\n");

	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->eeprom->addr);
		console_color(LIGHTGRAY, BLACK);
		goto swet_end;
	}
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;
	wbuf[0] = gSmbusConfig->eeprom->wsval;
	
	if(eeprom_type)	//word
	{
		wbuf[1] = wbuf[0];
		for(i=0; i<len; i++)
		{
			//write data to eeprom
			wbuf[0] = (uint8_t)(eeprom_saddr + i);
			status = SmbusWriteWord(eeprom_ch, ((eeprom_saddr + i) >> 8), eeprom_addr, wbuf);
			if(status != SMB_OK)
			{
				Print(L"Write Data Fail!! \n");
				goto swet_end;
			}


			usleep(5000);	//must set more than 3ms


			//read data
			status = SmbusWriteByte(eeprom_ch, ((eeprom_saddr + i) >> 8), eeprom_addr, wbuf);
			status = SmbusReceiveByte(eeprom_ch, 0x00, eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto swet_end;
			}

			//print data
			if((i & 0x0F) == 0)
			{
				console_color(YELLOW, BLACK);
				Print(L"(%02x) ", i);
				console_color(LIGHTGRAY, BLACK);
			}
			Print(L"[%02X]", rbuf[0]);
			if((i & 0x0F) == 0x0F)
			{
				Print(L"\n");
			}

			//compare read/write data
			if(wbuf[1] != rbuf[0])
			{
				console_color(RED, BLACK);
				Print(L"EEPROM R/W Compare Error!! \n");
				goto swet_end;
			}

			//option is write auto increase
			if(option)
			{
				wbuf[1]++;
			}
		}
	}
	else	//byte
	{
		for(i=0; i<len; i++)
		{
			//write data to eeprom
			status = SmbusWriteByte(eeprom_ch, (uint8_t)(eeprom_saddr + i), eeprom_addr, wbuf);
			if(status != SMB_OK)
			{
				Print(L"Write Data Fail!! \n");
				goto swet_end;
			}


			usleep(5000);	//must set more than 3ms


			//read data
			status = SmbusReadByte(eeprom_ch, (uint8_t)(eeprom_saddr + i), eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto swet_end;
			}

			//print data
			if((i & 0x0F) == 0)
			{
				console_color(YELLOW, BLACK);
				Print(L"(%02x) ", i);
				console_color(LIGHTGRAY, BLACK);
			}
			Print(L"[%02X]", rbuf[0]);
			if((i & 0x0F) == 0x0F)
			{
				Print(L"\n");
			}

			//compare read/write data
			if(wbuf[0] != rbuf[0])
			{
				console_color(RED, BLACK);
				Print(L"EEPROM R/W Compare Error!! \n");
				goto swet_end;
			}

			//option is write auto increase
			if(option)
			{
				wbuf[0]++;
			}
		}
	}
	//read/write compare complete and success
	console_color(GREEN, BLACK);
	Print(L"EEPROM R/W Compare OK!! \n");

swet_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusCheckEEPROMBoundary(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, wbuf[2], rbuf[2], bakbuf[2], spd[128];
	uint8_t buf[2], count = 0;
	uint16_t i, tmp = 0;

	clrscr();
	Print(L"SMBus - Check EEPROM Boundary. \n\n");
	Print(L"This function can't workable when SPD write disabled \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->eeprom->addr);
		console_color(LIGHTGRAY, BLACK);
		goto sceb_end;
	}
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;

	if(eeprom_type)
	{
		eeprom_addr = gSmbusConfig->eeprom->addr;

		//backup eeprom last byte
		wbuf[0] = 0xFF;
		status = SmbusWriteByte(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		status = SmbusReceiveByte(eeprom_ch, 0x00, eeprom_addr, bakbuf);
		if(status != SMB_OK)
		{
			Print(L"Backup Data Fail!! \n");
			goto sceb_end;
		}
		
		//write data to eeprom
		wbuf[0] = 0xFF;
		wbuf[1] = 0x55;
		status = SmbusWriteWord(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}

		usleep(5000);	//must set more than 3ms

		for(i=1; i<8; i++)
		{
			//read data
			status = SmbusWriteByte(eeprom_ch, (0xFF >> i), eeprom_addr, wbuf);
			status = SmbusReceiveByte(eeprom_ch, 0x00, eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto sceb_end;
			}

			if(rbuf[0] != 0x55)
			{
				tmp = i - 1;
				break;
			}
		}

		usleep(5000);	//must set more than 3ms

		//write data to eeprom
		wbuf[0] = 0xFF;
		wbuf[1] = 0xAA;
		status = SmbusWriteWord(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}

		usleep(5000);	//must set more than 3ms

		for(i=tmp; i>=0; i--)
		{
			//read data
			status = SmbusWriteByte(eeprom_ch, (0xFF >> i), eeprom_addr, wbuf);
			status = SmbusReceiveByte(eeprom_ch, 0x00, eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto sceb_end;
			}

			if(rbuf[0] == 0xAA)
			{
				tmp = (0xFF >> i) + 1;
				Print(L"This EEPROM is %d k bits", tmp * 2);
				break;
			}

			if(i == 0)
			{
				Print(L"Check EEPROM Boundary Fail !! Please Check EEPROM Type !! \n\n");
				goto sceb_end;
			}
		}

		//restore eeprom last byte
		wbuf[0] = 0xFF;
		wbuf[1] = bakbuf[0];
		status = SmbusWriteWord(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}
	}
	else
	{
		//if type is byte mode, search 0xAx total size
		for(i=0xA0; i<0xAF; i+=2)
		{
			//check slave is SPD?
			if(PCHSmbusCheckSPD(eeprom_ch, (uint8_t)i, spd) == 0)
			{
				continue;
			}
			
			status = SmbusReadByte(eeprom_ch, 0x01, (uint8_t)i, rbuf);
			if(status == SMB_OK)
			{
				//communication success
				buf[0] = (uint8_t)i;
				count++;
			}
		}
		if(count == 1)
		{
			eeprom_addr = buf[0];
		}
		else
		{
			Print(L"This EEPROM total is %d k bits", count * 2);
			goto sceb_end;
		}

		//backup eeprom last byte
		status = SmbusReadByte(eeprom_ch, 0xFF, eeprom_addr, bakbuf);
		if(status != SMB_OK)
		{
			Print(L"Backup Data Fail!! \n");
			goto sceb_end;
		}
		
		//write data to eeprom
		wbuf[0] = 0x55;
		status = SmbusWriteByte(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}

		usleep(5000);	//must set more than 3ms

		for(i=1; i<8; i++)
		{
			//read data
			status = SmbusReadByte(eeprom_ch, (0xFF >> i), eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto sceb_end;
			}

			if(rbuf[0] != 0x55)
			{
				tmp = i - 1;
				break;
			}
		}

		usleep(5000);	//must set more than 3ms

		//write data to eeprom
		wbuf[0] = 0xAA;
		status = SmbusWriteByte(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}

		usleep(5000);	//must set more than 3ms

		for(i=tmp; i>=0; i--)
		{
			//read data
			status = SmbusReadByte(eeprom_ch, (0xFF >> i), eeprom_addr, rbuf);
			if(status != SMB_OK)
			{
				Print(L"Read Data Fail!! \n");
				goto sceb_end;
			}

			if(rbuf[0] == 0xAA)
			{
				tmp = (0xFF >> i) + 1;
				Print(L"This EEPROM is %d k bits", tmp * 8 /1024);
				break;
			}

			if(i == 0)
			{
				Print(L"Check EEPROM Boundary Fail !! Please Check EEPROM Type !! \n\n");
				goto sceb_end;
			}
		}

		//restore eeprom last byte
		wbuf[0] = bakbuf[0];
		status = SmbusWriteByte(eeprom_ch, 0xFF, eeprom_addr, wbuf);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sceb_end;
		}
	}

sceb_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusEEPROMTest(void)
{
	uint16_t key;
	
	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - EERPOM\n\n");
		Print(L"0. EERPOM Config Page. \n");
		Print(L"1. Test SMBus - Read EERPOM. \n");
		Print(L"2. Test SMBus - Write EERPOM. \n");
		Print(L"3. Test SMBus - Auto Write EEPROM. \n");
		Print(L"4. Test SMBus - Check EEPROM Boundary. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			break;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusEepromConfiguration();
				break;
			}
			case '1':
			{
				PCHSmbusReadEepromTest();
				break;
			}
			case '2':
			{
				PCHSmbusWriteEepromTest(0);
				break;
			}
			case '3':
			{
				PCHSmbusWriteEepromTest(1);
				break;
			}
			case '4':
			{
				PCHSmbusCheckEEPROMBoundary();
				break;
			}
		}
	}
}

void PCHSmbusReadThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t rbuf[2], spd[128];
	uint16_t tmp;

	clrscr();
	Print(L"SMBus - Read Thermal IC Test. \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->thermal->addr);
		console_color(LIGHTGRAY, BLACK);
		goto srt_end;
	}
	
	thermal_ch = gSmbusConfig->thermal->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr = gSmbusConfig->thermal->addr;

	//get chip Manufacturer ID
	if(SmbusReadByte(thermal_ch, LM_REG_MFU_ID, thermal_addr, rbuf))
	{
		Print(L"Read Manufacturer ID Error \n");
		goto srt_end;
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	if(SmbusReadByte(thermal_ch, LM_REG_CHIP_REV, thermal_addr, rbuf))
	{
		Print(L"Read Revision ID Error \n");
		goto srt_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//read Local Temp MSB
	if(SmbusReadByte(thermal_ch, LM_REG_TEMP_H, thermal_addr, rbuf))
	{
		Print(L"Read Local Temp MSB Error \n");
		goto srt_end;
	}
	tmp = rbuf[0];
	
	//read Local Temp LSB
	if(SmbusReadByte(thermal_ch, LM_REG_TEMP_L, thermal_addr, rbuf))
	{
		Print(L"Read Local Temp LSB Error \n");
		goto srt_end;
	}
	if(tmp & 0x80)
	{
		tmp = (256 - tmp) * 1000;
		tmp -= ((rbuf[0]>>5) * 125);
		Print(L"Local Temp:  -%3d.%03d C \n", tmp / 1000, tmp % 1000);
	}
	else
	{
		Print(L"Local Temp:   %3d.%03d C \n", tmp, (uint16_t)(rbuf[0]>>5)*125);
	}

	//read Remote Offset High Byte
	if(SmbusReadByte(thermal_ch, LM_REG_REMOTE_OFFSET_H, thermal_addr, rbuf))
	{
		Print(L"Read Remote Offset High Byte Error \n");
		goto srt_end;
	}
	tmp = rbuf[0];

	//read Remote Offset Low Byte
	if(SmbusReadByte(thermal_ch, LM_REG_REMOTE_OFFSET_L, thermal_addr, rbuf))
	{
		Print(L"Read Remote Offset Low Byte Error \n");
		goto srt_end;
	}
	if(tmp & 0x80)
	{
		tmp = (256 - tmp) * 1000;
		tmp -= ((rbuf[0]>>5) * 125);
		Print(L"Remote Temp:-%3d.%03d C \n", tmp / 1000, tmp % 1000);
	}
	else
	{
		Print(L"Remote Temp: %3d.%03d C \n", tmp, (uint16_t)(rbuf[0]>>5)*125);
	}

	//read Local Shared OS and T_Crit Limit
	if(SmbusReadByte(thermal_ch, LM_REG_T_CRIT_LMT, thermal_addr, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Error \n");
		goto srt_end;
	}
	Print(L"Local T_Crit Limit:  %3d C \n", (uint16_t)rbuf[0]);

	//read Remote OS Limit
	if(SmbusReadByte(thermal_ch, LM_REG_REMOTE_OS_LMT, thermal_addr, rbuf))
	{
		Print(L"Read Remote OS Limit Error \n");
		goto srt_end;
	}
	Print(L"Remote OS Limit:     %3d C \n", (uint16_t)rbuf[0]);

	//read Remote T_Crit Limit
	if(SmbusReadByte(thermal_ch, LM_REG_REMOTE_CRIT_LMT, thermal_addr, rbuf))
	{
		Print(L"Read Remote T_Crit Limit Error \n");
		goto srt_end;
	}
	Print(L"Remote T_Crit Limit: %3d C \n", (uint16_t)rbuf[0]);

	//read Common Hysteresis
	if(SmbusReadByte(thermal_ch, LM_REG_COMM_HYSTERESIS, thermal_addr, rbuf))
	{
		Print(L"Read Common Hysteresis Error \n");
		goto srt_end;
	}
	Print(L"Common Hysteresis:    %2d C \n", (uint16_t)rbuf[0]);

	//read Status Register 1
	if(SmbusReadByte(thermal_ch, LM_REG_STATUS_1, thermal_addr, rbuf))
	{
		Print(L"Read Status Register 1 Error \n");
		goto srt_end;
	}
	Print(L"Status1: ");
	if(rbuf[0] != 0)
	{
		Print(L"\n");
		if(rbuf[0] & LM_STA1_BIT_BUSY)
		{
			Print(L"  *Temperature is converting.\n");
		}
		if(rbuf[0] & LM_STA1_BIT_ROS)
		{
			Print(L"  *Remote OS asserted\n");
		}
		if(rbuf[0] & LM_STA1_BIT_DIODE)
		{
			Print(L"  *Diode missing\n");
		}
		if(rbuf[0] & LM_STA1_BIT_RTCRIT)
		{
			Print(L"  *Remote TCRIT asserted\n");
		}
		if(rbuf[0] & LM_STA1_BIT_LOC)
		{
			Print(L"  *Locat OS and TCRIT asserted\n");
		}
	}
	else
	{
		Print(L" No Event \n");
	}

srt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusWriteThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2], spd[128];
	uint16_t tmp;

	clrscr();
	Print(L"SMBus - Write Thermal IC Test. \n\n");

	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->thermal->addr);
		console_color(LIGHTGRAY, BLACK);
		goto swt_end;
	}
	
	thermal_ch = gSmbusConfig->thermal->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr =  gSmbusConfig->thermal->addr;

	//get chip Manufacturer ID
	if(SmbusReadByte(thermal_ch, LM_REG_MFU_ID, thermal_addr, rbuf))
	{
		Print(L"Read Manufacturer ID Error \n");
		goto swt_end;
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	if(SmbusReadByte(thermal_ch, LM_REG_CHIP_REV, thermal_addr, rbuf))
	{
		Print(L"Read Revision ID Error \n");
		goto swt_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//enter new value
	wbuf[0] = gSmbusConfig->thermal->wsval;

	//write Local Shared OS and T_Crit Limit
	if(SmbusWriteByte(thermal_ch, LM_REG_T_CRIT_LMT, thermal_addr, wbuf))
	{
		Print(L"Write Local Shared OS and T_Crit Limit Error \n");
		goto swt_end;
	}

	//read Local Shared OS and T_Crit Limit
	if(SmbusReadByte(thermal_ch, LM_REG_T_CRIT_LMT, thermal_addr, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Error \n");
		goto swt_end;
	}

	//compare value
	if(wbuf[0] == rbuf[0])
	{
		console_color(GREEN, BLACK);
		Print(L"Write New OTP Threshold OK!! \n");
	}
	else
	{
		console_color(RED, BLACK);
		Print(L"Write New OTP Threshold Fail!! \n");
	}

swt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusThermalICTest(void)
{
	uint16_t key;
	
	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - Thermal\n\n");
		Print(L"0. Thermal Config Page. \n");
		Print(L"1. Test SMBus - Read Thermal IC. \n");
		Print(L"2. Test SMBus - Write Thermal IC. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			break;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusThermalConfiguration();
				break;
			}
			case '1':
			{
				PCHSmbusReadThermalICTest();
				break;
			}
			case '2':
			{
				PCHSmbusWriteThermalICTest();
				break;
			}
		}
	}
}

void PCHSmbusEatBoardTest(void)
{
	uint16_t key;
	
	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - EAT Board\n\n");
		Print(L"1. Test SMBus - EERPOM. \n");
		Print(L"2. Test SMBus - Thermal. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			break;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '1':
			{
				PCHSmbusEEPROMTest();
				break;
			}
			case '2':
			{
				PCHSmbusThermalICTest();
				break;
			}
		}
	}
}

void PCHSmbusDIOBoardTest(void)
{
	uint16_t key;

	//scan smbus
	PCHSmbusScanDevice();

	//search dio board address
	if(DIOBoardSearch(gSmbus))
	{
		Print(L"ERROR: Failed to found PCA9555 chip.\n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");
		while(bioskey(0) != (SCAN_ESC << 8)){}
		
		goto psdbt_end;
	}
	
	//backup dio board register
	DIOBackupRegister();

	//test loop
	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - DIO Board\n\n");
		//Print(L"0. DIO Board Config Page. \n");
		Print(L"1. Test SMBus - Read DIO Board\n");
		Print(L"2. Test SMBus - Write DIO Board\n");
		Print(L"3. Test SMBus - Auto R/W DIO Board\n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			/*case '0':
			{
				SmbusDIOBoardConfiguration();
				break;
			}*/
			case '1':
			{
				DIOBoardRead();
				break;
			}
			case '2':
			{
				DIOBoardWrite();
				break;
			}
			case '3':
			{
				DIOBoardAuto();
				break;
			}
		}
	}

	//restore dio board register
	DIORestoreRegister();

psdbt_end:
	NULL;
}

void PCHSmbusShowHardwareMonitorScreen(HWM_HANDLE_ST *hwm)
{
	CHAR16	*ChName = NULL;
	UINT8	CursorY = 0;
	UINTN	Index;
	UINTN	Delay = 0xFFFF;
	UINT16	Value = 0;
	EFI_STATUS	Status = EFI_SUCCESS;

	gST->ConOut->ClearScreen(gST->ConOut);
	print_title(hwm->ChipName);

	// Print Monitor Item & Info
	gotoxy(0, 4);
	if(hwm->MaxCh[HWM_TYPE_VOLT] > 0)
	{
		Print(L"Voltage:\n");
		for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_VOLT]; Index++)
		{
			Status = hwm->GetChannelName(HWM_TYPE_VOLT, Index, &ChName);
			if(Status == EFI_UNSUPPORTED) 
			{
				continue;
			}
			if(Status != EFI_SUCCESS) 
			{
				break;
			}

			Print(L"%10s :\n", ChName);

		}
		Print(L"\n\n");
	}

	if(hwm->MaxCh[HWM_TYPE_FAN] > 0)
	{
		Print(L"Fan Speed:\n");
		for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_FAN]; Index++)
		{
			Status = hwm->GetChannelName(HWM_TYPE_FAN, Index, &ChName);
			if(Status == EFI_UNSUPPORTED)
			{
				continue;
			}
			if(Status != EFI_SUCCESS) 
			{
				break;
			}

			Print(L"%10s :\n", ChName);
		}
		Print(L"\n\n");
	}

	CursorY = 4;
	if(hwm->MaxCh[HWM_TYPE_CURR] > 0)
	{
		gotoxy(35, CursorY);
		Print(L"Current:"); 
		CursorY++;
		for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_CURR]; Index++)
		{
			Status = hwm->GetChannelName(HWM_TYPE_CURR, Index, &ChName);
			if(Status == EFI_UNSUPPORTED) 
			{
				continue;
			}
			if(Status != EFI_SUCCESS) 
			{
				break;
			}

			gotoxy(35, CursorY);
			Print(L"%10s :", ChName);
			CursorY++;
		}
		CursorY += 2; // for one item 
	}

	if(hwm->MaxCh[HWM_TYPE_TEMP] > 0)
	{
		gotoxy(35, CursorY);
		Print(L"Temperature:");
		CursorY++;
		for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_TEMP]; Index++)
		{
			Status = hwm->GetChannelName(HWM_TYPE_TEMP, Index, &ChName);
			if(Status == EFI_UNSUPPORTED) 
			{
				continue;
			}
			if(Status != EFI_SUCCESS) 
			{
				break;
			}

			gotoxy(35, CursorY);
			Print(L"%10s :", ChName);
			CursorY++;
		}
		CursorY += 2; // for one item 
	}

	gotoxy(0, ConsoleHeight - 3);
	Print(L"Note: The above voltages are pin's voltage. They don't mean the real voltage.\n\n"); 

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to quit");

	// Read/Print Monitor Data
	do{
		if(Delay++ < 200)
		{
			if(bioskey(1) == SCAN_ESC << 8)
			{
				break;
			}
			gBS->Stall(1000);	// 1ms
			continue;
		}
		else
		{
			Delay = 0;
		}
		CursorY = 4; 
		if(hwm->MaxCh[HWM_TYPE_VOLT] > 0)
		{
			CursorY++;	// for Type Title
			for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_VOLT]; Index++)
			{
				Status = hwm->GetReading(HWM_TYPE_VOLT, Index, &Value);
				if(Status != EFI_SUCCESS)
				{
					break;
				}
				gotoxy(13, CursorY);
				Print(L"%4d.%02dV", Value * hwm->Resolution[HWM_TYPE_VOLT] / HMW_RESOLUTION_BASE, Value * hwm->Resolution[HWM_TYPE_VOLT] % HMW_RESOLUTION_BASE);
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		if(hwm->MaxCh[HWM_TYPE_FAN] > 0)
		{
			CursorY++; // for Type Title
			for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_FAN]; Index++)
			{
				Status = hwm->GetReading(HWM_TYPE_FAN, Index, &Value);
				if(Status != EFI_SUCCESS)
				{
					break;
				}
				gotoxy(13, CursorY);
				if(Value != 0xFFFF) 
				{
					Print(L"%6d RPM", (UINT32)Value * hwm->Resolution[HWM_TYPE_FAN] / HMW_RESOLUTION_BASE); 
				}
				else
				{
					Print(L"  Overflow");
				}
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		CursorY = 4; 
		if(hwm->MaxCh[HWM_TYPE_CURR] > 0)
		{
			CursorY++; // for Type Title
			for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_CURR]; Index++)
			{
				Status = hwm->GetReading(HWM_TYPE_CURR, Index, &Value);
				if(Status != EFI_SUCCESS)
				{
					break;
				}
				gotoxy(48, CursorY);
				Print(L"%4d.%1dA", Value * hwm->Resolution[HWM_TYPE_CURR] / HMW_RESOLUTION_BASE, Value * hwm->Resolution[HWM_TYPE_CURR] % HMW_RESOLUTION_BASE);
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		if(hwm->MaxCh[HWM_TYPE_TEMP] > 0)
		{
			CursorY++; // for Type Title
			for(Index = 0; Index < hwm->MaxCh[HWM_TYPE_TEMP]; Index++)
			{
				Status = hwm->GetReading(HWM_TYPE_TEMP, Index, &Value);
				if(Status != EFI_SUCCESS)
				{
					break;
				}
				gotoxy(48, CursorY);
				if(Value & 0x8000)
				{
					Value = (~Value) + 1;
					Print(L"-%3d.%1dC", Value * hwm->Resolution[HWM_TYPE_TEMP] / HMW_RESOLUTION_BASE, Value * hwm->Resolution[HWM_TYPE_TEMP] % HMW_RESOLUTION_BASE);
				}
				else
				{
					Print(L" %3d.%1dC", Value * hwm->Resolution[HWM_TYPE_TEMP] / HMW_RESOLUTION_BASE, Value * hwm->Resolution[HWM_TYPE_TEMP] % HMW_RESOLUTION_BASE);
				}
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}
	}while(1);
}

void PCHSmbusHardwareMonitorChipTest(void)
{
	uint16_t key;
	uint16_t 	u16;
	uint8_t	bank;
	uint8_t	addr, data;
	HWM_HANDLE_ST	*hwm[2];
	HWM_HANDLE_ST	**hwmp;
	
	//scan smbus
	PCHSmbusScanDevice();

	//check hardware monitor chip
	hwm[0] = NULL;
	hwm[1] = NULL;
	HardwareMonitorInit(gSmbus, HWM_CHIP_W83782, &hwm[0]);
	HardwareMonitorInit(gSmbus, HWM_CHIP_NCT7802, &hwm[1]);
	if (hwm[0] == NULL && hwm[1] == NULL)
	{
		Print(L"ERROR: Failed to found hardware monitor chip.\n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");
		
		goto pshmt_end;
	}

	//test loop
	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - Hardware Monitor Chip\n\n");
		Print(L"1. Test SMBus - Winbond W83782. \n");
		Print(L"2. Test SMBus - Nuvoton NCT7802. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			goto pshmt_endend;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '1':
			{
				if(hwm[0] == NULL)
				{
					Print(L"W83782 not found.\n");
					goto pshmt_end;
				}
				PCHSmbusShowHardwareMonitorScreen(hwm[0]);
				break;
			}
			case '2':
			{
				if(hwm[1] == NULL)
				{
					Print(L"NCT7802 not found.\n");
					goto pshmt_end;
				}
				PCHSmbusShowHardwareMonitorScreen(hwm[1]);
				break;
			}
			case 'r':	// debug
			{
				hwmp = &hwm[0];
				goto hwmdbg;
			}
			case 'R': // debug
			{
				hwmp = &hwm[1];
hwmdbg:
				if(*hwmp == NULL)
				{
					Print(L"HWN not found.\n");
				}
				else
				{
					Print(L"Read %s Register\n", (*hwmp)->ChipName);
					u16 = get_number_stdin("Enter Register bank:", 1, 1);
					bank = (uint8_t) u16;
					u16 = get_number_stdin("Enter Register address:", 1, 2);
					addr = (uint8_t) u16;
					(*hwmp)->ReadReg(bank, addr, &data);
					Print(L"Get Byte u8rb: 0x%X\n", data);
				}
				goto pshmt_end;
			}
		}
	}

pshmt_end:
	while(bioskey(0) != (SCAN_ESC << 8)){}

pshmt_endend:
	NULL;
}

void PCHSmbusReadSmartBatteryTest(void)
{
	uint8_t battery_ch = 0, battery_addr = 0x16, battery_max_cmd = 35;
	uint8_t j = 0, max_ddata = 32, offset = 0, max_block_data = 32;
	uint8_t *block_data;
	uint8_t *ddata, spd[128];
	uint8_t blen;
	UINTN x_cursor, y_cursor;
	uint16_t str16[32];
	
	const uint16_t *SmartBatteryRegName[]={
		L"Manufacturer",
		L"RemainingCapacityAlarm",
		L"RemainingTimeAlarm",
		L"BatteryMode",
		L"AtRate",
		L"AtRateTimeToFull",
		L"AtRateTimeToEmpty",
		L"AtRateOK",
		L"Temperature",
		L"Voltage",
		L"Current",
		L"AverageCurrent",
		L"MaxError",
		L"RelativeStateOfCharge",
		L"AbsoluteStateOfCharge",
		L"RemainingCapacity",
		L"FullChargeCapacity",
		L"RunTimeToEmpty",
		L"AverageTimeToEmpty",
		L"AverageTimeToFull",
		L"ChargingCurrent", 
		L"ChargingVoltage",
		L"BatteryStatus",
		L"CycleCount",
		L"DesignCapacity",
		L"DesignVoltage",
		L"SpecificationInfo",
		L"ManufactureDate",
		L"SerialNumber",
		L"ManufacturerName",
		L"DeviceName",
		L"DeviceChemistry",
		//"ManufacturerData",
		};

	ddata = malloc(max_ddata);
	block_data = malloc(max_block_data);
	if(!ddata)
	{
		Print(L"malloc error \n");
		goto malloc_error;
	}
	if(!block_data)
	{
		Print(L"malloc error \n");
		free(ddata);
		goto malloc_error;
	}
	
	memset(ddata, '\0', max_ddata);
	memset(block_data, '\0', max_block_data);

	clrscr();
	Print(L"SMBus - Read Smart Battery \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->battery->ch, gSmbusConfig->battery->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->battery->addr);
		console_color(LIGHTGRAY, BLACK);
		goto malloc_error;
	}
	
	battery_ch = gSmbusConfig->battery->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->battery->ch, gSmbusConfig->battery->freq);
	battery_addr = gSmbusConfig->battery->addr;
	
	for(j=0; j<battery_max_cmd; j++)
	{
		if(offset >= sizeof(SmartBatteryRegName)/sizeof(*SmartBatteryRegName))
		{
			Print(L"\n");
			//gST->ConOut->SetAttribute(gST->ConOut, 0x07);
			break;
		}
		
		y_cursor = gST->ConOut->Mode->CursorRow;
		if(offset%2)
		{
			gotoxy(ConsoleWidth/2, y_cursor);
			x_cursor = ConsoleWidth - 13;
		}
		else
		{
			gotoxy(0, y_cursor);
			x_cursor = ConsoleWidth/2 -13;
		}
		
		if(j<29)
		{
			if(SmbusReadWord(battery_ch, j, battery_addr, ddata) != SMB_OK)
			{
				Print(L"Read Command %d Error \n", (uint16_t)j);
				goto read_error;
			}

			//gST->ConOut->SetAttribute(gST->ConOut, 0x07);	//0:black 1:blue 2:green 3:cyan 4:red 5:magenta 6:yellow 7:light gray
			Print(L"%02X : ", (uint16_t)j);
			Print(L"%s", *(SmartBatteryRegName + offset));
			
			//gST->ConOut->SetAttribute(gST->ConOut, 0x02);
			gotoxy(x_cursor, y_cursor);
			Print(L": %04X", (uint16_t)(((uint16_t)ddata[1]<<8) | ddata[0]));
			if((offset%2))
			{
				Print(L"\n");
			}
			offset++;
		}
		else if(j>31)
		{
			if(SmbusReadBlock(battery_ch, j, battery_addr, block_data, &blen) != SMB_OK)
			{
				Print(L"Read Command %d Error \n", (uint16_t)j);
				goto read_error;
			}

			Print(L"%02X : ", (uint16_t)j);
			Print(L"%s", *(SmartBatteryRegName + offset));

			gotoxy(x_cursor, y_cursor);
			AsciiStrToUnicodeStr(block_data, str16);
			Print(L": %s", str16);

			if((offset%2))
			{
				Print(L"\n");
			}
			memset(block_data, '\0', max_block_data);
			offset++;
		}
	}

read_error:
	free(ddata);
	free(block_data);
	if(j == 0)
	{
		if(gSmbus->found[battery_ch])
		{
			console_color(RED, BLACK);
			Print(L"Please Check Smart Battery Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
		}
	}
	
malloc_error:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void PCHSmbusSmartBatteryTest(void)
{
	uint16_t key;

	while(1)
	{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"Test SMBus - Smart Battery\n\n");
		Print(L"0. Smart Battery Config Page. \n");
		Print(L"1. Test SMBus - Read Smart Battery. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			break;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusSmartBatteryConfiguration();
				break;
			}
			case '1':
			{
				PCHSmbusReadSmartBatteryTest();
				break;
			}
		}
	}
	
}

void PCHSmbusReadAnotherDeviceTest(void)
{
	uint8_t dev_ch = 0, dev_addr = 0, dev_cmd = 0;
	uint8_t status, rbuf[2], spd[128];
	uint16_t i, len;
	
	clrscr();
	Print(L"SMBus - Read Another Device Test. \n\n");

	while(1)
	{
		gotoxy(0, 2);
		Print(L"Press C to config parameter, Enter to start test, ESC to return");
		switch(bioskey(0))
		{
			case 'C':
			case 'c':
			{
				SmbusDeviceConfiguration();
				clrscr();
				Print(L"SMBus - Read Another Device Test. \n\n");
				break;
			}
			case CHAR_CARRIAGE_RETURN:
			{
				goto start_test;
				break;
			}
			case (SCAN_ESC << 8):
			{
				goto sret_endend;
				break;
			}
		}
	}
	
start_test:
	clrscr();
	Print(L"SMBus - Read Another Device Test. \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->dev->ch, gSmbusConfig->dev->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->dev->addr);
		console_color(LIGHTGRAY, BLACK);
		goto sret_end;
	}
	
	dev_ch = gSmbusConfig->dev->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->dev->ch, gSmbusConfig->dev->freq);
	dev_addr = gSmbusConfig->dev->addr;
	dev_cmd = gSmbusConfig->dev->cmd;
	len = 1;//gSmbusConfig->dev->rwlen;
	
	//start to read
	//read data
	status = SmbusReadByte(dev_ch, dev_cmd, dev_addr, rbuf);
	if(status != SMB_OK)
	{
		Print(L"Read Data Fail!! \n");
		goto sret_end;
	}
	
	for(i=0; i<len; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[0]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

sret_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
	
sret_endend:
	NULL;
}

void PCHSmbusWriteAnotherDeviceTest(void)
{
	uint8_t dev_ch = 0, dev_addr = 0, dev_cmd = 0;
	uint8_t status, wbuf[2], rbuf[2], spd[128];
	uint16_t i, len;

	clrscr();
	Print(L"SMBus - Write Another Device Test. \n\n");

	while(1)
	{
		gotoxy(0, 2);
		Print(L"Press C to config parameter, Enter to start test, ESC to return");
		switch(bioskey(0))
		{
			case 'C':
			case 'c':
			{
				SmbusDeviceConfiguration();
				clrscr();
				Print(L"SMBus - Write Another Device Test. \n\n");
				break;
			}
			case CHAR_CARRIAGE_RETURN:
			{
				goto start_test;
				break;
			}
			case (SCAN_ESC << 8):
			{
				goto swet_endend;
				break;
			}
		}
	}
	
start_test:
	clrscr();
	Print(L"SMBus - Write Another Device Test. \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->dev->ch, gSmbusConfig->dev->addr, spd) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Address %x is SPD, Stop test !! \n", gSmbusConfig->dev->addr);
		console_color(LIGHTGRAY, BLACK);
		goto swet_end;
	}
	
	dev_ch = gSmbusConfig->dev->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	dev_addr = gSmbusConfig->dev->addr;
	dev_cmd = gSmbusConfig->dev->cmd;
	len = 1;//gSmbusConfig->dev->rwlen;
	wbuf[0] = gSmbusConfig->dev->wsval;
	
	//write data to device
	status = SmbusWriteByte(dev_ch, dev_cmd, dev_addr, wbuf);
	if(status != SMB_OK)
	{
		Print(L"Write Data Fail!! \n");
		goto swet_end;
	}


	usleep(5000);


	//read data
	status = SmbusReadByte(dev_ch, dev_cmd, dev_addr, rbuf);
	if(status != SMB_OK)
	{
		Print(L"Read Data Fail!! \n");
		goto swet_end;
	}

	for(i=0; i<len; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[0]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}

		//compare read/write data
		if(wbuf[0] != rbuf[0])
		{
			console_color(RED, BLACK);
			Print(L"R/W Compare Error!! \n");
			goto swet_end;
		}
	}
	//read/write compare complete and success
	console_color(GREEN, BLACK);
	Print(L"R/W Compare OK!! \n");

swet_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
	
swet_endend:
	NULL;
}

void PCHSmbusSPD(void)
{
	uint8_t spd_ch = 0, source_addr = 0, destination_addr = 0;
	uint8_t status, wbuf[256], rbuf[256], fbuf[256], cbuf[256], spd[128];
	uint16_t i, key;

	FILE *fp;

	clrscr();
	Print(L"SMBus - Write SPD. \n\n");
	Print(L"This function can't workable when 'SPD write disabled' \n\n");

	while(1)
	{
		gotoxy(0, 5);
		Print(L"Press C to config parameter, Enter to start test, ESC to return");
		switch(bioskey(0))
		{
			case 'C':
			case 'c':
			{
				SmbusSPDConfiguration();
				clrscr();
				Print(L"SMBus - Write SPD. \n\n");
				Print(L"This function can't workable when 'SPD write disabled' \n\n");
				break;
			}
			case CHAR_CARRIAGE_RETURN:
			{
				goto start_test;
				break;
			}
			case (SCAN_ESC << 8):
			{
				goto sspd_endend;
				break;
			}
		}
	}
	
start_test:
	clrscr();
	Print(L"SMBus - Write SPD. \n\n");
	Print(L"This function can't workable when 'SPD write disabled' \n\n");
	
	//check slave is SPD?
	if(PCHSmbusCheckSPD(gSmbusConfig->spd->ch, gSmbusConfig->spd->source_addr, spd) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Source address %x not SPD, Halt !! \n", gSmbusConfig->spd->source_addr);
		console_color(LIGHTGRAY, BLACK);
		goto sspd_end;
	}
	
	spd_ch = gSmbusConfig->spd->ch;
	//SmbusSetClockFrequency(smb, gSmbusConfig->spd->ch, gSmbusConfig->spd->freq);
	source_addr = gSmbusConfig->spd->source_addr;
	destination_addr = gSmbusConfig->spd->destination_addr;

	//read source SPD data
	fp = NULL;
	fp = fopen("SPD.txt", "wb");
	if(fp)
	{
		for(i=0; i<256; i++)
		{
			status = SmbusReadByte(spd_ch, (uint8_t)i, source_addr, &rbuf[i]);
			if(status != SMB_OK)
			{
				fclose(fp);
				Print(L"Read Data Fail!! \n");
				goto sspd_end;
			}

			fwrite(&rbuf[i], 1, 1, fp);
		}
		
		fclose(fp);
	}
	else
	{
		Print(L"Create SPD.txt Fail!! \n");
		goto sspd_end;
	}
	
	//compare source SPD data and SPD.txt data
	fp = NULL;
	fp = fopen("SPD.txt", "rb");
	if(fp)
	{
		if(fread(fbuf, 1, 256, fp) != 256)
		{
			fclose(fp);
			goto sspd_end;
		}
		for(i=0; i<256; i++)
		{
			//print data
			if((i & 0x0F) == 0)
			{
				console_color(YELLOW, BLACK);
				Print(L"(%02x) ", i);
				console_color(LIGHTGRAY, BLACK);
			}
			Print(L"[%02X]",fbuf[i]);
			if((i & 0x0F) == 0x0F)
			{
				Print(L"\n");
			}

			//compare read/write data
			if(fbuf[i] != rbuf[i])
			{
				fclose(fp);
				console_color(RED, BLACK);
				Print(L"SPD File Compare Error!! \n");
				goto sspd_end;
			}
		}
		
		fclose(fp);
	}
	
	//prepare write data
	for(i=0; i<256; i++)
	{
		wbuf[i] = fbuf[i];
	}
	
	//check again
	while(1)
	{
		Print(L"\n\nSPD source data prepare ok, Are you sure write to destination SPD? (Y:Yes  N:No)\n\n");
		key = (uint16_t)bioskey(0);
		if((key == 'Y') || (key == 'y'))
		{
			break;
		}
		else if((key == 'N') || (key == 'n'))
		{
			Print(L"Process Exit !!\n");
			goto sspd_end;
		}
	}

	//check again
	while(1)
	{
		console_color(RED, BLACK);
		Print(L"\nAre you sure? (Y:Yes  N:No)\n\n");
		console_color(LIGHTGRAY, BLACK);
		
		key = (uint16_t)bioskey(0);
		if((key == 'Y') || (key == 'y'))
		{
			break;
		}
		else if((key == 'N') || (key == 'n'))
		{
			Print(L"Process Exit !!\n");
			goto sspd_end;
		}
	}
	
	//write destination SPD data
	for(i=0; i<256; i++)
	{
		//write data to eeprom
		status = SmbusWriteByte(spd_ch, (uint8_t)i, destination_addr, &wbuf[i]);
		if(status != SMB_OK)
		{
			Print(L"Write Data Fail!! \n");
			goto sspd_end;
		}


		usleep(5000);


		//read data
		status = SmbusReadByte(spd_ch, (uint8_t)i, destination_addr, &cbuf[i]);
		if(status != SMB_OK)
		{
			Print(L"Read Data Fail!! \n");
			goto sspd_end;
		}

		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", cbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}

		//compare read/write data
		if(wbuf[i] != cbuf[i])
		{
			console_color(RED, BLACK);
			Print(L"SPD Data Compare Error!! \n");
			goto sspd_end;
		}
	}
	//read/write compare complete and success
	console_color(GREEN, BLACK);
	Print(L"SPD Data Compare OK!! \n");

sspd_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
	
sspd_endend:
	NULL;
}

/*==============================================================*/
// MAIN
/*==============================================================*/
EFI_STATUS main(int argc, char **argv)
{
	EFI_STATUS ret = 0;
	uint8_t spd_flag = 0;
	uint16_t key;

	ret = console_init();
	if(ret != EFI_SUCCESS)
	{
		Print(L"ERROR: Failed to change console setting. \n");
		return ret;
	}
	
	clrscr();

	gSmbus = smb_init();
	if(!gSmbus)
	{
		Print(L"ERROR: Failed to initialize SMBus\n");
		ret = EFI_DEVICE_ERROR;
		goto end;
	}
	
	usleep(100);
	
	if(gSmbusConfig == NULL)
	{
		gSmbusConfig = (SmbusVariableConfig*)malloc(sizeof(SmbusVariableConfig));
		if(!gSmbusConfig)
		{
			Print(L"Smbus Config Data Fail !! \n\n");
			goto end;
		}
		memset(gSmbusConfig, 0, sizeof(SmbusVariableConfig));

		gSmbusConfig->eeprom = (Eeprom*)malloc(sizeof(Eeprom));
		if(gSmbusConfig->eeprom)
		{
			memset(gSmbusConfig->eeprom, 0, sizeof(Eeprom));
		}
		
		gSmbusConfig->thermal = (Thermal*)malloc(sizeof(Thermal));
		if(gSmbusConfig->thermal)
		{
			memset(gSmbusConfig->thermal, 0, sizeof(Thermal));
		}
		
		gSmbusConfig->dio = (DIO*)malloc(sizeof(DIO));
		if(gSmbusConfig->dio)
		{
			memset(gSmbusConfig->dio, 0, sizeof(DIO));
		}
		
		gSmbusConfig->dev = (Device*)malloc(sizeof(Device));
		if(gSmbusConfig->dev)
		{
			memset(gSmbusConfig->dev, 0, sizeof(Device));
		}
		
		gSmbusConfig->battery = (BATTERY*)malloc(sizeof(BATTERY));
		if(gSmbusConfig->battery)
		{
			memset(gSmbusConfig->battery, 0, sizeof(BATTERY));
		}

		gSmbusConfig->spd = (SPD*)malloc(sizeof(SPD));
		if(gSmbusConfig->spd)
		{
			memset(gSmbusConfig->spd, 0, sizeof(SPD));
		}

		SmbusTestConfigDefault();
	}

	//check rescue SPD function enable
	if(argc == 2)
	{
		if(strcmp(argv[1], "-rescuespd") == 0)
		{
			spd_flag = 1;
		}
	}
	
	// App Main menu
	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		Print(L"\nCopyright(c) , Advantech Corporation 2017-2018\n");
		Print(L"Advantech PCH SMBus utility.\n");
		Print(L"Date:%s\n",SMBTEST_DATE);
		Print(L"Version:%s\n\n",SMBTEST_VER);

		Print(L"1. List SMBus Device Address. \n");
		Print(L"2. Test SMBus - EAT Board. \n");
		Print(L"3. Test SMBus - DIO Board. \n");
		Print(L"4. Test SMBus - Hardware Monitor Chip. \n");
		Print(L"5. Test SMBus - Smart Battery. \n");
		/*Print(L"A. Test SMBus - Read Another Device. \n");
		Print(L"B. Test SMBus - Write Another Device. \n");*/
		
		if(spd_flag)
		{
			Print(L"S. Rescue SPD. \n");
		}
		
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = (uint16_t)bioskey(0);
		if(key == SCAN_ESC << 8)
		{
			goto endend;
		}
		
		gST->ConOut->ClearScreen(gST->ConOut);
		switch(key & 0xFF)
		{
			case '1':
			{
				PCHSmbusListDeviceAddress();
				break;
			}
			case '2':
			{
				PCHSmbusEatBoardTest();
				break;
			}
			case '3':
			{
				PCHSmbusDIOBoardTest();
				break;
			}
			case '4':
			{
				PCHSmbusHardwareMonitorChipTest();
				break;
			}
			case '5':
			{
				PCHSmbusSmartBatteryTest();
				break;
			}
			/*
			case 'A':
			case 'a':
			{
				PCHSmbusReadAnotherDeviceTest();
				break;
			}
			case 'B':
			case 'b':
			{
				PCHSmbusWriteAnotherDeviceTest();
				break;
			}*/
			case 'S':
			case 's':
			{
				if(spd_flag)
				{
					PCHSmbusSPD();
				}
				break;
			}
		}
	} while(1);

end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}

endend:
	console_exit();
	
	return ret;
}


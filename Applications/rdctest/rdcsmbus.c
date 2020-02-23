#include "rdctest.h"



RDC_SMBUS gSMB[2];
SmbusVariableConfig *gSmbusConfig = NULL;



void SmbusTestConfigDefault(void)
{
	gSmbusConfig->eeprom->ch = 0;
	gSmbusConfig->eeprom->freq = 0;
	gSmbusConfig->eeprom->type= 0;
	gSmbusConfig->eeprom->addr = 0xA0;
	gSmbusConfig->eeprom->spos = 0x00;
	gSmbusConfig->eeprom->rwlen = 256;
	gSmbusConfig->eeprom->wsval = 0x00;
	
	gSmbusConfig->thermal->ch = 0;
	gSmbusConfig->thermal->freq= 0;
	gSmbusConfig->thermal->addr = 0x98;
	gSmbusConfig->thermal->wsval= 85;
	
	gSmbusConfig->dio->ch = 0;
	gSmbusConfig->dio->freq= 0;
	gSmbusConfig->dio->addr = 0x40;
	gSmbusConfig->dio->wsval= 0x0000;

	gSmbusConfig->battery->ch = 0;
	gSmbusConfig->battery->freq= 0;
	gSmbusConfig->battery->addr = 0x16;
}

void SmbusEepromConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Eeprom Configuration Page \n\n");
	
	Print(L"Channel(0:Smb0 1:Smb1)   : %d\n", gSmbusConfig->eeprom->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->eeprom->freq);
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
			if(item < 6)
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
					case 1:
					case 2:
					case 5:
					{
						value = value / 10;
						break;
					}
					case 3:
					case 4:
					case 6:
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
				case 1:
				case 2:
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
				case 5:
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
				case 3:
				case 4:
				case 6:
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
				case 1:
				{
					gSmbusConfig->eeprom->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->eeprom->freq);
					break;
				}
				case 2:
				{
					gSmbusConfig->eeprom->type = (uint8_t)value;
					Print(L"%d", gSmbusConfig->eeprom->type);
					
					gotoxy(0, 6);
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
				case 3:
				{
					gSmbusConfig->eeprom->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->eeprom->addr);
					break;
				}
				case 4:
				{
					gSmbusConfig->eeprom->spos = value;
					Print(L"%-4X", gSmbusConfig->eeprom->spos);
					break;
				}
				case 5:
				{
					gSmbusConfig->eeprom->rwlen = value;
					Print(L"%-3d", gSmbusConfig->eeprom->rwlen);
					break;
				}
				case 6:
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
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->eeprom->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->eeprom->type;
					Print(L"%d", value);
					break;
				}
				case 3:
				{
					sprintf(buf, "%X", gSmbusConfig->eeprom->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->eeprom->addr;
					Print(L"%-2X", value);
					break;
				}
				case 4:
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
				case 5:
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
				case 6:
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
		case 1:
		{
			gSmbusConfig->eeprom->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->eeprom->type = (uint8_t)value;
			break;
		}
		case 3:
		{
			gSmbusConfig->eeprom->addr = (uint8_t)value;
			break;
		}
		case 4:
		{
			gSmbusConfig->eeprom->spos = value;
			break;
		}
		case 5:
		{
			gSmbusConfig->eeprom->rwlen = value;
			break;
		}
		case 6:
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
	Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->thermal->freq);
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
					case 1:
					case 3:
					{
						value = value / 10;
						break;
					}
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
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
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
				case 3:
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
				case 1:
				{
					gSmbusConfig->thermal->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->thermal->freq);
					break;
				}
				case 2:
				{
					gSmbusConfig->thermal->addr = (uint8_t)value;
					Print(L"%-2X", gSmbusConfig->thermal->addr);
					break;
				}
				case 3:
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
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->thermal->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%x", gSmbusConfig->thermal->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gSmbusConfig->thermal->addr;
					Print(L"%-2X", value);
					break;
				}
				case 3:
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
		case 1:
		{
			gSmbusConfig->thermal->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->thermal->addr = (uint8_t)value;
			break;
		}
		case 3:
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
	Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->dio->freq);
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
					case 1:
					{
						value = value / 10;
						break;
					}
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
			gotoxy(27 + len, item + 2);
			
			switch(item)
			{
				case 0:
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
				case 1:
				{
					gSmbusConfig->dio->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->dio->freq);
					break;
				}
				case 2:
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
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->dio->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
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
		case 1:
		{
			gSmbusConfig->dio->freq = (uint8_t)value;
			break;
		}
		case 2:
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
	Print(L"Frequency(0:100K 1:400K) : %d\n", gSmbusConfig->battery->freq);
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
					case 1:
					{
						value = value / 10;
						break;
					}
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
				case 1:
				{
					gSmbusConfig->battery->freq = (uint8_t)value;
					Print(L"%d", gSmbusConfig->battery->freq);
					break;
				}
				case 2:
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
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gSmbusConfig->battery->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
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
		case 1:
		{
			gSmbusConfig->battery->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gSmbusConfig->battery->addr = (uint8_t)value;
			break;
		}
	}
}

uint8_t InitSmbus(void)
{
	uint8_t ch, ldn, u8tmp;
	uint8_t prescale1, prescale2;
	uint16_t u16temp;

	for(ch=0; ch<2; ch++)
	{
		gSMB[ch].Flag.init = 0;
		
		//Open PNP port
		sys_open_ioport(RDC_PNP_INDEX);
		sys_open_ioport(RDC_PNP_DATA);
		
		outp(RDC_CFG_IDX_PORT, 0x87);	// enter config
		outp(RDC_CFG_IDX_PORT, 0x87);

		//Check Chip
		outp(RDC_CFG_IDX_PORT, 0x20);	// ID0
		u16temp = (uint16_t)inp(RDC_CFG_DAT_PORT);
		
		outp(RDC_CFG_IDX_PORT, 0x21);	// ID1
		u16temp <<= 8;
		u16temp |= (uint16_t)inp(RDC_CFG_DAT_PORT);
		
		if(u16temp != RDC_CHIP_ID)
		{
			console_color(RED, BLACK);
			Print(L"ERROR: Error Chip ID !! \n\n");
			console_color(LIGHTGRAY, BLACK);
			return 1;
		}
		
		if(ch == 0)
		{
			ldn = RDC_LDN_SMB0;
		}
		else
		{
			ldn = RDC_LDN_SMB1;
		}
		D_Print(L"SmBus %d \n", (uint16_t)ch);

		//Select Logic Device - Smbus
		outp(RDC_CFG_IDX_PORT, 0x07);
		outp(RDC_CFG_DAT_PORT, ldn);

		outp(RDC_CFG_IDX_PORT, 0x30);
		u8tmp = inp(RDC_CFG_DAT_PORT);
		if(u8tmp == 0)
		{
			D_Print(L"Smbus Logic Device Disable \n");
			continue;
		}
		
		//Get Smbus Base Address
		outp(RDC_CFG_IDX_PORT, 0x60);
		gSMB[ch].BaseAddr = inp(RDC_CFG_DAT_PORT);
		gSMB[ch].BaseAddr <<= 8;
		outp(RDC_CFG_IDX_PORT, 0x61);
		gSMB[ch].BaseAddr |= inp(RDC_CFG_DAT_PORT);
		if((gSMB[ch].BaseAddr == 0) || (gSMB[ch].BaseAddr == 0xFFFF))
		{
			D_Print(L"Smbus Logic Device Address Error(0x%X) \n", gSMB[ch].BaseAddr);
			continue;
		}

		//Reset Smbus
		outp(gSMB[ch].BaseAddr + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
		
		//set Smbus Receive Slave Address 0xFE, avoid scan self
		SET_REG_BM8(SMB_RSADD_SLAVEADDR_gm, gSMB[ch].BaseAddr + SMB_RSADD_REG_OFFSET);
		
		//---------------------------------------------------------------------
		// SMBus clock calculation
		//---------------------------------------------------------------------
		// CPU   clock :  50 MHz
		// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
		//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
		//---------------------------------------------------------------------
		prescale1 = 50;
		outp(gSMB[ch].BaseAddr + SMB_CLKCTL1_REG_OFFSET, prescale1);

		prescale2 = 9;
		prescale2 &= ~SMB_CLKCTL2_FAST_bm;
		outp(gSMB[ch].BaseAddr + SMB_CLKCTL2_REG_OFFSET, prescale2);

		gSMB[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
		gSMB[ch].Flag.init = 1;

		//Close PNP port
		outp(RDC_CFG_IDX_PORT, 0xAA);	// exit config
		sys_close_ioport(RDC_PNP_INDEX);
		sys_close_ioport(RDC_PNP_DATA);
	}

	if((gSMB[0].Flag.init == 0) && (gSMB[1].Flag.init == 0))
	{
		console_color(RED, BLACK);
		Print(L"Smbus Not Available !! \n\n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	
	return 0;
}

void SmbusSetClockFrequency(uint8_t ch, uint8_t clk)
{
	uint8_t prescale1, prescale2;

	switch(clk)
	{
		case 0:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 9;
			prescale2 &= ~SMB_CLKCTL2_FAST_bm;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL2_REG_OFFSET, prescale2);

			gSMB[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case 1:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 25 / 5 = 50000 kHz / 125 = 400 kHz
			//---------------------------------------------------------------------
			prescale1 = 25;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 4;
			prescale2 |= SMB_CLKCTL2_FAST_bm;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL2_REG_OFFSET, prescale2);

			gSMB[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case 2:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 1 = 50000 kHz / 50 = 1000 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 0;
			prescale2 |= SMB_CLKCTL2_FAST_bm;
			outp(gSMB[ch].BaseAddr + SMB_CLKCTL2_REG_OFFSET, prescale2);

			gSMB[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
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
	baddr = gSMB[ch].BaseAddr;
	
	//wait smbus in use
	retry = SMB_WAIT_RETRIES;
	while(TST_REG_BM8(SMB_STS_INUSE_bm, baddr + SMB_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			Print(L"Smbus In Use \n");
			err = SMB_ERR_WAIT_FREE_TMOUT;
			goto exit_trans;
		}
	}

	//check busy
	if(TST_REG_BM8(SMB_STS_HOSTBUSY_bm, baddr + SMB_STS_REG_OFFSET))
	{
		//kill transaction if busy
		SET_REG_BM8(SMB_CTL_KILL_bm, baddr + SMB_CTL_REG_OFFSET);
		CLR_REG_BM8(SMB_CTL_KILL_bm, baddr + SMB_CTL_REG_OFFSET);
	}

	//clean all data reg
	outp(baddr + SMB_DAT0_REG_OFFSET, SMB_DAT0_DATA0_gp);
	outp(baddr + SMB_DAT1_REG_OFFSET, SMB_DAT1_DATA1_gp);

	//set smb protocol
	value = inp(baddr + SMB_CTL_REG_OFFSET);
	SET_VAR_GM(SMB_CTL_SMBCMD_gm, (tmp->mode & SMB_CMD_MASK)<<SMB_CTL_SMBCMD_gp, value);	//cmd=mode
	outp(baddr + SMB_CTL_REG_OFFSET, value);

	//set tx slave address
	outp(baddr + SMB_SADD_REG_OFFSET, tmp->addr);

	//set tx command
	outp(baddr + SMB_CMD_REG_OFFSET, tmp->cmd);
	
	//set Packet Error Check Register
	outp(baddr + SMB_PEC_REG_OFFSET, tmp->pec);

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
				outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
				outp(baddr + SMB_DAT1_REG_OFFSET, *(tmp->wbuf+1));
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
			outp(baddr + SMB_DAT1_REG_OFFSET, *(tmp->wbuf+1));
			break;
		}
		case SMB_CMD_BLOCK:
		{
			//enable 32 byte buffer
			SET_REG_BM8(SMB_CTL2_E32B_bm, baddr + SMB_CTL2_REG_OFFSET);

			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_DAT0_REG_OFFSET, *(tmp->blen));
				for(i=0; i<*(tmp->blen); i++)
				{
					outp(baddr + SMB_BLK_REG_OFFSET, *(tmp->wbuf + i));
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
			//enable 32 byte buffer
			SET_REG_BM8(SMB_CTL2_E32B_bm, baddr + SMB_CTL2_REG_OFFSET);

			outp(baddr + SMB_DAT0_REG_OFFSET, tmp->wlen);
			for(i=0; i<tmp->wlen; i++)
			{
				outp(baddr + SMB_BLK_REG_OFFSET, *(tmp->wbuf + i));
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
	value = inp(baddr + SMB_CTL_REG_OFFSET);
	SET_VAR_BM(SMB_CTL_START_bm, value);	//start=1
	CLR_VAR_BM(SMB_CTL_I2CNACKEN_bm, value);	//nack=0
	CLR_VAR_BM(SMB_CTL_KILL_bm, value);		//kill=0
	
	if(tmp->mode & SMB_CMD_PEC_MASK)
	{
		SET_VAR_BM(SMB_CTL_PECEN_bm, value);	//PEC enable
	}
	else
	{
		CLR_VAR_BM(SMB_CTL_PECEN_bm, value);	// PEC disable
	}

	outp(baddr + SMB_CTL_REG_OFFSET, value);

	//wait tx done
	retry = SMB_WAIT_RETRIES;
	while(TST_REG_BM8(SMB_STS_HOSTBUSY_bm, baddr + SMB_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"Smbus Tx Timeout \n");
			err = SMB_ERR_TXDONE_TMOUT;
			goto exit_trans;
		}
	}

	//check status
	status = inp(baddr + SMB_STS_REG_OFFSET);
	if(status & SMB_STS_FAILED_bm)
	{
		D_Print(L"Smbus Transaction Fail \n");
		err = SMB_ERR_FAILED;
	}
	else if(status & SMB_STS_ARLERR_bm)
	{
		D_Print(L"Smbus Arbitration Lost \n");
		err = SMB_ERR_ARBI;
	}
	else
	{
		status = inp(baddr + SMB_STS2_REG_OFFSET);
		if(status & SMB_STS2_TOERR_bm)
		{
			D_Print(L"Smbus Time-Out Error \n");
			err = SMB_ERR_TMOUT;
		}
		else if(status & SMB_STS2_NACKERR_bm)
		{
			D_Print(L"Smbus Not Response ACK Error \n");
			err = SMB_ERR_NACK;
		}
		else if(status & SMB_STS2_PECERR_bm)
		{
			D_Print(L"Smbus PEC Error \n");
			err = SMB_ERR_PEC;
		}
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
				if((inp(baddr + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
				}
				else
				{
					D_Print(L"Smbus Rx Not Ready \n");
					err = SMB_ERR_FAILED;
					goto exit_trans;
				}
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				if((inp(baddr + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
					*(tmp->rbuf + 1) = inp(baddr + SMB_DAT1_REG_OFFSET);
				}
				else
				{
					D_Print(L"Smbus Rx Not Ready \n");
					err = SMB_ERR_FAILED;
					goto exit_trans;
				}
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
			*(tmp->rbuf + 1) = inp(baddr + SMB_DAT1_REG_OFFSET);
			break;
		}
		case SMB_CMD_BLOCK:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				if((inp(baddr + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*tmp->blen = inp(baddr + SMB_DAT0_REG_OFFSET);
					if(*tmp->blen > tmp->rlen)
					{
						err = SMB_ERR_BUF;
						goto exit_trans;
					}
					
					for(i=0; i<*tmp->blen; i++)
					{
						*(tmp->rbuf + i) = inp(baddr + SMB_BLK_REG_OFFSET);
					}
				}
				else
				{
					D_Print(L"Smbus Rx Not Ready \n");
					err = SMB_ERR_FAILED;
					goto exit_trans;
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
			*tmp->blen = inp(baddr + SMB_DAT0_REG_OFFSET);
			if(*tmp->blen > tmp->rlen)
			{
				err = SMB_ERR_BUF;
				goto exit_trans;
			}
			
			for(i=0; i<*tmp->blen; i++)
			{
				*(tmp->rbuf + i) = inp(baddr + SMB_BLK_REG_OFFSET);
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
	CLR_REG_BM8(SMB_CTL2_E32B_bm, baddr + SMB_CTL2_REG_OFFSET);
	//clear all status
	outp(baddr + SMB_STS_REG_OFFSET, 0xFF);
	outp(baddr + SMB_STS2_REG_OFFSET, 0xFF);

	return err;
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

void ListSmbusDeviceAddress(void)
{
	uint8_t ch, status, count=0;
	uint8_t rdata=0;
	uint16_t i;
	
	clrscr();
	Print(L"List SMBus Device Address \n\n");

	for(ch=0; ch<2; ch++)
	{
		if(gSMB[ch].Flag.init == 0)
		{
			Print(L"Smbus %d Can Not Use \n\n", (uint16_t)ch);
			continue;
		}
		
		Print(L"Scan Smbus %d... \n\n", (uint16_t)ch);
		for(i=0x10; i<0xF0; i+=2)
		{
			status = SmbusReadByte(ch, 0x01, (uint8_t)i, &rdata);
			if(status == SMB_OK)
			{
				Print(L"Device %d Address : 0x%X \n", count, i);
				gSMB[ch].DevAddr[count] = (uint8_t)i;
				count++;
			}
		}
		
		gSMB[ch].Cnt = count;
		if(count == 0)
		{
			Print(L"No Device \n\n");
		}
		Print(L"\n\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusReadEepromTest(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;
	
	clrscr();
	Print(L"SMBus - Read Eeprom Test. \n\n");
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;

	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto sret_end;
	}
	
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
	while(!CheckESC(GetKey())){}
}

void SmbusWriteEepromTest(uint8_t option)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, wbuf[2], rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;

	clrscr();
	if(option)
	{
		Print(L"SMBus - Write EEPROM Test With Auto Increase Data. \n\n");
	}
	else
	{
		Print(L"SMBus - Write Eeprom Test. \n\n");
	}
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;
	wbuf[0] = gSmbusConfig->eeprom->wsval;
	
	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto swet_end;
	}
	
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
	while(!CheckESC(GetKey())){}
}

void SmbusCheckEEPROMBoundary(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t status, wbuf[2], rbuf[2], bakbuf[2];
	uint8_t buf[2], count = 0;
	uint16_t i, tmp = 0;

	clrscr();
	Print(L"SMBus - Check Eeprom Boundary. \n\n");
	
	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;

	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto sceb_end;
	}
	
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
	while(!CheckESC(GetKey())){}
}

void SmbusEEPROMTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test SMBus - EERPOM\n\n");
		Print(L"0. EERPOM Config Page. \n");
		Print(L"1. Test SMBus - Read EERPOM. \n");
		Print(L"2. Test SMBus - Write EERPOM. \n");
		Print(L"3. Test SMBus - Auto Write EEPROM. \n");
		Print(L"4. Test SMBus - Check EEPROM Boundary. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusEepromConfiguration();
				break;
			}
			case '1':
			{
				SmbusReadEepromTest();
				break;
			}
			case '2':
			{
				SmbusWriteEepromTest(0);
				break;
			}
			case '3':
			{
				SmbusWriteEepromTest(1);
				break;
			}
			case '4':
			{
				SmbusCheckEEPROMBoundary();
				break;
			}
		}
	}
}

void SmbusReadThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"SMBus - Read Thermal IC Test. \n\n");

	thermal_ch = gSmbusConfig->thermal->ch;
	SmbusSetClockFrequency(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr = gSmbusConfig->thermal->addr;

	if(gSMB[thermal_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto srt_end;
	}
	
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
	while(!CheckESC(GetKey())){}
}

void SmbusWriteThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"SMBus - Write Thermal IC Test. \n\n");

	thermal_ch = gSmbusConfig->thermal->ch;
	SmbusSetClockFrequency(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr =  gSmbusConfig->thermal->addr;

	if(gSMB[thermal_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto swt_end;
	}
	
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
	while(!CheckESC(GetKey())){}
}

void SmbusThermalICTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test SMBus - Thermal\n\n");
		Print(L"0. Thermal Config Page. \n");
		Print(L"1. Test SMBus - Read Thermal IC. \n");
		Print(L"2. Test SMBus - Write Thermal IC. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusThermalConfiguration();
				break;
			}
			case '1':
			{
				SmbusReadThermalICTest();
				break;
			}
			case '2':
			{
				SmbusWriteThermalICTest();
				break;
			}
		}
	}
}

void SmbusEatBoardTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test SMBus - EAT Board\n\n");
		Print(L"1. Test SMBus - EERPOM. \n");
		Print(L"2. Test SMBus - Thermal. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '1':
			{
				SmbusEEPROMTest();
				break;
			}
			case '2':
			{
				SmbusThermalICTest();
				break;
			}
		}
	}
}

void SmbusReadDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;

	clrscr();
	Print(L"SMBus - Read DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto srdbt_end;
	}
	
	// set all pins of port0 as input
	u8wb = 0xFF;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8wb))
	{
		Print(L"Set Port 0 As Input Fail \n");
		goto srdbt_end;
	}
	// set all pins of port1 as input
	u8wb = 0xFF;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8wb))
	{
		Print(L"Set Port 1 As Input Fail \n");
		goto srdbt_end;
	}
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Short I/O Pin To Gnd For Test");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto srdbt_endend;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto srdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto srdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto srdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto srdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto srdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto srdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto srdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto srdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto srdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto srdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

srdbt_end:
	while(!CheckESC(GetKey())){}

srdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusWriteDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t bit = 0xFF;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Write DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto swdbt_end;
	}
	
	// set all pins of port0 as output
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto swdbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto swdbt_end;
	}
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");

	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Key    : ");
	for(i=1; i<=8; i++)
	{
		Print(L"%2d ", i);
	}
	for(i=1; i<=8; i++)
	{
		Print(L"F%d ", i);
	}
	Print(L"\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Press Key To Set Pin Level");
	
	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto swdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto swdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto swdbt_endend;
		}
		
		switch(key)
		{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			{
				bit = (uint8_t)(key - 0x31);
				break;
			}
			case (SCAN_F1 << 8):
			case (SCAN_F2 << 8):
			case (SCAN_F3 << 8):
			case (SCAN_F4 << 8):
			case (SCAN_F5 << 8):
			case (SCAN_F6 << 8):
			case (SCAN_F7 << 8):
			case (SCAN_F8 << 8):
			{
				bit = (uint8_t)((key >> 8) - 3);
				break;
			}
			default:
			{
				break;
			}
		}

		if(bit != 0xFF)
		{
			level = (level & ~(1 << bit)) | (~level & (1 << bit));
			bit = 0xFF;
		}
		
		// set new level to port0
		u8wb = level & 0xFF;
		if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8wb))
		{
			Print(L"Write New Level To Port 0 Fail \n");
			goto swdbt_end;
		}
		// set new level to port1
		u8wb = (level >> 8) & 0xFF;
		if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8wb))
		{
			Print(L"Write New Level To Port 1 Fail \n");
			goto swdbt_end;
		}

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto swdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto swdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto swdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto swdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto swdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto swdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto swdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto swdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

swdbt_end:
	while(!CheckESC(GetKey())){}

swdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusAutoDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint8_t bit = 0;
	uint8_t cnt = 0;
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Auto R/W DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto sadbt_end;
	}
	
	// set all pins of port0 as output
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto sadbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto sadbt_end;
	}

	// set port0 to low
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8wb))
	{
		Print(L"Write Port 0 To Low Fail \n");
		goto sadbt_end;
	}
	// set port1 to low
	u8wb = 0;
	if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8wb))
	{
		Print(L"Write Port 1 To Low Fail \n");
		goto sadbt_end;
	}

	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");
	
	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto sadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto sadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto sadbt_endend;
		}
		
		usleep(100 * 1000);
		cnt++;

		if(cnt % 2)
		{
			if(bit < 16)
			{
				level |= (1 << bit);
			}
			else if((bit >= 16) && (bit <32))
			{
				level &= ~(1 << (bit % 16));
			}
			else if((bit >= 32) && (bit <48))
			{
				level |= (1 << (15 - (bit % 16)));
			}
			else if((bit >= 48) && (bit <64))
			{
				level &= ~(1 << (15 - (bit % 16)));
			}
			else if((bit >= 64) && (bit <80))
			{
				level = (1 << (bit % 16));
			}
			else if((bit >= 80) && (bit <96))
			{
				level = (1 << (15 - (bit % 16)));
			}
			else
			{
				bit = 0;
			}
			
			// set new level to port0
			u8wb = level & 0xFF;
			if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8wb))
			{
				Print(L"Write New Level To Port 0 Fail \n");
				goto sadbt_end;
			}
			// set new level to port1
			u8wb = (level >> 8) & 0xFF;
			if(SmbusWriteByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8wb))
			{
				Print(L"Write New Level To Port 1 Fail \n");
				goto sadbt_end;
			}

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto sadbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_IN_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto sadbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto sadbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_OUT_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto sadbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT1, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto sadbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_INV_PORT0, dio_addr, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto sadbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG1, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto sadbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(SmbusReadByte(dio_ch, DIO_REG_CONFIG0, dio_addr, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto sadbt_end;
		}
		Print(L"%02X \n", u8rb);

			bit++;
		}
	}

sadbt_end:
	while(!CheckESC(GetKey())){}

sadbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusDIOBoardTest(void)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test SMBus - DIO Board\n\n");
		Print(L"0. DIO Board Config Page. \n");
		Print(L"1. Test SMBus - Read DIO Board\n");
		Print(L"2. Test SMBus - Write DIO Board\n");
		Print(L"3. Test SMBus - Auto R/W DIO Board\n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusDIOBoardConfiguration();
				break;
			}
			case '1':
			{
				SmbusReadDIOBoardTest();
				break;
			}
			case '2':
			{
				SmbusWriteDIOBoardTest();
				break;
			}
			case '3':
			{
				SmbusAutoDIOBoardTest();
				break;
			}
		}
	}
}

void SmbusReadSmartBatteryTest(void)
{
	uint8_t battery_ch = 0, battery_addr = 0x16, battery_max_cmd = 35;
	uint8_t j = 0, max_ddata = 32, offset = 0, max_block_data = 32;
	uint8_t *block_data;
	uint8_t *ddata;
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
	
	battery_ch = gSmbusConfig->battery->ch;
	SmbusSetClockFrequency(gSmbusConfig->battery->ch, gSmbusConfig->battery->freq);
	battery_addr = gSmbusConfig->battery->addr;
	
	if(gSMB[battery_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)battery_ch);
		goto read_error;
	}
	
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
			AsciiToUnicodeString(block_data, str16);
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
		if(gSMB[battery_ch].Flag.init)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Smart Battery Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
		}
	}
	
malloc_error:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusSmartBatteryTest(void)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test SMBus - Smart Battery\n\n");
		Print(L"0. Smart Battery Config Page. \n");
		Print(L"1. Test SMBus - Read Smart Battery. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusSmartBatteryConfiguration();
				break;
			}
			case '1':
			{
				SmbusReadSmartBatteryTest();
				break;
			}
		}
	}
	
}

void SmbusTest(void)
{
	int key;

	clrscr();
	if(InitSmbus())
	{
		goto st_end;
	}
	usleep(100);
	
	if(gSmbusConfig == NULL)
	{
		gSmbusConfig = (SmbusVariableConfig*)malloc(sizeof(SmbusVariableConfig));
		if(!gSmbusConfig)
		{
			Print(L"Smbus Config Data Fail !! \n\n");
			goto st_end;
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
		
		gSmbusConfig->dio= (DIO*)malloc(sizeof(DIO));
		if(gSmbusConfig->dio)
		{
			memset(gSmbusConfig->dio, 0, sizeof(DIO));
		}
		
		gSmbusConfig->battery= (BATTERY*)malloc(sizeof(BATTERY));
		if(gSmbusConfig->battery)
		{
			memset(gSmbusConfig->battery, 0, sizeof(BATTERY));
		}

		SmbusTestConfigDefault();
	}
	
	while(1)
	{
		clrscr();
		Print(L"1. List SMBus Device Address. \n");
		Print(L"2. Test SMBus - EAT Board. \n");
		Print(L"3. Test SMBus - DIO Board. \n");
		Print(L"4. Test SMBus - Smart Battery. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			if(gSMB[0].Flag.init == 1)
			{
				//Reset Smbus to reset RDC Smbus slave address
				outp(gSMB[0].BaseAddr + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
			}
			if(gSMB[1].Flag.init == 1)
			{
				//Reset Smbus to reset RDC Smbus slave address
				outp(gSMB[1].BaseAddr + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
			}
			goto st_endend;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ListSmbusDeviceAddress();
				break;
			}
			case '2':
			{
				SmbusEatBoardTest();
				break;
			}
			case '3':
			{
				SmbusDIOBoardTest();
				break;
			}
			case '4':
			{
				SmbusSmartBatteryTest();
				break;
			}
		}
	}

st_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

st_endend:
	NULL;
}

uint8_t SmbusUseI2CModeWaitTxDone(uint16_t addr, uint8_t p)
{
	uint8_t status, status2;
	uint16_t retry = SMB_WAIT_RETRIES;

	while(retry)
	{
		usleep(1);
		status = inp(addr + SMB_STS_REG_OFFSET);
		status2 = inp(addr + SMB_STS2_REG_OFFSET);

		if(status & SMB_STS_ARLERR_bm)
		{
			if(p)
			{
				Print(L"Smbus Arbitration Lost \n");
			}
			return 1;
		}
		else if(status2 & SMB_STS2_NACKERR_bm)
		{
			if(p)
			{
				Print(L"Smbus Not Response ACK Error \n");
			}
			return 1;
		}
		else if(status & SMB_STS_TXDONE_bm)
		{
			break;
		}
		
		retry--;
		if(retry == 0)
		{
			if(p)
			{
				Print(L"Smbus Tx Timeout \n");
			}
			return 1;
		}
	}

	//clean tx done status register
	SET_REG_BM8(SMB_STS_TXDONE_bm, addr + SMB_STS_REG_OFFSET);

	return 0;
}

uint8_t SmbusUseI2CModeWaitRxDone(uint16_t addr)
{
	uint16_t retry = SMB_WAIT_RETRIES;

	while(!TST_REG_BM8(SMB_STS_RXRDY_bm, addr + SMB_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"Smbus Rx Timeout \n");
			return 1;
		}
		usleep(1);
	}

	//clean rx done status register
	SET_REG_BM8(SMB_STS_RXRDY_bm, addr + SMB_STS_REG_OFFSET);

	return 0;
}

uint8_t SmbusUseI2CModeWaitStopDone(uint16_t addr)
{
	uint16_t retry = SMB_WAIT_RETRIES;
	
	while(TST_REG_BM8(SMB_CTL_LASTBYTE_bm, addr + SMB_CTL_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"Smbus Stop Timeout \n");
			return 1;
		}
		usleep(1);
	}

	return 0;
}

void SmbusUseI2CModeListDeviceAddress(void)
{
	uint8_t ch, count;
	uint16_t i;
	
	clrscr();
	Print(L"List I2C Device Address \n\n");

	for(ch=0; ch<2; ch++)
	{
		if(gSMB[ch].Flag.init == 0)
		{
			Print(L"Smbus %d Can Not Use \n\n", (uint16_t)ch);
			continue;
		}

		//set Smbus mapping to I2C enable
		SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[ch].BaseAddr + SMB_CTL2_REG_OFFSET);

		count=0;
		Print(L"Scan Smbus %d... \n\n", (uint16_t)ch);
		for(i=0x10; i<0xF0; i+=2)
		{
			//clean status register
			SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[ch].BaseAddr + SMB_STS_REG_OFFSET);
			SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[ch].BaseAddr + SMB_STS2_REG_OFFSET);

			//send addr
			outp(gSMB[ch].BaseAddr + SMB_SADD_REG_OFFSET, (uint8_t)i);
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
			{
				//set stop flag
				SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);
				SmbusUseI2CModeWaitStopDone(gSMB[ch].BaseAddr);

				continue;
			}

			//set stop flag
			SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);
			//wait stop
			if(SmbusUseI2CModeWaitStopDone(gSMB[ch].BaseAddr) != 0)
			{
				goto smtilda_end;
			}

			//communication success
			Print(L"Device %d Address : 0x%X \n", count, i);
			gSMB[ch].DevAddr[count] = (uint8_t)i;
			count++;
		}

		gSMB[ch].Cnt = count;
		if(count == 0)
		{
			Print(L"No Device \n\n");
		}
		Print(L"\n\n");
	}

smtilda_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeReadEepromTest(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;
	
	clrscr();
	Print(L"Smbus I2C Mode - Read EERPOM Test. \n\n");

	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;
	
	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto smtiret_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	//start to read
	for(i=0; i<len; i++)
	{
		//clean status register
		SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
		SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);
		
		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto smtiret_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto smtiret_end;
			}
		}

		//send command
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiret_end;
		}

		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			Print(L"Send Address Error!! \n\n");
			goto smtiret_end;
		}

		//dummy read to trigger read process
		inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		//set stop flag
		SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);

		//wait rx data ready
		if(SmbusUseI2CModeWaitRxDone(gSMB[eeprom_ch].BaseAddr) != 0)
		{
			goto smtiret_end;
		}

		//get data
		rbuf[0] = inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

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

smtiret_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeWriteEepromTest(uint8_t option)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;

	clrscr();
	Print(L"Smbus I2C Mode - Write EERPOM Test. \n\n");

	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	eeprom_addr = gSmbusConfig->eeprom->addr;
	eeprom_saddr = gSmbusConfig->eeprom->spos;
	len = gSmbusConfig->eeprom->rwlen;
	wbuf[0] = gSmbusConfig->eeprom->wsval;
	
	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto smtiwet_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	for(i=0; i<len; i++)
	{
		//clean status register
		SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
		SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);

		//write data to eeprom
		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto smtiwet_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto smtiwet_end;
			}
		}

		//send command
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiwet_end;
		}

		//send data
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, wbuf[0]);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiwet_end;
		}

		//set stop flag
		SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
		//wait stop
		if(SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr) != 0)
		{
			goto smtiwet_end;
		}


		usleep(5000);	//must set more than 2ms


		//read data
		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiwet_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto smtiwet_end;
			}
		}

		//send command
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiwet_end;
		}

		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smtiwet_end;
		}

		//dummy read to trigger read process
		inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		//set stop flag
		SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);

		//wait rx data ready
		if(SmbusUseI2CModeWaitRxDone(gSMB[eeprom_ch].BaseAddr) != 0)
		{
			goto smtiwet_end;
		}

		//get data
		rbuf[0] = inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

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
			goto smtiwet_end;
		}

		//option is write auto increase
		if(option)
		{
			wbuf[0]++;
		}
	}
	//read/write compare complete and success
	console_color(GREEN, BLACK);
	Print(L"EEPROM R/W Compare OK!! \n");

smtiwet_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeCheckEEPROMBoundary(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t buf[2], bakbuf[2], count = 0;
	uint16_t i, tmp = 0;

	clrscr();
	Print(L"Smbus I2C Mode - Check EEPROM Boundary. \n\n");

	eeprom_ch = gSmbusConfig->eeprom->ch;
	SmbusSetClockFrequency(gSmbusConfig->eeprom->ch, gSmbusConfig->eeprom->freq);
	eeprom_type = gSmbusConfig->eeprom->type;
	
	if(gSMB[eeprom_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto smticeb_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	//if type is byte mode, search 0xAx total size
	if(eeprom_type == 0)
	{
		for(i=0xA0; i<0xAF; i+=2)
		{
			//clean status register
			SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
			SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);
			
			//send addr
			outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, (uint8_t)i);
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 0) != 0)
			{
				//set stop flag
				SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
				SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr);
				
				continue;
			}
			
			//set stop flag
			SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
			//wait stop
			if(SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr) != 0)
			{
				goto smticeb_end;
			}

			//communication success
			buf[0] = (uint8_t)i;
			count++;
		}

		if(count == 1)
		{
			eeprom_addr = buf[0];
		}
		else
		{
			Print(L"This EEPROM total is %d k bits", count * 2);
			goto smticeb_end;
		}
	}

	if(eeprom_type)
	{
		eeprom_addr = gSmbusConfig->eeprom->addr;
	}

	//clean status register
	SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
	SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);

	//backup eeprom last byte
	//send addr
	outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	//if word mode, need to send two command
	//send command 1
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}
	}

	//send addr
	outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr | 0x01);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	//dummy read to trigger read process
	inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);

	//wait rx data ready
	if(SmbusUseI2CModeWaitRxDone(gSMB[eeprom_ch].BaseAddr) != 0)
	{
		goto smticeb_end;
	}

	//get data
	bakbuf[0] = inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

	usleep(5000);	//must set more than 2ms

	//clean status register
	SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
	SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);
	
	//write data to eeprom
	//send addr
	outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto smticeb_end;
	}

	//send command 1
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}
	}

	//send data
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0x55);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
	//wait stop
	if(SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr) != 0)
	{
		goto smticeb_end;
	}

	usleep(5000);	//must set more than 2ms

	for(i=1; i<8; i++)
	{
		//read data
		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		//if word mode, need to send two command
		//send command 1
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, (0xFF >> i));
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		if(eeprom_type)
		{
			//send command 2
			outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto smticeb_end;
			}
		}

		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		//dummy read to trigger read process
		inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		//set stop flag
		SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);

		//wait rx data ready
		if(SmbusUseI2CModeWaitRxDone(gSMB[eeprom_ch].BaseAddr) != 0)
		{
			goto smticeb_end;
		}

		//get data
		buf[0] = inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		if(buf[0] != 0x55)
		{
			tmp = i - 1;
			break;
		}
	}

	usleep(5000);	//must set more than 2ms

	//clean status register
	SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS_REG_OFFSET);
	SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[eeprom_ch].BaseAddr + SMB_STS2_REG_OFFSET);

	//write data to eeprom
	//send addr
	outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto smticeb_end;
	}

	//send command 1
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}
	}

	//send data
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xAA);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
	//wait stop
	if(SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr) != 0)
	{
		goto smticeb_end;
	}

	usleep(5000);	//must set more than 2ms

	for(i=tmp; i>=0; i--)
	{
		//read data
		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		//if word mode, need to send two command
		//send command 1
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, (0xFF >> i));
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		if(eeprom_type)
		{
			//send command 2
			outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
			//wait tx done
			if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto smticeb_end;
			}
		}

		//send addr
		outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}

		//dummy read to trigger read process
		inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		//set stop flag
		SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);

		//wait rx data ready
		if(SmbusUseI2CModeWaitRxDone(gSMB[eeprom_ch].BaseAddr) != 0)
		{
			goto smticeb_end;
		}

		//get data
		buf[0] = inp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET);

		if(buf[0] == 0xAA)
		{
			tmp = (0xFF >> i) + 1;
			if(eeprom_type)
			{
				Print(L"This EEPROM is %d k bits", tmp * 2);
			}
			else
			{
				Print(L"This EEPROM is %d k bits", tmp * 8 /1024);
			}
			break;
		}

		if(i == 0)
		{
			Print(L"Check EEPROM Boundary Fail !! Please Check EEPROM Type !! \n\n");
			goto smticeb_end;
		}
	}

	usleep(5000);	//must set more than 2ms
	
	//restore eeprom last byte
	//send addr
	outp(gSMB[eeprom_ch].BaseAddr + SMB_SADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto smticeb_end;
	}

	//send command 1
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, 0xFF);
		//wait tx done
		if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto smticeb_end;
		}
	}

	//send data
	outp(gSMB[eeprom_ch].BaseAddr + SMB_DAT0_REG_OFFSET, bakbuf[0]);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto smticeb_end;
	}

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[eeprom_ch].BaseAddr + SMB_CTL_REG_OFFSET);
	//wait stop
	if(SmbusUseI2CModeWaitStopDone(gSMB[eeprom_ch].BaseAddr) != 0)
	{
		goto smticeb_end;
	}

smticeb_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeEEPROMTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test Smbus I2C Mode - EERPOM\n\n");
		Print(L"0. EERPOM Config Page. \n");
		Print(L"1. Test Smbus I2C Mode - Read EERPOM. \n");
		Print(L"2. Test Smbus I2C Mode - Write EERPOM. \n");
		Print(L"3. Test Smbus I2C Mode - Auto Write EEPROM. \n");
		Print(L"4. Test Smbus I2C Mode - Check EEPROM Boundary. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusEepromConfiguration();
				break;
			}
			case '1':
			{
				SmbusUseI2CModeReadEepromTest();
				break;
			}
			case '2':
			{
				SmbusUseI2CModeWriteEepromTest(0);
				break;
			}
			case '3':
			{
				SmbusUseI2CModeWriteEepromTest(1);
				break;
			}
			case '4':
			{
				SmbusUseI2CModeCheckEEPROMBoundary();
				break;
			}
		}
	}
}

uint8_t SmbusUseI2CModeReadRegister(uint8_t ch, uint8_t addr, uint8_t reg, uint8_t *rbuf)
{
	//clean status register
	SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[ch].BaseAddr + SMB_STS_REG_OFFSET);
	SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[ch].BaseAddr + SMB_STS2_REG_OFFSET);
	
	//send addr
	outp(gSMB[ch].BaseAddr + SMB_SADD_REG_OFFSET, addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtirr_end;
	}

	//send command
	outp(gSMB[ch].BaseAddr + SMB_DAT0_REG_OFFSET, reg);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtirr_end;
	}

	//send addr
	outp(gSMB[ch].BaseAddr + SMB_SADD_REG_OFFSET, addr | 0x01);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtirr_end;
	}

	//dummy read to trigger read process
	inp(gSMB[ch].BaseAddr + SMB_DAT0_REG_OFFSET);

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);

	//wait rx data ready
	if(SmbusUseI2CModeWaitRxDone(gSMB[ch].BaseAddr) != 0)
	{
		goto smtirr_end;
	}

	//get data
	*rbuf = inp(gSMB[ch].BaseAddr + SMB_DAT0_REG_OFFSET);

	return 0;

smtirr_end:
	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);

	return 1;
}

uint8_t SmbusUseI2CModeWriteRegister(uint8_t ch, uint8_t addr, uint8_t reg, uint8_t * wbuf)
{
	//clean status register
	SET_REG_BM8(SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm, gSMB[ch].BaseAddr + SMB_STS_REG_OFFSET);
	SET_REG_BM8(SMB_STS2_NACKERR_bm, gSMB[ch].BaseAddr + SMB_STS2_REG_OFFSET);
	
	//send addr
	outp(gSMB[ch].BaseAddr + SMB_SADD_REG_OFFSET, addr);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtiwr_end;
	}

	//send command
	outp(gSMB[ch].BaseAddr + SMB_DAT0_REG_OFFSET, reg);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtiwr_end;
	}

	//send data
	outp(gSMB[ch].BaseAddr + SMB_DAT0_REG_OFFSET, *wbuf);
	//wait tx done
	if(SmbusUseI2CModeWaitTxDone(gSMB[ch].BaseAddr, 0) != 0)
	{
		goto smtiwr_end;
	}

	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);
	//wait stop
	if(SmbusUseI2CModeWaitStopDone(gSMB[ch].BaseAddr) != 0)
	{
		return 1;
	}

	return 0;

smtiwr_end:
	//set stop flag
	SET_REG_BM8(SMB_CTL_LASTBYTE_bm, gSMB[ch].BaseAddr + SMB_CTL_REG_OFFSET);

	return 1;
}

void SmbusUseI2CModeReadThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"Smbus I2C Mode - Read Thermal IC Test. \n\n");

	thermal_ch = gSmbusConfig->thermal->ch;
	SmbusSetClockFrequency(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr = gSmbusConfig->thermal->addr;

	if(gSMB[thermal_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto smtirtit_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[thermal_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	//get chip Manufacturer ID
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_MFU_ID, rbuf))
	{
		Print(L"Read Manufacturer ID Fail \n");
		goto smtirtit_end;
	}
	tmp = rbuf[0] << 8;

	//get chip Revision ID
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_CHIP_REV, rbuf))
	{
		Print(L"Read Revision ID Fail \n");
		goto smtirtit_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//read Local Temp MSB
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_TEMP_H, rbuf))
	{
		Print(L"Read Local Temp MSB Fail \n");
		goto smtirtit_end;
	}
	tmp = rbuf[0];

	//read Local Temp LSB
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_TEMP_L, rbuf))
	{
		Print(L"Read Local Temp LSB Fail \n");
		goto smtirtit_end;
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
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OFFSET_H, rbuf))
	{
		Print(L"Read Remote Offset High Byte Fail \n");
		goto smtirtit_end;
	}
	tmp = rbuf[0];

	//read Remote Offset Low Byte
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OFFSET_L, rbuf))
	{
		Print(L"Read Remote Offset Low Byte Fail \n");
		goto smtirtit_end;
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
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
		goto smtirtit_end;
	}
	Print(L"Local T_Crit Limit:  %3d C \n", (uint16_t)rbuf[0]);

	//read Remote OS Limit
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OS_LMT, rbuf))
	{
		Print(L"Read Remote OS Limit Fail \n");
		goto smtirtit_end;
	}
	Print(L"Remote OS Limit:     %3d C \n", (uint16_t)rbuf[0]);

	//read Remote T_Crit Limit
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_CRIT_LMT, rbuf))
	{
		Print(L"Read Remote T_Crit Limit Fail \n");
		goto smtirtit_end;
	}
	Print(L"Remote T_Crit Limit: %3d C \n", (uint16_t)rbuf[0]);

	//read Common Hysteresis
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_COMM_HYSTERESIS, rbuf))
	{
		Print(L"Read Common Hysteresis Fail \n");
		goto smtirtit_end;
	}
	Print(L"Common Hysteresis:    %2d C \n", (uint16_t)rbuf[0]);

	//read Status Register 1
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_STATUS_1, rbuf))
	{
		Print(L"Read Status Register 1 Fail \n");
		goto smtirtit_end;
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
	
smtirtit_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeWriteThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"Smbus I2C Mode - Write Thermal IC Test. \n\n");

	thermal_ch = gSmbusConfig->thermal->ch;
	SmbusSetClockFrequency(gSmbusConfig->thermal->ch, gSmbusConfig->thermal->freq);
	thermal_addr =  gSmbusConfig->thermal->addr;

	if(gSMB[thermal_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto smtiwtit_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[thermal_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	//get chip Manufacturer ID
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_MFU_ID, rbuf))
	{
		Print(L"Read Manufacturer ID Fail \n");
		goto smtiwtit_end;
	}
	tmp = rbuf[0] << 8;

	//get chip Revision ID
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_CHIP_REV, rbuf))
	{
		Print(L"Read Revision ID Fail \n");
		goto smtiwtit_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//enter new value
	wbuf[0] = gSmbusConfig->thermal->wsval;

	//write Local Shared OS and T_Crit Limit
	if(SmbusUseI2CModeWriteRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, wbuf))
	{
		Print(L"Write Local Shared OS and T_Crit Limit Fail \n");
		goto smtiwtit_end;
	}

	//read Local Shared OS and T_Crit Limit
	if(SmbusUseI2CModeReadRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
		goto smtiwtit_end;
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

smtiwtit_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void SmbusUseI2CModeThermalICTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test Smbus I2C Mode - Thermal\n\n");
		Print(L"0. Thermal Config Page. \n");
		Print(L"1. Test Smbus I2C Mode - Read Thermal IC. \n");
		Print(L"2. Test Smbus I2C Mode - Write Thermal IC. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusThermalConfiguration();
				break;
			}
			case '1':
			{
				SmbusUseI2CModeReadThermalICTest();
				break;
			}
			case '2':
			{
				SmbusUseI2CModeWriteThermalICTest();
				break;
			}
		}
	}
}

void SmbusUseI2CModeEatBoardTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"Test Smbus I2C Mode - EAT Board\n\n");
		Print(L"1. Test Smbus I2C Mode - EERPOM. \n");
		Print(L"2. Test Smbus I2C Mode - Thermal. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '1':
			{
				SmbusUseI2CModeEEPROMTest();
				break;
			}
			case '2':
			{
				SmbusUseI2CModeThermalICTest();
				break;
			}
		}
	}
}

void SmbusUseI2CModeReadDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;

	clrscr();
	Print(L"SMBus - Read DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto smtirdbt_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[dio_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	// set all pins of port0 as input
	u8wb = 0xFF;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Input Fail \n");
		goto smtirdbt_end;
	}
	// set all pins of port1 as input
	u8wb = 0xFF;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Input Fail \n");
		goto smtirdbt_end;
	}
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Short I/O Pin To Gnd For Test");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto smtirdbt_endend;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto smtirdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto smtirdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto smtirdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

smtirdbt_end:
	while(!CheckESC(GetKey())){}

smtirdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusUseI2CModeWriteDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t bit = 0xFF;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Write DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto smtiwdbt_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[dio_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	// set all pins of port0 as output
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto smtiwdbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto smtiwdbt_end;
	}
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");

	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Key    : ");
	for(i=1; i<=8; i++)
	{
		Print(L"%2d ", i);
	}
	for(i=1; i<=8; i++)
	{
		Print(L"F%d ", i);
	}
	Print(L"\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Press Key To Set Pin Level");
	
	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto smtiwdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto smtiwdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto smtiwdbt_endend;
		}
		
		switch(key)
		{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			{
				bit = (uint8_t)(key - 0x31);
				break;
			}
			case (SCAN_F1 << 8):
			case (SCAN_F2 << 8):
			case (SCAN_F3 << 8):
			case (SCAN_F4 << 8):
			case (SCAN_F5 << 8):
			case (SCAN_F6 << 8):
			case (SCAN_F7 << 8):
			case (SCAN_F8 << 8):
			{
				bit = (uint8_t)((key >> 8) - 3);
				break;
			}
			default:
			{
				break;
			}
		}

		if(bit != 0xFF)
		{
			level = (level & ~(1 << bit)) | (~level & (1 << bit));
			bit = 0xFF;
		}
		
		// set new level to port0
		u8wb = level & 0xFF;
		if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
		{
			Print(L"Write New Level To Port 0 Fail \n");
			goto smtiwdbt_end;
		}
		// set new level to port1
		u8wb = (level >> 8) & 0xFF;
		if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
		{
			Print(L"Write New Level To Port 1 Fail \n");
			goto smtiwdbt_end;
		}

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto smtiwdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

smtiwdbt_end:
	while(!CheckESC(GetKey())){}

smtiwdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusUseI2CModeAutoDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint8_t bit = 0;
	uint8_t cnt = 0;
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Auto R/W DIO Board Test\n");

	dio_ch = gSmbusConfig->dio->ch;
	SmbusSetClockFrequency(gSmbusConfig->dio->ch, gSmbusConfig->dio->freq);
	dio_addr = gSmbusConfig->dio->addr;

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gSMB[dio_ch].Flag.init == 0)
	{
		Print(L"Smbus %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto sadbt_end;
	}

	//set Smbus mapping to I2C enable
	SET_REG_BM8(SMB_CTL2_I2CEN_bm, gSMB[dio_ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	// set all pins of port0 as output
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto sadbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto sadbt_end;
	}

	// set port0 to low
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
	{
		Print(L"Write Port 0 To Low Fail \n");
		goto sadbt_end;
	}
	// set port1 to low
	u8wb = 0;
	if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
	{
		Print(L"Write Port 1 To Low Fail \n");
		goto sadbt_end;
	}

	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");
	
	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto sadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto sadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto sadbt_endend;
		}
		
		usleep(100 * 1000);
		cnt++;

		if(cnt % 2)
		{
			if(bit < 16)
			{
				level |= (1 << bit);
			}
			else if((bit >= 16) && (bit <32))
			{
				level &= ~(1 << (bit % 16));
			}
			else if((bit >= 32) && (bit <48))
			{
				level |= (1 << (15 - (bit % 16)));
			}
			else if((bit >= 48) && (bit <64))
			{
				level &= ~(1 << (15 - (bit % 16)));
			}
			else if((bit >= 64) && (bit <80))
			{
				level = (1 << (bit % 16));
			}
			else if((bit >= 80) && (bit <96))
			{
				level = (1 << (15 - (bit % 16)));
			}
			else
			{
				bit = 0;
			}
			
			// set new level to port0
			u8wb = level & 0xFF;
			if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
			{
				Print(L"Write New Level To Port 0 Fail \n");
				goto sadbt_end;
			}
			// set new level to port1
			u8wb = (level >> 8) & 0xFF;
			if(SmbusUseI2CModeWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
			{
				Print(L"Write New Level To Port 1 Fail \n");
				goto sadbt_end;
			}

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
			{
				Print(L"Read Input Port 1 Fail \n");
				goto sadbt_end;
			}
			Print(L"Input port registers    : 0x%02X", u8rb);
			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
			{
				Print(L"Read Input Port 0 Fail \n");
				goto sadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
			{
				Print(L"Read Output Port 1 Fail \n");
				goto sadbt_end;
			}
			Print(L"Output port registers   : 0x%02X", u8rb);
			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
			{
				Print(L"Read Output Port 0 Fail \n");
				goto sadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto sadbt_end;
			}
			Print(L"Polarity registers      : 0x%02X", u8rb);
			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto sadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto sadbt_end;
			}
			Print(L"Configuration registers : 0x%02X", u8rb);
			if(SmbusUseI2CModeReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto sadbt_end;
			}
			Print(L"%02X \n", u8rb);

			bit++;
		}
	}

sadbt_end:
	while(!CheckESC(GetKey())){}

sadbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void SmbusUseI2CModeDIOBoardTest(void)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test Smbus I2C Mode - DIO Board\n\n");
		Print(L"0. DIO Board Config Page. \n");
		Print(L"1. Test Smbus I2C Mode - Read DIO Board\n");
		Print(L"2. Test Smbus I2C Mode - Write DIO Board\n");
		Print(L"3. Test Smbus I2C Mode - Auto R/W DIO Board\n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				SmbusDIOBoardConfiguration();
				break;
			}
			case '1':
			{
				SmbusUseI2CModeReadDIOBoardTest();
				break;
			}
			case '2':
			{
				SmbusUseI2CModeWriteDIOBoardTest();
				break;
			}
			case '3':
			{
				SmbusUseI2CModeAutoDIOBoardTest();
				break;
			}
		}
	}
}

void SmbusUseI2CModeTest(void)
{
	int key;

	clrscr();
	if(InitSmbus())
	{
		goto suimt_end;
	}
	usleep(100);
	
	if(gSmbusConfig == NULL)
	{
		gSmbusConfig = (SmbusVariableConfig*)malloc(sizeof(SmbusVariableConfig));
		if(!gSmbusConfig)
		{
			Print(L"Smbus Config Data Fail !! \n\n");
			goto suimt_end;
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
		
		gSmbusConfig->dio= (DIO*)malloc(sizeof(DIO));
		if(gSmbusConfig->dio)
		{
			memset(gSmbusConfig->dio, 0, sizeof(DIO));
		}
		
		gSmbusConfig->battery= (BATTERY*)malloc(sizeof(BATTERY));
		if(gSmbusConfig->battery)
		{
			memset(gSmbusConfig->battery, 0, sizeof(BATTERY));
		}

		SmbusTestConfigDefault();
	}
	
	while(1)
	{
		clrscr();
		Print(L"1. List I2C Device Address. \n");
		Print(L"2. Test Smbus I2C Mode - EAT Board. \n");
		Print(L"3. Test Smbus I2C Mode - DIO Board. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");
		
		key = GetKey();
		if(CheckESC(key))
		{
			if(gSMB[0].Flag.init == 1)
			{
				//Reset Smbus to reset RDC Smbus slave address
				outp(gSMB[0].BaseAddr + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
			}
			if(gSMB[1].Flag.init == 1)
			{
				//Reset Smbus to reset RDC Smbus slave address
				outp(gSMB[1].BaseAddr + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
			}
			goto suimt_endend;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				SmbusUseI2CModeListDeviceAddress();
				break;
			}
			case '2':
			{
				SmbusUseI2CModeEatBoardTest();
				break;
			}
			case '3':
			{
				SmbusUseI2CModeDIOBoardTest();
				break;
			}
		}
	}

suimt_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

suimt_endend:
	NULL;
}


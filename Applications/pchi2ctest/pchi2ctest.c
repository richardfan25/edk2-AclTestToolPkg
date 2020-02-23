#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "util.h"
#include "x86hw.h"
#include "global.h"
#include "pchi2ctest.h"



uint16_t *tool_name = L"PCH I2C Test";
uint16_t major_v = 0;
uint16_t minor_v = 4;

uint8_t gLM95245Address[LM_SMB_ADDR_NUM] = { LM_SMB_ADDR0, 
										LM_SMB_ADDR1, 
										LM_SMB_ADDR2, 
										LM_SMB_ADDR3, 
										LM_SMB_ADDR4};

UINT8 gPCA9555Address[8] = {0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E};

UINT32 gI2cSdpAddressList[] = {0xFEDC2000,   0xFEDC3000,   0xFEDC4000,   0xFEDC5000,   0xFEDC6000,   0xFEDCB000};


I2CVariableConfig *gI2CConfig = NULL;
uint16_t gVendor = 0;	//8086:Intel   1022:AMD



void I2CTestConfigDefault(void)
{
	gI2CConfig->eeprom->ch = 0;
	gI2CConfig->eeprom->freq = 0;
	gI2CConfig->eeprom->type= 0;
	gI2CConfig->eeprom->addr = 0xA0;
	gI2CConfig->eeprom->spos = 0x00;
	gI2CConfig->eeprom->rwlen = 256;
	gI2CConfig->eeprom->wsval = 0x00;
	gI2CConfig->eeprom->loop = 1;
	
	gI2CConfig->thermal->ch = 0;
	gI2CConfig->thermal->freq= 0;
	gI2CConfig->thermal->addr = 0x98;
	gI2CConfig->thermal->wsval= 85;
	
	gI2CConfig->dio->ch = 0;
	gI2CConfig->dio->freq= 0;
	gI2CConfig->dio->addr = 0x40;
	gI2CConfig->dio->wsval= 0x0000;
}

void ShowI2CChannel(I2C_BUS *i2c)
{
	uint8_t i;

	for(i=0; i<8; i++)
	{
		if(i2c->found & (1 << i))
		{
			Print(L"%d:I2C%d ", i, i);
		}
	}
	Print(L"\n");
}

void EepromConfiguration(I2C_BUS *i2c)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key;
	uint32_t value = 0;
	
	clrscr();
	Print(L"Eeprom Configuration Page \n\n");
	
	ShowI2CChannel(i2c);
	Print(L"Channel                  : %d\n", gI2CConfig->eeprom->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->eeprom->freq);
	Print(L"Type(0:Byte 1:Word)      : %d\n", gI2CConfig->eeprom->type);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->eeprom->addr);

	if(gI2CConfig->eeprom->type)
	{
		Print(L"Position(0x0000~0xFFFF)  : %X\n", gI2CConfig->eeprom->spos);
	}
	else
	{
		Print(L"Position(0x00~0xFF)      : %X\n", gI2CConfig->eeprom->spos);
	}
	Print(L"Length(0~256)            : %d\n", gI2CConfig->eeprom->rwlen);
	Print(L"Value(0x00~0xFF)         : %X\n", gI2CConfig->eeprom->wsval);
	Print(L"Loop(1~60000)            : %d\n", gI2CConfig->eeprom->loop);
	
	gotoxy(0, ConsoleHeight - 2);
	Print(L"Please press Backspace to delete value, then key in new value\n");
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
			if(item < 7)
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
				gotoxy(27 + len, item + 3);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					case 1:
					case 2:
					case 5:
					case 7:
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
			gotoxy(27 + len, item + 3);
			
			switch(item)
			{
				case 0:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(i2c->found & (1 << (key - '0')))
						{
							if(len != t_len)
							{
								len++;
								Print(L"%c", key);
								
								value = key - '0';
							}
						}
					}
					break;
				}
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
							if(value > 60000)
							{
								value = 60000;
							}
						}
					}
					break;
				}
				
				case 7:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(value > 60000)
							{
								value = 60000;
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
			gotoxy(27, item_bak + 3);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gI2CConfig->eeprom->ch = (uint8_t)value;
					Print(L"%d", gI2CConfig->eeprom->ch);
					break;
				}
				case 1:
				{
					gI2CConfig->eeprom->freq = (uint8_t)value;
					Print(L"%d", gI2CConfig->eeprom->freq);
					break;
				}
				case 2:
				{
					gI2CConfig->eeprom->type = (uint8_t)value;
					Print(L"%d", gI2CConfig->eeprom->type);
					
					gotoxy(0, 7);
					if(gI2CConfig->eeprom->type)
					{
						Print(L"Position(0x0000~0xFFFF)  : %X\n", gI2CConfig->eeprom->spos);
					}
					else
					{
						Print(L"Position(0x00~0xFF)      : %X\n", gI2CConfig->eeprom->spos);
					}
					break;
				}
				case 3:
				{
					gI2CConfig->eeprom->addr = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->eeprom->addr);
					break;
				}
				case 4:
				{
					gI2CConfig->eeprom->spos = (uint16_t)value;
					Print(L"%-4X", gI2CConfig->eeprom->spos);
					break;
				}
				case 5:
				{
					gI2CConfig->eeprom->rwlen = (uint16_t)value;
					Print(L"%-3d", gI2CConfig->eeprom->rwlen);
					break;
				}
				case 6:
				{
					gI2CConfig->eeprom->wsval = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->eeprom->wsval);
					break;
				}
				case 7:
				{
					if(value == 0)
					{
						value = 1;
					}
					gI2CConfig->eeprom->loop = (uint16_t)value;
					Print(L"%-5d", gI2CConfig->eeprom->loop);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 3);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->eeprom->ch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->eeprom->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->eeprom->type;
					Print(L"%d", value);
					break;
				}
				case 3:
				{
					sprintf(buf, "%X", gI2CConfig->eeprom->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gI2CConfig->eeprom->addr;
					Print(L"%-2X", value);
					break;
				}
				case 4:
				{
					sprintf(buf, "%X", gI2CConfig->eeprom->spos);
					len = (uint8_t)strlen(buf);
					
					if(gI2CConfig->eeprom->type)
					{
						t_len = 4;
					}
					else
					{
						t_len = 2;
					}
					
					value = gI2CConfig->eeprom->spos;
					Print(L"%-4X", value);
					break;
				}
				case 5:
				{
					if(gI2CConfig->eeprom->rwlen >= 100)
					{
						len = 3;
					}
					else if(gI2CConfig->eeprom->rwlen >= 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gI2CConfig->eeprom->rwlen;
					Print(L"%-3d", value);
					break;
				}
				case 6:
				{
					sprintf(buf, "%X", gI2CConfig->eeprom->wsval);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gI2CConfig->eeprom->wsval;
					Print(L"%-2X", value);
					break;
				}
				case 7:
				{
					if(gI2CConfig->eeprom->loop >= 10000)
					{
						len = 5;
					}
					else if(gI2CConfig->eeprom->loop >= 1000)
					{
						len = 4;
					}
					else if(gI2CConfig->eeprom->loop >= 100)
					{
						len = 3;
					}
					else if(gI2CConfig->eeprom->loop >= 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 5;
					value = gI2CConfig->eeprom->loop;
					Print(L"%-5d", value);
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
			gI2CConfig->eeprom->ch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gI2CConfig->eeprom->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gI2CConfig->eeprom->type = (uint8_t)value;
			break;
		}
		case 3:
		{
			gI2CConfig->eeprom->addr = (uint8_t)value;
			break;
		}
		case 4:
		{
			gI2CConfig->eeprom->spos = (uint16_t)value;
			break;
		}
		case 5:
		{
			gI2CConfig->eeprom->rwlen = (uint16_t)value;
			break;
		}
		case 6:
		{
			gI2CConfig->eeprom->wsval = (uint8_t)value;
			break;
		}
		case 7:
		{
			if(value == 0)
			{
				value = 1;
			}
			gI2CConfig->eeprom->loop = (uint16_t)value;
			break;
		}
	}
}

void ThermalICConfiguration(I2C_BUS *i2c)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Thermal IC Configuration Page \n\n");
	
	ShowI2CChannel(i2c);
	Print(L"Channel                  : %d\n", gI2CConfig->thermal->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->thermal->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->thermal->addr);
	Print(L"Value(0~127)             : %d\n", gI2CConfig->thermal->wsval);
	
	gotoxy(0, ConsoleHeight - 2);
	Print(L"Please press Backspace to delete value, then key in new value\n");
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
				gotoxy(27 + len, item + 3);
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
			gotoxy(27 + len, item + 3);
			
			switch(item)
			{
				case 0:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(i2c->found & (1 << (key - '0')))
						{
							if(len != t_len)
							{
								len++;
								Print(L"%c", key);
								
								value = key - '0';
							}
						}
					}
					break;
				}
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
			gotoxy(27, item_bak + 3);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gI2CConfig->thermal->ch = (uint8_t)value;
					Print(L"%d", gI2CConfig->thermal->ch);
					break;
				}
				case 1:
				{
					gI2CConfig->thermal->freq = (uint8_t)value;
					Print(L"%d", gI2CConfig->thermal->freq);
					break;
				}
				case 2:
				{
					gI2CConfig->thermal->addr = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->thermal->addr);
					break;
				}
				case 3:
				{
					gI2CConfig->thermal->wsval = (uint8_t)value;
					Print(L"%-3d", gI2CConfig->thermal->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 3);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->thermal->ch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->thermal->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%x", gI2CConfig->thermal->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gI2CConfig->thermal->addr;
					Print(L"%-2X", value);
					break;
				}
				case 3:
				{
					if(gI2CConfig->thermal->wsval >= 100)
					{
						len = 3;
					}
					else if(gI2CConfig->thermal->wsval >= 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gI2CConfig->thermal->wsval;
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
			gI2CConfig->thermal->ch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gI2CConfig->thermal->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gI2CConfig->thermal->addr = (uint8_t)value;
			break;
		}
		case 3:
		{
			gI2CConfig->thermal->wsval = (uint8_t)value;
			break;
		}
	}
}

void DIOBoardConfiguration(I2C_BUS *i2c)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"DIO Board Configuration Page \n\n");
	
	ShowI2CChannel(i2c);
	Print(L"Channel                  : %d\n", gI2CConfig->dio->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->dio->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->dio->addr);
	Print(L"Value(0x0000~0xFFFF)     : %X\n", gI2CConfig->dio->wsval);
	
	gotoxy(0, ConsoleHeight - 2);
	Print(L"Please press Backspace to delete value, then key in new value\n");
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
				gotoxy(27 + len, item + 3);
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
			gotoxy(27 + len, item + 3);
			
			switch(item)
			{
				case 0:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(i2c->found & (1 << (key - '0')))
						{
							if(len != t_len)
							{
								len++;
								Print(L"%c", key);
								
								value = key - '0';
							}
						}
					}
					break;
				}
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
			}
		}
		
		//select item
		if(item != item_bak)
		{
			gotoxy(27, item_bak + 3);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gI2CConfig->dio->ch = (uint8_t)value;
					Print(L"%d", gI2CConfig->dio->ch);
					break;
				}
				case 1:
				{
					gI2CConfig->dio->freq = (uint8_t)value;
					Print(L"%d", gI2CConfig->dio->freq);
					break;
				}
				case 2:
				{
					gI2CConfig->dio->addr = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->dio->addr);
					break;
				}
				case 3:
				{
					gI2CConfig->dio->wsval = value;
					Print(L"%-4X", gI2CConfig->dio->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 3);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->dio->ch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->dio->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%X", gI2CConfig->dio->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gI2CConfig->dio->addr;
					Print(L"%-2X", value);
					break;
				}
				case 3:
				{
					sprintf(buf, "%X", gI2CConfig->dio->wsval);
					len = (uint8_t)strlen(buf);
					
					t_len = 4;
					value = gI2CConfig->dio->wsval;
					Print(L"%-4X", value);
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
			gI2CConfig->dio->ch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gI2CConfig->dio->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gI2CConfig->dio->addr = (uint8_t)value;
			break;
		}
		case 3:
		{
			gI2CConfig->dio->wsval = value;
			break;
		}
	}
}

void DeviceConfiguration(I2C_BUS *i2c)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Device Configuration Page \n\n");
	
	ShowI2CChannel(i2c);
	Print(L"Channel                  : %d\n", gI2CConfig->dev->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->dev->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->dev->addr);
	Print(L"Command(0x00~0xFF)       : %X\n", gI2CConfig->dev->cmd);
	//Print(L"Length(0~256)            : %d\n", gI2CConfig->dev->rwlen);
	Print(L"Value(0x00~0xFF)         : %X\n", gI2CConfig->dev->wsval);
	
	gotoxy(0, ConsoleHeight - 2);
	Print(L"Please press Backspace to delete value, then key in new value\n");
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
			if(item < 4)
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
				gotoxy(27 + len, item + 3);
				Print(L" ");
				
				switch(item)
				{
					case 0:
					case 1:
					//case 4:
					{
						value = value / 10;
						break;
					}
					case 2:
					case 3:
					case 4:
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
			gotoxy(27 + len, item + 3);
			
			switch(item)
			{
				case 0:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(i2c->found & (1 << (key - '0')))
						{
							if(len != t_len)
							{
								len++;
								Print(L"%c", key);
								
								value = key - '0';
							}
						}
					}
					break;
				}
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
				case 3:
				case 4:
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
			gotoxy(27, item_bak + 3);
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gI2CConfig->dev->ch = (uint8_t)value;
					Print(L"%d", gI2CConfig->dev->ch);
					break;
				}
				case 1:
				{
					gI2CConfig->dev->freq = (uint8_t)value;
					Print(L"%d", gI2CConfig->dev->freq);
					break;
				}
				case 2:
				{
					gI2CConfig->dev->addr = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->dev->addr);
					break;
				}
				case 3:
				{
					gI2CConfig->dev->cmd = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->dev->cmd);
					break;
				}
				/*case 4:
				{
					gI2CConfig->dev->rwlen = value;
					Print(L"%-3d", gI2CConfig->dev->rwlen);
					break;
				}*/
				case 4:
				{
					gI2CConfig->dev->wsval= (uint8_t)value;
					Print(L"%-2X", gI2CConfig->dev->wsval);
					break;
				}
			}
			
			//set select item color to yellow
			gotoxy(27, item + 3);
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->dev->ch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gI2CConfig->dev->freq;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					sprintf(buf, "%X", gI2CConfig->dev->addr);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gI2CConfig->dev->addr;
					Print(L"%-2X", value);
					break;
				}
				case 3:
				{
					sprintf(buf, "%X", gI2CConfig->dev->cmd);
					len = (uint8_t)strlen(buf);
					
					t_len = 4;
					value = gI2CConfig->dev->cmd;
					Print(L"%-2X", value);
					break;
				}
				/*case 4:
				{
					if(gI2CConfig->dev->rwlen > 100)
					{
						len = 3;
					}
					else if(gI2CConfig->dev->rwlen > 10)
					{
						len = 2;
					}
					else
					{
						len = 1;
					}
					
					t_len = 3;
					value = gI2CConfig->dev->rwlen;
					Print(L"%-3d", value);
					break;
				}*/
				case 4:
				{
					sprintf(buf, "%X", gI2CConfig->dev->wsval);
					len = (uint8_t)strlen(buf);
					
					t_len = 4;
					value = gI2CConfig->dev->wsval;
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
			gI2CConfig->dev->ch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gI2CConfig->dev->freq = (uint8_t)value;
			break;
		}
		case 2:
		{
			gI2CConfig->dev->addr = (uint8_t)value;
			break;
		}
		case 3:
		{
			gI2CConfig->dev->cmd = (uint8_t)value;
			break;
		}
		/*case 4:
		{
			gI2CConfig->dev->rwlen = (uint8_t)value;
			break;
		}*/
		case 4:
		{
			gI2CConfig->dev->wsval = (uint8_t)value;
			break;
		}
	}
}

static UINT32 AMDI2CSclHcnt(UINT32 ic_clk, UINT32 tSYMBOL, UINT32 tf, UINT32 cond, UINT32 offset)
{
	if (cond)
		/*
		* Conditional expression: 
		*
		*   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		*
		* This is based on the DW manuals, and representing an
		* ideal configuration.  The resulting I2C bus speed will
		* be faster than any of the others.
		*
		* If your hardware is free from tHD;STA issue, try this one.
		*/
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	else
		/*
		* Conditional expression:
		*
		*   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		*
		* This is just experimental rule; the tHD;STA period turned
		* out to be proportinal to (_HCNT + 3).  With this setting,
		* we could meet both tHIGH and tHD;STA timing specs.
		*
		* If unsure, you'd better to take this alternative.
		*
		* The reason why we need to take into account "tf" here,
		* is the same as described in i2c_dw_scl_lcnt().
		*/
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
}

static UINT32 AMDI2CSclLcnt(UINT32 ic_clk, UINT32 tLOW, UINT32 tf, UINT32 offset)
{
	/*
	* Conditional expression:
	*
	*   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	*
	* DW I2C core starts counting the SCL CNTs for the LOW period
	* of the SCL clock (tLOW) as soon as it pulls the SCL line.
	* We need to take into account tf to meet the tLOW timing spec.
	* Default tf value should be 0.3 us, for safety.
	*/
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

/* Enable this I2C controller */
static void LpssI2CEnable(struct Lpss_I2C_Regs *baddr)
{
	uint32_t enable = read32(&baddr->enable);

	if(!(enable & ENABLE_CONTROLLER))
	{
		write32(&baddr->enable, enable | ENABLE_CONTROLLER);
	}
}

/* Disable this I2C controller */
static int LpssI2CDisable(struct Lpss_I2C_Regs *baddr)
{
	uint32_t enable = read32(&baddr->enable);
	uint32_t timeout = LPSS_I2C_TIMEOUT_US;

	if(enable & ENABLE_CONTROLLER)
	{
		write32(&baddr->enable, enable & ~ENABLE_CONTROLLER);

		/* Wait for enable bit to clear */
		while(read32(&baddr->enable_status) & ENABLE_CONTROLLER)
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				return -1;
			}
		}
	}

	return 0;
}

/* Wait for this I2C controller to go idle for transmit */
static int LpssI2CWaitForBusIdle(struct Lpss_I2C_Regs *baddr)
{
	uint32_t status;
	uint32_t timeout = LPSS_I2C_TIMEOUT_US;
	
	while(timeout)
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			/* Timed out while waiting for bus to go idle */
			return -1;
		}
		
		status = read32(&baddr->status);

		/* Check for master activity and keep waiting */
		if(status & STATUS_MASTER_ACTIVITY)
		{
			continue;
		}

		/* Check for TX FIFO empty to indicate TX idle */
		if(status & STATUS_TX_FIFO_EMPTY)
		{
			break;
		}
	}
	
	return 0;
}

static int LpssI2CSetSpeedConfig(struct Lpss_I2C_Regs *baddr, const struct Lpss_I2C_Speed_Config *config)
{
	void *hcnt_reg, *lcnt_reg;

	/* Nothing to do if no values are set */
	if (!config->scl_lcnt && !config->scl_hcnt && !config->sda_hold)
	{
		return 0;
	}

	if (config->speed >= I2C_SPEED_HIGH)
	{
		/* High and Fast Ultra speed */
		hcnt_reg = &baddr->hs_scl_hcnt;
		lcnt_reg = &baddr->hs_scl_lcnt;
	}
	else if (config->speed >= I2C_SPEED_FAST)
	{
		/* Fast and Fast-Plus speed */
		hcnt_reg = &baddr->fs_scl_hcnt;
		lcnt_reg = &baddr->fs_scl_lcnt;
	}
	else
	{
		/* Standard speed */
		hcnt_reg = &baddr->ss_scl_hcnt;
		lcnt_reg = &baddr->ss_scl_lcnt;
	}

	/* SCL count must be set after the speed is selected */
	if(config->scl_hcnt)
	{
		write32(hcnt_reg, config->scl_hcnt);
	}
	if(config->scl_lcnt)
	{
		write32(lcnt_reg, config->scl_lcnt);
	}

	/* Set SDA Hold Time register */
	if(config->sda_hold)
	{
		write32(&baddr->sda_hold, config->sda_hold);
	}

	return 0;
}

int LpssI2CGenSpeedConfig(struct Lpss_I2C_Regs *baddr, enum I2C_Speed speed, struct Lpss_I2C_Speed_Config *config)
{
	uint32_t ic_clk = CONFIG_SOC_INTEL_COMMON_LPSS_CLOCK_MHZ;
	uint32_t hcnt_min, lcnt_min;

	if(gVendor == 0x8086)	//Intel
	{
		if(speed >= I2C_SPEED_HIGH)
		{
			/* High speed */
			hcnt_min = MIN_HS_SCL_HIGHTIME;
			lcnt_min = MIN_HS_SCL_LOWTIME;
		}
		else if(speed >= I2C_SPEED_FAST_PLUS)
		{
			/* Fast-Plus speed */
			hcnt_min = MIN_FP_SCL_HIGHTIME;
			lcnt_min = MIN_FP_SCL_LOWTIME;
		}
		else if (speed >= I2C_SPEED_FAST)
		{
			/* Fast speed */
			hcnt_min = MIN_FS_SCL_HIGHTIME;
			lcnt_min = MIN_FS_SCL_LOWTIME;
		}
		else
		{
			/* Standard speed */
			hcnt_min = MIN_SS_SCL_HIGHTIME;
			lcnt_min = MIN_SS_SCL_LOWTIME;
		}
		
		config->speed = speed;
		config->scl_hcnt = (uint16_t)(((ic_clk*1000) * (hcnt_min + 171) + 500000) / MHz) - 3;//(uint16_t)(ic_clk * hcnt_min / KHz);
		config->scl_lcnt = (uint16_t)(((ic_clk*1000) * (lcnt_min + 208) + 500000) / MHz) - 1;//(uint16_t)(ic_clk * lcnt_min / KHz);
		config->sda_hold = ((ic_clk*1000) * 207 + 500000) / MHz;//0x000100CF;//0x00270014;//(ic_clk * DEFAULT_SDA_HOLD_TIME / KHz);
	}
	else if(gVendor == 0x1022)	//AMD
	{
		ic_clk = 150000;
		
		if(&baddr->control == (uint32_t*)0xFEDC6000)//V1000 I2C4
		{
			if(speed >= I2C_SPEED_HIGH)
			{
				/* High speed */
				hcnt_min = 0;
				lcnt_min = 0;
			}
			else if(speed >= I2C_SPEED_FAST_PLUS)
			{
				/* Fast-Plus speed */
				hcnt_min = 0;
				lcnt_min = 0;
			}
			else if (speed >= I2C_SPEED_FAST)
			{
				/* Fast speed */
				hcnt_min = 0x1A;
				lcnt_min = 0x28;
			}
			else
			{
				/* Standard speed */
				hcnt_min = 0x64;
				lcnt_min = 0xC8;
			}
		}
		else
		{
			if(speed >= I2C_SPEED_HIGH)
			{
				/* High speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										1,     /* tHD;STA = tHIGH = 0.06 us */
										0,      /* tf = 0.08 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										1,     /* tLOW = 0.16 us */
										0,      /* tf = 0.08 us */
										0);     /* No offset */
			}
			else if(speed >= I2C_SPEED_FAST_PLUS)
			{
				/* Fast-Plus speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										2,     /* tHD;STA = tHIGH = 0.26 us */
										1,      /* tf = 0.12 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										5,     /* tLOW = 0.5 us */
										1,      /* tf = 0.12 us */
										0);     /* No offset */
			}
			else if (speed >= I2C_SPEED_FAST)
			{
				/* Fast speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										6,     /* tHD;STA = tHIGH = 0.6 us */
										3,      /* tf = 0.3 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										13,     /* tLOW = 1.3 us */
										3,      /* tf = 0.3 us */
										0);     /* No offset */
			}
			else
			{
				/* Standard speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										40,     /* tHD;STA = tHIGH = 4.0 us */
										3,      /* tf = 0.3 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										47,     /* tLOW = 4.7 us */
										3,      /* tf = 0.3 us */
										0);     /* No offset */
			}
		}

		config->speed = speed;
		config->scl_hcnt = (uint16_t)hcnt_min;
		config->scl_lcnt = (uint16_t)lcnt_min;
		config->sda_hold = 1;
	}

	return 0;
}

static int LpssI2CSetSpeed(struct Lpss_I2C_Regs *baddr, enum I2C_Speed speed)
{
	struct Lpss_I2C_Speed_Config config;
	uint32_t control;

	control = read32(&baddr->control);
	control &= ~CONTROL_SPEED_MASK;

	if(speed >= I2C_SPEED_HIGH)
	{
		/* High and Fast-Ultra speed share config registers */
		control |= CONTROL_SPEED_HS;
	}
	else if(speed >= I2C_SPEED_FAST)
	{
		/* Fast speed and Fast-Plus */
		control |= CONTROL_SPEED_FS;
	}
	else
	{
		/* Standard speed */
		control |= CONTROL_SPEED_SS;
	}

	/* Generate speed config based on clock */
	if(LpssI2CGenSpeedConfig(baddr, speed, &config) < 0)
	{
		return -1;
	}

	/* Select this speed in the control register */
	write32(&baddr->control, control);

	/* Write the speed config that was generated earlier */
	LpssI2CSetSpeedConfig(baddr, &config);

	return 0;
}

int LpssI2CChoiceSpeed(struct Lpss_I2C_Regs *baddr, uint8_t clk)
{
	uint32_t speed = 0;
	
	switch(clk)
	{
		case 0:
		{
			speed = I2C_SPEED_STANDARD;
			break;
		}
		case 1:
		{
			speed = I2C_SPEED_FAST;
			break;
		}
		case 2:
		{
			speed = I2C_SPEED_FAST_PLUS;
			break;
		}
	}
	if(LpssI2CSetSpeed(baddr, speed) < 0)
	{
		Print(L"I2C failed to set speed for bus \n");
		return -1;
	}

	return 0;
}

int LpssI2CTargetTest(struct Lpss_I2C_Regs *baddr, uint16_t saddr)
{
	int ret = -1;
	uint32_t cmd = CMD_DATA_CMD;
	uint32_t timeout = LPSS_I2C_TIMEOUT_US;
	
	/* Set target slave address */
	write32(&baddr->target_addr, saddr);
	
	LpssI2CEnable(baddr);
	
	/* Write op only: Wait for FIFO not full */
	timeout = LPSS_I2C_TIMEOUT_US;
	while (!(read32(&baddr->status) & STATUS_TX_FIFO_NOT_FULL))
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C transmit timeout \n\n");
			goto litt_end;
		}
	}
	cmd = 0;//CMD_DATA_CMD;
	cmd |= CMD_DATA_STOP;
	write32(&baddr->cmd_data, cmd);

	/* Wait for interrupt status to indicate transfer is complete */
	timeout = LPSS_I2C_TIMEOUT_US;
	while(!(read32(&baddr->raw_intr_stat) & INTR_STAT_STOP_DET))
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			//Print(L"I2C stop bit not received \n\n");
			goto litt_end;
		}
	}

	//check tx complete
	if(read32(&baddr->raw_intr_stat) & INTR_STAT_TX_ABORT)
	{
		//Print(L"I2C TX Abort. sts=0x%x rists=0x%x abr=0x%x \n\n", read32(&baddr->status), read32(&baddr->raw_intr_stat), read32(&baddr->tx_abort_source));
		goto litt_end;
	}

	/* Read to clear INTR_STAT_STOP_DET */
	read32(&baddr->clear_stop_det_intr);

	/* Wait for the bus to go idle */
	if (LpssI2CWaitForBusIdle(baddr))
	{
		Print(L"I2C timeout waiting for bus idle \n");
		goto litt_end;
	}

	/* Flush the RX FIFO in case it is not empty */
	timeout = LPSS_I2C_TIMEOUT_US;
	while (read32(&baddr->status) & STATUS_RX_FIFO_NOT_EMPTY)
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C timeout flushing RX FIFO \n\n");
			goto litt_end;
		}
		read32(&baddr->cmd_data);
	}

	ret = 0;

litt_end:
	read32(&baddr->clear_intr);
	LpssI2CDisable(baddr);
	return ret;
}

int LpssI2CRxData(struct Lpss_I2C_Regs *baddr, struct I2C_Msg *msg)
{
	int ret = -1;
	uint16_t i;
	uint32_t cmd = 0;
	uint32_t timeout = LPSS_I2C_TIMEOUT_US;

	if(msg->wlen)
	{
		/* Set target slave address */
		write32(&baddr->target_addr, msg->slave);
	}

	LpssI2CEnable(baddr);

	if (LpssI2CWaitForBusIdle(baddr))
	{
		Print(L"I2C timeout waiting for bus idle \n");
		goto lird_endend;
	}

	//if(msg->wlen)
	//{
	//	/* Set target slave address */
	//	write32(&baddr->target_addr, msg->slave);
	//}

	//write command
	for(i=0; i<msg->wlen; i++)
	{
		/* Write op only: Wait for FIFO not full */
		timeout = LPSS_I2C_TIMEOUT_US;
		while (!(read32(&baddr->status) & STATUS_TX_FIFO_NOT_FULL))
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				Print(L"I2C transmit timeout \n\n");
				goto lird_end;
			}
		}
		cmd = msg->wbuf[i];

		/* Send stop on last byte, if desired */
		if (i == (msg->wlen - 1))
		{
			cmd |= CMD_DATA_STOP;
		}

		write32(&baddr->cmd_data, cmd);

		//wait tx complete
		/*timeout = LPSS_I2C_TIMEOUT_US;
		while (!(read32(&baddr->status) & STATUS_TX_FIFO_EMPTY))
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				Print(L"I2C transmit timeout \n\n");
				goto lird_end;
			}
		}*/
	}

	//read data
	/* Set target slave address */
	write32(&baddr->target_addr, msg->slave);
	
	for(i=0; i<msg->rlen; i++)
	{
		msg->rbuf[i] = 0;
		
		cmd = CMD_DATA_CMD; /* Read op */
		/* Send stop on last byte, if desired */
		if (i == (msg->rlen - 1))
		{
			cmd |= CMD_DATA_STOP;
		}

		write32(&baddr->cmd_data, cmd);
		
		/* Read op only: Wait for FIFO data and store it */
		timeout = LPSS_I2C_TIMEOUT_US;
		while (!(read32(&baddr->status) & STATUS_RX_FIFO_NOT_EMPTY))
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				Print(L"I2C receive timeout \n\n");
				goto lird_end;
			}
		}
		msg->rbuf[i] = (uint8_t)read32(&baddr->cmd_data);
	}

	/* Wait for interrupt status to indicate transfer is complete */
	timeout = LPSS_I2C_TIMEOUT_US;
	while(!(read32(&baddr->raw_intr_stat) & INTR_STAT_STOP_DET))
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C stop bit not received \n\n");
			goto lird_end;
		}
	}

	/* Read to clear INTR_STAT_STOP_DET */
	read32(&baddr->clear_stop_det_intr);

	//check tx complete
	if(read32(&baddr->raw_intr_stat) & INTR_STAT_TX_ABORT)
	{
		Print(L"I2C TX Abort. sts=0x%x rists=0x%x abr=0x%x \n\n", read32(&baddr->status), read32(&baddr->raw_intr_stat), read32(&baddr->tx_abort_source));
		goto lird_endend;
	}

	/* Wait for the bus to go idle */
	if (LpssI2CWaitForBusIdle(baddr))
	{
		Print(L"I2C timeout waiting for bus idle \n");
		goto lird_endend;
	}

	/* Flush the RX FIFO in case it is not empty */
	timeout = LPSS_I2C_TIMEOUT_US;
	while (read32(&baddr->status) & STATUS_RX_FIFO_NOT_EMPTY)
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C timeout flushing RX FIFO \n\n");
			goto lird_endend;
		}
		read32(&baddr->cmd_data);
	}

	ret = 0;

	goto lird_endend;

lird_end:
	cmd = CMD_DATA_CMD | CMD_DATA_STOP;
	write32(&baddr->cmd_data, cmd);
	
lird_endend:
	read32(&baddr->clear_intr);
	LpssI2CDisable(baddr);
	return ret ;
}

int LpssI2CTxData(struct Lpss_I2C_Regs *baddr, struct I2C_Msg *msg)
{
	int ret = -1;
	uint16_t i;
	uint32_t cmd = 0;
	uint32_t timeout = LPSS_I2C_TIMEOUT_US;
	
	/* Set target slave address */
	write32(&baddr->target_addr, msg->slave);

	LpssI2CEnable(baddr);

	if (LpssI2CWaitForBusIdle(baddr))
	{
		Print(L"I2C timeout waiting for bus idle \n");
		goto litd_endend;
	}

	/* Set target slave address */
	//write32(&baddr->target_addr, msg->slave);

	for(i=0; i<msg->wlen; i++)
	{
		/* Write op only: Wait for FIFO not full */
		timeout = LPSS_I2C_TIMEOUT_US;
		while (!(read32(&baddr->status) & STATUS_TX_FIFO_NOT_FULL))
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				Print(L"I2C transmit timeout \n\n");
				goto litd_end;
			}
		}
		cmd = msg->wbuf[i];

		/* Send stop on last byte, if desired */
		if (i == (msg->wlen - 1))
		{
			cmd |= CMD_DATA_STOP;
		}

		write32(&baddr->cmd_data, cmd);

		//wait tx complete
		timeout = LPSS_I2C_TIMEOUT_US;
		while (!(read32(&baddr->status) & STATUS_TX_FIFO_EMPTY))
		{
			usleep(1);
			timeout--;
			if(timeout <= 0)
			{
				Print(L"I2C transmit timeout \n\n");
				goto litd_end;
			}
		}
	}

	/* Wait for interrupt status to indicate transfer is complete */
	timeout = LPSS_I2C_TIMEOUT_US;
	while(!(read32(&baddr->raw_intr_stat) & INTR_STAT_STOP_DET))
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C stop bit not received \n\n");Print(L"I2C TX Abort. sts=0x%x rists=0x%x abr=0x%x \n\n", read32(&baddr->status), read32(&baddr->raw_intr_stat), read32(&baddr->tx_abort_source));
			goto litd_end;
		}
	}

	/* Read to clear INTR_STAT_STOP_DET */
	read32(&baddr->clear_stop_det_intr);

	//check tx complete
	if(read32(&baddr->raw_intr_stat) & INTR_STAT_TX_ABORT)
	{
		Print(L"I2C TX Abort. sts=0x%x rists=0x%x abr=0x%x \n\n", read32(&baddr->status), read32(&baddr->raw_intr_stat), read32(&baddr->tx_abort_source));
		goto litd_endend;
	}

	/* Wait for the bus to go idle */
	if (LpssI2CWaitForBusIdle(baddr))
	{
		Print(L"I2C timeout waiting for bus idle \n");
		goto litd_endend;
	}

	/* Flush the RX FIFO in case it is not empty */
	timeout = LPSS_I2C_TIMEOUT_US;
	while (read32(&baddr->status) & STATUS_RX_FIFO_NOT_EMPTY)
	{
		usleep(1);
		timeout--;
		if(timeout <= 0)
		{
			Print(L"I2C timeout flushing RX FIFO \n\n");
			goto litd_endend;
		}
		read32(&baddr->cmd_data);
	}

	ret = 0;

	goto litd_endend;

litd_end:
	cmd = CMD_DATA_STOP;
	write32(&baddr->cmd_data, cmd);
	
litd_endend:
	read32(&baddr->clear_intr);
	LpssI2CDisable(baddr);
	return ret ;
}

int AMDSMNI2CXfer(uint8_t ch, uint8_t freq, struct I2C_Msg *msg)
{
	if(msg->wlen)
	{
		if(Mp2I2cWrite(ch, (uint8_t)msg->slave, freq, msg->wlen, msg->wbuf))
		{
			return 1;
		}
	}

	if(msg->rlen)
	{
		if(Mp2I2cRead(ch, (uint8_t)msg->slave, freq, msg->rlen, msg->rbuf))
		{
			return 2;
		}
	}

	return 0;
}

uint8_t I2CMMIORegisterConfig(struct Lpss_I2C_Regs *baddr, uint8_t i)
{
	if(LpssI2CDisable(baddr) != 0)
	{
		Print(L"Lpss I2C %d Disable Fail !! \n", i);
		return 1;
	}

	/* Put controller in master mode with restart enabled */
	write32(&baddr->control, CONTROL_MASTER_MODE | CONTROL_SLAVE_DISABLE | CONTROL_RESTART_ENABLE);

	/* Set bus speed to STANDARD by default */
	if(LpssI2CSetSpeed(baddr, I2C_SPEED_STANDARD) < 0)
	{
		Print(L"I2C %d Failed to set speed for bus \n", i);
		return 2;
	}

	/* Set RX/TX thresholds to smallest values */
	write32(&baddr->rx_thresh, 0);
	write32(&baddr->tx_thresh, 0);

	/* Enable stop detection interrupt */
	//write32(&baddr->intr_mask, INTR_STAT_STOP_DET);

	return 0;
}

uint8_t IntelPCIConfig(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_BUS *i2c, uint8_t i)
{
	struct Lpss_I2C_Regs *baddr;
	uint16_t key;
	
	if(pi2c->vendor_id == 0x8086)
	{
		//if pci Configuration - base address not config, next
		if((pi2c->bar[0] == 0xFFFFFFFF) || (pi2c->bar[0] == 0))
		{
			return 1;
		}
		
		//if pci Configuration - Memory Space not enable
		if((pi2c->command & 0x02) == 0)
		{
			Print(L"I2C %d PCI Configuration Memory Space Not Enable!\n", i);
			Print(L"Press 'Y' To Enable, And Continue Test. Press 'N' To Return.\n\n");
			while(1)
			{
				key = (uint16_t)bioskey(0);
				if((key== 'Y') || (key == 'y'))
				{
					break;
				}
				else if((key == 'N') || (key == 'n'))
				{
					return 2;
				}
			}
			
			pci_select_device(pbus, pi2c->bus, pi2c->dev, pi2c->fun);
			pci_write_byte(pbus, 0x04, (pi2c->command & 0xFF) | 0x02);
			i2c->set_pci_cmd[i] = 1;
		}
		
		i2c->base[i] = (pi2c->bar[0] & 0xFFFFFFE0);	// 10h
		baddr = (struct Lpss_I2C_Regs *)i2c->base[i];
		if((baddr->control == 0xFFFFFFFF) || (baddr->control == 0))
		{
			if(i2c->set_pci_cmd[i])
			{
				pci_select_device(pbus, pi2c->bus, pi2c->dev, pi2c->fun);
				pci_write_byte(pbus, 0x04, pi2c->command & 0xFF);
			}
			return 3;
		}
		
		if(I2CMMIORegisterConfig(baddr, i))
		{
			return 4;
		}
		
		i2c->found |= 1 << i;
		i2c->bus[i] = pi2c->bus;
		i2c->dev[i] = pi2c->dev;
		i2c->fun[i] = pi2c->fun;
		i2c->vendor = pi2c->vendor_id;
		i2c->command[i] = pi2c->command;
		i2c->mode[i] = MMIO;
	}
	else
	{
		return 5;
	}

	return 0;
}

uint8_t CoffeeLake(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_BUS *i2c)
{
	uint8_t i;
	
	// Intel I2C
	//pi2c = find_pci_function(pbus, 0, 21, 0);

	// Serial Controlller - I2C
	if(pci_find_class_subclass_code(pbus, 0x0C, 0x80))
	{
		for(i=0; i<4; i++)
		{
			pi2c = pci_find_device(pbus, 0xA368 + i);

			if(pi2c)
			{
				IntelPCIConfig(pbus, pi2c, i2c, i);
			}
		}

		if(!i2c->found)
		{
			Print(L"Lpss I2C Not Found !! \n\n");
			return 1;
		}
	}
	else
	{
		Print(L"Lpss I2C Controlller Not Found !! \n\n");
		return 2;
	}

	return 0;
}

uint8_t ApolloLake(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_BUS *i2c)
{
	uint8_t i;
	
	// Intel I2C
	//pi2c = find_pci_function(pbus, 0, 22, 0);

	// Serial Controlller - I2C
	if(pci_find_class_subclass_code(pbus, 0x11, 0x80))
	{
		for(i=0; i<8; i++)
		{
			pi2c = pci_find_device(pbus, 0x5AAC + (i * 2));

			if(pi2c)
			{
				IntelPCIConfig(pbus, pi2c, i2c, i);
			}
		}

		if(!i2c->found)
		{
			Print(L"Lpss I2C Not Found !! \n\n");
			return 1;
		}
	}
	else
	{
		Print(L"Lpss I2C Controlller Not Found !! \n\n");
		return 2;
	}

	return 0;
}

uint8_t AMD17H(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_BUS *i2c)
{
	struct Lpss_I2C_Regs *baddr;
	uint8_t i;
	
	for(i=0; i<5; i++)
	{
		if(i<2)
		{
			pi2c = pci_find_device(pbus, 0x15E6);
			if(pi2c == NULL)
			{
				continue;
			}
			
			if(pi2c->vendor_id == 0x1022)
			{
				if(Mp2I2CEnable(i, 0))
				{
					continue;
				}
				
				i2c->found	|= 1 << i;
				i2c->bus[i] = pi2c->bus;
				i2c->dev[i] = pi2c->dev;
				i2c->fun[i] = pi2c->fun;
				i2c->vendor = pi2c->vendor_id;
				i2c->command[i] = pi2c->command;
				i2c->mode[i] = PCISMN;
			}
		}
		else
		{
			i2c->base[i]	= gI2cSdpAddressList[i];
			if(i == 2)
			{
				if((read8((uint32_t*)0xFED80D71) != 0x01) && (read8((uint32_t*)0xFED80D72) != 0x01))
				{
					continue;
				}
			}
			if(i == 3)
			{
				if((read8((uint32_t*)0xFED80D13) != 0x01) && (read8((uint32_t*)0xFED80D14) != 0x01))
				{
					continue;
				}
			}
			
			baddr = (struct Lpss_I2C_Regs *)i2c->base[i];
			if((baddr->control == 0xFFFFFFFF) || (baddr->control == 0))
			{
				continue;
			}

			if(I2CMMIORegisterConfig(baddr, i))
			{
				continue;
			}
			
			i2c->found	|= 1 << i;
			//i2c->bus[i] = pi2c->bus;
			//i2c->dev[i] = pi2c->dev;
			//i2c->fun[i] = pi2c->fun;
			//i2c->vendor = pi2c->vendor_id;
			//i2c->command[i] = pi2c->command;
			i2c->mode[i] = MMIO;
		}

		if(!i2c->found)
		{
			Print(L"Lpss I2C Not Found !! \n\n");
			return 3;
		}
	}
	
	return 0;
}

I2C_BUS *LpssI2CInit(void)
{
	PCI_BUS		*pbus;
	PCI_DEVICE	*pdev;
	PCI_DEVICE	*pi2c = NULL;
	I2C_BUS		*i2c;

	uint32_t family = 0, model = 0, buf32[4];

	if(EFIPCIInit())
	{
		Print(L"PCI Initial Fail  !! \n\n");
		return NULL;
	}
	
	pbus = pci_bus_scan();

	if (!pbus)
	{
		return NULL;
	}

	i2c = (I2C_BUS*)malloc(sizeof(I2C_BUS));
	if (!i2c)
	{
		pci_bus_free(pbus);
		return NULL;
	}

	// Intel I2C : Bus 0, Device 22, Function 0 = 8086
	
	memset(i2c, 0, sizeof(I2C_BUS));

	// Vendor Check : Bus 0, Device 0, Function 0
	pdev = pci_find_function(pbus, 0, 0, 0);
	if(pdev)
	{
		if(pdev->vendor_id == 0x8086)		//intel
		{
			if((pdev->device_id & 0xFF00) == 0x3E00)	//coffee lake
			{
				if(CoffeeLake(pbus, pi2c, i2c))
				{
					goto lii_end;
				}
			}
			else if((pdev->device_id & 0xFF00) == 0x5A00)	//apollo lake
			{
				if(ApolloLake(pbus, pi2c, i2c))
				{
					goto lii_end;
				}
			}
			else
			{
				Print(L"This Platform Not Support !! \n\n");
			}
		}
		else if(pdev->vendor_id == 0x1022)		//amd
		{
			buf32[0] = buf32[1] = buf32[2] = buf32[3] = 0;
			AsmCpuid(1, &buf32[0], &buf32[1], &buf32[2], &buf32[3]);

			family = CPUIDTOFAMILY(buf32[0]);
			model = CPUIDTOMODEL(buf32[0]);
			
			if(family == 0x17)
			{
				if(AMD17H(pbus, pi2c, i2c))
				{
					goto lii_end;
				}
			}
			else
			{
				Print(L"This Platform Not Support !! \n\n");
			}
		}

		gVendor = pdev->vendor_id;
	}

lii_end:
	pci_bus_free(pbus);
	return i2c;
}

void LpssI2CScanBus(I2C_BUS	*i2c)
{
	struct Lpss_I2C_Regs *baddr;
	uint8_t ch, buf;
	uint16_t i;
	
	clrscr();
	Print(L"List I2C Device Address \n\n");
	
	gotoxy(0, 2);
	ShowI2CChannel(i2c);
	Print(L"Please choice channel : ");
	ch = (uint8_t)(bioskey(0) - 0x30);
	Print(L"%d\n", ch);
	
	if(!(i2c->found & (1 << ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", ch);
		goto lisb_end;
	}
	
	i2c->dev_num = 0;
	
	if(i2c->mode[ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[ch];
		
		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, 0))
		{
			goto lisb_end;
		}
		
		Print(L"Scan I2C%d ... \n\n", ch);
		for(i=0x08; i<0x78; i++)
		{
			gotoxy(0, 4);
			Print(L"Scaning... 0x%X", i * 2);
			
			if(LpssI2CTargetTest(baddr, i) != 0)
			{
				continue;
			}
			
			//communication success
			gotoxy(0, 5 + i2c->dev_num);
			Print(L"Device %d Address : 0x%X \n", i2c->dev_num, i * 2);
			i2c->dev_num++;
		}
	}
	else if(i2c->mode[ch] == PCISMN)
	{
		Print(L"Scan I2C%d ... \n\n", ch);
		for(i=0x08; i<0x78; i++)
		{
			gotoxy(0, 4);
			Print(L"Scaning... 0x%X", i * 2);
			
			if(Mp2I2cRead(ch, (uint8_t)i, gI2CConfig->eeprom->freq, 1, &buf) != EFI_SUCCESS)
			{
				continue;
			}
			
			//communication success
			gotoxy(0, 4 + i2c->dev_num);
			Print(L"Device %d Address : 0x%X \n", i2c->dev_num, i * 2);
			i2c->dev_num++;
		}
	}

	gotoxy(0, 4);
	Print(L"                ");

	if(i2c->dev_num == 0)
	{
		gotoxy(0, 5);
		Print(L"No Device \n\n");
	}
	Print(L"\n\n");

lisb_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void LpssI2CReadEepromTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t wbuf[2], rbuf[256];
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint16_t i, len, loop, eeprom_saddr = 0;
	
	clrscr();
	Print(L"I2C - Read Eeprom Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->eeprom->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->eeprom->ch);
		goto liret_end;
	}
	
	eeprom_ch = gI2CConfig->eeprom->ch;
	eeprom_type = gI2CConfig->eeprom->type;
	eeprom_addr = gI2CConfig->eeprom->addr;
	eeprom_saddr = gI2CConfig->eeprom->spos;
	len = gI2CConfig->eeprom->rwlen;
	loop = gI2CConfig->eeprom->loop; 

	if(eeprom_type)
	{
		wbuf[0] = (eeprom_saddr >> 8) & 0xFF;
		wbuf[1] = eeprom_saddr & 0xFF;
	}
	else
	{
		wbuf[0] = eeprom_saddr & 0xFF;
	}

	if(i2c->mode[eeprom_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[eeprom_ch];
		
		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->eeprom->freq))
		{
			goto liret_end;
		}
	}
	
	//start to read
	msg.flags = 0x0001;
	msg.slave = eeprom_addr / 2;
	if(eeprom_type)
	{
		msg.wlen = 2;
	}
	else
	{
		msg.wlen = 1;
	}
	msg.wbuf = wbuf;
	msg.rlen = len;
	msg.rbuf = rbuf;

	for(i=0; i<loop; i++)
	{
		if(i2c->mode[eeprom_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liret_end;
			}
		}
		else if(i2c->mode[eeprom_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liret_end;
			}
		}

		gotoxy(0, 2);
		Print(L"Read EEPROM Loop %5d complet !! \n\n", i+1);
		usleep(100 * 1000);	//100ms
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
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

liret_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void LpssI2CWriteEepromTest(I2C_BUS *i2c, uint8_t option)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr;
	uint8_t buf[260], wbuf[4], rbuf[256];
	uint8_t value, eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint16_t i, len, tmp, eeprom_saddr = 0;
	
	clrscr();
	Print(L"I2C - Write Eeprom Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->eeprom->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->eeprom->ch);
		goto liwet_end;
	}
	
	eeprom_ch = gI2CConfig->eeprom->ch;
	eeprom_type = gI2CConfig->eeprom->type;
	eeprom_addr = gI2CConfig->eeprom->addr;
	eeprom_saddr = gI2CConfig->eeprom->spos;
	len = gI2CConfig->eeprom->rwlen;
	value = gI2CConfig->eeprom->wsval;

	tmp = eeprom_saddr;
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[eeprom_ch];
		
		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->eeprom->freq))
		{
			goto liwet_end;
		}

		//start to write
		for(i=0; i<len; i++)
		{
			msg.flags = 0x0000;
			msg.slave = eeprom_addr / 2;
			if(eeprom_type)
			{
				wbuf[0] = (tmp >> 8) & 0xFF;
				wbuf[1] = tmp & 0xFF;
				wbuf[2] = buf[i] = value;
				msg.wlen = 3;
			}
			else
			{
				wbuf[0] = tmp & 0xFF;
				wbuf[1] = buf[i] = value;
				msg.wlen = 2;
			}
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;

			if(LpssI2CTxData(baddr, &msg) != 0)
			{
				Print(L"I2C Tx Fail !! \n\n");
				goto liwet_end;
			}

			usleep(5000);
			
			tmp++;
			
			//option is write auto increase
			if(option)
			{
				value++;
			}
		}

		usleep(5000);

		//start to read
		msg.flags = 0x0001;
		msg.slave = eeprom_addr / 2;
		if(eeprom_type)
		{
			wbuf[0] = (eeprom_saddr >> 8) & 0xFF;
			wbuf[1] = eeprom_saddr & 0xFF;
			msg.wlen = 2;
		}
		else
		{
			wbuf[0] = eeprom_saddr & 0xFF;
			msg.wlen = 1;
		}
		msg.wbuf = wbuf;
		msg.rlen = len;
		msg.rbuf = rbuf;

		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liwet_end;
		}
	}
	else if(i2c->mode[eeprom_ch] == PCISMN)
	{
		//start to write
		for(i=0; i<len; i++)
		{
			msg.flags = 0x0000;
			msg.slave = eeprom_addr / 2;
			if(eeprom_type)
			{
				wbuf[0] = (tmp >> 8) & 0xFF;
				wbuf[1] = tmp & 0xFF;
				wbuf[2] = buf[i] = value;
				msg.wlen = 3;
			}
			else
			{
				wbuf[0] = tmp & 0xFF;
				wbuf[1] = buf[i] = value;
				msg.wlen = 2;
			}
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;
			if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
			{
				Print(L"I2C Tx Fail !! \n\n");
				goto liwet_end;
			}

			usleep(5000);
			
			tmp++;

			//option is write auto increase
			if(option)
			{
				value++;
			}
		}

		usleep(5000);

		//start to read
		msg.flags = 0x0001;
		msg.slave = eeprom_addr / 2;
		if(eeprom_type)
		{
			wbuf[0] = (eeprom_saddr >> 8) & 0xFF;
			wbuf[1] = eeprom_saddr & 0xFF;
			msg.wlen = 2;
		}
		else
		{
			wbuf[0] = eeprom_saddr & 0xFF;
			msg.wlen = 1;
		}
		msg.wbuf = wbuf;
		msg.rlen = len;
		msg.rbuf = rbuf;
		if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liwet_end;
		}
	}

	//print data and compare
	for(i=0; i<len; i++)
	{
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}

		//compare read/write data
		if(buf[i] != rbuf[i])
		{
			console_color(RED, BLACK);
			Print(L"EEPROM R/W Compare Error!! \n");
			goto liwet_end;
		}
	}
	console_color(GREEN, BLACK);
	Print(L"EEPROM R/W Compare OK!! \n");

liwet_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void LpssI2CCheckEEPROMBoundary(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t wbuf[4], rbuf[2];
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t buf[2], bakbuf[2], count = 0;
	uint16_t i, tmp = 0;
	
	clrscr();
	Print(L"I2C - Check Eeprom Boundary. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->eeprom->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->eeprom->ch);
		goto liceb_end;
	}
	
	eeprom_ch = gI2CConfig->eeprom->ch;
	eeprom_type = gI2CConfig->eeprom->type;
	
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[eeprom_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->eeprom->freq))
		{
			goto liceb_end;
		}
	}
	
	//if type is byte mode, search 0xAx total size
	if(eeprom_type == 0)
	{
		for(i=0xA0; i<0xAF; i+=2)
		{
			if(i2c->mode[eeprom_ch] == MMIO)
			{
				if(LpssI2CTargetTest(baddr, i/2) != 0)
				{
					continue;
				}
			}
			else if(i2c->mode[eeprom_ch] == PCISMN)
			{
				if(Mp2I2cRead(eeprom_ch, (uint8_t)i/2, gI2CConfig->eeprom->freq, 1, buf) != EFI_SUCCESS)
				{
					continue;
				}
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
			goto liceb_end;
		}
	}

	if(eeprom_type)
	{
		eeprom_addr = gI2CConfig->eeprom->addr;
	}

	//backup eeprom last byte
	msg.flags = 0x0001;
	msg.slave = eeprom_addr / 2;
	if(eeprom_type)
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0xFF;
		msg.wlen = 2;
	}
	else
	{
		wbuf[0] = 0xFF;
		msg.wlen = 1;
	}
	msg.wbuf = wbuf;
	msg.rlen = 1;
	msg.rbuf = bakbuf;
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liceb_end;
		}
	}
	else if(i2c->mode[eeprom_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liceb_end;
		}
	}

	usleep(5000);	//must set more than 2ms

	//write data to eeprom
	msg.flags = 0x0000;
	msg.slave = eeprom_addr / 2;
	if(eeprom_type)
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0xFF;
		wbuf[2] = 0x55;
		msg.wlen = 3;
	}
	else
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0x55;
		msg.wlen = 2;
	}
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}
	else if(i2c->mode[eeprom_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}

	usleep(5000);	//must set more than 2ms
	
	for(i=1; i<8; i++)
	{
		//read data
		msg.flags = 0x0001;
		msg.slave = eeprom_addr / 2;
		if(eeprom_type)
		{
			wbuf[0] = (0xFF >> i);
			wbuf[1] = 0xFF;
			msg.wlen = 2;
		}
		else
		{
			wbuf[0] = (0xFF >> i);
			msg.wlen = 1;
		}
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[eeprom_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liceb_end;
			}
		}
		else if(i2c->mode[eeprom_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liceb_end;
			}
		}

		if(rbuf[0] != 0x55)
		{
			tmp = i - 1;
			break;
		}
	}
	
	usleep(5000);	//must set more than 2ms
	
	//write data to eeprom
	msg.flags = 0x0000;
	msg.slave = eeprom_addr / 2;
	if(eeprom_type)
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0xFF;
		wbuf[2] = 0xAA;
		msg.wlen = 3;
	}
	else
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0xAA;
		msg.wlen = 2;
	}
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}
	else if(i2c->mode[eeprom_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}

	usleep(5000);	//must set more than 2ms

	for(i=tmp; i>=0; i--)
	{
		//read data
		msg.flags = 0x0001;
		msg.slave = eeprom_addr / 2;
		if(eeprom_type)
		{
			wbuf[0] = (0xFF >> i);
			wbuf[1] = 0xFF;
			msg.wlen = 2;
		}
		else
		{
			wbuf[0] = (0xFF >> i);
			msg.wlen = 1;
		}
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[eeprom_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liceb_end;
			}
		}
		else if(i2c->mode[eeprom_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
			{
				Print(L"I2C Rx Fail !! \n\n");
				goto liceb_end;
			}
		}

		if(rbuf[0] == 0xAA)
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
			goto liceb_end;
		}
	}

	usleep(5000);	//must set more than 2ms

	//restore eeprom last byte
	msg.flags = 0x0000;
	msg.slave = eeprom_addr / 2;
	if(eeprom_type)
	{
		wbuf[0] = 0xFF;
		wbuf[1] = 0xFF;
		wbuf[2] = bakbuf[0];
		msg.wlen = 3;
	}
	else
	{
		wbuf[0] = 0xFF;
		wbuf[1] = bakbuf[0];
		msg.wlen = 2;
	}
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[eeprom_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}
	else if(i2c->mode[eeprom_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(eeprom_ch, gI2CConfig->eeprom->freq, &msg) != 0)
		{
			Print(L"I2C Tx Fail !! \n\n");
			goto liceb_end;
		}
	}

liceb_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void I2CEEPROMTest(I2C_BUS *i2c)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test I2C - EERPOM\n\n");
		Print(L"0. EERPOM Config Page. \n");
		Print(L"1. Test I2C - Read EERPOM. \n");
		Print(L"2. Test I2C - Write EERPOM. \n");
		Print(L"3. Test I2C - Auto Write EEPROM. \n");
		Print(L"4. Test I2C - Check EEPROM Boundary. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				EepromConfiguration(i2c);
				break;
			}
			case '1':
			{
				LpssI2CReadEepromTest(i2c);
				break;
			}
			case '2':
			{
				LpssI2CWriteEepromTest(i2c, 0);
				break;
			}
			case '3':
			{
				LpssI2CWriteEepromTest(i2c, 1);
				break;
			}
			case '4':
			{
				LpssI2CCheckEEPROMBoundary(i2c);
				break;
			}
		}
	}
}

void LpssI2CReadThermalICTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"I2C - Read Thermal IC Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->thermal->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->thermal->ch);
		goto lirt_end;
	}
	
	thermal_ch = gI2CConfig->thermal->ch;
	thermal_addr = gI2CConfig->thermal->addr;

	if(i2c->mode[thermal_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[thermal_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->thermal->freq))
		{
			goto lirt_end;
		}
	}

	//get chip Manufacturer ID
	wbuf[0] = LM_REG_MFU_ID;
	msg.flags = 0x0001;
	msg.slave = thermal_addr / 2;
	msg.wlen = 1;
	msg.wbuf = wbuf;
	msg.rlen = 1;
	msg.rbuf = rbuf;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Manufacturer ID Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Manufacturer ID Fail \n");
			goto lirt_end;
		}
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	wbuf[0] = LM_REG_CHIP_REV;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Revision ID Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Revision ID Fail \n");
			goto lirt_end;
		}
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//read Local Temp MSB
	wbuf[0] = LM_REG_TEMP_H;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Local Temp MSB Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Local Temp MSB Fail \n");
			goto lirt_end;
		}
	}
	tmp = rbuf[0];
	
	//read Local Temp LSB
	wbuf[0] = LM_REG_TEMP_L;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Local Temp LSB Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Local Temp LSB Fail \n");
			goto lirt_end;
		}
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
	wbuf[0] = LM_REG_REMOTE_OFFSET_H;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Remote Offset High Byte Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Remote Offset High Byte Fail \n");
			goto lirt_end;
		}
	}
	tmp = rbuf[0];

	//read Remote Offset Low Byte
	wbuf[0] = LM_REG_REMOTE_OFFSET_L;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Remote Offset Low Byte Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Remote Offset Low Byte Fail \n");
			goto lirt_end;
		}
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
	wbuf[0] = LM_REG_T_CRIT_LMT;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
			goto lirt_end;
		}
	}
	Print(L"Local T_Crit Limit:  %3d C \n", (uint16_t)rbuf[0]);

	//read Remote OS Limit
	wbuf[0] = LM_REG_REMOTE_OS_LMT;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Remote OS Limit Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Remote OS Limit Fail \n");
			goto lirt_end;
		}
	}
	Print(L"Remote OS Limit:     %3d C \n", (uint16_t)rbuf[0]);

	//read Remote T_Crit Limit
	wbuf[0] = LM_REG_REMOTE_CRIT_LMT;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Remote T_Crit Limit Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Remote T_Crit Limit Fail \n");
			goto lirt_end;
		}
	}
	Print(L"Remote T_Crit Limit: %3d C \n", (uint16_t)rbuf[0]);

	//read Common Hysteresis
	wbuf[0] = LM_REG_COMM_HYSTERESIS;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Common Hysteresis Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Common Hysteresis Fail \n");
			goto lirt_end;
		}
	}
	Print(L"Common Hysteresis:    %2d C \n", (uint16_t)rbuf[0]);

	//read Status Register 1
	wbuf[0] = LM_REG_STATUS_1;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Status Register 1 Fail \n");
			goto lirt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Status Register 1 Fail \n");
			goto lirt_end;
		}
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

lirt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void LpssI2CWriteThermalICTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t tmp;
	
	clrscr();
	Print(L"I2C - Write Thermal IC Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->thermal->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->thermal->ch);
		goto liwt_end;
	}
	
	thermal_ch = gI2CConfig->thermal->ch;
	thermal_addr = gI2CConfig->thermal->addr;

	if(i2c->mode[thermal_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[thermal_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->thermal->freq))
		{
			goto liwt_end;
		}
	}
	
	//get chip Manufacturer ID
	wbuf[0] = LM_REG_MFU_ID;
	msg.flags = 0x0001;
	msg.slave = thermal_addr / 2;
	msg.wlen = 1;
	msg.wbuf = wbuf;
	msg.rlen = 1;
	msg.rbuf = rbuf;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Manufacturer ID Fail \n");
			goto liwt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Manufacturer ID Fail \n");
			goto liwt_end;
		}
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	wbuf[0] = LM_REG_CHIP_REV;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Revision ID Fail \n");
			goto liwt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Revision ID Fail \n");
			goto liwt_end;
		}
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//enter new value
	wbuf[1] = gI2CConfig->thermal->wsval;

	//write Local Shared OS and T_Crit Limit
	wbuf[0] = LM_REG_T_CRIT_LMT;
	msg.flags = 0x0000;
	msg.slave = thermal_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Write Local Shared OS and T_Crit Limit Fail \n");
			goto liwt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Write Local Shared OS and T_Crit Limit Fail \n");
			goto liwt_end;
		}
	}

	//read Local Shared OS and T_Crit Limit
	wbuf[0] = LM_REG_T_CRIT_LMT;
	msg.flags = 0x0001;
	msg.slave = thermal_addr / 2;
	msg.wlen = 1;
	msg.wbuf = wbuf;
	msg.rlen = 1;
	msg.rbuf = rbuf;
	if(i2c->mode[thermal_ch] == MMIO)
	{
		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
			goto liwt_end;
		}
	}
	else if(i2c->mode[thermal_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(thermal_ch, gI2CConfig->thermal->freq, &msg) != 0)
		{
			Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
			goto liwt_end;
		}
	}

	//compare value
	if(wbuf[1] == rbuf[0])
	{
		console_color(GREEN, BLACK);
		Print(L"Write New OTP Threshold OK!! \n");
	}
	else
	{
		console_color(RED, BLACK);
		Print(L"Write New OTP Threshold Fail!! \n");
	}

liwt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

void I2CThermalICTest(I2C_BUS *i2c)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test I2C - Thermal\n\n");
		Print(L"0. Thermal Config Page. \n");
		Print(L"1. Test I2C - Read Thermal IC. \n");
		Print(L"2. Test I2C - Write Thermal IC. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				ThermalICConfiguration(i2c);
				break;
			}
			case '1':
			{
				LpssI2CReadThermalICTest(i2c);
				break;
			}
			case '2':
			{
				LpssI2CWriteThermalICTest(i2c);
				break;
			}
		}
	}
}

void I2CEatBoardTest(I2C_BUS *i2c)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test I2C - EERPOM\n\n");
		Print(L"1. Test I2C - EERPOM. \n");
		Print(L"2. Test I2C - Thermal. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				I2CEEPROMTest(i2c);
				break;
			}
			case '2':
			{
				I2CThermalICTest(i2c);
				break;
			}
		}
	}
}

void LpssI2CReadDIOBoardTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint8_t i;
	uint16_t key = 0;

	clrscr();
	Print(L"I2C - Read DIO Board Test\n");

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(!(i2c->found & (1 << gI2CConfig->dio->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->dio->ch);
		goto lirdbt_end;
	}
	
	dio_ch = gI2CConfig->dio->ch;
	dio_addr = gI2CConfig->dio->addr;

	if(i2c->mode[dio_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[dio_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->dio->freq))
		{
			goto lirdbt_end;
		}
	}

	//set all pins of port0 as input
	wbuf[0] = DIO_REG_CONFIG0;
	wbuf[1] = 0xFF;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto lirdbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto lirdbt_end;
		}
	}
	// set all pins of port1 as input
	wbuf[0] = DIO_REG_CONFIG1;
	wbuf[1] = 0xFF;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto lirdbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto lirdbt_end;
		}
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
		if(key == (SCAN_ESC << 8))
		{
			goto lirdbt_endend;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		wbuf[0] = DIO_REG_IN_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto lirdbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}

		//read port1 level
		wbuf[0] = DIO_REG_IN_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto lirdbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}
		Print(L"\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		wbuf[0] = DIO_REG_IN_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Input Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Input Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"Input port registers    : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_IN_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Input Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Input Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_OUT_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Output Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Output Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"Output port registers   : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_OUT_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Output Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Output Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_INV_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"Polarity registers      : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_INV_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_CONFIG1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"Configuration registers : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_CONFIG0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Configuration Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Configuration Port 0 Fail \n");
				goto lirdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);
	}

lirdbt_end:
	while(!CheckESC(bioskey(0))){}

lirdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void LpssI2CWriteDIOBoardTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t bit = 0xFF;
	uint8_t wbuf[2], rbuf[2];
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Write DIO Board Test\n");

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(!(i2c->found & (1 << gI2CConfig->dio->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->dio->ch);
		goto liwdbt_end;
	}
	
	dio_ch = gI2CConfig->dio->ch;
	dio_addr = gI2CConfig->dio->addr;
	
	if(i2c->mode[dio_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[dio_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->dio->freq))
		{
			goto liwdbt_end;
		}
	}
	
	// set all pins of port0 as output
	wbuf[0] = DIO_REG_CONFIG0;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto liwdbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto liwdbt_end;
		}
	}
	// set all pins of port1 as output
	wbuf[0] = DIO_REG_CONFIG1;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto liwdbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto liwdbt_end;
		}
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
		wbuf[0] = DIO_REG_IN_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto liwdbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}

		//read port1 level
		wbuf[0] = DIO_REG_IN_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto liwdbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			goto liwdbt_endend;
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
		wbuf[0] = DIO_REG_OUT_PORT0;
		wbuf[1] = level & 0xFF;
		msg.flags = 0x0000;
		msg.slave = dio_addr / 2;
		msg.wlen = 2;
		msg.wbuf = wbuf;
		msg.rlen = 0;
		msg.rbuf = NULL;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CTxData(baddr, &msg) != 0)
			{
				Print(L"Write New Level To Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Write New Level To Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		// set new level to port1
		wbuf[0] = DIO_REG_OUT_PORT1;
		wbuf[1] = (level >> 8) & 0xFF;
		msg.flags = 0x0000;
		msg.slave = dio_addr / 2;
		msg.wlen = 2;
		msg.wbuf = wbuf;
		msg.rlen = 0;
		msg.rbuf = NULL;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CTxData(baddr, &msg) != 0)
			{
				Print(L"Write New Level To Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Write New Level To Port 1 Fail \n");
				goto liwdbt_end;
			}
		}

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		wbuf[0] = DIO_REG_IN_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Input Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Input Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"Input port registers    : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_IN_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Input Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Input Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_OUT_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Output Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Output Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"Output port registers   : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_OUT_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Output Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Output Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_INV_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"Polarity registers      : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_INV_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);

		wbuf[0] = DIO_REG_CONFIG1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"Configuration registers : 0x%02X", rbuf[0]);
		wbuf[0] = DIO_REG_CONFIG0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Configuration Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Configuration Port 0 Fail \n");
				goto liwdbt_end;
			}
		}
		Print(L"%02X \n", rbuf[0]);
	}

liwdbt_end:
	while(!CheckESC(bioskey(0))){}

liwdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void LpssI2CAutoDIOBoardTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr = NULL;
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t wbuf[2], rbuf[2];
	uint8_t bit = 0;
	uint8_t cnt = 0;
	uint16_t key = 0;
	uint16_t level = 0;

	clrscr();
	Print(L"SMBus - Auto R/W DIO Board Test\n");

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(!(i2c->found & (1 << gI2CConfig->dio->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->dio->ch);
		goto liadbt_end;
	}
	
	dio_ch = gI2CConfig->dio->ch;
	dio_addr = gI2CConfig->dio->addr;
	
	if(i2c->mode[dio_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[dio_ch];

		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->dio->freq))
		{
			goto liadbt_end;
		}
	}
	
	// set all pins of port0 as output
	wbuf[0] = DIO_REG_CONFIG0;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto liadbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 0 As Output Fail \n");
			goto liadbt_end;
		}
	}
	// set all pins of port1 as output
	wbuf[0] = DIO_REG_CONFIG1;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto liadbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Set Port 1 As Output Fail \n");
			goto liadbt_end;
		}
	}

	// set port0 to low
	wbuf[0] = DIO_REG_OUT_PORT0;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Write Port 0 To Low Fail \n");
			goto liadbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Write Port 0 To Low Fail \n");
			goto liadbt_end;
		}
	}
	// set port1 to low
	wbuf[0] = DIO_REG_OUT_PORT1;
	wbuf[1] = 0;
	msg.flags = 0x0000;
	msg.slave = dio_addr / 2;
	msg.wlen = 2;
	msg.wbuf = wbuf;
	msg.rlen = 0;
	msg.rbuf = NULL;
	if(i2c->mode[dio_ch] == MMIO)
	{
		if(LpssI2CTxData(baddr, &msg) != 0)
		{
			Print(L"Write Port 1 To Low Fail \n");
			goto liadbt_end;
		}
	}
	else if(i2c->mode[dio_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
		{
			Print(L"Write Port 1 To Low Fail \n");
			goto liadbt_end;
		}
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

		wbuf[0] = DIO_REG_IN_PORT0;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto liadbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 0 Level Fail \n");
				goto liadbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}

		wbuf[0] = DIO_REG_IN_PORT1;
		msg.flags = 0x0001;
		msg.slave = dio_addr / 2;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = 1;
		msg.rbuf = rbuf;
		if(i2c->mode[dio_ch] == MMIO)
		{
			if(LpssI2CRxData(baddr, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto liadbt_end;
			}
		}
		else if(i2c->mode[dio_ch] == PCISMN)
		{
			if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
			{
				Print(L"Read Port 1 Level Fail \n");
				goto liadbt_end;
			}
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (rbuf[0] >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			goto liadbt_endend;
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
			wbuf[0] = DIO_REG_OUT_PORT0;
			wbuf[1] = level & 0xFF;
			msg.flags = 0x0000;
			msg.slave = dio_addr / 2;
			msg.wlen = 2;
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CTxData(baddr, &msg) != 0)
				{
					Print(L"Write New Level To Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Write New Level To Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			// set new level to port1
			wbuf[0] = DIO_REG_OUT_PORT1;
			wbuf[1] = (level >> 8) & 0xFF;
			msg.flags = 0x0000;
			msg.slave = dio_addr / 2;
			msg.wlen = 2;
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CTxData(baddr, &msg) != 0)
				{
					Print(L"Write New Level To Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Write New Level To Port 1 Fail \n");
					goto liadbt_end;
				}
			}

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			wbuf[0] = DIO_REG_IN_PORT1;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Input Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Input Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"Input port registers    : 0x%02X", rbuf[0]);
			wbuf[0] = DIO_REG_IN_PORT0;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Input Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Input Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"%02X \n", rbuf[0]);

			wbuf[0] = DIO_REG_OUT_PORT1;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Output Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Output Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"Output port registers   : 0x%02X", rbuf[0]);
			wbuf[0] = DIO_REG_OUT_PORT0;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Output Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Output Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"%02X \n", rbuf[0]);

			wbuf[0] = DIO_REG_INV_PORT1;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Polarity Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Polarity Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"Polarity registers      : 0x%02X", rbuf[0]);
			wbuf[0] = DIO_REG_INV_PORT0;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Polarity Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Polarity Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"%02X \n", rbuf[0]);

			wbuf[0] = DIO_REG_CONFIG1;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Configuration Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Configuration Port 1 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"Configuration registers : 0x%02X", rbuf[0]);
			wbuf[0] = DIO_REG_CONFIG0;
			msg.flags = 0x0001;
			msg.slave = dio_addr / 2;
			msg.wlen = 1;
			msg.wbuf = wbuf;
			msg.rlen = 1;
			msg.rbuf = rbuf;
			if(i2c->mode[dio_ch] == MMIO)
			{
				if(LpssI2CRxData(baddr, &msg) != 0)
				{
					Print(L"Read Configuration Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			else if(i2c->mode[dio_ch] == PCISMN)
			{
				if(AMDSMNI2CXfer(dio_ch, gI2CConfig->dio->freq, &msg) != 0)
				{
					Print(L"Read Configuration Port 0 Fail \n");
					goto liadbt_end;
				}
			}
			Print(L"%02X \n", rbuf[0]);

			bit++;
		}
	}

liadbt_end:
	while(!CheckESC(bioskey(0))){}

liadbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void I2CDIOBoardTest(I2C_BUS *i2c)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"Test SMBus - DIO Board\n\n");
		Print(L"0. DIO Board Config Page. \n");
		Print(L"1. Test I2C - Read DIO Board. \n");
		Print(L"2. Test I2C - Write DIO Board. \n");
		Print(L"3. Test I2C - Auto R/W DIO Board. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				DIOBoardConfiguration(i2c);
				break;
			}
			case '1':
			{
				LpssI2CReadDIOBoardTest(i2c);
				break;
			}
			case '2':
			{
				LpssI2CWriteDIOBoardTest(i2c);
				break;
			}
			case '3':
			{
				LpssI2CAutoDIOBoardTest(i2c);
				break;
			}
		}
	}
}

void LpssI2CReadAnotherDeviceTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr;
	uint8_t wbuf[2], rbuf[256];
	uint8_t dev_ch = 0, dev_addr = 0, dev_cmd = 0;
	uint16_t i, len;
	
	clrscr();
	Print(L"I2C - Read Another Device Test. \n\n");
	
	while(1)
	{
		gotoxy(0, 2);
		Print(L"Press C to config parameter, Enter to start test, ESC to return");
		switch(bioskey(0))
		{
			case 'C':
			case 'c':
			{
				DeviceConfiguration(i2c);
				clrscr();
				Print(L"I2C - Read Another Device Test. \n\n");
				break;
			}
			case CHAR_CARRIAGE_RETURN:
			{
				goto start_test;
				break;
			}
			case (SCAN_ESC << 8):
			{
				goto liret_endend;
				break;
			}
		}
	}
	
start_test:
	clrscr();
	Print(L"I2C - Read Another Device Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->dev->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->dev->ch);
		goto liret_end;
	}
	
	dev_ch = gI2CConfig->dev->ch;
	dev_addr = gI2CConfig->dev->addr;
	dev_cmd = gI2CConfig->dev->cmd;
	len = 1;//gI2CConfig->dev->rwlen;

	wbuf[0] = dev_cmd;

	//start to read
	msg.flags = 0x0001;
	msg.slave = dev_addr / 2;
	msg.wlen = 1;
	msg.wbuf = wbuf;
	msg.rlen = len;
	msg.rbuf = rbuf;

	if(i2c->mode[dev_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[dev_ch];
		
		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->dev->freq))
		{
			goto liret_end;
		}

		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liret_end;
		}
	}
	else if(i2c->mode[dev_ch] == PCISMN)
	{
		if(AMDSMNI2CXfer(dev_ch, gI2CConfig->dev->freq, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liret_end;
		}
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
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

liret_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}

liret_endend:
	NULL;
}

void LpssI2CWriteAnotherDeviceTest(I2C_BUS *i2c)
{
	struct I2C_Msg msg;
	struct Lpss_I2C_Regs *baddr;
	uint8_t buf[260], wbuf[4], rbuf[256];
	uint8_t value, dev_ch = 0, dev_addr = 0, dev_cmd = 0;
	uint16_t i, len, tmp;
	
	clrscr();
	Print(L"I2C - Write Another Device Test. \n\n");
	
	while(1)
	{
		gotoxy(0, 2);
		Print(L"Press C to config parameter, Enter to start test, ESC to return");
		switch(bioskey(0))
		{
			case 'C':
			case 'c':
			{
				DeviceConfiguration(i2c);
				clrscr();
				Print(L"I2C - Write Another Device Test. \n\n");
				break;
			}
			case CHAR_CARRIAGE_RETURN:
			{
				goto start_test;
				break;
			}
			case (SCAN_ESC << 8):
			{
				goto liwet_endend;
				break;
			}
		}
	}
	
start_test:
	clrscr();
	Print(L"I2C - Write Another Device Test. \n\n");
	
	if(!(i2c->found & (1 << gI2CConfig->dev->ch)))
	{
		Print(L"I2C %d Can't Use !! Please Reconfig !!", gI2CConfig->dev->ch);
		goto liwet_end;
	}
	
	dev_ch = gI2CConfig->dev->ch;
	dev_addr = gI2CConfig->dev->addr;
	dev_cmd = gI2CConfig->dev->cmd;
	len = 1;//gI2CConfig->dev->rwlen;
	value = gI2CConfig->dev->wsval;

	tmp = dev_cmd;
	if(i2c->mode[dev_ch] == MMIO)
	{
		baddr = (struct Lpss_I2C_Regs *)i2c->base[dev_ch];
		
		//set bus speed
		if(LpssI2CChoiceSpeed(baddr, gI2CConfig->dev->freq))
		{
			goto liwet_end;
		}

		//start to write
		for(i=0; i<len; i++)
		{
			msg.flags = 0x0000;
			msg.slave = dev_addr / 2;
			wbuf[0] = tmp & 0xFF;
			wbuf[1] = buf[i] = value;
			msg.wlen = 2;
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;

			if(LpssI2CTxData(baddr, &msg) != 0)
			{
				Print(L"I2C Tx Fail !! \n\n");
				goto liwet_end;
			}

			usleep(5000);
			
			tmp++;
		}

		usleep(5000);

		//start to read
		msg.flags = 0x0001;
		msg.slave = dev_addr / 2;
		wbuf[0] = dev_cmd;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = len;
		msg.rbuf = rbuf;

		if(LpssI2CRxData(baddr, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liwet_end;
		}
	}
	else if(i2c->mode[dev_ch] == PCISMN)
	{
		//start to write
		for(i=0; i<len; i++)
		{
			msg.flags = 0x0000;
			msg.slave = dev_addr / 2;
			wbuf[0] = tmp & 0xFF;
			wbuf[1] = buf[i] = value;
			msg.wlen = 2;
			msg.wbuf = wbuf;
			msg.rlen = 0;
			msg.rbuf = NULL;
			if(AMDSMNI2CXfer(dev_ch, gI2CConfig->dev->freq, &msg) != 0)
			{
				Print(L"I2C Tx Fail !! \n\n");
				goto liwet_end;
			}

			usleep(5000);
			
			tmp++;
		}

		usleep(5000);

		//start to read
		msg.flags = 0x0001;
		msg.slave = dev_addr / 2;
		wbuf[0] = dev_cmd;
		msg.wlen = 1;
		msg.wbuf = wbuf;
		msg.rlen = len;
		msg.rbuf = rbuf;
		if(AMDSMNI2CXfer(dev_ch, gI2CConfig->dev->freq, &msg) != 0)
		{
			Print(L"I2C Rx Fail !! \n\n");
			goto liwet_end;
		}
	}

	//print data and compare
	for(i=0; i<len; i++)
	{
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}

		//compare read/write data
		if(buf[i] != rbuf[i])
		{
			console_color(RED, BLACK);
			Print(L"R/W Compare Error!! \n");
			goto liwet_end;
		}
	}
	console_color(GREEN, BLACK);
	Print(L"R/W Compare OK!! \n");

liwet_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}

liwet_endend:
	NULL;
}

void DisplayToolHead(void)
{
	clrscr();
	printf("Copyright(c) , Advantech Corporation 2017-2018\n");
	Print(L"%s ver: v%d.%d \n\n", tool_name, major_v, minor_v);
}

EFI_STATUS main(int argc, char * * argv)
{
	EFI_STATUS status;
	PCI_BUS *pbus = NULL;
	I2C_BUS *i2c = NULL;
	int key;
	uint8_t i;

	status = console_init();
	if(status != EFI_SUCCESS)
	{
		Print(L"ERROR: Failed to change console setting. \n");
		return status;
	}
	
	clrscr();
	
	i2c = LpssI2CInit();
	if(!i2c->found)
	{
		Print(L"Init I2C Fail !! \n\n");
		goto end;
	}
	
	pbus = (PCI_BUS*)malloc(sizeof(PCI_BUS));
	if (!pbus)
	{
		Print(L"Create PCI Buffer Fail !! \n\n");
		goto end;
	}
	
	usleep(100);
	
	if(gI2CConfig == NULL)
	{
		gI2CConfig = (I2CVariableConfig*)malloc(sizeof(I2CVariableConfig));
		if(!gI2CConfig)
		{
			Print(L"I2C Config Data Fail !! \n\n");
			goto end;
		}
		memset(gI2CConfig, 0, sizeof(I2CVariableConfig));

		gI2CConfig->eeprom = (Eeprom*)malloc(sizeof(Eeprom));
		if(gI2CConfig->eeprom)
		{
			memset(gI2CConfig->eeprom, 0, sizeof(Eeprom));
		}
		
		gI2CConfig->thermal = (Thermal*)malloc(sizeof(Thermal));
		if(gI2CConfig->thermal)
		{
			memset(gI2CConfig->thermal, 0, sizeof(Thermal));
		}
		
		gI2CConfig->dio= (DIO*)malloc(sizeof(DIO));
		if(gI2CConfig->dio)
		{
			memset(gI2CConfig->dio, 0, sizeof(DIO));
		}

		gI2CConfig->dev= (Device*)malloc(sizeof(Device));
		if(gI2CConfig->dev)
		{
			memset(gI2CConfig->dev, 0, sizeof(Device));
		}

		I2CTestConfigDefault();
	}
	
	while(1)
	{
		DisplayToolHead();
		
		Print(L"1. List I2C Device Address. \n");
		Print(L"2. Test I2C - EAT Board. \n");
		Print(L"3. Test I2C - DIO Board. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(CheckESC(key))
		{
			if(i2c->vendor == 0x8086)
			{
				//restore pci Configuration - command
				for(i=0; i<8; i++)
				{
					if(i2c->set_pci_cmd[i])
					{
						pci_select_device(pbus, i2c->bus[i], i2c->dev[i], i2c->fun[i]);
						pci_write_byte(pbus, 0x04, i2c->command[i] & 0xFF);
					}
				}
			}
			goto endend;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				LpssI2CScanBus(i2c);
				break;
			}
			case '2':
			{
				I2CEatBoardTest(i2c);
				break;
			}
			case '3':
			{
				I2CDIOBoardTest(i2c);
				break;
			}
			
			/*case '9':
			{
				LpssI2CReadAnotherDeviceTest(i2c);
				break;
			}
			case 'A':
			case 'a':
			{
				LpssI2CWriteAnotherDeviceTest(i2c);
				break;
			}*/
		}
	}
	
end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}

endend:
	if(i2c)
	{
		free(i2c);
	}

	if(pbus)
	{
		free(pbus);
	}

	console_exit();
	
	return EFI_SUCCESS;
}

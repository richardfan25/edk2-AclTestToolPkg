#include "rdctest.h"



RDC_I2C gI2C[2];
I2CVariableConfig *gI2CConfig = NULL;



void I2CTestConfigDefault(void)
{
	gI2CConfig->eeprom->ch = 0;
	gI2CConfig->eeprom->freq = 0;
	gI2CConfig->eeprom->type= 0;
	gI2CConfig->eeprom->addr = 0xA0;
	gI2CConfig->eeprom->spos = 0x00;
	gI2CConfig->eeprom->rwlen = 256;
	gI2CConfig->eeprom->wsval = 0x00;
	
	gI2CConfig->thermal->ch = 0;
	gI2CConfig->thermal->freq= 0;
	gI2CConfig->thermal->addr = 0x98;
	gI2CConfig->thermal->wsval= 85;
	
	gI2CConfig->dio->ch = 0;
	gI2CConfig->dio->freq= 0;
	gI2CConfig->dio->addr = 0x40;
	gI2CConfig->dio->wsval= 0x0000;
}

void EepromConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Eeprom Configuration Page \n\n");
	
	Print(L"Channel(0:I2C0 1:I2C1)   : %d\n", gI2CConfig->eeprom->ch);
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
					
					gotoxy(0, 6);
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
					gI2CConfig->eeprom->spos = value;
					Print(L"%-4X", gI2CConfig->eeprom->spos);
					break;
				}
				case 5:
				{
					gI2CConfig->eeprom->rwlen = value;
					Print(L"%-3d", gI2CConfig->eeprom->rwlen);
					break;
				}
				case 6:
				{
					gI2CConfig->eeprom->wsval = (uint8_t)value;
					Print(L"%-2X", gI2CConfig->eeprom->wsval);
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
					if(gI2CConfig->eeprom->rwlen > 100)
					{
						len = 3;
					}
					else if(gI2CConfig->eeprom->rwlen > 10)
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
			gI2CConfig->eeprom->spos = value;
			break;
		}
		case 5:
		{
			gI2CConfig->eeprom->rwlen = value;
			break;
		}
		case 6:
		{
			gI2CConfig->eeprom->wsval = (uint8_t)value;
			break;
		}
	}
}

void ThermalICConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"Thermal IC Configuration Page \n\n");
	
	Print(L"Channel(0:I2C0 1:I2C1)   : %d\n", gI2CConfig->thermal->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->thermal->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->thermal->addr);
	Print(L"Value(0~127)             : %d\n", gI2CConfig->thermal->wsval);
	
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
			gotoxy(27, item + 2);
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
					if(gI2CConfig->thermal->wsval > 100)
					{
						len = 3;
					}
					else if(gI2CConfig->thermal->wsval > 10)
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

void DIOBoardConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key, value = 0;
	
	clrscr();
	Print(L"DIO Board Configuration Page \n\n");
	
	Print(L"Channel(0:I2C0 1:I2C1)   : %d\n", gI2CConfig->dio->ch);
	Print(L"Frequency(0:100K 1:400K) : %d\n", gI2CConfig->dio->freq);
	Print(L"Address(0x00~0xFF)       : %X\n", gI2CConfig->dio->addr);
	
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
	}
}

uint8_t InitI2C(void)
{
	uint8_t ch, ldn, u8tmp;
	uint16_t u16temp;

	for(ch=0; ch<2; ch++)
	{
		gI2C[ch].Flag.init = 0;
		
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
			ldn = RDC_LDN_I2C0;
		}
		else
		{
			ldn = RDC_LDN_I2C1;
		}
		D_Print(L"I2C %d \n", (uint16_t)ch);

		//Select Logic Device - I2C
		outp(RDC_CFG_IDX_PORT, 0x07);
		outp(RDC_CFG_DAT_PORT, ldn);

		outp(RDC_CFG_IDX_PORT, 0x30);
		u8tmp = inp(RDC_CFG_DAT_PORT);
		if(u8tmp == 0)
		{
			D_Print(L"I2C Logic Device Disable \n");
			continue;
		}
		
		//Get I2C Base Address
		outp(RDC_CFG_IDX_PORT, 0x60);
		gI2C[ch].BaseAddr = inp(RDC_CFG_DAT_PORT);
		gI2C[ch].BaseAddr <<= 8;
		outp(RDC_CFG_IDX_PORT, 0x61);
		gI2C[ch].BaseAddr |= inp(RDC_CFG_DAT_PORT);
		if((gI2C[ch].BaseAddr == 0) || (gI2C[ch].BaseAddr == 0xFFFF))
		{
			D_Print(L"I2C Logic Device Address Error(0x%X) \n", gI2C[ch].BaseAddr);
			continue;
		}

		//Reset I2C
		SET_REG_BM8(I2C_EXCTL_I2CRST_bm, gI2C[ch].BaseAddr + I2C_EXCTL_REG_OFFSET);
		
		//set I2C My_Address 0xFE, avoid scan self
		SET_REG_BM8(I2C_MYADD_MYADDR_gm, gI2C[ch].BaseAddr + I2C_MYADD_REG_OFFSET);
		
		gI2C[ch].Flag.init = 1;

		//Close PNP port
		outp(RDC_CFG_IDX_PORT, 0xAA);	// exit config
		sys_close_ioport(RDC_PNP_INDEX);
		sys_close_ioport(RDC_PNP_DATA);
	}

	if((gI2C[0].Flag.init == 0) && (gI2C[1].Flag.init == 0))
	{
		console_color(RED, BLACK);
		Print(L"I2C Not Available !! \n\n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	
	return 0;
}

void I2CSetClockFrequency(uint8_t ch, uint8_t clk)
{
	uint8_t prescale1, prescale2;

	switch(clk)
	{
		case 0:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			outp(gI2C[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 9;
			prescale2 &= ~I2C_CLK2_FAST_bm;
			outp(gI2C[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);

			gI2C[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case 1:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 25 / 5 = 50000 kHz / 125 = 400 kHz
			//---------------------------------------------------------------------
			prescale1 = 15;
			outp(gI2C[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 7;
			prescale2 |= I2C_CLK2_FAST_bm;
			outp(gI2C[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);
			
			outp(gI2C[0].BaseAddr + I2C_EXCTL_REG_OFFSET, I2C_EXCTL_NOFILTER_bm);
			
			gI2C[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case 2:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 1 = 50000 kHz / 50 = 1000 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			outp(gI2C[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 0;
			prescale2 |= I2C_CLK2_FAST_bm;
			outp(gI2C[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);

			gI2C[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
	}
}

uint8_t I2CWaitTxDone(uint16_t addr, uint8_t p)
{
	uint8_t status;
	uint16_t retry = SMB_WAIT_RETRIES;

	while(retry)
	{
		usleep(1);
		status = inp(addr + I2C_STS_REG_OFFSET);
		if(status & 0x10)
		{
			if(p)
			{
				Print(L"I2C Not Response ACK Error \n");
			}
			return 1;
		}
		else if(status & 0x08)
		{
			if(p)
			{
				Print(L"I2C Arbitration Lost \n");
			}
			return 1;
		}
		else if(status & 0x20)
		{
			//double check, avoid tx done flag set early than not response ack flag
			status = inp(addr + I2C_STS_REG_OFFSET);
			if(status & 0x10)
			{
				if(p)
				{
					Print(L"I2C Not Response ACK Error \n");
				}
				return 1;
			}
			else if(status & 0x08)
			{
				if(p)
				{
					Print(L"I2C Arbitration Lost \n");
				}
				return 1;
			}
			
			break;
		}
		
		retry--;
		if(retry == 0)
		{
			if(p)
			{
				Print(L"I2C Tx Timeout \n");
			}
			return 1;
		}
	}

	//clean tx done status register
	outp(addr + I2C_STS_REG_OFFSET, inp(addr + I2C_STS_REG_OFFSET) | I2C_STS_TXDONE_bm);
	
	return 0;
}

uint8_t I2CWaitRxDone(uint16_t addr)
{
	uint16_t retry = SMB_WAIT_RETRIES;
	
	while(!TST_REG_BM8(I2C_STS_RXRDY_bm, addr + I2C_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"I2C Rx Timeout \n");
			return 1;
		}
		usleep(1);
	}

	//clean rx done status register
	outp(addr + I2C_STS_REG_OFFSET, inp(addr + I2C_STS_REG_OFFSET) | I2C_STS_RXRDY_bm);

	return 0;
}

uint8_t I2CWaitStopDone(uint16_t addr)
{
	uint16_t retry = SMB_WAIT_RETRIES;
	
	while(TST_REG_BM8(I2C_CTL_STOP_bm, addr + I2C_CTL_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"I2C Stop Timeout \n");
			return 1;
		}
		usleep(1);
	}

	return 0;
}

void ListI2CDeviceAddress(void )
{
	uint8_t ch, count = 0;
	uint16_t i;
	
	clrscr();
	Print(L"List I2C Device Address \n\n");

	for(ch=0; ch<2; ch++)
	{
		if(gI2C[ch].Flag.init == 0)
		{
			Print(L"I2C %d Can Not Use \n\n", (uint16_t)ch);
			continue;
		}
		
		count=0;
		Print(L"Scan I2C %d... \n\n", (uint16_t)ch);
		for(i=0x10; i<0xF0; i+=2)
		{
			//clean status register
			outp(gI2C[ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
			
			//send addr
			outp(gI2C[ch].BaseAddr + I2C_TXADD_REG_OFFSET, (uint8_t)i);
			//wait tx done
			if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
			{
				//set stop flag
				outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
				I2CWaitStopDone(gI2C[ch].BaseAddr);
				
				continue;
			}
			
			//set stop flag
			outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
			//wait stop
			if(I2CWaitStopDone(gI2C[ch].BaseAddr) != 0)
			{
				goto list_i2c_end;
			}

			//communication success
			Print(L"Device %d Address : 0x%X \n", count, i);
			gI2C[ch].DevAddr[count] = (uint8_t)i;
			count++;
		}

		gI2C[ch].Cnt = count;
		if(count == 0)
		{
			Print(L"No Device \n\n");
		}
		Print(L"\n\n");
	}

list_i2c_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CReadEepromTest(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;

	clrscr();
	Print(L"I2C - Read Eeprom Test. \n\n");

	eeprom_ch = gI2CConfig->eeprom->ch;
	I2CSetClockFrequency(eeprom_ch, gI2CConfig->eeprom->freq);
	eeprom_type = gI2CConfig->eeprom->type;
	eeprom_addr = gI2CConfig->eeprom->addr;
	eeprom_saddr = gI2CConfig->eeprom->spos;
	len = gI2CConfig->eeprom->rwlen;

	if(gI2C[eeprom_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto ire_end;
	}
	
	//start to read
	for(i=0; i<len; i++)
	{
		//clean status register
		outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
		
		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto ire_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto ire_end;
			}
		}
		
		//send command
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto ire_end;
		}

		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 1);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			Print(L"Send Address Error!! \n\n");
			goto ire_end;
		}

		//dummy read to trigger read process
		inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//set stop flag
		outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);

		//wait rx data ready
		if(I2CWaitRxDone(gI2C[eeprom_ch].BaseAddr) != 0)
		{
			goto ire_end;
		}

		//get data
		rbuf[0] = inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

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

ire_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CWriteEepromTest(uint8_t option)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;
	
	clrscr();
	Print(L"I2C - Write Eeprom Test. \n\n");
	
	eeprom_ch = gI2CConfig->eeprom->ch;
	I2CSetClockFrequency(eeprom_ch, gI2CConfig->eeprom->freq);
	eeprom_type = gI2CConfig->eeprom->type;
	eeprom_addr = gI2CConfig->eeprom->addr;
	eeprom_saddr = gI2CConfig->eeprom->spos;
	len = gI2CConfig->eeprom->rwlen;

	wbuf[0] = gI2CConfig->eeprom->wsval;

	if(gI2C[eeprom_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto iwe_end;
	}
	
	for(i=0; i<len; i++)
	{
		//clean status register
		outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);

		//write data to eeprom
		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto iwe_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto iwe_end;
			}
		}
		
		//send command
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iwe_end;
		}

		//send data
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, wbuf[0]);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iwe_end;
		}

		//set stop flag
		outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
		//wait stop
		if(I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr) != 0)
		{
			goto iwe_end;
		}
		
		//clean tx done status register
		outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET)|I2C_STS_TXDONE_bm);


		usleep(5000);	//must set more than 2ms


		//read data
		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iwe_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto iwe_end;
			}
		}
		
		//send command
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iwe_end;
		}

		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iwe_end;
		}
		
		//dummy read to trigger read process
		inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//set stop flag
		outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
			
		//wait rx data ready
		if(I2CWaitRxDone(gI2C[eeprom_ch].BaseAddr) != 0)
		{
			goto iwe_end;
		}

		//get data
		rbuf[0] = inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		usleep(1000);	// 1ms

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
			goto iwe_end;
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

iwe_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CCheckEEPROMBoundary(void)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t buf[2], bakbuf[2], count = 0;
	uint16_t i, tmp = 0;

	clrscr();
	Print(L"I2C - Check Eeprom Boundary. \n\n");

	eeprom_ch = gI2CConfig->eeprom->ch;
	I2CSetClockFrequency(eeprom_ch, gI2CConfig->eeprom->freq);
	eeprom_type = gI2CConfig->eeprom->type;
	
	if(gI2C[eeprom_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)eeprom_ch);
		goto iceb_end;
	}
	
	//if type is byte mode, search 0xAx total size
	if(eeprom_type == 0)
	{
		for(i=0xA0; i<0xAF; i+=2)
		{
			//clean status register
			outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
			
			//send addr
			outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, (uint8_t)i);
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 0) != 0)
			{
				//set stop flag
				outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
				I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr);
				
				continue;
			}
			
			//set stop flag
			outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
			//wait stop
			if(I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr) != 0)
			{
				goto iceb_end;
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
			goto iceb_end;
		}
	}

	if(eeprom_type)
	{
		eeprom_addr = gI2CConfig->eeprom->addr;
	}

	//clean status register
	outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);

	//backup eeprom last byte
	//send addr
	outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto iceb_end;
	}

	//send command 1
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
	}

	//send addr
	outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 0x01);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}
	
	//dummy read to trigger read process
	inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

	//set stop flag
	outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
		
	//wait rx data ready
	if(I2CWaitRxDone(gI2C[eeprom_ch].BaseAddr) != 0)
	{
		goto iceb_end;
	}

	//get data
	bakbuf[0] = inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

	usleep(5000);	//must set more than 2ms

	//clean status register
	outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
	
	//write data to eeprom
	//send addr
	outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto iceb_end;
	}

	//send command 1
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
	}

	//send data
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0x55);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	//set stop flag
	outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
	//wait stop
	if(I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr) != 0)
	{
		goto iceb_end;
	}
	
	//clean tx done status register
	outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET)|I2C_STS_TXDONE_bm);

	usleep(5000);	//must set more than 2ms
	
	for(i=1; i<8; i++)
	{
		//read data
		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto iceb_end;
		}

		//send command 1
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (0xFF >> i));
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}

		if(eeprom_type)
		{
			//send command 2
			outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto iceb_end;
			}
		}

		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
		
		//dummy read to trigger read process
		inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//set stop flag
		outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
			
		//wait rx data ready
		if(I2CWaitRxDone(gI2C[eeprom_ch].BaseAddr) != 0)
		{
			goto iceb_end;
		}

		//get data
		buf[0] = inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		if(buf[0] != 0x55)
		{
			tmp = i - 1;
			break;
		}
	}
	
	usleep(5000);	//must set more than 2ms
	
	//clean status register
	outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);

	//write data to eeprom
	//send addr
	outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto iceb_end;
	}

	//send command 1
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
	}

	//send data
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xAA);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	//set stop flag
	outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
	//wait stop
	if(I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr) != 0)
	{
		goto iceb_end;
	}
	
	//clean tx done status register
	outp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET)|I2C_STS_TXDONE_bm);

	usleep(5000);	//must set more than 2ms

	for(i=tmp; i>=0; i--)
	{
		//read data
		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto iceb_end;
		}

		//send command 1
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (0xFF >> i));
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}

		if(eeprom_type)
		{
			//send command 2
			outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
			//wait tx done
			if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto iceb_end;
			}
		}

		//send addr
		outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 0x01);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
		
		//dummy read to trigger read process
		inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//set stop flag
		outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
			
		//wait rx data ready
		if(I2CWaitRxDone(gI2C[eeprom_ch].BaseAddr) != 0)
		{
			goto iceb_end;
		}

		//get data
		buf[0] = inp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

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
			goto iceb_end;
		}
	}

	usleep(5000);	//must set more than 2ms

	//restore eeprom last byte
	//send addr
	outp(gI2C[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		console_color(RED, BLACK);
		Print(L"Please Check Eeprom Connect Correctly!! \n\n");
		console_color(LIGHTGRAY, BLACK);
		goto iceb_end;
	}

	//send command 1
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	if(eeprom_type)
	{
		//send command 2
		outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, 0xFF);
		//wait tx done
		if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto iceb_end;
		}
	}

	//send data
	outp(gI2C[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, bakbuf[0]);
	//wait tx done
	if(I2CWaitTxDone(gI2C[eeprom_ch].BaseAddr, 1) != 0)
	{
		goto iceb_end;
	}

	//set stop flag
	outp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
	//wait stop
	if(I2CWaitStopDone(gI2C[eeprom_ch].BaseAddr) != 0)
	{
		goto iceb_end;
	}

iceb_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CEEPROMTest(void)
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

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				EepromConfiguration();
				break;
			}
			case '1':
			{
				I2CReadEepromTest();
				break;
			}
			case '2':
			{
				I2CWriteEepromTest(0);
				break;
			}
			case '3':
			{
				I2CWriteEepromTest(1);
				break;
			}
			case '4':
			{
				I2CCheckEEPROMBoundary();
				break;
			}
		}
	}
}

uint8_t I2CReadRegister(uint8_t ch, uint8_t addr, uint8_t reg, uint8_t *rbuf)
{
	//clean status register
	outp(gI2C[ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
	
	//send addr
	outp(gI2C[ch].BaseAddr + I2C_TXADD_REG_OFFSET, addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto irr_end;
	}

	//send command
	outp(gI2C[ch].BaseAddr + I2C_DAT_REG_OFFSET, reg);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto irr_end;
	}

	//send addr
	outp(gI2C[ch].BaseAddr + I2C_TXADD_REG_OFFSET, addr | 0x01);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto irr_end;
	}

	//dummy read to trigger read process
	inp(gI2C[ch].BaseAddr + I2C_DAT_REG_OFFSET);

	//set stop flag
	outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);

	//wait rx data ready
	if(I2CWaitRxDone(gI2C[ch].BaseAddr) != 0)
	{
		return 1;
	}

	//get data
	*rbuf = inp(gI2C[ch].BaseAddr + I2C_DAT_REG_OFFSET);

	return 0;

irr_end:
	//set stop flag
	outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);

	return 1;
}

uint8_t I2CWriteRegister(uint8_t ch, uint8_t addr, uint8_t reg, uint8_t *wbuf)
{
	//clean status register
	outp(gI2C[ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
	
	//send addr
	outp(gI2C[ch].BaseAddr + I2C_TXADD_REG_OFFSET, addr);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto iwr_end;
	}

	//send command
	outp(gI2C[ch].BaseAddr + I2C_DAT_REG_OFFSET, reg);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto iwr_end;
	}

	//send data
	outp(gI2C[ch].BaseAddr + I2C_DAT_REG_OFFSET, *wbuf);
	//wait tx done
	if(I2CWaitTxDone(gI2C[ch].BaseAddr, 0) != 0)
	{
		goto iwr_end;
	}

	//set stop flag
	outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);
	//wait stop
	if(I2CWaitStopDone(gI2C[ch].BaseAddr) != 0)
	{
		return 1;
	}

	return 0;

iwr_end:
	//set stop flag
	outp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(gI2C[ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);

	return 1;
}

void I2CReadThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"I2C - Read Thermal IC Test. \n\n");

	thermal_ch = gI2CConfig->thermal->ch;
	I2CSetClockFrequency(thermal_ch, gI2CConfig->thermal->freq);
	thermal_addr = gI2CConfig->thermal->addr;

	if(gI2C[thermal_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto irt_end;
	}
	
	//get chip Manufacturer ID
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_MFU_ID, rbuf))
	{
		Print(L"Read Manufacturer ID Fail \n");
		goto irt_end;
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_CHIP_REV, rbuf))
	{
		Print(L"Read Revision ID Fail \n");
		goto irt_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n", tmp);

	//read Local Temp MSB
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_TEMP_H, rbuf))
	{
		Print(L"Read Local Temp MSB Fail \n");
		goto irt_end;
	}
	tmp = rbuf[0];
	
	//read Local Temp LSB
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_TEMP_L, rbuf))
	{
		Print(L"Read Local Temp LSB Fail \n");
		goto irt_end;
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
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OFFSET_H, rbuf))
	{
		Print(L"Read Remote Offset High Byte Fail \n");
		goto irt_end;
	}
	tmp = rbuf[0];

	//read Remote Offset Low Byte
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OFFSET_L, rbuf))
	{
		Print(L"Read Remote Offset Low Byte Fail \n");
		goto irt_end;
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
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
		goto irt_end;
	}
	Print(L"Local T_Crit Limit:  %3d C \n", (uint16_t)rbuf[0]);

	//read Remote OS Limit
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_OS_LMT, rbuf))
	{
		Print(L"Read Remote OS Limit Fail \n");
		goto irt_end;
	}
	Print(L"Remote OS Limit:     %3d C \n", (uint16_t)rbuf[0]);

	//read Remote T_Crit Limit
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_REMOTE_CRIT_LMT, rbuf))
	{
		Print(L"Read Remote T_Crit Limit Fail \n");
		goto irt_end;
	}
	Print(L"Remote T_Crit Limit: %3d C \n", (uint16_t)rbuf[0]);

	//read Common Hysteresis
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_COMM_HYSTERESIS, rbuf))
	{
		Print(L"Read Common Hysteresis Fail \n");
		goto irt_end;
	}
	Print(L"Common Hysteresis:    %2d C \n", (uint16_t)rbuf[0]);

	//read Status Register 1
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_STATUS_1, rbuf))
	{
		Print(L"Read Status Register 1 Fail \n");
		goto irt_end;
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

irt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CWriteThermalICTest(void)
{
	uint8_t thermal_ch = 0, thermal_addr = 0;
	uint8_t wbuf[2], rbuf[2];
	uint16_t tmp;

	clrscr();
	Print(L"I2C - Write Thermal IC Test. \n\n");

	thermal_ch = gI2CConfig->thermal->ch;
	I2CSetClockFrequency(thermal_ch, gI2CConfig->thermal->freq);
	thermal_addr = gI2CConfig->thermal->addr;

	if(gI2C[thermal_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)thermal_ch);
		goto iwt_end;
	}
	
	//get chip Manufacturer ID
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_MFU_ID, rbuf))
	{
		Print(L"Read Manufacturer ID Fail \n");
		goto iwt_end;
	}
	tmp = rbuf[0] << 8;
	
	//get chip Revision ID
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_CHIP_REV, rbuf))
	{
		Print(L"Read Revision ID Fail \n");
		goto iwt_end;
	}
	tmp |= rbuf[0];
	Print(L"Thermal Chip ID: 0x%04X \n\n", tmp);

	//enter new value
	wbuf[0] = gI2CConfig->thermal->wsval;

	//write Local Shared OS and T_Crit Limit
	if(I2CWriteRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, wbuf))
	{
		Print(L"Write Local Shared OS and T_Crit Limit Fail \n");
		goto iwt_end;
	}

	//read Local Shared OS and T_Crit Limit
	if(I2CReadRegister(thermal_ch, thermal_addr, LM_REG_T_CRIT_LMT, rbuf))
	{
		Print(L"Read Local Shared OS and T_Crit Limit Fail \n");
		goto iwt_end;
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

iwt_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void I2CThermalICTest(void)
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

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				ThermalICConfiguration();
				break;
			}
			case '1':
			{
				I2CReadThermalICTest();
				break;
			}
			case '2':
			{
				I2CWriteThermalICTest();
				break;
			}
		}
	}
}

void I2CEatBoardTest(void)
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

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				I2CEEPROMTest();
				break;
			}
			case '2':
			{
				I2CThermalICTest();
				break;
			}
		}
	}
}

void I2CReadDIOBoardTest(void)
{
	uint8_t dio_ch = 0, dio_addr = 0;
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;

	clrscr();
	Print(L"I2C - Read DIO Board Test\n");

	dio_ch = gI2CConfig->dio->ch;
	I2CSetClockFrequency(dio_ch, gI2CConfig->dio->freq);
	dio_addr = gI2CConfig->dio->addr;

	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gI2C[dio_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto irdbt_end;
	}
	
	// set all pins of port0 as input
	u8wb = 0xFF;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Input Fail \n");
		goto irdbt_end;
	}
	// set all pins of port1 as input
	u8wb = 0xFF;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Input Fail \n");
		goto irdbt_end;
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
			goto irdbt_endend;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");

		//read port0 level
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto irdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto irdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto irdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto irdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto irdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto irdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto irdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto irdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto irdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
		{
			Print(L"Read Configuration Port 0 Fail \n");
			goto irdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

irdbt_end:
	while(!CheckESC(GetKey())){}

irdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void I2CWriteDIOBoardTest(void)
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

	dio_ch = gI2CConfig->dio->ch;
	if(gI2C[dio_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto iwdbt_end;
	}
	
	I2CSetClockFrequency(dio_ch, gI2CConfig->dio->freq);
	dio_addr = gI2CConfig->dio->addr;
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gI2C[dio_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto iwdbt_end;
	}
	
	// set all pins of port0 as output
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto iwdbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto iwdbt_end;
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
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Port 0 Level Fail \n");
			goto iwdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		//read port1 level
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Port 1 Level Fail \n");
			goto iwdbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto iwdbt_endend;
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
		if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
		{
			Print(L"Write New Level To Port 0 Fail \n");
			goto iwdbt_end;
		}
		// set new level to port1
		u8wb = (level >> 8) & 0xFF;
		if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
		{
			Print(L"Write New Level To Port 1 Fail \n");
			goto iwdbt_end;
		}

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto iwdbt_end;
		}
		Print(L"Input port registers    : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto iwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
		{
			Print(L"Read Output Port 1 Fail \n");
			goto iwdbt_end;
		}
		Print(L"Output port registers   : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
		{
			Print(L"Read Output Port 0 Fail \n");
			goto iwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
		{
			Print(L"Read Polarity Port 1 Fail \n");
			goto iwdbt_end;
		}
		Print(L"Polarity registers      : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
		{
			Print(L"Read Polarity Port 0 Fail \n");
			goto iwdbt_end;
		}
		Print(L"%02X \n", u8rb);

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
		{
			Print(L"Read Configuration Port 1 Fail \n");
			goto iwdbt_end;
		}
		Print(L"Configuration registers : 0x%02X", u8rb);
		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
		{
			Print(L"Read Configuration Port 0 Fail \n");
			goto iwdbt_end;
		}
		Print(L"%02X \n", u8rb);
	}

iwdbt_end:
	while(!CheckESC(GetKey())){}

iwdbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void I2CAutoDIOBoardTest(void)
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

	dio_ch = gI2CConfig->dio->ch;
	if(gI2C[dio_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto iadbt_end;
	}
	
	I2CSetClockFrequency(dio_ch, gI2CConfig->dio->freq);
	dio_addr = gI2CConfig->dio->addr;
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	gotoxy(0, 3);
	if(gI2C[dio_ch].Flag.init == 0)
	{
		Print(L"I2C %d Can Not Use \n\n", (uint16_t)dio_ch);
		goto iadbt_end;
	}
	
	// set all pins of port0 as output
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8wb))
	{
		Print(L"Set Port 0 As Output Fail \n");
		goto iadbt_end;
	}
	// set all pins of port1 as output
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8wb))
	{
		Print(L"Set Port 1 As Output Fail \n");
		goto iadbt_end;
	}

	// set port0 to low
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
	{
		Print(L"Write Port 0 To Low Fail \n");
		goto iadbt_end;
	}
	// set port1 to low
	u8wb = 0;
	if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
	{
		Print(L"Write Port 1 To Low Fail \n");
		goto iadbt_end;
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

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
		{
			Print(L"Read Input Port 0 Fail \n");
			goto iadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
		{
			Print(L"Read Input Port 1 Fail \n");
			goto iadbt_end;
		}
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(CheckESC(key))
		{
			goto iadbt_endend;
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
			if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8wb))
			{
				Print(L"Write New Level To Port 0 Fail \n");
				goto iadbt_end;
			}
			// set new level to port1
			u8wb = (level >> 8) & 0xFF;
			if(I2CWriteRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8wb))
			{
				Print(L"Write New Level To Port 1 Fail \n");
				goto iadbt_end;
			}

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT1, &u8rb))
			{
				Print(L"Read Input Port 1 Fail \n");
				goto iadbt_end;
			}
			Print(L"Input port registers    : 0x%02X", u8rb);
			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_IN_PORT0, &u8rb))
			{
				Print(L"Read Input Port 0 Fail \n");
				goto iadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT1, &u8rb))
			{
				Print(L"Read Output Port 1 Fail \n");
				goto iadbt_end;
			}
			Print(L"Output port registers   : 0x%02X", u8rb);
			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_OUT_PORT0, &u8rb))
			{
				Print(L"Read Output Port 0 Fail \n");
				goto iadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT1, &u8rb))
			{
				Print(L"Read Polarity Port 1 Fail \n");
				goto iadbt_end;
			}
			Print(L"Polarity registers      : 0x%02X", u8rb);
			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_INV_PORT0, &u8rb))
			{
				Print(L"Read Polarity Port 0 Fail \n");
				goto iadbt_end;
			}
			Print(L"%02X \n", u8rb);

			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG1, &u8rb))
			{
				Print(L"Read Configuration Port 1 Fail \n");
				goto iadbt_end;
			}
			Print(L"Configuration registers : 0x%02X", u8rb);
			if(I2CReadRegister(dio_ch, dio_addr, DIO_REG_CONFIG0, &u8rb))
			{
				Print(L"Read Configuration Port 0 Fail \n");
				goto iadbt_end;
			}
			Print(L"%02X \n", u8rb);

			bit++;
		}
	}

iadbt_end:
	while(!CheckESC(GetKey())){}

iadbt_endend:
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}

void I2CDIOBoardTest(void)
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

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '0':
			{
				DIOBoardConfiguration();
				break;
			}
			case '1':
			{
				I2CReadDIOBoardTest();
				break;
			}
			case '2':
			{
				I2CWriteDIOBoardTest();
				break;
			}
			case '3':
			{
				I2CAutoDIOBoardTest();
				break;
			}
		}
	}
}

void I2CTest(void)
{
	int key;

	clrscr();
	if(InitI2C())
	{
		goto it_end;
	}
	usleep(100);
	
	if(gI2CConfig == NULL)
	{
		gI2CConfig = (I2CVariableConfig*)malloc(sizeof(I2CVariableConfig));
		if(!gI2CConfig)
		{
			Print(L"I2C Config Data Fail !! \n\n");
			goto it_end;
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

		I2CTestConfigDefault();
	}
	
	while(1)
	{
		clrscr();
		Print(L"1. List I2C Device Address. \n");
		Print(L"2. Test I2C - EAT Board. \n");
		Print(L"3. Test I2C - DIO Board. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			if(gI2C[0].Flag.init == 1)
			{
				//Reset I2C to reset RDC I2C slave address
				outp(gI2C[0].BaseAddr + I2C_EXCTL_REG_OFFSET, I2C_EXCTL_I2CRST_bm);
			}
			if(gI2C[1].Flag.init == 1)
			{
				//Reset I2C to reset RDC I2C slave address
				outp(gI2C[1].BaseAddr + I2C_EXCTL_REG_OFFSET, I2C_EXCTL_I2CRST_bm);
			}
			goto it_endend;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ListI2CDeviceAddress();
				break;
			}
			case '2':
			{
				I2CEatBoardTest();
				break;
			}
			case '3':
			{
				I2CDIOBoardTest();
				break;
			}
		}
	}

it_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

it_endend:
	NULL;
}


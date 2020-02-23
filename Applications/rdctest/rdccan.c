#include "rdctest.h"



RDC_CANBUS gCAN[2];
CanVariableConfig *gCANConfig = NULL;



void CanTestConfigDefault(void)
{
	gCANConfig->can->txch = 0;
	gCANConfig->can->rxch = 0;
	gCANConfig->can->freq = 0;
	gCANConfig->can->format = 0;
	gCANConfig->can->mode = 0;
	gCANConfig->can->id = 0;
	gCANConfig->can->length = 0;
	gCANConfig->can->data = 0x00;
	gCANConfig->can->loop = 1;
}

void CANBusConfiguration(void)
{
	uint8_t buf[20];
	uint8_t len = 0, t_len = 0;
	uint8_t item = 0, item_bak = 0xFF;
	uint16_t key;
	uint32_t value = 0;
	
	clrscr();
	Print(L"CAN Configuration Page \n\n");
	
	Print(L"TxChannel(0:CAN0 1:CAN1)      : %d\n", gCANConfig->can->txch);
	Print(L"RxChannel(0:CAN0 1:CAN1)      : %d\n", gCANConfig->can->rxch);
	//Print(L"Frequncy(0:1000K 1:833.3K 2:500K 3:250K 4:125K 5:100K 6:62.5K 7:50K 8:25K 9:10K) : %d\n", gCANConfig->can->freq);
	Print(L"Frequncy(0. 1000 Kbps\n");
	Print(L"         1. 833.333 Kbps\n");
	Print(L"         2. 500 Kbps\n");
	Print(L"         3. 250 Kbps\n");
	Print(L"         4. 125 Kbps\n");
	Print(L"         5. 100 Kbps\n");
	Print(L"         6. 62.5 Kbps\n");
	Print(L"         7. 50 Kbps\n");
	Print(L"         8. 25 Kbps\n");
	Print(L"         9. 10 Kbps)          : %d\n", gCANConfig->can->freq);
	Print(L"Format(0:Standard 1:Extended) : %d\n", gCANConfig->can->format);
	Print(L"Mode(0:Normal 1:Increase)     : %d\n", gCANConfig->can->mode);

	if(gCANConfig->can->format)
	{
		Print(L"ID(0~268435455)               : %-9d\n", gCANConfig->can->id);
	}
	else
	{
		Print(L"ID(0~2047)                    : %-9d\n", gCANConfig->can->id);
	}
	
	Print(L"Length(0~8 Byte)              : %d\n", gCANConfig->can->length);
	Print(L"Data(0x00~0xFF)               : %X\n", gCANConfig->can->data);
	Print(L"Loop(1~99999)                 : %d\n", gCANConfig->can->loop);
	
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
			if(item < 8)
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
				if(item > 1)
				{
					gotoxy(32 + len, item + 2 + 9);
				}
				else
				{
					gotoxy(32 + len, item + 2);
				}
				Print(L" ");
				
				switch(item)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 8:
					{
						value = value / 10;
						break;
					}
					case 7:
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
			if(item > 1)
			{
				gotoxy(32 + len, item + 2 + 9);
			}
			else
			{
				gotoxy(32 + len, item + 2);
			}
			
			switch(item)
			{
				case 0:
				case 1:
				case 3:
				case 4:
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
					if((key >= '0') && (key <= '9'))
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
						if(len < t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(gCANConfig->can->format)
							{
								if(value > 268435455)
								{
									value = 268435455;
								}
							}
							else
							{
								if(value > 2047)
								{
									value = 2047;
								}
							}
						}
					}
					break;
				}
				case 6:
				{
					if((key >= '0') && (key <= '8'))
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
				case 7:
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
				case 8:
				{
					if((key >= '0') && (key <= '9'))
					{
						if(len != t_len)
						{
							len++;
							Print(L"%c", key);
							
							value = (value * 10) + (key - '0');
							if(value < 1)
							{
								value = 1;
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
			if(item_bak > 1)
			{
				gotoxy(32, item_bak + 2 + 9);
			}
			else
			{
				gotoxy(32, item_bak + 2);
			}
			console_color(LIGHTGRAY, BLACK);
			switch(item_bak)
			{
				case 0:
				{
					gCANConfig->can->txch = (uint8_t)value;
					Print(L"%d", gCANConfig->can->txch);
					break;
				}
				case 1:
				{
					gCANConfig->can->rxch = (uint8_t)value;
					Print(L"%d", gCANConfig->can->rxch);
					break;
				}
				case 2:
				{
					gCANConfig->can->freq = (uint8_t)value;
					Print(L"%d", gCANConfig->can->freq);
					break;
				}
				case 3:
				{
					gCANConfig->can->format = (uint8_t)value;
					Print(L"%d", gCANConfig->can->format);

					gotoxy(0, 16);
					if(gCANConfig->can->format)
					{
						Print(L"ID(0~268435455)               : %-9d\n", gCANConfig->can->id);
					}
					else
					{
						Print(L"ID(0~2047)                    : %-9d\n", gCANConfig->can->id);
					}
					
					break;
				}
				case 4:
				{
					gCANConfig->can->mode = (uint8_t)value;
					Print(L"%d", gCANConfig->can->mode);
					break;
				}
				case 5:
				{
					gCANConfig->can->id = value;
					Print(L"%-9d", gCANConfig->can->id);
					break;
				}
				case 6:
				{
					gCANConfig->can->length = (uint8_t)value;
					Print(L"%d", gCANConfig->can->length);
					break;
				}
				case 7:
				{
					gCANConfig->can->data = (uint8_t)value;
					Print(L"%X", gCANConfig->can->data);
					break;
				}
				case 8:
				{
					gCANConfig->can->loop = value;
					Print(L"%d", gCANConfig->can->loop);
					break;
				}
			}
			
			//set select item color to yellow
			if(item > 1)
			{
				gotoxy(32, item + 2 + 9);
			}
			else
			{
				gotoxy(32, item + 2);
			}
			console_color(YELLOW, BLACK);
			switch(item)
			{
				case 0:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->txch;
					Print(L"%d", value);
					break;
				}
				case 1:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->rxch;
					Print(L"%d", value);
					break;
				}
				case 2:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->freq;
					Print(L"%d", value);
					break;
				}
				case 3:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->format;
					Print(L"%d", value);
					break;
				}
				case 4:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->mode;
					Print(L"%d", value);
					break;
				}
				case 5:
				{
					sprintf(buf, "%d", gCANConfig->can->id);
					len = (uint8_t)strlen(buf);
					
					if(gCANConfig->can->format)
					{
						t_len = 9;
					}
					else
					{
						t_len = 4;
					}
					
					value = gCANConfig->can->id;
					Print(L"%-9d", value);
					break;
				}
				case 6:
				{
					len = 1;
					t_len = 1;
					value = gCANConfig->can->length;
					Print(L"%d", value);
					break;
				}
				case 7:
				{
					sprintf(buf, "%X", gCANConfig->can->data);
					len = (uint8_t)strlen(buf);
					
					t_len = 2;
					value = gCANConfig->can->data;
					Print(L"%-2X", value);
					break;
				}
				case 8:
				{
					sprintf(buf, "%d", gCANConfig->can->loop);
					len = (uint8_t)strlen(buf);
					
					t_len = 5;
					value = gCANConfig->can->loop;
					Print(L"%d", value);
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
			gCANConfig->can->txch = (uint8_t)value;
			break;
		}
		case 1:
		{
			gCANConfig->can->rxch = (uint8_t)value;
			break;
		}
		case 2:
		{
			gCANConfig->can->freq = (uint8_t)value;
			break;
		}
		case 3:
		{
			gCANConfig->can->format = (uint8_t)value;
			break;
		}
		case 4:
		{
			gCANConfig->can->mode = (uint8_t)value;
			break;
		}
		case 5:
		{
			gCANConfig->can->id = value;
			break;
		}
		case 6:
		{
			gCANConfig->can->length = (uint8_t)value;
			break;
		}
		case 7:
		{
			gCANConfig->can->data = (uint8_t)value;
			break;
		}
		case 8:
		{
			gCANConfig->can->loop = value;
			break;
		}
	}
}

uint8_t InitCANBus(void)
{
	uint8_t ch, ldn, u8tmp;
	uint16_t u16temp;

	for(ch=0; ch<2; ch++)
	{
		gCAN[ch].Flag.init = 0;
		
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
			ldn = RDC_LDN_CAN0;
		}
		else
		{
			ldn = RDC_LDN_CAN1;
		}
		D_Print(L"Can Bus %d \n", (uint16_t)ch);

		
		//Select Logic Device - CAN Bus
		outp(RDC_CFG_IDX_PORT, 0x07);
		outp(RDC_CFG_DAT_PORT, ldn);

		outp(RDC_CFG_IDX_PORT, 0x30);
		u8tmp = inp(RDC_CFG_DAT_PORT);
		if(u8tmp == 0)
		{
			D_Print(L"CAN Bus Logic Device Disable \n");
			continue;
		}
		
		//Get CAN Bus Base Address
		outp(RDC_CFG_IDX_PORT, 0x60);
		gCAN[ch].BaseAddr = inp(RDC_CFG_DAT_PORT);
		gCAN[ch].BaseAddr <<= 8;
		outp(RDC_CFG_IDX_PORT, 0x61);
		gCAN[ch].BaseAddr |= inp(RDC_CFG_DAT_PORT);
		if((gCAN[ch].BaseAddr == 0) || (gCAN[ch].BaseAddr == 0xFFFF))
		{
			D_Print(L"CAN Bus Logic Device Address Error(0x%X) \n", gCAN[ch].BaseAddr);
			continue;
		}

		//Reset CAN Bus
		SET_REG_BM32(CAN_GLB_CTL_RST_bm, gCAN[ch].BaseAddr + CAN_GLB_CTL);

		gCAN[ch].Flag.init = 1;
		
		//Close PNP port
		outp(RDC_CFG_IDX_PORT, 0xAA);	// exit config
		sys_close_ioport(RDC_PNP_INDEX);
		sys_close_ioport(RDC_PNP_DATA);
	}

	if((gCAN[0].Flag.init == 0) && (gCAN[1].Flag.init == 0))
	{
		console_color(RED, BLACK);
		Print(L"CAN Bus Not Available !! \n\n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	
	return 0;
}

void CANBusSetBaudrate(void)
{
	uint8_t br, ch;
	uint16_t retry = 3000;

	br = gCANConfig->can->freq;
	
	for(ch=0; ch<2; ch++)
	{
		if(gCAN[ch].Flag.init == 0)
		{
			D_Print(L"CAN Bus %d Can Not Use \n\n", ch);
			continue;
		}
		
		//Set CAN Bus controller Deactivate
		CLR_REG_BM32(CAN_GLB_CTL_CBA_bm, gCAN[ch].BaseAddr + CAN_GLB_CTL);

		//wait CAN Bus controller Deactivate
		while(retry)
		{
			if(!TST_REG_BM32(CAN_GLB_CTL_CBA_bm, gCAN[ch].BaseAddr + CAN_GLB_CTL))
			{
				break;
			}

			retry--;
			if(retry == 0)
			{
				continue;
			}
		}

		switch(br)
		{
			case 0:		//1000 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000330);
				gCAN[ch].Freq = 1000;
				break;
			}
			case 1:		//833.333 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000440);
				gCAN[ch].Freq = 833;
				break;
			}
			case 2:		//500 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 500;
				break;
			}
			case 3:		//250 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000001);	//CKDIV = 4
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 250;
				break;
			}
			case 4:		//125 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000003);	//CKDIV = 8
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 125;
				break;
			}
			case 5:		//100 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000004);	//CKDIV = 10
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 100;
				break;
			}
			case 6:		//62.5 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000007);	//CKDIV = 16
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 62;
				break;
			}
			case 7:		//50 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000009);	//CKDIV = 20
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 50;
				break;
			}
			case 8:		//25 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000013);	//CKDIV = 40
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 25;
				break;
			}
			case 9:		//10 Kbps
			{
				outpd(gCAN[ch].BaseAddr + CAN_CLK_PRE, 0x00000031);	//CKDIV = 100
				outpd(gCAN[ch].BaseAddr + CAN_BUS_TIM, 0x00000772);
				gCAN[ch].Freq = 10;
				break;
			}
		}
	}
}

void CANBusSetTxFrameControlID(uint8_t ch)
{
	uint32_t tmp;

	tmp = (gCANConfig->can->length << 4) | gCANConfig->can->format;
	outpd(gCAN[ch].BaseAddr + CAN_TXD_CTL0, tmp);
	//Print(L"Set TX Frame Control \n");

	outpd(gCAN[ch].BaseAddr + CAN_TXD_IDF0, gCANConfig->can->id);
	//Print(L"Set TX Frame ID \n");
}

void CANBusSetTxData(uint8_t ch, uint8_t index)
{
	uint32_t tmp;
	
	//Set TX Data
	//High
	for(tmp=0; index>=4; index--)
	{
		tmp <<= 8;
		tmp |= *(gCAN[ch].wbuf + index);
	}
	outpd(gCAN[ch].BaseAddr + CAN_TXD_DAH0, tmp);

	//Low
	for(tmp=0; index>=0; index--)
	{
		tmp <<= 8;
		tmp |= *(gCAN[ch].wbuf + index);
		if(index == 0)
		{
			break;
		}
	}
	outpd(gCAN[ch].BaseAddr + CAN_TXD_DAL0, tmp);
}

uint8_t CANBusSetGlobalControlActive(uint8_t ch)
{
	uint16_t retry = 3000;

	if(gCAN[ch].Flag.init == 0)
	{
		Print(L"CAN Bus %d Can Not Use \n\n", ch);
		return 1;
	}
	//Set CAN Bus controller Active
	SET_REG_BM32(CAN_GLB_CTL_CBA_bm, gCAN[ch].BaseAddr + CAN_GLB_CTL);

	//wait CAN Bus controller Active
	while(retry)
	{
		if(TST_REG_BM32(CAN_GLB_CTL_CBA_bm, gCAN[ch].BaseAddr + CAN_GLB_CTL))
		{
			break;
		}

		retry--;
		if(retry == 0)
		{
			return 1;
		}
	}
	
	return 0;
}

void CANBusGetRxFrame(uint8_t ch)
{
	uint8_t i;
	uint32_t tmp;

	//get RX Frame Type
	tmp = inpd(gCAN[ch].BaseAddr + CAN_RXD_FRM);
	gCAN[ch].format = (uint8_t)tmp & 0x01;
	gCAN[ch].length = ((uint8_t)tmp >> 4) & 0x0F;

	//get RX ID
	gCAN[ch].id = inpd(gCAN[ch].BaseAddr + CAN_RXD_IDF);

	//get RX Data
	tmp = inpd(gCAN[ch].BaseAddr + CAN_RXD_DAL);
	for(i=0; i<gCAN[ch].length; i++)
	{
		if(i==4)
		{
			tmp = inpd(gCAN[ch].BaseAddr + CAN_RXD_DAH);
		}
		gCAN[ch].rbuf[i] = tmp & 0xFF;
		tmp >>= 8;
	}

	//set Request to release Receive Status
	SET_REG_BM32(CAN_REQ_CTL_RRB_bm, gCAN[ch].BaseAddr + CAN_REQ_CTL);
}

uint8_t CANBusCompareModeLengthIDData(uint8_t ch)
{
	uint8_t i;
	uint8_t rxch=1-ch;

	if(gCAN[ch].format != gCAN[rxch].format)
	{
		Print(L"Mode : W[%s] R[%s] \n", gCAN[ch].format?L"Extended":L"Standard", gCAN[rxch].format?L"Extended":L"Standard");
		console_color(RED, BLACK);
		Print(L"Compare Mode Fail !! \n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	else
	{
		Print(L"Mode : W[%s] = R[%s] \n", gCAN[ch].format?L"Extended":L"Standard", gCAN[rxch].format?L"Extended":L"Standard");
	}

	if(gCAN[ch].id != gCAN[rxch].id)
	{
		Print(L"ID : W[%d] R[%d] \n", gCAN[ch].id, gCAN[rxch].id);
		console_color(RED, BLACK);
		Print(L"Compare ID Fail !! \n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	else
	{
		Print(L"ID : W[%d] = R[%d] \n", gCAN[ch].id, gCAN[rxch].id);
	}

	if(gCAN[ch].length != gCAN[rxch].length)
	{
		Print(L"Length : W[%d] R[%d] \n", gCAN[ch].length, gCAN[rxch].length);
		console_color(RED, BLACK);
		Print(L"Compare Length Fail !! \n");
		console_color(LIGHTGRAY, BLACK);
		return 1;
	}
	else
	{
		Print(L"Length : W[%d] = R[%d] \n", gCAN[ch].length, gCAN[rxch].length);
	}

	for(i=0; i<gCAN[0].length; i++)
	{
		if(gCAN[ch].wbuf[i] != gCAN[rxch].rbuf[i])
		{
			Print(L"Data %d : W[%x] R[%x] ", (i + 1), gCAN[ch].wbuf[i], gCAN[rxch].rbuf[i]);
			console_color(RED, BLACK);
			Print(L"Compare Data Fail !! \n");
			console_color(LIGHTGRAY, BLACK);
			return 1;
		}
		else
		{
			Print(L"Data %d : W[%x] = R[%x] \n", (i + 1), gCAN[ch].wbuf[i], gCAN[rxch].rbuf[i]);
		}
	}

	return 0;
}

uint8_t CANBusWaitTxComplete(uint8_t ch)
{
	uint8_t ret = 0;
	uint16_t retry = 3000;
	uint32_t status;

	while(retry)
	{
		usleep(1);
		status = inpd(gCAN[ch].BaseAddr + CAN_INT_STS);
		if(status & (/*CAN_INT_STS_RBOI_bm | */CAN_INT_STS_RBEI_bm | CAN_INT_STS_ALI_bm | CAN_INT_STS_BOI_bm | CAN_INT_STS_EPI_bm | CAN_INT_STS_ECI_bm))	//check error
		{
			/*if(status & CAN_INT_STS_RBOI_bm)
			{
				Print(L"Receive buffer is overrun. \n");
			}*/
			if(status & CAN_INT_STS_RBEI_bm)
			{
				Print(L"Bus receive error occurs. \n");
			}
			if(status & CAN_INT_STS_ALI_bm)
			{
				Print(L"The controller loses arbitration. \n");
			}
			if(status & CAN_INT_STS_BOI_bm)
			{
				Print(L"The controller enters bus off. \n");
			}
			if(status & CAN_INT_STS_EPI_bm)
			{
				Print(L"The controller is at the error passive state. \n");
			}
			if(status & CAN_INT_STS_ECI_bm)
			{
				Print(L"The error counter value is greater than or equal to the warning limit value. \n");
			}

			ret = 1;
			goto cbwtc_end;
		}
		else if(status & CAN_INT_STS_TX0I_bm)	//check completed
		{
			status = inpd(gCAN[ch].BaseAddr + CAN_TXD_STS0);
			if((status & (CAN_TXD_STS0_BEO0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_BEO0_bm | CAN_TXD_STS0_TRC0_bm))	//Bus error occur 0
			{
				Print(L"CAN Bus %d Tx Error !! (%x)\n", ch, status);
				switch((status & CAN_TXD_STS0_BEC0_gm) >> 16)	//Bus Error Type
				{
					case 1:
					{
						Print(L"Bus Error - Bit Error\n");
						break;
					}
					case 2:
					{
						Print(L"Bus Error - Stuff Error\n");
						break;
					}
					case 3:
					{
						Print(L"Bus Error - CRC Error\n");
						break;
					}
					case 4:
					{
						Print(L"Bus Error - Form Error\n");
						break;
					}
					case 5:
					{
						Print(L"Bus Error - Acknowledge Error\n");
						break;
					}
				}
				
				//status = inpd(can[ch].BaseAddr + CAN_GLB_STS);
				//Print(L"CtrlSts = %x \n", status);

				status = inpd(gCAN[ch].BaseAddr + CAN_INT_STS);
				if(status & (1 << 16))
				{
					Print(L"Bus error occurs at receiving. (%x)\n", status);
				}
				else
				{
					Print(L"Bus error occurs at transmitting. (%x)\n", status);
				}
				
				ret = 2;
				goto cbwtc_end;
			}
			else if((status & (CAN_TXD_STS0_ALO0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_ALO0_bm | CAN_TXD_STS0_TRC0_bm))	//Arbitration Lost
			{
				Print(L"Arbitration Lost !! \n");

				ret = 3;
				goto cbwtc_end;
			}
			else if((status & (CAN_TXD_STS0_TC0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_TC0_bm | CAN_TXD_STS0_TRC0_bm))	//Transmission completed
			{
				ret = 0;
				goto cbwtc_end;
			}
		}

		retry--;
		if(retry == 0)
		{
			Print(L"CAN Bus %d Tx Timeout !! \n", ch);
			Print(L"IntSts=%x\n", inpd(gCAN[ch].BaseAddr + CAN_INT_STS));
			Print(L"TxSts0=%x\n", inpd(gCAN[ch].BaseAddr + CAN_TXD_STS0));
			
			ret = 4;
			goto cbwtc_end;
		}
	}
	
cbwtc_end:
	//clean all Interrupt Status
	outpd(gCAN[ch].BaseAddr + CAN_INT_STS, 0x7FF);
	
	return ret;
}

uint8_t CANBusWaitRxComplete(uint8_t ch, uint8_t p)
{
	uint8_t ret = 0;
	uint16_t retry = 3000;
	uint32_t status;
	
	while(retry)
	{
		usleep(1);
		status = inpd(gCAN[ch].BaseAddr + CAN_INT_STS);
		if(status & (/*CAN_INT_STS_RBOI_bm | */CAN_INT_STS_RBEI_bm | CAN_INT_STS_ALI_bm | CAN_INT_STS_BOI_bm | CAN_INT_STS_EPI_bm | CAN_INT_STS_ECI_bm))	//check error
		{
			/*if(status & CAN_INT_STS_RBOI_bm)
			{
				Print(L"Receive buffer is overrun. \n");
			}*/
			if(status & CAN_INT_STS_RBEI_bm)
			{
				Print(L"Bus receive error occurs. \n");
			}
			if(status & CAN_INT_STS_ALI_bm)
			{
				Print(L"The controller loses arbitration. \n");
			}
			if(status & CAN_INT_STS_BOI_bm)
			{
				Print(L"The controller enters bus off. \n");
			}
			if(status & CAN_INT_STS_EPI_bm)
			{
				Print(L"The controller is at the error passive state. \n");
			}
			if(status & CAN_INT_STS_ECI_bm)
			{
				Print(L"The error counter value is greater than or equal to the warning limit value. \n");
			}

			ret = 1;
			goto cbwrc_end;
		}
		else if(status & CAN_INT_STS_RXI_bm)	//check receive
		{
			status = inpd(gCAN[ch].BaseAddr + CAN_RCV_STS);
			if(status & CAN_RCV_STS_BEOR_bm)	//Bus error occur
			{
				Print(L"CAN Bus %d Rx Error !! (%x)\n", ch, status);
				switch((status & CAN_RCV_STS_RBEC_gm) >> 4)	//Bus Error Type
				{
					case 1:
					{
						Print(L"Bus Error - Bit Error\n");
						break;
					}
					case 2:
					{
						Print(L"Bus Error - Stuff Error\n");
						break;
					}
					case 3:
					{
						Print(L"Bus Error - CRC Error\n");
						break;
					}
					case 4:
					{
						Print(L"Bus Error - Form Error\n");
						break;
					}
					case 5:
					{
						Print(L"Bus Error - Acknowledge Error\n");
						break;
					}
				}
				
				status = inpd(gCAN[ch].BaseAddr + CAN_INT_STS);
				if(status & (1 << 16))
				{
					Print(L"Bus error occurs at receiving. (%x)\n", status);
				}
				else
				{
					Print(L"Bus error occurs at transmitting. (%x)\n", status);
				}
				
				ret = 2;
				goto cbwrc_end;
			}
			else if(status & CAN_RCV_STS_RBO_bm)	//Receive Buffer Overrun
			{
				if(p)
				{
					Print(L"Receive Buffer Overrun !! (%x)\n", status);
				}
				ret = 0;
				goto cbwrc_end;
			}
			else if(status & CAN_RCV_STS_RBS_bm)	//there are pending messages in the receive buffer
			{
				ret = 0;
				goto cbwrc_end;
			}
		}

		retry--;
		if(retry == 0)
		{
			if(p)
			{
				Print(L"CAN Bus %d Rx Timeout !! \n", ch);
				Print(L"IntSts=%x\n", inpd(gCAN[ch].BaseAddr + CAN_INT_STS));
				Print(L"RxSts0=%x\n", inpd(gCAN[ch].BaseAddr + CAN_RCV_STS));
			}
			ret = 4;
			goto cbwrc_end;
		}
	}

cbwrc_end:
	//clean all Interrupt Status
	outpd(gCAN[ch].BaseAddr + CAN_INT_STS, 0x7FF);
	
	return ret;
}

void CANBusCANToCANTest(uint8_t ch)
{
	uint32_t i, loop;
	uint8_t wbuf[8], rbuf[8];
	uint8_t j, idauto, rxch = 1 - ch;

	clrscr();
	Print(L"CAN Bus Test - CAN %d -> CAN %d \n\n", ch, rxch);
	
	if(gCAN[ch].Flag.init == 0)
	{
		Print(L"Can Bus %d Can Not Use \n", (uint16_t)ch);
		goto cbctct_end;
	}
	if(gCAN[rxch].Flag.init == 0)
	{
		Print(L"Can Bus %d Can Not Use \n", (uint16_t)rxch);
		goto cbctct_end;
	}
	
	//clean all Interrupt Status
	outpd(gCAN[ch].BaseAddr + CAN_INT_STS, 0x7FF);
	
	CANBusSetBaudrate();
	
	gCAN[ch].format = gCANConfig->can->format;
	gCAN[ch].id = gCANConfig->can->id;
	gCAN[ch].length = gCANConfig->can->length;
	
	wbuf[0] = gCANConfig->can->data;
	gCAN[ch].wbuf = wbuf;
	gCAN[ch].rbuf = NULL;

	gCAN[rxch].wbuf = NULL;
	gCAN[rxch].rbuf = rbuf;

	//set can bus mode id length
	CANBusSetTxFrameControlID(ch);
	
	idauto = gCANConfig->can->mode;
	loop = gCANConfig->can->loop;
	
	clrscr();
	Print(L"CAN Bus Test - CAN %d -> CAN %d \n\n", ch, rxch);
	ConsoleCurrRow = 2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Format: %s  ", gCAN[ch].format?L"Extended":L"Standard");
	gotoxy(20, ConsoleCurrRow);
	Print(L"Baudrate: %d K  ", gCAN[ch].Freq);
	gotoxy(40, ConsoleCurrRow);
	Print(L"ID: %-9d\n", gCAN[ch].id);
	gotoxy(55, ConsoleCurrRow);
	Print(L"Loop: %d  ", loop);
	ConsoleCurrRow++;
	
	gotoxy(0, ConsoleCurrRow);
	Print(L"Length: %d  ", gCAN[ch].length);
	gotoxy(20, ConsoleCurrRow);
	Print(L"Data: ");
	for(j=0; j<gCAN[ch].length; j++)
	{
		Print(L"%02x ", gCAN[ch].wbuf[0] + j);
	}
	Print(L"\n\n");
	ConsoleCurrRow += 3;
	
	//set can bus active
	CANBusSetGlobalControlActive(ch);
	CANBusSetGlobalControlActive(rxch);
	
	for(i=0; i<loop; i++)
	{
		//increase mode, renew data
		if(i > 0)
		{
			gCAN[ch].wbuf[0] += gCAN[ch].length;
		}
		for(j=1; j<gCAN[ch].length; j++)
		{
			gCAN[ch].wbuf[j] = gCAN[ch].wbuf[0] + j;
		}
		CANBusSetTxData(ch, gCAN[ch].length);
		
		//gotoxy(0, ConsoleCurrRow);
		
		//Set Tx Request
		SET_REG_BM32(CAN_REQ_CTL_TBR0_bm, gCAN[ch].BaseAddr + CAN_REQ_CTL);
		
		//wait tx complete
		if(CANBusWaitTxComplete(ch))
		{
			goto cbctct_end;
		}
		
		//wait rx complete
		if(CANBusWaitRxComplete(rxch, 1))
		{
			goto cbctct_end;
		}
		
		//get rx data
		CANBusGetRxFrame(rxch);

		//compare mode length id data
		if(CANBusCompareModeLengthIDData(ch))
		{
			console_color(RED, BLACK);
			Print(L"Loop %d Compare Fail !! \n", i + 1);
			console_color(LIGHTGRAY, BLACK);
			goto cbctct_end;
		}
		else
		{
			console_color(GREEN, BLACK);
			Print(L"Loop %d Compare Success !! \n", i + 1);
			console_color(LIGHTGRAY, BLACK);
		}

		//if increase mode, renew id
		if(idauto)
		{
			gCAN[ch].id++;
			if(gCAN[ch].format)
			{
				if(gCAN[ch].id > 268435455)
				{
					gCAN[ch].id = 0;
				}
			}
			else
			{
				if(gCAN[ch].id > 2047)
				{
					gCAN[ch].id = 0;
				}
			}
			outpd(gCAN[ch].BaseAddr + CAN_TXD_IDF0, gCAN[ch].id);
		}
	}

cbctct_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void CANBusTransmitterTest(void)
{
	uint32_t i, loop;
	uint8_t wbuf[8];
	uint8_t j, ch, idauto;

	clrscr();
	Print(L"CAN Bus Test - CAN Bus Transmitter \n\n");

	ch = gCANConfig->can->txch;
	if(gCAN[ch].Flag.init == 0)
	{
		Print(L"CAN Bus %d Can Not Use \n\n", ch);
		goto cbtt_end;
	}
	
	//clean all Interrupt Status
	outpd(gCAN[ch].BaseAddr + CAN_INT_STS, 0x7FF);
	
	CANBusSetBaudrate();
	
	gCAN[ch].format = gCANConfig->can->format;
	gCAN[ch].id = gCANConfig->can->id;
	gCAN[ch].length = gCANConfig->can->length;

	clrscr();
	Print(L"CAN Bus Test - CAN Bus Transmitter \n\n");
	ConsoleCurrRow = 2;

	gCAN[ch].wbuf = wbuf;
	gCAN[ch].rbuf = NULL;

	//set can bus mode id length
	CANBusSetTxFrameControlID(ch);
	
	idauto = gCANConfig->can->mode;
	loop = gCANConfig->can->loop;

	//set can bus active
	CANBusSetGlobalControlActive(ch);

	for(i=1; i<=loop; i++)
	{
		//increase mode, renew data
		if(i > 1)
		{
			gCAN[ch].wbuf[0] += gCAN[ch].length;
		}
		for(j=1; j<gCAN[ch].length; j++)
		{
			gCAN[ch].wbuf[j] = gCAN[ch].wbuf[0] + j;
		}
		CANBusSetTxData(ch, gCAN[ch].length);
		
		gotoxy(0, ConsoleCurrRow);
		Print(L"%-5d. Mode: %s  ", i, gCAN[ch].format?L"Extended":L"Standard");
		gotoxy(23, ConsoleCurrRow);
		Print(L"ID: %-9d  ", gCAN[ch].id);
		gotoxy(38, ConsoleCurrRow);
		Print(L"Length: %d  ", gCAN[ch].length);
		gotoxy(49, ConsoleCurrRow);
		Print(L"Data: ");
		for(j=0; j<gCAN[ch].length; j++)
		{
			Print(L"%02x ", gCAN[ch].wbuf[j]);
		}
		Print(L"\n");
		ConsoleCurrRow++;
		
		//Set Tx Request
		SET_REG_BM32(CAN_REQ_CTL_TBR0_bm, gCAN[ch].BaseAddr + CAN_REQ_CTL);
		
		//wait tx complete
		if(CANBusWaitTxComplete(ch))
		{
			Print(L"CAN Bus Tx Fail !! \n");
			goto cbtt_end;
		}

		//if increase mode, renew id
		if(idauto)
		{
			gCAN[ch].id++;
			if(gCAN[ch].format)
			{
				if(gCAN[ch].id > 268435455)
				{
					gCAN[ch].id = 0;
				}
			}
			else
			{
				if(gCAN[ch].id > 2047)
				{
					gCAN[ch].id = 0;
				}
			}
			outpd(gCAN[ch].BaseAddr + CAN_TXD_IDF0, gCAN[ch].id);
		}
	}

cbtt_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void CANBusReceiverTest(void)
{
	uint32_t i, loop = 0;
	uint8_t rbuf[8];
	uint8_t ch;

	clrscr();
	Print(L"CAN Bus Test - CAN Bus Receiver \n\n");

	ch = gCANConfig->can->rxch;
	if(gCAN[ch].Flag.init == 0)
	{
		Print(L"CAN Bus %d Can Not Use \n\n", ch);
		goto cbrt_end;
	}
	
	//clean all Interrupt Status
	outpd(gCAN[ch].BaseAddr + CAN_INT_STS, 0x7FF);
	
	CANBusSetBaudrate();

	gCAN[ch].format = gCANConfig->can->format;
	gCAN[ch].id = gCANConfig->can->id;
	gCAN[ch].length = gCANConfig->can->length;

	clrscr();
	Print(L"CAN Bus Test - CAN Bus Receiver \n\n");
	ConsoleCurrRow = 2;

	gCAN[ch].wbuf = NULL;
	gCAN[ch].rbuf = rbuf;

	//set can bus active
	CANBusSetGlobalControlActive(ch);

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(1)
	{
		//wait rx complete
		if(!CANBusWaitRxComplete(ch, 0))
		{
			//get rx data
			CANBusGetRxFrame(ch);

			gotoxy(0, ConsoleCurrRow);
			Print(L"%-5d. Mode: %s  ", ++loop, gCAN[ch].format?L"Extended":L"Standard");
			//gotoxy(23, ConsoleCurrRow);
			Print(L"ID: %-9d  ", gCAN[ch].id);
			//gotoxy(38, ConsoleCurrRow);
			Print(L"Length: %d  ", gCAN[ch].length);
			//gotoxy(49, ConsoleCurrRow);
			Print(L"Data: ");
			for(i=0; i<gCAN[ch].length; i++)
			{
				Print(L"%02x ", gCAN[ch].rbuf[i]);
			}
			Print(L"\n");
			ConsoleCurrRow++;
		}
		
		//get key to return
		if(CheckESC(bioskey(1)))
		{
			goto cbrt_endend;
		}
	}

cbrt_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
	
cbrt_endend:
	NULL;
}

void CANBusTest(void)
{
	int key;

	clrscr();
	if(InitCANBus())
	{
		goto cbt_end;
	}
	usleep(100);
	
	if(gCANConfig == NULL)
	{
		gCANConfig = (CanVariableConfig*)malloc(sizeof(CanVariableConfig));
		if(!gCANConfig)
		{
			Print(L"CAN Config Data Fail !! \n\n");
			goto cbt_end;
		}
		memset(gCANConfig, 0, sizeof(CanVariableConfig));

		gCANConfig->can = (CAN*)malloc(sizeof(CAN));
		if(gCANConfig->can)
		{
			memset(gCANConfig->can, 0, sizeof(CAN));
		}

		CanTestConfigDefault();
	}
	
	while(1)
	{
		clrscr();
		Print(L"0. CAN Config Page. \n");
		Print(L"1. CAN Bus Test - CAN 0 -> CAN 1. \n");
		Print(L"2. CAN Bus Test - CAN 1 -> CAN 0. \n");
		Print(L"3. CAN Bus Test - CAN Bus Transmitter. \n");
		Print(L"4. CAN Bus Test - CAN Bus Receiver. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");
		
		key = GetKey();
		if(CheckESC(key))
		{
			goto cbt_endend;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				CANBusConfiguration();
				break;
			}
			case '1':
			{
				CANBusCANToCANTest(0);
				break;
			}
			case '2':
			{
				CANBusCANToCANTest(1);
				break;
			}
			case '3':
			{
				CANBusTransmitterTest();
				break;
			}
			case '4':
			{
				CANBusReceiverTest();
				break;
			}
		}
	}

cbt_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

cbt_endend:
	NULL;
}





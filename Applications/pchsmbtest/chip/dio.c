#include <stdio.h>
#include "x86hw.h"
#include "util.h"
#include "pca9555.h"

#include "pchsmbtest.h"

static uint8_t gDIOreg[8];
uint8_t gDIOch;
uint8_t gDIOaddr;

//=============================================================================
void DIORestoreRegister(void)
{
	uint8_t idx;
	
	for(idx = 0; idx < 8; idx++)
	{
		SmbusWriteByte(gDIOch, idx, gDIOaddr, &gDIOreg[idx]);
	}
}
//=============================================================================
void DIOBackupRegister(void)
{
	uint8_t idx;
	
	for(idx = 0; idx < 8; idx++)
	{
		SmbusReadByte(gDIOch, idx, gDIOaddr, &gDIOreg[idx]);
	}
}
//=============================================================================
void DIOBoardRead(void)
{
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;

	gST->ConOut->ClearScreen(gST->ConOut);
	
	gotoxy(0, 1);
	Print(L"SMBus - Read DIO Board Test\n");

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
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	// set all pins of port0 as input
	u8wb = 0xFF;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8wb);
	// set all pins of port1 as input
	u8wb = 0xFF;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8wb);
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");
		
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}

		SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
		Print(L"Input port registers    : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8rb);
		Print(L"Output port registers   : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_INV_PORT1, gDIOaddr, &u8rb);
		Print(L"Polarity registers      : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_INV_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8rb);
		Print(L"Configuration registers : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}
//=============================================================================
void DIOBoardWrite(void)
{
	uint8_t i;
	uint8_t bit = 0xFF;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;
	uint16_t level = 0;

	gST->ConOut->ClearScreen(gST->ConOut);

	gotoxy(0, 1);
	Print(L"SMBus - Write DIO Board Test\n");

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
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	// set all pins of port0 as output
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8wb);
	// set all pins of port1 as output
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8wb);

	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");
		
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			break;
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
		SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8wb);
		// set new level to port1
		u8wb = (level >> 8) & 0xFF;
		SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8wb);

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
		Print(L"Input port registers    : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8rb);
		Print(L"Output port registers   : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_INV_PORT1, gDIOaddr, &u8rb);
		Print(L"Polarity registers      : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_INV_PORT0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
		
		SmbusReadByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8rb);
		Print(L"Configuration registers : 0x%02X", u8rb);
		SmbusReadByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8rb);
		Print(L"%02X \n", u8rb);
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}
//=============================================================================
void DIOBoardAuto(void)
{
	uint8_t i;
	uint8_t u8wb;
	uint8_t u8rb;
	uint8_t bit = 0;
	uint8_t cnt = 0;
	uint16_t key = 0;
	uint16_t level = 0;

	gST->ConOut->ClearScreen(gST->ConOut);

	gotoxy(0, 1);
	Print(L"SMBus - Auto R/W DIO Board Test\n");

	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<16; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	Print(L"Press ESC to return.");

	// set all pins of port0 as output
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8wb);
	// set all pins of port1 as output
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8wb);

	// set port0 to low
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8wb);
	// set port1 to low
	u8wb = 0;
	SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8wb);

	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");
		
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		
		SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
		for(i=0; i<8; i++)
		{
			Print(L"%2d ", (u8rb >> i) & 0x01);
		}
		Print(L"\n");

		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			break;
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
			SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8wb);
			// set new level to port1
			u8wb = (level >> 8) & 0xFF;
			SmbusWriteByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8wb);

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			SmbusReadByte(gDIOch, DIO_REG_IN_PORT1, gDIOaddr, &u8rb);
			Print(L"Input port registers    : 0x%02X", u8rb);
			SmbusReadByte(gDIOch, DIO_REG_IN_PORT0, gDIOaddr, &u8rb);
			Print(L"%02X \n", u8rb);
			
			SmbusReadByte(gDIOch, DIO_REG_OUT_PORT1, gDIOaddr, &u8rb);
			Print(L"Output port registers   : 0x%02X", u8rb);
			SmbusReadByte(gDIOch, DIO_REG_OUT_PORT0, gDIOaddr, &u8rb);
			Print(L"%02X \n", u8rb);
			
			SmbusReadByte(gDIOch, DIO_REG_INV_PORT1, gDIOaddr, &u8rb);
			Print(L"Polarity registers      : 0x%02X", u8rb);
			SmbusReadByte(gDIOch, DIO_REG_INV_PORT0, gDIOaddr, &u8rb);
			Print(L"%02X \n", u8rb);
			
			SmbusReadByte(gDIOch, DIO_REG_CONFIG1, gDIOaddr, &u8rb);
			Print(L"Configuration registers : 0x%02X", u8rb);
			SmbusReadByte(gDIOch, DIO_REG_CONFIG0, gDIOaddr, &u8rb);
			Print(L"%02X \n", u8rb);

			bit++;
		}
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
}
//=============================================================================
uint8_t DIOBoardSearch(SMB_BUS *smb)
{
	uint8_t i, ch;
	uint8_t rbyte;
	
	for(ch=0; ch<2; ch++)
	{
		for(i=0; i<smb->dev_num[ch]; i++)
		{
			if((smb->dev_addr[ch][i] & 0xF0) == 0x40)
			{
				SmbusReadByte(ch, DIO_REG_INV_PORT0, smb->dev_addr[ch][i], &rbyte);
				if(rbyte != 0)
				{
					continue;
				}
				
				SmbusReadByte(ch, DIO_REG_INV_PORT1, smb->dev_addr[ch][i], &rbyte);
				if(rbyte != 0)
				{
					continue;
				}
				
				SmbusReadByte(ch, DIO_REG_CONFIG0, smb->dev_addr[ch][i], &rbyte);
				if(rbyte != 0xFF)
				{
					continue;
				}
				
				SmbusReadByte(ch, DIO_REG_CONFIG1, smb->dev_addr[ch][i], &rbyte);
				if(rbyte != 0xFF)
				{
					continue;
				}
				
				gDIOch = ch;
				gDIOaddr = smb->dev_addr[ch][i];
				
				return 0;
			}
		}
	}
	
	return 1;
}


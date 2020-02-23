#include <stdio.h>
#include "x86hw.h"
#include "util.h"
#include "pca9555.h"
#include "tsmb.h"

static uint8_t dio_reg[8];
//=============================================================================
void dio_restore_reg_value(uint8_t addr)
{
	uint8_t idx;
	
	for(idx = 0; idx < 8; idx++)
	{
		smbus->write_byte(smbus, addr, idx, dio_reg[idx]);
	}
}
//=============================================================================
void dio_backup_reg_value(uint8_t addr)
{
	uint8_t idx;
	
	for(idx = 0; idx < 8; idx++)
	{
		dio_reg[idx] = smbus->read_byte(smbus, addr, idx);
	}
}
//=============================================================================
void dio_board_read(SMB_DEVICE *dev)
{
	uint8_t i;
	uint8_t u8rb;
	uint16_t key = 0;

	gST->ConOut->ClearScreen(gST->ConOut);
	
	// hide cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);
	
	gotoxy(0, 1);
	fprintf(stderr, "=== Read GPIO Port ===\n");

	gotoxy(0, 5);
	fprintf(stderr, "Pin    : ");
	for(i=0; i<16; i++)
	{
		fprintf(stderr, "%2d ", i);
	}
	fprintf(stderr, "\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	fprintf(stderr, "Short I/O Pin To Gnd For Test");
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	fprintf(stderr, "press q key to exit");

	// set all pins of port0 as input
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG0, 0xFF);
	// set all pins of port1 as input
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG1, 0xFF);
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == 'q' || key == 'Q')
		{
			break;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		fprintf(stderr, "Status : ");
		
		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}

		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}
		fprintf(stderr, "\n");

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		fprintf(stderr, "Input port registers    : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0));
		fprintf(stderr, "Output port registers   : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT0));
		fprintf(stderr, "Polarity registers      : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT0));
		fprintf(stderr, "Configuration registers : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG1), smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG0));
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	
	// show cursor
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}
//=============================================================================
void dio_board_write(SMB_DEVICE *dev)
{
	uint8_t i;
	uint8_t bit = 0xFF;
	uint8_t u8wb;
	uint8_t u8rb;
	uint16_t key = 0;
	uint16_t level = 0;

	gST->ConOut->ClearScreen(gST->ConOut);
	
	// hide cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);

	gotoxy(0, 1);
	fprintf(stderr, "=== Write GPIO Port ===\n");

	gotoxy(0, 5);
	fprintf(stderr, "Pin    : ");
	for(i=0; i<16; i++)
	{
		fprintf(stderr, "%2d ", i);
	}
	fprintf(stderr, "\n");

	console_color(EFI_CYAN, EFI_BLACK);
	fprintf(stderr, "Key    : ");
	for(i=1; i<=8; i++)
	{
		fprintf(stderr, "%2d ", i);
	}
	for(i=1; i<=8; i++)
	{
		fprintf(stderr, "F%d ", i);
	}
	fprintf(stderr, "\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	fprintf(stderr, "Press Key To Set Pin Level");
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	fprintf(stderr, "press q key to exit");

	// set all pins of port0 as output
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG0, 0);
	// set all pins of port1 as output
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG1, 0);

	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		fprintf(stderr, "Status : ");
		
		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}

		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}
		fprintf(stderr, "\n");

		key = (uint16_t)bioskey(1);
		if(key == 'q' || key == 'Q')
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
		smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT0, u8wb);
		// set new level to port1
		u8wb = (level >> 8) & 0xFF;
		smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT1, u8wb);

		gotoxy(0, 12);
		console_color(EFI_LIGHTGRAY, EFI_BLACK);
		fprintf(stderr, "Input port registers    : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0));
		fprintf(stderr, "Output port registers   : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT0));
		fprintf(stderr, "Polarity registers      : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT0));
		fprintf(stderr, "Configuration registers : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG1), smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG0));
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	
	// show cursor
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}
//=============================================================================
void dio_board_auto(SMB_DEVICE *dev)
{
	uint8_t i;
	uint8_t u8rb;
	uint8_t u8wb;
	uint8_t bit = 0;
	uint8_t cnt = 0;
	uint16_t key = 0;
	uint16_t level = 0;

	gST->ConOut->ClearScreen(gST->ConOut);
	
	// hide cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);

	gotoxy(0, 1);
	fprintf(stderr, "=== Auto R/W GPIO Port ===\n");

	gotoxy(0, 5);
	fprintf(stderr, "Pin    : ");
	for(i=0; i<16; i++)
	{
		fprintf(stderr, "%2d ", i);
	}
	fprintf(stderr, "\n");
	
	gotoxy(0, 21);
	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	fprintf(stderr, "press q key to exit");

	// set all pins of port0 as output
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG0, 0);
	// set all pins of port1 as output
	smbus->write_byte(smbus, dev->addr, DIO_REG_CONFIG1, 0);

	// set port0 to low
	smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT0, 0);
	// set port1 to low
	smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT1, 0);

	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		fprintf(stderr, "Status : ");
		
		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}

		u8rb = smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1);
		for(i=0; i<8; i++)
		{
			fprintf(stderr, "%2d ", (u8rb >> i) & 0x01);
		}
		fprintf(stderr, "\n");

		key = (uint16_t)bioskey(1);
		if(key == 'q' || key == 'Q')
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
			smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT0, u8wb);
			// set new level to port1
			u8wb = (level >> 8) & 0xFF;
			smbus->write_byte(smbus, dev->addr, DIO_REG_OUT_PORT1, u8wb);

			gotoxy(0, 12);
			console_color(EFI_LIGHTGRAY, EFI_BLACK);
			fprintf(stderr, "Input port registers    : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_IN_PORT0));
			fprintf(stderr, "Output port registers   : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_OUT_PORT0));
			fprintf(stderr, "Polarity registers      : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT1), smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT0));
			fprintf(stderr, "Configuration registers : 0x%02X%02X \n", smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG1), smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG0));

			bit++;
		}
	}

	console_color(EFI_LIGHTGRAY, EFI_BLACK);
	
	// show cursor
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}
//=============================================================================
uint8_t show_dio_option(void)
{
	char key;
	SMB_DEVICE	*dev;

	if(smbus == NULL)
		return 1;

	dev = tsmb_find_dev_type(smbus->dev_head, SMB_DEV_LED_BOARD);

	if(dev == NULL)
	{
		fprintf(stderr, "ERROR: Failed to found PCA5555 chip.\n");
		fprintf(stderr, "\npress any key to quit");
		while (bioskey(1) == 0);
		return 1;
	}
	
	dio_backup_reg_value(dev->addr);
	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		//fprintf(stderr, "\n\n");
		print_title("DIO Board Test");
		
		fprintf(stderr, "1. Read DIO GPIO Port\n");
		fprintf(stderr, "2. Write DIO GPIO Port\n");
		fprintf(stderr, "3. Auto R/W DIO GPIO Port\n");
		fprintf(stderr, "q. Quit test\n\n");
		key = (char) (bioskey(0) & 0xFF);

		if(key == 'q' || key == 'Q')
			goto end;
		
		switch(key)
		{
			case '1':
				dio_board_read(dev);
				break;
			
			case '2':
				dio_board_write(dev);
				break;

			case '3':
				dio_board_auto(dev);
				break;

			default:
				fprintf(stderr, "press Unknown key.\n");
				gotoxy(0, 15);
				fprintf(stderr, "\npress any key to continue");
				while (bioskey(1) == 0);
				break;
		}
	} while(1);

end:
	dio_restore_reg_value(dev->addr);
	return 0;
}


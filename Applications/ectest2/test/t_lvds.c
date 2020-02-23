#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "mbxapp.h"
#include "util.h"

/*===========================================================
 * Name  : test_lvds_main
 * Pupose: Test LVDS Main
 * Input : 
 * Output: 
 *===========================================================*/
int test_lvds_main(void)
{
	int 	key;
	CHAR8 	keyChar;
	UINT8	lvdsBackup;
	UINT8	lvds_state;
	INT32	row;
	
	clrscr();
	kdebug(0, 0);		
	console_color(LIGHTGRAY, BLACK);
	mApp.ec->readEcRam(0x01, 0x50, 1, &lvdsBackup);				// backup brightness
	fprintf(stderr, "LVDS Default Setting: ");
	if(lvdsBackup & 0x80)
		fprintf(stderr, " ON, ");
	else
		fprintf(stderr, "OFF, ");
	fprintf(stderr, "Level %d\n\n", lvdsBackup & 0x7F);
		
	fprintf(stderr, "[ LVDS Test ]\n");
	fprintf(stderr, "  1. Press Key '0'~'9' to change brightness level.\n");
	fprintf(stderr, "  2. Press Key 'space' to on/off LVDS.\n\n");

	row = gST->ConOut->Mode->CursorRow;
	gotoxy(0, ConsoleHeight - 1);
	fprintf(stderr, "Press Alt+X to return main menu.");
	
	lvds_state = lvdsBackup;
	while(1)
	{
		gotoxy(0, row);
		key = bioskey(0);
		keyChar = (CHAR8) (key & 0xFF);

		if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
		{
			mApp.ec->writeEcRam(0x01, 0x50, 1, &lvdsBackup);
			fprintf(stderr, "Restore LVDS Setting and quit test.\n");
			break;
		}
		else if((keyChar >= '0') && (keyChar <= '9'))
		{
			lvds_state = (lvds_state & 0x80) | (UINT8)(keyChar - '0');
			fprintf(stderr, "Level %d", lvds_state & 0x7F);
			mApp.ec->writeEcRam(0x01, 0x50, 1, &lvds_state);
		}
		else if(keyChar == ' ')
		{
			if(lvds_state & 0x80){
				fprintf(stderr, "Trun OFF LVDS");
				lvds_state &= 0x7F;
				mApp.ec->writeEcRam(0x01, 0x50, 1, &lvds_state);
			}
			else{
				fprintf(stderr, "Trun ON LVDS"); 
				lvds_state |= 0x80;
				mApp.ec->writeEcRam(0x01, 0x50, 1, &lvds_state);
			}
		}
		else
		{
			fprintf(stderr, "Invalid input, please try again."); 
		}
		gBS -> Stall (300000);
		gotoxy(0, row);
		fprintf(stderr, "%70s", " ");
		
		mApp.ec->readEcRam(0x01, 0x50, 1, &lvds_state);			// read brightness setting
		gotoxy(0, 0);
		fprintf(stderr, "LVDS Current Setting: ");
		if(lvds_state & 0x80)
			fprintf(stderr, " ON, ");
		else
			fprintf(stderr, "OFF, ");
		fprintf(stderr, "Level %d", lvds_state & 0x7F);
	}

	menuSelect = _mid_main;
	return _MBEr_Success;
}

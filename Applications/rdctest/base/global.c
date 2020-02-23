/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  global.CPP
* Purpose : global variable
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include <stdio.h>
#include "global.h"

/*==============================================================*/
UINTN ConsoleCurrRow = 0;
UINTN ConsoleWidth	 = 80;
UINTN ConsoleHeight	 = 25;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInEx;

int menuSelect;
int doneFlag;

UINTN mode_bak = 0;
INT32 attr_bak = EFI_TEXT_ATTR(EFI_YELLOW, EFI_BLACK);
/*==============================================================*/
EFI_STATUS console_init(void)
{
	EFI_STATUS status;
	UINTN 	mode_num;
	UINTN 	col;
	UINTN 	row;
	
	// open extension console input
    status = gBS->OpenProtocol(
                 gST->ConsoleInHandle,
                 &gEfiSimpleTextInputExProtocolGuid,
                 (VOID **) &ConInEx,
                 gImageHandle,
                 NULL,
                 EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	if(EFI_ERROR(status))
		return status;
	
	// backup simple console setting
	mode_bak = gST->ConOut->Mode->Mode;
	attr_bak = gST->ConOut->Mode->Attribute;
	status = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &col, &row);
	
	if(EFI_ERROR(status))
		return status;
	
	// search 80X25 mode.
	for(mode_num = 0; mode_num < gST->ConOut->Mode->MaxMode; mode_num++)
	{
		status = gST->ConOut->QueryMode(gST->ConOut, mode_num, &col, &row);
		
		if(EFI_ERROR(status))
			continue;
		
		if(col == ConsoleWidth && row == ConsoleHeight)
			break;
	}
	// change global X/Y edge if 80x25 doesn't find.
	if(mode_num >= gST->ConOut->Mode->MaxMode)
	{
		ConsoleWidth	= col;
		ConsoleHeight	= row;
	}
	else
	{
		status = gST->ConOut->SetMode(gST->ConOut, mode_num);
		if(EFI_ERROR(status))
			return status;
	}
	// clear screen
	gST->ConOut->ClearScreen(gST->ConOut);
	// hide cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);
	return EFI_SUCCESS;
}

/*==============================================================*/
void console_exit(void)
{
	// backup simple console setting
	gST->ConOut->SetMode(gST->ConOut, mode_bak);
	gST->ConOut->SetAttribute(gST->ConOut, attr_bak);
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}
/*==============================================================*/
void console_color(UINTN txt_color, UINTN bg_color)
{
//	UINTN text;
//	UINTN bg;
	
	gST->ConOut->SetAttribute(gST->ConOut, EFI_TEXT_ATTR(txt_color, bg_color));
	//text	= gST->ConOut->Mode->Attribute & 0xF;
	//bg		= (gST->ConOut->Mode->Attribute & 0xF0) >> 4;
	//fprintf(stderr, "Text: 0x%X, BG: 0x%X\n", text, bg);
}

void myEventNotify(EFI_EVENT Event, void *Context)
{
	UINT32 *cnt;
	
	cnt = (UINT32 *)Context;
	*cnt = *cnt + 1;
}

/*==============================================================*/
int bioskey(uint8_t cmd)
{
	EFI_STATUS 		Status;
	EFI_KEY_DATA 	KeyData;
	UINTN  			EventIndex;

	if(cmd == 0)
	{
		gBS->WaitForEvent(1, &ConInEx->WaitForKeyEx, &EventIndex);
		Status = ConInEx->ReadKeyStrokeEx(ConInEx, &KeyData);
		if(Status != EFI_SUCCESS)
		{
			return 0;
		}
	}
	else if(cmd == 1){
		Status = Status = ConInEx->ReadKeyStrokeEx(ConInEx, &KeyData);
		if(Status != EFI_SUCCESS)
			return 0;
		if(KeyData.Key.ScanCode == 0 && KeyData.Key.UnicodeChar == 0)
			return 0;
	}
	else
		return 0;
	
	if(KeyData.KeyState.KeyShiftState & (EFI_RIGHT_ALT_PRESSED | EFI_LEFT_ALT_PRESSED))
		KeyData.Key.ScanCode |= EFI_SCANEX_ALT;
	if(KeyData.KeyState.KeyShiftState & (EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_CONTROL_PRESSED))
		KeyData.Key.ScanCode |= EFI_SCANEX_CTRL;
	
	//fprintf(stderr, "Scan Key: 0x%X, Char is: 0x%X\n", KeyData.Key.ScanCode, KeyData.Key.UnicodeChar);
	//fprintf(stderr, "Shift: 0x%X, Toggle: 0x%X\n", KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState);
	return (((int)KeyData.Key.ScanCode << 8) | ((int)KeyData.Key.UnicodeChar & 0xFF));
}

/*==============================================================*/
void kdebug(uint8_t code1, uint8_t code2)
{
	INT32	col;
	INT32	row;
	
	col = gST->ConOut->Mode->CursorColumn;
	row = gST->ConOut->Mode->CursorRow;
	
    gotoxy(ConsoleWidth - 7, ConsoleHeight - 1);
    fprintf(stderr, "%02X%02X%02X", (uint8_t)menuSelect, code1, code2);
    gotoxy(col, row);
}


void AsciiToUnicodeString(uint8_t *origin, uint16_t *str)
{
	while(*origin != '\0')
	{
		*str++ = (uint16_t)*origin++;
	}
	*str = '\0';
}

void ByteArrayToOneNumber(uint8_t *origin, uint16_t len, void *one)
{
	uint16_t i;

	i = len;
	while(i>0)
	{
		*(uint32_t*)one = *(uint32_t*)one<<8;
		i--;
		*(uint32_t*)one |= (uint32_t)origin[i];
	}
}

int GetKey(void)
{
	EFI_STATUS 		Status;
	EFI_KEY_DATA 	KeyData;
	UINTN  			EventIndex;

	gBS->WaitForEvent(1, &ConInEx->WaitForKeyEx, &EventIndex);

	Status = ConInEx->ReadKeyStrokeEx(ConInEx, &KeyData);
	if(Status != EFI_SUCCESS)
	{
		return 0;
	}

	if(KeyData.KeyState.KeyShiftState & (EFI_RIGHT_ALT_PRESSED | EFI_LEFT_ALT_PRESSED))
	{
		KeyData.Key.ScanCode |= EFI_SCANEX_ALT;
	}
	
	if(KeyData.KeyState.KeyShiftState & (EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_CONTROL_PRESSED))
	{
		KeyData.Key.ScanCode |= EFI_SCANEX_CTRL;
	}
	
	//Print(L"Scan Key: 0x%X, Char is: 0x%X\n", KeyData.Key.ScanCode, KeyData.Key.UnicodeChar);
	//Print(L"Shift: 0x%X, Toggle: 0x%X\n", KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState);
	return (((int)KeyData.Key.ScanCode << 8) | ((int)KeyData.Key.UnicodeChar & 0xFF));
}

uint8_t CheckESC(int key)
{
	if(key == (SCAN_ESC << 8))    //ESC
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t WaitReturnOrRefreshEvent(void)
{
	EFI_KEY_DATA KeyData;
	UINTN event_index;
	EFI_EVENT wait_event[2];

	wait_event[0] = ConInEx->WaitForKeyEx;
	gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (void*)NULL, &wait_event[1]);
	gBS->SetTimer(wait_event[1], TimerPeriodic, 1000*1000*10);	//unit 100ns
	
	gBS->WaitForEvent(2, wait_event, &event_index);
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);
	if(event_index == 0)
	{
		gBS->SetTimer(wait_event[1], TimerCancel, 1000*1000*10);
		ConInEx->ReadKeyStrokeEx(ConInEx, &KeyData);

		if(KeyData.Key.ScanCode == SCAN_ESC)
		{
			//gST->ConOut->EnableCursor(gST->ConOut, TRUE);
			gBS->CloseEvent(wait_event);
			return 1;
		}
	}

	gBS->CloseEvent(wait_event);
	return 0;
}


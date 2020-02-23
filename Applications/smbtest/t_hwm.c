#include <stdio.h>
#include "x86hw.h"
#include "util.h"
#include "hwm.h"
#include "tsmb.h"

//=============================================================================
EFI_STATUS show_hwm_screen(HWM_HANDLE_ST *hwm)
{
	CHAR8	*ChName = NULL;
	UINT8	CursorY = 0;
	UINTN	Index;
	UINTN	Delay = 0xFFFF;
	UINT16	Value = 0;
	EFI_STATUS	Status = EFI_SUCCESS;

	gST->ConOut->ClearScreen(gST->ConOut);
	print_title(hwm->ChipName);

	// Print Monitor Item & Info
	gotoxy(0, 4);
	if (hwm->MaxCh[HWM_TYPE_VOLT] > 0) {
		fprintf(stderr, "Voltage:\n");
		for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_VOLT]; Index++) {
			Status = hwm->GetChannelName(HWM_TYPE_VOLT, Index, &ChName);
			if (Status == EFI_UNSUPPORTED) 
				continue;
			if (Status != EFI_SUCCESS) 
				break;


			fprintf(stderr, "%10s :\n", ChName);

		}
		fprintf(stderr, "\n\n");
	}

	if (hwm->MaxCh[HWM_TYPE_FAN] > 0) {
		fprintf(stderr, "Fan Speed:\n");
		for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_FAN]; Index++) {
			Status = hwm->GetChannelName(HWM_TYPE_FAN, Index, &ChName);
			if (Status == EFI_UNSUPPORTED)
				continue;
			if (Status != EFI_SUCCESS) 
				break;

			fprintf(stderr, "%10s :\n", ChName);
		}
		fprintf(stderr, "\n\n");
	}

	CursorY = 4;
	if (hwm->MaxCh[HWM_TYPE_CURR] > 0) {
		gotoxy(35, CursorY);
		fprintf(stderr, "Current:"); 
		CursorY++;
		for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_CURR]; Index++) {
			Status = hwm->GetChannelName(HWM_TYPE_CURR, Index, &ChName);
			if (Status == EFI_UNSUPPORTED) 
				continue;
			if (Status != EFI_SUCCESS) 
				break;

			gotoxy(35, CursorY);
			fprintf(stderr, "%10s :", ChName);
			CursorY++;
		}
		CursorY += 2; // for one item 
	}

	if (hwm->MaxCh[HWM_TYPE_TEMP] > 0) {
		gotoxy(35, CursorY);
		fprintf(stderr, "Temperature:");
		CursorY++;
		for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_TEMP]; Index++) {
			Status = hwm->GetChannelName(HWM_TYPE_TEMP, Index, &ChName);
			if (Status == EFI_UNSUPPORTED) 
				continue;
			if (Status != EFI_SUCCESS) 
				break;

			gotoxy(35, CursorY);
			fprintf(stderr, "%10s :", ChName);
			CursorY++;
		}
		CursorY += 2; // for one item 
	}

	gotoxy(0, ConsoleHeight - 3);
	fprintf(stderr, "Note: The above voltages are pin's voltage. They don't mean the real voltage.\n\n"); 

	gotoxy(0, ConsoleHeight - 1);
	fprintf(stderr, "press any key to quit");

	// Read/Print Monitor Data
	do {
		if (Delay++ < 200) {
			if(bioskey(1) != 0)
				break;
			gBS->Stall(1000);	// 1ms
			continue;
		}
		else
			Delay = 0;
		CursorY = 4; 
		if (hwm->MaxCh[HWM_TYPE_VOLT] > 0) {
			CursorY++;	// for Type Title
			for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_VOLT]; Index++) {
				Status = hwm->GetReading(HWM_TYPE_VOLT, Index, &Value);
				if (Status != EFI_SUCCESS) 
					break;
				gotoxy(13, CursorY);
				fprintf(stderr, "%4.2fV", (float)Value * hwm->Resolution[HWM_TYPE_VOLT] / HMW_RESOLUTION_BASE);
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		if (hwm->MaxCh[HWM_TYPE_FAN] > 0) {
			CursorY++; // for Type Title
			for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_FAN]; Index++) {
				Status = hwm->GetReading(HWM_TYPE_FAN, Index, &Value);
				if (Status != EFI_SUCCESS) 
					break;
				gotoxy(13, CursorY);
				if (Value != 0xFFFF) 
					fprintf(stderr, "%6d RPM", (UINT32)Value * hwm->Resolution[HWM_TYPE_FAN] / HMW_RESOLUTION_BASE); 
				else
					fprintf(stderr, "  Overflow");
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		CursorY = 4; 
		if (hwm->MaxCh[HWM_TYPE_CURR] > 0) {
			CursorY++; // for Type Title
			for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_CURR]; Index++) {
				Status = hwm->GetReading(HWM_TYPE_CURR, Index, &Value);
				if (Status != EFI_SUCCESS) 
					break;
				gotoxy(48, CursorY);
				fprintf(stderr, "%4.1fA", (float)Value * hwm->Resolution[HWM_TYPE_CURR] / HMW_RESOLUTION_BASE);
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

		if (hwm->MaxCh[HWM_TYPE_TEMP] > 0) {
			CursorY++; // for Type Title
			for (Index = 0; Index < hwm->MaxCh[HWM_TYPE_TEMP]; Index++) {
				Status = hwm->GetReading(HWM_TYPE_TEMP, Index, &Value);
				if (Status != EFI_SUCCESS) 
					break;
				gotoxy(48, CursorY);
				if (Value & 0x8000) {
					Value = (~Value) + 1;
					fprintf(stderr, "-%3.1fC", (float)Value * hwm->Resolution[HWM_TYPE_TEMP] / HMW_RESOLUTION_BASE);
				}
				else
					fprintf(stderr, " %3.1fC", (float)Value * hwm->Resolution[HWM_TYPE_TEMP] / HMW_RESOLUTION_BASE);
				CursorY++; // for one channel 
			}
			CursorY += 2; // for one item 
		}

	}while (1);
	return Status; 
}

//=============================================================================
uint8_t show_hwm_option(void)
{
	char 		key;
	uint16_t 	u16;
	uint8_t		bank;
	uint8_t		addr, data;
	HWM_HANDLE_ST	*hwm[2];
	HWM_HANDLE_ST	**hwmp;

	if(smbus == NULL)
		return 1;

	hwm[0] = NULL;
	hwm[1] = NULL;
	HardwareMonitorInit(smbus, HWM_CHIP_W83782, &hwm[0]);
	HardwareMonitorInit(smbus, HWM_CHIP_NCT7802, &hwm[1]);
	if (hwm[0] == NULL && hwm[1] == NULL) {
		fprintf(stderr, "ERROR: Failed to found hwm chip.\n");
		fprintf(stderr, "\npress any key to quit");
		while (bioskey(1) == 0);
		return 1;
	}

	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		print_title("Hardware Monitor");

		fprintf(stderr, "\n");
		fprintf(stderr, "1. Winbond W83782\n");
		fprintf(stderr, "2. Nuvoton NCT7802\n");
		fprintf(stderr, "q. Quit test\n\n");

		key = (char) (bioskey(0) & 0xFF);
		if(key == 'q' || key == 'Q')
			return 0;

		switch(key)
		{
		case '1':
			if(hwm[0] == NULL)
			{
				fprintf(stderr, "W83782 not found.\n");
				goto WaitKey;
			}
			show_hwm_screen(hwm[0]);
			break;
		
		case '2':
			if(hwm[1] == NULL)
			{
				fprintf(stderr, "NCT7802 not found.\n");
				goto WaitKey;
			}
			show_hwm_screen(hwm[1]);

			break;

		case 'r': // debug
			hwmp = &hwm[0];
			goto hwmdbg;
		case 'R': // debug
			hwmp = &hwm[1];
hwmdbg:
			if(*hwmp == NULL)
			{
				fprintf(stderr, "HWN not found.\n");
			}
			else{
				//smbus->write_byte(smbus, dev.addr, LM_REG_T_CRIT_LMT, u8wb);
				fprintf(stderr, "Read %s Register\n", (*hwmp)->ChipName);
				u16 = get_number_stdin("Enter Register bank:", 1, 1);
				bank = (uint8_t) u16;
				u16 = get_number_stdin("Enter Register address:", 1, 2);
				addr = (uint8_t) u16;
				(*hwmp)->ReadReg(bank, addr, &data);
				fprintf(stderr, "Get Byte u8rb: 0x%X\n", data);
			}
			goto WaitKey;

		default:
			fprintf(stderr, "press Unknown key.\n");
WaitKey:
			fprintf(stderr, "\npress any key to continue");
			while (bioskey(1) == 0);
			break;
		}
	} while(1);

	return 0;
}


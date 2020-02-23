#include <stdio.h>
#include <stdlib.h>

#include "x86hw.h"
#include "util.h"
#include "smbtest.h"
#include "tsmb.h"
#include "t_hwm.h"
#include "t_thermal.h"
#include "t_dio.h"

//=============================================================================
//  variables
//=============================================================================

//=============================================================================
uint8_t show_pch_smb(void)
{
	uint8_t i;
	SMB_DEVICE	*dev;
	
	if(smbus == NULL)
		return 1;
	
	gST->ConOut->ClearScreen(gST->ConOut);
	//fprintf(stderr, "\n\n");
	print_title("SMBus Controller");
	
	fprintf(stderr, "Vendor:   0x%0X\n", smbus->vendor);
	fprintf(stderr, "Base:     0x%0X\n", smbus->base);
	fprintf(stderr, "Slv_Num:  %d\n",	smbus->dev_num);
	fprintf(stderr, "Slv_Addr: ");
	
	dev = smbus->dev_head;
	for(i = 0; i<smbus->dev_num; i++)
	{
		fprintf(stderr, "0x%0X, ", dev->addr);
		dev = dev->next;
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "\npress any key to quit");
	while (bioskey(1) == 0);
	return 0; 
}

/*==============================================================*/
// MAIN
/*==============================================================*/
EFI_STATUS main( IN int argc, IN char **argv)
{
	EFI_STATUS	ret = 0;
	char	key;

	// Register Ctrl-C for quit app
	console_init();
	fprintf(stderr, "smbtest is initializing... \n");

	if(tsmb_init() != 0)
	{
		fprintf(stderr, "ERROR: Failed to initialize SMBus\n");
		ret = EFI_DEVICE_ERROR;
		goto end;
	}
	
	// Check Slave Device Type
	if(smbus->dev_num == 0)
	{
		fprintf(stderr, "ERROR: Failed to get any device on SMBus.\n");
		ret = EFI_NOT_FOUND;
		goto end;
	}
	
	// App Main menu
	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		//fprintf(stderr, "\n\n");
		printf("\nCopyright(c) , Advantech Corporation 2017-2018\n");
		printf("Program:smbtest  Advantech PCH SMBus utility.\n");
		printf("Date:%s\n",SMBTEST_DATE);
		printf("Version:%s\n\n",SMBTEST_VER);

		fprintf(stderr, "1. Show SMBus Controller Info\n");
		fprintf(stderr, "2. Refresh SMBus Device List\n");
		fprintf(stderr, "3. Test SMBus By Thermal IC\n");
		fprintf(stderr, "4. Test SMBus By DIO Board\n");
		fprintf(stderr, "5. Test Hardware Monitor Chip\n");
		fprintf(stderr, "q. Quit test\n");

		key = (char)(bioskey(0) & 0xFF);

		if(key == 'q' || key == 'Q')
			break;		
		gST->ConOut->ClearScreen(gST->ConOut);
		//fprintf(stderr, "\n\n");
		switch(key)
		{
		case '1':
			// Show PCH SMBus Controller info
			show_pch_smb();
			break;
		
		case '2':
			fprintf(stderr, "refresh SMBus device list ...\n");
	
			if(tsmb_init() != 0)
			{
				fprintf(stderr, "ERROR: Failed to refresh SMBus\n");
				ret = EFI_DEVICE_ERROR;
				goto end;
			}
			break;

		case '3':
			show_thermal_option();
			break;
			
		case '4':
			show_dio_option();
			break;

		case '5':
			show_hwm_option();
			break;

		default:
			fprintf(stderr, "press Unknown key.\n");
			fprintf(stderr, "\npress any key to continue");
			while (bioskey(1) == 0);
			break;
		}

	} while(1);
	fprintf(stderr, "\n");
	
end:
	tsmb_exit();
	console_exit();
	return ret;
}


#include <stdio.h>
#include <stdlib.h>

#include "Library/UefiLib.h"
#include "x86hw.h"
#include "util.h"
#include "rdc.h"
#include "rdci2c.h"

//=============================================================================
//  variables
//=============================================================================

/*==============================================================*/
// MAIN
/*==============================================================*/
EFI_STATUS main( IN int argc, IN char **argv)
{
	uint8_t 	rbuf[512];
	uint16_t	len = 0;
	uint8_t		addr;
	uint8_t		p_sw = 0;
	uint32_t	cnt2;
	uint32_t	retry = 1;
	int			freq = 0;
	int			dly = 1;
	
	switch(argc){
	case 2:
		// freg
		break;
	case 3:
		retry	= atoi(argv[2]);
		break;
	case 4:
		retry	= atoi(argv[2]);
		dly 	= atoi(argv[3]) * 1000;
		break;
	case 5:
		retry 	= atoi(argv[2]);
		dly 	= atoi(argv[3]) * 1000;
		p_sw	= (uint8_t)atoi(argv[4]);
		break;
	default:
		Print(L"rdci2c [freq] [retry] [delay(ms)] [bool]\n");
		return EFI_SUCCESS;
	}

	freq = atoi(argv[1]);

	memset(rbuf, 0, 512);

	if(rdc_i2c_master_init(0, (uint16_t)freq) != 0)
		return EFI_SUCCESS;
	
	if(rdc_i2c_get_token(0, 1000) != 0)
		return EFI_SUCCESS;

	addr = 0x42; // DIO Board I2C Address
	
	for(cnt2 = 0; cnt2 < retry; cnt2++){
		len = 16;
		rbuf[0] = 0x04;
			
		if(p_sw)
			Print(L"%6d\r", cnt2);
			
		if(rdc_i2c_smb_request(0, addr | 0x01, &len, rbuf) != 0){
			Print(L"\nFail\n");
			goto end;
		}
		gBS->Stall(dly);
	}
	Print(L"\nTest End");
end:
	rdc_i2c_return_token(0);

	return EFI_SUCCESS;
}


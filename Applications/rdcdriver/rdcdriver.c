#include "rdcdriver.h"





EFI_STATUS CheckEC(void)
{
	UINT16 aec_type;
	UINT8 res;
	
	res = aec_init();
	if(res != AEC_OK)
	{
		//printk("aec_init err! res=%x\n", res);
		return EFI_NOT_FOUND;
	}
	
	aec_type = aec_get_type();
	if((aec_type != AEC_TYPE_RDC_FW) && (aec_type != AEC_TYPE_RDC_SW))
	{
		//printk("EC Type Can Not Support !! \n\n");
		return EFI_UNSUPPORTED;
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI RDCDxeInit(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT8 success = 0;
	
	//check EC
	status = CheckEC();
	if(EFI_ERROR(status))
	{
		return status;
	}

	//init Protocol
	status = RDCProtocolDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}
	
	//init CAN
	status = RDCCanMasterDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}
	
	//init I2C
	status = RDCI2cMasterDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}

	//init SMBus
	status = RDCSMBusMasterDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}

	//init SI2C
	status = RDCSi2cMasterDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}

	//gpio
	status = RDCGPIOMasterDxeInstall(ImageHandle, SystemTable);
	if(status == EFI_SUCCESS)
	{
		success++;
	}
	
	if(success)
	{
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_NOT_FOUND;
	}
}



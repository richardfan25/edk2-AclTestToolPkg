#include "rdcdriver.h"



//
// Platform specific RDC Communicate device path
//
RDC_COMMUNICATE_DEVICE_PATH gRDCCommunicateMemMapPathTemplate = {
	{
		HARDWARE_DEVICE_PATH,
		HW_MEMMAP_DP,
		(UINT8)(sizeof(MEMMAP_DEVICE_PATH)),
		(UINT8)((sizeof(MEMMAP_DEVICE_PATH)) >> 8),
		EfiMemoryMappedIO,
		0,
		0
	},
	{
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		(sizeof(EFI_DEVICE_PATH_PROTOCOL)),
		0
	}
};

EFI_GUID gEfiRdcProtocolGuid = EFI_RDC_PROTOCOL_GUID;



EFI_STATUS RdcCommunicate(RDCCOMMUNICATE_PRIVATE *private)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT16 aec_type = aec_get_type();

	if(aec_type == AEC_TYPE_RDC_FW)
	{
		if((private->Arg1 == RDC_FW_CMD_PMC0) || (private->Arg1 == RDC_FW_CMD_PMC1))	//status
		{
			status = ProtocolStatus(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else if(private->Arg1 & rdc_read)		//read
		{
			status = rdc_ec_read_prot(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else		//write
		{
			status = rdc_ec_write_prot(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
	}
	else if(aec_type == AEC_TYPE_RDC_SW)
	{
		if((private->Arg1 == RDC_SW_CMD_WDT_START) || (private->Arg1 == RDC_SW_CMD_WDT_STOP) || (private->Arg1 == RDC_SW_CMD_WDT_REF) || \
			(private->Arg1 == RDC_SW_CMD_PWR_CNT_ICS) || (private->Arg1 == RDC_SW_CMD_SAVE_CFG))	//write command only
		{
			status = SWProtocolOnlyWritePMC(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else if((private->Arg1 == RDC_SW_CMD_STORAGE_LK) || (private->Arg1 == RDC_SW_CMD_STORAGE_ULK))	//storage lock unlock
		{
			status = StorageSWProtocolSetLockUnlock(private->Arg1, private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else if((private->Arg1 == RDC_SW_CMD_WR_INFO) || (private->Arg1 == RDC_SW_CMD_RD_INFO) || \
				(private->Arg1 == RDC_SW_CMD_WR_FACTORY) || (private->Arg1 == RDC_SW_CMD_RD_FACTORY) || \
				(private->Arg1 == RDC_SW_CMD_RD_STORAGE) || (private->Arg1 == RDC_SW_CMD_WR_STORAGE)
				)	//status replace index
		{
			status = SWRWProtocolStatusReplaceIndex(private->Arg1, private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else if((private->Arg1 == RDC_SW_CMD_WR_GPIO) || (private->Arg1 == RDC_SW_CMD_RD_GPIO))	//gpio
		{
			status = EFI_UNSUPPORTED;
		}
		else if(private->Arg1 & rdc_read)		//read
		{
			status = rdc_sw_ec_read_prot(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
		else		//write
		{
			status = rdc_sw_ec_write_prot(private->Arg1, *private->Arg2, private->Arg3, private->Arg4, private->Arg5);
		}
	}
	else
	{
		status = EFI_UNSUPPORTED;
	}

	return status;
}

EFI_STATUS EFIAPI RdcECExcute(IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
									IN UINT8 Arg1,
									IN UINT8 *Arg2,
									IN UINT8 Arg3,
									IN UINT8 Arg4,
									IN UINT8 *Arg5)
{
	EFI_STATUS status = EFI_SUCCESS;
	RDCCOMMUNICATE_PRIVATE *private;

	private = (RDCCOMMUNICATE_PRIVATE*)This;

	private->Arg1 = Arg1;
	private->Arg2 = Arg2;
	private->Arg3 = Arg3;
	private->Arg4 = Arg4;
	private->Arg5 = Arg5;
	
	status = RdcCommunicate(private);
	
	return status;
}
#if 0
EFI_STATUS EFIAPI RdcECRead(IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
									IN UINT8 Arg1,
									IN UINT8 Arg2,
									IN UINT8 Arg3,
									IN UINT8 Arg4,
									IN UINT8 *Arg5)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT16 aec_type;
	RDCCOMMUNICATE_PRIVATE *private;

	aec_type = aec_get_type();
	private = (RDCCOMMUNICATE_PRIVATE*)This;

	private->Arg1 = Arg1;
	private->Arg2 = Arg2;
	private->Arg3 = Arg3;
	private->Arg4 = Arg4;
	private->Arg5 = Arg5;

	if(aec_type == AEC_TYPE_RDC_FW)
	{
		status = rdc_ec_read_prot(Arg1, Arg2, Arg3, Arg4, Arg5);
	}
	else if(aec_type == AEC_TYPE_RDC_SW)
	{
		status = rdc_sw_ec_read_prot(Arg1, Arg2, Arg3, Arg4, Arg5);
	}

	return status;
}

EFI_STATUS EFIAPI RdcECWrite(IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
									IN UINT8 Arg1,
									IN UINT8 Arg2,
									IN UINT8 Arg3,
									IN UINT8 Arg4,
									IN UINT8 *Arg5)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT16 aec_type;
	RDCCOMMUNICATE_PRIVATE *private;

	aec_type = aec_get_type();
	private = (RDCCOMMUNICATE_PRIVATE*)This;

	private->Arg1 = Arg1;
	private->Arg2 = Arg2;
	private->Arg3 = Arg3;
	private->Arg4 = Arg4;
	private->Arg5 = Arg5;

	if(aec_type == AEC_TYPE_RDC_FW)
	{
		status = rdc_ec_write_prot(Arg1, Arg2, Arg3, Arg4, Arg5);
	}
	else if(aec_type == AEC_TYPE_RDC_SW)
	{
		status = rdc_sw_ec_write_prot(Arg1, Arg2, Arg3, Arg4, Arg5);
	}

	return status;
}

EFI_STATUS EFIAPI RdcECReadStatus(IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
									IN UINT8 Arg1,
									IN UINT8 Arg2,
									IN UINT8 Arg3,
									IN UINT8 Arg4,
									IN UINT8 *Arg5)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT16 aec_type;
	RDCCOMMUNICATE_PRIVATE *private;

	aec_type = aec_get_type();
	private = (RDCCOMMUNICATE_PRIVATE*)This;

	private->Arg1 = Arg1;
	private->Arg2 = Arg2;
	private->Arg3 = Arg3;
	private->Arg4 = Arg4;
	private->Arg5 = Arg5;

	if(aec_type == AEC_TYPE_RDC_FW)
	{
		status = ProtocolStatus(Arg1, Arg2, Arg3, Arg4, Arg5);
	}
	else if(aec_type == AEC_TYPE_RDC_SW)
	{
		status = SWProtocolStatus(Arg1, Arg2, Arg3, Arg4, Arg5);
	}

	return status;
}
#endif
EFI_STATUS EFIAPI RDCProtocolDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	RDC_COMMUNICATE_PRIVATE *rdcPrivate;
	RDC_COMMUNICATE_DEVICE_PATH *rdcDevPath;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(RDC_COMMUNICATE_PRIVATE), &rdcPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(rdcPrivate, sizeof(RDC_COMMUNICATE_PRIVATE), 0);
	}
	
	handle = ImageHandle;
	
	//rdcPrivate->RDCCommunicate->Handle = Handle;
	rdcPrivate->RDCCommunicate.CmdPort = aec.acl.cmd;
	rdcPrivate->RDCCommunicate.DataPort = aec.acl.dat;
	rdcPrivate->RDCCommunicate.RDCController.RdcECExecute = RdcECExcute;
	//rdcPrivate->RDCCommunicate.RDCController.RdcECRead = RdcECRead;
	//rdcPrivate->RDCCommunicate.RDCController.RdcECWrite = RdcECWrite;
	//rdcPrivate->RDCCommunicate.RDCController.RdcECReadStatus = RdcECReadStatus;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(RDC_COMMUNICATE_DEVICE_PATH), &rdcDevPath);
	if(EFI_ERROR (status))
	{
		rdcDevPath = &gRDCCommunicateMemMapPathTemplate;
	}
	else
	{
		// Update instances content.
		gBS->CopyMem(rdcDevPath, &gRDCCommunicateMemMapPathTemplate, sizeof(RDC_COMMUNICATE_DEVICE_PATH));

		rdcDevPath->RDCCommunicateMemMapDevPath.StartingAddress = rdcPrivate->RDCCommunicate.CmdPort;
	}
	rdcPrivate->RDCCommunicate.DevPath = (EFI_DEVICE_PATH_PROTOCOL *)rdcDevPath;
	
	// Install Protocol
	status = gBS->InstallMultipleProtocolInterfaces(&rdcPrivate->RDCCommunicate.Handle, &gEfiRdcProtocolGuid, &rdcPrivate->RDCCommunicate, &gEfiDevicePathProtocolGuid, rdcPrivate->RDCCommunicate.DevPath, NULL);

	return status;
}


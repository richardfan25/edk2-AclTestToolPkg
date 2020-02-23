#include "rdcdriver.h"



//
// Platform specific SMBus device path
//
SMBUS_DEVICE_PATH gSMBusDevMemMapPathTemplate = {
	/*{
		HARDWARE_DEVICE_PATH,
		HW_MEMMAP_DP,
		(UINT8)(sizeof(MEMMAP_DEVICE_PATH)),
		(UINT8)((sizeof(MEMMAP_DEVICE_PATH)) >> 8),
		EfiMemoryMappedIO,
		0,
		0
	},*/
	{
		MESSAGING_DEVICE_PATH,
		MSG_DEVICE_LOGICAL_UNIT_DP,
		(UINT8)(sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH)),
		(UINT8)((sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH)) >> 8),
		0
	},
	{
		HARDWARE_DEVICE_PATH,
		HW_CONTROLLER_DP,
		(UINT8)(sizeof(CONTROLLER_DEVICE_PATH)),
		(UINT8)((sizeof(CONTROLLER_DEVICE_PATH)) >> 8),
		0
	},
	{
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		(sizeof(EFI_DEVICE_PATH_PROTOCOL)),
		0
	}
};



UINT8 gSMBusLDN[SMB_CH_NUM] = { RDC_LDN_SMBUS0, RDC_LDN_SMBUS1 };



EFI_STATUS RDCSMBusInit(UINT32 ch, UINT16 *bio)
{
	UINT8 act;
	UINT8 prescale1, prescale2;

	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, gSMBusLDN[ch]);

	// active
	IoWrite8(RDC_CFG_IDX_PORT, 0x30);
	act = IoRead8(RDC_CFG_DAT_PORT);
	if ((act & 0x1) == 0)
	{
		return EFI_NOT_FOUND;
	}

	// base
	IoWrite8(RDC_CFG_IDX_PORT, 0x60);	// MSB of base
	*bio = IoRead8(RDC_CFG_DAT_PORT);
	*bio <<= 8;

	IoWrite8(RDC_CFG_IDX_PORT, 0x61);	// LSB of base
	*bio |= IoRead8(RDC_CFG_DAT_PORT);

	// base : address error
	if (*bio == 0x0000 || *bio == 0xFFFF)
	{
		return EFI_NOT_FOUND;
	}

	//Reset Smbus
	IoWrite8(*bio + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);
	
	//set Smbus Receive Slave Address 0xFE, avoid scan self
	IoWrite8(*bio + SMB_RSADD_REG_OFFSET, IoRead8(*bio + SMB_RSADD_REG_OFFSET) | SMB_RSADD_SLAVEADDR_gm);
	
	//---------------------------------------------------------------------
	// SMBus clock calculation
	//---------------------------------------------------------------------
	// CPU   clock :  50 MHz
	// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
	//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
	//---------------------------------------------------------------------
	prescale1 = 50;
	IoWrite8(*bio + SMB_CLKCTL1_REG_OFFSET, prescale1);

	prescale2 = 9;
	prescale2 &= ~SMB_CLKCTL2_FAST_bm;
	IoWrite8(*bio + SMB_CLKCTL2_REG_OFFSET, prescale2);

	//gSMB[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);

	IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC: exit config
	
	return EFI_SUCCESS;
}

EFI_STATUS TransToRDCMode(IN OUT SMBUSMASTER_PRIVATE *private)
{
	switch(private->Operation)
	{
		case EfiSmbusQuickRead:
		{
			private->SlaveAddress = (private->SlaveAddress << 1) | 1;
			private->Operation = RDC_SMB_CMD_QUICK;
			break;
		}
		case EfiSmbusQuickWrite:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_QUICK;
			break;
		}
		case EfiSmbusReceiveByte:
		{
			private->SlaveAddress = (private->SlaveAddress << 1) | 1;
			private->Operation = RDC_SMB_CMD_BYTE;
			break;
		}
		case EfiSmbusSendByte:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_BYTE;
			break;
		}
		case EfiSmbusReadByte:
		{
			private->SlaveAddress = (private->SlaveAddress << 1) | 1;
			private->Operation = RDC_SMB_CMD_BYTE_DATA;
			break;
		}
		case EfiSmbusWriteByte:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_BYTE_DATA;
			break;
		}
		case EfiSmbusReadWord:
		{
			private->SlaveAddress = (private->SlaveAddress << 1) | 1;
			private->Operation = RDC_SMB_CMD_WORD_DATA;
			break;
		}
		case EfiSmbusWriteWord:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_WORD_DATA;
			break;
		}
		case EfiSmbusReadBlock:
		{
			private->SlaveAddress = (private->SlaveAddress << 1) | 1;
			private->Operation = RDC_SMB_CMD_BLOCK;
			break;
		}
		case EfiSmbusWriteBlock:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_BLOCK;
			break;
		}
		case EfiSmbusProcessCall:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_PROC_CALL;
			break;
		}
		case EfiSmbusBWBRProcessCall:
		{
			private->SlaveAddress = (private->SlaveAddress << 1);
			private->Operation = RDC_SMB_CMD_BLOCK_PROC;
			break;
		}
		default:
		{
			return EFI_INVALID_PARAMETER;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS SMBusTransaction(IN OUT SMBUSMASTER_PRIVATE *private)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT16 retry;
	UINT8 i, value, sts;
	UINT8 block_len;
	UINT16 bio;
	
	bio = private->SMBusBusIO;

	status = TransToRDCMode(private);
	if(EFI_ERROR (status))
	{
		goto exit_trans;
	}
	
	//wait smbus in use
	retry = 3000;
	while(IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_INUSE_bm)
	{
		retry--;
		if(retry == 0)
		{
			//Print(L"Smbus In Use \n");
			status = EFI_TIMEOUT;
			goto exit_trans;
		}
	}

	//check busy
	if(IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_HOSTBUSY_bm)
	{
		//kill transaction if busy
		IoWrite8(bio + SMB_CTL_REG_OFFSET, IoRead8(bio + SMB_CTL_REG_OFFSET) | SMB_CTL_KILL_bm);
		IoWrite8(bio + SMB_CTL_REG_OFFSET, IoRead8(bio + SMB_CTL_REG_OFFSET) & ~SMB_CTL_KILL_bm);
	}

	//clear all status
	IoWrite8(bio + SMB_STS_REG_OFFSET, 0xFF);
	IoWrite8(bio + SMB_STS2_REG_OFFSET, 0xFF);
	
	//clean all data reg
	IoWrite8(bio + SMB_DAT0_REG_OFFSET, SMB_DAT0_DATA0_gp);
	IoWrite8(bio + SMB_DAT1_REG_OFFSET, SMB_DAT1_DATA1_gp);

	//set smb protocol
	value = IoRead8(bio + SMB_CTL_REG_OFFSET);
	value &= ~SMB_CTL_SMBCMD_gm;
	value |= (private->Operation & RDC_SMB_CMD_MASK)<<SMB_CTL_SMBCMD_gp;	//cmd=mode
	IoWrite8(bio + SMB_CTL_REG_OFFSET, value);

	//set tx slave address
	IoWrite8(bio + SMB_SADD_REG_OFFSET, (UINT8)private->SlaveAddress);

	//set tx command
	IoWrite8(bio + SMB_CMD_REG_OFFSET, (UINT8)private->Command);
	
	//set Packet Error Check Register
	IoWrite8(bio + SMB_PEC_REG_OFFSET, private->PecCheck);

	//according SMB_CMD to set tx data
	switch(private->Operation & RDC_SMB_CMD_MASK)
	{
		case RDC_SMB_CMD_QUICK:
		{
			break;
		}
		case RDC_SMB_CMD_BYTE:
		{
			break;
		}
		case RDC_SMB_CMD_BYTE_DATA:
		{
			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				IoWrite8(bio + SMB_DAT0_REG_OFFSET, *private->RWData);
			}
			break;
		}
		case RDC_SMB_CMD_WORD_DATA:
		{
			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				IoWrite8(bio + SMB_DAT0_REG_OFFSET, *private->RWData);
				IoWrite8(bio + SMB_DAT1_REG_OFFSET, *(private->RWData+1));
			}
			break;
		}
		case RDC_SMB_CMD_PROC_CALL:
		{
			IoWrite8(bio + SMB_DAT0_REG_OFFSET, *private->RWData);
			IoWrite8(bio + SMB_DAT1_REG_OFFSET, *(private->RWData+1));
			break;
		}
		case RDC_SMB_CMD_BLOCK:
		{
			//enable 32 byte buffer
			IoWrite8(bio + SMB_CTL2_REG_OFFSET, IoRead8(bio + SMB_CTL2_REG_OFFSET) | SMB_CTL2_E32B_bm);

			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				IoWrite8(bio + SMB_DAT0_REG_OFFSET, *private->RWLength);
				for(i=0; i<*private->RWLength; i++)
				{
					IoWrite8(bio + SMB_BLK_REG_OFFSET, *(private->RWData + i));
				}
			}
			break;
		}
		case RDC_SMB_CMD_I2C_READ:
		{
			status = EFI_PROTOCOL_ERROR;
			break;
		}
		case RDC_SMB_CMD_BLOCK_PROC:
		{
			if(*private->RWLength > 32)
			{
				status = EFI_BAD_BUFFER_SIZE;
				break;
			}
			
			//enable 32 byte buffer
			IoWrite8(bio + SMB_CTL2_REG_OFFSET, IoRead8(bio + SMB_CTL2_REG_OFFSET) | SMB_CTL2_E32B_bm);

			IoWrite8(bio + SMB_DAT0_REG_OFFSET, *private->RWLength);
			for(i=0; i<*private->RWLength; i++)
			{
				IoWrite8(bio + SMB_BLK_REG_OFFSET, *(private->RWData + i));
			}
			break;
		}
		default :
		{
			status = EFI_PROTOCOL_ERROR;
			break;
		}
	}

	if(status != EFI_SUCCESS)
	{
		goto exit_trans;
	}

	//start transaction
	value = IoRead8(bio + SMB_CTL_REG_OFFSET);
	value |= SMB_CTL_START_bm;	//start=1
	value &= ~SMB_CTL_I2CNACKEN_bm;	//nack=0
	value &= ~ SMB_CTL_KILL_bm;		//kill=0
	
	if(private->Operation & RDC_SMB_CMD_PEC_MASK)
	{
		value |= SMB_CTL_PECEN_bm;	//PEC enable
	}
	else
	{
		value &= ~SMB_CTL_PECEN_bm;	// PEC disable
	}

	IoWrite8(bio + SMB_CTL_REG_OFFSET, value);

	//wait tx done
	retry = 3000;
	while(IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_HOSTBUSY_bm)
	{
		retry--;
		if(retry == 0)
		{
			//D_Print(L"Smbus Tx Timeout \n");
			status = EFI_TIMEOUT;
			goto exit_trans;
		}
	}

	//check status
	sts = IoRead8(bio + SMB_STS_REG_OFFSET);
	if(sts & SMB_STS_FAILED_bm)
	{
		//D_Print(L"Smbus Transaction Fail \n");
		status = EFI_ABORTED;
	}
	else if(sts & SMB_STS_ARLERR_bm)
	{
		//D_Print(L"Smbus Arbitration Lost \n");
		status = EFI_PROTOCOL_UNREACHABLE;
	}
	else
	{
		sts = IoRead8(bio + SMB_STS2_REG_OFFSET);
		if(sts & SMB_STS2_TOERR_bm)
		{
			//D_Print(L"Smbus Time-Out Error \n");
			status = EFI_TIMEOUT;
		}
		else if(sts & SMB_STS2_NACKERR_bm)
		{
			//D_Print(L"Smbus Not Response ACK Error \n");
			status = EFI_DEVICE_ERROR;
		}
		else if(sts & SMB_STS2_PECERR_bm)
		{
			//D_Print(L"Smbus PEC Error \n");
			status = EFI_INVALID_PARAMETER;
		}
	}

	if(status != EFI_SUCCESS)
	{
		goto exit_trans;
	}

	//read data from buffer
	switch(private->Operation & RDC_SMB_CMD_MASK)
	{
		case RDC_SMB_CMD_QUICK:
		{
			break;
		}
		case RDC_SMB_CMD_BYTE:
		case RDC_SMB_CMD_BYTE_DATA:
		{
			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_READ)
			{
				if((IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*private->RWData = IoRead8(bio + SMB_DAT0_REG_OFFSET);
				}
				else
				{
					//D_Print(L"Smbus Rx Not Ready \n");
					status = EFI_NOT_READY;
					goto exit_trans;
				}
			}
			break;
		}
		case RDC_SMB_CMD_WORD_DATA:
		{
			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_READ)
			{
				if((IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*private->RWData = IoRead8(bio + SMB_DAT0_REG_OFFSET);
					*(private->RWData + 1) = IoRead8(bio + SMB_DAT1_REG_OFFSET);
				}
				else
				{
					//D_Print(L"Smbus Rx Not Ready \n");
					status = EFI_NOT_READY;
					goto exit_trans;
				}
			}
			break;
		}
		case RDC_SMB_CMD_PROC_CALL:
		{
			*private->RWData = IoRead8(bio + SMB_DAT0_REG_OFFSET);
			*(private->RWData + 1) = IoRead8(bio + SMB_DAT1_REG_OFFSET);
			break;
		}
		case RDC_SMB_CMD_BLOCK:
		{
			if((private->SlaveAddress & SMB_RW_MASK) == SMB_RW_READ)
			{
				if((IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm))
				{
					*private->RWLength = IoRead8(bio + SMB_DAT0_REG_OFFSET);
					
					if(*private->RWLength > 32)
					{
						status = EFI_BAD_BUFFER_SIZE;
						goto exit_trans;
					}
					
					for(i=0; i<*private->RWLength; i++)
					{
						*(private->RWData + i) = IoRead8(bio + SMB_BLK_REG_OFFSET);
					}
				}
				else
				{
					//D_Print(L"Smbus Rx Not Ready \n");
					status = EFI_NOT_READY;
					goto exit_trans;
				}
			}
			break;
		}
		case RDC_SMB_CMD_I2C_READ:
		{
			break;
		}
		case RDC_SMB_CMD_BLOCK_PROC:
		{
			block_len = *private->RWLength;
			*private->RWLength = IoRead8(bio + SMB_DAT0_REG_OFFSET);
			if(block_len < *private->RWLength)
			{
				status = EFI_BUFFER_TOO_SMALL;
				goto exit_trans;
			}
			
			for(i=0; i<block_len; i++)
			{
				*(private->RWData + i) = IoRead8(bio + SMB_BLK_REG_OFFSET);
			}
			break;
		}
		default:
		{
			status = EFI_PROTOCOL_ERROR;
			goto exit_trans;
		}
	}

exit_trans:
	//disable 32 byte buffer
	IoWrite8(bio + SMB_CTL2_REG_OFFSET, IoRead8(bio + SMB_CTL2_REG_OFFSET) & ~SMB_CTL2_E32B_bm);
	//clear all status
	IoWrite8(bio + SMB_STS_REG_OFFSET, 0xFF);
	IoWrite8(bio + SMB_STS2_REG_OFFSET, 0xFF);

	return status;
}

EFI_STATUS EFIAPI Execute(
	IN CONST EFI_SMBUS_HC_PROTOCOL *This,
	IN EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
	IN EFI_SMBUS_DEVICE_COMMAND Command,
	IN EFI_SMBUS_OPERATION Operation,
	IN BOOLEAN PecCheck,
	IN OUT UINTN *Length,
	IN OUT VOID *Buffer)
{
	SMBUSMASTER_PRIVATE *private;
	
	private = (SMBUSMASTER_PRIVATE*)This;
	
	private->SlaveAddress = SlaveAddress.SmbusDeviceAddress;
	private->Command = Command;
	private->Operation = Operation;
	private->PecCheck = PecCheck;
	private->RWLength = (UINT8*)Length;
	private->RWData = Buffer;
	
	return SMBusTransaction(private);
}

EFI_STATUS EFIAPI ArpDevice(
	IN CONST EFI_SMBUS_HC_PROTOCOL *This,
	IN BOOLEAN ArpAll,
	IN EFI_SMBUS_UDID *SmbusUdid, OPTIONAL
	IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress OPTIONAL)
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS EFIAPI GetArpMap(
	IN CONST EFI_SMBUS_HC_PROTOCOL *This,
	IN OUT UINTN *Length,
	IN OUT EFI_SMBUS_DEVICE_MAP **SmbusDeviceMap)
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS EFIAPI Notify(
	IN CONST EFI_SMBUS_HC_PROTOCOL *This,
	IN CONST EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
	IN CONST UINTN Data,
	IN CONST EFI_SMBUS_NOTIFY_FUNCTION NotifyFunction)
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS EFIAPI RDCSMBusMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	SMBUS_MASTER_PRIVATE *smbusPrivate;
	UINT8 index;
	SMBUS_DEVICE_PATH *smbusDevPath;

	UINT16 bio = 0xFFFF;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(SMBUS_MASTER_PRIVATE), &smbusPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(smbusPrivate, sizeof(SMBUS_MASTER_PRIVATE), 0);
	}
	
	handle = ImageHandle;
	
	for(index=0; index<SMB_CH_NUM; index++)
	{
		status = RDCSMBusInit(index, &bio);
		if(EFI_ERROR (status))
		{
			continue;
		}
		
		//smbusPrivate->SMBusMaster[index].Handle = Handle;
		smbusPrivate->SMBusMaster[index].SMBusBusIO = bio;
		smbusPrivate->SMBusMaster[index].SMBusBusId = index;
		smbusPrivate->SMBusMaster[index].SMBusController.Execute = Execute;
		smbusPrivate->SMBusMaster[index].SMBusController.ArpDevice = ArpDevice;
		smbusPrivate->SMBusMaster[index].SMBusController.GetArpMap = GetArpMap;
		smbusPrivate->SMBusMaster[index].SMBusController.Notify = Notify;

		status = gBS->AllocatePool(EfiBootServicesData, sizeof(SMBUS_DEVICE_PATH), &smbusDevPath);
		if(EFI_ERROR (status))
		{
			smbusDevPath = &gSMBusDevMemMapPathTemplate;
		}
		else
		{
			// Update instances content.
			gBS->CopyMem(smbusDevPath, &gSMBusDevMemMapPathTemplate, sizeof(SMBUS_DEVICE_PATH));

			smbusDevPath->SMBusBus.ControllerNumber = smbusPrivate->SMBusMaster[index].SMBusBusId;
			smbusDevPath->RDCSMBusLDN.Lun = gSMBusLDN[index];
			//smbusDevPath->SMBusMemMapDevPath.StartingAddress = bio;
		}
		smbusPrivate->SMBusMaster[index].DevPath = (EFI_DEVICE_PATH_PROTOCOL *)smbusDevPath;
		
		// Install Protocol
		status = gBS->InstallMultipleProtocolInterfaces(&smbusPrivate->SMBusMaster[index].Handle, &gEfiSmbusHcProtocolGuid, &smbusPrivate->SMBusMaster[index], &gEfiDevicePathProtocolGuid, smbusPrivate->SMBusMaster[index].DevPath, NULL);
	}
	
	return status;
}


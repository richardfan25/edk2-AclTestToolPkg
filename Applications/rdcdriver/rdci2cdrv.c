#include "rdcdriver.h"



//
// Platform specific I2C device path
//
I2C_DEVICE_PATH gI2cDevMemMapPathTemplate = {
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



UINT8 gI2CLDN[I2C_CH_NUM] = { RDC_LDN_I2C0, RDC_LDN_I2C1 };



EFI_STATUS EFIAPI RDCI2CInit(UINT32 ch, UINT16 *bio)
{
	UINT8 act;
	
	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, gI2CLDN[ch]);

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
	
	// reset I2C bus
	IoWrite8(*bio + I2C_EXCTL_REG_OFFSET, IoRead8(*bio + I2C_EXCTL_REG_OFFSET) | I2C_EXCTL_I2CRST_bm);

	// set SMBus receive slave address 0xFE, avoid scan self
	IoWrite8(*bio + I2C_MYADD_REG_OFFSET, IoRead8(*bio + I2C_MYADD_REG_OFFSET) | I2C_MYADD_MYADDR_gm);
	
	IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC: exit config
	
	return EFI_SUCCESS;
}

EFI_STATUS I2CWaitBusy(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while((IoRead8(bio + I2C_STS_REG_OFFSET) & I2C_STS_BBUSY_bm) == 1)
	{
		retry--;
		if(retry == 0)
		{
			//D_Print(L"I2C Busy Timeout \n");
			return EFI_TIMEOUT;
		}
		gBS->Stall(1);
	}

	return EFI_SUCCESS;
}

EFI_STATUS I2CWaitTxDone(UINT32 bio)
{
	UINT8 status;
	UINT16 retry = 3000;

	while(retry)
	{
		gBS->Stall(1);
		status = IoRead8(bio + I2C_STS_REG_OFFSET);
		if(status & 0x10)
		{
			//Print(L"I2C Not Response ACK Error \n");
			return EFI_DEVICE_ERROR;
		}
		else if(status & 0x08)
		{
			//Print(L"I2C Arbitration Lost \n");
			return EFI_PROTOCOL_UNREACHABLE;
		}
		else if(status & 0x20)
		{
			//double check, avoid tx done flag set early than not response ack flag
			status = IoRead8(bio + I2C_STS_REG_OFFSET);
			if(status & 0x10)
			{
				//Print(L"I2C Not Response ACK Error \n");
				return EFI_DEVICE_ERROR;
			}
			else if(status & 0x08)
			{
				//Print(L"I2C Arbitration Lost \n");
				return EFI_PROTOCOL_UNREACHABLE;
			}
			
			break;
		}
		
		retry--;
		if(retry == 0)
		{
			//Print(L"I2C Tx Timeout \n");
			return EFI_TIMEOUT;
		}
	}

	//clean tx done status register
	IoWrite8(bio + I2C_STS_REG_OFFSET, IoRead8(bio + I2C_STS_REG_OFFSET) | I2C_STS_TXDONE_bm);
	
	return EFI_SUCCESS;
}

EFI_STATUS I2CWaitRxDone(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while((IoRead8(bio + I2C_STS_REG_OFFSET) & I2C_STS_RXRDY_bm) == 0)
	{
		retry--;
		if(retry == 0)
		{
			//D_Print(L"I2C Rx Timeout \n");
			return EFI_TIMEOUT;
		}
		gBS->Stall(1);
	}

	//clean rx done status register
	IoWrite8(bio + I2C_STS_REG_OFFSET, IoRead8(bio + I2C_STS_REG_OFFSET) | I2C_STS_RXRDY_bm);

	return EFI_SUCCESS;
}

EFI_STATUS I2CWaitStopDone(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while(IoRead8(bio + I2C_CTL_REG_OFFSET) & I2C_CTL_STOP_bm)
	{
		retry--;
		if(retry == 0)
		{
			//D_Print(L"I2C Stop Timeout \n");
			return EFI_TIMEOUT;
		}
		gBS->Stall(1);
	}

	return EFI_SUCCESS;
}

EFI_STATUS I2cWriteRead(IN OUT I2CMASTER_PRIVATE *Private)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 Base;
	UINT32 Index;

	Base = Private->I2cBusIO;  // For convenience

	if(I2CWaitBusy(Base))
	{
		return EFI_NOT_READY;
	}
	
	//clean status register
	IoWrite8(Base + I2C_STS_REG_OFFSET, 0xFF);
	
	if((Private->WriteCount == 0) && (Private->ReadCount == 0))		//check device busy
	{
		//send addr
		IoWrite8(Base + I2C_TXADD_REG_OFFSET, (UINT8)Private->SlaveAddress);
		//wait tx done
		Status = I2CWaitTxDone(Base);
		if(Status != EFI_SUCCESS)
		{
			goto err_stop;
		}
	}
	else
	{
		//write
		if(0 < Private->WriteCount)
		{
			//send addr
			IoWrite8(Base + I2C_TXADD_REG_OFFSET, (UINT8)Private->SlaveAddress);
			//wait tx done
			Status = I2CWaitTxDone(Base);
			if(Status != EFI_SUCCESS)
			{
				goto err_stop;
			}
			
			for(Index = 0; Index < Private->WriteCount ; Index++)
			{
				//send data
				IoWrite8(Base + I2C_DAT_REG_OFFSET, *(Private->WriteData++));
				//wait tx done
				Status = I2CWaitTxDone(Base);
				if(Status != EFI_SUCCESS)
				{
					goto err_stop;
				}
				
				gBS->Stall(100);
			}
			
			//clean tx done status register
			IoWrite8(Base + I2C_STS_REG_OFFSET, IoRead8(Base + I2C_STS_REG_OFFSET) | I2C_STS_TXDONE_bm);
		}

		//read
		if(0 < Private->ReadCount)
		{
			//send addr
			IoWrite8(Base + I2C_TXADD_REG_OFFSET, (UINT8)Private->SlaveAddress | 0x01);
			//wait tx done
			Status = I2CWaitTxDone(Base);
			if(Status != EFI_SUCCESS)
			{
				goto err_stop;
			}
			
			//dummy read to trigger read process
			IoRead8(Base + I2C_DAT_REG_OFFSET);
			
			for(Index = 0; Index < Private->ReadCount; Index++)
			{
				if(Index == (Private->ReadCount - 1))
				{
					//set stop flag
					IoWrite8(Base + I2C_CTL_REG_OFFSET, IoRead8(Base + I2C_CTL_REG_OFFSET) | I2C_CTL_STOP_bm);
				}
				
				//Waiting for Rx data ready
				Status = I2CWaitRxDone(Base);
				if(Status != EFI_SUCCESS)
				{
					goto err_stop;
				}
				
				*(Private->ReadData++) = IoRead8(Base + I2C_DAT_REG_OFFSET);
			}
				
			goto end;
		}
	}
	
err_stop:
	//set stop flag
	IoWrite8(Base + I2C_CTL_REG_OFFSET, IoRead8(Base + I2C_CTL_REG_OFFSET) | I2C_CTL_STOP_bm);

end:
	return Status;
}

EFI_STATUS I2cAccess(IN I2CMASTER_PRIVATE *Private, IN UINTN SlaveAddress, IN OUT EFI_I2C_REQUEST_PACKET *RequestPacket)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN IsWriteOperation;

	//Print(L"I2cAccess: start\n");

	if(RequestPacket->Operation[0].Flags & (I2C_FLAG_SMBUS_PEC | I2C_FLAG_SMBUS_PROCESS_CALL))
	{
		Status = EFI_UNSUPPORTED;
		//Print(L"I2cAccess: %r\n",Status);
		return Status;
	}

	if(RequestPacket->OperationCount == 1)
	{
		if(RequestPacket->Operation[0].Flags & I2C_FLAG_READ)
		{
			//command READ
			Private->ReadCount = RequestPacket->Operation[0].LengthInBytes;
			Private->ReadData = RequestPacket->Operation[0].Buffer;
			Private->WriteCount = 0;
			Private->WriteData = NULL;
		}
		else
		{
			//command WRITE
			Private->ReadCount = 0;
			Private->ReadData = NULL;
			Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
			Private->WriteData = RequestPacket->Operation[0].Buffer;
		}
	}
	else if(RequestPacket->OperationCount == 2)
	{
		IsWriteOperation = (RequestPacket->Operation[1].Flags & I2C_FLAG_READ) ? FALSE : TRUE;
		if(RequestPacket->Operation[1].Flags & I2C_FLAG_READ)
		{
			//ReceviedByte
			Private->ReadCount = RequestPacket->Operation[1].LengthInBytes;
			Private->ReadData = RequestPacket->Operation[1].Buffer;
			Private->WriteCount = RequestPacket->Operation[0].LengthInBytes;
			Private->WriteData = RequestPacket->Operation[0].Buffer;
		}
		else
		{
			//only READ operation has two OperationCount
			Status = EFI_INVALID_PARAMETER;
			//Print(L"I2cAccess: %r\n",Status);
			return Status;
		}
	}
	Private->TransferCount = Private->ReadCount + Private->WriteCount;

	//Set target device slave address
	Private->SlaveAddress = (SlaveAddress << 1);
	
	Status = I2cWriteRead(Private);

	//Print(L"I2cAccess: %r\n",Status);

	return Status;
}

EFI_STATUS EFIAPI SetBusFrequency(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN *speed)
{
	UINT8 prescale1, prescale2;
	I2CMASTER_PRIVATE *private;
	UINT16 bio;

	private = (I2CMASTER_PRIVATE*)This;
	bio = private->I2cBusIO;

	switch(*speed)
	{
		case SS_SPEED:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			IoWrite8(bio + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 9;
			prescale2 &= ~I2C_CLK2_FAST_bm;
			IoWrite8(bio + I2C_CLK2_REG_OFFSET, prescale2);

			private->ClockFrequency = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case FS_SPEED:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 25 / 5 = 50000 kHz / 125 = 400 kHz
			//---------------------------------------------------------------------
			prescale1 = 15;
			IoWrite8(bio + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 7;
			prescale2 |= I2C_CLK2_FAST_bm;
			IoWrite8(bio + I2C_CLK2_REG_OFFSET, prescale2);
			
			IoWrite8(bio + I2C_EXCTL_REG_OFFSET, I2C_EXCTL_NOFILTER_bm);
			
			private->ClockFrequency = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case FP_SPEED:
		{
			//---------------------------------------------------------------------
			// I2C clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 1 = 50000 kHz / 50 = 1000 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			IoWrite8(bio + I2C_CLK1_REG_OFFSET, prescale1);

			prescale2 = 0;
			prescale2 |= I2C_CLK2_FAST_bm;
			IoWrite8(bio + I2C_CLK2_REG_OFFSET, prescale2);

			private->ClockFrequency = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI Reset(IN CONST EFI_I2C_MASTER_PROTOCOL *This)
{
	I2CMASTER_PRIVATE *private;
	UINT16 bio;
	
	private = (I2CMASTER_PRIVATE*)This;
	bio = private->I2cBusIO;
	
	// reset I2C bus
	IoWrite8(bio + I2C_EXCTL_REG_OFFSET, IoRead8(bio + I2C_EXCTL_REG_OFFSET) | I2C_EXCTL_I2CRST_bm);

	// set I2C Bus receive slave address 0xFE, avoid scan self
	IoWrite8(bio + I2C_MYADD_REG_OFFSET, IoRead8(bio + I2C_MYADD_REG_OFFSET) | I2C_MYADD_MYADDR_gm);
	
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI StartRequest(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN SlaveAddress, IN EFI_I2C_REQUEST_PACKET *RequestPacket, IN EFI_EVENT Event OPTIONAL, OUT EFI_STATUS *I2cStatus OPTIONAL)
{
	EFI_STATUS status = EFI_SUCCESS;

	//Print(L"StartRequest: start\n");

	status = I2cAccess((I2CMASTER_PRIVATE*)This, SlaveAddress, RequestPacket);
	//Print(L"StartRequest: %r\n",Status);

	return status;
}

EFI_STATUS EFIAPI RDCI2cMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	I2C_MASTER_PRIVATE *i2cPrivate;
	EFI_I2C_CONTROLLER_CAPABILITIES *i2cControllerCapabilities;
	UINT8 index;
	I2C_DEVICE_PATH *i2cDevPath;
	UINTN busclockhertz;

	UINT16 bio = 0xFFFF;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(I2C_MASTER_PRIVATE), &i2cPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(i2cPrivate, sizeof(I2C_MASTER_PRIVATE), 0);
	}

	status = gBS->AllocatePool(EfiBootServicesData, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), &i2cControllerCapabilities);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem (i2cControllerCapabilities, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), 0);
	}

	//Init I2C cotroller capabilities data structure
	i2cControllerCapabilities->StructureSizeInBytes = sizeof(EFI_I2C_CONTROLLER_CAPABILITIES);
	i2cControllerCapabilities->MaximumReceiveBytes = 0xFFFFFFFF;
	i2cControllerCapabilities->MaximumTransmitBytes = 0xFFFFFFFF;
	i2cControllerCapabilities->MaximumTotalBytes = 0xFFFFFFFF;

	handle = ImageHandle;
	
	for(index=0; index<I2C_CH_NUM; index++)
	{
		status = RDCI2CInit(index, &bio);
		if(EFI_ERROR (status))
		{
			continue;
		}
		
		//I2cPrivate->I2cMaster[index].Handle = Handle;
		i2cPrivate->I2cMaster[index].I2cBusIO = bio;
		i2cPrivate->I2cMaster[index].I2cBusId = index;
		i2cPrivate->I2cMaster[index].I2cController.SetBusFrequency = SetBusFrequency;
		i2cPrivate->I2cMaster[index].I2cController.Reset = Reset;
		i2cPrivate->I2cMaster[index].I2cController.StartRequest = StartRequest;
		i2cPrivate->I2cMaster[index].I2cController.I2cControllerCapabilities = i2cControllerCapabilities;
		//I2cInit(&i2cPrivate->i2cMaster[index]);
		
		//Set default to Standard Speed
		busclockhertz = SS_SPEED;
		SetBusFrequency(&i2cPrivate->I2cMaster[index].I2cController, &busclockhertz);

		status = gBS->AllocatePool(EfiBootServicesData, sizeof(I2C_DEVICE_PATH), &i2cDevPath);
		if(EFI_ERROR (status))
		{
			i2cDevPath = &gI2cDevMemMapPathTemplate;
		}
		else
		{
			// Update instances content.
			gBS->CopyMem(i2cDevPath, &gI2cDevMemMapPathTemplate, sizeof(I2C_DEVICE_PATH));

			i2cDevPath->I2cBus.ControllerNumber = i2cPrivate->I2cMaster[index].I2cBusId;
			i2cDevPath->RDCI2cLDN.Lun = gI2CLDN[index];
			//i2cDevPath->I2cMemMapDevPath.StartingAddress = bio;
		}
		i2cPrivate->I2cMaster[index].DevPath = (EFI_DEVICE_PATH_PROTOCOL *)i2cDevPath;

		// Install Protocol
		status = gBS->InstallMultipleProtocolInterfaces(&i2cPrivate->I2cMaster[index].Handle, &gEfiI2cMasterProtocolGuid, &i2cPrivate->I2cMaster[index], &gEfiDevicePathProtocolGuid, i2cPrivate->I2cMaster[index].DevPath, NULL);
	}

	return status;
}



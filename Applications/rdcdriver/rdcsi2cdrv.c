#include "rdcdriver.h"



//
// Platform specific SI2C device path
//
SI2C_DEVICE_PATH gSi2cDevMemMapPathTemplate = {
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



UINT8 gSI2CLDN[SMB_CH_NUM] = { RDC_LDN_SMBUS0, RDC_LDN_SMBUS1 };



EFI_STATUS EFIAPI RDCSi2CInit(UINT32 ch, UINT16 *bio)
{
	UINT8 act;
	UINT8 prescale1, prescale2;

	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, gSI2CLDN[ch]);

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

EFI_STATUS Si2cWaitBusy(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while((IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_HOSTBUSY_bm) == 1)
	{
		retry--;
		if(retry == 0)
		{
			//D_Print(L"SI2C Busy Timeout \n");
			return EFI_TIMEOUT;
		}
		gBS->Stall(1);
	}

	return EFI_SUCCESS;
}

EFI_STATUS Si2cWaitTxDone(UINT32 bio)
{
	UINT8 status, status2;
	UINT16 retry = 3000;

	while(retry)
	{
		gBS->Stall(1);
		status = IoRead8(bio + SMB_STS_REG_OFFSET);
		status2 = IoRead8(bio + SMB_STS2_REG_OFFSET);

		if(status & SMB_STS_ARLERR_bm)
		{
			//Print(L"Smbus Arbitration Lost \n");
			return EFI_PROTOCOL_UNREACHABLE;
		}
		else if(status2 & SMB_STS2_NACKERR_bm)
		{
			//Print(L"Smbus Not Response ACK Error \n");
			return EFI_DEVICE_ERROR;
		}
		else if(status & SMB_STS_TXDONE_bm)
		{
			break;
		}
		
		retry--;
		if(retry == 0)
		{
			//Print(L"Smbus Tx Timeout \n");
			return EFI_TIMEOUT;
		}
	}

	//clean tx done status register
	IoWrite8(bio + SMB_STS_REG_OFFSET, IoRead8(bio + SMB_STS_REG_OFFSET) | SMB_STS_TXDONE_bm);
	
	return EFI_SUCCESS;
}

EFI_STATUS Si2cWaitRxDone(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while((IoRead8(bio + SMB_STS_REG_OFFSET) & SMB_STS_RXRDY_bm) == 0)
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
	IoWrite8(bio + SMB_STS_REG_OFFSET, IoRead8(bio + SMB_STS_REG_OFFSET) | SMB_STS_RXRDY_bm);

	return EFI_SUCCESS;
}

EFI_STATUS Si2cWaitStopDone(UINT32 bio)
{
	UINT16 retry = 3000;
	
	while(IoRead8(bio + SMB_CTL_REG_OFFSET) & SMB_CTL_LASTBYTE_bm)
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

EFI_STATUS Si2cWriteRead(IN OUT SI2CMASTER_PRIVATE *Private)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 Base;
	UINT32 Index;

	Base = Private->Si2cBusIO;  // For convenience

	//set Smbus mapping to I2C enable
	IoWrite8(Base + SMB_CTL2_REG_OFFSET, IoRead8(Base + SMB_CTL2_REG_OFFSET) | SMB_CTL2_I2CEN_bm);
	
	if(Si2cWaitBusy(Base))
	{
		return EFI_NOT_READY;
	}
	
	//clean status register
	IoWrite8(Base + SMB_STS_REG_OFFSET, IoRead8(Base + SMB_STS_REG_OFFSET) | SMB_STS_TXDONE_bm | SMB_STS_RXRDY_bm | SMB_STS_ARLERR_bm);
	IoWrite8(Base + SMB_STS2_REG_OFFSET, IoRead8(Base + SMB_STS2_REG_OFFSET) | SMB_STS2_NACKERR_bm);
	
	if((Private->WriteCount == 0) && (Private->ReadCount == 0))		//check device busy
	{
		//send addr
		IoWrite8(Base + SMB_SADD_REG_OFFSET, (UINT8)Private->SlaveAddress);
		//wait tx done
		Status = Si2cWaitTxDone(Base);
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
			IoWrite8(Base + SMB_SADD_REG_OFFSET, (UINT8)Private->SlaveAddress);
			//wait tx done
			Status = Si2cWaitTxDone(Base);
			if(Status != EFI_SUCCESS)
			{
				goto err_stop;
			}
			
			for(Index = 0; Index < Private->WriteCount ; Index++)
			{
				//send data
				IoWrite8(Base + SMB_DAT0_REG_OFFSET, *(Private->WriteData++));
				//wait tx done
				Status = Si2cWaitTxDone(Base);
				if(Status != EFI_SUCCESS)
				{
					goto err_stop;
				}
				
				gBS->Stall(100);
			}
			
			//clean tx done status register
			IoWrite8(Base + SMB_STS_REG_OFFSET, IoRead8(Base + SMB_STS_REG_OFFSET) | SMB_STS_TXDONE_bm);
		}

		//read
		if(0 < Private->ReadCount)
		{
			//send addr
			IoWrite8(Base + SMB_SADD_REG_OFFSET, (UINT8)Private->SlaveAddress | 0x01);
			//wait tx done
			Status = Si2cWaitTxDone(Base);
			if(Status != EFI_SUCCESS)
			{
				goto err_stop;
			}
			
			//dummy read to trigger read process
			IoRead8(Base + SMB_DAT0_REG_OFFSET);
			
			for(Index = 0; Index < Private->ReadCount; Index++)
			{
				if(Index == (Private->ReadCount - 1))
				{
					//set stop flag
					IoWrite8(Base + SMB_CTL_REG_OFFSET, IoRead8(Base + SMB_CTL_REG_OFFSET) | SMB_CTL_LASTBYTE_bm);
				}
				
				//Waiting for Rx data ready
				Status = Si2cWaitRxDone(Base);
				if(Status != EFI_SUCCESS)
				{
					goto err_stop;
				}
				
				*(Private->ReadData++) = IoRead8(Base + SMB_DAT0_REG_OFFSET);
			}
				
			goto end;
		}
	}
	
err_stop:
	//set stop flag
	IoWrite8(Base + SMB_CTL_REG_OFFSET, IoRead8(Base + SMB_CTL_REG_OFFSET) | SMB_CTL_LASTBYTE_bm);
	
	//set Smbus mapping to I2C disable
	IoWrite8(Base + SMB_CTL2_REG_OFFSET, IoRead8(Base + SMB_CTL2_REG_OFFSET) & ~SMB_CTL2_I2CEN_bm);

end:
	return Status;
}

EFI_STATUS Si2cAccess(IN SI2CMASTER_PRIVATE *Private, IN UINTN SlaveAddress, IN OUT EFI_I2C_REQUEST_PACKET *RequestPacket)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN IsWriteOperation;

	//Print(L"Si2cAccess: start\n");

	if(RequestPacket->Operation[0].Flags & (I2C_FLAG_SMBUS_PEC | I2C_FLAG_SMBUS_PROCESS_CALL))
	{
		Status = EFI_UNSUPPORTED;
		//Print(L"Si2cAccess: %r\n",Status);
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
			//Print(L"Si2cAccess: %r\n",Status);
			return Status;
		}
	}
	Private->TransferCount = Private->ReadCount + Private->WriteCount;

	//Set target device slave address
	Private->SlaveAddress = (SlaveAddress << 1);
	
	Status = Si2cWriteRead(Private);

	//Print(L"Si2cAccess: %r\n",Status);

	return Status;
}

EFI_STATUS EFIAPI Si2cSetBusFrequency(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN *speed)
{
	UINT8 prescale1, prescale2;
	SI2CMASTER_PRIVATE *private;
	UINT16 bio;

	private = (SI2CMASTER_PRIVATE*)This;
	bio = private->Si2cBusIO;

	switch(*speed)
	{
		case SS_SPEED:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			IoWrite8(bio + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 9;
			prescale2 &= ~SMB_CLKCTL2_FAST_bm;
			IoWrite8(bio + SMB_CLKCTL2_REG_OFFSET, prescale2);

			private->ClockFrequency = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case FS_SPEED:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 25 / 5 = 50000 kHz / 125 = 400 kHz
			//---------------------------------------------------------------------
			prescale1 = 25;
			IoWrite8(bio + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 4;
			prescale2 |= SMB_CLKCTL2_FAST_bm;
			IoWrite8(bio + SMB_CLKCTL2_REG_OFFSET, prescale2);

			private->ClockFrequency = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
		case FP_SPEED:
		{
			//---------------------------------------------------------------------
			// SMBus clock calculation
			//---------------------------------------------------------------------
			// CPU   clock :  50 MHz
			// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
			//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 1 = 50000 kHz / 50 = 1000 kHz
			//---------------------------------------------------------------------
			prescale1 = 50;
			IoWrite8(bio + SMB_CLKCTL1_REG_OFFSET, prescale1);

			prescale2 = 0;
			prescale2 |= SMB_CLKCTL2_FAST_bm;
			IoWrite8(bio + SMB_CLKCTL2_REG_OFFSET, prescale2);

			private->ClockFrequency = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
			break;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI Si2cReset(IN CONST EFI_I2C_MASTER_PROTOCOL *This)
{
	SI2CMASTER_PRIVATE *private;
	UINT16 bio;
	
	private = (SI2CMASTER_PRIVATE*)This;
	bio = private->Si2cBusIO;
	
	// reset I2C bus
	IoWrite8(bio + SMB_CTL2_REG_OFFSET, SMB_CTL2_SSRESET_bm);

	// set SMBus receive slave address 0xFE, avoid scan self
	IoWrite8(bio + SMB_RSADD_REG_OFFSET, IoRead8(bio + SMB_RSADD_REG_OFFSET) | SMB_RSADD_SLAVEADDR_gm);
	
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI Si2cStartRequest(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN SlaveAddress, IN EFI_I2C_REQUEST_PACKET *RequestPacket, IN EFI_EVENT Event OPTIONAL, OUT EFI_STATUS *I2cStatus OPTIONAL)
{
	EFI_STATUS status = EFI_SUCCESS;

	//Print(L"StartRequest: start\n");

	status = Si2cAccess((SI2CMASTER_PRIVATE*)This, SlaveAddress, RequestPacket);
	//Print(L"StartRequest: %r\n",Status);

	return status;
}

EFI_STATUS EFIAPI RDCSi2cMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	SI2C_MASTER_PRIVATE *si2cPrivate;
	EFI_I2C_CONTROLLER_CAPABILITIES *si2cControllerCapabilities;
	UINT8 index;
	SI2C_DEVICE_PATH *si2cDevPath;
	UINTN busclockhertz;

	UINT16 bio = 0xFFFF;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(SI2C_MASTER_PRIVATE), &si2cPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(si2cPrivate, sizeof(SI2C_MASTER_PRIVATE), 0);
	}

	status = gBS->AllocatePool(EfiBootServicesData, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), &si2cControllerCapabilities);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem (si2cControllerCapabilities, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), 0);
	}

	//Init I2C cotroller capabilities data structure
	si2cControllerCapabilities->StructureSizeInBytes = sizeof(EFI_I2C_CONTROLLER_CAPABILITIES);
	si2cControllerCapabilities->MaximumReceiveBytes = 0xFFFFFFFF;
	si2cControllerCapabilities->MaximumTransmitBytes = 0xFFFFFFFF;
	si2cControllerCapabilities->MaximumTotalBytes = 0xFFFFFFFF;

	handle = ImageHandle;
	
	for(index=0; index<I2C_CH_NUM; index++)
	{
		status = RDCSi2CInit(index, &bio);
		if(EFI_ERROR (status))
		{
			continue;
		}
		
		//si2cPrivate->Si2cMaster[index].Handle = Handle;
		si2cPrivate->Si2cMaster[index].Si2cBusIO = bio;
		si2cPrivate->Si2cMaster[index].Si2cBusId = index;
		si2cPrivate->Si2cMaster[index].Si2cController.SetBusFrequency = Si2cSetBusFrequency;
		si2cPrivate->Si2cMaster[index].Si2cController.Reset = Si2cReset;
		si2cPrivate->Si2cMaster[index].Si2cController.StartRequest = Si2cStartRequest;
		si2cPrivate->Si2cMaster[index].Si2cController.I2cControllerCapabilities = si2cControllerCapabilities;
		//Si2cInit(&si2cPrivate->si2cMaster[index]);
		
		//Set default to Standard Speed
		busclockhertz = SS_SPEED;
		Si2cSetBusFrequency(&si2cPrivate->Si2cMaster[index].Si2cController, &busclockhertz);

		status = gBS->AllocatePool(EfiBootServicesData, sizeof(SI2C_DEVICE_PATH), &si2cDevPath);
		if(EFI_ERROR (status))
		{
			si2cDevPath = &gSi2cDevMemMapPathTemplate;
		}
		else
		{
			// Update instances content.
			gBS->CopyMem(si2cDevPath, &gSi2cDevMemMapPathTemplate, sizeof(SI2C_DEVICE_PATH));

			si2cDevPath->Si2cBus.ControllerNumber = si2cPrivate->Si2cMaster[index].Si2cBusId + 2;//smbus 0 1, si2c 2 3
			si2cDevPath->RDCSi2cLDN.Lun = gSI2CLDN[index];
			//i2cDevPath->I2cMemMapDevPath.StartingAddress = bio;
		}
		si2cPrivate->Si2cMaster[index].DevPath = (EFI_DEVICE_PATH_PROTOCOL *)si2cDevPath;

		// Install Protocol
		status = gBS->InstallMultipleProtocolInterfaces(&si2cPrivate->Si2cMaster[index].Handle, &gEfiI2cMasterProtocolGuid, &si2cPrivate->Si2cMaster[index], &gEfiDevicePathProtocolGuid, si2cPrivate->Si2cMaster[index].DevPath, NULL);
	}

	return status;
}



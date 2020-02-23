#include "rdcdriver.h"



//
// Platform specific GPIO device path
//
GPIO_DEVICE_PATH gGPIODevMemMapPathTemplate = {
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
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		(sizeof(EFI_DEVICE_PATH_PROTOCOL)),
		0
	}
};



EFI_GUID gEfiGpioProtocolGuid = EFI_GPIO_PROTOCOL_GUID;



EFI_STATUS EFIAPI RDCGPIOInit(UINT16 *bio)
{
	UINT8 act;

	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, RDC_LDN_GPIO0);

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

	//Reset CAN Bus
	IoWrite8(*bio + CAN_GLB_CTL, IoRead8(*bio + CAN_GLB_CTL) | CAN_GLB_CTL_RST_bm);

	IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC: exit config

	return EFI_SUCCESS;
}

EFI_STATUS GPIODataRW(GPIOCONTROL_PRIVATE *private)
{
	UINT16 bio;
	UINT8 port;
	UINT8 *data;
	
	bio = private->GPIOIO;
	port = private->Port;
	data = private->Data;
	
	//set data
	if(private->Flag == GPIO_WRITE)
	{
		IoWrite8(bio + RDC_GPIO_DATA_OFFSET + port, *data);
	}
	else if(private->Flag == GPIO_READ)
	{
		*data =IoRead8 (bio + RDC_GPIO_DATA_OFFSET + port);
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS GPIODirectionRW(GPIOCONTROL_PRIVATE *private)
{
	UINT16 bio;
	UINT8 port;
	UINT8 *direction;
	
	bio = private->GPIOIO;
	port = private->Port;
	direction = private->Direction;
	
	//set direction
	if(private->Flag == GPIO_WRITE)
	{
		IoWrite8(bio + RDC_GPIO_DIRECTION_OFFSET + port, *direction);
	}
	else if(private->Flag == GPIO_READ)
	{
		*direction = IoRead8(bio + RDC_GPIO_DIRECTION_OFFSET + port);
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS GPIOInterruptStatusRW(GPIOCONTROL_PRIVATE *private)
{
	UINT16 bio;
	UINT8 port;
	UINT8 *status;
	
	bio = private->GPIOIO;
	port = private->Port;
	status = private->Status;
	
	//set direction
	if(private->Flag == GPIO_WRITE)
	{
		IoWrite8(bio + RDC_GPIO_INTERRUPT_STATUS_OFFSET + port, *status);
	}
	else if(private->Flag == GPIO_READ)
	{
		*status = IoRead8(bio + RDC_GPIO_INTERRUPT_STATUS_OFFSET + port);
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS GPIOInterruptControlRW(GPIOCONTROL_PRIVATE *private)
{
	UINT16 bio;
	UINT8 port;
	UINT32 *control;
	
	bio = private->GPIOIO;
	port = private->Port;
	control = private->Control;
	
	//set direction
	if(private->Flag == GPIO_WRITE)
	{
		IoWrite32(bio + RDC_GPIO_INTERRUPT_CONTROL_OFFSET + (port * 4), *control);
	}
	else if(private->Flag == GPIO_READ)
	{
		*control = IoRead32(bio + RDC_GPIO_INTERRUPT_CONTROL_OFFSET + (port * 4));
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI GPIOData(IN CONST EFI_GPIO_CONTROL_PROTOCOL *This, IN UINT8 Flag, IN UINT8 Port, IN OUT UINT8 *Data)
{
	EFI_STATUS status = EFI_SUCCESS;
	GPIOCONTROL_PRIVATE *private;

	private = (GPIOCONTROL_PRIVATE*)This;
	
	private->Flag = Flag;
	private->Port = Port;
	private->Data = Data;
	
	status = GPIODataRW(private);
	
	return status;
}

EFI_STATUS EFIAPI GPIODirection(IN CONST EFI_GPIO_CONTROL_PROTOCOL *This, IN UINT8 Flag, IN UINT8 Port, IN OUT UINT8 *Direction)
{
	EFI_STATUS status = EFI_SUCCESS;
	GPIOCONTROL_PRIVATE *private;

	private = (GPIOCONTROL_PRIVATE*)This;
	
	private->Flag = Flag;
	private->Port = Port;
	private->Direction = Direction;
	
	status = GPIODirectionRW(private);
	
	return status;
}

EFI_STATUS EFIAPI GPIOInterruptStatus(IN CONST EFI_GPIO_CONTROL_PROTOCOL *This, IN UINT8 Flag, IN UINT8 Port, IN OUT UINT8 *State)
{
	EFI_STATUS status = EFI_SUCCESS;
	GPIOCONTROL_PRIVATE *private;

	private = (GPIOCONTROL_PRIVATE*)This;
	
	private->Flag = Flag;
	private->Port = Port;
	private->Status = State;
	
	status = GPIOInterruptStatusRW(private);
	
	return status;
}

EFI_STATUS EFIAPI GPIOInterruptControl(IN CONST EFI_GPIO_CONTROL_PROTOCOL *This, IN UINT8 Flag, IN UINT8 Port, IN OUT UINT32 *Control)
{
	EFI_STATUS status = EFI_SUCCESS;
	GPIOCONTROL_PRIVATE *private;

	private = (GPIOCONTROL_PRIVATE*)This;
	
	private->Flag = Flag;
	private->Port = Port;
	private->Control = Control;
	
	status = GPIOInterruptControlRW(private);
	
	return status;
}

EFI_STATUS EFIAPI RDCGPIOMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	GPIO_CONTROL_PRIVATE *gpioPrivate;
	GPIO_DEVICE_PATH *gpioDevPath;
	
	UINT16 bio = 0xFFFF;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(GPIO_CONTROL_PRIVATE), &gpioPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(gpioPrivate, sizeof(GPIO_CONTROL_PRIVATE), 0);
	}
	
	handle = ImageHandle;
	
	status = RDCGPIOInit(&bio);
	if(EFI_ERROR (status))
	{
		return status;
	}
	
	//gpioPrivate->RDCCommunicate->Handle = Handle;
	gpioPrivate->GPIOControl.GPIOIO = bio;
	gpioPrivate->GPIOControl.GPIOController.GPIODirection = GPIODirection;
	gpioPrivate->GPIOControl.GPIOController.GPIOData = GPIOData;
	gpioPrivate->GPIOControl.GPIOController.GPIOInterruptStatus = GPIOInterruptStatus;
	gpioPrivate->GPIOControl.GPIOController.GPIOInterruptControl = GPIOInterruptControl;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(GPIO_DEVICE_PATH), &gpioDevPath);
	if(EFI_ERROR (status))
	{
		gpioDevPath = &gGPIODevMemMapPathTemplate;
	}
	else
	{
		// Update instances content.
		gBS->CopyMem(gpioDevPath, &gGPIODevMemMapPathTemplate, sizeof(GPIO_DEVICE_PATH));

		gpioDevPath->RDCGpioLDN.Lun = RDC_LDN_GPIO0;
	}
	gpioPrivate->GPIOControl.DevPath = (EFI_DEVICE_PATH_PROTOCOL *)gpioDevPath;
	
	// Install Protocol
	status = gBS->InstallMultipleProtocolInterfaces(&gpioPrivate->GPIOControl.Handle, &gEfiGpioProtocolGuid, &gpioPrivate->GPIOControl, &gEfiDevicePathProtocolGuid, gpioPrivate->GPIOControl.DevPath, NULL);

	return status;
}



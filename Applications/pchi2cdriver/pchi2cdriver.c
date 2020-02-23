#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>

#include "dwi2cregs.h"
#include "pchi2cdriver.h"
#include "x86hw.h"


#define MicroSecondDelay(n)	{ gBS->Stall(n); }

//
// Platform specific I2C device path
//
I2C_DEVICE_PATH gI2cDevMemMapPathTemplate = {
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

UINT32 gAMDI2cSdpAddressList[] = {0xFEDC2000,   0xFEDC3000,   0xFEDC4000,   0xFEDC5000,   0xFEDC6000,   0xFEDCB000};
UINT32 gI2CControllerCount = 0;
UINT16 gVendor = 0;	//8086:Intel   1022:AMD



UINT8 IntelPCIConfig(PCI_BUS *pbus, PCI_DEVICE *pi2c, UINT32 *baddr, UINT8 i)
{
	if(pi2c->vendor_id == 0x8086)
	{
		//if pci Configuration - base address not config, next
		if((pi2c->bar[0] == 0xFFFFFFFF) || (pi2c->bar[0] == 0))
		{
			return 1;
		}
		
		baddr[i] = (pi2c->bar[0] & 0xFFFFFFE0);	// 10h
		//if((MmioRead32(baddr[i] + DW_IC_CON) == 0xFFFFFFFF) || (MmioRead32(baddr[i] + DW_IC_CON) == 0))
		//{
		//	return 2;
		//}
	}
	else
	{
		return 3;
	}

	return 0;
}

UINT8 AMDPCIConfig(PCI_BUS *pbus, PCI_DEVICE *pi2c, UINT32 *baddr, UINT8 i)
{
	if(pi2c->vendor_id == 0x1022)
	{
		//if pci Configuration - base address not config, next
		if((pi2c->bar[2] == 0xFFFFFFFF) || (pi2c->bar[2] == 0))
		{
			return 1;
		}
		
		baddr[i] = (pi2c->bar[2] & 0xFFFFFFE0);	// 10h
		//if((MmioRead32(baddr[i] + DW_IC_CON) == 0xFFFFFFFF) || (MmioRead32(baddr[i] + DW_IC_CON) == 0))
		//{
		//	return 2;
		//}
	}
	else
	{
		return 3;
	}

	return 0;
}

UINT8 CoffeeLake(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_MASTER_PRIVATE *I2cPrivate, UINT32 *baddr)
{
	UINT8 i, cnt = 0;
	
	// Serial Controlller - I2C
	if(pci_find_class_subclass_code(pbus, 0x0C, 0x80))
	{
		for(i=0; i<4; i++)
		{
			pi2c = pci_find_device(pbus, 0xA368 + i);

			if(pi2c)
			{
				if(IntelPCIConfig(pbus, pi2c, baddr, cnt) == 0)
				{
					I2cPrivate->I2cMaster[cnt].I2cBusId = i;
					cnt++;
				}
			}
		}
		gI2CControllerCount = cnt;
	}
	else
	{
		//Print(L"Lpss I2C Controlller Not Found !! \n\n");
		return 1;
	}

	return 0;
}

UINT8 ApolloLake(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_MASTER_PRIVATE *I2cPrivate, UINT32 *baddr)
{
	UINT8 i, cnt = 0;
	
	// Serial Controlller - I2C
	if(pci_find_class_subclass_code(pbus, 0x11, 0x80))
	{
		for(i=0; i<8; i++)
		{
			pi2c = pci_find_device(pbus, 0x5AAC + (i * 2));

			if(pi2c)
			{
				if(IntelPCIConfig(pbus, pi2c, baddr, cnt) == 0)
				{
					I2cPrivate->I2cMaster[cnt].I2cBusId = i;
					cnt++;
				}
			}
		}
		gI2CControllerCount = cnt;
	}
	else
	{
		//Print(L"Lpss I2C Controlller Not Found !! \n\n");
		return 1;
	}

	return 0;
}

UINT8 AMD17H(PCI_BUS *pbus, PCI_DEVICE *pi2c, I2C_MASTER_PRIVATE *I2cPrivate, UINT32 *baddr)
{
	UINT8 i ,cnt = 0;
	
	for(i=0; i<5; i++)
	{
		if(i<2)
		{
			pi2c = pci_find_device(pbus, 0x15E6);
			if(pi2c == NULL)
			{
				continue;
			}
			
			if(pi2c->vendor_id == 0x1022)
			{
				if(pi2c)
				{
					if(AMDPCIConfig(pbus, pi2c, baddr, cnt) == 0)
					{
						I2cPrivate->I2cMaster[cnt].I2cBusId = i;
						I2cPrivate->I2cMaster[cnt].Mp2 = 1;
						cnt++;
					}
				}
			}
		}
		else
		{
			if(i == 2)
			{
				if((MmioRead8(0xFED80D71) != 0x01) && (MmioRead8(0xFED80D72) != 0x01))	//check io is config to i2c
				{
					continue;
				}
			}
			if(i == 3)
			{
				if((MmioRead8(0xFED80D13) != 0x01) && (MmioRead8(0xFED80D14) != 0x01))	//check io is config to i2c
				{
					continue;
				}
			}
			//if(i == 4)		//I2C4 is dedicated for USBC I2C purpose on 17H
			//{
			//	continue;
			//}
			I2cPrivate->I2cMaster[cnt].I2cBusId = i;
			baddr[cnt] = gAMDI2cSdpAddressList[i];
			cnt++;
		}
	}
	gI2CControllerCount = cnt;
	
	return 0;
}

UINT8 I2CFindControllerAndBaseAddress(I2C_MASTER_PRIVATE *I2cPrivate, UINT32 *baddr)
{
	PCI_BUS		*pbus;
	PCI_DEVICE	*pdev;
	PCI_DEVICE	*pi2c = NULL;

	UINT32 family = 0, model = 0, buf32[4];

	/*if(EFIPCIInit())
	{
		Print(L"PCI Initial Fail  !! \n\n");
		return NULL;
	}*/
	
	pbus = pci_bus_scan();

	if (!pbus)
	{
		return 1;
	}

	/*i2c = (I2C_BUS*)malloc(sizeof(I2C_BUS));
	if (!i2c)
	{
		pci_bus_free(pbus);
		return NULL;
	}*/

	// Intel I2C : Bus 0, Device 22, Function 0 = 8086
	
	//memset(i2c, 0, sizeof(I2C_BUS));

	// Vendor Check : Bus 0, Device 0, Function 0
	pdev = pci_find_function(pbus, 0, 0, 0);
	if(pdev)
	{
		if(pdev->vendor_id == 0x8086)		//intel
		{
			if((pdev->device_id & 0xFF00) == 0x3E00)	//coffee lake
			{
				if(CoffeeLake(pbus, pi2c, I2cPrivate, baddr))
				{
					goto lii_end;
				}
			}
			else if((pdev->device_id & 0xFF00) == 0x5A00)	//apollo lake
			{
				if(ApolloLake(pbus, pi2c, I2cPrivate, baddr))
				{
					goto lii_end;
				}
			}
		}
		else if(pdev->vendor_id == 0x1022)		//amd
		{
			buf32[0] = buf32[1] = buf32[2] = buf32[3] = 0;
			AsmCpuid(1, &buf32[0], &buf32[1], &buf32[2], &buf32[3]);

			family = CPUIDTOFAMILY(buf32[0]);
			model = CPUIDTOMODEL(buf32[0]);
			
			if(family == 0x17)
			{
				if(AMD17H(pbus, pi2c, I2cPrivate, baddr))
				{
					goto lii_end;
				}
			}
		}

		gVendor = pdev->vendor_id;
	}

lii_end:
	pci_bus_free(pbus);
	
	return 0;
}

static UINT32 AMDI2CSclHcnt(UINT32 ic_clk, UINT32 tSYMBOL, UINT32 tf, UINT32 cond, UINT32 offset)
{
	if (cond)
	{
		/*
		* Conditional expression: 
		*
		*   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		*
		* This is based on the DW manuals, and representing an
		* ideal configuration.  The resulting I2C bus speed will
		* be faster than any of the others.
		*
		* If your hardware is free from tHD;STA issue, try this one.
		*/
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	}
	else
	{
		/*
		* Conditional expression:
		*
		*   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		*
		* This is just experimental rule; the tHD;STA period turned
		* out to be proportinal to (_HCNT + 3).  With this setting,
		* we could meet both tHIGH and tHD;STA timing specs.
		*
		* If unsure, you'd better to take this alternative.
		*
		* The reason why we need to take into account "tf" here,
		* is the same as described in i2c_dw_scl_lcnt().
		*/
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
	}
}

static UINT32 AMDI2CSclLcnt(UINT32 ic_clk, UINT32 tLOW, UINT32 tf, UINT32 offset)
{
	/*
	* Conditional expression:
	*
	*   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	*
	* DW I2C core starts counting the SCL CNTs for the LOW period
	* of the SCL clock (tLOW) as soon as it pulls the SCL line.
	* We need to take into account tf to meet the tLOW timing spec.
	* Default tf value should be 0.3 us, for safety.
	*/
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

int I2CGenSpeedConfig(UINT32 baddr, UINT32 speed)
{
	UINT32 ic_clk;
	UINT32 hcnt_min, lcnt_min;
	UINT16 scl_hcnt = 0, scl_lcnt = 0;
	UINT32 sda_hold = 0;
	UINT32 hcnt_reg = 0, lcnt_reg = 0;

	if(gVendor == 0x8086)	//Intel
	{
		ic_clk = 133;
		
		if(speed >= HS_SPEED)
		{
			/* High speed */
			hcnt_min = 60;
			lcnt_min = 160;
			hcnt_reg = baddr + DW_IC_HS_SCL_HCNT;
			lcnt_reg = baddr + DW_IC_HS_SCL_LCNT;
		}
		else if(speed >= FP_SPEED)
		{
			/* Fast-Plus speed */
			hcnt_min = 260;
			lcnt_min = 500;
			hcnt_reg = baddr + DW_IC_FS_SCL_HCNT;
			lcnt_reg = baddr + DW_IC_FS_SCL_HCNT;
		}
		else if (speed >= FS_SPEED)
		{
			/* Fast speed */
			hcnt_min = 600;
			lcnt_min = 1300;
			hcnt_reg = baddr + DW_IC_FS_SCL_HCNT;
			lcnt_reg = baddr + DW_IC_FS_SCL_HCNT;
		}
		else
		{
			/* Standard speed */
			hcnt_min = 4000;
			lcnt_min = 4700;
			hcnt_reg = baddr + DW_IC_SS_SCL_HCNT;
			lcnt_reg = baddr + DW_IC_SS_SCL_LCNT;
		}
		
		scl_hcnt = (UINT16)(((ic_clk*1000) * (hcnt_min + 171) + 500000) / 1000000) - 3;//(UINT16)(ic_clk * hcnt_min / KHz);
		scl_lcnt = (UINT16)(((ic_clk*1000) * (lcnt_min + 208) + 500000) / 1000000) - 1;//(UINT16)(ic_clk * lcnt_min / KHz);
		sda_hold = ((ic_clk*1000) * 207 + 500000) / 1000000;//0x000100CF;//0x00270014;//(ic_clk * DEFAULT_SDA_HOLD_TIME / KHz);
	}
	else if(gVendor == 0x1022)	//AMD
	{
		ic_clk = 150000;
		
		if(baddr == 0xFEDC6000)//V1000 I2C4
		{
			if(speed >= HS_SPEED)
			{
				/* High speed */
				hcnt_min = 0;
				lcnt_min = 0;
			}
			else if(speed >= FP_SPEED)
			{
				/* Fast-Plus speed */
				hcnt_min = 0;
				lcnt_min = 0;
			}
			else if (speed >= FS_SPEED)
			{
				/* Fast speed */
				hcnt_min = 0x1A;
				lcnt_min = 0x28;
			}
			else
			{
				/* Standard speed */
				hcnt_min = 0x64;
				lcnt_min = 0xC8;
			}
		}
		else
		{
			if(speed >= HS_SPEED)
			{
				/* High speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										1,     /* tHD;STA = tHIGH = 0.06 us */
										0,      /* tf = 0.08 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										1,     /* tLOW = 0.16 us */
										0,      /* tf = 0.08 us */
										0);     /* No offset */
			}
			else if(speed >= FP_SPEED)
			{
				/* Fast-Plus speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										2,     /* tHD;STA = tHIGH = 0.26 us */
										1,      /* tf = 0.12 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										5,     /* tLOW = 0.5 us */
										1,      /* tf = 0.12 us */
										0);     /* No offset */
			}
			else if (speed >= FS_SPEED)
			{
				/* Fast speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										6,     /* tHD;STA = tHIGH = 0.6 us */
										3,      /* tf = 0.3 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										13,     /* tLOW = 1.3 us */
										3,      /* tf = 0.3 us */
										0);     /* No offset */
			}
			else
			{
				/* Standard speed */
				hcnt_min = AMDI2CSclHcnt(ic_clk,
										40,     /* tHD;STA = tHIGH = 4.0 us */
										3,      /* tf = 0.3 us */
										0,      /* 0: DW default, 1: Ideal */
										0);     /* No offset */
				lcnt_min = AMDI2CSclLcnt(ic_clk,
										47,     /* tLOW = 4.7 us */
										3,      /* tf = 0.3 us */
										0);     /* No offset */
			}
		}

		scl_hcnt = (UINT16)hcnt_min;
		scl_lcnt = (UINT16)lcnt_min;
		sda_hold = 1;
	}

	/* Write the speed config that was generated earlier */
	/*if(speed >= HS_SPEED)
	{}
	else if(speed >= FP_SPEED)
	{}
	else if (speed >= FS_SPEED)
	{}
	else
	{}*/
	MmioWrite32(hcnt_reg, scl_hcnt);
	MmioWrite32(lcnt_reg, scl_lcnt);
	MmioWrite32(baddr + DW_IC_SDA_HOLD, sda_hold);

	return 0;
}

EFI_STATUS EFIAPI SetBusFrequency(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN *speed)
{
	I2CMASTER_PRIVATE *private;
	UINT32 baddr;
	UINT32 control;

	private = (I2CMASTER_PRIVATE*)This;
	baddr = private->I2cSdpAddress;

	if(!private->Mp2)
	{
		// Disable the interface
		MmioWrite32(baddr + DW_IC_ENABLE, 0);
		if(I2cDwWaitI2cEnable(baddr, CHECK_IC_EN_HIGH))
		{
			return EFI_NOT_READY;
		}

		control = MmioRead32(baddr + DW_IC_CON);
		control &= ~DW_I2C_CON_SPEED_MASK;

		if(*speed >= HS_SPEED)
		{
			/* High and Fast-Ultra speed share config registers */
			control |= DW_I2C_CON_SPEED_HS;
		}
		else if(*speed >= FS_SPEED)
		{
			/* Fast speed and Fast-Plus */
			control |= DW_I2C_CON_SPEED_FS;
		}
		else
		{
			/* Standard speed */
			control |= DW_I2C_CON_SPEED_SS;
		}

		/* Select this speed in the control register */
		MmioWrite32(baddr + DW_IC_CON, control);

		/* Generate speed config based on clock */
		if(I2CGenSpeedConfig(baddr, (UINT32)*speed) < 0)
		{
			return 1;
		}
	}
	else
	{
		switch(*speed)
		{
			case SS_SPEED:
			{
				private->ClockFrequency = speed100k;
				break;
			}
			case FS_SPEED:
			{
				private->ClockFrequency = speed400k;
				break;
			}
			case FP_SPEED:
			{
				private->ClockFrequency = speed1000k;
				break;
			}
		}
	}

	return 0;
}

EFI_STATUS EFIAPI Reset(IN CONST EFI_I2C_MASTER_PROTOCOL *This)
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS EFIAPI StartRequest(IN CONST EFI_I2C_MASTER_PROTOCOL *This, IN UINTN SlaveAddress, IN EFI_I2C_REQUEST_PACKET *RequestPacket, IN EFI_EVENT Event OPTIONAL, OUT EFI_STATUS *I2cStatus OPTIONAL)
{
	EFI_STATUS Status = EFI_SUCCESS;

	//Print(L"StartRequest: start\n");

	Status = I2cAccess((I2CMASTER_PRIVATE*)This, SlaveAddress, RequestPacket);
	//Print(L"StartRequest: %r\n",Status);

	return Status;
}

EFI_STATUS EFIAPI PCHI2cMasterDxeInit(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_HANDLE Handle;
	I2C_MASTER_PRIVATE *I2cPrivate;
	EFI_I2C_CONTROLLER_CAPABILITIES *I2cControllerCapabilities;
	UINT32 Index;
	I2C_DEVICE_PATH *I2cDevPath;

	UINT32 baddr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	Status = gBS->AllocatePool(EfiBootServicesData, sizeof(I2C_MASTER_PRIVATE), &I2cPrivate);
	if(EFI_ERROR (Status))
	{
		return Status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(I2cPrivate, sizeof(I2C_MASTER_PRIVATE), 0);
	}

	Status = gBS->AllocatePool(EfiBootServicesData, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), &I2cControllerCapabilities);
	if(EFI_ERROR (Status))
	{
		return Status;
	}
	else
	{
		//clear instances content
		gBS->SetMem (I2cControllerCapabilities, sizeof(EFI_I2C_CONTROLLER_CAPABILITIES), 0);
	}

	//Init I2C cotroller capabilities data structure
	I2cControllerCapabilities->StructureSizeInBytes = sizeof(EFI_I2C_CONTROLLER_CAPABILITIES);
	I2cControllerCapabilities->MaximumReceiveBytes = 0xFFFFFFFF;
	I2cControllerCapabilities->MaximumTransmitBytes = 0xFFFFFFFF;
	I2cControllerCapabilities->MaximumTotalBytes = 0xFFFFFFFF;

	Handle = ImageHandle;

	I2CFindControllerAndBaseAddress(I2cPrivate, baddr);
	
	for(Index=0; Index<gI2CControllerCount; Index++)
	{
		//I2cPrivate->I2cMaster[Index].Handle = Handle;
		I2cPrivate->I2cMaster[Index].I2cSdpAddress = baddr[Index];
		I2cPrivate->I2cMaster[Index].I2cController.SetBusFrequency = SetBusFrequency;
		I2cPrivate->I2cMaster[Index].I2cController.Reset = Reset;
		I2cPrivate->I2cMaster[Index].I2cController.StartRequest = StartRequest;
		I2cPrivate->I2cMaster[Index].I2cController.I2cControllerCapabilities = I2cControllerCapabilities;
		I2cInit(&I2cPrivate->I2cMaster[Index]);

		Status = gBS->AllocatePool(EfiBootServicesData, sizeof(I2C_DEVICE_PATH), &I2cDevPath);
		if(EFI_ERROR (Status))
		{
			I2cDevPath = &gI2cDevMemMapPathTemplate;
		}
		else
		{
			// Update instances content.
			gBS->CopyMem(I2cDevPath, &gI2cDevMemMapPathTemplate, sizeof(I2C_DEVICE_PATH));

			I2cDevPath->I2cBus.ControllerNumber = I2cPrivate->I2cMaster[Index].I2cBusId;
			if(I2cPrivate->I2cMaster[Index].Mp2)
			{
				I2cDevPath->I2cMemMapDevPath.StartingAddress = I2cPrivate->I2cMaster[Index].I2cBusId;
			}
			else
			{
				I2cDevPath->I2cMemMapDevPath.StartingAddress = baddr[Index];
			}
		}
		I2cPrivate->I2cMaster[Index].DevPath = (EFI_DEVICE_PATH_PROTOCOL *)I2cDevPath;

		// Install Protocol
		Status = gBS->InstallMultipleProtocolInterfaces(&I2cPrivate->I2cMaster[Index].Handle, &gEfiI2cMasterProtocolGuid, &I2cPrivate->I2cMaster[Index], &gEfiDevicePathProtocolGuid, I2cPrivate->I2cMaster[Index].DevPath, NULL);
	}

	return Status;
}

UINT32 I2cDwCheckErrors(IN UINT32 Base)
{
	UINT32 I2cStatusCnt = 250;
	UINT32 TxAbrt = 0;
	UINT32 Status = MmioRead32(Base + DW_IC_RAW_INTR_STAT);

	if(Status & DW_IC_INTR_RX_UNDER)
	{
		MmioRead32(Base + DW_IC_CLR_RX_UNDER);
	}

	if(Status & DW_IC_INTR_RX_OVER)
	{
		MmioRead32(Base + DW_IC_CLR_RX_OVER);
	}

	if(Status & DW_IC_INTR_TX_ABRT)
	{
		Status = MmioRead32(Base + DW_IC_TX_ABRT_SOURCE);
		MmioRead32(Base + DW_IC_CLR_TX_ABRT);
		DEBUG((EFI_D_ERROR, "I2C: TX_ABORT Error\n"));
	}

	if (Status & DW_IC_ERR_CONDITION)
	{
		if(Status || TxAbrt)
		{
			DEBUG((EFI_D_ERROR, "Errors on I2C bus %08x %08x: \n", Status, TxAbrt));
		}

		MmioWrite32(Base + DW_IC_ENABLE, 0); // Disable the adapter
		I2cDwWaitI2cEnable(Base, CHECK_IC_EN_HIGH);  //Wait controller status change

		do{
			MicroSecondDelay(1);
			if(I2cStatusCnt == 0)
			{
				DEBUG((EFI_D_ERROR, "I2C: Timeout during disable \n"));
			}
		}while(((MmioRead32(Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);
		MmioWrite32(Base + DW_IC_TAR, 0); // Clear Target address
	}

	return (Status & DW_IC_ERR_CONDITION);
}

BOOLEAN I2cDwWaitBusNotBusy(IN UINT32 Base)
{
	INTN Timeout = 10000;

	while(MmioRead32(Base + DW_IC_STATUS) & 0x20)
	{
		if(Timeout <= 0)
		{
			DEBUG((EFI_D_ERROR, "I2C: Timeout waiting for bus ready\n"));
			return TRUE;
		}
		Timeout--;
		MicroSecondDelay(1);
	}

	return FALSE;
}

EFI_STATUS I2cDwWaitStop(IN UINT32 Base)
{
	INTN Timeout = 10000;

	while(0 == (MmioRead32(Base + DW_IC_RAW_INTR_STAT) & DW_IC_INTR_STOP_DET))
	{
		if(Timeout <= 0)
		{
			DEBUG((EFI_D_ERROR, "I2C: Timeout waiting for received stop bit \n"));
			return EFI_TIMEOUT;
		}
		Timeout--;
		MicroSecondDelay(1);
	}
	DEBUG((EFI_D_ERROR, "I2C_STATUS : %x, Tx Stop timeout remaining : %d of %d\n", MmioRead32(Base + DW_IC_RAW_INTR_STAT), Timeout, DW_TX_DATA_RETRY));

	/* Read to clear INTR_STAT_STOP_DET */
	MmioRead32(Base + DW_IC_CLR_STOP_DET);

	//check tx complete
	if(MmioRead32(Base + DW_IC_RAW_INTR_STAT) & DW_IC_INTR_TX_ABRT)
	{
		DEBUG((EFI_D_ERROR, "I2C TX Abort. sts=0x%x rists=0x%x abr=0x%x \n\n", MmioRead32(Base + DW_IC_STATUS), MmioRead32(Base + DW_IC_RAW_INTR_STAT), MmioRead32(Base + DW_IC_TX_ABRT_SOURCE)));
		return EFI_DEVICE_ERROR;
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cDwWaitTxData(IN UINT32 Base)
{
	INTN Timeout = 10000;

	while(0 == (MmioRead32(Base + DW_IC_STATUS) & DW_I2C_STATUS_TFE))
	{
		if(Timeout <= 0)
		{
			DEBUG((EFI_D_ERROR, "I2C: Timeout waiting for Tx Fifo full empty \n"));
			return EFI_TIMEOUT;
		}
		Timeout--;
		MicroSecondDelay(1);
	}
	DEBUG((EFI_D_ERROR, "I2C_STATUS : %x, Tx timeout remaining : %d of %d\n", MmioRead32(Base + DW_IC_STATUS), Timeout, DW_TX_DATA_RETRY));
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cDwWaitRxData(IN UINT32 Base, IN UINT32 RxFifoDepth)
{
	INTN Timeout = 10000;
	
	while(MmioRead32(Base + DW_IC_RXFLR) < RxFifoDepth)
	{
		if(Timeout <= 0)
		{
			DEBUG((EFI_D_ERROR, "I2C: Timeout waiting for Rx data ready\n"));
			return EFI_TIMEOUT;
		}
		Timeout--;
		MicroSecondDelay(1);
	}
	DEBUG((EFI_D_ERROR, "I2C_STATUS : %x, Rx timeout remaining : %d of %d\n", MmioRead32(Base + DW_IC_STATUS), Timeout, DW_TX_DATA_RETRY));
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cDwWaitI2cEnable(IN UINT32 Base, IN UINT32 I2cEnBit)
{
	INTN Timeout = 10000;
	
	while(I2cEnBit == (MmioRead32(Base + DW_IC_ENABLE_STATUS) & DW_I2C_EN_STATUS_IC_EN))
	{
		if(Timeout <= 0)
		{
			DEBUG((EFI_D_ERROR, "I2C: Timeout waiting for I2C Enable : %x\n", (MmioRead32(Base + DW_IC_ENABLE_STATUS))));
			return EFI_TIMEOUT;
		}
		Timeout--;
		MicroSecondDelay(1);
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cDwWaitDeviceBusy(IN OUT I2CMASTER_PRIVATE *Private)
{
	UINT32 Base = Private->I2cSdpAddress;
	
	if(I2cDwWaitBusNotBusy(Base))
	{
		return EFI_NOT_READY;
	}
	
	MmioWrite32(Base + DW_IC_INTR_MASK, 0);
	(void)MmioRead32(Base + DW_IC_CLR_INTR);
	
	// Enable the interface
	MmioWrite32(Base + DW_IC_ENABLE, DW_I2C_ENABLE);
	if(I2cDwWaitI2cEnable(Base, CHECK_IC_EN_LOW))
	{
		return EFI_NOT_READY;
	}
	
	MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_CMD | DW_I2C_DATA_STOP);
	gBS->Stall(100);
	
	if(I2cDwCheckErrors(Base))
	{
		MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
		return EFI_DEVICE_ERROR;
	}
	MmioRead32(Base + DW_IC_DATA_CMD); // clear receive buf
	MmioWrite32(Base + DW_IC_ENABLE, 0);
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cInit(IN OUT I2CMASTER_PRIVATE *Private)
{
	EFI_STATUS Status;
	UINT32 Base = Private->I2cSdpAddress;
	UINTN BusClockHertz;
	I2C_CONFIGURATION *configuration = &Private->I2cConfiguration;

	if(!Private->Mp2)
	{
		// Disable the interface
		MmioWrite32(Base + DW_IC_ENABLE, 0);
		if(I2cDwWaitI2cEnable(Base, CHECK_IC_EN_HIGH))
		{
			return EFI_NOT_READY;
		}

		//Mask Interrupt and Clear interrupt status
		MmioWrite32(Base + DW_IC_INTR_MASK, 0);
		(void)MmioRead32(Base + DW_IC_CLR_INTR);
		(void)MmioRead32(Base + DW_IC_CLR_TX_ABRT);
		configuration->RxFifoDepth = ((MmioRead32(Base + DW_IC_COMP_PARAM_1) & DW_I2C_RX_BUFFER_DEPTH_MASK) >> DW_I2C_RX_BUFFER_DEPTH_SHIFT) + 1;
		configuration->TxFifoDepth = ((MmioRead32(Base + DW_IC_COMP_PARAM_1) & DW_I2C_TX_BUFFER_DEPTH_MASK) >> DW_I2C_TX_BUFFER_DEPTH_SHIFT) + 1;
	}
	else		//amd mp2
	{
		Mp2I2CEnable(Base, Private->I2cBusId, speed100k);
	}
		
	//Set default to Standard Speed
	BusClockHertz = SS_SPEED;
	Status = SetBusFrequency(&Private->I2cController, &BusClockHertz);
	
	return Status;
}

EFI_STATUS I2cSetTarget(IN UINT32 Chip, IN UINT32 Base)
{
	UINT32 I2cStatusCnt;
	UINT32 I2cPollTime;
	EFI_STATUS Status = EFI_SUCCESS;

	//Print(L"I2cSetTarget: start\n");

	if(MmioRead32(Base + DW_IC_TAR) != Chip)
	{
		MmioWrite32(Base + DW_IC_ENABLE, 0);	// Disable the Controller
		if(I2cDwWaitI2cEnable(Base, CHECK_IC_EN_HIGH))
		{
			return EFI_NOT_READY;
		}
		I2cPollTime = (10 * 1000000) / configI2C_MAX_SPEED;
		I2cStatusCnt = DW_STATUS_WAIT_RETRY;

		do{
			MicroSecondDelay(I2cPollTime);
			if(I2cStatusCnt == 0)
			{
				DEBUG((EFI_D_ERROR, "I2C Set Target: Timeout during disable\n"));
				MmioWrite32(Base + DW_IC_TAR, 0);	// Clear Target Address
				Status = EFI_DEVICE_ERROR;
				return Status;
			}
		}while(((MmioRead32(Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);

		MmioWrite32(Base + DW_IC_TAR, Chip);	// Set Target Address
		DEBUG((EFI_D_ERROR, "I2C Target Set - Chip Address:%x\n", Chip));
		I2cStatusCnt = DW_STATUS_WAIT_RETRY;
		MmioWrite32(Base + DW_IC_ENABLE, 1);	// Enable the Controller
		I2cDwWaitI2cEnable(Base, CHECK_IC_EN_LOW);		//Check IC_EN bit
		do{
			MicroSecondDelay(I2cPollTime);
			if(I2cStatusCnt == 0)
			{
				DEBUG((EFI_D_ERROR, "I2C Set Target: Timeout during enable\n"));
				//Print(L"I2C Set Target: Timeout during enable\n");
				MmioWrite32(Base + DW_IC_TAR, 0); // Clear Target Address
				Status = EFI_DEVICE_ERROR;
				return Status;
				}
		}while((!(MmioRead32(Base + DW_IC_ENABLE_STATUS) & 0x01)) && I2cStatusCnt--);

		if(MmioRead32(Base + DW_IC_TAR) != Chip)
		{
			DEBUG((EFI_D_ERROR, "Cannot set the target on I2c bus to %x\n", Chip));
			Status = EFI_INVALID_PARAMETER;
		}
	}
	//Print(L"I2cSetTarget: %r\n",Status);
	
	return Status;
}

EFI_STATUS I2cMP2WriteRead(IN OUT I2CMASTER_PRIVATE *Private, IN UINTN SlaveAddress)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 base;

	base = Private->I2cSdpAddress;  // For convenience

	// TX
	if(Private->WriteCount)
	{
		Status = Mp2I2cWrite(Private, SlaveAddress);
		if(Status)
		{
			return Status;
		}
	}

	// RX
	if(Private->ReadCount)
	{
		Status = Mp2I2cRead(Private, SlaveAddress);
		if(Status)
		{
			return Status;
		}
	}

	return EFI_SUCCESS;
}

EFI_STATUS I2cWriteRead(IN OUT I2CMASTER_PRIVATE *Private)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 Base;
	UINT32 TxFifoCount;
	UINT32 RxFifoCount;
	UINT32 Index;

	Base = Private->I2cSdpAddress;  // For convenience

	if(I2cDwWaitBusNotBusy(Base))
	{
		return EFI_NOT_READY;
	}

	MmioWrite32(Base + DW_IC_INTR_MASK, 0);
	(void)MmioRead32(Base + DW_IC_CLR_INTR);
	DEBUG((EFI_D_ERROR, "1 : Enable I2C interface\n"));
	
	// Enable the interface
	MmioWrite32(Base + DW_IC_ENABLE, DW_I2C_ENABLE);
	if(I2cDwWaitI2cEnable(Base, CHECK_IC_EN_LOW))
	{
		return EFI_NOT_READY;
	}

	// Load up the TX FIFO
	if(0 < Private->TransferCount)
	{
		while(0 < Private->WriteCount)
		{
			TxFifoCount = (Private->I2cConfiguration.TxFifoDepth < Private->WriteCount) ? Private->I2cConfiguration.TxFifoDepth : Private->WriteCount;
			DEBUG((EFI_D_ERROR, "2 : Load up the TX FIFO, TxFifofDepth: %d\n", TxFifoCount));
			
			for(Index = 0; Index < TxFifoCount ; Index++)
			{
				//Print(L"I2cAccess:Loop:WriteCount : %d, Transfer Count : %d\n",Private->WriteCount, Private->TransferCount);
				MicroSecondDelay(1000); //E140X025_2
				
				if((Private->TransferCount == 1) && (Private->ReadCount == 0))
				{
					DEBUG((EFI_D_ERROR, "Write Data: 0x%x\n", *(Private->WriteData)));
					MmioWrite32(Base + DW_IC_DATA_CMD, (*(Private->WriteData++) & DW_I2C_DATA_MASK) | DW_I2C_DATA_STOP);
					//Print(L"I2cAccess:Write Data1: 0x%x\n",*(Private->WriteData));
				}
				else
				{
					DEBUG((EFI_D_ERROR, "Write Data: 0x%x\n", *(Private->WriteData)));
					if(Private->WriteCount == 1)
					{
						MmioWrite32(Base + DW_IC_DATA_CMD, (*(Private->WriteData++) & DW_I2C_DATA_MASK) | DW_I2C_DATA_STOP);
					}
					else
					{
						MmioWrite32(Base + DW_IC_DATA_CMD, *(Private->WriteData++) & DW_I2C_DATA_MASK);
					}
					//Print(L"I2cAccess:Write Data: 0x%x\n",*(Private->WriteData));
				}
				MicroSecondDelay(1000);  //E140X025_2
				Private->WriteCount--;
				Private->TransferCount--;
			}
			DEBUG((EFI_D_ERROR, "3 : Wait for TxFifo empty\n"));
			
			//Wait for TxFifo empty
			Status = I2cDwWaitTxData(Base);
			//Print(L"I2cAccess:I2cDwWaitRxData: %r\n",Status);
			if(Status)
			{
				//Transcation failed, send STOP command to free the bus
				MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
				return Status;
			}
			
			/* Wait for interrupt status to indicate transfer is complete */
			Status = I2cDwWaitStop(Base);
			if(Status)
			{
				//Transcation failed, send STOP command to free the bus
				MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
				return Status;
			}
		}
	}
	DEBUG((EFI_D_ERROR, "Write Count : %d, Transfer Count : %d\n", Private->WriteCount, Private->TransferCount));

	if(0 < Private->TransferCount)
	{
		DEBUG((EFI_D_ERROR, "4 : Start Read trasaction\n"));

		//Print(L"I2cAccess:Start Read trasaction: \n");
		while(0 < Private->ReadCount)
		{
			RxFifoCount = (Private->I2cConfiguration.RxFifoDepth < Private->ReadCount) ? Private->I2cConfiguration.RxFifoDepth : Private->ReadCount;
			DEBUG((EFI_D_ERROR, "5 : fille up Rx Fifo, RxFifoDepth : %d\n", RxFifoCount));
			//Print(L"I2cAccess:Read Count : %d, Transfer Count : %d\n",Private->ReadCount, Private->TransferCount);
			
			// Fill up Rx Fifo
			for(Index = 0; Index < RxFifoCount ; Index++)
			{
				//Print(L"I2cAccess:Loop:Read Count : %d, Transfer Count : %d\n",Private->ReadCount, Private->TransferCount);
				//MicroSecondDelay(1000);   //E140X025_2
				if(Private->ReadCount == 1)
				{
					MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_CMD | DW_I2C_DATA_STOP);
				}
				else
				{
					MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_CMD);
				}
				gBS->Stall(50);//MicroSecondDelay(1000);     //E140X025_2
				Private->ReadCount--;
				Private->TransferCount--;
			}
			DEBUG((EFI_D_ERROR, "6 : Wait Rx data ready \n"));
			
			//Waiting for Rx data ready
			Status = I2cDwWaitRxData(Base, RxFifoCount);
			//Print(L"I2cAccess:I2cDwWaitRxData: %r\n",Status);

			if(Status)
			{
				//Transcation failed, send STOP command to free the bus
				MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
				return Status;
			}
			DEBUG((EFI_D_ERROR, "7 : Read data from Rxfifo\n"));
			
			for(Index = 0; Index < RxFifoCount; Index++)
			{
				if(I2cDwCheckErrors(Base))
				{
					MmioWrite32(Base + DW_IC_DATA_CMD, DW_I2C_DATA_STOP);
					return EFI_DEVICE_ERROR;
				}
				*(Private->ReadData++) = (UINT8)(MmioRead32(Base + DW_IC_DATA_CMD) & DW_I2C_DATA_MASK); // Receive data unit from RxFifo
			}
			*(Private->ReadData++) = MmioRead32(Base + DW_IC_DATA_CMD) & DW_I2C_DATA_MASK;
			DEBUG((EFI_D_ERROR, "Read Count : %d, Transfer Count : %d\n", Private->ReadCount, Private->TransferCount));
			//Print(L"I2cAccess:Read:Read Count : %d, Transfer Count : %d\n",Private->ReadCount, Private->TransferCount);
		}
	}
	
	// Transaction complete
	DEBUG((EFI_D_ERROR, "8 : transaction complete\n"));
	
	return EFI_SUCCESS;
}

EFI_STATUS I2cAccess(IN I2CMASTER_PRIVATE *Private, IN UINTN SlaveAddress, IN OUT EFI_I2C_REQUEST_PACKET *RequestPacket)
{
	EFI_STATUS Status = EFI_SUCCESS;
	BOOLEAN IsWriteOperation;
	UINT32 Base = Private->I2cSdpAddress;

	//Print(L"I2cAccess: start\n");

	if(RequestPacket->Operation[0].Flags & (I2C_FLAG_SMBUS_PEC | I2C_FLAG_SMBUS_PROCESS_CALL))
	{
		Status = EFI_UNSUPPORTED;
		//Print(L"I2cAccess: %r\n",Status);
		return Status;
	}

	if(0 == RequestPacket->Operation[0].LengthInBytes)
	{
		Status = EFI_UNSUPPORTED;
		//Print(L"I2cAccess: %r\n",Status);
		return Status;
	}

	if(RequestPacket->OperationCount == 1)
	{
		if(RequestPacket->Operation[0].Flags & (I2C_FLAG_READ << 1))
		{
			//command test
			if(!Private->Mp2)
			{
				//Set target device slave address
				if(I2cSetTarget((UINT32)SlaveAddress, Private->I2cSdpAddress) != EFI_SUCCESS)
				{
					Status = EFI_DEVICE_ERROR;
					return Status;
				}
				
				return I2cDwWaitDeviceBusy(Private);
			}
			else
			{
				Private->ReadCount = RequestPacket->Operation[0].LengthInBytes;
				Private->ReadData = RequestPacket->Operation[0].Buffer;
				Private->WriteCount = 0;
				Private->WriteData = NULL;
				
				gBS->Stall(10000);	// 10ms
				return Mp2I2cWaitDeviceBusy(Private, SlaveAddress);
			}
		}
		else if(RequestPacket->Operation[0].Flags & I2C_FLAG_READ)
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

	if(!Private->Mp2)
	{
		//Set target device slave address
		if(I2cSetTarget((UINT32)SlaveAddress, Private->I2cSdpAddress) != EFI_SUCCESS)
		{
			Status = EFI_DEVICE_ERROR;
			//Print(L"I2cAccess: %r\n",Status);
			return Status;
		}
		
		Status = I2cWriteRead(Private);

		//Disable the interface
		DEBUG((EFI_D_ERROR, "9 : disable I2C interface\n"));
		MmioWrite32(Base + DW_IC_ENABLE, 0);
		I2cDwWaitI2cEnable(Base, CHECK_IC_EN_HIGH);		//Wait controller status change
	}
	else		//amd mp2
	{
		Status = I2cMP2WriteRead(Private, SlaveAddress);
	}

	//Print(L"I2cAccess: %r\n",Status);

	return Status;
}

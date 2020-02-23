#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
//#include "util.h"
#include "x86hw.h"
#include "pchi2ctest.h"

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  PCI                                                                      *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  pci_select_device
//=============================================================================
void pci_select_device(PCI_BUS *pbus, uint8_t b, uint8_t d, uint8_t f)
{
	//----------------------------------
	//  3         2         1         0
	// 10987654321098765432109876543210
	//----------------------------------
	// 10000000BBBBBBBBDDDDDFFFRRRRRR00
	//         bus     dev  fun reg
	//----------------------------------
	pbus->iocmd = 0x8000;
	pbus->iocmd |= b;		// bus
	pbus->iocmd <<= 5;
	pbus->iocmd |= d;		// device
	pbus->iocmd <<= 3;
	pbus->iocmd |= f;		// function
	pbus->iocmd <<= 8;
}
//=============================================================================
//  pci_read_dword
//=============================================================================
uint32_t pci_read_dword(PCI_BUS *pbus, uint8_t reg)
{
	pbus->iocmd &= 0xFFFFFF00;
	pbus->iocmd |= (reg & 0xFC);

	outpd( PCI_CMD_PORT, pbus->iocmd );

	return inpd( PCI_DAT_PORT );
}
//=============================================================================
//  pci_read_byte
//=============================================================================
uint8_t pci_read_byte(PCI_BUS *pbus, uint8_t reg)
{
	uint32_t	data32;
	uint8_t		data8;

	pbus->iocmd &= 0xFFFFFF00;
	pbus->iocmd |= (reg & 0xFC);

	outpd( PCI_CMD_PORT, pbus->iocmd );
	data32 = inpd( PCI_DAT_PORT );

	switch (reg & 0x3)
	{
		case 0: data8 = data32 & 0xff;			break;
		case 1: data8 = (data32>>8) & 0xff;		break;
		case 2: data8 = (data32>>16) & 0xff;	break;
		case 3: data8 = (data32>>24) & 0xff;	break;
	}

	return data8;
//	return ( (data32 >> ((reg & 0x3) << 3)) & 0xff ); 
}
//=============================================================================
//  pci_write_dword
//=============================================================================
void pci_write_dword(PCI_BUS *pbus, uint8_t reg, uint32_t data)
{
	pbus->iocmd &= 0xFFFFFF00;
	pbus->iocmd |= (reg & 0xFC);

	outpd(PCI_CMD_PORT,pbus->iocmd);
	outpd(PCI_DAT_PORT, data);
}
//=============================================================================
//  pci_write_byte
//=============================================================================
void pci_write_byte(PCI_BUS *pbus, uint8_t reg, uint8_t data)
{
	uint32_t	tmp;
	uint32_t	mask;
	uint8_t		off;

	pbus->iocmd &= 0xFFFFFF00;
	pbus->iocmd |= (reg & 0xFC);
	outpd(PCI_CMD_PORT,pbus->iocmd);


	off = (reg & 0x3)<<3;		// 0, 8, 16, 24
	mask = 0xFF;
	mask <<= (1<<off);			// 0x000000FF
								// 0x0000FF00
								// 0x00FF0000
								// 0xFF000000

	mask ^= 0xFFFFFFFF;			// 0xFFFFFF00
								// 0xFFFF00FF
								// 0xFF00FFFF
								// 0x00FFFFFF

	tmp = inpd(PCI_DAT_PORT);
	tmp &= mask;

	mask = data;
	mask <<= off;
	tmp |= mask;

	outpd(PCI_DAT_PORT, tmp);
}
//=============================================================================
//  pci_create_device
//=============================================================================
PCI_DEVICE* pci_create_device(void)
{
	PCI_DEVICE	*dev;

	dev = (PCI_DEVICE *)malloc( sizeof(PCI_DEVICE) );
	if (!dev)
	{
		//fprintf(stderr, "malloc err!\n");
		return NULL;
	}
	dev->prev = NULL;
	dev->next = NULL;

	return dev;
}
//=============================================================================
//  pci_bus_free
//=============================================================================
void pci_bus_free(PCI_BUS *pbus)
{
	PCI_DEVICE *dev;

	if(pbus == NULL)
		return;
	
	dev = pbus->head;
	
	while(dev->next != NULL)
	{
		dev = dev->next;
	}
	
	while(dev->prev != NULL)
	{
		dev = dev->prev;
		free(dev->next);
	}

	free(dev);
	free(pbus);
}
//=============================================================================
//  pci_bus_scan
//=============================================================================
//int pci_scan_bus(void)
PCI_BUS *pci_bus_scan(void)
{
	uint16_t	b;
	uint8_t		d;
	uint8_t		f;
	uint8_t		i;
	uint32_t	data32;
	uint32_t	pci_conf[16];
	int			dev_cnt;
	PCI_DEVICE	*dev;
	PCI_BUS		*pbus;
	PCI_DEVICE	*pdev = NULL;


	pbus = (PCI_BUS*)malloc( sizeof(PCI_BUS) );
	if ( !pbus )
	{
		return NULL;
	}

	dev_cnt = 0;

	pbus->head		= NULL;
	pbus->num_dev	= 0;
	pbus->iocmd		= 0;

	for (b=0; b<256; b++)
	{
		for (d=0; d<32; d++)
		{
			for (f=0; f<8; f++)
			{
				pci_select_device( pbus, (uint8_t)b, d, f );

				data32 = pci_read_dword(pbus, 0x00);
				if ( data32 == 0xFFFFFFFF || data32 == 0 )
				{
					// no device
					continue;
				}

				dev = pci_create_device();
				if (!dev)
				{
					return NULL;
				}

				if ( dev_cnt == 0 )
				{
					pbus->head = dev;
				}
				else
				{
					pdev->next = dev;
					dev->prev = pdev;
				}
				pdev = dev;
				dev_cnt++;

				pdev->bus	= (uint8_t)b;
				pdev->dev	= d;
				pdev->fun	= f;

				// 00h~03h
				pdev->vendor_id	= (uint16_t)(data32 & 0xFFFF);
				pdev->device_id	= (uint16_t)((data32>>16) & 0xFFFF);
				pci_conf[0] = data32;

				for (i=1; i<16; i++)
				{
					pci_conf[i] = pci_read_dword(pbus, (i<<2));
				}

				// 04h~07h
				pdev->command	= pci_conf[1] & 0xFFFF;
				pdev->status	= ((pci_conf[1]>>16) & 0xFFFF);

				// 08h~0Bh
				pdev->rev_id		= (uint8_t)(pci_conf[2] & 0xFF);
				pdev->class_code[0]	= (uint8_t)((pci_conf[2]>>8) & 0xFF);
				pdev->class_code[1]	= (uint8_t)((pci_conf[2]>>16) & 0xFF);
				pdev->class_code[2]	= (uint8_t)((pci_conf[2]>>24) & 0xFF);

				// 0Ch~0Fh
				pdev->cache_line_sz	= (uint8_t)((pci_conf[3]>>0) & 0xFF);
				pdev->latency_timer	= (uint8_t)((pci_conf[3]>>8) & 0xFF);
				pdev->header_type	= (uint8_t)((pci_conf[3]>>16) & 0xFF);
				pdev->bist			= (uint8_t)((pci_conf[3]>>24) & 0xFF);

				// 10h~27h
				pdev->bar[0]	= pci_conf[4];
				pdev->bar[1]	= pci_conf[5];
				pdev->bar[2]	= pci_conf[6];
				pdev->bar[3]	= pci_conf[7];
				pdev->bar[4]	= pci_conf[8];
				pdev->bar[5]	= pci_conf[9];

				// 28h
				pdev->cb_cis_ptr		= pci_conf[10];

				// 2Ch~2Fh
				pdev->ssys_vendor_id	= (pci_conf[11] & 0xFFFF);
				pdev->ssys_id			= ((pci_conf[11]>>16) & 0xFFFF);

				// 30h
				pdev->exp_rom_base		= pci_conf[12];

				// 34h
				pdev->capability_ptr	= (uint8_t)((pci_conf[13]>>0) & 0xFF);

				// 35h~3Bh
				pdev->res[0]	= (uint8_t)((pci_conf[13]>>8) & 0xFF);
				pdev->res[1]	= (uint8_t)((pci_conf[13]>>16) & 0xFF);
				pdev->res[2]	= (uint8_t)((pci_conf[13]>>24) & 0xFF);
				pdev->res[3]	= (uint8_t)((pci_conf[14]>>0) & 0xFF);
				pdev->res[4]	= (uint8_t)((pci_conf[14]>>8) & 0xFF);
				pdev->res[5]	= (uint8_t)((pci_conf[14]>>16) & 0xFF);
				pdev->res[6]	= (uint8_t)((pci_conf[14]>>24) & 0xFF);

				// 3Ch~3Fh
				pdev->int_line	= (uint8_t)((pci_conf[15]>>0) & 0xFF);
				pdev->int_pin	= (uint8_t)((pci_conf[15]>>8) & 0xFF);
				pdev->min_gnt	= (uint8_t)((pci_conf[15]>>16) & 0xFF);
				pdev->max_lat	= (uint8_t)((pci_conf[15]>>24) & 0xFF);

			}//for
		}//for
	}//for

	pbus->num_dev = dev_cnt;
	// debug
	/*
	gST->ConOut->ClearScreen(gST->ConOut);
	fprintf(stderr, "pci_dev_num = %d\n", pci_dev_num);
	dev = pci_dev_head;
	for (i=0; i<pci_dev_num; i++, dev=dev->next) {
		fprintf(stderr, "%d:%d.%d = %04X:%04X\n",
			dev->bus, dev->dev, dev->fun, dev->vendor_id, dev->device_id);
		getch();
	}
	*/
	return pbus;
}
//=============================================================================
//  pci_find_vendor
//=============================================================================
PCI_DEVICE* pci_find_vendor(PCI_BUS *pbus, uint16_t vendor)
{
	int			i;
	PCI_DEVICE	*dev;
	PCI_DEVICE	*d;

	if (!pbus->num_dev)
		return NULL;

	dev = pbus->head;

	for (i=0, d=dev; i<pbus->num_dev; i++, d=d->next)
	{
		if (d->vendor_id == vendor)
		{
			return d;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_device
//=============================================================================
PCI_DEVICE* pci_find_device(PCI_BUS *pbus, uint16_t device)
{
	int			i;
	PCI_DEVICE	*dev;
	PCI_DEVICE	*d;

	if ( !pbus->num_dev )
	{
		return NULL;
	}

	dev = pbus->head;

	for (i=0, d=dev; i<pbus->num_dev; i++, d=d->next)
	{
		if (d->device_id == device)
		{
			return d;
		}
	}

	return NULL;
}
//=============================================================================
//  pci_find_vendor_device
//=============================================================================
PCI_DEVICE* pci_find_vendor_device(PCI_BUS *pbus, uint16_t vendor, uint16_t device)
{
	int			i;
	PCI_DEVICE	*dev;
	PCI_DEVICE	*d;

	if (!pbus->num_dev)
		return NULL;

	dev = pbus->head;

	for (i=0, d=dev; i<pbus->num_dev; i++, d=d->next)
	{
		if (d->vendor_id == vendor && d->device_id == device)
		{
			return d;
		}
	}

	return NULL;
}
//=============================================================================
//  pci_find_function
//=============================================================================
PCI_DEVICE* pci_find_function(PCI_BUS *pbus, uint8_t bus, uint8_t dev, uint8_t fun)
{
	int			i;
	PCI_DEVICE	*pdev;
	PCI_DEVICE	*d;

	if (!pbus->num_dev)
		return NULL;

	pdev = pbus->head;

	for (i=0, d=pdev; i<pbus->num_dev; i++, d=d->next)
	{
		if ((d->bus == bus) && (d->dev == dev) && (d->fun == fun))
		{
			return d;
		}
	}

	return NULL;

}
//=============================================================================
//  pci_find_class_subclass_code
//=============================================================================
PCI_DEVICE* pci_find_class_subclass_code(PCI_BUS *pbus, uint8_t class_code, uint8_t subclass_code)
{
	int		i;
	PCI_DEVICE *pdev;
	PCI_DEVICE *d;

	if (!pbus->num_dev)
		return NULL;

	pdev = pbus->head;

	for (i=0, d=pdev; i<pbus->num_dev; i++, d=d->next)
	{
		if ((d->class_code[2] == class_code) && (d->class_code[1] == subclass_code))
		{
			return d;
		}
	}

	return NULL;
}

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev = NULL;
uint8_t EFIPCIInit(void)
{
	EFI_STATUS status;
	status = gBS->LocateProtocol(&gEfiPciRootBridgeIoProtocolGuid, NULL, &IoDev);
	if(EFI_ERROR(status))
	{
		return 1;
	}
	
	return 0;
}

uint8_t PCIAMDSMNRead(uint32_t addr, uint32_t *value, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH width)
{
	uint64_t offset;
	
	//write base address to pci config 0x60
	offset = EFI_PCI_ADDRESS(0, 0, 0, SMN_ADDR_REG);
	IoDev->Pci.Write(IoDev, width, offset, 1, &addr);
	
	//read data from pci config 0x64
	offset = EFI_PCI_ADDRESS(0, 0, 0, SMN_DATA_REG);
	IoDev->Pci.Read(IoDev, width, offset, 1, value);

	return 0;
}

uint8_t PCIAMDSMNWrite(uint32_t addr, uint32_t *value, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH width)
{
	uint64_t offset;
	
	//write base address to pci config 0x60
	offset = EFI_PCI_ADDRESS(0, 0, 0, SMN_ADDR_REG);
	IoDev->Pci.Write(IoDev, width, offset, 1, &addr);
	
	//write data to pci config 0x64
	offset = EFI_PCI_ADDRESS(0, 0, 0, SMN_DATA_REG);
	IoDev->Pci.Write(IoDev, width, offset, 1, value);

	return 0;
}

VOID CleanUpMp2Regs(VOID)
{
	UINT32 Data32;
	
	Data32 = 0;
	PCIAMDSMNWrite(MP2_C2PMSG_2, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_3, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_4, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_5, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_6, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_7, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_8, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_9, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_P2CMSG_0, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_P2CMSG_1, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
	
	Data32 = 0xFFFFFFFF;
	PCIAMDSMNWrite(MP2_P2CMSG_INTSTS, &Data32, EfiPciWidthUint32);
}

EFI_STATUS Mp2I2CEnable(IN UINT8 BusId, IN UINTN ClockSpeed)
{
	AMD_MP2_I2C_CMD_REG     CmdReg;
	AMD_MP2_I2C_RESPOND_REG ResReg;
	UINTN                   TimeoutCount;
	UINT32                  Data32 = 0;

	// Clear I2C status register
	CleanUpMp2Regs();
	CmdReg.Raw = 0;
	ResReg.Raw = 0;

	CmdReg.Field.i2cBusId = BusId;
	CmdReg.Field.i2c_cmd  = i2cEnable;  
	switch(ClockSpeed)
	{
		case speed100k:
		{
			CmdReg.Field.i2cSpeed = speed100k; 
			Data32 = I2C_SPEED_STANDARD;
			break;
		}
		case speed400k:
		{
			CmdReg.Field.i2cSpeed = speed400k;   
			Data32 = I2C_SPEED_FAST;
			break;
		}
		case speed1000k:
		{
			CmdReg.Field.i2cSpeed = speed1000k;
			Data32 = I2C_SPEED_FAST;
			break;
		}
	}

	// MP2 I2C transction  
	PCIAMDSMNWrite(MP2_C2PMSG_2, &Data32, EfiPciWidthUint32);
	PCIAMDSMNWrite(MP2_C2PMSG_0, &CmdReg.Raw, EfiPciWidthUint32);

	//Read I2C status
	TimeoutCount = 1000;
	PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
	while((Data32 == 0) && (TimeoutCount > 0))
	{
		gBS->Stall(10);
		PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
		TimeoutCount--;
	}

	if(TimeoutCount == 0)
	{
		return EFI_NO_RESPONSE;
	}
	
	PCIAMDSMNRead(MP2_P2CMSG_INTSTS, &Data32, EfiPciWidthUint32);
	if(Data32 == 0)
	{
		return EFI_NO_RESPONSE;
	}

	PCIAMDSMNRead(MP2_P2CMSG_0, &ResReg.Raw, EfiPciWidthUint32);
	if((ResReg.Field.response != CommandSuccess) || (ResReg.Field.status != I2CBusEnableComplete))
	{
		return EFI_DEVICE_ERROR;
	}

	CleanUpMp2Regs();  
	return EFI_SUCCESS;
}

EFI_STATUS Mp2I2cRead(IN UINT8 BusId, IN UINT8 SlaveAddress, IN UINTN ClockSpeed, IN UINT32 Length, IN UINT8 *Buffer)
{
	EFI_STATUS               Status;
	AMD_MP2_I2C_CMD_REG      CmdReg;
	AMD_MP2_I2C_RESPOND_REG  ResReg;
	UINT32                   Data32, Index, TimeoutCount, RemainingLength;
	UINT32                   *I2cBuffer = NULL;
	UINT32                   *AllocatedBuffer = NULL;

	if((Length == 0) || (Buffer == NULL))
	{
		return EFI_INVALID_PARAMETER;
	}

	// Clear I2C status register
	CleanUpMp2Regs();
	CmdReg.Raw = 0;
	ResReg.Raw = 0;

	CmdReg.Field.i2c_cmd = i2cRead;
	CmdReg.Field.i2cBusId = BusId;
	CmdReg.Field.devAddr = SlaveAddress;
	CmdReg.Field.length = Length;
	switch(ClockSpeed)
	{
		case speed100k:
		{
			CmdReg.Field.i2cSpeed = speed100k;
			break;
		}
		case speed400k:
		{
			CmdReg.Field.i2cSpeed = speed400k;
			break;
		}
		case speed1000k:
		{
			CmdReg.Field.i2cSpeed = speed1000k;
			break;
		}
	}

	if(Length <= 32)
	{
		// Using C2PMSG directly.
		CmdReg.Field.memTypeI2cMode = useC2PMsg;
	}
	else
	{
		// Using DRAM. And it must be 32 bytes aligned.
		PCIAMDSMNRead(NBIF0EPF7CFGx00000004, &Data32, EfiPciWidthUint32);
		Data32 &= 0xFFFFFFFF;
		Data32 |= 0x4;
		PCIAMDSMNWrite(NBIF0EPF7CFGx00000004, &Data32, EfiPciWidthUint32);
		if((UINTN)Buffer & 0x1F)
		{
			Status = gBS->AllocatePool(EfiBootServicesData, Length + 32, &AllocatedBuffer);
			if(EFI_ERROR(Status))
			{
				return Status;
			}
			gBS->SetMem(AllocatedBuffer, Length + 32, 0);
			I2cBuffer = (UINT32 *)(((UINTN)AllocatedBuffer + 32) & ~0x1F);
		}
		else
		{
			I2cBuffer = (UINT32 *)Buffer;
		}
		Data32 = (UINT32)((UINTN)I2cBuffer & 0xFFFFFFFF);
		PCIAMDSMNWrite(MP2_C2PMSG_2, &Data32, EfiPciWidthUint32);
		Data32 = (UINT32)(((UINTN)I2cBuffer >> 32) & 0xFFFFFFFF);
		PCIAMDSMNWrite(MP2_C2PMSG_3, &Data32, EfiPciWidthUint32);
		CmdReg.Field.memTypeI2cMode = useDRAM;
	}

	// MP2 I2C Operation.
	PCIAMDSMNWrite(MP2_C2PMSG_0, &CmdReg.Raw, EfiPciWidthUint32);

	//Wait for mp2 response with interrupt.
	TimeoutCount = 50000;
	PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
	while((Data32 == 0) && (TimeoutCount > 0))
	{
		gBS->Stall(100);//100us
		PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
		TimeoutCount--;
	}

	if(TimeoutCount == 0)
	{
		Status = EFI_NO_RESPONSE;
		goto Exit;
	}

	PCIAMDSMNRead(MP2_P2CMSG_INTSTS, &Data32, EfiPciWidthUint32);
	if(Data32 == 0)
	{
		Status = EFI_NO_RESPONSE;
		goto Exit;
	}

	PCIAMDSMNRead(MP2_P2CMSG_0, &ResReg.Raw, EfiPciWidthUint32);
	if((ResReg.Field.response != CommandSuccess) || (ResReg.Field.status != I2CReadCompleteEvent))
	{
		Status = EFI_DEVICE_ERROR;
		goto Exit;
	}

	if(Length <= 32)
	{
		I2cBuffer = (UINT32 *) Buffer;
		RemainingLength = Length;
		for(Index = 0; RemainingLength > 0; Index++)
		{
			PCIAMDSMNRead(MP2_C2PMSG_2 + Index * sizeof(UINT32), &Data32, EfiPciWidthUint32);
			I2cBuffer[Index] = Data32;
			RemainingLength -= sizeof(UINT32);
		}
	}
	else
	{
		if(AllocatedBuffer != NULL)
		{
			gBS->CopyMem (Buffer, I2cBuffer, (UINTN)Length);
		}
	}

	Status = EFI_SUCCESS;

Exit:  
	if(AllocatedBuffer != NULL)
	{ 
		gBS->FreePool(AllocatedBuffer);
	}
	
	// Clear I2C status register
	CleanUpMp2Regs();
	
	return Status;
}

EFI_STATUS Mp2I2cWrite(IN UINT8 BusId, IN UINT8 SlaveAddress, IN UINTN ClockSpeed, IN UINT32 Length, IN UINT8 *Buffer)
{
	EFI_STATUS               Status;
	AMD_MP2_I2C_CMD_REG      CmdReg;
	AMD_MP2_I2C_RESPOND_REG  ResReg;
	UINT32                   Data32, Index, TimeoutCount;
	UINT32                   *I2cBuffer = NULL;
	UINT32                   *AllocatedBuffer = NULL;

	if((Length == 0) || (Buffer == NULL))
	{
		return EFI_INVALID_PARAMETER;
	}

	// Clear I2C status register
	CleanUpMp2Regs();
	CmdReg.Raw = 0;
	ResReg.Raw = 0;

	CmdReg.Field.i2c_cmd = i2cWrite;
	CmdReg.Field.i2cBusId = BusId;
	CmdReg.Field.devAddr = SlaveAddress;
	CmdReg.Field.length = Length;
	switch(ClockSpeed)
	{
		case speed100k:
		{
			CmdReg.Field.i2cSpeed = speed100k;
			break;
		}
		case speed400k:
		{
			CmdReg.Field.i2cSpeed = speed400k;
			break;
		}
		case speed1000k:
		{
			CmdReg.Field.i2cSpeed = speed1000k;
			break;
		}
	}

	if(Length <= 32)
	{
		// Using C2PMSG directly.
		I2cBuffer = (UINT32 *)Buffer;
		for(Index = 0; Length > 0; Index++)
		{
			PCIAMDSMNWrite(MP2_C2PMSG_2 + Index * sizeof(UINT32), &I2cBuffer[Index], EfiPciWidthUint32);
			Length -= sizeof(UINT32);
		}  
		CmdReg.Field.memTypeI2cMode = useC2PMsg;
	}
	else
	{
		// Pass the data to MP2 from DRAM. And it must be 32 bytes aligned.
		PCIAMDSMNRead(NBIF0EPF7CFGx00000004, &Data32, EfiPciWidthUint32);
		Data32 &= 0xFFFFFFFF;
		Data32 |= 0x4;
		PCIAMDSMNWrite(NBIF0EPF7CFGx00000004, &Data32, EfiPciWidthUint32);
		if((UINTN)Buffer & 0x1F)
		{
			Status = gBS->AllocatePool(EfiBootServicesData, Length + 32, &AllocatedBuffer);
			if(EFI_ERROR(Status))
			{
				return Status;
			}
			gBS->SetMem(AllocatedBuffer, Length + 32, 0);
			I2cBuffer = (UINT32 *)(((UINTN)AllocatedBuffer + 32) & ~0x1F);
			gBS->CopyMem (I2cBuffer, Buffer, (UINTN) Length);
		}
		else
		{
			I2cBuffer = (UINT32 *)Buffer;
		}
		Data32 = (UINT32)((UINTN)I2cBuffer & 0xFFFFFFFF);
		PCIAMDSMNWrite(MP2_C2PMSG_2, &Data32, EfiPciWidthUint32);
		Data32 = (UINT32)(((UINTN)I2cBuffer >> 32) & 0xFFFFFFFF);
		PCIAMDSMNWrite(MP2_C2PMSG_3, &Data32, EfiPciWidthUint32);
		CmdReg.Field.memTypeI2cMode = useDRAM;
	}

	// MP2 I2C Operation.
	PCIAMDSMNWrite(MP2_C2PMSG_0, &CmdReg.Raw, EfiPciWidthUint32);

	//Wait for mp2 response with interrupt.
	TimeoutCount = 50000;
	PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
	while((Data32 == 0) && (TimeoutCount > 0))
	{
		gBS->Stall(100);//100us
		PCIAMDSMNRead(MP2_P2CMSG_INTEN, &Data32, EfiPciWidthUint32);
		TimeoutCount--;
	}

	if(TimeoutCount == 0)
	{
		Status = EFI_NO_RESPONSE;
		goto Exit;
	}

	PCIAMDSMNRead(MP2_P2CMSG_INTSTS, &Data32, EfiPciWidthUint32);
	if(Data32 == 0)
	{
		Status = EFI_NO_RESPONSE;
		goto Exit;
	}

	PCIAMDSMNRead(MP2_P2CMSG_0, &ResReg.Raw, EfiPciWidthUint32);
	if((ResReg.Field.response != CommandSuccess) || (ResReg.Field.status != I2CWriteCompleteEvent))
	{
		Status = EFI_DEVICE_ERROR;
		goto Exit;
	}

	Status = EFI_SUCCESS;

Exit:  
	if(AllocatedBuffer != NULL)
	{
		gBS->FreePool(AllocatedBuffer);
	}
	
	// Clear I2C status register
	CleanUpMp2Regs();
	
	return Status;
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  SMBus                                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//=============================================================================
//  smb_create_device
//=============================================================================
static SMB_DEVICE* smb_create_device(void)
{
	SMB_DEVICE	*dev;

	dev = (SMB_DEVICE *)malloc( sizeof(SMB_DEVICE) );
	if (!dev)
	{
		//fprintf(stderr, "malloc err!\n");
		return NULL;
	}
	dev->addr = 0xFF;
	dev->type = 0xFF;
	dev->prev = NULL;
	dev->next = NULL;

	return dev;
}

//=============================================================================
//  smb_delay
//=============================================================================
void smb_delay(void)
{
	outp(0x80, 0x80);	// gBS->Stall
}
//=============================================================================
//  smb_scan_bus_intel_amd
//=============================================================================
int smb_scan_bus_intel_amd(SMB_BUS *smbus)
{
	//int			i;
	uint16_t	i;
	uint8_t		sts;
	uint8_t		addr;
	SMB_DEVICE	*dev;
	SMB_DEVICE	*sdev = NULL;

	smbus->dev_num = 0;

	// 8-bit address (including R/W bit)
	for (i=0; i<256; i+=2)
	{
		addr = (uint8_t)i + 1;

    	//outp( smbus->base+SMB_HST_STS, 0x1E );   
    	outp( smbus->base+SMB_HST_STS, 0xFE );
	    outp( smbus->base+SMB_HST_ADD, addr );   
    	outp( smbus->base+SMB_HST_CMD, 0x00 );   
    	outp( smbus->base+SMB_HST_CNT, 0x48 );
    	
    	// Todo : might block here
    	while( 1 )
    	{
    		sts = inp( smbus->base );
    		if ((sts & 0x01) == 0)
    		{
    			break;
    		}
        	gBS->Stall(1);
    	}
		// 0x42 device found
    	// 0x44 device not found

    	//if (sts == 0x42) {
    	if ( (sts & 0x06) == 0x02 )
    	{
    		//smbus->dev_list[ smbus->dev_num++ ] = addr & 0xFE;	// mask R/W bit

			dev = smb_create_device();
			if (!dev)
			{
				fprintf(stderr, "smb_create_device error!\n");
				return -1;
			}

			dev->addr = addr & 0xFE;	// mask R/W bit

			if ( smbus->dev_num == 0 )
			{
				// 1st device
				smbus->dev_head = dev;
				sdev = dev;
			}
			else
			{	
				// 2nd device..
				sdev->next = dev;
				dev->prev = sdev;
				sdev = dev;
			}

			smbus->dev_num++;
    	}
    }

	return 0;
}
//=============================================================================
//  smb_scan_bus_nvidia
//=============================================================================
int smb_scan_bus_nvidia(SMB_BUS *smbus)
{
	int			i;
	uint8_t		sts;
	uint8_t		addr;
	uint32_t	loops;
	SMB_DEVICE	*dev;
	SMB_DEVICE	*sdev = NULL;

	smbus->dev_num = 0;

	for (i=0; i<256; i+=2)
	{
		addr = (i & 0xFF) + 1;

		outp(smbus->base+0x02, addr);
		smb_delay();

		outp(smbus->base+0x03, 0x00);
		smb_delay();

		outp(smbus->base+0x00, 0x07);
		smb_delay();

		loops = 1000000;
		do {

			smb_delay();

			sts = inp(smbus->base+0x01);
			if ( (sts & 0xFF) != 0 )
			{
				break;
			}
		} while (--loops);

		sts = inp(smbus->base+0x01) & 0x80;
    	if (sts == 0x80)
    	{
    		//smbus->dev_list[ smbus->dev_num++ ] = addr & 0xFE;	// mask R/W bit

			dev = smb_create_device();
			if (!dev)
			{
				fprintf(stderr, "smb_create_device error!\n");
				return -1;
			}

			dev->addr = addr & 0xFE;	// mask R/W bit

			if ( smbus->dev_num == 0 )
			{
				// 1st device
				smbus->dev_head = dev;
				sdev = dev;
			}
			else
			{	
				// 2nd device..
				sdev->next = dev;
				dev->prev = sdev;
				sdev = dev;
			}

			smbus->dev_num++;

    	}
    }

	return 0;
}
//=============================================================================
//  smb_read_byte_intel_amd
//=============================================================================
uint8_t	smb_read_byte_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg)
#if 1
{
	while ( inp(smbus->base+SMB_HST_STS) & 0x01 )
	{
		gBS->Stall(1);
	}
	outp( smbus->base+SMB_HST_STS, 0x1E );
	// slave address + R/W operation
	outp( smbus->base+SMB_HST_ADD, addr|0x1 );	// slave address + read
	// device register address
	outp( smbus->base+SMB_HST_CMD, reg );
	outp( smbus->base+SMB_HST_CNT, 0x48);	// START + Byte Read

	while ( inp(smbus->base+SMB_HST_STS) & 0x01 )
	{
		gBS->Stall(1);
	}

	return inp( smbus->base+SMB_HST_DAT0 );
}
#else
{
	uint8_t	status;
	int		timeout = 0;

	// Intel, ATI and AMD

	// clear register status
	//outp(smbus->base+SMB_HST_STS, 0x1E);

	outp( smbus->base+SMB_HST_STS, 0xFE );
	gBS->Stall( 1 );

	// slave address + R/W operation
	outp( smbus->base+SMB_HST_ADD, addr|0x1 );	// slave address + read

	// device register address
	outp( smbus->base+SMB_HST_CMD, reg );

	// SMB_HST_CNT
	//-------------
	// [7]-PEC_EN
	// [6]-START
	// [5]-LAST_BYTE
	// [4:2]-SMB_CMD
	//   000-Quick
	//   001-Byte
	//   010-Byte Read
	//   011-Word Data
	//   100-Process Call
	//   101-Block
	//   110-I2C Read
	//   111-Block Process
	// [1]-KILL
	// [0]-INTREN
	outp(smbus->base+SMB_HST_CNT, 0x48);	// START + Byte Read


	// SMB_HST_STS
	//-------------
	// [7]-BYTE_DONE
	// [6]-INUSE_STS
	// [5]-SMBALERT_STS
	// [4]-FAILED
	// [3]-BUS_ERR
	// [2]-DEV_ERR
	// [1]-INTR
	// [0]-HOST_BUSY
	//---------------
	// STS_ERR_FLAGS : [4] | [3] | [2] = 0x1C
	// STS_FLAGS     : [7] | [1] | ERR_FLAG = 0x9E
	do
	{
		gBS->Stall(1);
		status = inp(smbus->base+SMB_HST_STS);

	} while ( ((status & 0x01) || !(status & 0x1E) ) &&
				(timeout++ < 100));

	if ( timeout >= 100 )
	{
		// try to stop the current command
		// timeout : 0x02=HST_CNT.KILL
		outp( smbus->base+SMB_HST_CNT, (inp(smbus->base+SMB_HST_CNT) | 0x02) );
		gBS->Stall(1);
		outp( smbus->base+SMB_HST_CNT, (inp(smbus->base+SMB_HST_CNT) & (~0x02)) );

		// check if it worked
		status = inp(smbus->base+SMB_HST_STS);	// 01-BUSY, 
		if ( (status & 0x01) || !(status & 0x10) )
		{
			// failed terminating the transaction
		}

		outp( smbus->base+SMB_HST_STS, 0x9E );

		return 0xFF;	// error
	}
	else
	{
		outp( smbus->base+SMB_HST_STS, status );
	}


	return inp( smbus->base+SMB_HST_DAT0 );
}
#endif
//=============================================================================
//  smb_read_word
//=============================================================================
uint16_t	smb_read_word_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg)
#if 1
{
	uint16_t	data;

	// Intel, ATI and AMD
	outp(smbus->base+SMB_HST_STS, 0xFE);
	gBS->Stall(10);
	
	outp(smbus->base+SMB_HST_ADD, addr|0x1);	// slave address + read
	outp(smbus->base+SMB_HST_CMD, reg);
	outp(smbus->base+SMB_HST_CNT, 0x4C);

	while ( inp(smbus->base+SMB_HST_STS) & 0x1 )
	{
		gBS->Stall(1);
	}

	data = inp( smbus->base+SMB_HST_DAT1 );
	data <<= 8;
	data |= inp( smbus->base+SMB_HST_DAT0 );

	return data;
}
#else
{
	uint8_t		status;
	int		timeout = 0;
	uint16_t		data;

	// slave address + R/W operation
	outp( smbus->base+SMB_HST_ADD, addr|0x1 );	// slave address + read

	// device register address
	outp( smbus->base+SMB_HST_CMD, reg );
	
	outp( smbus->base+SMB_HST_CNT, 0x4C );	// START + Word Read

	do
	{
		gBS->Stall(1);
		status = inp(smbus->base+SMB_HST_STS);

	} while ( ((status & 0x01) || !(status & 0x1E) ) &&
				(timeout++ < 100));

	if ( timeout >= 100 )
	{
		outp( smbus->base+SMB_HST_CNT, (inp(smbus->base+SMB_HST_CNT) | 0x02) );
		gBS->Stall(1);
		outp( smbus->base+SMB_HST_CNT, (inp(smbus->base+SMB_HST_CNT) & (~0x02)) );

		// check if it worked
		status = inp(smbus->base+SMB_HST_STS);	// 01-BUSY, 
		if ( (status & 0x01) || !(status & 0x10) )
		{
			// failed terminating the transaction
		}

		outp( smbus->base+SMB_HST_STS, 0x9E );

		return 0xFFFF;	// error
	}
	else
	{
		outp( smbus->base+SMB_HST_STS, status );
	}

	data = inp( smbus->base+SMB_HST_DAT1 );
	data <<= 8;
	data |= inp( smbus->base+SMB_HST_DAT0 );

	return data;
}
#endif
//=============================================================================
//  write_byte
//=============================================================================
uint8_t	smb_write_byte_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg, uint8_t data)
#if 1
{
	while ( inp(smbus->base+SMB_HST_STS) & 0x1 )
	{
		gBS->Stall(1);
	}
	outp( smbus->base+SMB_HST_STS, 0xFE);
	gBS->Stall(10);
	
	outp( smbus->base+SMB_HST_ADD, addr);	// slave address + write
	outp( smbus->base+SMB_HST_CMD, reg);
	
	outp( smbus->base+SMB_HST_DAT0, data);

	outp( smbus->base+SMB_HST_CNT, 0x48);
	return 0;
}

#else
{
	// slave address + R/W operation
	outp( smbus->base+SMB_HST_ADD, addr );	// slave address + read

	// device register address
	outp( smbus->base+SMB_HST_CMD, reg );

	outp( smbus->base+SMB_HST_DAT0, data);


	outp(smbus->base+SMB_HST_CNT, 0x48);	// START + Byte Read
	
}
#endif

//=============================================================================
//  write_word
//=============================================================================
uint8_t	smb_write_word_intel_amd(SMB_BUS *smbus, uint8_t addr, uint8_t reg, uint16_t data)
#if 1
{
	// Intel, ATI and AMD
	outp( smbus->base+SMB_HST_STS, 0xFE);
	gBS->Stall(10);
	
	outp( smbus->base+SMB_HST_ADD, addr);	// slave address + write
	outp( smbus->base+SMB_HST_CMD, reg);
	
	outp( smbus->base+SMB_HST_DAT0, data&0xFF );
	outp( smbus->base+SMB_HST_DAT1, (data>>8)&0xFF );

	outp( smbus->base+SMB_HST_CNT, 0x4C);

	return 0;
}
#endif

//=============================================================================
//  smb_read_byte_nvidia
//=============================================================================
uint8_t	smb_read_byte_nvidia(SMB_BUS *smbus, uint8_t addr, uint8_t reg)
{
	uint32_t	loops;

	// nVidia
	outp(smbus->base+0x02, addr|0x1); // slave address + read
	smb_delay();

	outp(smbus->base+0x03, reg);
	smb_delay();

	outp(smbus->base+0x00, 0x07);
	smb_delay();

	loops = 1000000;
	do {

		if ((inp(smbus->base+0x01) & 0xFF) != 0)
		{
			break;
		}
		smb_delay();

	} while (loops--);

	//outp(smbus->base+0x01, 0x80);

	return inp(smbus->base+0x04);
}

//=============================================================================
//  smb_init
//=============================================================================
SMB_BUS *smb_init(void)
{
	PCI_BUS		*pbus;
	PCI_DEVICE	*pdev;
	PCI_DEVICE	*psmb;
	SMB_BUS		*smbus;

	pbus = pci_bus_scan();

	if ( !pbus )
	{
		return NULL;
	}

	smbus = (SMB_BUS*)malloc( sizeof(SMB_BUS) );
	if ( !smbus )
	{
		pci_bus_free( pbus );
		return NULL;
	}

	// AMD    SMBus : Bus 0, Device 31, Function 3 = 8086:8C22 (DPX-S435)
	// Intel  SMBus : Bus 0, Device 20, Function 0 = 1002:4385 (DPX-E130)
	// nVidia SMBus : Bus 0, Device  3, Function 2 = 10DE:XXXX

	memset( smbus, 0, sizeof(SMB_BUS) );

	// Vendor Check : Bus 0, Device 0, Function 0
	pdev = pci_find_function(pbus, 0, 0, 0);
	if ( pdev )
	{
		switch ( pdev->vendor_id )
		{
			// Intel
			case 0x8086:

				// Intel SMBus
				//psmb = find_pci_function(pbus, 0, 31, 3);
				//psmb = find_pci_function(pbus, 0, 31, 4);

				// Serial Controlller - SMBus
				psmb = pci_find_class_subclass_code(pbus, 0x0C, 0x05);

				if ( psmb )
				{
					if ( psmb->vendor_id == 0x8086 )
					{
						smbus->base		= (uint16_t)( psmb->bar[4] & 0xFFE0 );	// 20h
						smbus->found	= 1;
						smbus->vendor	= psmb->vendor_id;
						// scan bus
						smb_scan_bus_intel_amd( smbus );

						smbus->read_byte	= smb_read_byte_intel_amd;
						smbus->read_word	= smb_read_word_intel_amd;
						smbus->write_byte	= smb_write_byte_intel_amd;
						smbus->write_word	= smb_write_word_intel_amd;
					}
				}

				break;

			// AMD
			// ATI
			case 0x1022:
			case 0x1002:

				// AMD/ATI SMBus
				//psmb = find_pci_function(0, 20, 0);

				// Serial Controlller - SMBus
				psmb = pci_find_class_subclass_code(pbus, 0x0C, 0x05);
				if ( psmb )
				{
					if ( psmb->vendor_id == 0x1002 || psmb->vendor_id == 0x1022 ) {

						smbus->base		= (uint16_t)( 0xB00 );
						smbus->found	= 1;
						smbus->vendor	= psmb->vendor_id;
						// scan bus
						smb_scan_bus_intel_amd( smbus );

						smbus->read_byte	= smb_read_byte_intel_amd;
						smbus->read_word	= smb_read_word_intel_amd;
						smbus->write_byte	= smb_write_byte_intel_amd;
						smbus->write_word	= smb_write_word_intel_amd;

					}
				}

				break;

			// nVidia
			case 0x10DE:

				// nVidia SMBus controller
				// Serial Controlller - SMBus
				psmb = pci_find_class_subclass_code(pbus, 0x0C, 0x05);
				if ( psmb )
				{
					if ( psmb->vendor_id == 0x10DE )
					{
						smbus->base		= (uint16_t)( psmb->bar[5] & 0xFFFE );	// 24h
						smbus->found	= 1;
						smbus->vendor	= psmb->vendor_id;
						// scan bus
						smb_scan_bus_nvidia( smbus );

						smbus->read_byte= smb_read_byte_nvidia;
						// todo 
						smbus->read_word	= smb_read_word_intel_amd;
						smbus->write_byte	= smb_write_byte_intel_amd;
						smbus->write_word	= smb_write_word_intel_amd;
					}
				}

				break;

		}//switch
	}
	pci_bus_free( pbus );

	// device
/*
	if ( smbus->dev_num )
	{
		for (i=0; i<smbus->dev_num; i++)
		{
			dev = smb_create_device();
			if (!dev)
			{
				break;
			}

			if ( i==0 )
			{
				smbus->dev_head = dev;
				sdev = dev;
			}
			else
			{
				sdev->next = dev;
				dev->prev = sdev;
				sdev = dev;
			}
		}
	}
*/
	return smbus;
}
//=============================================================================
//  smb_exit
//=============================================================================
void smb_exit(SMB_BUS *smbus)
{
	SMB_DEVICE	*dev;
	SMB_DEVICE	*sdev;

	if ( smbus )
	{
		dev = smbus->dev_head;
		while ( dev )
		{
			sdev = dev->next;
			free(dev);
			dev = sdev;
		}		
		free( smbus );
	}
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  Reset                                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************


// Port : CF9
// bit3 - FULL_RST: full rest (reset SLP_S3#/SLP_S4#/SLP_S5# pin state)
//        1=reset SLP_S3#/SLP_S4#/SLP_S5# pin state
//        0=keep  SLP_S3#/SLP_S4#/SLP_S5# pin state
// bit2 - RST_CPU : reset CPU (sw or hw reset --> CPU)
// bit1 - SYS_RST : system reset

//  RST_CPU   SYS_RST
//     1        0       CPU_INIT# = L (16 PCI clock) --> software reset
//     1        1       PLT_RST# = L, SUS_STAT# = L  --> hardware reset
//===========================================================================
//  full_reset
//===========================================================================
void full_reset(void)
{
	outp(0xCF9, inp(0xCF9) | 0x0E);	// hardware reset + SLP pin state
}

//===========================================================================
//  hard_reset
//===========================================================================
void hard_reset(void)
{
	outp(0xCF9, inp(0xCF9) | 0x06);	// aka platform reset
}

//===========================================================================
//  soft_reset
//===========================================================================
void soft_reset(void)
{
	outp(0xCF9, inp(0xCF9) | 0x04);	// software reset, cpu reset
}

//===========================================================================
//  soft92_reset
//===========================================================================
void soft92_reset(void)
{
	outp(0x92, 0x01);	// software reset
}

//===========================================================================
//  kbc_reset
//===========================================================================
void kbc_reset(void)
{
	outp(0x64, 0xFE);	// software reset
}

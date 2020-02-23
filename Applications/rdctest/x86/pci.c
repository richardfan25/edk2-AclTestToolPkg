//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - PCI                                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

#include <stdlib.h>
#include <string.h>


#include "pci.h"


//=============================================================================
//  define
//=============================================================================
#define	PCI_CMD_PORT	0xcf8
#define	PCI_DAT_PORT	0xcfc

//=============================================================================
//  variables
//=============================================================================
pci_bus_t *pci = NULL;
pci_dev_t *gPciOperational = NULL;


static pci_dev_t *PciAddDev(void)
{
	pci_dev_t	*dev;

	dev = (pci_dev_t *)malloc(sizeof(pci_dev_t));
	if (!dev)
		return NULL;

	memset(dev, 0, sizeof(pci_dev_t));

	return dev;
}

static pci_cap_t *PciAddCap(void)
{
	pci_cap_t	*cap;

	cap = (pci_cap_t *)malloc(sizeof(pci_cap_t));
	if (!cap)
		return NULL;

	memset(cap, 0, sizeof(pci_cap_t));

	return cap;
}

EFI_STATUS PciGetProtocolAndResource(EFI_HANDLE Handle, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL **IoDev, EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR **Descriptors)
{
	EFI_STATUS  Status;

	Status = gBS->HandleProtocol(Handle, &gEfiPciRootBridgeIoProtocolGuid, IoDev);

	if(EFI_ERROR(Status))
	{
		return Status;
	}
	
	Status = (*IoDev)->Configuration(*IoDev, Descriptors);
	if(Status == EFI_UNSUPPORTED)
	{
		*Descriptors = NULL;
		return EFI_SUCCESS;
	}
	else
	{
		return Status;
	}
}

EFI_STATUS PciGetNextBusRange(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors, UINT16 *MinBus, UINT16 *MaxBus, BOOLEAN *IsEnd)
{
	*IsEnd = FALSE;

	if((*Descriptors) == NULL)
	{
		*MinBus = 0;
		*MaxBus = (UINT16)PCI_MAX_BUS;
		return EFI_SUCCESS;
	}

	while((*Descriptors)->Desc != ACPI_END_TAG_DESCRIPTOR)
	{
		if((*Descriptors)->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS)
		{
			*MinBus = (UINT16) (*Descriptors)->AddrRangeMin;
			*MaxBus = (UINT16) (*Descriptors)->AddrRangeMax;
			(*Descriptors)++;
			return EFI_SUCCESS;
		}

		(*Descriptors)++;
	}

	if((*Descriptors)->Desc == ACPI_END_TAG_DESCRIPTOR)
	{
		*IsEnd = TRUE;
	}

	return EFI_SUCCESS;
}

uint8_t EFIPCIInit(void)
{
	EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors = NULL;
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev = NULL;
	EFI_HANDLE *HandleBuf = NULL;
	EFI_STATUS status;
	
	UINT64 address;
	UINT16 bus;
	UINT16 device;
	UINT16 func;
	UINT16 minbus;
	UINT16 maxbus;
	UINTN index;
	UINTN count;
	UINTN bufsize;
	BOOLEAN isend;

	pci_dev_t *d = NULL, *pd = NULL;
	pci_cap_t *cap = NULL, *pcap = NULL;
	uint32_t data32;
	uint32_t pci_cfg[64];
	uint32_t pci_cfg0[64];
	uint8_t cap_addr, *pci_conf;

	bufsize = sizeof(EFI_HANDLE);
	HandleBuf = (EFI_HANDLE *)AllocatePool(bufsize);
	if(HandleBuf == NULL)
	{
		status = PCI_ERR_NULL_BUS;
		goto done;
	}

	status = gBS->LocateHandle(ByProtocol, &gEfiPciRootBridgeIoProtocolGuid, NULL, &bufsize, HandleBuf);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		HandleBuf = ReallocatePool(bufsize, sizeof (EFI_HANDLE), HandleBuf);
		if(HandleBuf == NULL)
		{
			status = PCI_ERR_NULL_BUS;
			goto done;
		}

		status = gBS->LocateHandle(ByProtocol, &gEfiPciRootBridgeIoProtocolGuid, NULL, &bufsize, HandleBuf);
		if(EFI_ERROR(status))
		{
			status = PCI_ERR_NULL_BUS;
			goto done;
		}
	}

	pci = (pci_bus_t *)malloc(sizeof(pci_bus_t));
	if(!pci)
	{
		status = PCI_ERR_MALLOC_BUS;
		goto done;
	}
	memset(pci, 0, sizeof(pci_bus_t));
	
	count = bufsize / sizeof(EFI_HANDLE);
	for(index=0; index<count; index++)
	{
		status = PciGetProtocolAndResource(HandleBuf[index], &IoDev, &Descriptors);
		if(EFI_ERROR(status))
		{
			status = PCI_ERR_NULL_BUS;
			goto done;
		}
		
		while(TRUE)
		{
			status = PciGetNextBusRange(&Descriptors, &minbus, &maxbus, &isend);
			if(EFI_ERROR(status))
			{
				status = PCI_ERR_NULL_BUS;
				goto done;
			}

			if(isend)
			{
				break;
			}

			for(bus=minbus; bus<=maxbus; bus++)
			{
				for(device=0; device<PCI_NUM_DEV; device++)
				{
					for(func=0; func<PCI_NUM_FUN; func++)
					{
						// select device
						address = EFI_PCI_ADDRESS(bus, device, func, 0);
						
						IoDev->Pci.Read(IoDev, EfiPciWidthUint32, address, 1, &data32);
						if((data32 == 0xffffffff) || (data32 == 0))
						{
							if(func == 0)
							{
								break;
							}
							continue;
						}

						// reading pci configuration
						IoDev->Pci.Read(IoDev, EfiPciWidthUint32, address, sizeof(pci_cfg) / sizeof(UINT32), &pci_cfg);
						if(memcmp(pci_cfg0, pci_cfg, sizeof(pci_cfg)) == 0)
						{
							continue;	// duplicate, skip the device
						}
						memcpy(pci_cfg0, pci_cfg, sizeof(pci_cfg));

						// TODO : PCI bridge
						d = PciAddDev();
						if(!d)
						{
							status = PCI_ERR_MALLOC_DEV;
							goto done;
						}

						// device link-list
						if(pci->num_dev == 0)
						{
							pci->dev = d;		// 1st device
							pci->dev->prev = NULL;
						}
						else
						{
							pd->next = d;
							d->prev = pd;
							d->next = NULL;
						}
						pd = d;
						pci->num_dev++;

						// loading device info
						pd->bus = (uint8_t)bus;
						pd->dev	= (uint8_t)device;
						pd->fun = (uint8_t)func;
						pd->IoDev = IoDev;
						pd->Handle = HandleBuf[index];

						// loading pci configuration
						memcpy(&pd->cfg, pci_cfg, sizeof(pci_cfg_t));

						// capabilities list
						pci_conf = (uint8_t *)&pd->cfg;
						cap_addr = pci_conf[0x34];	// capabilities pointer
						while(cap_addr)
						{
							cap = PciAddCap();
							if(!cap)
							{
								status = PCI_ERR_MALLOC_CAP;
								goto done;
							}

							if(pd->num_cap == 0)
							{
								pd->cap_lst = cap;	// 1st cap
							}
							else
							{
								pcap->next = cap;
								cap->prev = pcap;
							}
							pcap = cap;
							pd->num_cap++;

							cap->addr = cap_addr;
							cap->id = pci_conf[cap_addr];
							
							// next capabilities
							cap_addr = pci_conf[(cap_addr + 1)];
						}
					}
				}
			}
			
			if(Descriptors == NULL)
			{
				break;
			}
		}
	}

	pci->status = 0x55AA33CC;	// initialized
  	status = PCI_OK;

done:
	if(HandleBuf != NULL)
	{
		FreePool(HandleBuf);
	}
	return (uint8_t)status;
}

uint8_t PciExit(void)
{
	pci_dev_t	*dev, *d;
	pci_cap_t	*cap, *c;
	int			i, j;

	if (!pci)
		return PCI_ERR_NULL_BUS;

	// pci device
	if (pci->dev)
	{
		for (i=0, dev=pci->dev; i<pci->num_dev; i++)
		{
			// free capabilities list
			if (dev->num_cap)
			{
				for (j=0, cap=dev->cap_lst; j<dev->num_cap; j++)
				{
					c = cap;
					cap = c->next;
					if (c)
						free(c);
				}
			}

			// free pci device list
			d = dev;
			dev = d->next;
			if (d)
				free(d);
		}
	}

	pci->status = 0;	// uninitialized
	free(pci);
	pci = NULL;

	return PCI_OK;
}

uint32_t PciStatus(void)
{
	if (!pci)
		return 0;

	return pci->status;
}

uint8_t PciFindOperatingBusDevFun(uint8_t bus, uint8_t dev, uint8_t fun)
{
	pci_dev_t *pd;
	int i;

	if (pci->num_dev == 0)
		return 1;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->bus == bus) &&
			(pd->dev == dev) &&
			(pd->fun == fun))
		{
			gPciOperational = pd;
			return 0;
		}
	}

	return 1;
}

uint8_t PciFindOperatingVenDev(uint16_t ven_id, uint16_t dev_id)
{
	pci_dev_t *pd;
	int i;

	if (pci->num_dev == 0)
		return 1;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->cfg.ven_id == ven_id) &&
			(pd->cfg.dev_id == dev_id))
		{
			gPciOperational = pd;
			return 0;
		}
	}

	return 1;
}

uint8_t PCIRWAMDSMN(uint32_t addr, uint32_t *value, uint8_t rw, EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH width)
{
	uint64_t address;

	address = EFI_PCI_ADDRESS(gPciOperational->bus, gPciOperational->dev, gPciOperational->fun, addr);
	if(rw)	//write
	{
		gPciOperational->IoDev->Pci.Write(gPciOperational->IoDev, width, address, 1, value);
	}
	else		//read
	{
		gPciOperational->IoDev->Pci.Read(gPciOperational->IoDev, width, address, 1, value);
	}

	return 0;
}

uint8_t PCIGetAMDThrottleStatus(uint32_t family)
{
	uint8_t status = 0;
	uint32_t tmp;
	
	PCIRWAMDSMN(SMN_DATA_REG, &tmp, 0, EfiPciWidthUint8);
	switch(family)
	{
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x14:
		case 0x15:
		case 0x16:
		{
			if(tmp & (1 << 4))//check HTC-active state(bit 4)
			{
				status = 1;
			}
			else
			{
				status = 0;
			}
			break;
		}
		case 0x17:
		{
			if(tmp & (1 << 2))//check external PROCHOT pad being asserted(bit 2)
			{
				status = 1;
			}
			else
			{
				status = 0;
			}
			break;
		}
	}

	return status;
}

uint8_t PCIRestoreAMDThrottleConfig(uint32_t family, uint32_t reg_bak, uint32_t data_bak)
{
	switch(family)
	{
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x14:
		case 0x15:
		case 0x16:
		{
			//restore pci config 0x64 to origin data, and clear HTC_ACTIVE_LOG(bit 5)
			data_bak |= (1 << 5);
			PCIRWAMDSMN(SMN_DATA_REG, &data_bak, 1, EfiPciWidthUint8);
			break;
		}
		case 0x17:
		{
			//restore pci config 0x64 to origin data, and clear HTC_ACTIVE_LOG(bit 5)
			data_bak |= (1 << 5);
			PCIRWAMDSMN(SMN_DATA_REG, &data_bak, 1, EfiPciWidthUint16);
			
			//restore pci config 0x60 to origin data
			PCIRWAMDSMN(SMN_ADDR_REG, &reg_bak, 1, EfiPciWidthUint32);
			
			break;
		}
	}

	return 0;
}


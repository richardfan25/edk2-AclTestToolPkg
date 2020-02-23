//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - PCI                                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <dos.h>
#include <stdlib.h>
#include <string.h>

#include "x86io.h"
#include "x86io32.h"
#include "pci.h"
#include "csm.h"

//=============================================================================
//  define
//=============================================================================
#define	PCI_CMD_PORT	0xcf8
#define	PCI_DAT_PORT	0xcfc

//=============================================================================
//  variables
//=============================================================================
pci_bus_t	*pci = NULL;

//=============================================================================
//  pci_sel_dev
//=============================================================================
void pci_sel_dev(uint8_t bus, uint8_t dev, uint8_t fun)
{
	//----------------------------------
	//  iocmd
	//----------------------------------
	// bit[31]    - always 1
	// bit[30:24] - 0
	// bit[23:16] - bus
	// bit[15:11] - device
	// bit[10:8]  - function
	// bit[7:2]   - register
	// bit[1:0]   - 0
	//----------------------------------
	pci->iocmd = 0x8000;
	pci->iocmd |= bus;
	pci->iocmd <<= 5;
	pci->iocmd |= dev;
	pci->iocmd <<= 3;
	pci->iocmd |= fun;
	pci->iocmd <<= 8;

	//----------------------------------
	//  mmio
	//----------------------------------
	// bit[31:28] pci_e base address
	// bit[27:20] bus
	// bit[19:15] device
	// bit[14:12] function
	// bit[11:0]  ext config space (4KB)
	//----------------------------------
	pci->mmio = bus;
	pci->mmio <<= 5;
	pci->mmio |= dev;
	pci->mmio <<= 3;
	pci->mmio |= fun;
	pci->mmio <<= 12;

	if (pci->nocsm == 0)
		pci->mmio += pci->csmp->info.pcie_base;
}

//=============================================================================
//  pci_read_dword
//=============================================================================
uint32_t pci_read_dword(uint8_t reg)
{
	pci->iocmd &= 0xffffff00;
	pci->iocmd |= (reg & 0xfc);

	outd(PCI_CMD_PORT, pci->iocmd);

	return ind(PCI_DAT_PORT);
}

//=============================================================================
//  pci_read_byte
//=============================================================================
uint8_t pci_read_byte(uint8_t reg)
{
	uint32_t	data32;
	uint8_t		data8 = 0;

	pci->iocmd &= 0xffffff00;
	pci->iocmd |= (reg & 0xfc);

	outd(PCI_CMD_PORT, pci->iocmd);
	data32 = ind(PCI_DAT_PORT);

	switch(reg & 0x3)
	{
		case 0: data8 = data32 & 0xff;			break;
		case 1: data8 = (data32>>8) & 0xff;		break;
		case 2: data8 = (data32>>16) & 0xff;	break;
		case 3: data8 = (data32>>24) & 0xff;	break;
	}

	return data8;	//return ( (data32 >> ((reg & 0x3) << 3)) & 0xff );
}

//=============================================================================
//  pci_write_dword
//=============================================================================
void pci_write_dword(uint8_t reg, uint32_t data)
{
	pci->iocmd &= 0xffffff00;
	pci->iocmd |= (reg & 0xfc);

	outd(PCI_CMD_PORT, pci->iocmd);
	outd(PCI_DAT_PORT, data);
}

//=============================================================================
//  pci_write_byte
//=============================================================================
void pci_write_byte(uint8_t reg, uint8_t data)
{
	uint32_t	tmp;
	uint32_t	mask;
	uint8_t		off;

	pci->iocmd &= 0xffffff00;
	pci->iocmd |= (reg & 0xfc);
	outd(PCI_CMD_PORT, pci->iocmd);


	off = (reg & 0x3)<<3;		// 0, 8, 16, 24
	mask = 0xff;
	mask <<= (1<<off);			// 0x000000FF
								// 0x0000FF00
								// 0x00FF0000
								// 0xFF000000

	mask ^= 0xffffffff;			// 0xFFFFFF00
								// 0xFFFF00FF
								// 0xFF00FFFF
								// 0x00FFFFFF

	tmp = ind(PCI_DAT_PORT);
	tmp &= mask;

	mask = data;
	mask <<= off;
	tmp |= mask;

	outd(PCI_DAT_PORT, tmp);
}

//=============================================================================
//  pci_mmio_read_dword
//=============================================================================
uint32_t pci_mmio_read_dword(uint16_t reg)
{
	return *(uint32_t *)(pci->mmio + reg);
}

//=============================================================================
//  pci_mmio_read_byte
//=============================================================================
uint8_t pci_mmio_read_byte(uint16_t reg)
{
	return *(uint8_t *)(pci->mmio + reg);
}

//=============================================================================
//  pci_mmio_write_dword
//=============================================================================
void pci_mmio_write_dword(uint16_t reg, uint32_t data)
{
	*(uint32_t *)(pci->mmio + reg) = data;
}

//=============================================================================
//  pci_mmio_write_byte
//=============================================================================
void pci_mmio_write_byte(uint16_t reg, uint8_t data)
{
	*(uint8_t *)(pci->mmio + reg) = data;
}

//=============================================================================
//  pci_add_dev
//=============================================================================
static pci_dev_t *pci_add_dev(void)
{
	pci_dev_t	*dev;

	dev = (pci_dev_t *)malloc(sizeof(pci_dev_t));
	if (!dev)
		return NULL;

	memset(dev, 0, sizeof(pci_dev_t));

	return dev;
}

//=============================================================================
//  pci_del_dev
//=============================================================================
#if 0
// mark this unused function due to avoid warning message
static pci_dev_t *pci_del_dev(pci_dev_t *pd)
{
	pci_dev_t	*pd0, *pd1;

	if (!pd)
		return NULL;

	pd0 = pd->prev;
	pd1 = pd->next;

	if (pd0)
		pd0->next = pd1;

	if (pd1)
		pd1->prev = pd0;

	free(pd);

	return pd0;
}
#endif

//=============================================================================
//  pci_add_cap
//=============================================================================
static pci_cap_t *pci_add_cap(void)
{
	pci_cap_t	*cap;

	cap = (pci_cap_t *)malloc(sizeof(pci_cap_t));
	if (!cap)
		return NULL;

	memset(cap, 0, sizeof(pci_cap_t));

	return cap;
}

//=============================================================================
//  pci_add_ecap
//=============================================================================
static pci_ecap_t *pci_add_ecap(void)
{
	pci_ecap_t	*ecap;

	ecap = (pci_ecap_t *)malloc(sizeof(pci_ecap_t));
	if (!ecap)
		return NULL;

	memset(ecap, 0, sizeof(pci_ecap_t));

	return ecap;
}

uint8_t EFIPCIInit(void)
{
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev;
	EFI_STATUS status;
	
	UINT64 address;
	UINT16 bus;
	UINT16 device;
	UINT16 func;

	pci_dev_t *d = NULL, *pd = NULL;
	pci_cap_t *cap = NULL, *pcap = NULL;
	pci_ecap_t *ecap = NULL, *pecap = NULL;
	uint32_t data32;
	uint32_t pci_cfg[64];
	uint32_t pci_cfg0[64];
	uint8_t cap_addr, *pci_conf;
	uint8_t pcie;
	uint16_t ecap_addr, ecap_id;
	
	if(pci)
	{
		if (pci->status == PCI_INIT_OK)
		{
			return PCI_OK;
		}
		else
		{
			pci_exit();
		}
	}

	pci = (pci_bus_t *)malloc(sizeof(pci_bus_t));
	if(!pci)
	{
		status = PCI_ERR_MALLOC_BUS;
		goto done;
	}
	memset(pci, 0, sizeof(pci_bus_t));
	
	status = gBS->LocateProtocol(&gEfiPciRootBridgeIoProtocolGuid, NULL, &IoDev);
	if(EFI_ERROR(status))
	{
		status = PCI_ERR_NULL_BUS;
		goto done;
	}
	
	for(bus=0; bus<PCI_NUM_BUS; bus++)
	{
		for(device=0; device<PCI_NUM_DEV; device++)
		{
			for(func=0; func<PCI_NUM_FUN; func++)
			{
				// select device
				pci_sel_dev((uint8_t)bus, (uint8_t)device, (uint8_t)func);

				data32 = pci_read_dword(PCI_CFG_VID);
				if((data32 == 0xffffffff) || (data32 == 0))
				{
					if(func == 0)
					{
						break;
					}
					continue;
				}

				// reading pci configuration
				address = EFI_PCI_ADDRESS(bus, device, func, 0);
				IoDev->Pci.Read(IoDev, EfiPciWidthUint32, address, sizeof(pci_cfg) / sizeof(UINT32), &pci_cfg);
				if (memcmp(pci_cfg0, pci_cfg, sizeof(pci_cfg)) == 0)
				{
					continue;	// duplicate, skip the device
				}
				memcpy(pci_cfg0, pci_cfg, sizeof(pci_cfg));

				// TODO : PCI bridge
				d = pci_add_dev();
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

				// loading pci configuration
				memcpy(&pd->cfg, pci_cfg, sizeof(pci_cfg_t));

				// capabilities list
				pci_conf = (uint8_t *)&pd->cfg;
				cap_addr = pci_conf[0x34];	// capabilities pointer
				pcie = 0;
				while(cap_addr)
				{
					cap = pci_add_cap();
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
					
					if(cap->id == PCI_CAP_ID_PCIE)
						pcie = 1;
					
					// next capabilities
					cap_addr = pci_conf[(cap_addr + 1)];
				}

				if(pcie)
				{
					pd->ecfg_addr = (uint32_t)address;
					
					ecap_addr = 0x100;
					IoDev->Pci.Read(IoDev, EfiPciWidthUint16, EFI_PCI_ADDRESS(bus, device, func, ecap_addr), 1, &ecap_id);
					
					while(ecap_addr)
					{
						ecap = pci_add_ecap();
						if(!ecap)
							return PCI_ERR_MALLOC_EXT_CAP;

						if(pd->num_ecap == 0)
						{
							pd->ecap_lst = ecap;	// 1st ext cap
						}
						else
						{
							pecap->next = ecap;
							ecap->prev = pecap;
						}
						pecap = ecap;
						pd->num_ecap++;
						
						ecap->addr	= ecap_addr;
						ecap->id	= ecap_id;
						
						IoDev->Pci.Read(IoDev, EfiPciWidthUint16, EFI_PCI_ADDRESS(bus, device, func, ecap_addr + 2), 1, &ecap_addr);
						ecap->ver = (uint8_t)(ecap_addr & 0xF);

						ecap_addr >>= 4;
						IoDev->Pci.Read(IoDev, EfiPciWidthUint16, EFI_PCI_ADDRESS(bus, device, func, ecap_addr), 1, &ecap_id);
					}
				}

				if(func == 0 && ((pd->cfg.hdr_type & HEADER_TYPE_MULTI_FUNCTION) == 0x00))
				{
					break;
				}
			}
		}
	}

  	status = PCI_OK;

done:
	return (uint8_t)status;
}

//=============================================================================
//  pci_set_dev_info
//=============================================================================
void pci_set_dev_info(pci_dev_t *pd, pci_dev_info_t *pdi)
{
	memset(pdi, 0, sizeof(pci_dev_info_t));

	pdi->bus	= pd->bus;
	pdi->dev	= pd->dev;
	pdi->fun	= pd->fun;

	pdi->intf	= pd->cfg.classcode.class_code[0];
	pdi->subcla	= pd->cfg.classcode.class_code[1];
	pdi->cla	= pd->cfg.classcode.class_code[2];
	
	pdi->cap	= pd->cfg.cap_ptr;

	pdi->vid	= pd->cfg.ven_id;
	pdi->did	= pd->cfg.dev_id;

	pdi->mmio	= pd->ecfg_addr;

	pdi->IoDev = pd->IoDev;
}

//=============================================================================
//  pci_init
//=============================================================================
uint8_t pci_init(void)
{
	uint8_t status;
#if 0
	uint16_t	bus;
	uint8_t		dev, fun;
	uint32_t	data32;
	uint32_t	pci_cfg[64];
	uint32_t	pci_cfg0[64];
	uint8_t		cap_addr, header_type, *pci_conf;
	uint8_t		pcie;
	uint16_t	ecap_addr, ecap_id;
	uint32_t	ext_cfg_addr;

	pci_dev_t *d, *pd;
	pci_cap_t *cap, *pcap;
	pci_ecap_t	*ecap, *pecap;
	int			i;
	uint8_t		nocsm = 0;

	// csm info : get csm info before pci_init
	if (csm_init() < 0)
	{
		// return PCI_ERR_CSM_INFO;
		nocsm = 1;
	}

	if (pci)
	{
		if (pci->status == PCI_INIT_OK)
		{
			return PCI_OK;
		}
		else
		{
			pci_exit();
		}
	}

	pci = (pci_bus_t *)malloc(sizeof(pci_bus_t));
	if (!pci)
		return PCI_ERR_MALLOC_BUS;

	memset(pci, 0, sizeof(pci_bus_t));

	if (nocsm)
		pci->csmp = NULL;
	else
		pci->csmp = csm_get();
	
	pci->nocsm = nocsm;
	
	for (bus=0; bus<PCI_NUM_BUS; bus++)
	{
		// last_pci_bus in csm info is wrong in server platform
		// There are some devices @ BDF=ff:xx:yy.
		//if (bus > (uint16_t)csmp->info.last_pci_bus)
		//	break;

		for (dev=0; dev<PCI_NUM_DEV; dev++)
		{
			for (fun=0; fun<PCI_NUM_FUN; fun++)
			{
				// select device
				pci_sel_dev(bus, dev, fun);

				data32 = pci_read_dword(PCI_CFG_VID);
				if ( data32 == 0xffffffff || data32 == 0 )
				{
					if (fun == 0)
						break;	// no device on (b, d, 0), skip scanning

					continue;	// no device
				}

				// reading pci configuration
				for (i=0; i<64; i++)
				{
					pci_cfg[i] = pci_read_dword(i*sizeof(uint32_t));
				}

			
				// the same pci configuration
				if (memcmp(pci_cfg0, pci_cfg, sizeof(pci_cfg)) == 0)
				{
					continue;	// duplicate, skip the device
				}

				// save configuration of previous device
				memcpy(pci_cfg0, pci_cfg, sizeof(pci_cfg));

				// TODO : PCI bridge

				d = pci_add_dev();
				if (!d)
				{
					return PCI_ERR_MALLOC_DEV;
				}

				// device link-list
				if (pci->num_dev == 0)
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
				pd->dev	= dev;
				pd->fun = fun;

				// loading pci configuration
				memcpy(&pd->cfg, pci_cfg, sizeof(pci_cfg_t));

				// capabilities list
				pci_conf = (uint8_t *)&pd->cfg;
				cap_addr = pci_conf[0x34];	// capabilities pointer
				pcie = 0;
				while (cap_addr)
				{
					cap = pci_add_cap();
					if (!cap)
						return PCI_ERR_MALLOC_CAP;

					if (pd->num_cap == 0)
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
					
					if (cap->id == PCI_CAP_ID_PCIE)
						pcie = 1;
					
					// next capabilities
					cap_addr = pci_conf[(cap_addr + 1)];
				}

				// fill ext_cfg_addr as mmio address regardless of pcie 
				ext_cfg_addr = pd->bus;
				ext_cfg_addr <<= 5;
				ext_cfg_addr |= pd->dev;
				ext_cfg_addr <<= 3;
				ext_cfg_addr |= pd->fun;
				ext_cfg_addr <<= 12;
				if (pci->nocsm == 0)
					ext_cfg_addr |= pci->csmp->info.pcie_base;

				pd->ecfg_addr = ext_cfg_addr;
					
				if (pcie)
				{
					ecap_addr = 0x100;
					ecap_id = *(uint16_t *)(ext_cfg_addr + ecap_addr);
					
					while (ecap_addr)
					{
						ecap = pci_add_ecap();
						if (!ecap)
							return PCI_ERR_MALLOC_EXT_CAP;

						if (pd->num_ecap == 0)
						{
							pd->ecap_lst = ecap;	// 1st ext cap
						}
						else
						{
							pecap->next = ecap;
							ecap->prev = pecap;
						}
						pecap = ecap;
						pd->num_ecap++;
						
						ecap->addr	= ecap_addr;
						ecap->id	= ecap_id;
						
						ecap_addr	= *(uint16_t *)(ext_cfg_addr + ecap_addr + 2);
						ecap->ver	= (uint8_t)(ecap_addr & 0xF);

						ecap_addr	>>= 4;
						ecap_id		= *(uint16_t *)(ext_cfg_addr + ecap_addr);
					}
				}
				
				header_type = pci_conf[0x0E];	// capabilities pointer
				// pci_conf[0E] : header type,
				// bit23=1:multi-function, =0:single function
				if (fun == 0 && (header_type & 0x80) == 0)
				{
					// single-function : skip another fun
					break;
				}
			}
		}
	}
#endif
	status = EFIPCIInit();
	if(status)
	{
		return status;
	}
	pci->status = PCI_INIT_OK;	// initialized

	return PCI_OK;
}

//=============================================================================
//  pci_exit
//=============================================================================
uint8_t pci_exit(void)
{
	pci_dev_t	*dev, *d;
	pci_cap_t	*cap, *c;
	pci_ecap_t	*ecap, *e;
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

			if (dev->num_ecap)
			{
				for (j=0, ecap=dev->ecap_lst; j<dev->num_ecap; j++)
				{
					e = ecap;
					ecap = e->next;
					if (e)
						free(e);
				}
			}
				

			// free pci device list
			d = dev;
			dev = d->next;
			if (d)
				free(d);
		}
	}

	// pci bridge
#if 0
	if (pci->bdg)
	{
		for (i=0, bdg=pci->bdg; i<pci->num_bdg; i++)
		{
			b = bdg;
			bdg = b->next;
			if (b)
				free(b);
		}
	}
#endif

	pci->status = 0;	// uninitialized
	free(pci);

	pci = NULL;

	return PCI_OK;
}

//=============================================================================
//  pci_status
//=============================================================================
uint32_t pci_status(void)
{
	if (!pci)
		return 0;

	return pci->status;
}

//=============================================================================
//  pci_find_ven
//=============================================================================
pci_dev_t *pci_find_ven(uint16_t ven_id)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if (pd->cfg.ven_id == ven_id)
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_dev
//=============================================================================
pci_dev_t *pci_find_dev(uint16_t dev_id)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if (pd->cfg.dev_id == dev_id)
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_ven_dev
//=============================================================================
pci_dev_t *pci_find_ven_dev(uint16_t ven_id, uint16_t dev_id)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->cfg.ven_id == ven_id) &&
			(pd->cfg.dev_id == dev_id))
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_bus_dev_fun
//=============================================================================
pci_dev_t *pci_find_bus_dev_fun(uint8_t bus, uint8_t dev, uint8_t fun)
{
	pci_dev_t	*pd;
	int			i;


	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->bus == bus) &&
			(pd->dev == dev) &&
			(pd->fun == fun))
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_bus_dev_fun_vid_did
//=============================================================================
pci_dev_t *pci_find_bus_dev_fun_vid_did(uint8_t bus, uint8_t dev, uint8_t fun, uint16_t vid, uint16_t did)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->bus == bus) &&
			(pd->dev == dev) &&
			(pd->fun == fun) &&
			(pd->cfg.ven_id == vid) &&
			(pd->cfg.dev_id == did))
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_bus
//=============================================================================
pci_dev_t *pci_find_bus(uint8_t bus)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if (pd->bus == bus)
			return pd;
	}

	return NULL;
}

//=============================================================================
//  pci_find_cla_subcla
//=============================================================================
pci_dev_t *pci_find_cla_subcla(uint8_t cla, uint8_t subcla)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->cfg.classcode.class_code[2] == cla) &&
			(pd->cfg.classcode.class_code[1] == subcla))
		{
			return pd;
		}
	}

	return NULL;
}

//=============================================================================
//  pci_find_class
//=============================================================================
pci_dev_t *pci_find_class(uint8_t cla, uint8_t subcla, uint8_t intf)
{
	pci_dev_t	*pd;
	int			i;

	if (pci->num_dev == 0)
		return NULL;

	for (i=0, pd=pci->dev; i<pci->num_dev; i++, pd=pd->next)
	{
		if ((pd->cfg.classcode.class_code[2] == cla) &&
			(pd->cfg.classcode.class_code[1] == subcla) &&
			(pd->cfg.classcode.class_code[0] == intf))
		{
			return pd;
		}
	}

	return NULL;
}



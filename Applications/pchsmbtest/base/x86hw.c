#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util.h"
#include "x86hw.h"

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

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  SMBus                                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  smb_amd_select_port
//=============================================================================
void smb_amd_select_port(uint8_t port)
{
	uint8_t		data8;
	uint8_t		dirty;

	// AMD SMBus Selection @ PMIO Reg 0x00 bit[20:19]
	// (PMIO:0xFED80300) @ bit[20:19]
	// (PMIO:Port IDX=CD7h,DAT=CD8h) Reg[02] bit[4:3]

	// select SMBus Port 0
	outp(0xCD6, 0x02);
	data8 = inp(0xCD7);
	dirty = 0;
	
	if (port)
	{
		// Port 2 : TSI
		if ((data8 & 0x18) == 0x00)
		{
			data8 |= 0x08;		// 01=Port 2
			dirty = 1;
		}
	}
	else
	{
		// Port 0
		if ((data8 & 0x18) != 0x00)
		{
			data8 &= ~0x18;		// 00=Port 0
			dirty = 1;
		}
	}

	if (dirty)
	{
		outp(0xCD6, 0x02);
		outp(0xCD7, data8);
	}
}

//=============================================================================
//  smb_delay
//=============================================================================
void smb_delay(void)
{
	outp(0x80, 0x80);	// gBS->Stall
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

	uint32_t family = 0, model = 0, buf32[4];
	
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
						smbus->base[0]		= (uint16_t)( psmb->bar[4] & 0xFFE0 );	// 20h
						smbus->found[0]	= 1;
						smbus->vendor	= psmb->vendor_id;
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
					if ( psmb->vendor_id == 0x1002 || psmb->vendor_id == 0x1022 )
					{
						// default select port 0
						smb_amd_select_port(0);

						buf32[0] = buf32[1] = buf32[2] = buf32[3] = 0;
						AsmCpuid(1, &buf32[0], &buf32[1], &buf32[2], &buf32[3]);

						family = CPUIDTOFAMILY(buf32[0]);
						model = CPUIDTOMODEL(buf32[0]);
						
						if(family == 0x17)
						{
							//SMB0
							if((read8((uint32_t*)0xFED80D71) == 0x00) && (read8((uint32_t*)0xFED80D72) == 0x00))
							{
								smbus->base[0]	= (uint16_t)( 0xB00 );
								smbus->found[0]	= 1;
								smbus->vendor	= psmb->vendor_id;
							}

							//SMB1
							if((read8((uint32_t*)0xFED80D13) == 0x00) && (read8((uint32_t*)0xFED80D14) == 0x00))
							{
								smbus->base[1]	= (uint16_t)( 0xB20 );
								smbus->found[1]	= 1;
								smbus->vendor	= psmb->vendor_id;
							}
						}
						else
						{
							smbus->base[0]	= (uint16_t)( 0xB00 );
							smbus->found[0]	= 1;
							smbus->vendor	= psmb->vendor_id;
						}
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
						smbus->base[0]		= (uint16_t)( psmb->bar[5] & 0xFFFE );	// 24h
						smbus->found[0]	= 1;
						smbus->vendor	= psmb->vendor_id;
					}
				}

				break;

		}//switch
	}
	pci_bus_free( pbus );

	return smbus;
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

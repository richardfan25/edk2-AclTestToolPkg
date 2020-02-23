//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - PCINAME - PCI Name                                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include "pciname.h"

//=============================================================================
//  pci device name table
//=============================================================================
pci_dev_name_t pci_dev_name_table[141] =
{
	{ 0x00, 0x00, 0x00, "Non-VGA Device" },
	{ 0x00, 0x01, 0x00, "VGA-Compatible Device" },
	{ 0x01, 0x00, 0x00, "SCSI Bus Controller" },
	{ 0x01, 0x01, 0xFF, "IDE Controller" },
	{ 0x01, 0x02, 0x00, "Floppy Disk Controller" },
	{ 0x01, 0x03, 0x00, "IPI Bus Controller" },
	{ 0x01, 0x04, 0x00, "RAID Controller" },
	{ 0x01, 0x05, 0x20, "ATA Controller (Single DMA)" },
	{ 0x01, 0x05, 0x30, "ATA Controller (Chained DMA)" },
	{ 0x01, 0x06, 0x00, "SATA Controller (Vendor Specfic Intf)" },
	{ 0x01, 0x06, 0x01, "SATA Controller (AHCI 1.0)" },
	{ 0x01, 0x07, 0x00, "Serial Attached SCSI Controller (SAS)" },
	{ 0x01, 0x80, 0x00, "Mass Storage Controller" },
	{ 0x02, 0x00, 0x00, "Ethernet Controller" },
	{ 0x02, 0x01, 0x00, "Token Ring Controller" },
	{ 0x02, 0x02, 0x00, "FDDI Controller" },
	{ 0x02, 0x03, 0x00, "ATM Controller" },
	{ 0x02, 0x04, 0x00, "ISDN Controller" },
	{ 0x02, 0x05, 0x00, "WorldFip Controller" },
	{ 0x02, 0x06, 0xFF, "PICMG 2.14 Multi Computing" },
	{ 0x02, 0x80, 0x00, "Network Controller" },
	{ 0x03, 0x00, 0x00, "VGA-Compatible Controller" },
	{ 0x03, 0x00, 0x01, "8512-Compatible Controller" },
	{ 0x03, 0x01, 0x00, "XGA Controller" },
	{ 0x03, 0x02, 0x00, "3D Controller (Not VGA-Compatible)" },
	{ 0x03, 0x80, 0x00, "Display Controller" },
	{ 0x04, 0x00, 0x00, "Video Device" },
	{ 0x04, 0x01, 0x00, "Audio Device" },
	{ 0x04, 0x02, 0x00, "Computer Telephony Device" },
	{ 0x04, 0x03, 0x00, "Multimedia Device" },
	{ 0x04, 0x80, 0x00, "Multimedia Device" },
	{ 0x05, 0x00, 0x00, "RAM Controller" },
	{ 0x05, 0x01, 0x00, "Flash Controller" },
	{ 0x05, 0x80, 0x00, "Memory Controller" },
	{ 0x06, 0x00, 0x00, "Host Bridge" },
	{ 0x06, 0x01, 0x00, "ISA Bridge" },
	{ 0x06, 0x02, 0x00, "EISA Bridge" },
	{ 0x06, 0x03, 0x00, "MCA Bridge" },
	{ 0x06, 0x04, 0x00, "PCI-to-PCI Bridge" },
	{ 0x06, 0x04, 0x01, "PCI-to-PCI Bridge(Subtractive Decode)" },
	{ 0x06, 0x05, 0x00, "PCMCIA Bridge" },
	{ 0x06, 0x06, 0x00, "NuBus Bridge" },
	{ 0x06, 0x07, 0x00, "CardBus Bridge" },
	{ 0x06, 0x08, 0xFF, "RACEway Bridge" },
	{ 0x06, 0x09, 0x40, "PCI-to-PCI Bridge (Semi-Tran,Primary)" },
	{ 0x06, 0x09, 0x80, "PCI-to-PCI Bridge (Semi-Tran,Second)" },
	{ 0x06, 0x0A, 0x00, "InfiniBrand-to-PCI Host Bridge" },
	{ 0x06, 0x80, 0x00, "Bridge Device" },
	{ 0x07, 0x00, 0x00, "Generic XT-Compatible Serial Ctlr" },
	{ 0x07, 0x00, 0x01, "16450-Compatible Serial Controller" },
	{ 0x07, 0x00, 0x02, "16550-Compatible Serial Controller" },
	{ 0x07, 0x00, 0x03, "16650-Compatible Serial Controller" },
	{ 0x07, 0x00, 0x04, "16750-Compatible Serial Controller" },
	{ 0x07, 0x00, 0x05, "16850-Compatible Serial Controller" },
	{ 0x07, 0x00, 0x06, "16950-Compatible Serial Controller" },
	{ 0x07, 0x01, 0x00, "Parallel Port" },
	{ 0x07, 0x01, 0x01, "Bi-Directional Parallel Port" },
	{ 0x07, 0x01, 0x02, "ECP 1.X Compliant Parallel Port" },
	{ 0x07, 0x01, 0x03, "IEEE 1284 Controller" },
	{ 0x07, 0x01, 0xFE, "IEEE 1284 Target Device" },
	{ 0x07, 0x02, 0x00, "Multiport Serial Controller" },
	{ 0x07, 0x03, 0x00, "Generic Modem" },
	{ 0x07, 0x03, 0x01, "Hayes Compatible Modem (16450-Intf)" },
	{ 0x07, 0x03, 0x02, "Hayes Compatible Modem (16550-Intf)" },
	{ 0x07, 0x03, 0x03, "Hayes Compatible Modem (16650-Intf)" },
	{ 0x07, 0x03, 0x04, "Hayes Compatible Modem (16750-Intf)" },
	{ 0x07, 0x04, 0x00, "IEEE 488.1/2 (GPIB) Controller" },
	{ 0x07, 0x05, 0x00, "Smart Card" },
	{ 0x07, 0x80, 0x00, "Communications Device" },
	{ 0x08, 0x00, 0x00, "Generic 8259 PIC" },
	{ 0x08, 0x00, 0x01, "ISA PIC" },
	{ 0x08, 0x00, 0x02, "EISA PIC" },
	{ 0x08, 0x00, 0x10, "I/O APIC Interrupt Controller" },
	{ 0x08, 0x00, 0x20, "I/O(x) APIC Interrupt Controller" },
	{ 0x08, 0x01, 0x00, "Generic 8237 DMA Controller" },
	{ 0x08, 0x01, 0x01, "ISA DMA Controller" },
	{ 0x08, 0x01, 0x02, "EISA DMA Controller" },
	{ 0x08, 0x02, 0x00, "Generic 8254 System Timer" },
	{ 0x08, 0x02, 0x01, "ISA System Timer" },
	{ 0x08, 0x02, 0x02, "EISA System Timer" },
	{ 0x08, 0x03, 0x00, "Generic RTC Controller" },
	{ 0x08, 0x03, 0x01, "ISA RTC Controller" },
	{ 0x08, 0x04, 0x00, "Generic PCI Hot-Plug Controller" },
	{ 0x08, 0x80, 0x00, "System Peripheral" },
	{ 0x09, 0x00, 0x00, "Keyboard Controller" },
	{ 0x09, 0x01, 0x00, "Digitizer" },
	{ 0x09, 0x02, 0x00, "Mouse Controller" },
	{ 0x09, 0x03, 0x00, "Scanner Controller" },
	{ 0x09, 0x04, 0x00, "Gameport Controller (Generic)" },
	{ 0x09, 0x04, 0x10, "Gameport Contrlller (Legacy)" },
	{ 0x09, 0x80, 0x00, "Input Controller" },
	{ 0x0A, 0x00, 0x00, "Generic Docking Station" },
	{ 0x0A, 0x80, 0x00, "Docking Station" },
	{ 0x0B, 0x00, 0x00, "386 Processor" },
	{ 0x0B, 0x01, 0x00, "486 Processor" },
	{ 0x0B, 0x02, 0x00, "Pentium Processor" },
	{ 0x0B, 0x10, 0x00, "Alpha Processor" },
	{ 0x0B, 0x20, 0x00, "PowerPC Processor" },
	{ 0x0B, 0x30, 0x00, "MIPS Processor" },
	{ 0x0B, 0x40, 0x00, "Co-Processor" },
	{ 0x0C, 0x00, 0x00, "IEEE 1394 Ctler (FireWire)" },
	{ 0x0C, 0x00, 0x10, "IEEE 1394 Ctler (1394 OpenHCI Spec)" },
	{ 0x0C, 0x01, 0x00, "ACCESS.bus" },
	{ 0x0C, 0x02, 0x00, "SSA" },
	{ 0x0C, 0x03, 0x00, "USB UHCI Controller" },
	{ 0x0C, 0x03, 0x10, "USB OHCI Controller" },
	{ 0x0C, 0x03, 0x20, "USB2 EHCI Controller" },
	{ 0x0C, 0x03, 0x30, "USB3 XHCI Controller" },
	{ 0x0C, 0x03, 0x80, "Unspecified USB Controller" },
	{ 0x0C, 0xFE, 0xFF, "USB (Not Host Controller)" },
	{ 0x0C, 0x04, 0x00, "Fibre Channel" },
	{ 0x0C, 0x05, 0x00, "SMBus Controller" },
	{ 0x0C, 0x06, 0x00, "InfiniBand" },
	{ 0x0C, 0x07, 0x00, "IPMI SMIC Interface" },
	{ 0x0C, 0x07, 0x01, "IPMI Kybd Controller Style Interface" },
	{ 0x0C, 0x07, 0x02, "IPMI Block Transfer Interface" },
	{ 0x0C, 0x08, 0x00, "SERCOS Interface Standard (IEC 61491)" },
	{ 0x0C, 0x09, 0x00, "CAN Bus" },
	{ 0x0D, 0x00, 0x00, "iRDA Compatible Controller" },
	{ 0x0D, 0x01, 0x00, "Consumer IR Controller" },
	{ 0x0D, 0x10, 0x00, "RF Controller" },
	{ 0x0D, 0x11, 0x00, "Bluetooth Controller" },
	{ 0x0D, 0x12, 0x00, "Broadband Controller" },
	{ 0x0D, 0x20, 0x00, "Ethernet Controller (802.11a)" },
	{ 0x0D, 0x21, 0x00, "Ethernet Controller (802.11b)" },
	{ 0x0D, 0x80, 0x00, "Wireless Controller" },
	{ 0x0E, 0x00, 0x00, "I20 Architecture" },
	{ 0x0E, 0x00, 0x00, "Message FIFO" },
	{ 0x0F, 0x01, 0x00, "TV Controller" },
	{ 0x0F, 0x02, 0x00, "Audio Controller" },
	{ 0x0F, 0x03, 0x00, "Voice Controller" },
	{ 0x0F, 0x04, 0x00, "Data Controller" },
	{ 0x10, 0x00, 0x00, "Network and Computing En/Decryption" },
	{ 0x10, 0x10, 0x00, "Entertainment Encryption/Decryption" },
	{ 0x10, 0x80, 0x00, "Encryption/Decryption" },
	{ 0x11, 0x00, 0x00, "DPIO Modules" },
	{ 0x11, 0x01, 0x00, "Performance Counters" },
	{ 0x11, 0x10, 0x00, "Com Sync+ Time & Freq Test/Measurment" },
	{ 0x11, 0x20, 0x00, "Management Card" },
	{ 0x11, 0x80, 0x00, "Data Acq/Signal Processing Controller" },
	{ 0xFF, 0xFF, 0xFF, "Unknown Device" },
};

//{ 0x10, 0x00, 0x00, "Network and Computing Encrpytion/Decryption" },
//{ 0x11, 0x10, 0x00, "Communications Syncrhonization Plus Time and Frequency Test/Measurment" },
//{ 0x11, 0x80, 0x00, "Data Acquisition/Signal Processing Controller" },


//=============================================================================
//  pci ven name table
//=============================================================================
pci_ven_name_t pci_ven_name_table[129] =
{
	{ 0x0357, "TTTech" },
	{ 0x0675, "Dynalink" },
	{ 0x0871, "BERKOM" },
	{ 0x0E11, "Compaq" },
	{ 0x1000, "NCR" },
	{ 0x1002, "ATI" },
	{ 0x100A, "Phoenix" },
	{ 0x100B, "NS" },
	{ 0x1011, "DEC" },
	{ 0x1014, "IBM" },
	{ 0x101C, "WD" },
	{ 0x101D, "Maxim" },
	{ 0x101E, "AMI" },
	{ 0x1022, "AMD"	},
	{ 0x1025, "ALi" },
	{ 0x1028, "Dell" },
	{ 0x102A, "LSI" },
	{ 0x102B, "Matrox" },
	{ 0x1033, "NEC" },
	{ 0x1039, "Sis" },
	{ 0x103C, "HP" },
	{ 0x1043, "AsusTek" },
	{ 0x1044, "Adaptec" },
	{ 0x1045, "OPTi" },
	{ 0x104A, "STM" },
	{ 0x104C, "TI" },
	{ 0x104D, "Sony" },
	{ 0x1050, "Winbond" },
	{ 0x1054, "Hitachi" },
	{ 0x1057, "Motorola" },
	{ 0x105A, "Promise" },
	{ 0x1060, "UMC" },
	{ 0x106B, "Apple" },
	{ 0x1073, "Yamaha" },
	{ 0x1077, "QLogic" },
	{ 0x1078, "Cyrix" },
	{ 0x1093, "National Ins." },
	{ 0x1095, "Silicon Img."},
	{ 0x10A9, "SGI" },
	{ 0x10B5, "PLX" },
	{ 0x10B7, "3Com" },
	{ 0x10B9, "ULi"},
	{ 0x10C4, "Award" },
	{ 0x10C8, "Neomagic" },
	{ 0x10CD, "ASP" },
	{ 0x10D9, "MXIC" },
	{ 0x10DE, "nVidia" },
	{ 0x10EC, "Realtek" },
	{ 0x10EE, "Xilinx" },
	{ 0x1101, "Initio" },
	{ 0x1102, "Creative" },
	{ 0x1106, "VIA" },
	{ 0x110A, "Siemens" },
	{ 0x111D, "IDT" },
	{ 0x1131, "Philips" },
	{ 0x1137, "Cisco" },
	{ 0x1166, "Broadcom" },
	{ 0x1172, "Altera" },
	{ 0x1179, "Toshiba" },
	{ 0x1180, "Ricoh" },
	{ 0x1186, "D-Link" },
	{ 0x11AB, "Marvell" },
	{ 0x11AD, "Lite-On" },
	{ 0x11C1, "LSI" },
	{ 0x11D4, "Analog Devices" },
	{ 0x1204, "Lattice" },
	{ 0x1217, "O2 Micro" },
	{ 0x121A, "3Dfx" },
	{ 0x125D, "ESS" },
	{ 0x1260, "Intersil" },
	{ 0x1274, "Ensoniq" },
	{ 0x1279, "Transmeta" },
	{ 0x127A, "Rockwell" },
	{ 0x1283, "ITE" },
	{ 0x12B9, "3Com" },
	{ 0x12C3, "Holtek" },
	{ 0x12C4, "Connect" },
	{ 0x12D2, "SGS" },
	{ 0x12D8, "Pericom" },
	{ 0x1307, "Measurement" },
	{ 0x1374, "Silicon" },
	{ 0x1393, "Moxa" },
	{ 0x13A8, "Exar" },
	{ 0x13C1, "3ware" },
	{ 0x13D1, "Abocom" },
	{ 0x13F6, "C-Media" },
	{ 0x13FE, "Advantech" },
	{ 0x1402, "Meilhaus" },
	{ 0x1412, "VIA" },
	{ 0x1425, "Chelsio" },
	{ 0x144A, "Adlink" },
	{ 0x144D, "Samsung" },
	{ 0x14C0, "Compal" },
	{ 0x14D2, "Titan" },
	{ 0x14E4, "Broadcom" },
	{ 0x14F1, "Conexant" },
	{ 0x1516, "Myson" },
	{ 0x1522, "MainPine" },
	{ 0x1524, "ENE" },
	{ 0x15AD, "VMware" },
	{ 0x15BB, "Portwell" },
	{ 0x168C, "Atheros" },
	{ 0x16CD, "Advantech Innocore" },
	{ 0x16DA, "Advantech" },
	{ 0x1737, "Linksys" },
	{ 0x17AA, "Lenovo" },
	{ 0x17CB, "Qualcomm" },
	{ 0x17D3, "Areca" },
	{ 0x17F3, "RDC" },
	{ 0x180C, "IEI" },
	{ 0x1814, "Ralink" },
	{ 0x1912, "Renesas" },
	{ 0x1957, "Freescale" },
	{ 0x1959, "PA Semi" },
	{ 0x1969, "Qualcomm Atheros"},
	{ 0x197B, "JMicron" },
	{ 0x19A2, "Emulex"},
	{ 0x1A03, "ASPEED" },
	{ 0x1B4B, "Marvell" },
	{ 0x1D17, "Zhaoxin" },
	{ 0x1FC9, "Tehuti" },
	{ 0x3D3D, "3DLabs" },
	{ 0x4005, "Avence Logic"},
	{ 0x494F, "Access" },
	{ 0x5333, "S3" },
	{ 0x8086, "Intel" },
	{ 0x9004, "Adaptec" },
	{ 0x9005, "Adaptec" },	
	{ 0xFFFF, "Unknown"}
};

#define NUM_PCI_VEN	(sizeof(pci_ven_name_table)/sizeof(pci_ven_name_table[0]))

//=============================================================================
//  pci_find_ven_name
//=============================================================================
char *pci_find_ven_name(pci_dev_t *pd)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_PCI_VEN-1; i++)
	{
		if (pd->cfg.ven_id == pci_ven_name_table[i].id)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return pci_ven_name_table[i].name;
	else
		return pci_ven_name_table[NUM_PCI_VEN-1].name;	// Unknown
}

#define NUM_PCI_DEV	(sizeof(pci_dev_name_table)/sizeof(pci_dev_name_table[0]))

//=============================================================================
//  pci_find_dev_name
//=============================================================================
char *pci_find_dev_name(pci_dev_t *pd)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_PCI_DEV-1; i++)
	{
		if ((pd->cfg.classcode.class_code[2] == pci_dev_name_table[i].cla) &&
			(pd->cfg.classcode.class_code[1] == pci_dev_name_table[i].subcla) &&
			(pd->cfg.classcode.class_code[0] == pci_dev_name_table[i].intf))
		{
			found = 1;
			break;
		}
	}

	if (!found)
	{
		for (i=0; i<NUM_PCI_DEV-1; i++)
		{
			// find other device
			if (pci_dev_name_table[i].cla == pd->cfg.classcode.code.cla &&
				pci_dev_name_table[i].subcla == 0x80)
			{
				found = 1;
				break;
			}
		}
	}

	if (found)
		return pci_dev_name_table[i].name;
	else
		return pci_dev_name_table[NUM_PCI_DEV-1].name;	// Unknown
}

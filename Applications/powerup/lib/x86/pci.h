//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - PCI                                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_PCI_H
#define __X86_PCI_H

#include "typedef.h"
#include "csm.h"
#include "pci.h"

//=============================================================================
//  define
//=============================================================================
#define PCI_NUM_BUS				(256)
#define PCI_NUM_DEV				(32)
#define PCI_NUM_FUN				(8)
#define PCI_INIT_OK				0x55AA33CC

// error code
#define PCI_OK					0
#define PCI_ERR_NULL_BUS		1
#define PCI_ERR_MALLOC_BUS		2
#define PCI_ERR_MALLOC_DEV		3
#define PCI_ERR_NO_DEV			4
#define PCI_ERR_NOT_FOUND		5
#define PCI_ERR_MALLOC_CAP		6
#define PCI_ERR_MALLOC_EXT_CAP	7
#define PCI_ERR_CSM_INFO		8

//=============================================================================
//  PCI configuration space
//=============================================================================
#define PCI_CFG_VID				0x00	// (2) Vendor ID
#define PCI_CFG_DID				0x02	// (2) Device ID
#define PCI_CFG_CMD				0x04	// (2) Command
#define   PCI_CMD_IO_EN_bp		0
#define   PCI_CMD_IO_EN_bm		1
#define PCI_CFG_STS				0x06	// (2) Status
#define PCI_CFG_REV				0x08	// (1) Revision
#define PCI_CFG_CLASS			0x09	// (3) Class Code
#define PCI_CFG_CLSZ			0x0C	// (1) Cache Line Size
#define PCI_CFG_MLT				0x0D	// (1) Master Latency Timer
#define PCI_CFG_HTYPE			0x0E	// (1) Header Type
#define PCI_CFG_BIST			0x0F	// (1) BIST
#define PCI_CFG_BAR0			0x10	// (4) Base Address Register 0
#define PCI_CFG_BAR1			0x14	// (4) Base Address Register 1
#define PCI_CFG_BAR2			0x18	// (4) Base Address Register 2
#define PCI_CFG_BAR3			0x1C	// (4) Base Address Register 3
#define PCI_CFG_BAR4			0x20	// (4) Base Address Register 4
#define PCI_CFG_BAR5			0x24	// (4) Base Address Register 5
#define   PCI_BAR_MEM_IO_bp		0
#define	  PCI_BAR_MEM_IO_bm		0x1
#define	  PCI_BAR_IO_ADDR_gp	1
#define	  PCI_BAR_IO_ADDR_gm	0xFFFE
#define	  PCI_BAR_MEM_LOC_gp	1
#define   PCI_BAR_MEM_LOC_gm	0x6
#define	  PCI_BAR_MEM_PREF_bp	3
#define	  PCI_BAR_MEM_PREF_bm	0x8
#define	  PCI_BAR_MEM_ADDR_gp	4
#define	  PCI_BAR_MEM_ADDR_gm	0xFFFFFFF0

#define PCI_CFG_CBCISPTR		0x28	// (4) CardBus CIS Pointer
#define PCI_CFG_SVID			0x2C	// (2) Subsystem Vendor ID
#define PCI_CFG_SDID			0x2E	// (2) Subsystem ID
#define PCI_CFG_ROMADDR			0x30	// (4) Expansion ROM Address
#define   PCI_ROM_MEM_IO_bp		0
#define	  PCI_ROM_MEM_IO_bm		0x1
#define   PCI_ROM_ADDR_W_gp		1
#define	  PCI_ROM_ADDR_W_gm		0x6
#define	  PCI_ROM_MEM_PREF_bp	3
#define	  PCI_ROM_MEM_PREF_bm	0x8
#define	  PCI_ROM_BASE_ADDR_gp	4
#define	  PCI_ROM_BASE_ADDR_gm	0xFFFFFFF0
#define PCI_CFG_CAPPTR			0x34	// (1) Capabilities Pointer
#define PCI_CFG_RSVD			0x35	// (7) Reserved
#define PCI_CFG_INTL			0x3C	// (1) Interrupt Line
#define PCI_CFG_INTP			0x3D	// (1) Interrupt Pin
#define PCI_CFG_MINGNT			0x3E	// (1) Interrupt Line
#define PCI_CFG_MAXGNT			0x3F	// (1) Interrupt Line

//=============================================================================
//  PCI extended configuration space
//=============================================================================

//=============================================================================
//  PCI capabilities ID
//=============================================================================
#define PCI_CAP_ID_RSVD		0	// Reserved
#define PCI_CAP_ID_PM		1	// Power Management
#define PCI_CAP_ID_AGP		2	// AGP
#define PCI_CAP_ID_VPD		3	// Vital Product Data
#define PCI_CAP_ID_SLOT		4	// Slot Identification
#define PCI_CAP_ID_MSI		5	// Message Signaled Interrupts
#define PCI_CAP_ID_HS		6	// CompactPCI Hot Swap
#define PCI_CAP_ID_PCIX		7	// PCI-X
#define PCI_CAP_ID_HT		8	// HyperTransport
#define PCI_CAP_ID_VENDOR	9	// Vendor specific
#define PCI_CAP_ID_DEBUG	10	// Debug Port
#define PCI_CAP_ID_RC		11	// CompactPCI Central Resource Control
#define PCI_CAP_ID_HOTPLUG	12	// PCI Hot-Plug
#define PCI_CAP_ID_BVID		13	// PCI bridge subsystem vendor ID
#define PCI_CAP_ID_AGP8X	14	// AGP 8x
#define PCI_CAP_ID_SECURE	15	// Secure Device
#define PCI_CAP_ID_PCIE		16	// PCI Express
#define PCI_CAP_ID_MSIX		17	// MSI-X
#define PCI_CAP_ID_RSVD2	18	// 12h~FFh reserved

//=============================================================================
//  PCI capabilities (01h) : Power Management (bit)
//=============================================================================
#define PCI_CAP_PM_ID		0	// (8) Cap ID
#define PCI_CAP_PM_NXCAP	8	// (8) Next capabilities
#define PCI_CAP_PM_VER		16	// (3) Version	
#define PCI_CAP_PM_PMECLK	19	// (1) PME Clock
#define PCI_CAP_PM_RSVD		20	// (1) Reserved
#define PCI_CAP_PM_DSI		21	// (1) Device Specific Initialization
#define PCI_CAP_PM_AUXCURR	22	// (3) Aux Current
#define PCI_CAP_PM_D1		25	// (1) D1 Support
#define PCI_CAP_PM_D2		26	// (1) D2 Support
#define PCI_CAP_PM_PME		27	// (5) PME Support

//=============================================================================
//  PCI capabilities (10h) : PCI Express (byte)
//=============================================================================
#define PCI_CAP_PCIE_ID			0x00	// (1) Cap ID
#define PCI_CAP_PCIE_NXCAP		0x01	// (1) Next Capabilities Pointer

#define PCI_CAP_PCIE_CAP		0x02	// (2) PCI Express Capabilities Register
// [15:14]   RsvdP
// [14]   TCS Routing Supported ?
// [13:9] Interrupt Message Number
// [8]    Slot Implemented
//        0:must be 0 for an endpoint device
//        1:PCIe slot
// [7:4]  Device/Port Type - ....
//        0000b:PCIe endpoint
//        0001b:legacy PCIe endpoint
// [3:0]  Capabilities Version
//        fixed = 2, PCIe 2.x

#define PCI_CAP_PCIE_DEVCAP		0x04	// (4) Device Capabilities
// [31:29]  Rsvdp
// [28]     Function level reset capabilities
// [27:26]  captured slot power limit scale
// [25:18]  captured slot power limit value
// [17:16]  Rsvdp
// [15]     Role-Based Error handling
// [14:12]  undefined for PCI Express 1.1
// [11:9]   endpoint L1 acceptable latency
// [8:6]    endpoint L0s acceptable latency
// [5]      extended tag field supported
// [4:3]    phantom function supported
// [2:0]    max payload size supported

#define PCI_CAP_PCIE_DEVCTL		0x08	// (2) Device Control

#define PCI_CAP_PCIE_DEVSTS		0x0A	// (2) Device Status
// [15:6] Rsvdp
// [5]    transaction pending
// [4]    aux power detected
// [3]    unsupported requested detected
// [2]    fatal error detected
// [1]    non-fatal error detected
// [0]    correctable error detected

#define PCI_CAP_PCIE_LNKCAP		0x0C	// (4) Link Capabilities
// [31:24] port number
// [23]    rsvdp
// [22]    ASPM optional compliance
// [21]    link bandwidth notification capability
// [20]    data link layer active reporting capable
// [19]    suprise down error reporting capable
// [18]    clock power management
// [17:15] L1 exit latency
// [14:12] L0s exit latency
// [11:10] Active state link PM support
// [9:4]   Maximum Link Width <<<<<<<<<<<<<<<<<<<<< x ?
// [3:0]   Maximum Link Speed <<<<<<<<<<<<<<<<<<<<< Gen 1,2,3 (2.5, 5, 8 GT/s)


#define PCI_CAP_PCIE_LNKCTL		0x10	// (2) Link Control
// [15:12] Rsvdp
// [11]    Link autonomous bandwidth interrupt enable
// [10]    Link bandwidth management interrupt enable
// [9]     Hardware Autonomous width disable
// [8]     Enable clock power management
// [7]     Extended Sync
// [6]     Common Clock Configuration
// [5]     Retrain Link
// [4]     Link Disable
// [3]     Read Completion Boundary Control
// [2]     Rsvdp
// [1:0]   Active State PM Control
//         00=disable
//         01=L0s entry enabled
//         10=L1 entry enalbed
//         11=L0s and L1 entry enabled

#define PCI_CAP_PCIE_LNKSTS		0x12	// (2) Link Status
#define PCI_CAP_PCIE_SLOTCAP	0x14	// (4) Slot Capabilities
#define PCI_CAP_PCIE_SLOTCTL	0x18	// (2) Slot Control
#define PCI_CAP_PCIE_SLOTSTS	0x1A	// (2) Slot Status
#define PCI_CAP_PCIE_ROOTCTL	0x1C	// (2) Root Control
#define PCI_CAP_PCIE_ROOTCAP	0x1E	// (2) Root Capabilities
#define PCI_CAP_PCIE_ROOTSTS	0x20	// (4) Root Status
#define PCI_CAP_PCIE_DEVCAP2	0x24	// (4) Device Capabilities 2
#define PCI_CAP_PCIE_DEVCTL2	0x28	// (2) Device Control 2
#define PCI_CAP_PCIE_DEVSTS2	0x2A	// (2) Device Status 2
#define PCI_CAP_PCIE_LNKCAP2	0x2C	// (4) Link Capabilities 2
#define PCI_CAP_PCIE_LNKCTL2	0x30	// (2) Link Control 2
#define PCI_CAP_PCIE_LNKSTS2	0x32	// (2) Link Status 2
#define PCI_CAP_PCIE_SLOTCAP2	0x34	// (4) Slot Capabilities 2
#define PCI_CAP_PCIE_SLOTCTL2	0x38	// (2) Slot Control 2
#define PCI_CAP_PCIE_SLOTSTS2	0x3A	// (2) Slot Status 2

//=============================================================================
//  PCI Vendor ID
//=============================================================================
#define PCI_VID_TTTECH			0x0357
#define PCI_VID_DYNALINK		0x0675
#define PCI_VID_BERKOM			0x0871
#define PCI_VID_COMPAQ			0x0E11
#define PCI_VID_NCR				0x1000
#define PCI_VID_ATI				0x1002
#define PCI_VID_NS				0x100B
#define PCI_VID_AMI				0x101E
#define PCI_VID_AMD				0x1022
#define PCI_VID_DELL			0x1028
#define PCI_VID_NEC				0x1033
#define PCI_VID_HP				0x103C
#define PCI_VID_ASUS			0x1043
#define PCI_VID_TI				0x104C
#define PCI_VID_WINBOND			0x1050
#define PCI_VID_YAMAHA			0x1073
#define PCI_VID_NVIDIA			0x10DE
#define PCI_VID_REALTEK			0x10EC
#define PCI_VID_XILINX			0x10EE
#define PCI_VID_VIA				0x1106
#define PCI_VID_SIEMENS			0x110A
#define PCI_VID_PHILIPS			0x1131
#define PCI_VID_CISCO			0x1137
#define PCI_VID_TOSHIBA			0x1179
#define PCI_VID_RICOH			0x1180
#define PCI_VID_DLINK			0x1186
#define PCI_VID_MARVELL			0x11AB
#define PCI_VID_ADI				0x11D4
#define PCI_VID_ITE				0x1283
#define PCI_VID_EXAR			0x13A8
#define PCI_VID_ADVANTECH		0x13FE	// Advantech ?
#define PCI_VID_ADLINK			0x144A
#define PCI_VID_SAMSUNG			0x144D
#define PCI_VID_BROADCOM		0x14E4
#define PCI_VID_ENE				0x1524
#define PCI_VID_ATHEROS			0x168C
#define PCI_VID_INNOCORE		0x16CD
#define PCI_VID_ADVANTECH_2		0x16DA	// Advantech ?
#define PCI_VID_LINKSYS			0x1737
#define PCI_VID_LENOVO			0x17AA
#define PCI_VID_QUALCOMM		0x17CB
#define PCI_VID_RENESAS			0x1912
#define PCI_VID_QUALCOMM_ATHER	0x1969	// Qualcomm Atheros
#define PCI_VID_INTEL			0x8086
#define PCI_VID_ADAPTEC			0x9004
#define PCI_VID_UNKNOWN			0xFFFF

#pragma pack(1)
//=============================================================================
//  pci_cfg_t
//=============================================================================
typedef struct _pci_cfg_t
{
	uint16_t	ven_id;			// 00 : vendor ID
	uint16_t	dev_id;			// 02 : device ID
	uint16_t	cmd;			// 04 : command
	uint16_t	sts;			// 06 : status
	uint8_t		rev_id;			// 08 : revision ID
	union
	{
		uint8_t		class_code[3];	// 09 : class code
		struct
		{
			uint8_t	intf;		// 09 : interface
			uint8_t	subcla;		// 0A : sub-class
			uint8_t	cla;		// 0B : class
		}code;
	}classcode;
	uint8_t		cache_line_sz;	// 0C : cache line size
	uint8_t		lat_tmr;		// 0D : latency timer
	uint8_t		hdr_type;		// 0E : header type
	uint8_t		bist;			// 0F : BIST
	uint32_t	bar[6];			// 10 : Base Address Register[6]
	uint32_t	cb_cis_ptr;		// 28 : Cardbus CIS Pointer
	uint16_t	sub_ven_id;		// 2C : Subsystem Vendor ID
	uint16_t	sub_sys_id;		// 2E : Subsystem ID
	uint32_t	exp_rom_base;	// 30 : Expansion ROM base
	uint8_t		cap_ptr;		// 34 : Capabilities Pointer
	uint8_t		res[7];			// 35 : Reserved
	uint8_t		int_line;		// 3C : Interrupt Line
	uint8_t		int_pin;		// 3D : Interrupt Pin
	uint8_t		min_gnt;		// 3E : Min Gnt.
	uint8_t		max_lat;		// 3F : Max Latency

	uint8_t		reg[192];		// 40~FF : device specific registers

} pci_cfg_t;

//=============================================================================
//  pci_bdg_hdr_t
//=============================================================================
typedef struct _pci_bdg_hdr_t
{
	uint16_t	ven_id;			// 00 : vendor ID
	uint16_t	dev_id;			// 02 : device ID
	uint16_t	cmd;			// 04 : command
	uint16_t	sts;			// 06 : status
	uint8_t		rev_id;			// 08 : revision ID
	uint8_t		class_code[3];	// 09 : class code
	uint8_t		cache_line_sz;	// 0C : cache line size
	uint8_t		lat_tmr;		// 0D : latency timer
	uint8_t		hdr_type;		// 0E : header type
	uint8_t		bist;			// 0F : BIST
	uint32_t	bar[6];			// 10 : Base Address Register[6]
	uint32_t	cb_cis_ptr;		// 28 : Cardbus CIS Pointer
	uint16_t	sub_ven_id;		// 2C : Subsystem Vendor ID
	uint16_t	sub_sys_id;		// 2E : Subsystem ID
	uint16_t	io_base_msb;	// 30 : I/O Base Upper 16 Bits
	uint16_t	io_limit_msb;	// 32 : I/O Limit Upper 16 Bits
	uint8_t		cap_ptr;		// 34 : Capabilities Pointer
	uint8_t		res[3];			// 35 : Reserved
	uint32_t	exp_rom_base;	// 38 : Expansion ROM Base Address
	uint8_t		int_line;		// 3C : Interrupt Line
	uint8_t		int_pin;		// 3D : Interrupt Pin
	uint16_t	bdg_ctl;		// 3E : bridge control

} pci_bdg_hdr_t;

// PCI-to-PCI bridge
// ..
// 10  4 : BAR0
// 14  4 : BAR1
// 18  1 : Primary bus number
// 19  1 : Secondary bus number
// 1A  1 : Subordinate bus number
// 1B  1 : Secondary Latency Timer
// 1C  1 : I/O Base
// 1D  1 : I/O Limit
// 1E  2 : Secondary Status
// 20  2 : Memory Base
// 22  2 : Memory Limit
// 24  2 : Prefetchable Memory Base
// 26  2 : Prefetchable Memory Limit
// 28  4 : Prefecthable Base Upper 32 Bits
// 2C  4 : Prefecthable Limit Upper 32 Bits

//=============================================================================
//  pci_cap_pcie_t
//=============================================================================
typedef struct _pci_cap_pcie_t
{
	uint8_t		id;			// 00h (1)
	uint8_t		next_cap;	// 01h (1)
	uint16_t	pcie_cap;	// 02h (2)
	uint32_t	dev_cap;	// 04h (4)
	uint16_t	dev_ctl;	// 08h (2)
	uint16_t	dev_sts;	// 0Ah (2)
	uint32_t	lnk_cap;	// 0Ch (4)
	uint16_t	lnk_ctl;	// 10h (2)
	uint16_t	lnk_sts;	// 12h (2)
	uint32_t	slot_cap;	// 14h (4)
	uint16_t	slot_ctl;	// 18h (2)
	uint16_t	slot_sts;	// 1Ah (2)
	uint16_t	root_ctl;	// 1Ch (2)
	uint16_t	rsvdp;		// 1Eh (2)
	uint32_t	root_sts;	// 20h (4), sizeof=24h=36

} pci_cap_pcie_t;


//=============================================================================
//  pci_cap_t
//=============================================================================
typedef struct _pci_cap_t
{
	uint8_t id;
	uint8_t	addr;

	struct _pci_cap_t	*prev;
	struct _pci_cap_t	*next;	

} pci_cap_t;

//=============================================================================
//  pci_ecap_t
//=============================================================================
typedef struct _pci_ecap_t
{
	// PCIE extended configuration space
	uint16_t 	id;
	uint16_t	addr;
	uint8_t		ver;

	struct _pci_ecap_t	*prev;
	struct _pci_ecap_t	*next;	

} pci_ecap_t;

//=============================================================================
//  pci_dev_t
//=============================================================================
typedef struct _pci_dev_t
{
	uint8_t		bus;
	uint8_t		dev;
	uint8_t		fun;
	uint8_t		num_cap;
	uint8_t		num_ecap;
	uint32_t	ecfg_addr;
	
	pci_cfg_t	cfg;

	pci_cap_t	*cap_lst;
	pci_ecap_t	*ecap_lst;
	
	struct _pci_dev_t	*prev;
	struct _pci_dev_t	*next;

	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev;

} pci_dev_t;

//=============================================================================
//  pci_dev_info_t
//=============================================================================
typedef struct _pci_dev_info_t
{
	uint8_t		bus;
	uint8_t		dev;
	uint8_t		fun;

	uint8_t		intf;		// interface
	uint8_t		subcla;		// sub class code
	uint8_t		cla;		// class code

	uint8_t		cap;		// capabilities pointer
	uint8_t		rsvd;

	uint16_t	vid;		// vendor id
	uint16_t	did;		// device id

	//----------------------------------
	// bit[31]    - always 1
	// bit[30:24] - 0
	// bit[23:16] - bus
	// bit[15:11] - device
	// bit[10:8]  - function
	// bit[7:2]   - register
	// bit[1:0]   - 0
	//----------------------------------
	uint32_t	mmio;

	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev;
	EFI_HANDLE *Handle;

} pci_dev_info_t;

//=============================================================================
//  pci_bdg_t
//=============================================================================
#if 0
typedef struct _pci_bdg_t
{
	uint8_t		bus;
	uint8_t		dev;
	uint8_t		fun;
	uint8_t		rsvd;

	pci_bdg_t_cfg		cfg;
	
	struct pci_bdg_t	*prev;
	struct pci_bdg_t	*next;

} pci_bdg_t;
#endif

//=============================================================================
//  pci_bus_t
//=============================================================================
typedef struct _pci_bus_t
{
	pci_dev_t	*dev;
	int			num_dev;
	uint8_t		nocsm;
	csm_t		*csmp;	// csm info pointer

	// TODO : pci bridge
	//pci_bdg_t	*bdg;
	//int			num_bdg;

	//  3         2         1         0
	// 10987654321098765432109876543210
	//----------------------------------
	// 10000000BBBBBBBBDDDDDFFFRRRRRR00
	//         bus     dev  fun reg

	//----------------------------------
	// bit[31]    - always 1
	// bit[30:24] - 0
	// bit[23:16] - bus
	// bit[15:11] - device
	// bit[10:8]  - function
	// bit[7:2]   - register
	// bit[1:0]   - 0
	//----------------------------------
	uint32_t	iocmd;


	//----------------------------------
	// bit[31]    - always 1
	// bit[30:24] - 0
	// bit[23:16] - bus
	// bit[15:11] - device
	// bit[10:8]  - function
	// bit[7:2]   - register
	// bit[1:0]   - 0
	//----------------------------------
	uint32_t	mmio;
	
	//----------------------------------
	// bit[0]   - init status (0x55AA33CC=initialized)
	//----------------------------------
	uint32_t	status;

} pci_bus_t;
#pragma pack()
//=============================================================================
//  extern
//=============================================================================
extern pci_bus_t *pci;

//=============================================================================
//  functions
//=============================================================================
void	 pci_sel_dev(uint8_t bus, uint8_t dev, uint8_t fun);
uint32_t pci_read_dword(uint8_t reg);
uint8_t	 pci_read_byte(uint8_t reg);
void	 pci_write_dword(uint8_t reg, uint32_t data);
void	 pci_write_byte(uint8_t reg, uint8_t data);

uint32_t pci_mmio_read_dword(uint16_t reg);
uint8_t	 pci_mmio_read_byte(uint16_t reg);
void	 pci_mmio_write_dword(uint16_t reg, uint32_t data);
void	 pci_mmio_write_byte(uint16_t reg, uint8_t data);

void	 pci_set_dev_info(pci_dev_t *pd, pci_dev_info_t *pdi);

uint8_t	 pci_init(void);
uint8_t  pci_exit(void);
uint32_t pci_status(void);

pci_dev_t *pci_find_vendor(uint16_t ven_id);
pci_dev_t *pci_find_device(uint16_t dev_id);
pci_dev_t *pci_find_ven_dev(uint16_t ven_id, uint16_t dev_id);
pci_dev_t *pci_find_bus_dev_fun(uint8_t bus, uint8_t dev, uint8_t fun);
pci_dev_t *pci_find_bus_dev_fun_vid_did(uint8_t bus, uint8_t dev, uint8_t fun, uint16_t vid, uint16_t did);
pci_dev_t *pci_find_bus(uint8_t bus);
pci_dev_t *pci_find_cla_subcla(uint8_t cla, uint8_t subcla);
pci_dev_t *pci_find_class(uint8_t cla, uint8_t subcla, uint8_t intf);
//uint8_t pci_comp_cfg(pci_dev_t *p, pci_dev_t *q);

#endif

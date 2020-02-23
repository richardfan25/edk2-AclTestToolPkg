//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - IMC (Integrated Memory Controller)                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//	tristate "Intel iMC (LGA 2011) SMBus Controller"
//	depends on PCI && X86
//	help
//	  If you say yes to this option, support will be included for the Intel
//	  Integrated Memory Controller SMBus host controller interface.  This
//	  controller is found on LGA 2011 Sandy Bridge Xeons and Core i7
//	  Extremes.
//
//	  There are currently no systems on which the kernel knows that it can
//	  safely enable this driver.  For now, you need to pass this driver a
//	  scary module parameter, and you should only pass that parameter if you
//	  have a special motherboard and know exactly what you are doing.
//	  Special motherboards include the Supermicro X9DRH-iF-NV.
//
//	  This driver can also be built as a module.  If so, the module will be
//	  called i2c-imc.
//*****************************************************************************

#define DEBUG_X86_IMC	0

//=============================================================================
//  Intel Xeon Processor E5-1600/2400/2600/4600 (E5-Product Family)
//  - Sandy Bridge
//=============================================================================
//   ff:0f.0     : Memory Controller Target Address Decoder Registers (00h~FCh)

// DID
// 3C00h~3C1Fh : PCI Express and DMI ports
// 3C20h~3C3Fh : IO Features (QDDMA, APIC, Intel VT, RAS, Intel TXT)
// 3C40h~3C5Fh : Performance Monitor
// 3C60h~3C7Fh : DFX
// 3C80h~3C9Fh : Intel QuickPath Interconnect
//     ff:08.0 : Port 0
//     ff:09.0 : Port 1
// 3CA0h~3CBFh : Home Agent/Memory Controller
//   ff:0f.0     : Memory Controller Target Address Decoder Registers (00h~FCh)
//   ff:0f.0     : Memory Controller MemHot and SMBus Registers (100h~1FCh)
//   ff:0f.1     : Memory Controller RAS Registers (00h~FCh)
//   ff:0f.[2-5] : Memory Controller DIMM Timing and Interleave Registers (00h~FCh)
//   ff:0f.[2-5] : Memory Controller Channel Rank Registers (100h~1FCh)
// 3CC0h~3CDFh : Power Management
// 3CE0h~3CFFh : Cbo/Ring
//   ff:0c.[0-3] : Unicast CSR's 
//   ff:0d.[0-3] : Unicast CSR's
//   ff:0c.6     : System Address Decoder (CBo)
//   ff:0c.7     : Caching agent broadcast register (CBo)
//   ff:0d.7     : Caching agent broadcast register (CBo)


//=============================================================================
//  Intel Xeon Processor D-1500 Product Family
//  - Broadwell DE
//=============================================================================
// ff.13.0 : 6FA8h : IMC Main

// ff.13.1 : 6F71h : IMC RAS Register
// ff.13.2 : 6FAAh : IMC channel 0 Target Address Decode Registers
// ff.13.3 : 6FAAh : IMC channel 1 Target Address Decode Registers
// ff.14.0 : 6FB0h : IMC channel 0 Registers
// ff.14.1 : 6FB1h : IMC channel 1 Registers
// ff.14.2 : 6FB2h : IMC channel 0 Registers
// ff.14.3 : 6FB3h : IMC channel 1 Registers

#ifndef __X86_IMC_H
#define __X86_IMC_H

//=============================================================================
//  IMC Registers
//=============================================================================

// offset	len		rename		name
//----------------------------------------------------------------
// 180h		4		SMBSTS0		SMB_STAT_0
// 184h		4		SMBCMD0		SMBCMD_0
// 188h		4		SMBCTL0		SMBCntl_0
// 18Ch		4		SMBTCNT0	SMB_TSOD_POLL_RATE_CNTR_0
// 190h		4		SMBSTS1		SMB_STAT_1
// 194h		4		SMBCMD1		SMBCMD_1
// 198h		4		SMBCTL1		SMBCntl_1
// 19Ch		4		SMBTCNT1	SMB_TSOD_POLL_RATE_CNTR_1
// 1A0h		4		SMBPCFG		SMB_PERIOD_CFG
// 1A4h		4		SMBPCNT		SMB_PERIOD_CNTR
// 1A8h		4		SMBTCFG		SMB_TSOD_POLL_RATE

// iMC register offsets (in PCI configuration space)
// c : channel, there are two channels (0 and 1)

//=============================================================================
//  180h : SMBSTS
//=============================================================================
#define IMC_REG_SMBSTS(c)			(0x180 + 0x10*(c))
#define IMC_REG_SMBSTS0				0x180
#define IMC_REG_SMBSTS1				0x190

#define   IMC_SMBSTS_RDO			(1<<31)		// [31] Read Data Valid
#define   IMC_SMBSTS_WOD			(1<<30)		// [30] Write Operation Done
#define   IMC_SMBSTS_SBE			(1<<29)		// [29] SMBus Error
#define   IMC_SMBSTS_BUSY			(1<<28)		// [28] SMBus Busy State

// DID=3CA8h (Sandy Bridge)
// [27] Reserved
// [26:24] Last Issued TSOD Slave Address

// DID=6FA8h (Broadwell DE)
// [27:24] Last Issued TSOD Slave Address
#define   IMC_SMBSTS_LAST_TSOD		(0x0F<<24)

// [23:16] Reserved
#define   IMC_SMBSTS_RDATA_MASK		0xFFFF		// [15:0] Read Data Mask

//=============================================================================
//  184h : SMBCMD
//=============================================================================
#define IMC_REG_SMBCMD(c)			(0x184 + 0x10*(c))
#define IMC_REG_SMBCMD0				0x184
#define IMC_REG_SMBCMD1				0x194

#define   IMC_SMBCMD_TRIGGER		(1<<31)		// [31] Command Triggered
#define   IMC_SMBCMD_PTR_SEL		(1<<30)		// [30] Pointer Selection
#define   IMC_SMBCMD_WORD_ACCESS	(1<<29)		// [29] Word Access

// [28:27]
// 00 = SMBus Read
// 01 = SMBus Write
// 10 = illegal combination
// 11 = write pointer register SMBus/I2C pointer update
#define   IMC_SMBCMD_WRT_PTR		(1<<28)		// [28] 
#define   IMC_SMBCMD_WRT_CMD		(1<<27)		// [27] 0=read command, 1=write command

#define   IMC_SMBCMD_SA_MASK		(0x7<<24)	// [26:24] Slave Address
#define   IMC_SMBCMD_SA_SHFT		(24)

#define   IMC_SMBCMD_BA_MASK		(0xFF<<16)	// [23:16] Bus Txn Address
#define   IMC_SMBCMD_BA_SHFT		(16)

#define   IMC_SMBCMD_WDATA_MASK		0xFFFF		// [15:0] Write Data Mask

#define   IMC_SMBCMD_OUR_BITS		(~(uint32_t)IMC_SMBCMD_TRIGGER)

//=============================================================================
//  188h : SMBCTL
//=============================================================================
#define IMC_REG_SMBCTL(c)			(0x188 + 0x10*(c))
#define IMC_REG_SMBCTL0				0x188
#define IMC_REG_SMBCTL1				0x198

// [31:28] Device Type Identifier
//  0011=TSOD
//  1010=EEPROM
//  0110=Write Protect Operation
#define   IMC_SMBCTL_DTI_MASK		(0xF<<28)
#define   IMC_SMBCTL_DTI_SHFT		(28)

#define   IMC_SMBCTL_CKOVRD			(1<<27)		// [27] Clock Override

#define   IMC_SMBCTL_DIS_WRT		(1<<26)		// [26] Disable Write (sadly)

// [25:24] Reserved
// [23:21] ?
// [20:11] Reserved

#define   IMC_SMBCTL_SOFT_RST		(1<<10)		// [10] Soft Reset

// DID=3CA8h (Sandy Bridge)
// [9] Reserved

// DID=6FA8h (Broadwell DE)
// [9] Start TSOD polling
#define   IMC_SMBCTL_TSOD_POLL_ST	(1<<9)

#define   IMC_SMBCTL_TSOD_POLL_EN	(1<<8)		// [8] TSOD polling enable

#define   IMC_SMBCTL_TSOD_PRE		0xFF		// [7:0] TSOD_PRESENT for the lower and upper channels

#define   IMC_SMBCTL_OUR_BITS		(IMC_SMBCTL_DTI_MASK | IMC_SMBCTL_TSOD_POLL_EN)

//=============================================================================
//  18Ch : SMBTCNT
//=============================================================================
#define IMC_REG_SMBTCNT(c)			(0x18C + 0x10*(c))
#define IMC_REG_SMBTCNT0			0x18C
#define IMC_REG_SMBTCNT1			0x19C

// [31:18] Reserved

#define   IMC_SMBTCNT				0x3FFFF		// [17:0]  TSOD poll rate counter


//=============================================================================
//  1A0h : SMBPCFG (SMB_PERIOD_CFG)
//=============================================================================
#define IMC_REG_SMBPCFG				0x1A0

// DID=3CA8h (Sandy Bridge)
// [31:16] = ?

// DID=6FA8h (Broadwell DE)
// [31:16] = Reserved = 0x445C

// DCLK=400MHz, SMB_CLK=100kHz => DCLK/SMB_CLK=4000M/100k = 4000
#define   IMC_SMBPCFG_CLK			0xFFFF	// [15:0]=0x0FA0=4000

//=============================================================================
//  1A4h : SMBPCNT (SMB_PERIOD_CNTR)
//=============================================================================
#define IMC_REG_SMBPCNT				0x1A4

// DID=3CA8h (Sandy Bridge)
// [31:16] = SMB1_CLK_PRD_CNTR
// [15:0]  = SMB0_CLK_PRD_CNTR
#define   IMC_SMBPCNT_SMB1			(0xFFFF<<16)
#define   IMC_SMBPCNT_SMB0			(0xFFFF)

// DID=6FA8h (Broadwell DE)
// [31:16] = Reserved
// [15:0]  = SMB0_CLK_PRD_CNTR

//=============================================================================
//  1A8h : SMBTCFG (SMB_TSOD_POLL_RATE)
//=============================================================================
#define IMC_REG_SMBTCFG				0x1A8

// [31:18] = Reserved
// [17:0]  = SMB_TSOD_POLL_RATE = 0x3E800 = 256000
#define   IMC_SMBTCFG_RATE			0x3FFFF


//=============================================================================
//  IMC error code
//=============================================================================
#define	IMC_OK					0x00
#define IMC_ERR					0x01
#define IMC_ERR_BUSY			0x02
#define IMC_ERR_TMOUT			0x03
#define IMC_ERR_IO				0x04
#define IMC_ERR_BUS				0x05
#define IMC_ERR_READ			0x06
#define IMC_ERR_WRITE			0x07
#define IMC_ERR_PCI				0x08
#define IMC_ERR_PCI_DEV			0x09
#define IMC_ERR_MALLOC			0x0A
#define IMC_ERR_SPD_NOT_FOUND	0x0B
#define IMC_ERR_TSOD_NOT_FOUND	0x0C

//=============================================================================
//  IMC operations
//=============================================================================
#define IMC_SMBUS_WRITE		(0)
#define IMC_SMBUS_READ		(1)

#define IMC_SMBUS_BYTE		(1)
#define IMC_SMBUS_WORD		(2)

//=============================================================================
//  IMC command code
//=============================================================================
#define IMC_SMBUS_CMD_QUICK		(0)
#define IMC_SMBUS_CMD_BYTE		(1)
#define IMC_SMBUS_CMD_BYTE_DATA	(2)
#define IMC_SMBUS_CMD_WORD_DATA	(3)
#define IMC_SMBUS_CMD_PROC_CALL	(4)
#define IMC_SMBUS_CMD_BLOCK		(5)
#define IMC_SMBUS_CMD_I2C_READ	(6)
#define IMC_SMBUS_CMD_RSVD		(7)

//=============================================================================
//  imc_dev_t
//=============================================================================
typedef struct _imc_dev_t
{
	uint8_t		ch;
	uint8_t		addr;

} imc_dev_t;

//=============================================================================
//  imc_ch_t
//=============================================================================
typedef struct _imc_ch_t
{
	uint8_t		can_off_tsod;
	uint8_t		can_write;

} imc_ch_t;
	
//=============================================================================
//  imc_info_t
//=============================================================================
typedef struct _imc_info_t
{
	pci_dev_info_t	pci;
	imc_ch_t		ch[2];
	uint8_t			ch_no;		// current channel

	imc_dev_t		spd[16];
	uint8_t			spd_num;

	imc_dev_t		tsod[16];
	uint8_t			tsod_num;

	uint8_t			sts;	// status
	uint8_t			prev_tsod_poll;

} imc_info_t;

//=============================================================================
//  extern
//=============================================================================
extern imc_info_t	*imc;

//=============================================================================
//  functions
//=============================================================================
void		imc_sel_ch(uint8_t ch);
uint16_t	imc_read_spd(uint8_t addr, uint8_t *buf);
uint16_t	imc_read_tsod(uint8_t addr, uint16_t *buf);

uint8_t		imc_read_byte(uint8_t addr, uint8_t cmd);
uint8_t		imc_write_byte(uint8_t addr, uint8_t cmd, uint8_t data);
uint16_t	imc_read_word(uint8_t addr, uint8_t cmd);
uint8_t		imc_write_word(uint8_t addr, uint8_t cmd, uint16_t data);

void		imc_set_spd_page(uint8_t page);
void		imc_probe_spd(void);
void		imc_probe_tsod(void);
uint8_t		imc_scan_spd(uint8_t ch, uint8_t addr);
uint8_t		imc_scan_tsod(uint8_t ch, uint8_t addr);

uint8_t		imc_init(void);
void		imc_exit(void);

#endif

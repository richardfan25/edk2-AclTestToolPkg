//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SPD (Serial Presence Detect)                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_SPD_H
#define __X86_SPD_H


//=============================================================================
//  define
//=============================================================================
// slave address :
//   SPD : one of A0,A2,A4,A6,A8,AA,AC and AE
//   TS  : one of 30,32,34,36,38,3A,3C and 3E depends on SPD Ax -> 3x (DDR4)
//   WP  : 6x

//=============================================================================
//  DDR4 SPD : JEDEC Standard No.21-C page 4.1.2.12-1
//=============================================================================
//  Block 0:    0~127 0x000~0x07F  Base Configuration and DRAM Parameters
//  Block 1:  128~255 0x080~0x0FF  Module Specific Parameters
//  Block 2L: 256~319 0x100~0x13F  Reserved -- must be coded as 0x00
//  Block 2:  320~383 0x140~0x17F  Manufacturing Information
//  Block 3:  384~511 0x180~0x1FF  End User Programmable

//=============================================================================
//  DDR4 SPD Device Select Code : JEDEC Standard No.21-C page 4.1.6-6
//=============================================================================
//  3xh : Thermal Sensor
//  Axh : EEPROM (SPD)
//  62h : SWP0 : Set Write Protection, block 0
//  68h : SWP1
//  6Ah : SWP2
//  60h : SWP3
//  66h : CWP  : Clear Write Protect
//  63h : RPS0 : Read Protection Status, block 0
//  69h : RPS1
//  6Bh : RPS2
//  61h : RPS3
//  6Ch : SPA0 : Set EE Page Address 0
//  6Eh : SPA1 : Set EE Page Address 1
//  6Dh : RPA  : Read EE Page Address

//=============================================================================
//  Layout
//=============================================================================
// addr  len   desc
// 00    1     [SDR] number of bytes used by module manufacturer (bytes) (e.g. 92h)
//             [DDR3] bit7 : 0=crc size 126, 1=crc size 117
//                    bit[6:4]=SPD byte total (undef/256)
//                    bit[3:0]=SPD byte used (undef/128/176/256) (e.g. 92h)
//             [DDR4] bit7    =reserved
//                    bit[6:4]=SPD byte total (undef/256/512)
//                    bit[3:0]=SPD byte used (undef/128/256/384/512)

// 01    1     [SDR]  total SPD memory size (00h~0Dh)
//             [DDR3] bit[7:4]=SPD major ver
//			          bit[3:0]=SPD minor ver

// 02    1     [SDR]  memory type
//             [DDR2] basic memory type = 0x08
//             [DDR3] basic memory type = 0x0B
//             [DDR4] basic memory type = 0x0C
#define SPD_RAM_TYPE_RSVD		0x00	// Reserved
#define SPD_RAM_TYPE_FP			0x01	// Fast Page Mode
#define SPD_RAM_TYPE_EDO		0x02	// EDO
#define SPD_RAM_TYPE_PLNB		0x03	// Pipelined Nibble
#define SPD_RAM_TYPE_SDR		0x04	// SDRAM
#define SPD_RAM_TYPE_ROM		0x05	// ROM
#define SPD_RAM_TYPE_SGR		0x06	// DDR SGRAM
#define SPD_RAM_TYPE_DDR		0x07	// DDR SDRAM
#define SPD_RAM_TYPE_DDR2		0x08	// DDR2 SDRAM
#define SPD_RAM_TYPE_DDR2FB		0x09	// DDR2 SDRAM FB-DIMM
#define SPD_RAM_TYPE_DDR2FBP	0x0A	// DDR2 SDRAM FB-DIMM Probe
#define SPD_RAM_TYPE_DDR3		0x0B	// DDR3 SDRAM
#define SPD_RAM_TYPE_DDR4		0x0C	// DDR4 SDRAM
#define SPD_RAM_TYPE_UNKNOWN	0xFF

// 03    1     [SDR]  number of raw address bits
//             [DDR3] bit[3:0]=module type
//             [DDR4] bit[3:0]=module type, bit[7:4]=0000
#define SPD_MOD_TYPE_UNDEF		0x00
#define SPD_MOD_TYPE_RDIMM		0x01
#define SPD_MOD_TYPE_UDIMM		0x02
#define SPD_MOD_TYPE_SODIMM		0x03
#define SPD_MOD_TYPE_MICRODIMM	0x04
#define SPD_MOD_TYPE_MNRDIMM	0x05
#define SPD_MOD_TYPE_MNUDIMM	0x06
#define SPD_MOD_TYPE_MNCDIMM	0x07
#define SPD_MOD_TYPE_72SORDIMM	0x08
#define SPD_MOD_TYPE_72SOUDIMM	0x09
#define SPD_MOD_TYPE_72SOCDIMM	0x0A
#define SPD_MOD_TYPE_LRDIMM		0x0B
#define SPD_MOD_TYPE_16SODIMM	0x0C
#define SPD_MOD_TYPE_32SODIMM	0x0D
#define SPD_MOD_TYPE_ABSENT		0x0F	// 
//#define SPD_MOD_TYPE_LRDIMM		0x0B	// ?
#define SPD_MOD_TYPE_UNKNOWN	0x0E

// 04 1 [DDR4] SDRAM Density and Banks
//       bit[7:6]=bank group bits   (0=no bank,1=2 bank,2=4 bank, 3=rsvd)
//       bit[5:4]=bank address bits (0=2, 4 banks, 1=3, 8 banks)
//       bit[3:0]=total sdram capacity in Mb module type,
//         (0=256Mb, 1=512Mb, 2=1Gb, 3=2Gb, 4=4Gb, 5=8Gb, 6=16Gb, 7=32Gb)

//      [DDR3] DRAM Density and Banks
//       01h=512Mb, 02h=1Gb, 03h=2Gb, 04h=4Gb

// 05 1 [DDR4] SDRAM Addressing
//       bit[7:6]=reserved
//       bit[5:3]=row address bits    (0=12,1=13,2=14,3=15,4=16,5=17,6=18, 7=rsvd)
//       bit[2:0]=column address bits (0=9,1=10,2=11,3=12, 4~7=rsvd)

//      [DDR3] SDRAM Addressing
//       same with DDR4

// 06 1 [DDR3]
//       bit[2] = DDR3U - 1.25 volt : 0=DDR3, 1=DDR3U(1.25V)
//       bit[1] = DDR3L - 1.35 volt : 0=DDR3, 1=DDR3L(1.35V)
//       bit[0] = DDR3  - 1.5  volt : 0=DDR3 (1.5V), 1= 1.5V not support 

// 07 1 [DDR3] Module Organization
//

// 08 1 [DDR3] Module Memory Bus Width
//       01h=16-bit, 04h=32-bit, 03h=64-bit, 0Bh=64-bit ECC

// 09 1 [DDR3] Fine timebase dividend/divisor

// 0A 1 [DDR3] Medium timebase dividend

// 0B 1 [DDR3] Medium timebase divisor
//      [DDR2] DIMM configuration type (00h=nonECC, 02h=ECC, 06h=A/C Parity with ECC)

// 0C 1 [DDR4] Module Organization
//       bit[7:6]=reserved
//       bit[5:3]=num of package rank per dimm (0=1, 1=2, 2=3, 3=4 package ranks)
//       bit[2:0]=SDRAM device width (0=4,1=8,2=16,3=32 bits)

//      [DDR3] Min. SDRAM Cycle Time (tCK min)
//       0Ah=DDR3-800, 0Ch=DDR3-667, 0Fh=DDR3-533, 14h=DDR3-400

// 0D 1 [DDR4] Module Memory Bus Width
//       01h=16-bit, 02h=32-bit, 03h=64-bit, 0Bh=64-bit+ECC
//      [DDR3] Reserved

// 0E 1 [DDR4] Module Thermal Sensor(TS)
//       bit[7]  =TS, (0=without TS, 1=with TS)
//       bit[6:0]=rsvd

// 75 1 [DDR3] LSB of Manufacturer ID
//      [DDR4] CAS-CAS delay ?

// 76 1 [DDR3] MSB of Manufacturer ID

// 7E 1 [DDR3] LSB of CRC (CRC-16)
//      [DDR4] LSB of CRC (CRC-16)

// 7F 1 [DDR3] MSB of CRC (CRC-16)
//      [DDR4] MSB of CRC (CRC-16), crc size always 126

// 80 18 [DDR3] Module Part Number

// 140 1 [DDR4] LSB of Manufacturer ID
//        bit7     = odd parity for 140h
//        bit[6:0] = bank no of manufacturer
// 141 1 [DDR4] MSB of Manufacturer ID
//        bit[7:0] = manufacturer ID of bank

// 141:140 = RAM module manufacturer ID


// reference
// http://www.idhw.com/textual/chip/jedec_spd_man.html

#define SPD_MANU_ID_MICRON			0x2C00
#define SPD_MANU_ID_HYNIX			0xAD00
#define SPD_MANU_ID_SAMSUNG			0xCE00
#define SPD_MANU_ID_TRANSCEND		0x4F01
#define SPD_MANU_ID_APACER			0x7A01
#define SPD_MANU_ID_FOXCONN			0x7C01
#define SPD_MANU_ID_SMART_MODULAR	0x9401
#define SPD_MANU_ID_KINGSTON		0x9801
#define SPD_MANU_ID_CORSAIR			0x9E02
#define SPD_MANU_ID_ELPIDA			0xFE02
#define SPD_MANU_ID_NANYA			0x0B03
#define SPD_MANU_ID_KINGMAX			0x2503
#define SPD_MANU_ID_RAMAXEL			0x4304
#define SPD_MANU_ID_ADATA			0xCB04
#define SPD_MANU_ID_TEAM_GROUP		0xEF04
#define SPD_MANU_ID_PQI				0x3E05
#define SPD_MANU_ID_INNODISK		0xF106
#define SPD_MANU_ID_UNIFOSA			0x0707
#define SPD_MANU_ID_DSL				0xF708
#define SPD_MANU_ID_TOSHIBA			0x987F
#define SPD_MANU_ID_UNKNOWN			0xFFFF

// 142 1 [DDR4] Manufacturer Location

// 143 1 [DDR4] Manufacture Date - year
// 144 1 [DDR4] Manufacture Date - week of year
// 145 4 [DDR4] Module Serial Number
// 149 20 [DDR4] Module Part Number

// 15D 1  [DDR4] Module Revision Code

// 15E 1  [DDR4] LSB of Manufacturer ID (DRAM chip)
// 15F 1  [DDR4] MSB of Manufacturer ID (DRAM chip)

// 15F:15E = RAM chip manufacturer ID

// 161 29 [DDR4] Manufacturer Specific Data

// 17E 1  [DDR4] LSB of CRC
// 17F 1  [DDR4] MSB of CRC



//=============================================================================
//  DDR2 DIMM type
//=============================================================================
#define SPD_DDR2_MOD_UNDEF			0x00
#define SPD_DDR2_MOD_RDIMM			0x01
#define SPD_DDR2_MOD_UDIMM			0x02
#define SPD_DDR2_MOD_SODIM			0x04
#define SPD_DDR2_MOD_MDIMM			0x08
#define SPD_DDR2_MOD_MINI_RDIMM		0x10
#define SPD_DDR2_MOD_MINI_UDIMM		0x20
#define SPD_DDR2_MOD_UNKNOWN		0xFF

//=============================================================================
//  SPD EEPROM Size
//=============================================================================
//  Name         Len
// ---------------------------------------------
//  SDR  SDRAM   128
//  DDR  SDRAM   128
//  DDR2 SDRAM   128
//  DDR3 SDRAM   256  0~175:SPD, 176~:XMP
//  DDR4 SDRAM   512
//=============================================================================

#pragma pack(1)

#if 0
//=============================================================================
//  spd_hdr_t
//=============================================================================
typedef struct _spd_hdr_t
{
	uint8_t		num_bytes;			// [00]
	uint8_t		sz_ee_log2;			// [01]
	uint8_t		mem_type;			// [02]
	uint8_t		bank_row;			// [03]=[7:4]-bank2, [3:0]-bank1
	uint8_t		bank_col;			// [04]=[7:4]-bank2, [3:0]-bank1
	uint8_t		num_banks;			// [05]
	uint16_t	mod_data_width;		// [06]~[07]

	uint8_t		volt_level;			// [08]
	uint8_t		cas_lat;			// [09]=[7:4]-nsec, [3:0]-tenth-nsec
	uint8_t		acc_time;			// [0A]=[7:4]-nsec, [3:0]-tenth-nsec
	uint8_t		dimm_cfg;			// [0B]
	uint8_t		refresh;			// [0C]= [7]-  [6:0]
	uint8_t		pri_width;			// [0D]
	uint8_t		ecc_sdram_width		// [0E]
	uint8_t		clock_delay;		// [0F]

	uint8_t		burst_len;			// [10]
	uint8_t		banks_per_sdram;	// [11]
	uint8_t		cas_lat_sup;		// [12]
	uint8_t		cs_lat_sup;			// [13]	
	uint8_t		we_lat_sup;			// [14]
	uint8_t		mod_feat_bmp;		// [15]
	uint8_t		chip_feat_bmp;		// [16]
	uint8_t		cas_lat_mid;		// [17]

	uint8_t		dat_acc_tm;			// [18]
	uint8_t		cas_lat_sht;		// [19]
	uint8_t		dat_acc_tm_ns;		// [1A]
	uint8_t		t_rp;				// [1B]
	uint8_t		t_rrd;				// [1C]
	uint8_t		t_rcd;				// [1D]
	uint8_t		t_ras;				// [1E]
	uint8_t		mod_bank_den;		// [1F]

	uint8_t		ac_setup_tm;		// [20]
	uint8_t		ac_hold_tm;			// [21]
	uint8_t		di_setup_tm;		// [22]
	uint8_t		di_hold_tm;			// [23]
	uint8_t		rsvd[26];			// [24]~[3D]
	
	uint8_t		rev_level;			// [3E]
	uint8_t		checksum;			// [3F]

} spd_hdr_t;

//=============================================================================
//  spd_mod_t
//=============================================================================
typedef struct _spd_mod_t
{
	uint8_t		manu_jedec_id;		// [40]~[47]
	uint8_t		location;			// [48]
	uint8_t		part_num[18]		// [49]~[5A]
	uint16_t	rev_code;			// [5B]
	uint8_t		years;				// [5D]
	uint8_t		weeks;				// [5E]
	uint32_t	sn;					// [5F]~[62] module serial number
	uint8_t		manu_data[27];		// [63]~[7F]
	
} spd_mod_t;

//=============================================================================
//  spd_hdr_t
//=============================================================================
typedef struct _spd_hdr_t
{
	// 
	spd_hdr_t	hdr;
	spd_mod_t	mod;
	
} spd_hdr_t;

//=============================================================================
//  spd_ddr2_t
//=============================================================================
typedef struct _spd_ddr2_t
{
	spd_hdr_t	hdr;
	spd_mod_t	mod;

} spd_ddr2_t;

//=============================================================================
//  spd_ddr3_t
//=============================================================================
typedef struct _spd_ddr3_t
{
	// 176 : [00]~[AF] : hdr
	
	//  80 : [B0]~[FF] : xmp
	//    10:[B0]~[B9] : XMP header
	//    33:[BA]~[DB] : XMP profile 1
	//    36:[DC]~[FE] : XMP profile 2

	// [00]  1 [7]-crc exclude
	// [01]  1 SPD version, [7:4]major version, [3:0]minor version
	// [02]  1 RAM type (DDR3-SDRAM)
	// [03]  1 [3:0]=RAM module type (2=unbuffered DIMM, 3=SO-DIMM,11=LR-DIMM)

	// TODO
	// [04]    module size 
	// [07]
	// [08]
	
	// [0C]  1 RAM speed (14h=800,0Fh=1066,0Ch=1333,0Ah=1600 MHz)

	// [76]  1 manufacturer id (vendor id)

	// [78]  2 manufacturer time (78:years, 79:weeks)
	// [7A]  4 module serial number
	// [7E]  2 crc16
	// [80] 18 module part number
	// [92]

	uint8_t	data[256];
	

} spd_ddr3_t;

//=============================================================================
//  spd_ddr4_t
//=============================================================================
typedef struct _spd_ddr4_t
{
	// 176 : [00]~[AF] : hdr
	
	//  80 : [B0]~[FF] : xmp
	//    10:[B0]~[B9] : XMP header
	//    33:[BA]~[DB] : XMP profile 1
	//    36:[DC]~[FE] : XMP profile 2

	uint8_t	data[512];

} spd_ddr4_t;

#endif

//=============================================================================
//  spd_name_t
//=============================================================================
typedef struct _spd_name_t
{
	uint16_t	id;
	char		*name;

} spd_name_t;

//=============================================================================
//  spd_info_t
//=============================================================================
typedef struct _spd_info_t
{
	uint8_t		addr;		// address
	uint8_t		ctype;		// [2] chip type
	uint8_t		mtype;		// [3] module type ?
	uint8_t		flag;		// [8] ddr4=ts, ddr3=low volt type
	uint8_t		ecc;		// [10]
	uint8_t		cytime;		// [9] cycle time
	uint32_t	ee_sz;		// eeprom size
	uint32_t	ee_used;	// eeprom used
	uint16_t	mid;
	uint16_t	mbsz;		// module size in MB
	char		model[24];

} spd_info_t;

#pragma pack()
//=============================================================================
//  extern
//=============================================================================
extern spd_name_t	spd_ram_type_table[13];
extern spd_name_t	spd_manu_id_table[21];
extern spd_name_t	spd_ddr2_ecc_table[3];
extern spd_name_t	spd_ddr3_ecc_table[2];
extern spd_name_t	spd_ddr4_ecc_table[2];

//=============================================================================
//  functions
//=============================================================================
char		*spd_find_ram_type(uint8_t ram_type);
char		*spd_find_manu_id(uint16_t mid);
char		*spd_find_module_type(uint8_t mtype);

char		*spd_find_ddr2_module_type(uint8_t mtype);
char		*spd_find_ddr2_cycle_time(uint8_t cytime);
char		*spd_find_ddr3_cycle_time(uint8_t cytime);
char		*spd_find_ddr3_volt(uint8_t volt);
char		*spd_find_ddr4_cycle_time(uint8_t cytime);

char		*spd_find_ddr2_ecc(uint8_t ecc);
char		*spd_find_ddr3_ecc(uint8_t ecc);
char		*spd_find_ddr4_ecc(uint8_t ecc);

uint16_t	spd_ram_module_size(uint8_t *spd);
void		spd_get_info(spd_info_t *info, uint8_t *raw);
uint16_t	spd_get_crc(uint8_t *spd, uint8_t *len);
uint16_t	spd_get_crc16(uint8_t *spd, int len);
int			spd_dev_check(uint8_t addr, uint8_t *data);

int			spd_imc_dev_check(uint8_t addr, uint8_t *data);

#endif


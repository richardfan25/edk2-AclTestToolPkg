//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  DAT : Data File - POWERUP.DAT                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __DAT_H
#define __DAT_H

#include <time.h>


//=============================================================================
//  general
//=============================================================================
#define PU_HDR_MAGIC				"PUHDRV30"		// header
#define PU_PAR_MAGIC				"PUPARV30"		// parameter
#define PU_DES_MAGIC				"PUDESV30"		// description

//=============================================================================
//  raw_id (powerup_v3)
//=============================================================================
#define RAW_ID_CPU_CPUID			0x8085		// collect all cpuid
#define RAW_ID_CPU_VENDOR			0x8086		// vendor (str)
#define RAW_ID_CPU_NAME				0x8087		// name (str)
#define RAW_ID_CPU_CLOCK			0x8088		// clock (sz=32-bit, unit=mhz)
#define RAW_ID_CPU_TEMP				0x8089		// MSR (sz=1)

#define RAW_ID_MEM_E820				0xE820		// all E820 (N*E820)
#define RAW_ID_MEM_E820_NUM			0xE821		// num of item (sz=1)
#define RAW_ID_MEM_E820_KSZ			0xE822		// kbz (sz=4)
#define RAW_ID_MEM_E820_MSZ			0xE823		// mbz (sz=4)

#define RAW_ID_ACL_BIOS				0xB105		// ACL BIOS String (sz<256, variable)
#define RAW_ID_ACL_BIOS_ADDR		0xB106		// address of ACL BIOS string
#define RAW_ID_ACL_BIOS_MODEL		0xB107		// ACL BIOS model name
#define RAW_ID_ACL_BIOS_VER			0xB108		// ACL BIOS version

#define RAW_ID_ACL_EC_CHIP			0xEC00		// ACL EC Chip : IT8528, RDC A9610
#define RAW_ID_ACL_EC_BOARD			0xEC01		// ACL EC BOARD Name	(sz=16)
#define RAW_ID_ACL_EC_INFO			0xEC02		// ACL EC Info
#define RAW_ID_ACL_EC_VER			0xEC03		// ACL EC VER (sz=16)

#define RAW_ID_BDA					0x0BDA		// BIOS Data Area

#define RAW_ID_DMI_SMBIOS			0x5B00		// DMI-SMBIOS (sz=var)
#define RAW_ID_DMI_SMBIOS_HDR		0x5B01		// DMI-SMBIOS header
#define RAW_ID_DMI_SMBIOS_ADDR		0x5B02		// DMI-SMBIOS header
#define RAW_ID_DMI_SMBIOS_V3HDR		0x5B03		// DMI-SMBIOS header
#define RAW_ID_DMI_SMBIOS_V3ADDR	0x5B04		// DMI-SMBIOS V3 header


#define RAW_ID_PCI_DEV_LST			0x9C10		// vid/did list of all PCI device(4*N)
#define RAW_ID_PCI_DEV_NUM			0x9C11		// num of PCI devices
#define RAW_ID_PCI_DEV_BDF			0x9C1B		// Bus:Dev:Fun (3*N)
#define RAW_ID_PCI_DEV_CFG			0x9C1C		// config of all PCI devices (256*N)

#define RAW_ID_CSM_INFO				0x9C53		// CSM info

#define RAW_ID_SMB_DEV_LST			0x5500		// slave address list of all SMBus devices (1*N)
#define RAW_ID_SMB_DEV_NUM			0x5501		// num of SMBus device
#define RAW_ID_SMB_SPD_LST			0x5591		// SPD addr list (1*N)
#define RAW_ID_SMB_SPD_NUM			0x5592		// num of SPD device
#define RAW_ID_SMB_SPD				0x559D		// SPD (256*N)

#define RAW_ID_SPD_MODEL			0x5600		// SPD Model

#define RAW_ID_RTC_CMOS				0x2744		// RTC CMOS RAM : 128 bytes
#define RAW_ID_RTC_ABD				0x274D		// RTC [0A][0B][0D] : 3 bytes

//#define RAW_ID_PU_PAR				0x8080		// powerup parameters : pu_par_t
//#define RAW_ID_PU_REC				0x0080		// power up record    : pu_rec_t

//=============================================================================
//  pu_par
//=============================================================================

// reset type
#define PU_PAR_RST_NONE			0x00
#define PU_PAR_RST_KBC			0x01
#define PU_PAR_RST_SOFT92		0x02
#define PU_PAR_RST_SOFT			0x03
#define PU_PAR_RST_HARD			0x04
#define PU_PAR_RST_FULL			0x05


// feature
#define PU_PAR_FEAT_ABIOS		0x8000		// bit[15]: abios present
#define PU_PAR_FEAT_AEC			0x4000		// bit[14]: aec present
#define PU_PAR_FEAT_VIDEO_EN	0x2000		// bit[13]: video enable/disable ? INT 10h
#define PU_PAR_FEAT_CPU_INTEL	0x1000		// bit[12]: Intel CPU with temperature
#define PU_PAR_FEAT_CPU_NO_TEMP	0x800		// bit[11]: Intel CPU without temperature
#define PU_PAR_FEAT_SPD			0x400		// bit[10]: spd present
#define PU_PAR_FEAT_SMBIOS		0x200		// bit[9]:  smbios present
#define PU_PAR_FEAT_SMB			0x100		// bit[8]:  smbus present
#define PU_PAR_FEAT_INTEL_CPU	0x80		// bit[7]:  1=intel cpu, 0=non-intel cpu
#define PU_PAR_FEAT_IMC_SPD		0x40		// bit[6]:  1=IMC SPD

											// bit[5:3] reserved

#define PU_PAR_FEAT_ALM_SW		0x4			// bit[2]: alm_sw
#define PU_PAR_FEAT_NO_RTC		0x2			// bit[1]: no_rtc
#define PU_PAR_FEAT_AT_ATX		0x1			// bit[0]: at_atx

// sort type
#define PU_PAR_SORT_NO_INC		0x00
#define PU_PAR_SORT_NO_DEC		0x01
#define PU_PAR_SORT_TEMP_INC	0x02
#define PU_PAR_SORT_TEMP_DEC	0x03
#define PU_PAR_SORT_TSC_INC		0x04
#define PU_PAR_SORT_TSC_DEC		0x05
#define PU_PAR_SORT_INTV_INC	0x06
#define PU_PAR_SORT_INTV_DEC	0x07

// cmp_sw : compare switch
#define PU_PAR_CMP_RTC			0x100	// bit[8]: RTC= RTC
#define PU_PAR_CMP_SP			0x80	// bit[7]: SP = SPD
#define PU_PAR_CMP_SB			0x40	// bit[6]: SB = SMBus
#define PU_PAR_CMP_PC			0x20	// bit[5]: PC = PCI
#define PU_PAR_CMP_SM			0x10	// bit[4]: SM = SMBIOS
#define PU_PAR_CMP_E8			0x8		// bit[3]: E8 = E820
#define PU_PAR_CMP_CP			0x4		// bit[2]: CP = CPU vendor/name
#define PU_PAR_CMP_AEC			0x2		// bit[1]: AEC= ACL EC
#define PU_PAR_CMP_AB			0x1		// bit[0]: AB = ACL BIOS
#define PU_PAR_CMP_DEF			0x01FF	// default comapre switch


//=============================================================================
//  pu_rec
//=============================================================================
#define PU_REC_MAX_NUM			(65535)

// acl_flag
#define PU_REC_ACL_ABIOS_PRE	0x80	// ABIOS present : this time
#define PU_REC_ACL_ABIOS_MODEL	0x40	// ABIOS model name
#define PU_REC_ACL_ABIOS_VER	0x20	// ABIOS version
#define PU_REC_ACL_AEC_PRE		0x8		// AEC present : this time
#define PU_REC_ACL_AEC_CHIP		0x4		// AEC chip name
#define PU_REC_ACL_AEC_BOARD	0x2		// AEC board name 
#define PU_REC_ACL_AEC_VER		0x1		// AEC version

#define PU_REC_ACL_CHK_MASK		0x67	// find error mask
#define PU_REC_ACL_BIOS_MASK	0x60	// ABIOS mask
#define PU_REC_ACL_EC_MASK		0x07	// AEC mask

// sys_flag
#define PU_REC_SYS_CPU_VENDOR	0x8000	// CPU vendor
#define PU_REC_SYS_CPU_NAME		0x4000	// CPU name
#define PU_REC_SYS_SMBIOS		0x800	// SMBIOS
#define PU_REC_SYS_E820			0x400	// E820 mem size
#define PU_REC_SYS_SMB_SCAN		0x80	// SMBUS scan
#define PU_REC_SYS_SMB_SPD		0x40	// SMBUS SPD
#define PU_REC_SYS_PCI_SCAN		0x8		// PCI scan
#define PU_REC_SYS_RTC_0A		0x4		// RTC[0A]
#define PU_REC_SYS_RTC_0B		0x2		// RTC[0B]
#define PU_REC_SYS_RTC_0D		0x1		// RTC[0D]

#define PU_REC_SYS_CHK_MASK		0xCCCF	// find error mask

// qc
#define PU_REC_QC_NONE			0
#define PU_REC_QC_OK			1
#define PU_REC_QC_GG			2
#define PU_REC_QC_ER			3
#define PU_REC_QC_NA			4

// rt_flag
#define PU_REC_RT_CPU_INTEL		0x80


#pragma pack(1)

//*****************************************************************************
//*****************************************************************************
//
//  POWERUP V1
//
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  pu_v1_dat_t
//=============================================================================
typedef struct _pu_v1_dat_t
{
	uint8_t		magic[8];	// ADECGDQA
	uint32_t	fsz;		// file size
	uint8_t		feat;		// features
	uint8_t		rst_tmo;	// reset timeout, unit=sec, 0~255
	uint16_t	cnt_na;

	uint8_t		bios_ver[128];		// [10]
	uint8_t		ec_prj_name[16];	// [90]
	uint8_t		ec_ver_dat[10];		// [A0]
	uint16_t	ec_acc_type;
	uint16_t	ec_cmd_port;
	uint16_t	ec_dat_port;

	time_t		tm_init;
	uint16_t	cnt_total;
	uint16_t	cnt_pass;
	uint16_t	cnt_fail;
	uint16_t	cnt_err;

	uint16_t	tol;		// tolerance
	uint16_t	intv;		// interval

	time_t		tm_log[10000];

	uint32_t	crc32;

} pu_v1_dat_t;	// file size = 40196


//*****************************************************************************
//*****************************************************************************
//
//  POWERUP V2
//
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  pu_v2_dat_t
//=============================================================================
typedef struct _pu_v2_dat_t
{
	uint8_t		magic[8];	// ACLECDQA
	uint32_t	fsz;		// file size
	uint8_t		feat;		// features
	uint8_t		rst_tmo;	// reset timeout, unit=sec, 0~255
	uint16_t	cnt_na;

	uint8_t		bios_ver[128];		// [10]
	uint8_t		ec_prj_name[16];	// [90]
	uint8_t		ec_ver_dat[10];		// [A0]
	uint8_t		ec_acc_type;
	uint8_t		feat2;
	uint16_t	ec_cmd_port;
	uint16_t	ec_dat_port;

	time_t		tm_init;
	uint16_t	cnt_total;
	uint16_t	cnt_pass;
	uint16_t	cnt_fail;
	uint16_t	cnt_err;

	uint16_t	tol;		// tolerance
	uint16_t	intv;		// interval

	time_t		tm_log[65536];

	uint32_t	crc32;

} pu_v2_dat_t;	// file size = 262340 = 256.2KB

//=============================================================================
//  powerup_rec_t : todo.... old?
//=============================================================================
typedef struct _powerup_rec_t
{
	time_t		tm_log;
	time_t		tm_prev;

	uint16_t	no;			// number   : 0~16383
	int32_t		intv;		// interval
	uint8_t		res;		// result	: 2-bit
	uint8_t		rsvd;		// reserved
	
} powerup_rec_t;

//*****************************************************************************
//*****************************************************************************
//
//  POWERUP V3
//
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  DAT file layout
//=============================================================================
// offset  size   size   desc
//----------------------------------
// 0000    0x60     96   header
// 0060    0x40     64   paramter
// 00A0    0x100   256   description
// 01A0    raw_sz    M   raw
// 01A0+M

//=============================================================================
//  pu_hdr_t (v3) : header
//=============================================================================
typedef struct _pu_hdr_t
{
	uint8_t		magic[8];	// 00 8  magic : PUHDRV30
	uint32_t	fsz;		// 08 4  file size
	uint32_t	rsvd;		// 0C 4  rsvd

	// header
	uint32_t	hdr_off;	// 10 4  offset
	uint32_t	hdr_sz;		// 14 4  size
	uint32_t	hdr_rsvd;	// 18 4  rsvd
	uint32_t	hdr_crc;	// 1C 4  crc32

	// parameter
	uint32_t	par_off;	// 20 4  offset
	uint32_t	par_sz;		// 24 4  size
	uint32_t	par_rsvd;	// 28 4  rsvd
	uint32_t	par_crc;	// 2C 4  crc32

	// description
	uint32_t	des_off;	// 30 4  offset
	uint32_t	des_sz;		// 34 4  size
	uint32_t	des_rsvd;	// 38 4  rsvd
	uint32_t	des_crc;	// 3C 4  crc32

	// raw
	uint32_t	raw_off;	// 40 4  offset
	uint32_t	raw_sz;		// 44 4  size
	uint32_t	raw_num;	// 48 4  number
	uint32_t	raw_crc;	// 4C 4  crc32

	// record
	uint32_t	rec_off;	// 50 4  offset
	uint32_t	rec_sz;		// 54 4  size
	uint32_t	rec_num;	// 58 4  rsvd
	uint32_t	rec_crc;	// 5C 4  crc32
							// 60
} pu_hdr_t;

//=============================================================================
//  pu_par_t
//=============================================================================
typedef struct _pu_par_t
{
	uint8_t		magic[8];	// 00 8 : PUPARV30

	uint32_t	psz;		// 08 4 : parameter size
	
	time_t		tm_init;	// 0C 4 : initial time
	time_t		tm_zero;	// 10 4 : 1st time

	uint16_t	rec_cnt;	// 14 2 : record cnt : 0~65535

	// threshould
	uint16_t	intv;		// 16 2 : interval
	uint16_t	tol;		// 18 2 : tolerance

	// count : needed?
	/*
	uint16_t	cnt_total;	// num of total
	uint16_t	cnt_pass;	// num of pass (t < intv)
	uint16_t	cnt_fail;	// num of fail (t >= intv)
	uint16_t	cnt_error;	// num of error (rtc time err, time back to pass)
	uint16_t	cnt_na;		// num of n/a
	*/
	
	uint16_t	alm_hi;		// 1A 2 : alarm hi freq
	uint16_t	alm_lo;		// 1C 2 : alarm lo freq

	uint16_t	rst_tmo;	// 1E 2 : reset timeout : (seconds)

	// reset type
	uint8_t		rst_typ;	// 20 1 : reset type : CF9 04,06,0E (3-bit)
	
	// sort
	uint8_t		sort_typ;	// 21 1 : sort by rec_no/intv (1-bit)
	
	
	//uint8_t		at_atx;		// 24 1 : 0=atx mode, 1=at mode
	//uint8_t		no_rtc;		// 25 1 : 0=with rtc, 1=without rtc
	//uint8_t		alm_sw;		// 1B 1 : alarm on/off
	//uint8_t		video_en;	// 20 1 : video enable/disable ? INT 10h
	//uint8_t		aec_pre;	//
	//uint8_t		abios_pre	//

	
	// feat : feature
	// bit[15]: abios present
	// bit[14]: aec present
	// bit[13]: video enable/disable ? INT 10h
	// bit[12]: intel cpu with temperature
	// bit[11]: intel cpu without temperature
	// bit[10]: spd present
	// bit[9]:  smbios present
	// bit[8]:  smbus present
	// bit[7]:  intel cpu
	// bit[6]:  1=iMC SPD
	
	// bit[2]: alm_sw
	// bit[1]: no_rtc
	// bit[0]: at_atx
	uint16_t	feat;		// 22 2 : feature

	// feature
	// 1. bios ver exist
	// 2. ec info exist
	// 3. at/atx mode
	// 4. alarm on/off
	// 5. reset type (3-bit)

	// feature2
	// 1. sort_rec

	// compare switch : AB, AEC...
	uint16_t	cmp;		// 24 2 : compare switch
	
	// thermal source : temperature
	uint8_t		therm;		// 26 1 : thermal source
							//        bit[7:4] 0=CPU temp, 1=RDC-EC...
							//        bit[3:0] thermal channel (0~3)

	uint8_t		rsvd[25];	// 27 19 : reserved
							// 40

} pu_par_t;

//=============================================================================
//  pu_des_t : description
//=============================================================================
typedef struct _pu_des_t
{
	uint8_t		magic[8];	// 00 8 : magic : PUDESV30
	uint32_t	dsz;		// 08 4 : description size
	uint32_t	rsvd;		// 0C 4 : reserved
	
	char		uname[8];	// 00 8 : utility name        : POWERUP
	char		uver[8];	// 08 8 : utility version     : v3.0
	char		udesc[48];	// 10 30: utility description : Advantech Power Up Record Utility

	char		author[16];	// 40 10: author name         : Richard Fan
	char		email[32];	// 50 20: email address       : richard.fan@advantech.com.tw

	char		div[16];	// 70 10: division            : ECG-FW
	char		organ[48];	// 80 30: organization        : Advantech Embedded Computing Group
	char		copyr[64];	// C0 40: copyright           : Copyright (C) 2016-2017 Advantech Co.,Ltd. All Rights Reserved
							// 100
	
} pu_des_t;

//=============================================================================
//  pu_raw_t
//=============================================================================
typedef struct _pu_raw_t
{
	uint32_t	id;		// record id
	uint32_t	len;	// record data len
	uint8_t		*data;	// record data

	struct _pu_raw_t	*prev;
	struct _pu_raw_t	*next;
	
} pu_raw_t;

//=============================================================================
//  pu_rec_t : v3
//=============================================================================
typedef struct _pu_rec_t
{
	time_t		tm_rtc;		// 00 4 RTC time or SYS time

	float		cpu_clk;	// 04 4
	float		cpu_tsc;	// 08 4 tsc boot time (calculated)
	uint8_t		cpu_temp;	// 0C 1 CPU temperature

	// check status
	// 0 : err/none
	// 1 : ok
	
	// [7] ABIOS present
	// [6] ABIOS model name
	// [5] ABIOS version
	// [4] reserved
	// [3] AEC present
	// [2] AEC chip name
	// [1] AEC board name
	// [0] AEC fw version
	uint8_t		acl_flag;	// 0D 1 acl flag

	// [15]    CPU vendor
	// [14]    CPU name
	// [13:12] reserved
	// [11]    SMBIOS
	// [10]    E820 size
	// [9:8]   reserved
	// [7]     SMBus (SPD)
	// [6:4]   reserved
	// [3]     PCI
	// [2:0]   RTC[0A, 0B, 0D]
	uint16_t	sys_flag;	// 0E 2 sys flag

	uint16_t	no;			// 10 2 0~65535
	uint8_t		qc;			// 12 1 ?

	// [7]   Intel CPU
	// [6:0] reserved
	uint8_t		rt_flag;	// 13 1
	
	int			intv;		// 14 4
							// 18

	//uint8_t		rsvd[8];	// padding

} pu_rec_t;

//=============================================================================
//  pu_sta_t
//=============================================================================
typedef struct _pu_sta_t
{
	uint16_t	to;	// total
	uint16_t	ok;	
	uint16_t	gg;
	uint16_t	er;
	uint16_t	na;
	uint16_t	oo;	// check flag (num of record)
	uint16_t	xx;	// check flag (num of record)

	uint32_t	nx;	// check flag (num of x)

} pu_sta_t;

//=============================================================================
//  pu_cpu_t
//=============================================================================
/*
typedef struct _pu_cpu_t
{
	char		vendor[32];
	char		name[128];

} pu_cpu_t;
*/

//=============================================================================
//  pu_dat_t (v3)
//=============================================================================
// . variable length
// . compressed data
//=============================================================================
typedef struct _pu_dat_t
{
	FILE		*fp;

	pu_hdr_t	*hdr;
	pu_par_t	*par;
	pu_des_t	*des;
	uint8_t		*raw_pool;
	pu_rec_t	*rec_pool;

	
	uint32_t	raw_num;
	pu_raw_t	*head;
	pu_raw_t	*curr;
	
	pu_rec_t	rec;
	pu_sta_t	sta;
	
} pu_dat_t;

//=============================================================================
//  powerup_rt_t
//=============================================================================

//*****************************************************************************
//*****************************************************************************
//
//  POWERUP run-time
//
//*****************************************************************************
//*****************************************************************************
#pragma pack()
//=============================================================================
//  extern
//=============================================================================
extern pu_dat_t	*dat;

//=============================================================================
//  functions
//=============================================================================
uint8_t dat_exist(void);
uint8_t dat_create(void);
uint8_t dat_read(void);
uint8_t dat_update(void);
int		dat_verify(char *fname, uint16_t *reccnt);

uint8_t dat_raw_add(uint32_t id, uint8_t *data, uint32_t len);
uint8_t dat_raw_remove(uint32_t id);
uint8_t dat_check(void);
uint8_t dat_check_board(void);

uint8_t dat_init(void);
void	dat_exit(void);

#endif

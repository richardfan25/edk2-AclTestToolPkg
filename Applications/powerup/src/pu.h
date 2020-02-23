//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  PU : POWERUP General                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __PU_H
#define __PU_H

#include <time.h>

#include "typedef.h"
#include "dat.h"
#include "statis.h"

#pragma pack(1)
//=============================================================================
//  define
//=============================================================================
#define PU_DES_NAME	"POWERUP"
#define	PU_DES_VER		"v3.1a" 
#define PU_DES_DESC		"Advantech Power Up Record Utility"

#define PU_DES_AUTHOR	"Richard Fan"
#define PU_DES_EMAIL	"richard.fan@advantech.com.tw"
#define PU_DES_DIV		"ECG-FW team"
#define PU_DES_ORGAN	"Advantech Embedded Computing Group"
#define PU_DES_COPYR	"Copyright (C) 2016-2019 Advantech Co.,Ltd. All Rights Reserved"

// mode
#define PU_MODE_OFF_LINE	1
#define PU_MODE_ON_LINE	2


//=============================================================================
//  pu_rec_t
//=============================================================================
#if 0 // dat.h
typedef struct _pu_rec_t
{
	time_t		logtm;	// log time
	time_t		pretm;	// previous time
	uint16_t	no;		// number
	int32_t		intv;	// interval
	uint8_t		res;	// result
	uint8_t		rsvd;	// reserved
	
} pu_rec_t;
#endif

//=============================================================================
//  pu_cnt_t
//=============================================================================
typedef struct _pu_cnt_t
{
	uint16_t	total;
	uint16_t	pass;
	uint16_t	fail;
	uint16_t	error;
	uint16_t	na;

} pu_cnt_t;

//=============================================================================
//  pu_info_t
//=============================================================================
typedef struct _pu_info_t
{
	time_t		initm;		// initial time
	time_t		modtm;		// modified time

	uint16_t	tol;		// tolerance
	uint16_t	intv;		// interval

	uint16_t	feat;		// feature
	uint16_t	rst_tmo;	// reset time out

	uint16_t	mem_sz;		// init : memory size (E820)

	uint32_t	det;		// detect flag
							// BIT3
	
} pu_info_t;

//=============================================================================
//  pu_rt_t
//=============================================================================
typedef struct _pu_rt_t
{
	pu_rec_t	*prec;
	
	pu_cnt_t	cnt;

} pu_rt_t;

#if 0
typedef struct st_powerup_run_time
{
	ABIOS_INFO		*abios;
	AEC_INFO		*aec;

	POWERUP_REC		*pr;

	u8				video_en;
	u8				feature;
	u8				feature2;
	u8				sort;
	u8				reset_type;	// default : PAR_RESET_NONE
	u8				err_code;

	u16				par;		// parameter 
	u16				reset_tmout;

	char			fmodel[32];	// model name saved in DAT file
	char			cmodel[32];	// model name in memory

	u16				alarm_low_hz;
	u16				alarm_hi_hz;

	time_t			now_time;
	int				page_st_idx;

	int				init_stage;
	int				mode;			// 0=off-line, 1=on-line
	char			dat_file[32];
	int				err;
	
	u8				result;		// for big number showing
	u8				ver;		// ver 1 or 2

	u32				crc_sum;
	u32				crc_cal;
	u32				crc_cal2;

} POWERUP_RT;
#endif

//=============================================================================
//  pu_cpu_t
//=============================================================================
typedef struct _pu_cpu_t
{
	char		vendor[32];
	char		name[128];
	uint8_t		clock;

	uint64_t	boot_tsc;
	uint64_t	tsc0;
	uint64_t	tsc1;

	uint8_t		temp;	// C

} pu_cpu_t;

//=============================================================================
//  pu_abios_t
//=============================================================================
typedef struct _pu_abios_t
{
	char	magic[256];

} pu_abios_t;

//=============================================================================
//  pu_aec_t
//=============================================================================
typedef struct _pu_aec_t
{
	char	chip[32];
	char	ver[32];
	char	board[32];

} pu_aec_t;

//=============================================================================
//  pu_time_t
//=============================================================================
typedef struct _pu_time_t
{
	time_t		curr;
	time_t		prev;
	uint32_t	tmr;
	uint8_t		rst_tmr;

} pu_time_t;

//=============================================================================
//  pu_alarm_t
//=============================================================================
typedef struct _pu_alarm_t
{
	uint8_t		cmd;
	uint8_t		sts;
	uint32_t	tmo;

	uint16_t	lo;		// Hz
	uint16_t	hi;		// Hz

} pu_alarm_t;

//=============================================================================
//  pu_find_t
//=============================================================================
typedef struct _pu_find_t
{
	int		fail;
	int		error;
	int		na;
	int		ooxx;

} pu_find_t;

//=============================================================================
//  pu_font_t
//=============================================================================
typedef struct _pu_font_t
{
	uint8_t		*bmp;	// bitmap
	uint16_t	width;
	uint16_t	height;
	uint32_t	sz;		// size in byte

} pu_font_t;

//=============================================================================
//  pu_cnt_t
//=============================================================================
/*
typedef struct _pu_cnt_t
{
	int		intv;
	int		cnt;
	uint8_t	res;
	
	struct _pu_cnt_t *next;

} pu_cnt_t;
*/

//=============================================================================
//  pu_stt_t
//=============================================================================
/*
typedef struct _pu_stt_t
{
	pu_cnt_t	*pc_head;
	pu_cnt_t	*pc_curr;
	
} pu_stt_t;
*/

//=============================================================================
//  pu_t
//=============================================================================
typedef struct _pu_t
{
	time_t		tm_curr;
	time_t		tm_prev;

	uint16_t	no;
	uint8_t		mode;	// 0=offline, 1=online
	uint8_t		init;
	char		file_name[32];

	uint8_t		atatx_tmo;
	uint8_t		sort_typ;
	
	pu_cpu_t	cpu;
	pu_abios_t	abios;
	pu_aec_t	aec;

	pu_time_t	tm;
	pu_alarm_t	alm;

	pu_statis_t	stt;

	pu_find_t	find;

	//pu_stt_t	stt;
	
	pu_font_t	font;

} pu_t;
#pragma pack()

//=============================================================================
//  extern
//=============================================================================
extern pu_t	*pu;

//=============================================================================
//  functions
//=============================================================================
void	pu_help_show(void);			// F1 : help
void	pu_about_show(void);		// F2 : about
									// F3/F4 : sort
int		pu_trep_select(void);		// F5 : view text report
int		pu_sel_dat(void);			// F6 : select dat file
void	pu_create_grep(void);		// F7 : create graphic report
uint8_t pu_create_trep(void);		// F8 : create text report

void	pu_statis_init(void);		// F9 : statistic
void	pu_statis_exit(void);
void	pu_statis_show(void);

int		pu_log_select(void);		// L : view log file

#endif

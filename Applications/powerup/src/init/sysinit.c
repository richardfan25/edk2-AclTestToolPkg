//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  SYSINIT : System Init                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
//#include <malloc.h>

#include "typedef.h"
#include "tsc.h"
#include "hpet.h"
#include "cpuid.h"
#include "msr.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "t_hextab.h"
#include "t_scrcap.h"
#include "e820.h"
#include "smbios.h"
#include "pci.h"
#include "pciname.h"
#include "csm.h"
#include "smb.h"
#include "spd.h"
#include "cpuid.h"
#include "bda.h"
#include "key.h"
#include "abios.h"
#include "aec.h"
#include "acpi_ec.h"
#include "ite_ec.h"
#include "rdc_ec.h"
#include "mode3.h"
#include "sound.h"
#include "rtc.h"
#include "imc.h"

#include "dat.h"
#include "errcode.h"
#include "cpmenu.h"

#include "scr.h"
#include "pu.h"
#include "sysinit.h"

// debug
#include "t_bignum.h"
#include "t_hlbox.h"
#include "t_vlbox.h"
#include "t_svlbox.h"


#ifdef __WATCOMC__
//#include <i86.h>
#endif

#define SYS_INIT_CHECK(res)	{ if (res) return res; }

//extern uint64_t	boot_tsc;
//extern uint64_t	tsc0;
//extern uint64_t	tsc1;

//=============================================================================
//  1st time checking : sys_init
//=============================================================================
//  1. timer must be working.
//  2. cpu vendor and brand
//  3. memory size (E820)
//  4. get ABIOS and AEC    -> saved abios and aec info
//  5. scan all PCI bus     -> saved all pci device vid and did (device list)
//  6. scan all SMbus       -> saved SMBus device list

//=============================================================================
//  variables
//=============================================================================
aec_t		*aec;

//=============================================================================
//  sys_init_timer_check
//=============================================================================
uint8_t	sys_init_timer_check(void)
{
	char		title[32];
	char		msg[32];
	uint8_t		res = PU_OK;
	uint8_t		tcnt = 0;
	uint8_t		bs_cnt = 0;
	uint16_t	key;
	uint32_t	tms, tmc, tmo;
	uint8_t		cmos_ram[128];
	int			i;
	

	scr_clear_keyhlp();

	// 1 second
	msgbox_tmout(T_BLACK, T_CYAN, "Timer", "Check x86 timer is working......", 4, 1, 1000);
	key_flush_buf();

	tmo = 0;

	sprintf(title, "%s", "Timer should be running...");
	sprintf(msg, " %02d:%02d ", tmo/60, tmo%60);
	msgbox_doing(T_BLACK, T_CYAN, title, msg, 4, 1);
	
	tmc = bda_get_timer();
	tms = tmc;
	tmo = tmc + 18;			// 18.2 Hz

	scr_show_timer_keyhlp();

	do
	{
		tmc = bda_get_timer();

		if (tmc >= tmo)
		{
			msgbox_doing_done();

			tmo = (tmc - tms) / 18;

			sprintf(msg, "%02d:%02d", tmo/60, tmo%60);
			msgbox_doing(T_BLACK, T_CYAN, title, msg, 4, 1);
			
			tmo = tmc + 18;
			tcnt++;
			sound_ok();

			if (tcnt > 2)	// over 2 seconds
			{
				res = 0;
				break;
			}
			
		}

		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if(key)//(key != KEY_SC_NULL)
		{
			if (key == (SCAN_ESC << 8))
			{
				res = 0xFA;
				break;
			}
			else if (key == CHAR_BACKSPACE)
			{
				// press ESC key 2 times to exit in case the timer is not working
				bs_cnt++;
				if (bs_cnt >= 2)
				{
					res = 0xFF;
					break;
				}
			}
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
			}
			else
			{
				bs_cnt = 0;
			}
		}
		
	} while (1);

	msgbox_doing_done();

	if (res == 0xFF)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Timer", "Timer check aborted !\nPlease ask for help.", 4, 1);
		res = PU_ERR_TMR_DEAD;
	}
	else if (res == 0xFA)
	{
		msgbox_tmout(T_WHITE, T_RED, "Timer", "Timer check aborted !\nBack to previous step.", 4, 1, 1000);
		key_flush_buf();
		res = PU_ERR_ESC_ABORT;
	}
	else
	{
		pu->cpu.tsc0 = rdtsc();
		scr_clear_keyhlp();
		msgbox_tmout(T_BLACK, T_CYAN, "Timer", "Timer is running now!", 4, 1, 1000);
		key_flush_buf();
		pu->cpu.tsc1 = rdtsc();
	}

	// RAW : RTC_CMOS
	for (i=0; i<128; i++)
	{
		cmos_ram[i] = rtc_read_reg((uint8_t)i);
	}

	dat_raw_add(RAW_ID_RTC_CMOS, (uint8_t *)cmos_ram, 128);

	return res;
}

//=============================================================================
//  sys_init_cpu_check
//=============================================================================
uint8_t	sys_init_cpu_check(void)
{
	char		cpu_vendor[32];
	char		cpu_name[128];
	char		cpu_temp[64];
	char		cpu_clock[64];
	char		cpu_tsc[64];
	uint16_t		key = 0;
	int			sx, sy, sw, sh;
	int			temp = 0, temp0 = 0;
	int			i, len;
	uint8_t		res;
	uint32_t	mhz;
	uint8_t		fg, bg;
	uint8_t		cpu_temp_en = 0;
	uint32_t	cpu_id_data[4];	// EAX, EBX, ECX, EDX
	uint32_t	data[48][4];


	fg = T_BLACK;
	bg = T_LIGHTGRAY;

	//  CPU vendor
	if (cpuid_get_vendor(cpu_vendor))
		return PU_ERR_GET_CPU_VENDOR;

	// CPU name
	if (cpuid_get_brand(cpu_name))
		return PU_ERR_GET_CPU_NAME;


	if (pu->init == 0)
	{
		pu->cpu.tsc0 = rdtsc();
		msgbox_tmout(fg, bg, "CPU", "Getting CPU Info......", 4, 1, 1000);
		key_flush_buf();
		pu->cpu.tsc1 = rdtsc();
	}

	// CPU vendor
	// CPU name
	// Temperature
	// Clock
	if (strstr(cpu_vendor, "Intel"))
	{
		cpuid_get_id(cpu_id_data, 0x00);	// EAX:max number function supported
		
		if (cpu_id_data[0] < 7)
		{
			temp = temp0;	// not support DTS
		}
		else
		{
			cpuid_get_id(cpu_id_data, 0x06);

			// CPUID.06H.EAX
			// bit0 : DTS is supported if set
			// bit6 : PTM (Package thermal management is supported if set)
	
			// avoid rdmsr (0x1A2) #GP
			//if ((cpu_id_data[0] & 0x41)== 0x41)	// EAX
			if ((cpu_id_data[0] & 0x01)== 0x01)		// EAX : just DTS only
			{
				// Intel DTS
				temp = (int)((AsmReadMsr64(0x1A2) >> 16) & 0xFF) - (int)((AsmReadMsr64(0x19C) >> 16) & 0x7F);
				sprintf(cpu_temp, "CPU Temperature : %3d %cC", temp, 0xf8);
				temp0 = -1;
				cpu_temp_en = 1;
			}
			else
			{
				temp = temp0;	// not support msr(0x1a2) MSR_TEMPERATURE_TARGET
			}
			SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_INTEL_CPU);
		}
	}
	else
	{
		temp = temp0;	// not Intel
		CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_INTEL_CPU);
	}
		
	//sprintf(cpu_clock, "Clock : %.3f MHz", (double)(tsc1 - tsc0) / 1000000.0);
	sprintf(cpu_clock, "Clock : %.3f MHz", (float)(pu->cpu.tsc1 - pu->cpu.tsc0) / 1000000.0);
	sprintf(cpu_tsc, "TSC : %.3f seconds", (float)pu->cpu.boot_tsc/ (float)(pu->cpu.tsc1 - pu->cpu.tsc0));

	sw = 0;

	len = (int)strlen(cpu_vendor);
	if (len > sw)
		sw = len;

	len = (int)strlen(cpu_name);
	if (len > sw)
		sw = len;

	if (cpu_temp_en)
	{
		len = (int)strlen(cpu_temp);
		if (len > sw)
			sw = len;
	}

	len = (int)strlen(cpu_clock);
	if (len > sw)
		sw = len;
	sw += 8;
	if (sw > 80)
		sw = 80;

	//sh = 4 + 3 * 2;	// 3 items
	sh = 4 + 5 * 2;	// 5 items default : vendor, name, temp, clock, tsc

	if (cpu_temp_en == 0)
		sh -= 2;	// 2 items : without cpu temp

	sw++;	// shadow
	sh++;	// shadow

	sx = (80 - sw) / 2;
	sy = (25 - sh) / 2;

	t_copy_region(sx, sy, sx+sw, sy+sh);

	t_color(T_BLACK, T_LIGHTGRAY);

	// window
	t_put_win_single_double(sx, sy, sw, sh);
	for (i=sx+1; i<=sx+sw; i++)
		t_shadow_char(i, sy+sh);	// shadow : bottom border

	for (i=sy+1; i<=sy+sh; i++)
		t_shadow_char(sx+sw+0, i);	// shadow : right border
		
	t_xy_puts((sw-3)/2+sx, sy+1, "CPU");	// title
	t_xy_puts((sw-(int)strlen(cpu_vendor))/2+sx, sy+4, cpu_vendor);
	t_xy_cl_puts((sw-(int)strlen(cpu_name))/2+sx, sy+6, T_BLUE, bg, cpu_name);

	scr_show_back_confirm_keyhlp();
	
	if (strstr(cpu_vendor, "Intel"))
	{
		// Intel CPU
		if (cpu_temp_en)
		{
			t_xy_cl_puts((sw-(int)strlen(cpu_temp))/2+sx, sy+8, T_RED, bg, cpu_temp);
			t_xy_cl_puts((sw-(int)strlen(cpu_clock))/2+sx, sy+10, T_BLUE, bg, cpu_clock);
			t_xy_cl_puts((sw-(int)strlen(cpu_tsc))/2+sx, sy+12, T_MAGENTA, bg, cpu_tsc);
		}
		else
		{
			t_xy_cl_puts((sw-(int)strlen(cpu_clock))/2+sx, sy+8, T_BLUE, bg, cpu_clock);
			t_xy_cl_puts((sw-(int)strlen(cpu_tsc))/2+sx, sy+10, T_MAGENTA, bg, cpu_tsc);
		}

		while (1)
		{
			if (cpu_temp_en)
			{
				// update cpu temperature
				temp = (int)((AsmReadMsr64(0x1A2) >> 16) & 0xFF) - (int)((AsmReadMsr64(0x19C) >> 16) & 0x7F);
		
				if (temp ^ temp0)
				{
					sprintf(cpu_temp, "CPU Temperature : %3d %cC", temp, 0xf8);
					t_xy_cl_puts((sw-(int)strlen(cpu_temp))/2+sx, sy+8, T_RED, bg, cpu_temp);
					temp0 = temp;
				
					t_fg_color(fg);
				}
			}

			key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

			if(key)//(key != KEY_SC_NULL)
			{
				if (key == (SCAN_ESC << 8))
				{
					res = 0xFA;		// ESC abort
					break;
				}
				else if (key == (SCAN_F10 << 8))
				{
					t_scr_capture();
				}
				else if (key == CHAR_CARRIAGE_RETURN)
				{
					res = PU_OK;
					break;
				}
			}
			
			delay(100);
		}
		
		SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_INTEL);
	}
	else
	{
		// not Intel CPU
		t_xy_cl_puts((sw-(int)strlen(cpu_clock))/2+sx, sy+8, T_BLUE, bg, cpu_clock);
		t_xy_cl_puts((sw-(int)strlen(cpu_tsc))/2+sx, sy+10, T_MAGENTA, bg, cpu_tsc);

		key = (uint16_t)bioskey(0);//key_blk_read_sc();

		if (key == (SCAN_ESC << 8))
			res = 0xFA;		// ESC abort
		else
			res = PU_OK;
		
		CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_INTEL);
	}
	
	if (cpu_temp_en == 0)
		SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_NO_TEMP);
	else
		CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_NO_TEMP);

	t_paste_region(-1, -1);

	if (res == PU_OK)
	{
		// RAW : CPU
		dat_raw_add(RAW_ID_CPU_VENDOR, (uint8_t *)cpu_vendor, (uint32_t)strlen(cpu_vendor));
		dat_raw_add(RAW_ID_CPU_NAME, (uint8_t *)cpu_name, (uint32_t)strlen(cpu_name));

		mhz = cpuid_get_clock(cpu_name);
		if (mhz > 0)
		{
			dat_raw_add(RAW_ID_CPU_CLOCK, (uint8_t *)&mhz, sizeof(uint32_t));
		}

		
		for (i=0; i<48; i++)
		{
			if (i < 16)
			{
				// EAX = 0x0 ~ 0xF
				cpuid_get_id(data[i], (uint32_t)i);
			}
			else
			{
				// EAX = 0x80000000 ~ 0x8000001F
				cpuid_get_id(data[i], (uint32_t)(i-16+0x80000000));
			}
		}
		// RAW : collect all cpuid
		dat_raw_add(RAW_ID_CPU_CPUID, (uint8_t *)data, sizeof(data));
	}
	
	return res;
}

//=============================================================================
//  sys_init_spd_svlbox
//=============================================================================
t_svlbox_t	sys_init_spd_svlbox;

//=============================================================================
//  spd_ddr_ht
//=============================================================================
t_hextab_t	spd_ddr_ht;

//=============================================================================
//  sys_init_memory_spd_err_show
//=============================================================================
void sys_init_memory_spd_err_show(uint8_t addr, uint8_t *buf)
{
	char	title[32];
	uint16_t	key;

	sprintf(title, "%02X - EEPROM : Broken SPD", addr);

	t_copy_region(0, 0, 79, 24);
	t_clear_color_screen(T_LIGHTGRAY, T_BLACK);

	//if (page == 1)
	//	spd_ddr_ht.flag |= T_HEX_FL_IDX_WORD;
	//else
		spd_ddr_ht.flag |= T_HEX_FL_IDX_BYTE;

	spd_ddr_ht.title	= title;
	spd_ddr_ht.data		= buf;
	spd_ddr_ht.len		= 256;

	t_show_hextab(&spd_ddr_ht);

	while (1)
	{
		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if(key)//(key != KEY_SC_NULL)
		{
			if (key == (SCAN_ESC << 8))
			{
				break;
			}
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
				break;
			}
		}
		delay(10);
	}

	t_paste_region(-1, -1);
}

//=============================================================================
//  sys_init_memory_spd_show
//=============================================================================
uint16_t sys_init_memory_spd_show(uint8_t addr, uint8_t *buf, char *banner)
{
	char	title[80];
	uint16_t	key;
	uint8_t	ddr4 = 0;
	

	if (strstr(banner, "DDR4"))
		ddr4 |= 0x1;

	if (ddr4)
	{
		if (strstr(banner, "Page 1"))
			spd_ddr_ht.stidx = 0x100;
		else
			spd_ddr_ht.stidx = 0x0;
	}
	else
	{
		spd_ddr_ht.stidx = 0x0;
	}
		

	sprintf(title, "%02X : %s", addr, banner);


	t_copy_region(0, 0, 79, 24);
	//t_clear_color_screen(T_LIGHTGRAY, T_BLACK);

	if (ddr4)
		spd_ddr_ht.flag |= T_HEX_FL_IDX_WORD;
	else
		spd_ddr_ht.flag |= T_HEX_FL_IDX_BYTE;
	spd_ddr_ht.title	= title;
	spd_ddr_ht.data		= buf;
	spd_ddr_ht.len		= 256;

	t_show_hextab(&spd_ddr_ht);

	while (1)
	{
		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if (key)//(key != KEY_SC_NULL)
		{
			if (key == (SCAN_ESC << 8) || key == CHAR_CARRIAGE_RETURN)
			{
				break;
			}
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
				break;
			}
		}
		delay(10);
	}

	t_paste_region(-1, -1);
	
	return key;
}

//=============================================================================
//  sys_init_imc_spd_crc_check
//=============================================================================
uint16_t	sys_init_imc_spd_crc_check(uint8_t addr)
{
	char		*hb_item[2] = { "Yes", "No" };
	uint8_t		ret, ddrt;
	t_hlbox_t	hb;
	char		title[64];
	char		msg[256];
	int			i;
	uint8_t		buf[256];
	uint16_t	crc = 0, crc_spd = 0;
	uint16_t		res;
	
	

	hb.title	= "  Would you like to check SPD CRC ?  ";
	hb.item		= hb_item;
	hb.help		= NULL;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = 0;

	hb.fg = T_WHITE;
	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;
	hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;
	hb.bh = T_MAGENTA;

	hb.hm = 2;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 4;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel		= 1;	// default : no

	ret = t_show_hlbox(&hb);
	
	// CRC checking
	if (hb.sel == 0)	// 0=yes
	{
		sprintf(title, "%02Xh : SPD", addr);
		sprintf(msg, "Reading......0x%02X", 0);
		msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

		// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
		imc_set_spd_page(0);

		for (i=0; i<256; i++)
		{
			sprintf(msg, "Reading......0x%02X", i);
			msgbox_progress_cont(title, msg);

			buf[i] = imc_read_byte(addr/2, (uint8_t)i);
		}
		msgbox_progress_done();
		key_flush_buf();

		ddrt = 0;

		//-----------------------------------------------------
		//  DDR3
		//-----------------------------------------------------
		if ((buf[2] == SPD_RAM_TYPE_DDR3) ||
			(buf[2] == SPD_RAM_TYPE_LPDDR3))
		{
ddr3_crc_again:
			// crc size : 117/126 bytes, depends on buf[0].bit7 (0=117B, 1=126B)
			if (buf[0] & 0x80)
			{
				crc = spd_get_crc16((uint8_t*)buf, 117);
				crc_spd = *(uint16_t *)&buf[126];
				sprintf(msg, "SPD (CRC:00~74) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
			}
			else
			{
				crc = spd_get_crc16((uint8_t*)buf, 126);
				crc_spd = *(uint16_t *)&buf[126];
				sprintf(msg, "SPD (CRC:00~7D) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
			}


			if (crc == crc_spd)
			{
				sprintf(title, "%02X : DDR3 SPD CRC OK", addr);
				res = msgbox_waitkey(T_WHITE, T_BLUE, title, msg, 4, 1);
			}
			else
			{
				sprintf(title, "%02X : DDR3 SPD CRC Error", addr);
				res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
			}

			ddrt = 3;
		}
		//-----------------------------------------------------
		//  DDR4
		//-----------------------------------------------------
		else if ((buf[2] == SPD_RAM_TYPE_DDR4) ||
				 (buf[2] == SPD_RAM_TYPE_LPDDR4) ||
				 (buf[2] == SPD_RAM_TYPE_LPDDR4X) ||
				 (buf[2] == SPD_RAM_TYPE_DDR4E))
		{
ddr4_crc_again:
			// crc size : 126 bytes fixed
			crc = spd_get_crc16((uint8_t*)buf, 126);
			crc_spd = *(uint16_t *)&buf[126];
			sprintf(msg, "SPD (CRC:00~7D) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
	
			if (crc == crc_spd)
			{
				sprintf(title, "%02X : DDR4 SPD CRC OK", addr);
				res = msgbox_waitkey(T_WHITE, T_BLUE, title, msg, 4, 1);

			}
			else
			{
				sprintf(title, "%02X : DDR4 SPD CRC Error", addr);
				res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
			}

			ddrt = 4;
		}
		else
		{
			sprintf(title, "%02X : SPD Type Error", addr);
			sprintf(msg,   "SPD type : %02Xh", buf[2]);
			res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
		}

		key_flush_buf();

		if (res == (SCAN_ESC << 8))
			return res;

		//-----------------------------------------------------
		//  SPD shown
		//-----------------------------------------------------
		if (ddrt == 3)
		{
			if (buf[0] & 0x80)
				sprintf(msg, "DDR3 SPD : CRC(00~74)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);
			else
				sprintf(msg, "DDR3 SPD : CRC(00~7D)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);

			res = sys_init_memory_spd_show(addr, buf, msg);
			if (res == (SCAN_ESC << 8))
				goto ddr3_crc_again;
		}
		else if (ddrt == 4)
		{
page0_again:
			// Page 0
			sprintf(msg, "DDR4 SPD : Page 0 > CRC(00~7D)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);
			res = sys_init_memory_spd_show(addr, buf, msg);
			if (res == (SCAN_ESC << 8))
				goto ddr4_crc_again;
			
			// Page 1
			// send quick command 6Eh to switch to page 1 in case of DDR4 DRAM
			imc_set_spd_page(1);

			sprintf(title, "%02Xh : SPD", addr);
			sprintf(msg, "Reading......0x%02X", 0);
			msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

			for (i=0; i<256; i++)
			{
				sprintf(msg, "Reading......0x%02X", i);
				msgbox_progress_cont(title, msg);

				buf[i] = imc_read_byte(addr/2, (uint8_t)i);
			}
			msgbox_progress_done();
			key_flush_buf();
			
			// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
			imc_set_spd_page(0);
			
		
			res = sys_init_memory_spd_show(addr, buf, "DDR4 SPD : Page 1");
			if (res == (SCAN_ESC << 8))
			{
				// Page 0
				// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
				imc_set_spd_page(0);
				

				sprintf(title, "%02Xh : SPD", addr);
				sprintf(msg, "Reading......0x%02X", 0);
				msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

				for (i=0; i<256; i++)
				{
					sprintf(msg, "Reading......0x%02X", i);
					msgbox_progress_cont(title, msg);

					buf[i] = imc_read_byte(addr/2, (uint8_t)i);
				}
				msgbox_progress_done();
				key_flush_buf();

				goto page0_again;
			}
		}
	}
	else
	{
		// no checking CRC
		ret = PU_OK;
	}

	return ret;		// 0=yes, !0=no
}

//=============================================================================
//  sys_init_imc_spd_check
//=============================================================================
uint8_t	sys_init_imc_spd_check(void)
{
	int			d, i, j;
	uint8_t		addr;
	uint8_t		spd[4];
	uint8_t		buf[256];
	char		msg[128];
	uint8_t		sel;
	uint8_t		item[80];
	uint8_t		spd_num = 0;
	uint8_t		spd_addr[16];
	spd_info_t	spd_info[16];	// max num = 16
	uint8_t		*pool;
	uint16_t		key;
	uint32_t	spd_data_sz = 0;


	if (!imc)
	{
		// SMBus type : IMC SMBus
		if (imc_init() != IMC_OK)
			return PU_STS_IMC_FOUND;
	}

	scr_clear_keyhlp();

	// no need to "pci_init" and "imc_init" again due to they're initialzed

	msgbox_progress(T_WHITE, T_MAGENTA, "iMC", "iMC SPD probing......", 4, 1);
	imc_probe_spd();
	msgbox_progress_done();

	// iMC : spd not found
	if (imc->spd_num == 0)
	{
		//msgbox_waitkey(T_WHITE, T_RED, "iMC Error", "Can not detect any RAM module SPD, please ask for help!", 4, 1);
		msgbox_waitkey(T_WHITE, T_RED, "iMC Error", "Can not detect any RAM module SPD, skip SPD checking!", 4, 1);
		return PU_ERR_NO_SPD_FOUND;
	}

	spd_num = 0;

	msgbox_progress(T_WHITE, T_MAGENTA, "RAM Module SPD", "Reading SPD......", 4, 1);

	for (d=0; d<imc->spd_num; d++)
	{
		imc_sel_ch(imc->spd[d].ch);

		addr = imc->spd[d].addr;
		
		// spd : first 4 bytes for probe spd
		if (spd_imc_dev_check(addr, spd) < 0)
			continue;	// not spd

		memcpy(buf, spd, 4);
		
		if ((spd[2] == SPD_RAM_TYPE_DDR4) ||
			(spd[2] == SPD_RAM_TYPE_DDR4E) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4X))
		{
			//-----------------------------------------------------------------
			//  DDR4
			//-----------------------------------------------------------------
			// ddr4 : page 0
			buf[4] = imc_read_byte(addr, 0x04);		// chip size
			buf[5] = imc_read_byte(addr, 0x0C);		// module organ
			buf[8] = imc_read_byte(addr, 0x0E);		// thermal sensor
			buf[9] = imc_read_byte(addr, 0x12);		// cycle time
			buf[10]= imc_read_byte(addr, 0x0D);		// ecc support

			// ddr4 : page 1
			imc_set_spd_page(1);

			buf[6] = imc_read_byte(addr, 0x41);		// msb of manufacturer id : 0x141
			buf[7] = imc_read_byte(addr, 0x40);		// lsb of manufacturer id : 0x140

			// manufacturer part number : DDR4 size = 20 : 0x149h~0x15Ch
			for (i=0; i<20; i++)
			{
				buf[11+i] = imc_read_byte(addr, 0x49+(uint8_t)i);
				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}

		}
		else if ((spd[2] == SPD_RAM_TYPE_DDR3) ||
				 (spd[2] == SPD_RAM_TYPE_LPDDR3))
		{
			//-----------------------------------------------------------------
			//  DDR3
			//-----------------------------------------------------------------
			buf[4] = imc_read_byte(addr, 0x04);		// chip size
			buf[5] = imc_read_byte(addr, 0x07);		// module organ
			buf[6] = imc_read_byte(addr, 0x76);		// msb of manufacturer id
			buf[7] = imc_read_byte(addr, 0x75);		// lsb of manufacturer id
			// [06h] bit1 : 1=DDR3L (1.5V/1.35V)
			buf[8] = imc_read_byte(addr, 0x06);		// ddr3 : without thermal sensor, with ddr3 low voltage support 
			buf[9] = imc_read_byte(addr, 0x0C);		// cycle time
			buf[10]= imc_read_byte(addr, 0x08);		// ecc support

			for (i=0; i<18; i++)
			{
				buf[11+i] = imc_read_byte(addr, 0x80+(uint8_t)i);	// model : manufacturer part number
				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}
		}
		else if (spd[2] == SPD_RAM_TYPE_DDR2)
		{
			//-----------------------------------------------------------------
			//  DDR2
			//-----------------------------------------------------------------
			buf[3] = imc_read_byte(addr, 0x14);		// DIMM type
			buf[4] = imc_read_byte(addr, 0x05);		// num of banks(rank)
			buf[5] = imc_read_byte(addr, 0x1F);		// 
			buf[6] = imc_read_byte(addr, 0x41);		// msb of manufacturer id
			buf[7] = imc_read_byte(addr, 0x40);		// lsb of manufacturer id
			buf[8] = 0;								// ddr2 : without thermal sensor
			buf[9] = imc_read_byte(addr, 0x9);		// cycle time
			buf[10]= imc_read_byte(addr, 0xB);
			for (i=0; i<18; i++)
			{
				buf[11+i] = imc_read_byte(addr, 0x49+(uint8_t)i);	// model : manufacturer part number
				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}

		}

		//-----------------------------------------------------------------
		//  spd info
		//-----------------------------------------------------------------
		if ((spd[2] == SPD_RAM_TYPE_DDR2) ||
			(spd[2] == SPD_RAM_TYPE_DDR3) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR3) ||
			(spd[2] == SPD_RAM_TYPE_DDR4) ||
			(spd[2] == SPD_RAM_TYPE_DDR4E) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4X))
		{
			spd_get_info(&spd_info[spd_num], buf);

			spd_info[spd_num].addr = addr * 2;	// 7-bit >> 8-bit
			
			if (imc->spd[d].ch == 0)
				spd_addr[spd_num] = addr;			// ch 0 : 50~57
			else
				spd_addr[spd_num] = addr | 0x20;	// ch 1 : or 0x20=>70~77

			spd_num++;
		}
		else
		{
			sprintf(msg, "This is not SPD data : spd[2]=%02X, please ask for help.", spd[2]);
			msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);
		}
	}

	msgbox_progress_done();
	key_flush_buf();

	//-----------------------------------------------------------------
	//  VLBOX : sys_init_spd_svlbox
	//-----------------------------------------------------------------
	sys_init_spd_svlbox.item = (char **)malloc(sizeof(char*) * spd_num);
	if (!sys_init_spd_svlbox.item)
		return PU_ERR_MALLOC;
	
	for (i=0; i<spd_num; i++)
	{
		item[0] = 0;
		
		if (spd_info[i].ctype == SPD_RAM_TYPE_DDR2)
		{
			sprintf(item, "%02X: %s-%s %s%s  %d MB : %s %s",
				spd_info[i].addr,
				spd_find_ram_type(spd_info[i].ctype),
				spd_find_ddr2_cycle_time(spd_info[i].cytime),
				spd_find_ddr2_ecc(spd_info[i].ecc),
				spd_find_ddr2_module_type(spd_info[i].mtype),
				spd_info[i].mbsz,
				spd_find_manu_id(spd_info[i].mid),
				spd_info[i].model);
		}
		else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR3) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR3))
		{
			sprintf(item, "%02X: %s%s-%s %s%s  %d MB : %s %s",
				spd_info[i].addr,
				spd_find_ram_type(spd_info[i].ctype),
				spd_find_ddr3_volt(spd_info[i].flag),
				spd_find_ddr3_cycle_time(spd_info[i].cytime),
				spd_find_ddr3_ecc(spd_info[i].ecc),
				spd_find_module_type(spd_info[i].mtype),
				spd_info[i].mbsz,
				spd_find_manu_id(spd_info[i].mid),
				spd_info[i].model);
		}
		else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR4) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_DDR4E) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4X))
		{
			sprintf(item, "%02X: %s-%s %s%s  %d MB : %s %s",
				spd_info[i].addr,
				spd_find_ram_type(spd_info[i].ctype),
				spd_find_ddr4_cycle_time(spd_info[i].cytime),
				spd_find_ddr4_ecc(spd_info[i].ecc),
				spd_find_module_type(spd_info[i].mtype),
				spd_info[i].mbsz,
				spd_find_manu_id(spd_info[i].mid),
				spd_info[i].model);
		}

		sys_init_spd_svlbox.item[i] = (char*)malloc(strlen(item)+1);

		if (!sys_init_spd_svlbox.item[i])
			return PU_ERR_MALLOC;

		sprintf(sys_init_spd_svlbox.item[i], "%s", item);
	}

	sys_init_spd_svlbox.num_item	= spd_num;
	sys_init_spd_svlbox.sel		= 0;
		
	scr_show_back_confirm_keyhlp();

	sel = t_show_svlbox(&sys_init_spd_svlbox);


	if (sel == 0xFA)	// abort
	{
		for (i=0; i<spd_num; i++)
			free(sys_init_spd_svlbox.item[i]);
		free(sys_init_spd_svlbox.item);
	
		return 0xFA;	// return function
	}

	//-----------------------------------------------------------------
	//  RAW
	//-----------------------------------------------------------------
	// RAW : spd num
	dat_raw_add(RAW_ID_SMB_SPD_NUM, (uint8_t *)&spd_num, sizeof(uint8_t));

	// RAW : spd addr
	dat_raw_add(RAW_ID_SMB_SPD_LST, (uint8_t *)spd_addr, sizeof(uint8_t)*spd_num);

	// RAW : spd model
	for (i=0; i<spd_num; i++)
		dat_raw_add(RAW_ID_SPD_MODEL+i, (uint8_t *)sys_init_spd_svlbox.item[i], (uint32_t)strlen(sys_init_spd_svlbox.item[i]));

	// RAW : spd data
	if (spd_info[0].ctype == SPD_RAM_TYPE_DDR2)
	{
		spd_data_sz = 128 * sizeof(uint8_t) * spd_num;
	}
	else if ((spd_info[0].ctype == SPD_RAM_TYPE_DDR3) ||
			 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR3))
	{
		spd_data_sz = 256 * sizeof(uint8_t) * spd_num;
	}
	else if ((spd_info[0].ctype == SPD_RAM_TYPE_DDR4) ||
			 (spd_info[0].ctype == SPD_RAM_TYPE_DDR4E) ||
			 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR4) ||
			 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR4X))
	{
		spd_data_sz = 512 * sizeof(uint8_t) * spd_num;
	}

	pool = (uint8_t *)malloc(spd_data_sz * sizeof(uint8_t));
		
	for (i=0; i<spd_num; i++)
	{
		addr = spd_info[i].addr / 2;	// 8-bit -> 7-bit

		if (spd_info[i].ctype == SPD_RAM_TYPE_DDR2)
		{
			if (pool)
			{
				for (j=0; j<128; j++)
					pool[i*128+j] = imc_read_byte(addr, (uint8_t)j);
			}
		}
		else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR3) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR3))
		{
			if (pool)
			{
				for (j=0; j<256; j++)
					pool[i*256+j] = imc_read_byte(addr, (uint8_t)j);
			}
		}
		else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR4) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_DDR4E) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4) ||
				 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4X))
		{
			if (pool)
			{
				// page 0
				imc_set_spd_page(0);
				for (j=0; j<256; j++)
					pool[i*512+j] = imc_read_byte(addr, (uint8_t)j);
				// page 1
				imc_set_spd_page(1);
				for (j=0; j<256; j++)
					pool[i*512+256+j] = imc_read_byte(addr, (uint8_t)j);
			}
		}
	}

	dat_raw_add(RAW_ID_SMB_SPD, (uint8_t *)pool, spd_data_sz);	
	free(pool);

	// free item name
	for (i=0; i<spd_num; i++)
		free(sys_init_spd_svlbox.item[i]);
	free(sys_init_spd_svlbox.item);


	SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_IMC_SPD);
	
	//-----------------------------------------------------------------
	//  SPD CRC checking
	//-----------------------------------------------------------------
	key = sys_init_imc_spd_crc_check(spd_info[sel].addr);

	if (key == (SCAN_ESC << 8))
		return PU_ERR_ESC_ABORT;
	
	return PU_OK;
}

//=============================================================================
//  sys_init_memory_spd_crc_check
//=============================================================================
uint16_t	sys_init_memory_spd_crc_check(uint8_t addr)
{
	char		*hb_item[2] = { "Yes", "No" };
	uint8_t		ret, ddrt;
	t_hlbox_t	hb;
	char		title[64];
	char		msg[256];
	int			i;
	uint8_t		buf[256];
	uint16_t	crc = 0, crc_spd = 0;
	uint16_t		res;
	
	

	hb.title	= "  Would you like to check SPD CRC ?  ";
	hb.item		= hb_item;
	hb.help		= NULL;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = 0;

	hb.fg = T_WHITE;
	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;
	hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;
	hb.bh = T_MAGENTA;

	hb.hm = 2;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 4;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel		= 1;	// default : no

	ret = t_show_hlbox(&hb);
	
	// CRC checking
	if (hb.sel == 0)	// 0=yes
	{
		sprintf(title, "%02Xh : SPD", addr);
		sprintf(msg, "Reading......0x%02X", 0);
		msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

		// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
		smb->quick_command(0x6C);

		for (i=0; i<256; i++)
		{
			sprintf(msg, "Reading......0x%02X", i);
			msgbox_progress_cont(title, msg);

			buf[i] = smb->read_byte_data(addr, i);
		}
		msgbox_progress_done();
		key_flush_buf();

		ddrt = 0;

		//-----------------------------------------------------
		//  DDR3
		//-----------------------------------------------------
		if ((buf[2] == SPD_RAM_TYPE_DDR3) ||
			(buf[2] == SPD_RAM_TYPE_LPDDR3))
		{
ddr3_crc_again:
			// crc size : 117/126 bytes, depends on buf[0].bit7 (0=117B, 1=126B)
			if (buf[0] & 0x80)
			{
				crc = spd_get_crc16((uint8_t*)buf, 117);
				crc_spd = *(uint16_t *)&buf[126];
				sprintf(msg, "SPD (CRC:00~74) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
			}
			else
			{
				crc = spd_get_crc16((uint8_t*)buf, 126);
				crc_spd = *(uint16_t *)&buf[126];
				sprintf(msg, "SPD (CRC:00~7D) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
			}


			if (crc == crc_spd)
			{
				sprintf(title, "%02X : DDR3 SPD CRC OK", addr);
				res = msgbox_waitkey(T_WHITE, T_BLUE, title, msg, 4, 1);
			}
			else
			{
				sprintf(title, "%02X : DDR3 SPD CRC Error", addr);
				res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
			}

			ddrt = 3;
		}
		//-----------------------------------------------------
		//  DDR4
		//-----------------------------------------------------
		else if ((buf[2] == SPD_RAM_TYPE_DDR4) ||
				 (buf[2] == SPD_RAM_TYPE_LPDDR4) ||
				 (buf[2] == SPD_RAM_TYPE_LPDDR4X) ||
				 (buf[2] == SPD_RAM_TYPE_DDR4E))
		{
ddr4_crc_again:
			// crc size : 126 bytes fixed
			crc = spd_get_crc16((uint8_t*)buf, 126);
			crc_spd = *(uint16_t *)&buf[126];
			sprintf(msg, "SPD (CRC:00~7D) : 0x%04X\n\nSPD (CRC@7F:7E) : 0x%04X", crc, crc_spd);
	
			if (crc == crc_spd)
			{
				sprintf(title, "%02X : DDR4 SPD CRC OK", addr);
				res = msgbox_waitkey(T_WHITE, T_BLUE, title, msg, 4, 1);

			}
			else
			{
				sprintf(title, "%02X : DDR4 SPD CRC Error", addr);
				res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
			}

			ddrt = 4;
		}
		else
		{
			sprintf(title, "%02X : SPD Type Error", addr);
			sprintf(msg,   "SPD type : %02Xh", buf[2]);
			res = msgbox_waitkey(T_WHITE, T_RED, title, msg, 4, 1);
		}

		key_flush_buf();

		if (res == (SCAN_ESC << 8))
			return res;

		//-----------------------------------------------------
		//  SPD shown
		//-----------------------------------------------------
		if (ddrt == 3)
		{
			if (buf[0] & 0x80)
				sprintf(msg, "DDR3 SPD : CRC(00~74)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);
			else
				sprintf(msg, "DDR3 SPD : CRC(00~7D)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);

			res = sys_init_memory_spd_show(addr, buf, msg);
			if (res == (SCAN_ESC << 8))
				goto ddr3_crc_again;
		}
		else if (ddrt == 4)
		{
page0_again:
			// Page 0
			sprintf(msg, "DDR4 SPD : Page 0 > CRC(00~7D)=0x%04X  CRC(7F:7E)=0x%04X", crc, crc_spd);
			res = sys_init_memory_spd_show(addr, buf, msg);
			if (res == (SCAN_ESC << 8))
				goto ddr4_crc_again;
			
			// Page 1
			// send quick command 6Eh to switch to page 1 in case of DDR4 DRAM
			smb->quick_command(0x6E);

			sprintf(title, "%02Xh : SPD", addr);
			sprintf(msg, "Reading......0x%02X", 0);
			msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

			for (i=0; i<256; i++)
			{
				sprintf(msg, "Reading......0x%02X", i);
				msgbox_progress_cont(title, msg);

				buf[i] = smb->read_byte_data(addr, i);
			}
			msgbox_progress_done();
			key_flush_buf();
			
			// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
			smb->quick_command(0x6C);
		
			res = sys_init_memory_spd_show(addr, buf, "DDR4 SPD : Page 1");
			if (res == (SCAN_ESC << 8))
			{
				// Page 0
				// send quick command 6Ch to switch to page 0 in case of DDR4 DRAM
				smb->quick_command(0x6C);

				sprintf(title, "%02Xh : SPD", addr);
				sprintf(msg, "Reading......0x%02X", 0);
				msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

				for (i=0; i<256; i++)
				{
					sprintf(msg, "Reading......0x%02X", i);
					msgbox_progress_cont(title, msg);

					buf[i] = smb->read_byte_data(addr, i);
				}
				msgbox_progress_done();
				key_flush_buf();

				goto page0_again;
			}
		}
	}
	else
	{
		// no checking CRC
		return PU_OK;
	}

	return ret;		// 0=yes, !0=no
}

//=============================================================================
//  sys_init_memory_spd_check_yes_no
//=============================================================================
uint8_t	sys_init_memory_spd_check_yes_no(void)
{
	char		*hb_item[2] = { "Yes", "No" };
	uint8_t		ret;
	t_hlbox_t	hb;

	hb.title	= "  Would you like to check RAM SPD ?  ";
	hb.item		= hb_item;
	hb.help		= NULL;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = 0;

	hb.fg = T_WHITE;
	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;
	hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;
	hb.bh = T_MAGENTA;

	hb.hm = 2;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 4;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel		= 0;

	ret = t_show_hlbox(&hb);

	return ret;		// 0=yes, !0=no
}

//=============================================================================
//  sys_init_memory_spd_check
//=============================================================================
uint8_t	sys_init_memory_spd_check(void)
{
	uint8_t		res;
	uint8_t		addr;
	char		title[64];
	char		msg[128];
	char		item[80];
	uint8_t		spd[4];
	uint8_t		buf[256];
	uint8_t		spd_addr[8];
	uint8_t		ax_num, ax_addr[8];
	uint8_t		spd_num;
	uint8_t		sel;
	int			d, i, j;
	uint16_t	crc117, crc126;
	uint8_t		*pool;
	uint16_t		key;
	uint32_t	spd_data_sz = 0;

	pci_dev_t	*pd;
	spd_info_t	spd_info[8];	// max num = 8

	sys_init_spd_svlbox.title = "RAM Module SPD";
	sys_init_spd_svlbox.field = NULL;
	sys_init_spd_svlbox.item	= NULL;
	sys_init_spd_svlbox.help	= NULL;
	
	sys_init_spd_svlbox.wh = 16;
	sys_init_spd_svlbox.num_item = 0;
	sys_init_spd_svlbox.num_help = 0;
	
	sys_init_spd_svlbox.fg = T_WHITE;
	sys_init_spd_svlbox.bg = T_MAGENTA;
	sys_init_spd_svlbox.ft = T_WHITE;
	sys_init_spd_svlbox.bt = T_MAGENTA;
	sys_init_spd_svlbox.fi = T_WHITE;
	sys_init_spd_svlbox.bi = T_MAGENTA;
	sys_init_spd_svlbox.ff = T_WHITE;
	sys_init_spd_svlbox.bf = T_MAGENTA;
	sys_init_spd_svlbox.fn = T_WHITE;
	sys_init_spd_svlbox.bn = T_MAGENTA;

	sys_init_spd_svlbox.fs = T_YELLOW;
	sys_init_spd_svlbox.bs = T_BLUE;

	sys_init_spd_svlbox.fh = T_YELLOW;
	sys_init_spd_svlbox.bh = T_MAGENTA;
	
	sys_init_spd_svlbox.tm = 0;
	sys_init_spd_svlbox.bm = 0;
	sys_init_spd_svlbox.lm = 1;
	sys_init_spd_svlbox.rm = 1;
	
	sys_init_spd_svlbox.align = T_AL_CM;
	sys_init_spd_svlbox.ho = 0;
	sys_init_spd_svlbox.vo = 0;
	sys_init_spd_svlbox.ta = T_AL_CENTER;
	sys_init_spd_svlbox.ia = T_AL_LEFT;
	sys_init_spd_svlbox.ha = T_AL_CENTER;

	sys_init_spd_svlbox.flag	= ( T_VLBOX_FL_TITLE   |
				T_VLBOX_FL_KEYHELP |
				T_VLBOX_FL_SHADOW  |
				T_VLBOX_FL_FRAME );
				
	sys_init_spd_svlbox.sel	= 0;


	spd_ddr_ht.sx = 4;
	spd_ddr_ht.sy = 1;
	spd_ddr_ht.hm = 2;
	spd_ddr_ht.vm = 2;
	
	spd_ddr_ht.fg = T_LIGHTGRAY;
	spd_ddr_ht.bg = T_BLUE;
	spd_ddr_ht.ft = T_YELLOW;
	spd_ddr_ht.bt = T_BLUE;
	spd_ddr_ht.fi = T_LIGHTRED;
	spd_ddr_ht.bi = T_BLUE;

	//title
	spd_ddr_ht.flag	= ( T_HEX_FL_ASC |		// with ascii data
				T_HEX_FL_IDX |		// with index
				T_HEX_FL_FRM );		// with frame
				//T_HEX_FL_SHADOW;	// without shadow

	spd_ddr_ht.align	= T_AL_CM;


	scr_clear_keyhlp();

	// SMBus type : IMC SMBus
	if (imc_init() == IMC_OK)
		return PU_STS_IMC_FOUND;

	// SMBus type : ICH SMBus
	res = pci_init();

	if (res != PCI_OK)
	{
		sprintf(msg, "Scanning PCI bus error! code:%d", res);
		msgbox_waitkey(T_WHITE, T_RED, "PCI", msg, 4, 1);
		return PU_ERR_SCAN_PCI;
	}

	// SMBus Controller present ?
	pd = pci_find_class(0x0C, 0x05, 0x00);	// 0C/05/00 : SMBus controller
	if (pd == NULL)
	{
		msgbox_waitkey(T_WHITE, T_RED, "SMBus", "SMBus controller not found!", 4, 1);
		return PU_ERR_NO_SMB_CTLR;
	}

	// 8086:Intel, 1022:AMD, 1002:ATI (combined with AMD)
	if (!(pd->cfg.ven_id == 0x8086 || pd->cfg.ven_id == 0x1022 || pd->cfg.ven_id == 0x1002))
	{
		msgbox_waitkey(T_WHITE, T_RED, "SMBus", "Only support Intel or AMD SMBus controller!", 4, 1);
		return SMB_ERR_CTLR_NOT_SUP;
	}
	
	res = smb_init();

	if (res != SMB_OK)
	{
		msgbox_waitkey(T_WHITE, T_RED, "SMBus", "SMBus controller init error!", 4, 1);
		return SMB_ERR_NULL_BUS;
	}

	addr = 0xA0;

	// detect SPD
	spd_num = 0;
	//msgbox_progress(T_WHITE, T_MAGENTA, "RAM Module SPD", "Detecting SPD......", 4, 1);

	sprintf(title, "RAM Module SPD : %02X", 0xa0);
	sprintf(msg, "%s", "Detecting SPD......");
	msgbox_progress(T_BLACK, T_CYAN, title, msg, 4, 1);
	
	for (addr=0xA0, ax_num=0; addr<0xB0  ; addr+=2)
	{
		sprintf(title, "RAM Module SPD : %02X", addr);
		msgbox_progress_cont(title, msg);

		res = smb_scan_dev(addr);

		if (res != SMB_OK)
			continue;

		ax_addr[ax_num++] = addr;
	}

	msgbox_progress_done();
	key_flush_buf();

	if (ax_num == 0)
	{
		// There are some devices @ BDF=ff:xx:yy in intel server platform
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_INTEL_CPU) && pci_find_bus(0xFF))
		{
			msgbox_waitkey(T_WHITE, T_RED, "Warning", "Sorry, we can not detect RAM module SPD in this platform!", 4, 1);
			return PU_ERR_CANT_SUPPORT_SPD;
		}
		else
		{
			// todo : need to stop test ?
			//msgbox_waitkey(T_WHITE, T_RED, "Error", "Can not detect any RAM module SPD, please ask for help!", 4, 1);
			msgbox_waitkey(T_WHITE, T_RED, "Error", "Can not detect any RAM module SPD, skip SPD checking!", 4, 1);
			return PU_ERR_NO_SPD_FOUND;
		}
	}

	msgbox_progress(T_WHITE, T_MAGENTA, "RAM Module SPD", "Reading SPD......", 4, 1);

	for (d=0; d<ax_num; d++)
	{
		addr = ax_addr[d];
		
		// spd : first 4 bytes for probe spd
		if (spd_dev_check(addr, spd) < 0)
			continue;	// not spd
	
		memcpy(buf, spd, 4);

		if ((spd[2] == SPD_RAM_TYPE_DDR4) ||
			(spd[2] == SPD_RAM_TYPE_DDR4E) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4X))
		{
			//-----------------------------------------------------------------
			//  DDR4
			//-----------------------------------------------------------------
			// ddr4 : page 0
			buf[4] = smb->read_byte_data(addr, 0x04);	// chip size
			buf[5] = smb->read_byte_data(addr, 0x0C);	// module organ
			buf[8] = smb->read_byte_data(addr, 0x0E);	// thermal sensor
			buf[9] = smb->read_byte_data(addr, 0x12);	// cycle time
			buf[10]= smb->read_byte_data(addr, 0x0D);	// ecc support

			// ddr4 : page 1
			smb->quick_command(0x6E);
			buf[6] = smb->read_byte_data(addr, 0x41);	// msb of manufacturer id : 0x141
			buf[7] = smb->read_byte_data(addr, 0x40);	// lsb of manufacturer id : 0x140

			// manufacturer part number : DDR4 size = 20 : 0x149h~0x15Ch
			for (i=0; i<20; i++)
			{
				buf[11+i] = smb->read_byte_data(addr, 0x49+i);

				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}

		}
		else if ((spd[2] == SPD_RAM_TYPE_DDR3) ||
				 (spd[2] == SPD_RAM_TYPE_LPDDR3))
		{
			//-----------------------------------------------------------------
			//  DDR3
			//-----------------------------------------------------------------
			buf[4] = smb->read_byte_data(addr, 0x04);	// chip size
			buf[5] = smb->read_byte_data(addr, 0x07);	// module organ
			buf[6] = smb->read_byte_data(addr, 0x76);	// msb of manufacturer id
			buf[7] = smb->read_byte_data(addr, 0x75);	// lsb of manufacturer id
			// [06h] bit1 : 1=DDR3L (1.5V/1.35V)
			buf[8] = smb->read_byte_data(addr, 0x06);	// ddr3 : without thermal sensor, with ddr3 low voltage support 
			buf[9] = smb->read_byte_data(addr, 0x0C);	// cycle time
			buf[10]= smb->read_byte_data(addr, 0x08);	// ecc support

			for (i=0; i<18; i++)
			{
				buf[11+i] = smb->read_byte_data(addr, 0x80+i);	// model : manufacturer part number
				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}
		}
		else if (spd[2] == SPD_RAM_TYPE_DDR2)
		{
			//-----------------------------------------------------------------
			//  DDR2
			//-----------------------------------------------------------------
			buf[3] = smb->read_byte_data(addr, 0x14);	// DIMM type
			buf[4] = smb->read_byte_data(addr, 0x05);	// num of banks(rank)
			buf[5] = smb->read_byte_data(addr, 0x1F);	// 
			buf[6] = smb->read_byte_data(addr, 0x41);	// msb of manufacturer id
			buf[7] = smb->read_byte_data(addr, 0x40);	// lsb of manufacturer id
			buf[8] = 0;								// ddr2 : without thermal sensor
			buf[9] = smb->read_byte_data(addr, 0x9);		// cycle time
			buf[10]= smb->read_byte_data(addr, 0xB);
			for (i=0; i<18; i++)
			{
				buf[11+i] = smb->read_byte_data(addr, 0x49+i);	// model : manufacturer part number
				// there is 00h in the sucking dram module of module part number.
				// replace 00h to '_' for avoid part number showing
				if (buf[11+i] == 0)
					buf[11+i] = '_';
			}
		}

		//-----------------------------------------------------------------
		//  spd info
		//-----------------------------------------------------------------
		if ((spd[2] == SPD_RAM_TYPE_DDR2) ||
			(spd[2] == SPD_RAM_TYPE_DDR3) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR3) ||
			(spd[2] == SPD_RAM_TYPE_DDR4) ||
			(spd[2] == SPD_RAM_TYPE_DDR4E) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4) ||
			(spd[2] == SPD_RAM_TYPE_LPDDR4X))
		{
			spd_get_info(&spd_info[spd_num], buf);

			spd_info[spd_num].addr = addr;
			spd_addr[spd_num] = addr;

			spd_num++;
		}
		else
		{
			sprintf(msg, "This is not SPD data : spd[2]=%02X, please ask for help.", spd[2]);
			msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);
		}

	}

	msgbox_progress_done();
	key_flush_buf();
	
	if (spd_num == 0)
	{
		if (ax_num > 0)
		{
			j = 0;
			strcpy(&msg[j], "Address : ");
			j = (int)strlen(msg);

			for (i=0; i<ax_num; i++)
			{
				sprintf(&msg[j], "%02X ", ax_addr[i]);
				j += 3;
			}
			msg[j] = 0;
			
			sprintf(&msg[j], " found!\n\nThe SPD data might be broken!");

			msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);
			key_flush_buf();

			for (i=0; i<ax_num; i++)
			{
				sprintf(title, "%02Xh : EEPROM", ax_addr[i]);
				sprintf(msg, "Reading......0x%02X", 0);
				msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

				for (j=0; j<256; j++)
				{
					sprintf(msg, "Reading......0x%02X", j);
					msgbox_progress_cont(title, msg);

					buf[j] = smb->read_byte_data(ax_addr[i], j);
				}
				msgbox_progress_done();
				key_flush_buf();
				
				crc117 = spd_get_crc16((uint8_t*)buf, 117);
				crc126 = spd_get_crc16((uint8_t*)buf, 126);

				sprintf(msg, "Broken SPD CRC 117 bytes : 0x%04X\n\nBroken SPD CRC 126 bytes : 0x%04X", crc117, crc126);
				msgbox_waitkey(T_WHITE, T_RED, "SPD CRC", msg, 4, 1);
				key_flush_buf();
			
				// correct checksum
				buf[0] = 0x92;
				crc117 = spd_get_crc16((uint8_t*)buf, 117);
				crc126 = spd_get_crc16((uint8_t*)buf, 126);

				buf[0] = 0;
				sprintf(msg, "SPD_Byte[00h] = 92h\n\nBroken SPD CRC 117 bytes : 0x%04X\n\nBroken SPD CRC 126 bytes : 0x%04X", crc117, crc126);
				msgbox_waitkey(T_WHITE, T_RED, "SPD CRC correction", msg, 4, 1);
				key_flush_buf();

				sys_init_memory_spd_err_show(ax_addr[i], buf);
			}
		}
		else
		{
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_INTEL_CPU) && pci_find_bus(0xFF))
			{
				msgbox_waitkey(T_WHITE, T_RED, "Warning", "Sorry, we can not detect RAM module SPD in this platform!", 4, 1);
				return PU_ERR_CANT_SUPPORT_SPD;
			}
			else
			{
				// todo : need to stop test ?
				msgbox_waitkey(T_WHITE, T_RED, "Error", "Can not detect any RAM module SPD, please ask for help!", 4, 1);
				return PU_ERR_NO_SPD_FOUND;
			}
		}
	}
	else
	{

		//-----------------------------------------------------------------
		//  VLBOX : sys_init_spd_svlbox
		//-----------------------------------------------------------------
		sys_init_spd_svlbox.item = (char **)malloc(sizeof(char*) * spd_num);
		if (!sys_init_spd_svlbox.item)
			return PU_ERR_MALLOC;
	
		for (i=0; i<spd_num; i++)
		{
			item[0] = 0;

			if (spd_info[i].ctype == SPD_RAM_TYPE_DDR2)
			{
				sprintf(item, "%02X: %s-%s %s%s  %d MB : %s %s",
					spd_info[i].addr,
					spd_find_ram_type(spd_info[i].ctype),
					spd_find_ddr2_cycle_time(spd_info[i].cytime),
					spd_find_ddr2_ecc(spd_info[i].ecc),
					spd_find_ddr2_module_type(spd_info[i].mtype),
					spd_info[i].mbsz,
					spd_find_manu_id(spd_info[i].mid),
					spd_info[i].model);
			}
			else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR3) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR3))
			{
				sprintf(item, "%02X: %s%s-%s %s%s  %d MB : %s %s",
					spd_info[i].addr,
					spd_find_ram_type(spd_info[i].ctype),
					spd_find_ddr3_volt(spd_info[i].flag),
					spd_find_ddr3_cycle_time(spd_info[i].cytime),
					spd_find_ddr3_ecc(spd_info[i].ecc),
					spd_find_module_type(spd_info[i].mtype),
					spd_info[i].mbsz,
					spd_find_manu_id(spd_info[i].mid),
					spd_info[i].model);
			}
			else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR4) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_DDR4E) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4X))
			{
				sprintf(item, "%02X: %s-%s %s%s  %d MB : %s %s",
					spd_info[i].addr,
					spd_find_ram_type(spd_info[i].ctype),
					spd_find_ddr4_cycle_time(spd_info[i].cytime),
					spd_find_ddr4_ecc(spd_info[i].ecc),
					spd_find_module_type(spd_info[i].mtype),
					spd_info[i].mbsz,
					spd_find_manu_id(spd_info[i].mid),
					spd_info[i].model);
			}

			sys_init_spd_svlbox.item[i] = (char*)malloc(strlen(item)+1);

			if (!sys_init_spd_svlbox.item[i])
				return PU_ERR_MALLOC;

			sprintf(sys_init_spd_svlbox.item[i], "%s", item);
		}

		sys_init_spd_svlbox.num_item	= spd_num;
		sys_init_spd_svlbox.sel		= 0;
		
		scr_show_back_confirm_keyhlp();

		sel = t_show_svlbox(&sys_init_spd_svlbox);


		if (sel == 0xFA)	// abort
		{
			// free item name
			for (i=0; i<spd_num; i++)
				free(sys_init_spd_svlbox.item[i]);
			free(sys_init_spd_svlbox.item);
	
			return 0xFA;	// return function
		}

		//-----------------------------------------------------------------
		//  RAW
		//-----------------------------------------------------------------
		// RAW : spd num
		dat_raw_add(RAW_ID_SMB_SPD_NUM, (uint8_t *)&spd_num, sizeof(uint8_t));

		// RAW : spd addr
		dat_raw_add(RAW_ID_SMB_SPD_LST, (uint8_t *)spd_addr, sizeof(uint8_t)*spd_num);

		// RAW : spd model
		for (i=0; i<spd_num; i++)
			dat_raw_add(RAW_ID_SPD_MODEL+i, (uint8_t *)sys_init_spd_svlbox.item[i], (uint32_t)strlen(sys_init_spd_svlbox.item[i]));

		// RAW : spd data
		
		if (spd_info[0].ctype == SPD_RAM_TYPE_DDR2)
		{
			spd_data_sz = 128 * sizeof(uint8_t) * spd_num;
		}
		else if ((spd_info[0].ctype == SPD_RAM_TYPE_DDR3) ||
				 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR3))
		{
			spd_data_sz = 256 * sizeof(uint8_t) * spd_num;
		}
		else if ((spd_info[0].ctype == SPD_RAM_TYPE_DDR4) ||
				 (spd_info[0].ctype == SPD_RAM_TYPE_DDR4E) ||
				 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR4) ||
				 (spd_info[0].ctype == SPD_RAM_TYPE_LPDDR4X))
		{
			spd_data_sz = 512 * sizeof(uint8_t) * spd_num;
		}

		pool = (uint8_t *)malloc(spd_data_sz * sizeof(uint8_t));
		
		for (i=0; i<spd_num; i++)
		{
			if (spd_info[i].ctype == SPD_RAM_TYPE_DDR2)
			{
				if (pool)
				{
					for (j=0; j<128; j++)
						pool[i*128+j] = smb->read_byte_data(spd_info[i].addr, (uint8_t)j);
				}
			}
			else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR3) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR3))
			{
				if (pool)
				{
					for (j=0; j<256; j++)
						pool[i*256+j] = smb->read_byte_data(spd_info[i].addr, (uint8_t)j);
				}
			}
			else if ((spd_info[i].ctype == SPD_RAM_TYPE_DDR4) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_DDR4E) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4) ||
					 (spd_info[i].ctype == SPD_RAM_TYPE_LPDDR4X))
			{
				if (pool)
				{
					// page 0
					smb->quick_command(0x6C);
					for (j=0; j<256; j++)
						pool[i*512+j] = smb->read_byte_data(spd_info[i].addr, (uint8_t)j);

					// page 1
					smb->quick_command(0x6E);
					for (j=0; j<256; j++)
						pool[i*512+256+j] = smb->read_byte_data(spd_info[i].addr, (uint8_t)j);
				}
			}
		}

		dat_raw_add(RAW_ID_SMB_SPD, (uint8_t *)pool, spd_data_sz);	
		free(pool);

		// free item name
		for (i=0; i<spd_num; i++)
			free(sys_init_spd_svlbox.item[i]);
		free(sys_init_spd_svlbox.item);

		//-----------------------------------------------------------------
		//  SPD CRC checking
		//-----------------------------------------------------------------
		key = sys_init_memory_spd_crc_check(spd_info[sel].addr);

		if (key == (SCAN_ESC << 8))
			return PU_ERR_ESC_ABORT;
			
	}
	
	return PU_OK;
}

//=============================================================================
//  sys_init_memory_map_check
//=============================================================================
uint8_t	sys_init_memory_map_check(void)
{
	uint8_t		res;
	char		msg[128];
	uint8_t		*pool;
	e820_info_t	*e;

	int			i;
	uint16_t		key;

	scr_clear_keyhlp();

	msgbox_tmout(T_WHITE, T_BROWN, "Memory Map", "Getting INT15h, AX=E820h memory map...", 4, 1, 1000);

	res = e820_init();

	if (res != E820_OK)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Memory Map", "Getting INT15h, AX=E820h memory map error!", 4, 1);

		res = PU_ERR_E820;
	}
	else
	{
		pool = (uint8_t *)malloc(e820.num_item * 24);

		if (pool)
		{
			e = e820.item;
			for (i=0; (uint32_t)i<e820.num_item; i++)
			{
				memcpy(&pool[i*24], e, 24);
				e = e->next;
			}
			// RAW : E820
			dat_raw_add(RAW_ID_MEM_E820, pool, (uint32_t)(e820.num_item * 24));
		}
		free(pool);
		
		// RAW : E820 xxx
		dat_raw_add(RAW_ID_MEM_E820_NUM, (uint8_t *)&e820.num_item, sizeof(uint8_t));
		dat_raw_add(RAW_ID_MEM_E820_KSZ, (uint8_t *)&e820.kbsz, (uint32_t)sizeof(e820.kbsz));
		dat_raw_add(RAW_ID_MEM_E820_MSZ, (uint8_t *)&e820.mbsz, (uint32_t)sizeof(e820.mbsz));
		
		sprintf(msg, "Getting INT15h AX=E820h memory map ok!\n\n\b%cMemory Size : %d MiB", T_ARG_LIGHTGREEN, e820.mbsz);

		scr_show_back_confirm_keyhlp();

		while (1)
		{
			key = msgbox_waitkey(T_WHITE, T_BROWN, "Memory Map", msg, 4, 1);

			if (key == (SCAN_ESC << 8))
			{
				e820_exit();

				res = 0xFA;
				break;
			}
			else if (key == CHAR_CARRIAGE_RETURN)
			{
				res = PU_OK;
				break;
			}
		}
	}

	return res;
}

//=============================================================================
//  sys_init_acl_bios_check
//=============================================================================
uint8_t	sys_init_acl_bios_check(void)
{
	int		ret;
	uint8_t	res;
	char	*pch;
	char	*delim = " ";
	int		j, n, len;
	char	str[256];
	char	msg[256];
	char	magic[256];
	uint8_t	col;
	uint16_t key;

	scr_clear_keyhlp();

	//-------------------------------------------------------------------------
	//  ABIOS
	//-------------------------------------------------------------------------
	ret = abios_init();

	if (ret < 0)
	{
		// RAW : ACL Model
		sprintf(abios.model, "%s", "NON-ACL-MB");
		dat_raw_add(RAW_ID_ACL_BIOS_MODEL, (uint8_t*)abios.model, sizeof(abios.model));
		
		sprintf(abios.ver, "%s", "NON-ACL-VER");
		dat_raw_add(RAW_ID_ACL_BIOS_VER, (uint8_t*)abios.ver, sizeof(abios.ver));
		
		while (1)
		{
			key = msgbox_waitkey(T_WHITE, T_RED, "ACL BIOS", "This is non-Advantech motherboard", 4, 1);

			if (key == (SCAN_ESC << 8))
			{
				res = PU_ERR_ESC_ABORT;
				break;
			}
			else if (key == CHAR_CARRIAGE_RETURN)
			{
				res = PU_ERR_GET_ABIOS;
				break;
			}
		}
	}
	else
	{
		// copy another pool for strtok() due to modification
		// strtok() will pad a null to replace delim
		memcpy(magic, abios.magic, abios.len);
		magic[abios.len] = 0;

		pch = strtok(magic, delim);
		j = 0;
		n = 0;
		//msg[0] = '\r';
		while (pch != NULL)
		{
			if (n == 1)
				col = T_ARG_YELLOW;
			else if (n == 3)
				col = T_ARG_LIGHTGREEN;
			else
				col = T_WHITE;

			sprintf(str, "\r\b%c%s\n", col, pch);
			sprintf(&msg[j], "%s", str);
			len = (int)strlen(str);
			if ((j+len) >= 256)
				break;
			else
				j += len;
			
			//if (n == 1)	// model name
			//	sprintf(&msg[j], "\b%c%s\n\r", T_ARG_YELLOW, pch);
			//else if (n == 3)	// bios version
			//	sprintf(&msg[j], "\b%c%s\n\r", T_ARG_LIGHTGREEN, pch);
			//else
			//	sprintf(&msg[j], "%s\n\r", pch);
			
			//j += strlen(pch) + 2;	// +2: 0x0a 0x0d
	
			pch = strtok(NULL, delim);
			n++;
		}

		dat_raw_add(RAW_ID_ACL_BIOS, (uint8_t *)abios.magic, abios.len);
		dat_raw_add(RAW_ID_ACL_BIOS_ADDR, (uint8_t *)&abios.addr32, sizeof(uint32_t));
		dat_raw_add(RAW_ID_ACL_BIOS_MODEL, (uint8_t*)abios.model, sizeof(abios.model));
		dat_raw_add(RAW_ID_ACL_BIOS_VER, (uint8_t*)abios.ver, sizeof(abios.ver));
		
		scr_show_back_confirm_keyhlp();

		while (1)
		{
			key = msgbox_waitkey(T_WHITE, T_MAGENTA, "ACL BIOS", msg, 4, 1);

			if (key == (SCAN_ESC << 8))
			{
				res = PU_ERR_ESC_ABORT;
				break;
			}
			else if (key == CHAR_CARRIAGE_RETURN)
			{
				res = PU_OK;
				break;
			}
		}
	}

	abios_exit();

	return res;
}

//=============================================================================
//  sys_init_acl_ec_therm_vlbox
//=============================================================================
t_vlbox_t	sys_init_acl_ec_therm_vlbox;

//=============================================================================
//  sys_init_acl_ec_therm
//=============================================================================
static uint8_t sys_init_acl_ec_therm(void)
{
	rdc_fw_prot_t	fprot;
	//char			str[64];
	char			*item[5];
	uint8_t			therm_num;
	uint8_t			buf[8];
	uint8_t			therm_ch[4];
	uint8_t			therm_typ[4];
	int				i;
	uint8_t			ret, ret2;

	sys_init_acl_ec_therm_vlbox.title	= " Select Temperature Channel ";
	sys_init_acl_ec_therm_vlbox.item	= NULL;
	sys_init_acl_ec_therm_vlbox.help	= NULL;

	sys_init_acl_ec_therm_vlbox.num_item = 0;
	sys_init_acl_ec_therm_vlbox.num_help = 0;

	sys_init_acl_ec_therm_vlbox.fg = T_WHITE;
	sys_init_acl_ec_therm_vlbox.bg = T_MAGENTA;
	sys_init_acl_ec_therm_vlbox.fs = T_YELLOW;
	sys_init_acl_ec_therm_vlbox.bs = T_BLUE;
	sys_init_acl_ec_therm_vlbox.fh = T_YELLOW;
	sys_init_acl_ec_therm_vlbox.bh = T_MAGENTA;
	
	sys_init_acl_ec_therm_vlbox.hm = 1;
	sys_init_acl_ec_therm_vlbox.vm = 0;
	sys_init_acl_ec_therm_vlbox.ha = 0;
	sys_init_acl_ec_therm_vlbox.va = 0;
	
	sys_init_acl_ec_therm_vlbox.align	= T_AL_CM;
	sys_init_acl_ec_therm_vlbox.tialign= ((T_AL_CENTER<<4) | (T_AL_LEFT));
	sys_init_acl_ec_therm_vlbox.flag = (T_VLBOX_FL_TITLE | T_VLBOX_FL_KEYHELP | T_VLBOX_FL_SHADOW | T_VLBOX_FL_FRAME );
	sys_init_acl_ec_therm_vlbox.sel	= 0;
	
	// RDC_FW : thermal channel
	for (i=0; i<4; i++)
	{
		therm_ch[i] = 0xFF;
		therm_typ[i] = 0xFF;
	}
	
	for (i=0; i<5; i++)
	{
		item[i] = malloc(sizeof(uint8_t) * 128);
		if (!item[i])
			msgbox_waitkey(T_WHITE, T_RED, "Error", "malloc() err!", 4, 1);
	}

	sprintf(item[0], "%s", "   CPU : DTS temperature");
	
	therm_num = 0;
	for (i=0; i<4; i++)
	{
		fprot.cmd = RDC_FW_CMD_RD_THERM;
		fprot.ctl = 0x00;	// module status
		fprot.dev = (uint8_t)i;
		fprot.len = 2;
		fprot.dat = buf;

		ret = rdc_fw_prot_read(&fprot);
		if (ret == AEC_OK)
		{
			// debug
			//sprintf(str, "[%d] : %02X %02X", i, buf[0], buf[1]);
			//msgbox_waitkey(T_WHITE, T_RED, "Debug - module status", str, 4, 1);

			// device available
			if (buf[0] & 0x1)	
			{
				therm_num++;
				therm_ch[i] = (uint8_t)i;
				
				fprot.cmd = RDC_FW_CMD_RD_THERM;
				fprot.ctl = 0x01;	// device type
				fprot.dev = (uint8_t)i;
				fprot.len = 1;
				fprot.dat = buf;
				
				ret2 = rdc_fw_prot_read(&fprot);
				if (ret2 == AEC_OK)
				{
					therm_typ[i] = buf[0] & 0x1F;
					if ((buf[0] & 0x1F) < 4)
						sprintf(item[i+1], "RDC-EC : CPU%d temperature", buf[0] & 0x1F);
					else if ((buf[0] & 0x1F) < 8)
						sprintf(item[i+1], "RDC-EC : SYS%d temperature", buf[0] & 0x1F);
					else
						sprintf(item[i+1], "RDC-EC : UN%d  temperature", buf[0] & 0x1F);

					//msgbox_waitkey(T_YELLOW, T_RED, "Debug - thermal", item[i+1], 4, 1);
				}
				//else
				//{
					// debug
					//msgbox_waitkey(T_WHITE, T_RED, "Error", "Can not get thermal type!", 4, 1);
				//}
			}
		}
	}

	sys_init_acl_ec_therm_vlbox.item		= item;
	sys_init_acl_ec_therm_vlbox.num_item	= therm_num + 1;
	sys_init_acl_ec_therm_vlbox.sel			= 0;

	ret = t_show_vlbox(&sys_init_acl_ec_therm_vlbox);

#if 0
	if (ret == 0xFA)	// abort
	{
		sprintf(str, "Abort select!");
	}
	else
	{
		sprintf(str, "Select : %s", item[ret]);
	}
	msgbox_waitkey(T_BLACK, T_CYAN, "Temperature Channel", str, 4, 1);
#endif

	for (i=0; i<5; i++)
		free(item[i]);
	
	// x86 CPU
	if (ret != 0xFA)
	{
		if (ret == 0)
		{
			dat->par->therm &= ~0xF0;	// CPU
		}
		else if (ret < 5)	// max thermal source = 4
		{
			dat->par->therm &= ~0xF0;
			// bit [7:4] therm src
			dat->par->therm |= (1<<4);		// bit[7:4] EC, CPU

			// bit[3:2] therm type
			if (therm_typ[ret-1] > 8)
				dat->par->therm |= (3<<2);	// 3= ?
			else if (therm_typ[ret-1] > 4)
				dat->par->therm |= (1<<2);	// 1=SYS
			else
				dat->par->therm |= (0<<2);	// 0=CPU
	
			// bit[1:0] = therm ch
			dat->par->therm |= therm_ch[ret-1];	// THERM0~THERM3
		}
		else
		{
			dat->par->therm = 0;
		}
	}
	
	return ret;
}

//=============================================================================
//  sys_init_acl_ec_check
//=============================================================================
uint8_t	sys_init_acl_ec_check(void)
{
	rdc_fw_prot_t	fprot;
	rdc_sw_prot_t	sprot;
	ite_fw_prot_t	iprot;

	int		ret;
	char	ver[32];
	char	buf[32];
	char	cname[32];
	char	msg[256];
	char	found;
	uint8_t	res;
	uint8_t	len;
	uint16_t		ectype = 0, key;

	scr_clear_keyhlp();

	//-------------------------------------------------------------------------
	//  AEC
	//-------------------------------------------------------------------------
	aec = aec_init();
	if (!aec)
	{
		found = 0;
		res = PU_ERR_GET_AEC;
		goto err_acl_ec;
	}

	memset(msg, 0, sizeof(msg));
	memset(ver, 0, sizeof(ver));
	found = 1;

	// AEC type
	ectype = aec_get_type(aec);
	switch (ectype)
	{
		//---------------------------------------------------------------------
		//  ITE IT8518
		//---------------------------------------------------------------------
		case AEC_TYPE_ITE_8518:

			// TODO : need to confirm it is working at IT8518
			msgbox_doing(T_BLACK, T_LIGHTGRAY, "ACL EC", "Getting ITE IT8518 info.....", 4, 1);
	
			iprot.cmd = 0xF0;	// get version
			iprot.len = 17;
			iprot.dat = ver;
			ret = ite_ec_fw_prot_read(&iprot);

			msgbox_doing_done();
		
			if (ret != AEC_OK)
				sprintf(ver, "err! 0x%02X", ret);

			// TODO 
			memset(buf, 0, sizeof(buf));
			memcpy(buf, ver, 8);	// ver[0]~ver[7] : model name

			// RAW : aec chip id
			sprintf(msg, "ITE IT%04X", aec_get_chip_id(aec));
			dat_raw_add(RAW_ID_ACL_EC_CHIP, (uint8_t *)msg, (uint32_t)strlen(msg));

			// RAW : aec board
			dat_raw_add(RAW_ID_ACL_EC_BOARD, (uint8_t *)buf, (uint32_t)strlen(buf));
			sprintf(aec->fw.model, "%s", buf);

			// aec info
			memcpy(&aec->fw.ec.ite.tab_code, &ver[8], 9);	// len=9
			sprintf(msg, "ITE IT%04X\n\n\r Model : %s\n\r\b%cFW Ver : %c %02X.%02X\n\r\b%c FW ID : %02X\n\r  Chip : %c %02X\n\r Table : %02X\n\rKernel : %02X.%02X",
					aec_get_chip_id(aec),
					aec->fw.model,
					T_ARG_RED,
					aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
					T_ARG_BLACK,
					aec->fw.ec.ite.prj_id,
					aec->fw.ec.ite.chip_vendor, aec->fw.ec.ite.chip_id,
					aec->fw.ec.ite.tab_code,
					aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);

			// RAW : aec ver
			sprintf(buf, "%c%02X%02X_%02X%02X",
						aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
						aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);
			dat_raw_add(RAW_ID_ACL_EC_VER, buf, (uint32_t)strlen(buf));
		
			scr_show_back_confirm_keyhlp();

			break;

		//---------------------------------------------------------------------
		//  ITE IT8528
		//---------------------------------------------------------------------
		case AEC_TYPE_ITE_8528:

			msgbox_doing(T_BLACK, T_LIGHTGRAY, "ACL EC", "Getting ITE IT8528 info.....", 4, 1);
	
			iprot.cmd = 0xF0;	// get version
			iprot.len = 17;
			iprot.dat = ver;
			ret = ite_ec_fw_prot_read(&iprot);

			msgbox_doing_done();
		
			if (ret != AEC_OK)
				sprintf(ver, "err! 0x%02X", ret);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, ver, 8);	// ver[0]~ver[7] : model name

			// RAW : aec chip
			sprintf(msg, "ITE IT%04X", aec_get_chip_id(aec));
			dat_raw_add(RAW_ID_ACL_EC_CHIP, (uint8_t *)msg, (uint32_t)strlen(msg));

			// RAW : aec board
			dat_raw_add(RAW_ID_ACL_EC_BOARD, (uint8_t *)buf, (uint32_t)strlen(buf));

			sprintf(aec->fw.model, "%s", buf);

			memcpy(&aec->fw.ec.ite.tab_code, &ver[8], 9);	// len=9
			sprintf(msg, "ITE IT%04X\n\n\r Model : %s\n\r\b%cFW Ver : %c %02X.%02X\n\r\b%c FW ID : %02X\n\r  Chip : %c %02X\n\r Table : %02X\n\rKernel : %02X.%02X",
					aec_get_chip_id(aec),
					aec->fw.model,
					T_ARG_RED,
					aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
					T_ARG_BLACK,
					aec->fw.ec.ite.prj_id,
					aec->fw.ec.ite.chip_vendor, aec->fw.ec.ite.chip_id,
					aec->fw.ec.ite.tab_code,
					aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);

			// RAW : aec ver
			sprintf(buf, "%c%02X%02X_%02X%02X",
						aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
						aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);
			dat_raw_add(RAW_ID_ACL_EC_VER, buf, (uint32_t)strlen(buf));
		
			scr_show_back_confirm_keyhlp();

			break;

		//---------------------------------------------------------------------
		//  ITE IT5121
		//---------------------------------------------------------------------
		case AEC_TYPE_ITE_5121:

			msgbox_doing(T_BLACK, T_LIGHTGRAY, "ACL EC", "Getting ITE IT5121 info.....", 4, 1);
			ret = ite_mbox_read_fw_info(ver);
			msgbox_doing_done();
		
			if (ret != ITE_OK)
				sprintf(ver, "err! ret = 0x%02X", ret);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, ver, 8);	// ver[0]~ver[7] : model name

			// RAW : aec chip
			sprintf(msg, "ITE IT%04X", aec_get_chip_id(aec));
			dat_raw_add(RAW_ID_ACL_EC_CHIP, (uint8_t *)msg, (uint32_t)strlen(msg));

			// RAW : aec board
			dat_raw_add(RAW_ID_ACL_EC_BOARD, (uint8_t *)buf, (uint32_t)strlen(buf));

			sprintf(aec->fw.model, "%s", buf);

			memcpy(&aec->fw.ec.ite.tab_code, &ver[8], 9);	// len=9
			sprintf(msg, "ITE IT%04X\n\n\r Model : %s\n\r\b%cFW Ver : %c %02X.%02X\n\r\b%c FW ID : %02X\n\r  Chip : %c %02X\n\r Table : %02X\n\rKernel : %02X.%02X",
					aec_get_chip_id(aec),
					aec->fw.model,
					T_ARG_RED,
					aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
					T_ARG_BLACK,
					aec->fw.ec.ite.prj_id,
					aec->fw.ec.ite.chip_vendor, aec->fw.ec.ite.chip_id,
					aec->fw.ec.ite.tab_code,
					aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);

			// RAW : aec ver
			sprintf(buf, "%c%02X%02X_%02X%02X",
						aec->fw.ec.ite.type, aec->fw.ec.ite.ver_major, aec->fw.ec.ite.ver_minor,
						aec->fw.ec.ite.kver_major, aec->fw.ec.ite.kver_minor);
			dat_raw_add(RAW_ID_ACL_EC_VER, buf, (uint32_t)strlen(buf));
		
			scr_show_back_confirm_keyhlp();
	
			break;

		//---------------------------------------------------------------------
		//  RDC A9610 SW
		//---------------------------------------------------------------------
		case AEC_TYPE_RDC_SW:

			// RDC_SW : fw version
			sprot.cmd = RDC_SW_CMD_RD_FWVER;
			sprot.off = 0x00;
			sprot.len = 9;
			sprot.dat = ver;

			ret = rdc_sw_prot_noidx_read(&sprot);

			if (ret != AEC_OK)
				strcpy(ver, "err!");

			// RAW : aec chip
			sprintf(msg, "RDC A%04X", aec_get_chip_id(aec));
			dat_raw_add(RAW_ID_ACL_EC_CHIP, (uint8_t *)msg, (uint32_t)strlen(msg));

			// RAW : aec ver
			dat_raw_add(RAW_ID_ACL_EC_VER, (uint8_t *)ver, (uint32_t)strlen(ver));

			// RDC_SW : board name length
			sprot.cmd = RDC_SW_CMD_PLAT_INFO;
			sprot.off = 0x06;	// platform name length
			sprot.len = 1;
			sprot.dat = buf;
			res = rdc_sw_prot_noidx_read(&sprot);
			len = buf[0];

			if (res != AEC_OK)
			{
				sprintf(buf, "%s", "board err!");
			}
			else
			{
				memset(buf, 0, sizeof(buf));

				// board name string
				sprot.cmd = RDC_SW_CMD_PLAT_INFO;
				sprot.off = 0x07;	// platform name
				sprot.len = len;
				sprot.dat = buf;
				res = rdc_sw_prot_noidx_read(&sprot);

				if (res != AEC_OK)
				{
					sprintf(buf, "%s", "board err!");
				}
			}

			// RAW : aec board
			dat_raw_add(RAW_ID_ACL_EC_BOARD, (uint8_t *)buf, (uint32_t)strlen(buf));
			
			sprintf(msg, "RDC A%04X\n\n\rChip    : EIO-IS200\n\r\b%cBoard   : %s\n\r\b%cFW Type : RDC_SW\n\r\b%cFW Ver  : %s",
					aec_get_chip_id(aec), T_ARG_MAGENTA, buf, T_ARG_BLACK, T_ARG_RED, ver);

			scr_show_back_confirm_keyhlp();

			break;

		//---------------------------------------------------------------------
		//  RDC A9610 FW
		//---------------------------------------------------------------------
		case AEC_TYPE_RDC_FW:

			// RDC_FW : fw ver
			fprot.cmd = RDC_FW_CMD_RD_BOARD;
			fprot.ctl = 0x22;
			fprot.dev = 0;
			fprot.len = 16;
			fprot.dat = ver;

			ret = rdc_fw_prot_read(&fprot);


			if (ret != AEC_OK)
			{
				strcpy(ver, "err!");
			}
			else
			{
				// RAW : aec chip
				sprintf(msg, "RDC A%04X", aec_get_chip_id(aec));
				dat_raw_add(RAW_ID_ACL_EC_CHIP, (uint8_t *)msg, (uint32_t)strlen(msg));

				// RAW : aec ver
				dat_raw_add(RAW_ID_ACL_EC_VER, (uint8_t *)ver, (uint32_t)strlen(ver));
			}

			// RDC_FW : chip name
			fprot.cmd = RDC_FW_CMD_RD_BOARD;
			fprot.ctl = 0x12;	// chip name
			fprot.dev = 0;
			fprot.len = 12;
			fprot.dat = cname;

			ret = rdc_fw_prot_read(&fprot);

			if (ret != AEC_OK)
				strcpy(buf, "err!");

			// RDC_FW : board name
			fprot.cmd = RDC_FW_CMD_RD_BOARD;
			fprot.ctl = 0x10;	// board name
			fprot.dev = 0;
			fprot.len = 16;
			fprot.dat = buf;

			ret = rdc_fw_prot_read(&fprot);

			
			if (ret != AEC_OK)
			{
				strcpy(buf, "err!");
			}
			else
			{
				// RAW : aec board
				dat_raw_add(RAW_ID_ACL_EC_BOARD, (uint8_t *)buf, (uint32_t)strlen(buf));
			}

			sprintf(msg, "RDC A%04X\n\n\rChip    : %s\n\r\b%cBoard   : %s\n\r\b%cFW Type : RDC_FW\n\r\b%cFW Ver  : %s",
				aec_get_chip_id(aec), cname, T_ARG_MAGENTA, buf, T_ARG_BLACK, T_ARG_RED, ver);

			scr_show_back_confirm_keyhlp();

			break;
			
		default:
			found = 0;
			break;
	}


	
err_acl_ec:

	while (1)
	{
		if (found)
		{
			key = msgbox_waitkey(T_BLACK, T_LIGHTGRAY, "ACL EC", msg, 4, 1);
		}
		else
		{
			strcpy(msg, "ACL embedded controller not found!\n");
			key = msgbox_waitkey(T_WHITE, T_RED, "ACL EC", msg, 4, 1);
		}
		
		if (key == (SCAN_ESC << 8))
		{
			res = PU_ERR_ESC_ABORT;

			break;
		}
		else if (key == CHAR_CARRIAGE_RETURN)
		{
			if (found)
				res = PU_OK;
			else
				res = PU_ERR_GET_AEC;

			break;
		}
	}

	//-------------------------------------------------------------------------
	//  EC thermal
	//-------------------------------------------------------------------------
	if (ectype == AEC_TYPE_RDC_FW)
	{
		if (sys_init_acl_ec_therm() == 0xFA)
			res = PU_ERR_ESC_ABORT;
	}

	// todo : needed ?
	aec_exit(aec);

	return res;
}

//=============================================================================
//  sys_init_smbios_check
//=============================================================================
uint8_t	sys_init_smbios_check(void)
{
	uint8_t		ret;
	char		title[32];
	uint8_t		msg[512];
	uint8_t		tag[8];
	uint32_t	addr;
	uint8_t		*addr8;
	int			i, idx;
	uint8_t		res;
	uint16_t key;

	scr_clear_keyhlp();

	//-------------------------------------------------------------------------
	//  SMBIOS
	//-------------------------------------------------------------------------
	ret = smbios_init();

	if (ret)
	{
		msgbox_waitkey(T_WHITE, T_RED, "SMBIOS", "Getting SMBIOS error!", 4, 1);

		return PU_ERR_GET_SMBIOS;
	}
	else
	{
		addr = *(uint32_t*)smbios.hdr->sig;

		// RAW : smbios hdr
		dat_raw_add(RAW_ID_DMI_SMBIOS_HDR,  (uint8_t *)smbios.hdr, sizeof(smbios_hdr_t));

		// RAW : smbios addr
		dat_raw_add(RAW_ID_DMI_SMBIOS_ADDR, (uint8_t *)&addr, sizeof(uint32_t));

		for (i=0; i<4; i++)
			tag[i] = smbios.hdr->sig[i];
		tag[4] = 0;

		sprintf(msg, "\rTag      : %s\n\rAddr     : 0x%08X\n\rLen      : %d = %X\n\rVer      : %d.%d\n\rDMI Addr : 0x%08X\n\rDMI Size : %X = %d\n\rDMI Num  : %d",
				tag,
				addr,
				smbios.hdr->len, smbios.hdr->len,
				smbios.hdr->major_ver, smbios.hdr->minor_ver,
				smbios.hdr->smbios_addr,
				smbios.hdr->smbios_len, smbios.hdr->smbios_len,
				smbios.hdr->smbios_ent_num);

		sprintf(title, "%s", "SMBIOS");
		//key = msgbox_waitkey(T_BLACK, T_CYAN, "SMBIOS", msg, 4, 1);

		if (smbios.v3hdr != NULL)
		{
			addr = *(uint32_t*)smbios.v3hdr->sig;

			// RAW : smbios v3 hdr
			dat_raw_add(RAW_ID_DMI_SMBIOS_V3HDR, (uint8_t *)smbios.v3hdr, sizeof(smbios_v3hdr_t));

			// RAW : smbios v3 addr
			dat_raw_add(RAW_ID_DMI_SMBIOS_V3ADDR, (uint8_t *)&addr, sizeof(uint32_t));

			for (i=0; i<5; i++)
				tag[i] = smbios.v3hdr->sig[i];
			tag[5] = 0;
			
			idx = (int)strlen(msg);

			sprintf(&msg[idx], "\n\n\rTag      : %s\n\rAddr     : %08X\n\rLen      : %d = 0x%X\n\rVer      : %d.%d\n\rDMI Addr : 0x%LX\n\rDMI Size : %d\n\rDoc Rev  : %d\n\rEPS Rev  : %d",
				tag,
				addr,
				smbios.v3hdr->len, smbios.v3hdr->len,
				smbios.v3hdr->major_ver, smbios.v3hdr->minor_ver,
				smbios.v3hdr->tab_addr,
				smbios.v3hdr->tab_sz,
				smbios.v3hdr->doc_rev,
				smbios.v3hdr->eps_rev);

			sprintf(title, "%s", "SMBIOS V3");
		}
		
		scr_show_back_confirm_keyhlp();

		key = msgbox_waitkey(T_WHITE, T_BROWN, title, msg, 4, 0);

		addr8 = (uint8_t *)smbios.hdr->smbios_addr;

		// RAW : smbios len
		dat_raw_add(RAW_ID_DMI_SMBIOS, addr8, (uint32_t)smbios.hdr->smbios_len);

		// RAW : BDA
		addr8 = (uint8_t *)BDA_MEM_BASE;
		dat_raw_add(RAW_ID_BDA, addr8, BDA_SIZE);

		res = PU_OK;
	}

	if (key == (SCAN_ESC << 8))
	{
		smbios_exit();
		res = PU_ERR_ESC_ABORT;
	}

	// todo : smbios_exit ?

	return res;
}

oem_pci_name_t *load_oem_pci_name(uint32_t *num)//oem_pci_name_t **opn)
{
	FILE	*fp;
	int		cnt, len, i;
	char	str[256];
	char		*name;
	uint16_t	vid, did;

	oem_pci_name_t	*opn;
	
	
	fp = fopen("POWERUP.PCI", "rb");
	if (!fp)
		return NULL;

	cnt = 0;
	rewind(fp);
	while (!feof(fp))
	{
		memset(str, 0, sizeof(str));
		fgets(str, 255, fp);
		len = (int)strlen(str);
		
		if (len < 11)
			continue;

		cnt++;
	} 

	opn = (oem_pci_name_t *)malloc(sizeof(oem_pci_name_t) * cnt);
	
	cnt = 0;
	rewind(fp);
	
	while (!feof(fp))
	{
		memset(str, 0, sizeof(str));
		fgets(str, 255, fp);

		len = (int)strlen(str);

		if (len < 11)
			continue;

		// delete 0x0d, 0x0a
		for (i=0; i<len; i++)
		{
			// 0xd, 0x0a -> 0x00
			if (str[i] == 0x0d || str[i] == 0xa)
			{
				str[i] = 0;
				len = i;
				break;
			}
		}

		sscanf(&str[0], "%X", &vid);
		sscanf(&str[5], "%X", &did);
		name = &str[10];
		
		opn[cnt].vid = vid;
		opn[cnt].did = did;
		for (i=0; i<41; i++)
			opn[cnt].name[i] = 0;


		if (len > 50)
			memcpy(opn[cnt].name, name, 40);
		else
			memcpy(opn[cnt].name, name, len-10);
		
		cnt++;
	} 

	*num = cnt;

	fclose(fp);

	return opn;
}

char *get_oem_pci_name(uint16_t vid, uint16_t did, oem_pci_name_t *oem_pci_name, uint32_t num)
{
	int				i;
	oem_pci_name_t	*op;

	if (!oem_pci_name)
		return NULL;

	op = oem_pci_name;

	for (i=0; (uint32_t)i<num; i++, op++)
	{
		if (vid == op->vid && did == op->did)
			return op->name;
	}
	
	return NULL;
}

//=============================================================================
//  sys_init_pci_check
//=============================================================================
uint8_t	sys_init_pci_check(void)
{
	uint8_t		res;
	char		msg[1024];
	int			i, j;
	uint16_t		key;

	int			p, num_page;
	uint8_t		confirmed = 0;
	int			sx, sy, sw, sh;

	pci_dev_t	*pd;
	uint8_t		*pool;
	uint8_t		fg, bg, bk;
	csm_t		*pcsm;
	oem_pci_name_t *opn = NULL;
	uint32_t		num = 0;
	char		*oem_name;
	

	bk = t_get_color();

	scr_clear_keyhlp();

	fg = T_BLACK;
	bg = T_LIGHTGRAY;

	//-------------------------------------------------------------------------
	//  PCI
	//-------------------------------------------------------------------------
	msgbox_doing(fg, bg, "PCI", "Scanning PCI bus......", 4, 1);

	res = pci_init();

	msgbox_doing_done();

	scr_show_any_key_keyhlp();
	key_flush_buf();

	if (res != PCI_OK)
	{
		sprintf(msg, "Scanning PCI bus error! code:%d", res);
		msgbox_waitkey(T_WHITE, T_RED, "PCI", msg, 4, 1);
		t_set_color(bk);
		return PU_ERR_SCAN_PCI;
	}
	//else
	//{
	//	sprintf(msg, "Scanning PCI bus ok!\n\nNumber of PCI devices : %d", pci->num_dev);
	//	msgbox_timeout(fg, bg, "PCI", msg, 4, 1, 500);
	//}

	//key_flush_buf();
	
	pcsm = csm_get();

	pd = pci->dev;
	num_page = (pci->num_dev + 15)/16;

	sx = 2;
	sy = 2;
	sw = 75 + 1;	// shadow
	sh = 20 + 1;	// shadow

	t_copy_region(sx, sy, sx+sw, sy+sh);
	t_color(fg, bg);
	

	opn = load_oem_pci_name(&num);

	p = 0;
	while (1)
	{ 
		// point to dev
		pd = pci->dev;
		if (p > 0)
		{
			for (j=0; j<p*16; j++)
				pd = pd->next;
		}

		// canvas
		t_put_win_single_double(sx, sy, sw, sh);

		for (i=sx+1; i<=sx+sw; i++)
			t_shadow_char(i, sy+sh);	// shadow : bottom border

		for (i=sy+1; i<=sy+sh; i++)
			t_shadow_char(sx+sw+0, i);	// shadow : right border

		//B  D  F   Ven  Dev  E C  Sc I  Desc.
		//345678901234567890123456789012345678901234567890123456789
		//XX:XX.XX  xxxx xxxx * xx.xx.xx Intel <device name>

		sprintf(msg, "Confirm PCI Device : %d", pci->num_dev);
		t_xy_puts(3+(76-(int)strlen(msg))/2, 3, msg);
		
		sprintf(msg, " Page: %d/%d ", p+1, num_page);
		t_xy_puts(64, 3, msg);

		scr_show_pci_keyhlp();

		t_xy_cl_puts(3, 5, T_BLUE, bg, "B  D  F   VID  DID  Cl Sc In Description");
		t_xy_cl_puts(64, 5, T_MAGENTA, bg, "PCI-E Device");

		for (i=0; i<16; i++)
		{
			oem_name = get_oem_pci_name(pd->cfg.ven_id, pd->cfg.dev_id, opn, num);

			if (oem_name == NULL)
			{
				sprintf(msg, "%02X:%02X.%02X  %04X %04X %02X.%02X.%02X %s %s",
					pd->bus, pd->dev, pd->fun,
					pd->cfg.ven_id, pd->cfg.dev_id,
					//(pd->ecap_lst)?"*":" ",
					pd->cfg.classcode.class_code[2],
					pd->cfg.classcode.class_code[1],
					pd->cfg.classcode.class_code[0],
					pci_find_ven_name(pd),
					pci_find_dev_name(pd));
			}
			else
			{
				sprintf(msg, "%02X:%02X.%02X  %04X %04X %02X.%02X.%02X %s",
					pd->bus, pd->dev, pd->fun,
					pd->cfg.ven_id, pd->cfg.dev_id,
					//(pd->ecap_lst)?"*":" ",
					pd->cfg.classcode.class_code[2],
					pd->cfg.classcode.class_code[1],
					pd->cfg.classcode.class_code[0],
					oem_name);
			}

			if (pd->ecap_lst)
				t_xy_cl_puts(3, 6+i, T_MAGENTA, bg, msg);
			else
				t_xy_cl_puts(3, 6+i, T_DARKGRAY, bg, msg);
			
			pd = pd->next;

			if (!pd)
				break;	// last device
		}
		t_fg_color(T_BLACK);

		key = (uint16_t)bioskey(0);//key_blk_read_sc();

		if (key == (SCAN_PAGE_UP << 8))
		{
			// KEY : PGUP = last page
			p--;
			if (p < 0)
				p = num_page - 1;	// roll back last page
		}
		else if (key == (SCAN_PAGE_DOWN << 8))
		{
			// KEY : PGDN = next page
			p++;
			if (p >= num_page)
				p = 0;
		}
		else if (key == CHAR_CARRIAGE_RETURN)
		{
			// KEY : ENTER = Confirmed!

			// RAW : PCI num_dev
			dat_raw_add(RAW_ID_PCI_DEV_NUM, (uint8_t *)&pci->num_dev, sizeof(pci->num_dev));

			// vid/did
			pool = (uint8_t *)malloc(pci->num_dev * 4);
			if (pool)
			{
				pd = pci->dev;
				i = 0;
				do
				{
					memcpy(&pool[i], &pd->cfg.ven_id, sizeof(uint16_t));
					i += 2;
					memcpy(&pool[i], &pd->cfg.dev_id, sizeof(uint16_t));
					i += 2;
					pd = pd->next;

				} while (pd != NULL);

				// RAW : PCI dev list
				dat_raw_add(RAW_ID_PCI_DEV_LST, (uint8_t *)pool, pci->num_dev * 4);

				free(pool);
			}

			// bdf : bus dev fun
			pool = (uint8_t *)malloc(pci->num_dev * 3);

			if (pool)
			{
				pd = pci->dev;
				i = 0;
				do
				{
					pool[i++] = pd->bus;
					pool[i++] = pd->dev;
					pool[i++] = pd->fun;
					pd = pd->next;

				} while (pd != NULL);

				// RAW : PCI bus/dev/fun
				dat_raw_add(RAW_ID_PCI_DEV_BDF, (uint8_t *)pool, pci->num_dev * 3);

				free(pool);
			}

			// config : 256 bytes : bus dev fun
			pool = (uint8_t *)malloc(pci->num_dev * sizeof(pci_cfg_t));

			if (pool)
			{
				pd = pci->dev;
				i = 0;
				do
				{
					memcpy(&pool[i], &pd->cfg, sizeof(pci_cfg_t));
					i += sizeof(pci_cfg_t);
					pd = pd->next;
				} while (pd != NULL);

				// RAW : PCI dev CFG
				dat_raw_add(RAW_ID_PCI_DEV_CFG, (uint8_t *)pool, pci->num_dev * sizeof(pci_cfg_t));

				free(pool);
			}

			// RAW : CSM info
			dat_raw_add(RAW_ID_CSM_INFO, (uint8_t *)&pcsm->info, sizeof(csm_info_t));
			
			confirmed = 1;

			break;
		}
		else if (key == (SCAN_DELETE << 8))
		{
			confirmed = 0;	// not Confirmed!
			break;
		}
		else if (key == (SCAN_ESC << 8))
		{
			// back to last stage
			confirmed = 0xFA;
			break;
		} 
	}

	t_paste_region(-1, -1);

	t_set_color(bk);


	free(opn);

	// confirm checking
	if (confirmed == 0xFA)
	{
		pci_exit();

		return PU_ERR_ESC_ABORT;
	}
	else if (confirmed == 0)
	{
		scr_show_any_key_keyhlp();

		msgbox_waitkey(T_WHITE, T_RED, "PCI", "Abort PCI devices confirmed!", 4, 1);

		pci_exit();

		return PU_ERR_CONFIRM_PCI;
	}

	// don't call pci_exit() here

	return PU_OK;
}

//=============================================================================
//  sys_init_smbus_check
//=============================================================================
uint8_t	sys_init_smbus_check(void)
{
	uint8_t		res;
	char		title[64];
	char		msg[1536];
	int			i, j;

	smb_dev_t	*sd, *sdev = NULL;
	pci_dev_t	*pd;
	uint8_t		*pool;
	uint16_t		key;
	uint8_t		addr;
	int			idx;


	scr_clear_keyhlp();
	sprintf(title, "%s", "SMBus");

	//-------------------------------------------------------------------------
	//  SMBus
	//-------------------------------------------------------------------------
	// SMBus Controller present ?
	pd = pci_find_class(0x0C, 0x05, 0x00);	// 0C/05/00 : SMBus controller

	if (pd == NULL)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "SMBus controller not found!", 4, 1);

		return PU_ERR_NO_SMB_CTLR;
	}

	// ATI had merged in AMD
	if (!(pd->cfg.ven_id == PCI_VID_INTEL || pd->cfg.ven_id == PCI_VID_AMD || pd->cfg.ven_id == PCI_VID_ATI))
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "Only support Intel or AMD SMBus controller!", 4, 1);

		return PU_ERR_SMB_CTLR;
	}

	smb_exit();

	res = smb_init();

	if (res != SMB_OK)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "SMBus controller init error!", 4, 1);
		return SMB_ERR_NULL_BUS;
	}

	//-------------------------------------------------------------------------
	//  Detect SMBus device
	//-------------------------------------------------------------------------
	addr = 0x10;	// 0x10~0xEE : slave address range

	// detect SMBus device on SMBus
	sprintf(msg, "Detecting SMBus Device : 0x%02X .......  1/112", addr);
	msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);

	key = 0;

	scr_show_abort_keyhlp();
	
	// slave address range : 0x10 ~ 0xEE
	for (addr=0x10, i=0; ; addr+=2, i++)
	{
		sprintf(msg, "Detecting SMBus Device : 0x%02X .......%3d/112", addr, i+1);
		msgbox_progress_cont(title, msg);

		res = smb_scan_dev(addr);

		if (res == SMB_OK)
		{
			sd = smb_add_dev();
			if (!sd)
				continue;

			sd->addr = addr & 0xfe;	// mask R/W bit

			// device link-list
			if (smb->num_dev == 0)
			{
				smb->dev = sd;		// 1st device
			}
			else
			{
				sdev->next	= sd;	// 2nd device...
				sd->prev	= sdev;
			}
			sdev = sd;
			smb->num_dev++;
		}
		
		if (addr == 0xEE)
		{
			if (smb->num_dev > 0)
				res = SMB_OK;
			break;
		}

		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if(key)//(key != KEY_SC_NULL)
		{
			if (key == (SCAN_ESC << 8))
				break;
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
				msgbox_progress(T_WHITE, T_MAGENTA, title, msg, 4, 1);
			}
		}
	}

	scr_clear_keyhlp();

	msgbox_progress_done();
	
	key_flush_buf();

	// abort SMBus device detecting
	if (key == (SCAN_ESC << 8))
	{
		msgbox_tmout(T_WHITE, T_RED, title, "Abort SMBus device detecting!", 4, 1, 1000);
		goto abort_smbus_detect;
	}

	//-------------------------------------------------------------------------
	//  Detect SMBus result
	//-------------------------------------------------------------------------
	if (res != SMB_OK)
	{
		msgbox_waitkey(T_WHITE, T_RED, title, "Scanning SMBus device error!", 4, 1);

		return PU_ERR_SCAN_SMBUS;
	}
	else
	{
		//-------------------------------------------------------------------------
		//  RAW : SMBus
		//-------------------------------------------------------------------------
		// RAW : smbus num_dev
		dat_raw_add(RAW_ID_SMB_DEV_NUM, (uint8_t *)&smb->num_dev, sizeof(smb->num_dev));

		sprintf(msg, "Detecting SMBus devices done!\n\n\r\b%cNumber of device : %d", T_ARG_LIGHTGREEN, smb->num_dev);
		idx = (int)strlen(msg);

		if (smb->num_dev)
		{
			sprintf(&msg[idx], "\n\n\r\b%cDevice list :\n\r", T_ARG_YELLOW);
			idx = (int)strlen(msg);

			sd = smb->dev;
			for (i=0, j=idx; i<smb->num_dev; i++)
			{
				sprintf(&msg[j], "%02X ", sd->addr);
				sd = sd->next;
				j += 3;
				
				// 8 item per line
				if (((i&0x7) == 7) && (i != (smb->num_dev-1)))
				{
					msg[j++] = '\n';
					msg[j++] = '\r';
				}
			}
			msg[j] = 0;
		}

		scr_show_back_confirm_keyhlp();
		
		key = msgbox_waitkey(T_WHITE, T_MAGENTA, title, msg, 4, 1);
	}

	//-------------------------------------------------------------------------
	//  RAW : SMBus
	//-------------------------------------------------------------------------
abort_smbus_detect:

	if (key == (SCAN_ESC << 8))
	{
		// remove all smb dev
		sd = smb->dev;

		if (sd)
		{
			do
			{
				sdev = sd;
				sd = sd->next;
			
				free(sdev);

			} while (sd != NULL);
		}
		
		smb->num_dev = 0;

		return PU_ERR_ESC_ABORT;
	}
	
	// RAW_ID_SMB_DEV_LST
	i = 0;
	sd = smb->dev;

	pool = (uint8_t *)malloc(smb->num_dev * 1);

	if (pool)
	{
		sd = smb->dev;

		do
		{
			pool[i++] = sd->addr;
			sd = sd->next;

		} while (sd != NULL);

		// RAW : smbus dev lst
		dat_raw_add(RAW_ID_SMB_DEV_LST, (uint8_t *)pool, smb->num_dev * 1);
		free(pool);
	}

	scr_clear_keyhlp();

	return PU_OK;
}

//=============================================================================
//  sys_init_rtc_check
//=============================================================================
uint8_t	sys_init_rtc_check(void)
{
	int				i;
	char			str[128];
	uint8_t			cmos_ram[14];
	uint8_t			flag;
	uint16_t			key;
	t_msgbox_msg_t	msg;
	uint8_t sec_bak = 0;

	flag = 0;

	scr_show_back_confirm_keyhlp();

	while (1)
	{
		key = (uint16_t)bioskey(1);//key = key_non_blk_read_sc();

		if(key)//(key != KEY_SC_NULL)
		{
			if (key == (SCAN_ESC << 8))
			{
				msgbox_progress_done();
				key_flush_buf();
				scr_clear_keyhlp();
				return PU_ERR_ESC_ABORT;
			}
			else if (key == (SCAN_F10 << 8))
			{
				t_scr_capture();
				flag = 0;
			}
			else
			{
				break;
			}
		}
		
		msgbox_msg_init(&msg);

		// RTC register : 00h ~ 0Dh
		sprintf(str, "\r\b%c 0  1  2  3  4  5  6  7  8  9  A  B  C  D  \n\r\b%c", T_ARG_RED, T_ARG_DARKGRAY);
		msgbox_msg_add_str(str, &msg);

		for (i=0; i<14; i++)
			cmos_ram[i] = rtc_read_reg((uint8_t)i);

		for (i=0; i<14; i++)
		{
			sprintf(str, "%02X ", cmos_ram[i]);
			msgbox_msg_add_str(str, &msg);
		}
		msgbox_msg_add_char('\n', &msg);
		msgbox_msg_add_char('\n', &msg);
		
		sprintf(str, "\r\b%cDate  = %02X-%02X-%02X\n", T_ARG_BLACK, cmos_ram[9], cmos_ram[8], cmos_ram[7]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rWeek  = %02X\n", cmos_ram[6]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rTime  = %02X:%02X:%02X\n", cmos_ram[4], cmos_ram[2], cmos_ram[0]);
		msgbox_msg_add_str(str, &msg);
		
		sprintf(str, "\rAlarm = %02X:%02X:%02X\n", cmos_ram[5], cmos_ram[3], cmos_ram[1]);
		msgbox_msg_add_str(str, &msg);

		msgbox_msg_add_char('\n', &msg);
		
		msgbox_msg_add_char('\r', &msg);
		msgbox_msg_add_char('\b', &msg);
		msgbox_msg_add_char(T_ARG_BLACK, &msg);

		sprintf(str, "RTC [0Ah] = %02Xh : RTC crystal\n", 0x26);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC [0Bh] = %02Xh : RTC status\n", 0x02);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC [0Dh] = %02Xh : RTC power", 0x80);
		msgbox_msg_add_str(str, &msg);

		if (flag == 0)
		{
			msgbox_progress(T_BLACK, T_LIGHTGRAY, "RTC", msg.buf, 4, 0);
			flag = 1;
		}
		else
		{
			if(cmos_ram[0] != sec_bak)
			{
				msgbox_progress_cont("RTC", msg.buf);
				sec_bak = cmos_ram[0];
			}
		}

		delay(200);
	}

	msgbox_progress_done();

	key_flush_buf();

	scr_clear_keyhlp();

	return PU_OK;
}

//=============================================================================
//  sys_init
//=============================================================================
uint8_t sys_init(void)
{
	uint8_t		stage;
	uint8_t		res, res2;
	uint8_t		exit_loop;
	char		msg[64];

	stage = res = exit_loop = 0;

	pu->init = 1;

	while (exit_loop == 0)
	{
		switch(stage)
		{
		//-----------------------------------------------------
		//  0 : Timer
		//-----------------------------------------------------
		case 0:
			// Timer
			res = sys_init_timer_check();
			if (res == PU_ERR_TMR_DEAD)
				exit_loop = 1;
			else if (res == PU_ERR_ESC_ABORT)
				exit_loop = 1;
			else
				stage++;
			break;

		//-----------------------------------------------------
		//  1 : CPU
		//-----------------------------------------------------
		case 1:
			res = sys_init_cpu_check();
			if (res == PU_OK)
				stage++;
			else if (res == PU_ERR_ESC_ABORT)
				stage--;
			else
				exit_loop = 1;
			break;

		//-----------------------------------------------------
		//  2 : Memory SPD
		//-----------------------------------------------------
		case 2:

			// dialog : skip SPD checking ?
			res = sys_init_memory_spd_check_yes_no();
			if (res)
			{
				if (res == 0xFA)	// esc
				{
					stage--;
				}
				else
				{
					stage++;
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
				}
				break;
			}
		
			res = sys_init_memory_spd_check();
			
			if (res == PU_OK)
			{
				stage++;
				SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else if ((res == PU_ERR_CANT_SUPPORT_SPD) || (res == PU_ERR_NO_SPD_FOUND))
			{
				// SPD not present
				// or skip SPD checking
				CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
				stage++;
			}
			else if (res == PU_STS_IMC_FOUND)
			{
				res2 = sys_init_imc_spd_check();
				if (res2 == PU_OK)
				{
					stage++;
					SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
				}
				else if (res2 == PU_ERR_ESC_ABORT)
				{
					stage--;
				}
				else if (res2 == PU_ERR_NO_SPD_FOUND)
				{
					// skip SPD checking
					stage++;
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
				}
				else
				{
					// SPD might not present : keep going to test
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD);
					stage++;
				}
			}
			else
			{
				exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  3 : Memory Map
		//-----------------------------------------------------
		case 3:
			res = sys_init_memory_map_check();
			if (res == PU_OK)
				stage++;
			else if (res == PU_ERR_ESC_ABORT)
				stage--;
			else
				exit_loop = 1;
			break;

		//-----------------------------------------------------
		//  4 : ACL BIOS
		//-----------------------------------------------------
		case 4:
			res = sys_init_acl_bios_check();
			if (res == PU_OK)
			{
				stage++;
				SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS);
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else if (res == PU_ERR_GET_ABIOS)
			{
				// abios absent : allow non-acl board
				stage++;
				CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS);
			}
			else
			{
				exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  5 : ACL EC
		//-----------------------------------------------------
		case 5:
			res = sys_init_acl_ec_check();
			if (res == PU_OK)
			{
				// with EC
				stage++;
				SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC);
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else if (res == PU_ERR_GET_AEC)
			{
				// without EC : allow non-ec board
				stage++;
				CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC);
			}
			else
			{
				exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  6 : SMBIOS
		//-----------------------------------------------------
		case 6:
			res = sys_init_smbios_check();
			if (res == PU_OK)
			{
				stage++;
				SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS);
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else if (res == PU_ERR_GET_SMBIOS)
			{
				CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS);
				stage++;
			}
			else
			{
				exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  7 : PCI
		//-----------------------------------------------------
		case 7:
			res = sys_init_pci_check();
			if (res == PU_OK)
				stage++;
			else if (res == PU_ERR_ESC_ABORT)
				stage--;
			else
				exit_loop = 1;
			break;

		//-----------------------------------------------------
		//  8 : SMBus
		//-----------------------------------------------------
		case 8:
			res = sys_init_smbus_check();
			if (res == PU_OK)
			{
				stage++;
				SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB);
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else
			{
				CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB);
				stage++;
				//exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  9 : RTC
		//-----------------------------------------------------
		case 9:
			res = sys_init_rtc_check();
			if (res == PU_OK)
			{
				stage++;
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else
			{
				exit_loop = 1;
			}
			break;

		//-----------------------------------------------------
		//  10 : Check Switch
		//-----------------------------------------------------
		case 10:
			dat->par->cmp = PU_PAR_CMP_DEF;
			
			// ABIOS :no feature
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS) == 0)
				dat->par->cmp &= ~PU_PAR_CMP_AB;

			// AEC : no feature
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC) == 0)
				dat->par->cmp &= ~PU_PAR_CMP_AEC;

			// SMBIOS : no feature
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS) == 0)
				dat->par->cmp &= ~PU_PAR_CMP_SM;

			// SMB : no feature
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB) == 0)
				dat->par->cmp &= ~PU_PAR_CMP_SB;

			// SPD : no feature
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD) == 0)
				dat->par->cmp &= ~PU_PAR_CMP_SP;
			
			scr_show_chkbox_keyhlp();
			res = cpmenu_show();
			if (res == PU_OK)
			{
				stage++;
				exit_loop = 1;
			}
			else if (res == PU_ERR_ESC_ABORT)
			{
				stage--;
			}
			else
			{
				exit_loop = 1;
			}
			break;
		}
	}

	scr_show_main_body();
	scr_show_title();

	if (res == PU_ERR_TMR_DEAD || res == PU_ERR_ESC_ABORT)
	{
		return res;
	}
	
	//-----------------------------------------------------
	//   Create POWERUP.DAT
	//-----------------------------------------------------
	if (res != PU_OK)
	{
		sprintf(msg, "POWERUP init error : 0x%02X\n\nPlease ask for help.", res);
		msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);

		dat_exit();
		return res;
	}

	msgbox_doing(T_BLACK, T_LIGHTGRAY, "POWERUP.DAT", "Creating POWERUP.DAT........\n\nPlease wait a moment.", 4, 1);
	
	// TODO : rec
	//rec = (pu_rec_t *)malloc(sizeof(pu_rec_t)*PU_REC_MAX_NUM);
	//rec = (pu_rec_t *)malloc(sizeof(pu_rec_t)*(dat->par->rec_cnt + 1));

	// increase : 1024 -> 2048 ->3072..
	dat->hdr->rec_sz = sizeof(pu_rec_t)*(1024);
	dat->rec_pool = (pu_rec_t *)malloc(dat->hdr->rec_sz);
	if (dat->rec_pool)
	{
		memset(dat->rec_pool, 0, dat->hdr->rec_sz);

		res = dat_create();
		
		dat_exit();
		msgbox_doing_done();
		msgbox_doing(T_BLACK, T_CYAN, "Welcome", "POWERUP is ready for testing........", 4, 1);

		//mode3_create_exe();
	}
	else
	{
		msgbox_doing(T_WHITE, T_RED, "Error", "POWERUP is failed!........", 4, 1);
	}

	delay(1500);
	msgbox_doing_done();
	
	return res;
}

//=============================================================================
//  sys_exit
//=============================================================================
void sys_exit(void)
{
	abios_exit();
	e820_exit();
	smbios_exit();
	smb_exit();
	pci_exit();
}

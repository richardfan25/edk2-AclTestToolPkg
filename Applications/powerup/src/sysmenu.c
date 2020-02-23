//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  SYSMENU : System Menu                                                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
//#include <dos.h>

#include "typedef.h"
#include "t_video.h"
#include "t_vlbox.h"
#include "t_msgbox.h"

#include "aec.h"
#include "e820.h"
#include "pci.h"
#include "smbios.h"
#include "key.h"
#include "rtc.h"

#include "dat.h"
#include "upd.h"
#include "pu.h"
#include "scr.h"
#include "sysinit.h"

//=============================================================================
//  sysmenu_rtc_show
//=============================================================================
static void sysmenu_rtc_show(void)
{
	int				i;
	char			str[128];
	uint8_t			cmos_ram[14];
	uint8_t			flag;
	t_msgbox_msg_t	msg;
	uint8_t sec_bak = 0;

	flag = 0;

	while (1)
	{
		if (bioskey(1))
			break;
		
		msgbox_msg_init(&msg);

		// RTC register : 00h ~ 0Dh
		sprintf(str, "\r 0  1  2  3  4  5  6  7  8  9  A  B  C  D  \n\r\b%c", T_ARG_YELLOW);
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
		
		sprintf(str, "\r\b%cDate  = %02X-%02X-%02X\n", T_ARG_WHITE, cmos_ram[9], cmos_ram[8], cmos_ram[7]);
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
		msgbox_msg_add_char(T_ARG_WHITE, &msg);

		sprintf(str, "RTC [0Ah] = %02Xh : RTC crystal\n", 0x26);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC [0Bh] = %02Xh : RTC status\n", 0x02);
		msgbox_msg_add_str(str, &msg);
		sprintf(str, "\rRTC [0Dh] = %02Xh : RTC power", 0x80);
		msgbox_msg_add_str(str, &msg);

		if (flag == 0)
		{
			msgbox_progress(T_WHITE, T_MAGENTA, "RTC", msg.buf, 4, 0);
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
}

//=============================================================================
//  sysmenu_vlbox
//=============================================================================
char *sysmenu_item[10] =
{
	" SYS : Timer",
	" SYS : CPU Info",
	" SYS : RAM SPD",
	" SYS : E820",
	" SYS : ACL BIOS",
	" SYS : ACL EC",
	" SYS : SMBIOS",
	" SYS : PCI",
	" SYS : SMBus",
	" SYS : RTC"
};

char *sysmenu_help[10] =
{
	"Checking timer is working",
	"Showing CPU info",
	"Showing RAM SPD list",
	"Showing E820 info",
	"Showing ACL BIOS info",
	"Showing ACL EC info",
	"Showing SMBIOS info",
	"Showing PCI device list",
	"Showing SMBus device list",
	"Showing RTC registers"
};

t_vlbox_t	sysmenu_vlbox;

//=============================================================================
//  sysmenu_show
//=============================================================================
void sysmenu_show(void)
{
	char	str[32];
	uint8_t	sel;
	uint8_t	bk;
 
	sysmenu_vlbox.title	= NULL;
	sysmenu_vlbox.item	= sysmenu_item;
	sysmenu_vlbox.help	= sysmenu_help;

	sysmenu_vlbox.num_item = 10;
	sysmenu_vlbox.num_help = 10;

	sysmenu_vlbox.fg = T_BLACK;
	sysmenu_vlbox.bg = T_LIGHTGRAY;
	sysmenu_vlbox.fs = T_YELLOW;
	sysmenu_vlbox.bs = T_BLUE;
	sysmenu_vlbox.fh = T_RED;
	sysmenu_vlbox.bh = T_LIGHTGRAY;
	
	sysmenu_vlbox.hm = 1;
	sysmenu_vlbox.vm = 0;
	sysmenu_vlbox.ha = 0;
	sysmenu_vlbox.va = 0;
	
	sysmenu_vlbox.align	= T_AL_CM;
	sysmenu_vlbox.tialign= ((T_AL_CENTER<<4) | (T_AL_LEFT));
	sysmenu_vlbox.flag	= ( T_VLBOX_FL_TITLE   |
				T_VLBOX_FL_HELP	   |
				T_VLBOX_FL_KEYHELP |
				T_VLBOX_FL_SHADOW  |
				T_VLBOX_FL_FRAME );
	sysmenu_vlbox.sel	= 0;

	bk = t_get_color();
	
	sprintf(str, "%s", "System Function Menu");
	sysmenu_vlbox.title = (char *)malloc(strlen(str)+1);
	if (!sysmenu_vlbox.title)
		return;
	sprintf(sysmenu_vlbox.title, "%s", str);

	sysmenu_vlbox.sel = 0;
	
	sel = t_show_vlbox(&sysmenu_vlbox);

	free(sysmenu_vlbox.title);
	
	if (sel == 0xFA)	// abort
		return;

	switch(sel)
	{
		case 0:		sys_init_timer_check();			break;
		case 1:		sys_init_cpu_check();			break;
		case 2:
			if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_IMC_SPD))
				sys_init_imc_spd_check();
			else
				sys_init_memory_spd_check();
			break;
		case 3:		sys_init_memory_map_check();	break;
		case 4:		sys_init_acl_bios_check();		break;
		case 5:		sys_init_acl_ec_check();		break;
		case 6:		sys_init_smbios_check();		break;
		case 7:		sys_init_pci_check();			break;
		case 8:		sys_init_smbus_check();			break;
		case 9:		sysmenu_rtc_show();				break;
	}

	// recover key help
	scr_show_main_keyhlp();

	t_set_color(bk);
}

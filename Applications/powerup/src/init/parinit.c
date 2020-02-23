//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  PARINIT : Parameter Init                                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <direct.h>
#include <sys/stat.h>
//#include <i86.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_hlbox.h"
#include "t_indat.h"
#include "t_msgbox.h"
#include "bda.h"
#include "scr.h"
#include "pu.h"
#include "key.h"

//=============================================================================
//  par_rtc_batt
//=============================================================================
static uint8_t par_rtc_batt(void)
{
	char *hb_item[2] =
	{
		"Present",
		"Absent"
	};

	char *hb_help[2] =
	{
		"With RTC battery",
		"No RTC battery"
	};

	t_hlbox_t	hb;

	hb.title	= "RTC Battery";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 6;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );
	hb.sel = 0;

	scr_show_hlbox2_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_at_atx
//=============================================================================
static uint8_t par_at_atx(void)
{
	char *hb_item[2] =
	{
		"AT",
		"ATX"
	};

	char *hb_help[2] =
	{
		"Auto power on when AC is on.",
		"Need to press power button to power on."
	};

	t_hlbox_t	hb;

	hb.title	= "Power On Mode";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 0;

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_tolerance
//=============================================================================
static uint8_t par_tolerance(uint8_t *tol)
{
	t_indat_t	indat;
	uint8_t		res;

	//indat.title = "";//Dialog Box Test";
	indat.title = "Interval = Average + Tolerance";//"INDAT Test";
	indat.field	= "Tolerance";
	indat.mode	= (T_INDAT_BYTE | T_INDAT_UNSIGNED | T_INDAT_DEC);
	indat.unit  = "seconds";//"unit";
	indat.help  = "Average = [(1st ~ 10th) - min - max] / 8";//"This is a INDAT help";

	indat.fg = T_WHITE;		indat.bg = T_MAGENTA;
	indat.fi = T_YELLOW;	indat.bi = T_BLACK;
	indat.fh = T_YELLOW;	indat.bh = T_MAGENTA;

	indat.align = T_AL_CM;
	indat.shadow = 1;
	indat.data = 5;	// default : 5 seconds

	//indat.mode|= T_INDAT_MAX_MIN;
	//indat.max	= 256;
	//indat.min	= 1;

	scr_show_indat_dec_keyhlp();
	
	res = t_show_indat(&indat);
	if (res)
	{
		return res;
	}
	else
	{
		*tol = (uint8_t)indat.data;
		return 0;
	}
}

//=============================================================================
//  par_rec_sort_type
//=============================================================================
static uint8_t par_rec_sort_type(void)
{
	char *hb_item[2] =
	{
		"Max. No",
		"Max. Interval"
	};

	char *hb_help[2] =
	{
		"Showing the last record.",
		"Showing the record of maximum interval."
	};

	t_hlbox_t	hb;

	hb.title	= "Which sort type of record showing ?";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 6;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 0;

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_alarm
//=============================================================================
static uint8_t par_alarm(void)
{
	char *hb_item[2] =
	{
		"Silence",
		"Alarm"
	};

	char *hb_help[2] =
	{
		"No issue an alarm in any status.",
		"Issue an alarm when the fail status occurred."
	};

	t_hlbox_t		hb;

	hb.title	= "Issue an Alarm ?";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 0;

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_reset_type
//=============================================================================
static uint8_t par_reset_type(void)
{
	char *hb_item[6] =
	{
		"None",
		"KBC",
		"Soft92",
		"Soft",
		"Hard",
		"Full"
	};

	char *hb_help[6] =
	{
		"No Reset",
		"KBC Reset : 0xFE -> Port 64h",
		"Soft92 Reset : 0x01 -> Port 92h",
		"Soft Reset : 0x04 -> Port CF9h",
		"Hard Reset : 0x06 -> Port CF9h",
		"Full Reset : 0x0E -> Port CF9h"
	};

	t_hlbox_t		hb;

	hb.title	= "Time-Out Reset ?";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 6;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 0;

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_reset_timeout
//=============================================================================
static uint8_t par_reset_timeout(uint8_t *data)
{
	t_indat_t	indat;
	uint8_t		res;

	//indat.title = "";//Dialog Box Test";
	indat.title = "Reset Time-Out ?";//"INDAT Test";
	indat.field	= "";//Tolerance";
	indat.mode	= (T_INDAT_BYTE | T_INDAT_UNSIGNED | T_INDAT_DEC);
	indat.unit  = "seconds";//"unit";
	indat.help  = "";

	indat.fg = T_WHITE;		indat.bg = T_MAGENTA;
	indat.fi = T_YELLOW;	indat.bi = T_BLACK;
	indat.fh = T_YELLOW;	indat.bh = T_MAGENTA;

	indat.align = T_AL_CM;
	indat.shadow = 1;
	indat.data = 10;	// default : 10 seconds

	scr_show_indat_dec_keyhlp();

	res = t_show_indat(&indat);
	if (res)
	{
		return res;
	}
	else
	{
		*data = (uint8_t)indat.data;
		return 0;
	}
}

//=============================================================================
//  par_autoexec
//=============================================================================
static uint8_t par_autoexec(void)
{
	char *hb_item[2] =
	{
		"Yes",
		"No"
	};

	char *hb_help[2] =
	{
		"Create a startup.nsh to auto-run POWERUP after reboot.",
		"Need to edit startup.nsh manually. Please add 'POWERUP'"
	};

	t_hlbox_t		hb;

	hb.title	= "Create startup.nsh";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_MAGENTA;
	hb.fs = T_BLUE;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_MAGENTA;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap =8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 1;		// default : no

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_check_autoexec
//=============================================================================
static int par_check_autoexec(void)
{
	char	*cwd;		// working directory
	char	autoexec_bat[80];
	char path[80];
	struct stat	st;

	// file name
	cwd = getcwd(path, 80);
	if (!cwd)
		return -1;

	if(strchr(path, '\\'))
	{
		strncpy(strchr(path, '\\'), "\\startup.nsh\0", 13);
	}
	else
	{
		strncpy(path, "\\startup.nsh\0", 13);
	}
	sprintf(autoexec_bat, "%s", path);
	free(cwd);

	if (stat(autoexec_bat, &st))
	{
		// AUTOEXEC.BAT not found
		return -1;
	}

	// AUTOEXEC.BAT existed
	return 0;
}

//=============================================================================
//  par_create_autoexec
//=============================================================================
static void par_create_autoexec(void)
{
	char	*cwd;		// working directory
	char	autoexec_bat[80];
	char	autoexec_bak[80];
	char	autoexec_old[80];
	char *folder, path[80], file_path[80];

	struct stat	st;
	FILE		*fp;

	// file name
	cwd = getcwd(path, 80);
	if (!cwd)
		return;

	memcpy(file_path, path, strlen(path));
	if(strchr(file_path, '\\'))
	{
		strncpy(strchr(file_path, '\\'), "\\startup.nsh\0", 13);
	}
	else
	{
		strncpy(file_path, "\\startup.nsh\0", 13);
	}
	sprintf(autoexec_bat, "%s", file_path);

	memcpy(file_path, path, strlen(path));
	if(strchr(file_path, '\\'))
	{
		strncpy(strchr(file_path, '\\'), "\\startup.bak\0", 13);
	}
	else
	{
		strncpy(file_path, "\\startup.bak\0", 13);
	}
	
	sprintf(autoexec_bak, "%s", file_path);

	memcpy(file_path, path, strlen(path));
	if(strchr(file_path, '\\'))
	{
		strncpy(strchr(file_path, '\\'), "\\startup.old\0", 13);
	}
	else
	{
		strncpy(file_path, "\\startup.old\0", 13);
	}
	sprintf(autoexec_old, "%s", file_path);

	free(cwd);

	if (stat(autoexec_bat, &st) == 0)
	{
		if (stat(autoexec_bak, &st) == 0)
		{
			remove(autoexec_old);
			rename(autoexec_bak, autoexec_old);
		}
			
		// AUTOEXEC.BAT file exist
		rename(autoexec_bat, autoexec_bak);	// rename it
	}

	fp = fopen(autoexec_bat, "wb");
	if (fp)
	{
		folder = strtok(path, "\\");
		fprintf(fp, "%s\n", folder);
		while(folder != NULL)
		{
			folder = strtok(NULL, "\\");
			
			if(folder != NULL)
			{
				fprintf(fp, "cd %s\n", folder);
			}
		}
		fprintf(fp, "%s", "POWERUP\r\n");
		fflush(fp);
		fclose(fp);
	}
}

//=============================================================================
//  par_show_delete_autoexec
//=============================================================================
uint8_t par_show_delete_autoexec(void)
{
	char *hb_item[2] =
	{
		"Yes",
		"No"
	};

	char *hb_help[2] =
	{
		"Delete startup.nsh to disable auto-run POWERUP after reboot.",
		"Keep startup.nsh to enable auto-run after reboot"
	};

	t_hlbox_t		hb;

	hb.title	= "Delete startup.nsh ?";
	hb.item		= hb_item;
	hb.help		= hb_help;
	hb.num_item = sizeof(hb_item)/sizeof(hb_item[0]);
	hb.num_help = sizeof(hb_help)/sizeof(hb_help[0]);

	hb.fg = T_WHITE;	hb.bg = T_RED;
	hb.fs = T_RED;		hb.bs = T_LIGHTGRAY;
	hb.fh = T_YELLOW;	hb.bh = T_RED;

	hb.hm = 4;
	hb.vm = 1;
	hb.ha = 0;
	hb.va = 0;
	hb.gap = 8;

	hb.align	= T_AL_CM;

	hb.flag		= ( T_HLBOX_FL_TITLE   |
					T_HLBOX_FL_HELP	   |
					T_HLBOX_FL_KEYHELP |
					T_HLBOX_FL_SHADOW  |
					T_HLBOX_FL_FRAME );

	hb.sel = 0;

	scr_show_hlbox_keyhlp();

	return t_show_hlbox(&hb);
}

//=============================================================================
//  par_delete_autoexec
//=============================================================================
void par_delete_autoexec(void)
{
	char	*cwd;		// working directory
	char	autoexec_bat[80];
	char	autoexec_bak[80];
	char path[80], file_path[80];
	uint8_t	res;

	struct stat	st;

	// file name
	cwd = getcwd(path, 80);
	if (!cwd)
		return;

	memcpy(file_path, path, strlen(path));
	if(strchr(file_path, '\\'))
	{
		strncpy(strchr(file_path, '\\'), "\\startup.nsh\0", 13);
	}
	else
	{
		strncpy(file_path, "\\startup.nsh\0", 13);
	}
	sprintf(autoexec_bat, "%s", file_path);

	memcpy(file_path, path, strlen(path));
	if(strchr(file_path, '\\'))
	{
		strncpy(strchr(file_path, '\\'), "\\startup.bak\0", 13);
	}
	else
	{
		strncpy(file_path, "\\startup.bak\0", 13);
	}
	sprintf(autoexec_bak, "%s", file_path);

	free(cwd);

	// autoexec.bat existed
	if (stat(autoexec_bat, &st) == 0)
	{
		res = par_show_delete_autoexec();
		if (res == 0)	// Yes
		{
			remove(autoexec_bak);
			rename(autoexec_bat, autoexec_bak);
		}
	}
}

//=============================================================================
//  par_init
//=============================================================================
uint8_t par_init(uint8_t sta)
{
	uint8_t		stage;
	uint8_t		res, data8;

	stage = sta;
	
	dat->par->tm_init = time(NULL);

	// video_en
	if (bda_get_video_config() & BDA_VIDEO_EN)
	{
		SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_VIDEO_EN);
	}

	while (1)
	{
		switch(stage)
		{
		//-----------------------------------------------------
		//  RTC battery
		//-----------------------------------------------------
		case 0:
			
			res = par_rtc_batt();

			if (res == 0xFA)
			{
				stage = 0xFA;	// exit
			}
			else
			{
				stage++;
				if (!!res)
				{
					// without RTC battery
					SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC);
				}
				else
				{
					// with RTC battery
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC);
				}
			}

			break;

		//-----------------------------------------------------
		//  AT/ATX
		//-----------------------------------------------------
		case 1:

			res = par_at_atx();

			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				if (!!res)
				{
					// ATX
					SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX);
				}
				else
				{
					// AT
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AT_ATX);
				}
			}

			break;

		//-----------------------------------------------------
		//  Tolerence
		//-----------------------------------------------------
		case 2:

			res = par_tolerance(&data8);
	
			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				dat->par->tol = (uint16_t)data8;
			}

			break;

		//-----------------------------------------------------
		//  Sort Type
		//-----------------------------------------------------
		case 3:

			res = par_rec_sort_type();
	
			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				if (res)
					dat->par->sort_typ = 2;	// todo define
				else
					dat->par->sort_typ = 0;
			}

			break;

		//-----------------------------------------------------
		//  Alarm
		//-----------------------------------------------------
		case 4:

			res = par_alarm();

			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				
				if ((pu->alm.lo != 0) && (pu->alm.hi != 0))
				{
					dat->par->alm_lo = pu->alm.lo;
					dat->par->alm_hi = pu->alm.hi;
				}
				else
				{
					dat->par->alm_lo = 700;	// 650~750 Hz
					dat->par->alm_hi = 950;	// 900~1000 Hz
				}

				if (!!res)
				{
					// alarm sw : on
					SET_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW);
					
					msgbox_doing(T_WHITE, T_RED, "Alarm", "Alarm playing......\n\nYou should hear an alarm.", 4, 1);
					
					sound(dat->par->alm_lo); delay(400);
					sound(dat->par->alm_hi); delay(600);
					sound(dat->par->alm_lo); delay(400);
					sound(dat->par->alm_hi); delay(600);
					nosound();
					
					msgbox_doing_done();

					key_flush_buf();
	
				}
				else
				{
					// alarm sw : off
					CLR_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW);
				}
			}

			break;

		//-----------------------------------------------------
		//  Reset Type
		//-----------------------------------------------------
		case 5:

			res = par_reset_type();

			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				dat->par->rst_typ = res;

				if (res == PU_PAR_RST_NONE)
					stage++;	// skip reset timeout setting
			}

			break;

		//-----------------------------------------------------
		//  Reset Timeout
		//-----------------------------------------------------
		case 6:

			res = par_reset_timeout(&data8);

			if (res == 0xFA)
			{
				stage--;
			}
			else
			{
				stage++;
				dat->par->rst_tmo = (uint16_t)data8;
			}

			break;

		//-----------------------------------------------------
		//  Create AUTOEXEC.BAT
		//-----------------------------------------------------
		case 7:

			if (par_check_autoexec() == 0)
				msgbox_waitkey(T_WHITE, T_RED, "startup.nsh", "startup.nsh existed!\n\nIt will be renamed startup.bak", 4, 1);
			
			// AUTOEXEC.BAT not found or error!
			res = par_autoexec();

			if (res == 0xFA)
			{
				stage--;
				if (dat->par->rst_typ == PU_PAR_RST_NONE)
					stage--;	// skip reset timeout setting
			}
			else
			{
				stage++;
				if (res == 0)	// Yes
					par_create_autoexec();
			}

			break;
		}

		if (stage >= 8)
		{
			break;
		}

	}// while

	return stage;
}

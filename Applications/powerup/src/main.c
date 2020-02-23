#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <string.h>
//#include <malloc.h>
#include <sys/stat.h>
#include <time.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "tsc.h"
#include "abios.h"
#include "log.h"

#include "bda.h"
#include "key.h"
#include "dat.h"
#include "errcode.h"
#include "parinit.h"
#include "sysinit.h"

#include "scr.h"
#include "pu.h"

#include "datlst.h"

#ifdef __WATCOMC__
//#include <i86.h>
#endif

//=============================================================================
//  variables
//=============================================================================
uint64_t	boot_tsc;
pu_t		*pu;

//=============================================================================
//  checking : main_init
//=============================================================================
//  1. cpu vendor and brand changed ?
//  2. memory size changed ?
//  3. ABIOS or AEC changed ?
//  4. PCI device list changed ?
//  5. SMBus device changed ? (only checking device list)

//=============================================================================
//  main_init
//=============================================================================
uint8_t main_init(void)
{
	int		res = PU_OK;

	if (pu)
		free(pu);

	pu = (pu_t *)malloc(sizeof(pu_t));
	if (!pu)
	{
		fprintf(stderr, "main_init err!\n");
		res = PU_ERR_MALLOC;
	}

	memset(pu, 0, sizeof(pu_t));

	pu->cpu.boot_tsc = boot_tsc;

	pu->sort_typ	= 0xFF;	// unknown sort type

	// time zone settings : local time (Taiwan)
	// because of the default daylight savings in U.S. made time changed.

	// US Daylight Saving (before 2007) : 1st Sunday in April AM2:00 ~ last Sunday in October AM2:00)
	// US Daylight Saving (after 2007)  : 2nd Sunday in March AM2:00 ~ 1st Sunday in November AM2:00)
	// (depends on State.)
	//putenv("TZ=CST-0");
		
	return (uint8_t)res;
}
	
//=============================================================================
//  tsc_init
//=============================================================================
/*
void tsc_init(void)
{
	msgbox_doing(T_BLACK, T_LIGHTGRAY, "TSC", "Getting CPU TSC........", 4, 1);

	pu->cpu.tsc0 = rdtsc();
	delay(1000);
	pu->cpu.tsc1 = rdtsc();

	msgbox_doing_done();
}
*/

//=============================================================================
//  main_exit
//=============================================================================
uint8_t main_exit(void)
{
	if (pu)
		free(pu);

	return PU_OK;
}

//=============================================================================
//  main_scr_init
//=============================================================================
void main_scr_init(void)
{
	t_init();

	scr_show_main_body();

	scr_show_title();
}

//=============================================================================
//  main_help
//=============================================================================
void main_help(void)
{
	printf("\n%s %s %s\n", PU_DES_NAME, PU_DES_VER, PU_DES_DESC);
	printf("-------------------------------------------------------------------------------\n");
	printf("\t%s, %s\n", PU_DES_DIV, PU_DES_ORGAN);
	printf("\t%s\n\n", PU_DES_COPYR);

	printf("Online Mode >>\n");
	printf("\t[no_param]    : Recording time\n");
	printf("\t-a <low> <hi> : Assign alarm frequency\n\n");

	printf("Offline Mode >>\n");
	printf("\t-v            : View xxx.DAT file\n");
	printf("\t-h            : Help\n");
}

//=============================================================================
//  select_mode
//=============================================================================
int select_mode(int ac, char **av)
{
	uint16_t	low_alarm;
	uint16_t	hi_alarm;
	
	if (ac == 1)
	{
		// on-line
		pu->mode = PU_MODE_ON_LINE;
		sprintf(pu->file_name, "%s", "POWERUP.DAT");

		return 0;
	}
	else if (ac == 2)
	{
		// off-line
		if (strcmp(av[1], "-v") == 0)
		{
			pu->mode = PU_MODE_OFF_LINE;
		}
		else if (strcmp(av[1], "-h") == 0)
		{
			main_help();
			return -1;
		}

		return 0;	// launch!
	}
	else if (ac == 4)
	{
		if (strcmp(av[1], "-a") == 0)
		{
			low_alarm = (uint16_t)atoi(av[2]);
			hi_alarm = (uint16_t)atoi(av[3]);

			if ((low_alarm <= 20000 && low_alarm >= 20) &&
				(hi_alarm <= 20000 && hi_alarm >= 20) &&
				(hi_alarm > low_alarm))
			{
				pu->mode = PU_MODE_ON_LINE;
				sprintf(pu->file_name, "%s", "POWERUP.DAT");

				pu->alm.lo = low_alarm;
				pu->alm.hi = hi_alarm;

				return 0;	// launch!
			}
			else
			{
				main_help();
				return -1;
			}
		}
		else
		{
			main_help();
			return -1;
		}
	}
	else
	{
		printf("POWERUP        : on-line\n");
		printf("POWERUP <file> : off-line\n");
	}

	return -1;
}

EFI_STATUS EFIEnvironmentLog(void)
{
	CHAR8 fwvendor[128] = {'\0'};
	UINT32 majorver = 0, minorver = 0;
	
	if(gEfiShellProtocol != NULL)
	{
		majorver = gEfiShellProtocol->MajorVersion;
		minorver = gEfiShellProtocol->MinorVersion;
	}
	else if(mEfiShellEnvironment2 != NULL)
	{
		majorver = mEfiShellEnvironment2->MajorVersion;
		minorver = mEfiShellEnvironment2->MinorVersion;
	}
	
	UnicodeStrToAsciiStr(gST->FirmwareVendor, fwvendor);
	log_printf("EFI Shell Version=%d.%d / EFI Firmware Version=%d.%d / EFI Vendor=%s (0x%08x)", 
		majorver, minorver, (gST->Hdr.Revision&0xffff0000)>>16, (gST->Hdr.Revision&0x0000ffff), fwvendor, gST->FirmwareRevision);

	return EFI_SUCCESS;
}
//=============================================================================
//  main - function tree
//=============================================================================
// rdtsc
// main_init
// select_mode
// main_scr_init

// >> off-line
// datlst_show
// dat_init
// dat_exist
// dat_check
// dat_read
// dat_update
// scr_show_main-->

// >> on-line
// dat_exist
// par_init
// sys_init
// dat_check
// dat_read
// dat_check_board
// dat_update-->
//   upd_xxx....
// scr_show_main-->
//   scr_show_title
//   scr_show_main_keyhlp
//   scr_show_big_count
//   scr_show_time_info
//   scr_show_sta_info
//   scr_show_par_info
//   scr_show_acl_info
//   scr_show_rec_info
//   --> loop start
//   rt_tmr_handler
//     scr_show_time
//     scr_show_big_count_temp
//   scr_alarm_task
//   kbhit --> key handler
//     scr_quit_main
//     sysmenu_show
//     flmenu_show
//     scr_show_big_count_temp
//     scr_show_rec_info
//     pu_log_select
//     pu_raw_output
//     pu_err_raw_output
//     pu_help_show
//     pu_about_show
//     pu_trep_select
//     pu_sel_dat
//     pu_create_grep
//     pu_create_trep
//     pu_statis_show
//     t_scr_capture

//=============================================================================
//  main
//=============================================================================
int main(int ac, char **av)
{
	uint8_t		res;
	uint8_t		sta;
	int			ret;
	char		c;
	int			i;
	char		msg[64];

	//-------------------------------------------------------------------------
	//  tsc
	//-------------------------------------------------------------------------
	boot_tsc = rdtsc();

	//-------------------------------------------------------------------------
	//  main_init
	//-------------------------------------------------------------------------
	main_init();

	//-------------------------------------------------------------------------
	//  select mode
	//-------------------------------------------------------------------------
	if (select_mode(ac, av) < 0)
		return EFI_SUCCESS;

	//-------------------------------------------------------------------------
	//  main_scr_init
	//-------------------------------------------------------------------------
	main_scr_init();


	//-------------------------------------------------------------------------
	//  off-line : file select
	//-------------------------------------------------------------------------
	if (pu->mode == PU_MODE_OFF_LINE)
	{
		ret = datlst_show(pu->file_name, "*.DAT");

		if (ret < 0)
		{
			t_exit();
			if (ret == -1)
				printf("User aborted!\n");
			else if (ret == -2)
				printf("No DAT file found!\n");
			return ret;
		}

		for (i=0; i<strlen(pu->file_name); i++)
		{
			c = pu->file_name[i];
			if (c >= 'a' && c <= 'z')
				pu->file_name[i] &= 0xDF;	// toupper
		}
	}

	//-------------------------------------------------------------------------
	//  dat_init
	//-------------------------------------------------------------------------
	res = dat_init();
	if (res != PU_OK)
		goto err_main;

	if (pu->mode == PU_MODE_OFF_LINE)
	{
		//-------------------------------------------------------------------------
		//  OFF-line
		//-------------------------------------------------------------------------
		if (dat_exist())
		{
			sprintf(msg, "Loading %s........", pu->file_name);
			msgbox_doing(T_BLACK, T_LIGHTGRAY, "POWERUP", msg, 4, 1);

			res = dat_check();
			if (res == PU_OK)
			{
				res = dat_read();
				res = dat_update();
				msgbox_doing_done();

				scr_show_main();
			}
			else
			{
				msgbox_doing_done();
			}
		}
	}
	else
	{
		//-------------------------------------------------------------------------
		//  ON-line : 1st time
		//-------------------------------------------------------------------------
		if (!dat_exist())
		{
			sta = 0;

			EFIEnvironmentLog();//log efi shell information in first execute

par_init_again:
			
			res = par_init(sta);
			if (res == PU_ERR_ESC_ABORT)
				goto exit_main;	// user aborted

			res = sys_init();

			if (res == PU_ERR_ESC_ABORT)
			{
				sta = 7;
				goto par_init_again;	// go back par_init()
			}
			else if (res == PU_ERR_TMR_DEAD)
			{
				goto err_main;
			}
			else
			{
				// checking POWERUP.DAT exist?
				if (dat_exist() == 0)
				{
					msgbox_waitkey(T_WHITE, T_RED, "POWERUP", "Can't create POWERUP.DAT!\nPlease ask for help.", 4, 1);
					goto err_main;
				}
			}
		}
		//-------------------------------------------------------------------------
		//  ON-line : 2nd time and after
		//-------------------------------------------------------------------------
		else
		{
			msgbox_doing(T_BLACK, T_LIGHTGRAY, "POWERUP", "Loading POWERUP.DAT........", 4, 1);

			// dat check
			res = dat_check();
			if (res != PU_OK)
				goto err_main;
	
			//tsc_init();

			// dat read
			res = dat_read();
			if (res != PU_OK)
				goto err_main;

			// dat check board
			res = dat_check_board();
			if (res == PU_ERR_DAT_CHK_BRD)
				goto err_main;
			
			// dat update
			res = dat_update();
			if (res != PU_OK)
			{
				goto err_main;
			}

			// Loading POWERUP.DAT....
			msgbox_doing_done();

			// checking POWERUP.DAT exist?
			if (dat_exist() == 0)
			{
				msgbox_waitkey(T_WHITE, T_RED, "POWERUP", "Can't create POWERUP.DAT!\nPlease ask for help.", 4, 1);

				// marked : still going to show main screen
				//goto err_main;
			}

			// screen show
			scr_show_main();

			//dat_exit();
		}
	}

	//sys_exit();

	//-------------------------------------------------------------------------
	//  ready
	//-------------------------------------------------------------------------
	if (res == PU_OK)
	{
		EFITimerEventClose();
		t_exit();

		printf("\nPOWERUP %s is ready.", PU_DES_VER);
		nosound();
		return 0;
	}

	//-------------------------------------------------------------------------
	//  exit
	//-------------------------------------------------------------------------
exit_main:
	t_exit();
	EFITimerEventClose();

	printf("\nPOWERUP %s : User aborted!\nPU_ERR : %d = 0x%02X => %s\n", PU_DES_VER, res, res, pu_err_str(res));
	nosound();
	return 0;

	//-------------------------------------------------------------------------
	//  error
	//-------------------------------------------------------------------------
err_main:
	msgbox_doing_done();

	EFITimerEventClose();
	t_exit();

	printf("\nPOWERUP %s\nPU_ERR : %d = 0x%02X => %s\n", PU_DES_VER, res, res, pu_err_str(res));
	nosound();
	return EFI_SUCCESS;
}


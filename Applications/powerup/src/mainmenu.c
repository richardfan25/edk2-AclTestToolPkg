//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  MAINMENU : Main Menu                                                    *
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

#include "dat.h"
#include "pu.h"
#include "scr.h"
#include "errcode.h"
#include "flmenu.h"
#include "sysmenu.h"
#include "mainmenu.h"

//=============================================================================
//  mainmenu_vlbox
//=============================================================================
char *mainmenu_item[11] =
{
	"        C -   View : Check Flag Menu",
	"        S -   View : System Function Menu",
	"        L -   View : Log Files",
	"       F5 -   View : Text Report",
	"       F6 -   View : Record File",
	"       F9 -   View : Interval Statistic",
	"       F8 - Create : Text Report",
	"       F7 - Create : Graphic Report",
	"Backspace - Create : Error Log File for analysis",
	"       F1 -   Help : How to use POWERUP",
	"       F2 -  About : Copyright Information"
};

char *mainmenu_help[11] =
{
	"Check Flag between board and DAT file",
	"Re-Run system function for checking board",
	"Load log files for checking (*.LOG)",
	"Load text report files (*.TXT)",
	"Load record files (*.DAT)",
	"Show Interval Statistic",
	"Create Text Report File (*.TXT)",
	"Create graphic report files (*.BMP)",
	"Create error files for analysis (*.RAW, *.ERR)",
	"Help, How to use POWERUP",
	"About, copyright information"
};

t_vlbox_t	mainmenu_vlbox;

//=============================================================================
//  mainmenu_show
//=============================================================================
int mainmenu_show(void)
{
	char	str[32];
	uint8_t	sel;
	uint8_t	bk;
	int		iret = 1;
 
	mainmenu_vlbox.title	= NULL;
	mainmenu_vlbox.item	= mainmenu_item;
	mainmenu_vlbox.help	= mainmenu_help;

	mainmenu_vlbox.num_item = 11;
	mainmenu_vlbox.num_help = 11;

	mainmenu_vlbox.fg = T_BLACK;
	mainmenu_vlbox.bg = T_LIGHTGRAY;
	mainmenu_vlbox.fs = T_YELLOW;
	mainmenu_vlbox.bs = T_BLUE;
	mainmenu_vlbox.fh = T_RED;
	mainmenu_vlbox.bh = T_LIGHTGRAY;
	
	mainmenu_vlbox.hm = 1;
	mainmenu_vlbox.vm = 0;
	mainmenu_vlbox.ha = 0;
	mainmenu_vlbox.va = 0;
	
	mainmenu_vlbox.align	= T_AL_CM;
	mainmenu_vlbox.tialign= ((T_AL_CENTER<<4) | (T_AL_LEFT));
	mainmenu_vlbox.flag	= ( T_VLBOX_FL_TITLE   |
				T_VLBOX_FL_HELP	   |
				T_VLBOX_FL_KEYHELP |
				T_VLBOX_FL_SHADOW  |
				T_VLBOX_FL_FRAME );
	mainmenu_vlbox.sel	= 0;

	bk = t_get_color();
	
	sprintf(str, "%s", "Main Menu");
	mainmenu_vlbox.title = (char *)malloc(strlen(str)+1);
	if (!mainmenu_vlbox.title)
		return iret;
	sprintf(mainmenu_vlbox.title, "%s", str);

	mainmenu_vlbox.sel = 0;
	
	sel = t_show_vlbox(&mainmenu_vlbox);

	free(mainmenu_vlbox.title);
	
	if (sel == 0xFA)	// abort
		return iret;
		
	switch(sel)
	{
		case 0:		flmenu_show();			break;
		case 1:		sysmenu_show();			break;
		case 2:		pu_log_select();		break;
		case 3:		pu_trep_select();		break;
	
		case 4:
			iret = pu_sel_dat();
			break;
	
		case 5:		pu_statis_show();		break;
		case 6:		pu_create_trep();		break;
		case 7:		pu_create_grep();		break;

		case 8:
			msgbox_doing(T_WHITE, T_RED, "Output Error Log", "Create *.RAW and *.ERR files.....", 4, 1);
				
			pu_raw_output();		// POWERUP.DAT --> .RAW -> *.RAW
			pu_err_raw_output();	// POWERUP.ERR --> .ERR -> *.ERR
				
			msgbox_doing_done();
			break;

		case 9:		pu_help_show();			break;
		case 10:	pu_about_show();		break;
	}

	// recover key help
	scr_show_main_keyhlp();

	t_set_color(bk);

	return iret;
}

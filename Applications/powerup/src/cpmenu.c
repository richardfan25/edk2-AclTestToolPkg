//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  CPMENU : Check Switch Menu                                             *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_chkbox.h"
#include "t_msgbox.h"

#include "dat.h"

//=============================================================================
//  cpmenu_chkbox
//=============================================================================
char *cpmenu_item[9] =
{
	"AB : ACL BIOS",
	"AEC: ACL EC",
	"CP : CPU Info",
	"E8 : E820 Size",
	"SM : SMBIOS Data",
	"PC : PCI Bus Scan",
	"SB : SMBus Scan",
	"SP : SPD Detect",
	"RTC: RTC Check"
};

t_chkbox_t	cpmenu_chkbox;

//=============================================================================
//  cpmenu_show
//=============================================================================
uint8_t cpmenu_show(void)
{
	char	str[32];
	uint8_t	ret;

	cpmenu_chkbox.title = NULL;
	cpmenu_chkbox.item = cpmenu_item;
	
	cpmenu_chkbox.num_item = 9;
	
	cpmenu_chkbox.fg = T_WHITE;
	cpmenu_chkbox.bg = T_MAGENTA;
	cpmenu_chkbox.fs = T_YELLOW;
	cpmenu_chkbox.bs = T_BLUE;
	
	cpmenu_chkbox.hm = 1;
	cpmenu_chkbox.vm = 0;
	cpmenu_chkbox.ha = 0;
	cpmenu_chkbox.va = 0;
	cpmenu_chkbox.gap= 1;
	
	cpmenu_chkbox.align	= T_AL_CM;
	cpmenu_chkbox.tialign = ((T_AL_CENTER<<4) | (T_AL_LEFT));

	cpmenu_chkbox.flag	= ( T_CHKBOX_FL_TITLE   |
					T_CHKBOX_FL_KEYHELP |
					T_CHKBOX_FL_SHADOW  |
					T_CHKBOX_FL_FRAME );
	
	cpmenu_chkbox.sel = 0;

	sprintf(str, "%s", "Check Switch");
	cpmenu_chkbox.title = (char *)malloc(strlen(str)+1);
	if (!cpmenu_chkbox.title)
	{
		return 0;
	}
	sprintf(cpmenu_chkbox.title, "%s", str);

	cpmenu_chkbox.sel = dat->par->cmp;
	cpmenu_chkbox.sw  = dat->par->cmp;
	
	ret = t_show_chkbox(&cpmenu_chkbox);

	free(cpmenu_chkbox.title);
	
	if (ret == 0xFA)	// abort
		return ret;
	else if (ret == 0)
		dat->par->cmp = cpmenu_chkbox.sel;

	return 0;
}

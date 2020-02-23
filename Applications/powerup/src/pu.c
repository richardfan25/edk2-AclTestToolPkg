#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "t_video.h"
#include "t_msgbox.h"
#include "abios.h"
#include "sound.h"
#include "datlst.h"
#include "pu.h"
#include "errcode.h"
#include "scr.h"
#include "g_fbmp.h"

//===========================================================================
//  pu_sel_dat
//===========================================================================
int pu_sel_dat(void)
{
	uint8_t	res;
	int		ret;
	int		i;
	char	c;
	char	fname[32];
	char	msg[64];

	if (pu->mode != PU_MODE_OFF_LINE)
	{
		msgbox_waitkey(T_WHITE, T_RED, "Select DAT", "Can't load DAT in on-line mode", 4, 1);
		return -1;
	}
	else
	{
		ret = datlst_show(fname, "*.DAT");

		// check the same file name 
		if (strcmp(fname, pu->file_name) == 0)
		{
			sprintf(msg, "%s has been loaded!", fname);
			msgbox_waitkey(T_BLACK, T_LIGHTGRAY, "Select DAT", msg, 4, 1);
			return -1;
		}

		// overwrite if dat file is selected
		if (ret >= 0)
			strcpy(pu->file_name, fname);

		if (ret < 0)
		{
			msgbox_waitkey(T_WHITE, T_RED, "Select DAT", "Can't load DAT file list", 4, 1);
			return -1;
		}
		else
		{
			sound_end();

			for (i=0; i<strlen(pu->file_name); i++)
			{
				c = pu->file_name[i];

				if (c >= 'a' && c <= 'z')
					pu->file_name[i] &= 0xDF;	// toupper
			}
			
			res = dat_init();

			if (res != PU_OK)
			{
				msgbox_waitkey(T_WHITE, T_RED, "Select DAT", "dat_init error!", 4, 1);
				return -1;
				//t_exit();
				//fprintf(stderr, "dat_init error during select dat file!\n");
				//exit(0);
			}
			
			res = dat_check();
			if (res != PU_OK)
			{
				sound_end();
				msgbox_waitkey(T_WHITE, T_RED, "Select DAT", "dat_check error!", 4, 1);
				return -1;
			}
			else
			{
				res = dat_read();
				res = dat_update();
				sound_ok();
			}
		}
	}

	return 0;
}

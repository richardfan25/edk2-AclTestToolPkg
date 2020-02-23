//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ABOUT : About                                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "sound.h"
#include "pu.h"

//=============================================================================
//  about message
//=============================================================================
char pu_about_str[] = PU_DES_NAME " " PU_DES_VER " - " PU_DES_DESC "\n\n" PU_DES_DIV " " PU_DES_ORGAN "\nBuild : " __DATE__ " " __TIME__ "\n\n" PU_DES_COPYR;

#if 0
t_msgbox_t	msgbox_about =
{
	40, 11,			// cx, cy
	 4,  1,			// hm, vm

	T_BLACK,		// fg
	T_LIGHTGRAY,	// bg

	"About",		// title
	pu_about_str,	// name
	
	// flag
	TM_MSG_FL_WAITKEY |
	TM_MSG_FL_FRAME_SD |
	TM_MSG_FL_SHADOW,

	// tmout
	0
};
#endif

//=============================================================================
//  pu_about_show
//=============================================================================
void pu_about_show(void)
{
	//sound_ok();
	//t_show_msgbox(&msgbox_about);
	msgbox_waitkey(T_BLACK, T_LIGHTGRAY, "About", pu_about_str, 4, 1);
}

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_TITLE : Title                                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_title.h"

//=============================================================================
//  t_show_title
//=============================================================================
void t_show_title(t_title_t *t)
{
	int		px;
	int		nlen;
	uint8_t	bk;

	bk = t_get_color();

	// canvas
	t_color(t->fg, t->bg);
	t_put_hline(t->sx, t->sy, t->tw);

	px   = t->sx;
	nlen = (int)strlen(t->name);

	if (nlen > 80)
	{
		t->name[80] = 0;	// too long, force to shorten title name
	}
	
	switch(t->align)
	{
		case T_AL_CENTER:
			if (t->tw >= nlen)
				px = ((t->tw - nlen) >> 1);	// /2
			else
				px = 0;
			break;

		case T_AL_LEFT:
			px = t->ax;
			if ((px+nlen) >= t->tw)
				px = t->tw - nlen;
			break;

		case T_AL_RIGHT:
			px = t->ax - nlen;
			if (px < 0)
				px = 0;
			if ((px+nlen) >= t->tw)
				px = t->tw - nlen;
			break;

		default:
			px = 0;
			break;
	}

	t_gotoxy(px, t->sy);
	t_puts(t->name);

	t_set_color(bk);
}

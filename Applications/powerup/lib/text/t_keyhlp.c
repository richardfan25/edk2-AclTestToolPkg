//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_KEYHLP : Key Help                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <string.h>

#include "typedef.h"
#include "t_video.h"
#include "t_keyhlp.h"

//=============================================================================
//  t_show_keyhlp
//=============================================================================
void t_show_keyhlp(t_keyhlp_t *k)
{
	int		i;
	int		sx, sy;
	int		num;
	char	*key, *name;
	uint8_t	bk;

	bk = t_get_color();

	// canvas
	t_color(k->fg, k->bg);
	t_put_hline(k->sx, k->sy, k->kw);
	
	sx = k->sx + 1;
	sy = k->sy;
	num = k->num_key;
	for (i=0; i<num; i++)
	{
		key = k->key[i];
		t_fg_color(k->kg);
		t_xy_puts(sx, sy, key);

		sx += (int)strlen(key);
		sx += 1;

		name = k->name[i];
		t_fg_color(k->fg);
		t_xy_puts(sx, sy, name);
	
		sx += (int)strlen(name);
		sx += 2;
	}

	t_set_color(bk);
}

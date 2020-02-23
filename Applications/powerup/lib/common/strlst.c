//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - STRLIST : String List                                           *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strlst.h"
//=============================================================================
//  str_list_add
//=============================================================================
str_list_str_t *str_list_add(char *str, str_list_t *strlst)
{
	str_list_str_t		*s;

	int		len = (int)strlen(str);

	s = (str_list_str_t *)malloc(sizeof(str_list_str_t));
	if (!s)
		return NULL;

	memset(s, 0, sizeof(str_list_str_t));

	s->str = (char *)malloc(len + 1);
	if (s->str)
	{
		sprintf(s->str, "%s", str);
		s->len = len;

		if (len > strlst->maxlen)
			strlst->maxlen = len;
	}
	else
	{
		// error handling ?
	}

	if (strlst->num == 0)
	{
		strlst->head = s;
	}
	else
	{
		strlst->curr->next = s;

		s->prev = strlst->curr;
	}
	strlst->num++;
	strlst->curr = s;

	return s;
}

//=============================================================================
//  str_list_del_all
//=============================================================================
void str_list_del_all(str_list_t *strlst)
{
	str_list_str_t	*sl;
	str_list_str_t	*s;

	int		i;

	if (strlst->num)
	{
		sl = strlst->head;
		for (i=0; i<strlst->num; i++)
		{
			s = sl->next;
			if (sl->str)
				free(sl->str);
			free(sl);

			sl = s;
		}
	}
}

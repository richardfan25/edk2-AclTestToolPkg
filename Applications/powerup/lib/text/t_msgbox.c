#include <stdio.h>
#include <string.h>
//#include <conio.h>
//#include <dos.h>
//#include <bios.h>

#ifdef __WATCOMC__
#include <i86.h>
#endif

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "t_scrcap.h"
#include "key.h"

//===========================================================================
//  t_show_msgbox
//===========================================================================
uint16_t t_show_msgbox(t_msgbox_t *m)
{
	int		i, j, s;
	int		sx, sy;
	int		ex, ey;
	int		cx;	//, cy;
	int		mw, mh;
	int		mx = 0, my = 0;
	char	*msg[16];
	int		midx;
	int		tlen, nlen, slen;
	char	str[80];
	int		maxw, barlen, barx;

	uint32_t	tmout;
	uint16_t		key = 0;
	uint8_t		align;
	uint8_t		bk;

	bk = t_get_color();

	tlen = (int)strlen(m->title);
	
	// parse multiple line of message
	// msg : <line 1> \n <line 2> \n <line 3> ...
	nlen = (int)strlen(m->name);
	for (i=0, s=0, midx=0; i<nlen; i++)
	{
		// fold msg : \n or \0
		if (m->name[i] == 0x0A || (i == (nlen-1)))
		{
			msg[midx++] = &m->name[s];
			s = i + 1;
			continue;
		}
	}

	// maxw
	maxw = 78 - m->hm*2;
	
	// get max len
	for (i=0, mw=tlen; i<midx; i++)
	{
		// max len : 72 due to 80x25 and margin
		for (s=0; s<maxw; s++)
		{
			if (msg[i][s] == 0x0A || msg[i][s] == 0x00)
			{
				nlen = s;

				if (msg[i][0] == 0x0D)	// carriage return
				{
					nlen--;
					if (msg[i][1] == 0x08)	// backspace : set fg
						nlen -= 2;			// 0x08 and fg
				}

				break;
			}
		}
		if (nlen > mw)
			mw = nlen;
	}
	
	// message : multiple line

	// horizontal margin : left=4, right=4
	mw += ((m->hm * 2) + 2);	// 2:border left/right
	//if ((mw & 0x1) == 0)
	//	mw -= 1;

	// vertical margin 
	//mh = 6 + midx;
	mh = midx + 4 + (m->vm * 2);	// 4:title + border top/bottom

	// without title
	if (tlen == 0)
		mh -= 2;

	// sx, sy, ex, ey
	sx = m->cx - (mw>>1);
	if (sx < 0)
		sx = 0;
	sy = m->cy - (mh>>1) + 1;
	if (sy < 0)
		sy = 0;

	ex = sx + mw - 1;
	ey = sy + mh - 1;

	// center x, y
	//cx = ((sx + ex) >> 1);
	cx = sx + (mw>>1);
	//cy = ((sy + ey) >> 1);	// unused
	
	if (m->flag & TM_MSG_FL_SHADOW)
	{
		ex++;
		ey++;
		
		if (ex >= 80 && sx > 0)
		{
			ex--;	// left-shift
			sx--;
		}
		if (ey >= 24 && sy > 0)
		{
			ey--;	// up-shift
			sy--;
		}
	}

	t_color(m->fg, m->bg);

	if ((m->flag & TM_MSG_FL_PROGRESS) == 0)
		t_copy_region(sx, sy, ex, ey);

	// window
	if (tlen)
	{
		// with title
		switch(m->flag & TM_MSG_FL_FRAME)
		{
			case TM_MSG_FL_FRAME_S  :	t_put_win_single(sx, sy, mw, mh);			break;
			case TM_MSG_FL_FRAME_SD :	t_put_win_single_double(sx, sy, mw, mh);	break;
			case TM_MSG_FL_FRAME_DS :	t_put_win_double_single(sx, sy, mw, mh);	break;
			case TM_MSG_FL_FRAME_D  :	t_put_win_double(sx, sy, mw, mh);			break;
		}
	}
	else
	{
		// without title
		switch(m->flag & TM_MSG_FL_FRAME)
		{
			case TM_MSG_FL_FRAME_S:
			case TM_MSG_FL_FRAME_SD:
				t_put_rect_single_solid(sx, sy, mw, mh);
				break;

			case TM_MSG_FL_FRAME_DS:
			case TM_MSG_FL_FRAME_D:
				t_put_rect_double_solid(sx, sy, mw, mh);
				break;
		}
	}
	
	// with title
	if (tlen)
	{
		mx = cx - (int)(strlen(m->title) / 2);
		//if (mx < 0)
		//	mx = 4;
		my = sy + 1;
		t_xy_puts(mx, my, m->title);
	}

	if (m->flag & TM_MSG_FL_PROGRESS_BAR)
	{
		maxw = mw - 4;
		barlen = ((mw - 4) * m->percent) / 100;
		
		mx = sx + 2;
		// todo
		my = sy + 1 + m->vm + 2;
		
		for (i=0; i<maxw; i++)
		{
			if (i < barlen)
			{
				t_color(T_RED, T_LIGHTGRAY);
				t_xy_putc(mx+i, my, 0xDB);
			}
			else
			{
				t_color(T_LIGHTGRAY, T_RED);
				t_xy_putc(mx+i, my, 0xDB);
			}
			
			t_color(T_BLACK, T_LIGHTGRAY);
			sprintf(str, "%d%%", m->percent);

			barx = (cx-(int)(strlen(str)/2));
			for (s=0; s<strlen(str); s++)
			{
				if ((s+barx) <= (mx+barlen))
					t_color(T_LIGHTGRAY, T_RED);
				else
					t_color(T_RED, T_LIGHTGRAY);
				t_xy_putc(s+barx, my, str[s]);
			}
		}
		
		t_color(m->fg, m->bg);
	}
	else
	{
			
		// message name : multiple line
		for (i=0; i<midx; i++)
		{
			for (s=0; s<80; s++)
			{
				// \n : 0x0A
				if (msg[i][s] == 0x0A || msg[i][s] == 0x00)
				{
					str[s] = 0;
					break;
				}
				else
				{
					str[s] = msg[i][s];
				}
			}

			align = 0;

			for (s=0; s<strlen(str); s++)
			{
				if (s == 0)
				{
					slen = 0;
					for (j=0, slen=0; j<strlen(str); j++)
					{
						// < 0x10 : c
						if (str[j] > 0x10)
							slen++;
					}
						
					// \r = 0x0D : carriage return
					if (str[s] == 0x0D)
						align |= 0x1;	// align-left
					
					if (align & 0x1)
						mx = sx + 1 + m->hm;	// align-left, 1:border
					else
						mx = cx - (int)(slen/2);	// align-center

					my = sy + i + 1 + m->vm;	// 1:border

					if (tlen)
						my += 2;	// with title

					if (str[s] == 0x0D)
						continue;
				}

				// \b = 0x08 : with fg color
				if (str[s] == 0x08)
				{
					align |= 0x2;
					s++;
					t_fg_color(msg[i][s] & 0xF);
					continue;
				}
			
				t_xy_putc(mx++, my, str[s]);
			}
		}
	}

	if (m->flag & TM_MSG_FL_SHADOW)
	{
		for (i=sx+1; i<=sx+mw; i++)
			t_shadow_char(i, sy+mh);	// shadow : bottom border

		for (i=sy+1; i<=sy+mh; i++)
			t_shadow_char(sx+mw+0, i);	// shadow : right border
	}

	key_flush_buf();
	
	if (m->flag & TM_MSG_FL_WAITKEY)
	{
key_again:

		key = (uint16_t)bioskey(0);//key_blk_read_sc();
		
		if (key == (SCAN_F10 << 8))
		{
			t_scr_capture();
			goto key_again;
		}
	}
	else if (m->flag & TM_MSG_FL_TIMEOUT)
	{
		tmout = m->tmout;

		#ifdef __BORLANDC__
		do
		{
			delay(1);
			
			//if (kbhit())
			{
				if (key_non_blk_read_sc() == KEY_SC_F10)//if ((bioskey(0) >> 8 & 0xFF) == KEY_F10)
					t_capture_screen();
			}
			
			if (tv.update_func)
				tv.update_func();

		} while (--tmout);
		#endif
		
		#ifdef __WATCOMC__
		#if 0
		hpet_set_tmout(m->tmout);
		hpet_start();
		while (1)
		{
			if (hpet_is_tmout())
				break;
			
			//if (kbhit())
			{
				if (key_non_blk_read_sc() == KEY_SC_F10)//if ((bioskey(0) >> 8 & 0xFF) == KEY_F10)
					t_capture_screen();
			}
			
			if (tv.update_func)
				tv.update_func();
		}
		hpet_stop();
		#endif

		//tmout += 54;
		//tmout /= 55;	// 18.2Hz = 55ms

		while (tmout--)
		{
			delay(1);	// weird? 55ms instead of 1ms

			key = (uint16_t)bioskey(1);
			if(key)
			{
				if (key == (SCAN_F10 << 8))//if (key_non_blk_read_sc() == KEY_SC_F10)
					t_scr_capture();
					
			}
			
			if (tv.update_func)
				tv.update_func();
		} 
		
		#endif
	}

	if ((m->flag & TM_MSG_FL_NO_RECOVER) == 0)
		t_paste_region(-1, -1);

	t_set_color(bk);
	
	return key;
}

//=============================================================================
//  t_msgbox_waitkey
//=============================================================================
t_msgbox_t	t_msgbox_waitkey =
{
	40, 11,		// cx, cy
	 4,  2,		// hm, vm
	
	T_WHITE,	// fg
	T_RED,		// bg
	
	NULL,		// title
	NULL,		// message string
	
	// flag
	TM_MSG_FL_WAITKEY  |
	TM_MSG_FL_FRAME_SD |
	TM_MSG_FL_SHADOW,

	0
};

//=============================================================================
//  msgbox_waitkey
//=============================================================================
uint16_t msgbox_waitkey(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm)
{
	t_msgbox_waitkey.fg		= fg;
	t_msgbox_waitkey.bg		= bg;
	t_msgbox_waitkey.title	= title;
	t_msgbox_waitkey.name	= name;
	t_msgbox_waitkey.hm		= hm;
	t_msgbox_waitkey.vm		= vm;

	return t_show_msgbox(&t_msgbox_waitkey);
}

//=============================================================================
//  t_msgbox_doing
//=============================================================================
t_msgbox_t	t_msgbox_doing =
{
	40, 11,		// cx, cy
	 4,  2,		// hm, vm
	
	T_BLACK,	// fg
	T_CYAN,		// bg
	
	NULL,		// title
	NULL,		// message string
	
	// flag
	TM_MSG_FL_NO_RECOVER	|
	TM_MSG_FL_FRAME_SD 		|
	TM_MSG_FL_SHADOW,

	0
};

//=============================================================================
//  msgbox_doing
//=============================================================================
uint16_t msgbox_doing(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm)
{
	t_msgbox_doing.fg		= fg;
	t_msgbox_doing.bg		= bg;
	t_msgbox_doing.title	= title;
	t_msgbox_doing.name		= name;
	t_msgbox_doing.hm		= hm;
	t_msgbox_doing.vm		= vm;

	return t_show_msgbox(&t_msgbox_doing);
}

//=============================================================================
//  msgbox_doing_done
//=============================================================================
void msgbox_doing_done(void)
{
	t_paste_region(-1, -1);
}

//=============================================================================
//  tmout_msg
//=============================================================================
t_msgbox_t	t_msgbox_tmout =
{
	40, 11,		// cx, cy
	 4,  2,		// hm, vm
	
	T_BLACK,	// fg
	T_CYAN,		// bg
	
	NULL,		// title
	NULL,		// message string
	
	// flag
	TM_MSG_FL_TIMEOUT	|
	TM_MSG_FL_FRAME_SD	|
	TM_MSG_FL_SHADOW,

	// ms
	1000
};

//=============================================================================
//  msgbox_tmout
//=============================================================================
uint16_t msgbox_tmout(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm, uint32_t tmout)
{
	t_msgbox_tmout.fg	= fg;
	t_msgbox_tmout.bg	= bg;
	t_msgbox_tmout.title= title;
	t_msgbox_tmout.name	= name;
	t_msgbox_tmout.hm	= hm;
	t_msgbox_tmout.vm	= vm;
	t_msgbox_tmout.tmout= tmout;

	return t_show_msgbox(&t_msgbox_tmout);
}

//=============================================================================
//  t_msgbox_progress
//=============================================================================
t_msgbox_t	t_msgbox_progress =
{
	40, 11,		// cx, cy
	 4,  2,		// hm, vm
	
	T_BLACK,	// fg
	T_CYAN,		// bg
	
	NULL,		// title
	NULL,		// message string
	
	// flag
	TM_MSG_FL_NO_RECOVER	|
	TM_MSG_FL_FRAME_SD 		|
	TM_MSG_FL_SHADOW,

	0
};

//=============================================================================
//  msgbox_progress : 1st time
//=============================================================================
uint16_t msgbox_progress(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm)
{
	uint16_t		res;

	t_msgbox_progress.fg	= fg;
	t_msgbox_progress.bg	= bg;
	t_msgbox_progress.title	= title;
	t_msgbox_progress.name	= name;
	t_msgbox_progress.hm	= hm;
	t_msgbox_progress.vm	= vm;

	res = t_show_msgbox(&t_msgbox_progress);

	t_msgbox_progress.flag |= TM_MSG_FL_PROGRESS;

	return res;
}

//=============================================================================
//  msgbox_progress_cont : continue
//=============================================================================
void msgbox_progress_cont(char *title, char *name)
{
	t_msgbox_progress.title	= title;
	t_msgbox_progress.name	= name;

	t_show_msgbox(&t_msgbox_progress);
}

//=============================================================================
//  msgbox_progress_bar : continue
//=============================================================================
void msgbox_progress_bar(char *title, uint8_t percent)
{
	t_msgbox_progress.title	= title;
	
	t_msgbox_progress.flag |= TM_MSG_FL_PROGRESS_BAR;
	t_msgbox_progress.percent = percent;

	t_show_msgbox(&t_msgbox_progress);
}

//=============================================================================
//  msgbox_progress_done : done
//=============================================================================
void msgbox_progress_done(void)
{
	t_msgbox_progress.flag &= ~TM_MSG_FL_PROGRESS;
	t_msgbox_progress.flag &= ~TM_MSG_FL_PROGRESS_BAR;
	t_paste_region(-1, -1);
}

//=============================================================================
//  msgbox_msg_init
//=============================================================================
void msgbox_msg_init(t_msgbox_msg_t *msg)
{
	memset(msg->buf, 0, 2048);
	msg->idx = 0;
}
	
//=============================================================================
//  msgbox_msg_add_str
//=============================================================================
void msgbox_msg_add_str(char *str, t_msgbox_msg_t *msg)
{
	int		len = (int)strlen(str);

	if ((msg->idx + len) >= 2048)
		return;

	sprintf(&msg->buf[msg->idx], "%s", str);
	msg->idx += len;
}

//=============================================================================
//  msgbox_msg_add_strn
//=============================================================================
void msgbox_msg_add_strn(char *str, int n, t_msgbox_msg_t *msg)
{
	int		i;
	char	c;

	for (i=0; i<n; i++)
	{
		c = str[i];
		if (c == 0)
			break;	// null
		msg->buf[msg->idx++] = c;
	}
}

//=============================================================================
//  msgbox_msg_add_char
//=============================================================================
void msgbox_msg_add_char(char c, t_msgbox_msg_t *msg)
{
	if (msg->idx >= 2047)
		return;

	msg->buf[msg->idx] = c;
	msg->idx++;
}

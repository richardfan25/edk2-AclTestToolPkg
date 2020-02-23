//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_MSGBOX (Text Message Box)                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_MSGBOX_H
#define __TEXT_T_MSGBOX_H

#include "typedef.h"

//=============================================================================
//  define
//=============================================================================
// msg flag
#define TM_MSG_FL_FRAME			0x03	// bit[1:0]- 00=single, 01=single-double, 10=double-single, 11=double
#define TM_MSG_FL_SHADOW		0x04	// bit[2]  - 1=frame shadow, 0=no shadow
#define TM_MSG_FL_NO_RECOVER	0x08	// bit[3]  - 1=no recover, 0=recover
#define TM_MSG_FL_PROGRESS_BAR	0x10	// bit[4]  - 1=with bar, 0=no bar
#define TM_MSG_FL_PROGRESS		0x20	// bit[5]  - 0=copy region 1st time, 1=not copy
#define TM_MSG_FL_TIMEOUT		0x40	// bit[6]  - time out
#define TM_MSG_FL_WAITKEY		0x80	// bit[7]  - wait key pressed

#define TM_MSG_FL_FRAME_S	0		// single
#define TM_MSG_FL_FRAME_SD	1		// single double
#define TM_MSG_FL_FRAME_DS	2		// double single
#define TM_MSG_FL_FRAME_D	3		// double

//=============================================================================
//  t_msgbox_t
//=============================================================================
typedef struct _t_msgbox_t
{
	int			cx, cy;		// center pos
	int			hm, vm;		// margin : horizontal/vertical

	uint8_t		fg, bg;		// color

	char		*title;
	char		*name;

	uint8_t		flag;		// TM_FL_WAITKEY, TM_FL_TIMEOUT
	uint32_t	tmout;		// ms
	uint8_t		percent;	// percent : 0%~100%
	
} t_msgbox_t;

//=============================================================================
//  t_msgbox_msg_t
//=============================================================================
typedef struct _t_msgbox_msg_t
{
	uint8_t		buf[2048];
	int			idx;

} t_msgbox_msg_t;

//=============================================================================
//  functions
//=============================================================================
uint16_t t_show_msgbox(t_msgbox_t *m);

// waitkey msg
uint16_t msgbox_waitkey(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm);

// doing msg
uint16_t msgbox_doing(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm);
void	msgbox_doing_done(void);

// tmout msg
uint16_t msgbox_tmout(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm, uint32_t tmout);

// progress msg
uint16_t msgbox_progress(uint8_t fg, uint8_t bg, char *title, char *name, int hm, int vm);
void	msgbox_progress_cont(char *title, char *name);
void	msgbox_progress_bar(char *title, uint8_t percent);
void	msgbox_progress_done(void);

// msg
void	msgbox_msg_init(t_msgbox_msg_t *msg);
void	msgbox_msg_add_str(char *str, t_msgbox_msg_t *msg);
void	msgbox_msg_add_strn(char *str, int n, t_msgbox_msg_t *msg);
void	msgbox_msg_add_char(char c, t_msgbox_msg_t *msg);

#endif

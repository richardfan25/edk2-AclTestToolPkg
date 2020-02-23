//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_CHKBOX : Check Box                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_CHKBOX_H
#define __TEXT_T_CHKBOX_H


//=============================================================================
//  define
//=============================================================================
// flag
#define T_CHKBOX_FL_TITLE		0x80
#define T_CHKBOX_FL_KEYHELP		0x20
#define T_CHKBOX_FL_SHADOW		0x10
#define T_CHKBOX_FL_FRAME		0x08

// err
#define T_CHKBOX_ERR_NONE		0
#define T_CHKBOX_ERR_HEIGHT		0xfd
#define T_CHKBOX_ERR_WIDTH		0xfe
#define T_CHKBOX_ERR_NO_ITEM	0xff


#pragma pack(1)
//=============================================================================
//  t_chkbox_t
//=============================================================================
typedef struct _t_chkbox_t
{
	int			sx, sy;
	int			mw, mh;
	int			hm, vm;		// hori/vert margin
	int			ha, va;		// h/v align margin
	int			gap;		// gap between check mark and item name
	int			tlen;

	char		*title;
	char		**item;
	int			num_item;

	uint8_t		fg, bg;	// fg/bg color
	uint8_t		fs, bs;	// fg/bg color of selected item

	uint16_t	sel;
	uint16_t	sw;			// bit=0, can't switch, 1=can switch

	uint8_t		err;
	uint8_t		flag;		// keyhelp, shadow...
	uint8_t		align;
	uint8_t		tialign;	// bit[7:4]=title, bit[3:0]=item

} t_chkbox_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_chkbox(t_chkbox_t *cb);

#endif

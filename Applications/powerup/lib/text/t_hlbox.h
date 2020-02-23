//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_HLBOX : Horizontal List Box                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_HLBOX_H
#define __TEXT_T_HLBOX_H

//=============================================================================
//  hlbox
//=============================================================================
//sx,sy
//  +----------------------------------------------------------+
//  |                        title                             |
//  +----------------------------------------------------------+
//  |                         vm                               |
//  |                                                          |
//  |hm  item_0   item_1   item_2   item_3   item_4   item_5 hm|
//  |          gap      gap      gap      gap      gap         |
//  |                         vm                               |
//  +----------------------------------------------------------+
//  |                        help                              |
//  +----------------------------------------------------------+
//
//	sx,sy - start x, y
//	mw,mh - width, height
//	hm    - horizontal margin of hlbox
//	vm    - vertical margin of hlbox
//  item  - hlbox items
//  title - hlbox title
//	help  - hlbox help (each item has each help)
//	ha    - horizontal alignment margin of screen
//	va    - verital alignment margin of screen
//

//=============================================================================
//  define
//=============================================================================
// flag
#define T_HLBOX_FL_TITLE		0x80
#define T_HLBOX_FL_HELP			0x40
#define T_HLBOX_FL_KEYHELP		0x20
#define T_HLBOX_FL_SHADOW		0x10
#define T_HLBOX_FL_FRAME		0x08

// err
#define T_HLBOX_ERR_NONE		0
#define T_HLBOX_ERR_MALLOC		0xfc
#define T_HLBOX_ERR_HEIGHT		0xfd
#define T_HLBOX_ERR_WIDTH		0xfe
#define T_HLBOX_ERR_NO_ITEM		0xff


#pragma pack(1)

//=============================================================================
//  t_hlbox_t
//=============================================================================
typedef struct _t_hlbox_t
{
	int			sx, sy;
	int			mw, mh;
	int			hm, vm;		// hori/vert margin
	int			ha, va;		// h/v align margin
	int			gap;		// item gap
	int			tlen, hlen;

	char		*title;
	char		**item;
	char		**help;
	int			num_item;
	int			num_help;

	uint8_t		fg, bg;	// fg/bg color
	uint8_t		fs, bs;	// fg/bg color of selected item
	uint8_t		fh, bh;	// fg/bg color of help

	uint8_t		sel;		// default select
	uint8_t		err;
	uint8_t		flag;		// keyhelp, shadow...
	uint8_t		align;		// bit[7:0]=left,center,right / top,middle,bottom

} t_hlbox_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_hlbox(t_hlbox_t *hb);

#endif

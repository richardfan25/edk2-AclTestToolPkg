//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_VLBOX : Vertical List Box                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_VLBOX_H
#define __TEXT_T_VLBOX_H

//=============================================================================
//  vlbox
//=============================================================================
//
// sx,sy    mw
//  +-------------------+		sx,sy - start x, y
//  |      title        |		mw,mh - width, height
//  +-------------------+		vm    - vertical margin of vlbox
//  |       vm          |		hm    - horizontal margin of vlbox
//  |                   |       item  - vlbox items
//  |      item 0       |		title - vlbox title
//  | hm   item 1    hm | mh	help  - vlbox help (each item has each help)
//  |      ......       |		va    - verital alignment margin of screen
//  |      item n       |		ha    - horizontal alignment margin of screen
//  |                   |
//  |       vm          |
//  +-------------------+
//  |      help         |
//  +-------------------+
//

//=============================================================================
//  define
//=============================================================================
// flag
#define T_VLBOX_FL_TITLE		0x80
#define T_VLBOX_FL_HELP			0x40
#define T_VLBOX_FL_KEYHELP		0x20
#define T_VLBOX_FL_SHADOW		0x10
#define T_VLBOX_FL_FRAME		0x08

// err
#define T_VLBOX_ERR_NONE		0
#define T_VLBOX_ERR_HEIGHT		0xfd
#define T_VLBOX_ERR_WIDTH		0xfe
#define T_VLBOX_ERR_NO_ITEM		0xff


#pragma pack(1)
//=============================================================================
//  t_vlbox_t
//=============================================================================
typedef struct _t_vlbox_t
{
	int			sx, sy;		// start x,y
	int			mw, mh;		// width, height
	int			hm, vm;		// h/v margin
	int			ha, va;		// h/v align margin

	int			tlen, hlen;	// length of title, help

	char		*title;
	char		*field;
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
	uint8_t		align;		// vlbox align :      bit[7:0]=left,center,right / top,middle,bottom
	uint8_t		tialign;	// title/item align : bit[7:4]=title align, bit[3:0]=item align

} t_vlbox_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_vlbox(t_vlbox_t *vb);

#endif

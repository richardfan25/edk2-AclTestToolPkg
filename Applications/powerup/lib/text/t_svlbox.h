//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_SVLBOX : Scroll Vertical List Box                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_SVLBOX_H
#define __TEXT_T_SVLBOX_H

//=============================================================================
//  svlbox
//=============================================================================
//
// sx,sy    mw
//  +-------------------+		sx,sy - start x, y
//  |      title        |		mw,mh - width, height
//  +-------------------+
//  |       tm          |
//  |      <field>      |       field - field name
//  |                   |       item  - svlbox items
//  |      item 0       |		title - svlbox title
//  | lm   item 1    rm | mh	help  - svlbox help (each item has each help)
//  |      ......       |		va    - verital alignment margin of screen
//  |      item n       |		ha    - horizontal alignment margin of screen
//  |                   |
//  |       bm          |		tm    - top margin of svlbox
//  +-------------------+		bm    - bottom margin of svlbox
//  |      help         |		lm    - left margin of svlbox
//  +-------------------+		rm    - right margin of svlbox
//

//=============================================================================
//  define
//=============================================================================
// flag
#define T_SVLBOX_FL_TITLE		0x80
#define T_SVLBOX_FL_HELP		0x40
#define T_SVLBOX_FL_KEYHELP		0x20
#define T_SVLBOX_FL_SHADOW		0x10
#define T_SVLBOX_FL_FRAME		0x08
#define T_SVLBOX_FL_FIELD		0x04
#define T_SVLBOX_FL_IND			0x02

// err
#define T_SVLBOX_ERR_NONE		0
#define T_SVLBOX_ERR_HEIGHT		0xfd
#define T_SVLBOX_ERR_WIDTH		0xfe
#define T_SVLBOX_ERR_NO_ITEM	0xff


#pragma pack(1)
//=============================================================================
//  t_svlbox_t
//=============================================================================
typedef struct _t_svlbox_t
{
	int			sx, sy;		// start x,y
	int			mw, mh;		// width, height
	
	int			wh;			// window height
	int			tm, bm;		// top/bottom margin
	int			lm, rm;		// left/right margin

	int			tlen, hlen;	// length of title, help

	char		*title;
	char		*field;
	char		**item;
	char		**help;


	int			num_item;
	int			num_help;

	uint8_t		fg, bg;		// fg/bg color
	uint8_t		fs, bs;		// fg/bg color of selected item
	uint8_t		ft, bt;		// fg/bg color of title
	uint8_t		fi, bi;		// fg/bg color of ind (indicator)
	uint8_t		ff, bf;		// fg/bg color of field
	uint8_t		fn, bn;		// fg/bg color of item name
	uint8_t		fh, bh;		// fg/bg color of help

	uint8_t		ind;		// indicator : 1/N, 2/N...
	uint8_t		sel;		// default select
	uint8_t		err;
	uint8_t		flag;		// keyhelp, shadow...
	int			si;
	
	uint8_t		align;		// svlbox align :      bit[7:0]=left,center,right / top,middle,bottom
	int			ho, vo;		// h/v alignment offset
	
	uint8_t		ta;			// title alignment
	uint8_t		ia;			// item  alignment
	uint8_t		ha;			// help  alignment

} t_svlbox_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_svlbox(t_svlbox_t *svb);

#endif

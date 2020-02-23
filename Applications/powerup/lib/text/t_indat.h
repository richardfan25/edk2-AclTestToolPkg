//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_INDAT : Input Data                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_INDAT_H
#define __TEXT_T_INDAT_H

#define T_INDAT_HEX			0x80	// bit[7]
#define T_INDAT_DEC			0x00

#define T_INDAT_SIGNED		0x40	// bit[6]
#define T_INDAT_UNSIGNED	0x00

#define T_INDAT_MAX_MIN		0x08	// bit[3]

#define T_INDAT_BYTE		0x01	// bit[2:0]
#define T_INDAT_WORD		0x02
#define T_INDAT_DWORD		0x04
#define T_INDAT_SZ_MASK		0x07

#pragma pack(1)

//=============================================================================
//  t_indat_t
//=============================================================================
typedef struct _t_indat_t
{
	int			sx, sy;
	int			mw, mh;
	int			tlen, hlen;

	char		*title;
	char		*field;
	char		*unit;
	char		*help;

	uint8_t		fg, bg;	// fg/bg color
	uint8_t		fi, bi;	// fg/bg color of input field
	uint8_t		fh, bh;	// fg/bg color of help

	uint8_t		sel;
	uint8_t		err;
	uint8_t		shadow;
	uint8_t		align;
	uint8_t		mode;
	uint8_t		keyhelp;

	uint32_t	data;
	uint32_t	max;	// data max
	uint32_t	min;	// data min

} t_indat_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_indat(t_indat_t *indat);

#endif

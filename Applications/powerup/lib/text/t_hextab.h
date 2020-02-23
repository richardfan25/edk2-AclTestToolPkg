//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_HEXTAB : Hex Table                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_HEXTAB_H
#define __TEXT_T_HEXTAB_H

#pragma pack(1)

//=============================================================================
//  define
//=============================================================================
// bit[1:0]
#define T_HEX_FL_IDX_BYTE		0x0
#define T_HEX_FL_IDX_WORD		0x1
#define T_HEX_FL_IDX_DWORD		0x2
#define T_HEX_FL_IDX_MASK		0x03

// bit[2]
#define T_HEX_FL_ASC			0x4

// bit[3]
#define T_HEX_FL_IDX			0x8

// bit[4]
#define T_HEX_FL_FRM			0x10

// bit[5]
#define T_HEX_FL_SHADOW			0x20



//=============================================================================
//  t_hextab_t
//=============================================================================
typedef struct _t_hextab_t
{
	int			sx, sy;	// left/top if align==none
	int			hm, vm;	// horizonal/verital margin

	uint8_t		fg, bg;	// fg/bg color
	uint8_t		ft, bt;	// fg/bg color of title
	uint8_t		fi, bi;	// fg/bg color of index

	char		*title;
	uint8_t		flag;
	uint8_t		align;

	uint8_t		*data;
	uint16_t	len;

	uint32_t	stidx;

} t_hextab_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
uint8_t t_show_hextab(t_hextab_t *ht);

#endif

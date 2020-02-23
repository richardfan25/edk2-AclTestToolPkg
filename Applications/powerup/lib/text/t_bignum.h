//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_BIGNUM : show big number in text mode screen                    *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_BIGNUM_H
#define __TEXT_T_BIGNUM_H

//=============================================================================
//  define
//=============================================================================
#define T_BIGNUM_PAD0_DIS		0
#define T_BIGNUM_PAD0_EN		1

#define T_BIGNUM_ALIGN_LEFT		0
#define T_BIGNUM_ALIGN_RIGHT	1

//#define MAX_UINT32				(4294967295)
//#define MAX_UINT16				(65535)
//#define MAX_UINT8				(255)

#pragma pack(1)
//=============================================================================
//  t_big_num_t
//=============================================================================
typedef struct _t_big_num_t
{
	int			x;		// start pos x
	int			y;		// start pos y
	uint32_t	num;	// number to show
	uint8_t		pad0;	// padding 0 in high-digits
	uint8_t		fg;		// foreground color
	uint8_t		bg;		// background color
	uint8_t		digit;	// 0=no assigned, 3=3-digits, 5=5-digits or more
	uint8_t		align;	// 0=left align, 1=right align

} t_big_num_t;
#pragma pack()
//=============================================================================
//  functions
//=============================================================================
//void t_show_big_num(int x, int y, int num, int padzero);
//void t_show_big_hex(int x, int y, uint32_t num);
//void t_show_big_dec(int x, int y, uint32_t num);

void t_show_big_hex(t_big_num_t *bn);
void t_show_big_dec(t_big_num_t *bn);
void t_show_big_chr(t_big_num_t *bn);
void t_show_big_str(t_big_num_t *bn, char *str);

#endif

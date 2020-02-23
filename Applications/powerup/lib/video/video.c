//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  VIDEO - VIDEO (INT 10h - Video Service)                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifdef __BORLANDC__
#include <dos.h>
#include <conio.h>
#include <string.h>
#endif

#ifdef __WATCOMC__
#include <stdio.h>
#include <dos.h>
#include <i86.h>
#include <string.h>
#endif

#include "video.h"

//===========================================================================
//  INT 10h : video service
//===========================================================================
// AH
// 00h : set video mode
// 01h : change cursor size
// 02h : set cursor position
// 03h : get cursor position
// 04h : read light-pen posision (not work in VGA)
// 05h : select active display page
// 06h : scroll up the screen or a window
// 07h : scroll down the screen or a window
// 08h : read character at cursor
// 09h : write character and attribute
// 0Ah : write character
// 0Eh : display character and advance cursor
//===========================================================================

//===========================================================================
//  00h : video_set_mode
//===========================================================================
//  IN - AH=00h
//       AL=video mode
//  OUT- AL bit[7]:video mode flag
//===========================================================================
void video_set_mode(uint8_t mode)
{
	/*union	REGS 	regs;

	regs.h.ah = 0x00;
	regs.h.al = mode;

	#ifdef __BORLANDC__
	int86(0x10, &regs, &regs);
	#endif
	
	#ifdef __WATCOMC__
	int386(0x10, &regs, &regs);
	#endif*/
}

//===========================================================================
//  01h : video_set_cur_type
//===========================================================================
//  IN - AH=01h
//       CH=cursor starting scan line (top)
//       CL=cursor ending scan line (bottom)
//  OUT- nothing
//===========================================================================
void video_set_cur_type(uint8_t top, uint8_t bottom)
{
	/*union	REGS 	regs;

	regs.h.ah = 0x01;
	regs.h.ch = top;
	regs.h.cl = bottom;

	#ifdef __BORLANDC__	
	int86(0x10, &regs, &regs);
	#endif
	
	#ifdef __WATCOMC__
	int386(0x10, &regs, &regs);
	#endif*/
}

//===========================================================================
//  01h : video_hide_cur
//===========================================================================
void video_hide_cur(void)
{
	/*#ifdef __BORLANDC__
	_setcursortype(_NOCURSOR);	// hide cursor
	#endif

	
	#ifdef __WATCOMC__
	union	REGS 	regs;

	regs.h.ah = 0x01;
	regs.h.al = 0x00;
	regs.h.ch = 0x20;
	regs.h.cl = 0x00;
	int386(0x10, &regs, &regs);
	#endif*/
}

//===========================================================================
//  01h : video_show_cur
//===========================================================================
void video_show_cur(void)
{
	/*#ifdef __BORLANDC__
	_setcursortype(_NORMALCURSOR);	// show cursor
	#endif

	#ifdef __WATCOMC__
	union REGS regs;

	regs.h.ah = 0x01;
	regs.h.al = 0x00;
	regs.h.ch = 0x0c;
	regs.h.cl = 0x0d;
	int386(0x10, &regs, &regs);
	#endif*/
}

//===========================================================================
//  02h : video_set_cur_pos
//===========================================================================
//  IN - AH=02h
//       BH=page number (0 for graphic mode)
//       DH=row
//       DL=column
//  OUT- nothing
//===========================================================================
void video_set_cur_pos(int x, int y)
{
	/*union REGS regs;
	
	regs.h.ah = 0x02;
	regs.h.dh = y;
	regs.h.dl = x;
	regs.h.bh = 0x00;

	#ifdef __BORLANDC__	
	int86(0x10, &regs, &regs);
	#endif
	
	#ifdef __WATCOMC__
	int386(0x10, &regs, &regs);
	#endif*/
	
}

//===========================================================================
//  03h : video_get_cur_pos
//===========================================================================
//  IN - AH=03h
//       BH=page number (0 for graphic mode)
//  OUT- CH=cursor starting scan line (top)
//       CL=cursor ending scan line (bottom)
//       DH=row
//       DL=column
//===========================================================================
void video_get_cur_pos(int *x, int *y)
{
	/*union REGS regs;

	regs.h.ah = 0x03;
	regs.h.bh = 0x00;

	#ifdef __BORLANDC__	
	int86(0x10, &regs, &regs);
	#endif
	
	#ifdef __WATCOMC__
	int386(0x10, &regs, &regs);
	#endif

	*x = regs.h.dl;
	*y = regs.h.dh;*/
}

//===========================================================================
//  06h : video_scroll_up_win
//===========================================================================
//  IN - AH=03h
//       BH=page number (0 for graphic mode)
//  OUT- CH=cursor starting scan line (top)
//       CL=cursor ending scan line (bottom)
//       DH=row
//       DL=column
//===========================================================================
#if 0
void video_scroll_up_win(int sx, int sy, int ex, int ey, int num
{
	union REGS regs;

	regs.h.ah = 0x03;
	regs.h.bh = 0x00;

	#ifdef __BORLANDC__	
	int86(0x10, &regs, &regs);
	#endif
	
	#ifdef __WATCOMC__
	int386(0x10, &regs, &regs);
	#endif

	*x = regs.h.dl;
	*y = regs.h.dh;
}
#endif

//=============================================================================
//  video_get_8x16_charset
//=============================================================================
int video_get_8x16_charset(uint8_t *font, uint32_t fontsz)
{
	/*void far	*charset;

	#ifdef __BORLANDC__
	struct	  	REGPACK	regpack;
	regpack.r_ax = 0x1130;	// AH=11h, AL=30h
	regpack.r_bx = 0x0600;	// 8x16 charset
	intr(0x10, &regpack);
	charset = MK_FP(regpack.r_es, regpack.r_bp);
	#endif


	#ifdef __WATCOMC__
	union	  	REGPACK	regpack;
	regpack.x.eax = 0x1130;	// AH=11h, AL=30h
	regpack.x.ebx = 0x0600;	// 8x16 charset
	intr(0x10, &regpack);
	charset = MK_FP(regpack.w.es, regpack.w.bp);
	#endif
	
	
	if (!font)
		return -1;

	// load font char
	// per char  : 16 bytes
	// 256 chars : 16*256=4096bytes
	memcpy(font, (uint8_t*)charset, sizeof(uint8_t)*fontsz);

	return 0;*/
}

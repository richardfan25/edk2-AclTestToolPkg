//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_VIDEO (Text Video)                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __TEXT_T_VIDEO_H
#define __TEXT_T_VIDEO_H

#include <Library/UefiLib.h>
#include "typedef.h"

//=============================================================================
//  text mode video buffer
//=============================================================================
#ifdef __BORLANDC__
#define T_BASE			0xb8000000		// B800:0000
#endif

#ifdef __WATCOMC__
#define T_BASE			0x000b8000
#endif

#define	T_BUF_SZ		4000			// 80 col * 25 row * 2 = 4000
#define T_CHAR_SZ		2000			// 80 * 25 = 2000
#define T_LINE_SZ		160				// 80 * 2 = 160

//=============================================================================
//  text mode char
//=============================================================================
// 2 bytes for each character
// MSB : color attribute
// LSB : ASCII code

//=============================================================================
//  text mode color attribute
//=============================================================================
// bit    bit_sz  description
//------------------------------------------------------
// [7]    1       blinking (BLK)
// [6:4]  3       background color (8-colors,  0x0~0x7)
// [3:0]  4       foreground color (16-colors, 0x0~0xF)
//=============================================================================
#define T_BLACK			0
#define T_BLUE			1
#define T_GREEN			2
#define T_CYAN			3
#define T_RED			4
#define T_MAGENTA		5
#define T_BROWN			6
#define T_LIGHTGRAY		7
#define T_DARKGRAY		8
#define T_LIGHTBLUE		9
#define T_LIGHTGREEN	10
#define T_LIGHTCYAN		11
#define T_LIGHTRED		12
#define T_LIGHTMAGENTA	13
#define T_YELLOW		14
#define T_WHITE			15

#define T_BLINK			8

#define T_MASK_BL		0x80	// blink flag mask
#define T_MASK_BG		0x70	// background color mask
#define T_MASK_FG		0x0F	// foreground color mask

#define T_ARG_BLACK			0xA0
#define T_ARG_BLUE			0xA1
#define T_ARG_GREEN			0xA2
#define T_ARG_CYAN			0xA3
#define T_ARG_RED			0xA4
#define T_ARG_MAGENTA		0xA5
#define T_ARG_BROWN			0xA6
#define T_ARG_LIGHTGRAY		0xA7
#define T_ARG_DARKGRAY		0xA8
#define T_ARG_LIGHTBLUE		0xA9
#define T_ARG_LIGHTGREEN	0xAA
#define T_ARG_LIGHTCYAN		0xAB
#define T_ARG_LIGHTRED		0xAC
#define T_ARG_LIGHTMAGENTA	0xAD
#define T_ARG_YELLOW		0xAE
#define T_ARG_WHITE			0xAF
#define T_ARG_COLOR_MASK	T_ARG_BLACK

//=============================================================================
//  text mode alignment
//=============================================================================
#define T_AL_NONE		0x00
#define T_AL_LT			0x11	// left-top
#define T_AL_CT			0x12	// center-top
#define T_AL_RT			0x14	// right-top
#define T_AL_LM			0x21	// left-middle
#define T_AL_CM			0x22	// center-middle
#define	T_AL_RM			0x24	// right-middle
#define T_AL_LB			0x41	// left-bottom
#define T_AL_CB			0x42	// center-bottom
#define T_AL_RB			0x44	// right-bottom
#define T_AL_MASK_L		0x01	// left
#define T_AL_MASK_C		0x02	// center
#define T_AL_MASK_R		0x04	// right
#define T_AL_MASK_T		0x10	// top
#define T_AL_MASK_M		0x20	// middle
#define T_AL_MASK_B		0x40	// bottom

#define T_AL_LEFT		0x01
#define T_AL_CENTER		0x02
#define T_AL_RIGHT		0x04

//=============================================================================
//  t_video_clip_t
//=============================================================================
#pragma pack(1)

typedef struct _t_video_clip_t
{
	uint8_t		*buf;
	int			x, y;	// start
	int			w, h;	// size

} t_video_clip_t;

//=============================================================================
//  t_video_info_t
//=============================================================================
typedef struct _t_video_info_t
{
	#ifdef __BORLANDC__
	uint8_t	far		*buf;
	#endif

	#ifdef __WATCOMC__
	uint8_t			*buf;
	#endif

	uint8_t			color;
	int				x, y;

	t_video_clip_t	clip[16];	// FILO : first in last output
	uint8_t			clip_idx;

	void			(*update_func)(void);	// update function

} t_video_info_t;

#pragma pack()

//=============================================================================
//  extern
//=============================================================================
extern t_video_info_t	tv;
extern uint8_t *gTVBASE;

//=============================================================================
//  function
//=============================================================================
void t_color(uint8_t fg, uint8_t bg);
void t_fg_color(uint8_t fg);
void t_bg_color(uint8_t bg);
void t_bl_color(uint8_t blink);
uint8_t t_get_color(void);
void t_set_color(uint8_t color);
uint8_t t_xy_get_color(int x, int y);

void t_gotoxy(int x, int y);

void t_clear_screen(void);
void t_clear_color_screen(uint8_t fg, uint8_t bg);
void t_fill_screen(uint8_t c);
void t_fill_color_screen(uint8_t fg, uint8_t bg, uint8_t c);
void t_fill_region(int sx, int sy, int ex, int ey, uint8_t c);

void t_putc(char c);
void t_puts(char *s);
void t_putcs(char *cs);
void t_putd(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);
void t_putx(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);
void t_putb(uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);

void t_printf(const char *fmt, ...);

void t_xy_putc(int x, int y, char c);
void t_xy_puts(int x, int y, char *s);
void t_xy_putcs(int x, int y, char *cs);
void t_xy_putd(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);
void t_xy_putx(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);
void t_xy_putb(int x, int y, uint32_t data, uint8_t pad0, uint8_t padlen, uint8_t align);

void t_xy_cl_putc(int x, int y, uint8_t fg, uint8_t bg, char c);
void t_xy_cl_puts(int x, int y, uint8_t fg, uint8_t bg, char *s);
void t_xy_cl_putcs(int x, int y, uint8_t fg, uint8_t bg, char *cs);
void t_xy_cl_putd(int x, int y, uint8_t fg, uint8_t bg, uint32_t data);
void t_xy_cl_putx(int x, int y, uint8_t fg, uint8_t bg, uint32_t data);
void t_xy_cl_putb(int x, int y, uint8_t fg, uint8_t bg, uint32_t data);

void t_put_hline(int x, int y, int len);
void t_put_vline(int x, int y, int len);
void t_mark_hline(int x, int y, int len, uint8_t mark);
void t_mark_vline(int x, int y, int len, uint8_t mark);
void t_pen_hline(int x, int y, int len, uint8_t pen);
void t_pen_vline(int x, int y, int len, uint8_t pen);

void	t_cl_put_hline(int x, int y,  uint8_t fg, uint8_t bg, int len);
void	t_cl_put_vline(int x, int y,  uint8_t fg, uint8_t bg, int len);

void t_shadow_char(int x, int y);

void t_put_rect(int sx, int sy, int w, int h);
void t_put_rect_solid(int sx, int sy, int w, int h);
void t_put_rect_single(int x, int y, int w, int h);
void t_put_rect_single_solid(int x, int y, int w, int h);
void t_put_rect_double(int x, int y, int w, int h);
void t_put_rect_double_solid(int x, int y, int w, int h);

void	t_cl_put_rect(int sx, int sy, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_rect_solid(int sx, int sy, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_rect_single(int x, int y,  uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_rect_single_solid(int x, int y, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_rect_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_rect_double_solid(int x, int y, uint8_t fg, uint8_t bg, int w, int h);

void	t_xy_put_rect(int sx, int sy, int ex, int ey);
void	t_xy_put_rect_solid(int sx, int sy, int ex, int ey);
void	t_xy_put_rect_single(int sx, int sy, int ex, int ey);
void	t_xy_put_rect_single_solid(int x, int y, int ex, int ey);
void	t_xy_put_rect_double(int sx, int sy, int ex, int ey);
void	t_xy_put_rect_double_solid(int sx, int sy, int ex, int ey);

void	t_xy_cl_put_rect(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);
void	t_xy_cl_put_rect_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);
void	t_xy_cl_put_rect_single(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);
void	t_xy_cl_put_rect_single_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);
void	t_xy_cl_put_rect_double(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);
void	t_xy_cl_put_rect_double_solid(int sx, int sy, int ex, int ey, uint8_t fg, uint8_t bg);

void t_put_win_single(int x, int y, int w, int h);
void t_put_win_single_double(int x, int y, int w, int h);
void t_put_win_double(int x, int y, int w, int h);
void t_put_win_double_single(int x, int y, int w, int h);

void	t_cl_put_win_single(int x, int y, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_win_single_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_win_double(int x, int y, uint8_t fg, uint8_t bg, int w, int h);
void	t_cl_put_win_double_single(int x, int y, uint8_t fg, uint8_t bg, int w, int h);

void t_copy_region(int sx, int sy, int ex, int ey);
void t_paste_region(int x, int y);

void t_capture_screen(void);

EFI_STATUS t_init(void);
void t_exit(void);

#endif

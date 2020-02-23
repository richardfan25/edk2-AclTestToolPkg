//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  VIDEO - VIDEO (INT 10h - Video Service)                                  *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __VIDEO_VIDEO_H
#define __VIDEO_VIDEO_H

#include "typedef.h"

//=============================================================================
//  functions
//=============================================================================
void video_set_mode(uint8_t mode);
void video_set_cur_type(uint8_t top, uint8_t bottom);
void video_hide_cur(void);
void video_show_cur(void);
void video_set_cur_pos(int x, int y);
void video_get_cur_pos(int *x, int *y);
int  video_get_8x16_charset(uint8_t *font, uint32_t fontsz);

#endif

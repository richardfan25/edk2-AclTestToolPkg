//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - BDA (BIOS Data Area)                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//  address    len  description
//-----------------------------------------------------------------------------
// 0040:0000   2    COM1 base address
// 0040:0002   2    COM2 base address
// 0040:0004   2    COM3 base address
// 0040:0006   2    COM4 base address
// 0040:0008   2    LPT1 base address
// 0040:000A   2    LPT2 base address
// 0040:000C   2    LPT3 base address
// 0040:000E   2    LPT4 base address, EBDA (Extend BIOS Data Area Segment)

// 0040:0010   2    equipment list flag
// 0040:0012   1    PCjr : IR keyboard link error count
// 0040:0013   2    memory size in KiB (INT 12h)
// 0040:0015   1    reserved
// 0040:0016   1    PS/2 BIOS control flags
// 0040:0017   1    keyboard flag byte 0
// 0040:0018   1    keyboard flag byte 1
// 0040:0019   1    storage for alternative keypad entry
// 0040:001A   2    keyboard buffer pointer (head)
// 0040:001C   2    keyboard buffer pointer (tail)
// 0040:001E   32   keyboard buffer (scan code+ascii code)
// 0040:003E        -
// .... TODO
// 0040:006C   4    daily timer counter, equal to zero at midnight, (18.2Hz)
//                  incremented by INT 8h (IRQ0), read/set by INT 1Ah
//					18.2064815 Hz
//					daily : 18.2064815 Hz * 86400 seconds = 1573040
// .... TODO
// 0040:0089   1    video display data area (MCGA/VGA)
//                  bit[0] : 1=video is active
// .... TODO
// refernece : http://stanislavs.org/helppc/bios_data_area.html
//*****************************************************************************
#ifndef __X86_BDA_H
#define __X86_BDA_H

//=============================================================================
//  define
//=============================================================================
#define BDA_MEM_BASE	0x400	// 40:00
#define BDA_SIZE		(256)	// 40:00 ~ 40:FF

#define BDA_VIDEO_EN	0x01	// 489h bit[0]

#define BDA_TIMER_MAX	1573040	// daily timer


//=============================================================================
//  functions
//=============================================================================
uint32_t bda_get_timer(void);
uint8_t  bda_get_video_config(void);
void  EFITimerEventClose(void);

#endif

//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  HELP : Help                                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "t_scrcap.h"
#include "sound.h"
#include "dat.h"
#include "pu.h"
#include "key.h"
#include "strlst.h"
#include "rt.h"

//=============================================================================
//  pu_help_show
//=============================================================================
void pu_help_show(void)
{
	str_list_t		strlst;
	str_list_str_t	*s;

	char		str[128];
	uint8_t		loops;
	int			i, j, k, len;
	int			sx, sy;
	int			mw, mh;
	uint8_t		fg, bg;
	uint16_t		key, dirty;

	int			page_max_idx;
	int			page_st_idx;
	int			sb_sta, sb_end, sb_len;

	memset(&strlst, 0, sizeof(str_list_t));

	//------------------------------------------------------------------------------
	//  Hot Key - Function
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cHot Key - Function", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	// Space : main menu
	sprintf(str, "\b%cSpace \b%c       : Main Menu", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	// UP/DN : 1 rec
	sprintf(str, "\b%c\x18\x19    \b%cScroll : Up/Down 1 record", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// HOME : 1st rec
	sprintf(str, "\b%cHome  \b%cScroll : The 1st Record", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// END : last rec
	sprintf(str, "\b%cEnd   \b%cScroll : The Last Record", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// PGUP : last page
	sprintf(str, "\b%cPgUp  \b%cScroll : Last Page", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// PGDN : next page
	sprintf(str, "\b%cPgDn  \b%cScroll : Next Page", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// LEFT/RIGHT : 100 rec
	sprintf(str, "\b%c\x1b\x1a    \b%cScroll : Prev/Next 100 records", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// Alt-+ : 1000 rec
	sprintf(str, "\b%cAlt-+ \b%cScroll : Prev/Next 1000 records", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// Alt-G : go to record
	sprintf(str, "\b%cAlt-G \b%cScroll : Go to record no.", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// Enter : check record
	sprintf(str, "\b%cEnter     \b%c   : Check Record, detail of record", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// F : find : fail
	sprintf(str, "\b%cF     \b%cSearch : Fail Records", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// E : find : error
	sprintf(str, "\b%cE     \b%cSearch : Error Records", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// N : find : na
	sprintf(str, "\b%cN     \b%cSearch : N/A Records", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// X : find : error check flag
	sprintf(str, "\b%cX     \b%cSearch : Error Check Flags", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// L : view : log file
	sprintf(str, "\b%cL     \b%c  View : Log Files", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// C : check flag menu
	sprintf(str, "\b%cC     \b%c  View : Check Flag Menu", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// S : system function menu
	sprintf(str, "\b%cS     \b%c  View : System Function Menu", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// Backspace : create .ERR and .RAW
	sprintf(str, "\b%cBackspace \b%c   : Create *.ERR and *.RAW for analysis", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// TAB : AT/ATX
	sprintf(str, "\b%cTab   \b%c  View : Toggle AT/ATX", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// DEL : Delete all rec
	sprintf(str, "\b%cDel   \b%cDelete : Clear all records (when records < 10)", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// F1 : Help
	sprintf(str, "\b%cF1    \b%c  Help : You're watching now", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F2 : About
	sprintf(str, "\b%cF2    \b%c About : Copyright Info", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F3 : sort inc
	sprintf(str, "\b%cF3    \b%c  Sort : Sorted by Increase (No, INTV, TMP, TSC)", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F4 : sort dec
	sprintf(str, "\b%cF4    \b%c  Sort : Sorted by Decrease (No, INTV, TMP, TSC)", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F5 : View : text port
	sprintf(str, "\b%cF5    \b%c  View : Text Report", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F6 : View : select .dat file
	sprintf(str, "\b%cF6    \b%c  View : Select .DAT File", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F7 : Create : graphic report
	sprintf(str, "\b%cF7    \b%cCreate : Graphic Report", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F8 : Create : text report
	sprintf(str, "\b%cF8    \b%cCreate : Text Report", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F9 : View : interval statistic
	sprintf(str, "\b%cF9    \b%c  View : Interval Statistic", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	// F10 : Screen Shot
	sprintf(str, "\b%cF10   \b%cCreate : Screen Shot <ddhhmmss.BMP>", T_ARG_RED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	//------------------------------------------------------------------------------
	//  Online Mode
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cOnline Mode", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	sprintf(str, "\b%c[no_param]      : Recording Time", T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	sprintf(str, "\b%c-a <low_hz> <high_hz> : Assign alarm frequency", T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	sprintf(str, "\b%c  Example : powerup -a 700 950 (ambulance siren alarm)", T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	//------------------------------------------------------------------------------
	//  Offline Mode
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cOffline Mode", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	sprintf(str, "\b%c-v : View xxx.DAT file", T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c-h : Help", T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	//------------------------------------------------------------------------------
	//  Screen Info (Top Side)
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cScreen Info (Top Side of Screen)", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cIni \b%c: initial time, POWERUP.DAT is created after initial procedure", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	sprintf(str, "\b%cSta \b%c: start time, record count = 0, the reference time of all records", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cCur \b%c: current time, time of the latest record", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	sprintf(str, "\b%cEla \b%c: elapsed time, duration between start time and current time", T_ARG_LIGHTCYAN, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cInt \b%c: intervals time, it's the threshold of record time.", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cTol \b%c: tolerance time", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c nx \b%c: number of check flag x", T_ARG_LIGHTMAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	sprintf(str, "\b%cRTC_BAT \b%c: coin battery present", T_ARG_LIGHTCYAN, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c     AT \b%c: AT mode", T_ARG_LIGHTGREEN, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c    ATX \b%c: ATX mode", T_ARG_LIGHTGREEN, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c     ON \b%c: On-line mode of POWERUP", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c    OFF \b%c: Off-line mode of POWERUP", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%c    ALM \b%c: alarm at on-line", T_ARG_LIGHTRED, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cRST_NON \b%c: non-reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_KBC \b%c: KBC reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_92  \b%c: soft 92h reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_SOF \b%c: soft reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_HAR \b%c: hard reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_FUL \b%c: full reset", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRST_TMO \b%c: reset time-out", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	strcpy(str, "\n");
	str_list_add(str, &strlst);

	// TO, OK, GG, ER, NA, oo, xx
	sprintf(str, "\b%cTO \b%c: number of total record", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cOK \b%c: number of okay record", T_ARG_LIGHTGREEN, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cGG \b%c: number of fail record", T_ARG_LIGHTRED,	T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cER \b%c: number of error record (time return to the past)", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cNA \b%c: number of n/a record (time freeze, the same time as last record)", T_ARG_DARKGRAY, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%coo \b%c: number of oo record", T_ARG_LIGHTCYAN, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cxx \b%c: number of xx record", T_ARG_LIGHTMAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	// Big Number
	sprintf(str, "\b%c[Big Number] \b%c: current record", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%c    [UpTime] \b%c: uptime of POWERUP", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	//------------------------------------------------------------------------------
	//  Field
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cScreen Info (Field)", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cNo   \b%c: record number", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cDate \b%c: record date", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cTime \b%c: record time", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cAB   \b%c: ABIOS-Advantech BIOS (model, version)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cAEC  \b%c: AEC-Advantech EC (chip, model, version)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cCP   \b%c: CPU (vendor, name)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cE8   \b%c: E820 memory map", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cSM   \b%c: SMBIOS-System Management BIOS", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cPC   \b%c: PCI bus probe", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cSB   \b%c: SMBus probe", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cSP   \b%c: SPD probe", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cRTC  \b%c: RTC register check (0A, 0B, 0D)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cTMP  \b%c: CPU temperature (unit:%cC)", T_ARG_BLACK, T_ARG_BLACK, 0xf8);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cTSC  \b%c: Time Stamp Counter, uptime from CPU reset (unit:1G)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cINTV \b%c: power-up interval (unit:seconds)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "\b%cQC   \b%c: check interval result (OK,GG,ER,NA)", T_ARG_BLACK, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	//------------------------------------------------------------------------------
	//  Screen Info (Bottom Side)
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cScreen Info (Bottom Side of Screen)", T_ARG_BLUE);
	str_list_add(str, &strlst);
	strcpy(str, "------------------------------------------------------------------------");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cLeft   \b%c: Advantech BIOS string", T_ARG_YELLOW, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "%s", "         **** ......  ****");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cMiddle \b%c: Advantech EC information", T_ARG_LIGHTCYAN, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "%s", "         chip, board, EC f/w version");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cRight  \b%c: Miscellaneous information", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	sprintf(str, "%s", "         board name, file name, time to reset");
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	//------------------------------------------------------------------------------
	//  Files
	//------------------------------------------------------------------------------
	sprintf(str, "\b%cFiles >>", T_ARG_BLUE);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cPOWERUP.DAT \b%c: record file, updated if on-line mode", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cPOWERUP.LOG \b%c: log file, created if error occured", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cPOWERUP.ERR \b%c: error file, created for collecting error data", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cPOWERUP.TXT \b%c: text report file", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cPOWERUP.BMP \b%c: graphic report file", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cXXXXXXX.RAW \b%c: original data in *.DAT file for error checking", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "\b%cXXXXXXX.ERR \b%c: error data in *.ERR file for error checking", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	sprintf(str, "\b%cPOWERUP.PCI \b%c: OEM PCI device name", T_ARG_MAGENTA, T_ARG_BLACK);
	str_list_add(str, &strlst);

	sprintf(str, "%s", "  Naming the PCI device name, if you don't like \"Unknown Device\" shown,");
	str_list_add(str, &strlst);

	sprintf(str, "%s", "The name will be shown on PCI device list.");
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);

	sprintf(str, "\b%cFormat:\b%c", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	sprintf(str, "%s", "<vendor_id>:<device_id>  <OEM_PCI_device_name>");
	str_list_add(str, &strlst);

	strcpy(str, "\n");
	str_list_add(str, &strlst);
	
	sprintf(str, "\b%cExample of POWERUP.PCI:\b%c", T_ARG_WHITE, T_ARG_BLACK);
	str_list_add(str, &strlst);
	
	sprintf(str, "%s", "8086:1234 Intel some kind of device 1");
	str_list_add(str, &strlst);
	sprintf(str, "%s", "8086:5678 Intel some kind of device 2");
	str_list_add(str, &strlst);

	//-------------------------------------------------------------------------
	//  show
	//-------------------------------------------------------------------------

	mw = strlst.maxlen + 2;	// 2:border
	mh = 2 + 2 + 16;		// 2:border, 2:title, 16:item

	sx = (80 - mw) / 2;	// align center
	sy = (25 - mh) / 2;

	fg = T_BLACK;
	bg = T_LIGHTGRAY;

	page_max_idx = strlst.num - 1;
	page_st_idx = 0;

	t_copy_region(sx, sy, sx+mw, sy+mh);
	t_color(fg, bg);
	t_put_win_single_double(sx, sy, mw, mh);
	
	for (i=sx+1; i<=sx+mw; i++)
		t_shadow_char(i, sy+mh);	// shadow : bottom border

	for (i=sy+1; i<=sy+mh; i++)
		t_shadow_char(sx+mw+0, i);	// shadow : right border	

	sprintf(str, "%s", "Help");
	t_xy_puts(sx+(mw-(int)strlen(str))/2, sy+1, str);
	
	//sound_ok();

	loops = 1;
	dirty = 1;
	while (loops)
	{
		if (dirty)
		{
			// point to str of strlst
			s = strlst.head;
			if (page_st_idx > 0)
			{
				for (i=0; i<page_st_idx; i++)
					s = s->next;
			}
			
			for (i=0; i<16; i++)
				t_put_hline(sx+1, sy+3+i, strlst.maxlen);

			for (i=0; i<16; i++)
			{
				if ((page_st_idx+i) > page_max_idx)
				{
					t_put_hline(sx+1, sy+3+i, strlst.maxlen);
				}
				else
				{
					len = (int)strlen(s->str);
					for (j=0,k=0; j<len; j++)
					{
						if (s->str[0] == '\n')
							break;

						if (s->str[j] == '\b')
						{
							j++;
							t_fg_color(s->str[j] & 0x0F);
							continue;
						}
						t_xy_putc(sx+2+k, sy+3+i, s->str[j]);
						k++;
					}
					s = s->next;
				}
			}

			// scroll bar
			if (strlst.num > 16)
			{
				sb_len = 16 * 16 / strlst.num;
				if (sb_len == 0)
					sb_len = 1;		// len = 1 at least

				// top
				if (page_st_idx == 0)
				{
					sb_sta = 0;
					sb_end = sb_sta + sb_len - 1;
				}
				// bottom
				else if ((page_st_idx + (16-1)) == page_max_idx)
				{
					sb_end = (16 - 1);
					sb_sta = sb_end - sb_len + 1;
				}
				// middle
				else
				{
					sb_sta = (page_st_idx + 1) * 16 / strlst.num;
					if (sb_sta == 0)
						sb_sta = 1;
					sb_end = sb_sta + sb_len - 1;
					if (sb_end > (16-2))
					{
						sb_end = 16 - 2;
						sb_sta = sb_end - sb_len + 1;
					}
				}

				t_color(fg, bg);

				// 16: 0~15
				for (i=0; i<16; i++)
				{
					if (i<sb_sta || i>sb_end)
						t_xy_putc(sx+mw-2, i+sy+3, 0xb0);
					else
						t_xy_putc(sx+mw-2, i+sy+3, 0xdb);	// 0xb2
				}
			}
			
			dirty = 0;
		}

		key = (uint16_t)bioskey(1);
		if(key)
		{
			//key = key_blk_read_sc();
			switch(key)
			{
			case (SCAN_ESC << 8):
			case CHAR_CARRIAGE_RETURN:
				loops = 0;
				break;

			case (SCAN_PAGE_UP << 8):
			case (SCAN_UP << 8):

				if (key == (SCAN_PAGE_UP << 8))
					page_st_idx -= 16;
				else if (key == (SCAN_UP << 8))
					page_st_idx--;

				if (page_st_idx < 0)
					page_st_idx = 0;
			
				//page_no--;
				//if (page_no < 0)
				//	page_no = 0;
				dirty = 1;
				break;

			case (SCAN_PAGE_DOWN << 8):
			case (SCAN_DOWN << 8):

				if (key == (SCAN_PAGE_DOWN << 8))
					page_st_idx += 16;
				else if (key == (SCAN_DOWN << 8))
					page_st_idx++;

				if (page_max_idx < 16)
				{
					page_st_idx = 0;
				}
				else
				{
					if (page_st_idx > (page_max_idx-15))
						page_st_idx = page_max_idx - 15;
				}
				//page_no++;
				//if (page_no > page_no_max)
				//	page_no = page_no_max;
				dirty = 1;
				break;
	
			case (SCAN_HOME << 8):
				page_st_idx = 0;
				dirty = 1;
				break;

			case (SCAN_END << 8):
				page_st_idx = page_max_idx - 15;
				if (page_st_idx < 0)
					page_st_idx = 0;
				dirty = 1;
				break;

			case (SCAN_F10 << 8):
				t_scr_capture();
				break;
			}
		}
	}
	
	str_list_del_all(&strlst);

	t_paste_region(-1, -1);
}


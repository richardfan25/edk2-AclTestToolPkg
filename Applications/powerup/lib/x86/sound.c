//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SOUND                                                              *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//  x86 8253/8254 timer
//=============================================================================
// Port 40h : timer counter 0 register (18.2Hz)--IRQ 0
// Port 41h : timer counter 1 register (DRAM ?)
// Port 42h : timer counter 2 register (Speaker)

// Port 43h : timer control register

//  bit[7:6] - select timer
//       00 = tmr0
//       01 = tmr1
//       10 = tmr2

//  bit[5:4] - read/write configuration
//       00 = counter latch
//       01 = LSB value (bit[7:0])
//       10 = MSB value (bit[15:8])
//       11 = 2*read/2*write bit[7:0] then bit[15:8]

//  bit[3:1] - select mode
//      000 = mode 0 : interrupt and terminal count
//      001 = mode 1 : hardware re-triggerable one-shot
//      010 = mode 2 : rate generator
//      011 = mode 3 : square wave
//      100 = mode 4 : software triggered strobe
//      101 = mode 5 : hardware triggered strobe

//  bit[0] - counter type
//        0 = counter is a 16-bit binary counter  (binary)
//        1 = counter is a 16-bit decimal counter (BCD)

//  bit[7:0]
//    1101xxx0 = counter()
//    1110xx--

// spearker on and 1000 Hz => 1190000 / 1000 = 1190 = 0x4A6
// - outb B6h, 43h	; 43h port = B6h	; spearker on
// - outb A6h, 42h	; 42h port = A6h	; LSB of data
// - outb 04h, 42h	; 42h port = 04h	; MSB of data

//=============================================================================
//  x86 8255
//=============================================================================
// Port 61h : PC speaker control register
// bit[1:0] - 11=speaker switch on, 00=off


#ifdef __BORLANDC__
#include <dos.h>
#endif

#ifdef __WATCOMC__
#include <i86.h>
#endif

#include "typedef.h"
#include "sound.h"

//=============================================================================
//  Piano Frequency Chart
//=============================================================================
// C CS D DS E F FS G GS A AS B
//
//      C0    C1    C2    C3    C4    C5    C6     C7     C8
//-----------------------------------------------------------------------------
// B    31    62   123   247   494   988  1976   3951
// AS   29    58   117   233 = 466   932  1865   3729
// A    27    55   110   220   440   880  1760   3520
// GS         52   104   208 = 415   830  1661   3322
// G          49    98   196   392   784  1568   3136
// FS         46    92   185 = 370   740  1480   2960
// F          44    87   175   349   698  1397   2794
// E          41    82   165   329   659  1319   2637
// DS         39    78   156 = 311   622  1245   2489
// D          37    73   147   294   587  1175   2349
// CS         35    69   139 = 277   554  1109   2217
// C          33    65   131   262   523  1047   2093   4186
//-----------------------------------------------------------------------------
uint16_t piano_key_freq[88] =
{
	                                                        27,   29,   31, // C0
	  33,   35,   37,   39,   41,   44,   46,   49,   52,   55,   58,   62,	// C1
	  65,   69,   73,   78,   82,   87,   92,   98,  104,  110,  117,  123, // C2
	 131,  139,  147,  156,  165,  175,  185,  196,  208,  220,  223,  247, // C3
	 262,  277,  294,  311,  329,  349,  370,  392,  415,  440,  466,  494, // C4
	 523,  554,  587,  622,  659,  698,  740,  784,  830,  880,  932,  988, // C5
	1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, // C6
	2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, // C7
	4186                                                                    // C8
};

/*
	char	key_name[88][4] =
	{
		                                                          "A0", "A0#", "B0",
		"C1", "C1#", "D1", "D1#", "E1", "F1", "F1#", "G1", "G1#", "A1", "A1#", "B1",
		"C2", "C2#", "D2", "D2#", "E2", "F2", "F2#", "G2", "G2#", "A2", "A2#", "B2",
		"C3", "C3#", "D3", "D3#", "E3", "F3", "F3#", "G3", "G3#", "A3", "A3#", "B3",
		"C4", "C4#", "D4", "D4#", "E4", "F4", "F4#", "G4", "G4#", "A4", "A4#", "B4",
		"C5", "C5#", "D5", "D5#", "E5", "F5", "F5#", "G5", "G5#", "A5", "A5#", "B5",
		"C6", "C6#", "D6", "D6#", "E6", "F6", "F6#", "G6", "G6#", "A6", "A6#", "B6",
		"C7", "C7#", "D7", "D7#", "E7", "F7", "F7#", "G7", "G7#", "A7", "A7#", "B7",
		"C8"
	};
*/


//=============================================================================
//  sound_ok
//=============================================================================
void sound_ok(void)
{
	sound(3000);
	delay(50);
	nosound();
}

//=============================================================================
//  sound_error
//=============================================================================
void sound_error(void)
{
	sound(200);
	delay(600);
	nosound();
}

//=============================================================================
//  sound_warning
//=============================================================================
void sound_warning(void)
{
	sound(300);
	delay(100);
	sound(2000);
	delay(300);
	nosound();
}

//=============================================================================
//  sound_alarm
//=============================================================================
void sound_alarm(void)
{
	sound(3000);
	delay(1000);
	nosound();
}

//===========================================================================
//  sound_end
//===========================================================================
void sound_end(void)
{
	sound(200);
	delay(100);
	nosound();
}

//=============================================================================
//  sound_keyin
//=============================================================================
void sound_keyin(void)
{
	sound(1000);
	delay(30);
	nosound();
}

//=============================================================================
//  sound_tmout
//=============================================================================
void sound_tmout(void)
{
	sound(3000);
	delay(150);

	nosound();
	delay(150);

	sound(3000);
	delay(150);

	nosound();
	delay(550);

	nosound();
}

//=============================================================================
//  ambulance_siren
//=============================================================================
void ambulance_siren(int low, int hi)
{
	int		i;

	for (i=0; i<4; i++)
	{
		sound((uint16_t)low);
		delay(500);
		sound((uint16_t)hi);
		delay(500);
		nosound();
	}
}

//=============================================================================
//  little_bee
//=============================================================================
void little_bee(void)
{
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(800);	nosound(); delay(100);

	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(800);	nosound(); delay(100);

	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(800);	nosound(); delay(100);


	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(800);	nosound(); delay(100);

	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(800);	nosound(); delay(100);

	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(1600);	nosound(); delay(100);


	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(800);	nosound(); delay(100);

	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(800);	nosound(); delay(100);

	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(800);	nosound(); delay(100);

	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(800);	nosound(); delay(100);

	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_DO;	delay(1600);	nosound(); delay(100);
}

//=============================================================================
//  little_star
//=============================================================================
void little_star(void)
{
	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_LA;	delay(400);	nosound(); delay(100);
	SOUND_3_LA;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(800);	nosound(); delay(100);

	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_DO;	delay(800);	nosound(); delay(100);

	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(800);	nosound(); delay(100);

	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(800);	nosound(); delay(100);

	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_DO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(400);	nosound(); delay(100);
	SOUND_3_LA;	delay(400);	nosound(); delay(100);
	SOUND_3_LA;	delay(400);	nosound(); delay(100);
	SOUND_3_SO;	delay(800);	nosound(); delay(100);

	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_FA;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_MI;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_RE;	delay(400);	nosound(); delay(100);
	SOUND_3_DO;	delay(800);	nosound(); delay(100);
}

//#endif


//#############################################################################
//  __WATCOMC__
//#############################################################################
//#ifdef __WATCOMC__


//#endif

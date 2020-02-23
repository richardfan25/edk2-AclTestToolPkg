//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - KEY                                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//
//  PC keyboard Block Diagram
//
//  +------------+       +----------------------------------+    +---------+
//  |  Keyboard  |       |       BDA Keyboard Buffer        |    |   SW    |
//  |   (KBD)    |       |     (0040:001E ~ 0040:003D)      |    |  (App)  |
//  +-----+------+       +------+------------------+--------+    +----+----+
//        |sc [1]               ^    [3]           | [4]              ^
//  +-----v------+  [2]         | kc=sc+ac         | kc               |
//  | Keyboard   |  sc   +------+-----+   +--------v-----------+      | kc
//  | Controller |------>|   INT 9h   |   |      INT 16h       +------+
//  |   (KBC)    |<------|   (IRQ1)   |   | (Keyboard Service) |
//  +------------+  cmd  +------------+   +--------------------+
//                  [8]
//
//  sc  : scancode
//  ac  : ascii code
//  kc  : key code (sc+ac)
//  kbd : keyboard
//  kbc : keyboard controller
//  kbf : BDA keyboard buffer @ BDA (memory)
//  cmd : kbd/kbc command
//  std : standard
//  ext : extend
//  pic : periodic interrupt controller (PIC)
//
//  Steps
//  [1] : A key is pressed, kbd will send sc to kbc via keyboard cable.
//
//  [2] : The kbc will receive the sc, then issue IRQ1 (INT 9h)
//
//  [3] : Once the IRQ1 is accepted by CPU, the INT 9h will be performed.
//        After processing the sc, the kc will be put in kbf and wait for
//        SW to read. Here is the process of INT 9h :
//        - Read the sc from kbc
//        - Convert the sc to the ac, and update ctl-alt-shift key status.
//        - Pack the kc (sc+ac)
//
//  [4] : The service of INT 16h, AH=02h/12h is called. 
//
//  [5] : The SW called the keyboard service of reading key (INT 16h, AH=02h/12h),
//        it will get a kc from kbf.
//
//  Caution:
//        The sc [2] and the sc [3] might not be the same due to backward
//        compatible (std/ext keyboard). It means the sc is probably different
//        between kbc and kbf.

//        o sc[2] : scancode of keyboard         (INT 9h input)
//        o sc[3] : scancode of keyboard service (INT 9h output, for SW used)

//        There is an example of F11 key:
//        o F11 key
//          . The F11 key is absent  in std-kbd(83/84-keys)
//          . The F11 key is present in ext-kbd(103/104-keys)
//        o std-kbd
//          . kbc-sc = n/a   (sc[2])
//          . kbf-sc = 0x57  (sc[3])
//            - The kc of "Shift-F4" is defined 0x5700
//            - where kbf-sc=0x57 and kbf-ac=0x00
//        o ext-kbd
//          . kbc-sc = 0x57  (sc[2])
//            - The F11 key sc is defined 0x57.
//          . kbf-sc = 0x85  (sc[3])
//            - Because of kbf-sc=0x57 has been defined by std-kbd, so the kbf-sc
//              in ext-kbd can not be 0x57 to avoid confusion. A new value 0x85 is
//              given for kbf-sc in ext-kbd for backward compatible.
//        o The SW should be used INT 16h extended function (AH=10h~12h) to perfrom
//          keyboard service for ext-kbd.
//
//*****************************************************************************
//  INT 9h

//    - Made by  : BIOS

//    - Install  : INT 9h hander is installed by BIOS after boot.

//    - Active   : Actived by key pressed

//    - Flow     :
//             <1> Once a key is pressed
//             <2> The keyboard sent a corresponding scancode of the key
//             <3> The KBC will receive scancode and issue IRQ1 to PIC
//             <4> PIC will issue the interrupt request to CPU
//             <5> CPU accepted the request then CPU will call INT 9h handler

//    - Function : Reading key scancode and pack the 2-bytes key code (sc+ac)

//*****************************************************************************
//  INT 16h

//    - Made by  : BIOS

//    - Install  : INT 16h handler is installed after boot

//    - Active   : Actived by SW (APP)

//    - Flow     : INT 16h is a callback function and be called by SW (APP)
//             <1> SW check key stroke by using
//                 keyboard service : INT 16h, AH=00h or AH=10h
//                 (where AH=00h is for standard keyboard)
//                 (      AH=10h is for extended keyboard)
//             <2> If key stroke, SW can check key status by using
//                 keyboard service : INT 16h, AH=01h or AH=11h
//                 to check Ctrl/Shift/Alt key is pressed?
//             <3> As <2>, SW can read key (sc+ac) by using
//                 keyboard serivce : INT16h, AH=02h or AH=12h
//                 to read key from key buffer (32 keys @ 0040:001E~0040:003D)?
//                 - SW will get kc (2-byte) including sc (1-byte, MSB) and ac (1-byte, LSB),
//                   but the sc of kc is not equal to sc of kbc. See above details.
//             <4> If there is no SW to read key and the key is still pressed, the
//                 key buffer will be full or over-written.

//    - Standard Function (for standard keyboard)
//         1. AH=00h : check keystroke
//         2. AH=01h : read shift-key status
//         3. AH-02h : read key code
//
//    - Extended Function (for extended keyboard)
//         1. AH=10h : check keystroke
//         2. AH=11h : read shift-key status
//         3. AH-12h : read key code

//*****************************************************************************
//  key code
//  bit[15:8] = scan  code
//  bit[ 7:0] = ascii code (0=if non-printable char)
//
//
// http://members.tripod.com/vitaly_filatov/ng/asm/asm_021.html
//
// https://handmade.network/forums/t/2011-keyboard_inputs_-_scancodes,_raw_input,_text_input,_key_names
//
//*****************************************************************************
#ifndef __X86_KEY_H
#define __X86_KEY_H

#ifdef __WATCOMC__
//#include <bios.h>
//#define	bioskey(func)	_bios_keybrd(func)
#endif


// UP    : Arrow Up
// DOWN  : Arrow Down
// LEFT  : Arrow Left
// RIGHT : Arrow Right

// BS    :   : backspace

// SLASH   : / : slash
// BKSLASH : \ : backslash

// BRKT  : []: bracket
// LBRKT : [ : left bracket
// RBRKT : ] : right bracket

//=============================================================================
//  key code : INT 16h keyboard service (scan code + ascii code)
//=============================================================================
#define KEY_NULL			0x0000
#define KEY_ESC				0x011B
#define	KEY_1				0x0231
#define	KEY_2				0x0332
#define	KEY_3				0x0433
#define	KEY_4				0x0534
#define	KEY_5				0x0635
#define	KEY_6				0x0736
#define	KEY_7				0x0837
#define	KEY_8				0x0938
#define	KEY_9				0x0A39
#define	KEY_0				0x0B30
#define KEY_MINUS			0x0C2D	// -
#define KEY_EQUAL			0x0D3D	// =
#define KEY_BS				0x0E08
#define KEY_TAB				0x0F09
#define	KEY_Q				0x1071
#define	KEY_W				0x1177
#define	KEY_E				0x1265
#define	KEY_R				0x1372
#define	KEY_T				0x1474
#define	KEY_Y				0x1579
#define	KEY_U				0x1675
#define	KEY_I				0x1769
#define	KEY_O				0x186F
#define	KEY_P				0x1970
#define KEY_LBRKT			0x1A5B	// [
#define KEY_RBRKT			0x1B5D	// ]
#define KEY_ENTER			0x1C0D
#define	KEY_A				0x1E61
#define	KEY_S				0x1F73
#define	KEY_D				0x2064
#define	KEY_F				0x2166
#define	KEY_G				0x2267
#define	KEY_H				0x2368
#define	KEY_J				0x246A
#define	KEY_K				0x256B
#define	KEY_L				0x266C
#define KEY_COLON			0x273B	// ;
#define KEY_QUOT			0x2827	// '
#define KEY_TILDE			0x2960	// `
#define KEY_BKSLASH			0x2B5C	// back /
#define	KEY_Z				0x2C7A
#define	KEY_X				0x2D78
#define	KEY_C				0x2E63
#define	KEY_V				0x2F76
#define	KEY_B				0x3062
#define	KEY_N				0x316E
#define	KEY_M				0x326D
#define KEY_COMMA			0x332C	// ,
#define KEY_DOT				0x342E	// .
#define KEY_SLASH			0x352F	// /
#define KEY_PAD_STAR		0x372A
#define KEY_SPACE			0x3920
#define	KEY_F1				0x3B00
#define	KEY_F2				0x3C00
#define	KEY_F3				0x3D00
#define	KEY_F4				0x3E00
#define	KEY_F5				0x3F00
#define	KEY_F6				0x4000
#define	KEY_F7				0x4100
#define	KEY_F8				0x4200
#define	KEY_F9				0x4300
#define	KEY_F10				0x4400
#define KEY_PAD_7			0x4737
#define KEY_HOME			0x4700	// STD : AMI
#define KEY_HOME_EXT		0x47E0	// EXT
#define KEY_PAD_8			0x4838
#define KEY_UP				0x4800	// STD : AMI
#define KEY_UP_EXT			0x48E0	// EXT
#define KEY_PGUP			0x4900	// STD : AMI
#define KEY_PAD_9			0x4939
#define KEY_PGUP_EXT		0x49E0	// EXT
#define KEY_PAD_MINUS		0x4A2D
#define KEY_PAD_4			0x4B34
#define KEY_LEFT			0x4B00	// STD : AMI
#define KEY_LEFT_EXT		0x4BE0	// EXT
#define KEY_CENTER			0x4C00	// arrow center
#define KEY_PAD_5			0x4C35
#define KEY_PAD_6			0x4D36
#define KEY_RIGHT			0x4D00	// STD : AMI
#define KEY_RIGHT_EXT		0x4DE0	// EXT
#define KEY_PAD_PLUS		0x4E2B
#define KEY_PAD_1			0x4F31
#define KEY_END				0x4F00	// STD : AMI
#define KEY_END_EXT			0x4FE0	// EXT
#define KEY_PAD_2			0x5032
#define KEY_DOWN			0x5000	// STD : AMI
#define KEY_DOWN_EXT		0x50E0	// EXT
#define KEY_PAD_3			0x5133
#define KEY_PGDN			0x5100	// STD : AMI
#define KEY_PGDN_EXT		0x51E0	// EXT
#define KEY_PAD_0			0x5230
#define KEY_INS				0x5200	// STD : AMI
#define KEY_INS_EXT			0x52E0	// EXT
#define KEY_PAD_DOT			0x532E
#define KEY_DEL				0x5300	// STD : AMI
#define KEY_DEL_EXT			0x53E0	// EXT
#define KEY_45				0x565C	// this key sandwiched between L-Shift and Z

#define KEY_F11				0x8500
#define KEY_F12				0x8600

//-----------------------------------------------------------------------------
//  Ctrl
//-----------------------------------------------------------------------------
#define KEY_CTRL_ESC		0x011B
									// Ctrl- 1   : no key
#define KEY_CTRL_2			0x0300	// NUL
									// Ctrl- 3~5 : no key
#define KEY_CTRL_6			0x071E	// RS
									// Ctrl- 7~0 : no key
#define KEY_CTRL_MINUS		0x0C1F
									// Ctrl- =   : no key
#define KEY_CTRL_BS			0x0E7F
#define KEY_CTRL_Q			0x1011	// DC1
#define KEY_CTRL_W			0x1117	// ETB
#define KEY_CTRL_E			0x1205	// ENQ
#define KEY_CTRL_R			0x1312	// DC2
#define KEY_CTRL_T			0x1414	// DC4
#define KEY_CTRL_Y			0x1519	// EM
#define KEY_CTRL_U			0x1615	// NAK
#define KEY_CTRL_I			0x1709	// HT
#define KEY_CTRL_O			0x180F	// SI
#define KEY_CTRL_P			0x1910	// DEL
#define KEY_CTRL_LBRKT		0x1A1B	// ESC
#define KEY_CTRL_RBRKT		0x1B1D	// GS
#define KEY_CTRL_ENTER		0x1C0A	// LF
#define KEY_CTRL_A			0x1E01	// SOH
#define KEY_CTRL_S			0x1F13	// DC3
#define KEY_CTRL_D			0x2004	// EOT
#define KEY_CTRL_F			0x2106	// ACK
#define KEY_CTRL_G			0x2207	// BEL
#define KEY_CTRL_H			0x2308	// Backspace
#define KEY_CTRL_J			0x240A	// LF
#define KEY_CTRL_K			0x250B	// VT
#define KEY_CTRL_L			0x260C	// FF
#define KEY_CTRL_BKSLASH	0x2B1C	// FS
#define KEY_CTRL_Z			0x2C1A	// SUB
#define KEY_CTRL_X			0x2D18	// CAN
#define KEY_CTRL_C			0x2E03	// ETX
#define KEY_CTRL_V			0x2F16	// SYN
#define KEY_CTRL_B			0x3002	// STX
#define KEY_CTRL_N			0x310E	// SO
#define KEY_CTRL_M			0x320D	// CR
									// Ctrl-COMMA : no key
									// Ctrl-DOT   : no key
									// Ctrl-SLASH : no key
#define KEY_CTRL_SPACE		0x3920
#define KEY_CTRL_F1			0x5E00
#define KEY_CTRL_F2			0x5F00
#define KEY_CTRL_F3			0x6000
#define KEY_CTRL_F4			0x6100
#define KEY_CTRL_F5			0x6200
#define KEY_CTRL_F6			0x6300
#define KEY_CTRL_F7			0x6400
#define KEY_CTRL_F8			0x6500
#define KEY_CTRL_F9			0x6600
#define KEY_CTRL_F10		0x6700
#define KEY_CTRL_LEFT		0x7300	// AMI:4B00
#define KEY_CTRL_RIGHT		0x7400	// AMI:4D00
#define KEY_CTRL_END		0x7500	// AMI:4F00
#define KEY_CTRL_HOME		0x7700	// AMI:4700
#define KEY_CTRL_PGDN		0x7600	// AMI:5100
#define KEY_CTRL_PGUP		0x8400	// AMI:4900
#define KEY_CTRL_F11		0x8900
#define KEY_CTRL_F12		0x8A00
#define KEY_CTRL_UP			0x8D00	// AMI:4800
#define KEY_CTRL_PAD_MINUS	0x8E00	// AMI: no key reported
#define KEY_CTRL_PAD_5		0x8F00
#define KEY_CTRL_PAD_PLUS	0x9000	// AMI: no key reported
#define KEY_CTRL_DOWN		0x9100	// AMI:5000
#define KEY_CTRL_INS		0x9200	// AMI:5200
#define KEY_CTRL_DEL		0x9300	// AMI:5300
#define KEY_CTRL_TAB		0x9400
#define KEY_CTRL_PAD_SLASH	0x9500	// AMI: no key reported
#define KEY_CTRL_PAD_STAR	0x9600	// AMI: no key reported

//-----------------------------------------------------------------------------
//  Alt
//-----------------------------------------------------------------------------
#define KEY_ALT_ESC			0x0100
#define KEY_ALT_MINUS		0x0C00
#define KEY_ALT_EQUAL		0x0D00
#define KEY_ALT_BS			0x0E00
#define	KEY_ALT_Q			0x1000
#define	KEY_ALT_W			0x1100
#define	KEY_ALT_E			0x1200
#define	KEY_ALT_R			0x1300
#define	KEY_ALT_T			0x1400
#define	KEY_ALT_Y			0x1500
#define	KEY_ALT_U			0x1600
#define	KEY_ALT_I			0x1700
#define	KEY_ALT_O			0x1800
#define	KEY_ALT_P			0x1900
#define	KEY_ALT_LBRKT		0x1A00
#define	KEY_ALT_RBRKT		0x1B00
#define	KEY_ALT_ENTER		0x1C00
#define	KEY_ALT_A			0x1E00
#define	KEY_ALT_S			0x1F00
#define	KEY_ALT_D			0x2000
#define	KEY_ALT_F			0x2100
#define	KEY_ALT_G			0x2200
#define	KEY_ALT_H			0x2300
#define	KEY_ALT_J			0x2400
#define	KEY_ALT_K			0x2500
#define	KEY_ALT_L			0x2600
#define	KEY_ALT_COLON		0x2700	// ;
#define	KEY_ALT_QUOT		0x2800	// ' : AMI: no key reported
#define	KEY_ALT_TILDE		0x2900	// ` : AMI: no key reported
#define	KEY_ALT_BKSLASH		0x2B00	//
#define	KEY_ALT_Z			0x2C00
#define	KEY_ALT_X			0x2D00
#define	KEY_ALT_C			0x2E00
#define	KEY_ALT_V			0x2F00
#define	KEY_ALT_B			0x3000
#define	KEY_ALT_N			0x3100
#define	KEY_ALT_M			0x3200
#define	KEY_ALT_COMMA		0x3300	// AMI: no key reported
#define	KEY_ALT_DOT			0x3400	// AMI: no key reported
#define	KEY_ALT_SLASH		0x3500	// AMI: no key reported
#define	KEY_ALT_PAD_SLASH	0x3500	// AMI: no key reported
#define	KEY_ALT_PAD_STAR	0x3700	// AMI: no key reported
#define	KEY_ALT_SPACE		0x3920
#define	KEY_ALT_PAD_MINUS	0x4A00	// AMI: no key reported
#define	KEY_ALT_PAD_PLUS	0x4E00	// AMI: no key reported
#define	KEY_ALT_F1			0x6800
#define	KEY_ALT_F2			0x6900
#define	KEY_ALT_F3			0x6A00
#define	KEY_ALT_F4			0x6B00
#define	KEY_ALT_F5			0x6C00
#define	KEY_ALT_F6			0x6D00
#define	KEY_ALT_F7			0x6E00
#define	KEY_ALT_F8			0x6F00
#define	KEY_ALT_F9			0x7000
#define	KEY_ALT_F10			0x7100
#define KEY_ALT_1			0x7800
#define KEY_ALT_2			0x7900
#define KEY_ALT_3			0x7A00
#define KEY_ALT_4			0x7B00
#define KEY_ALT_5			0x7C00
#define KEY_ALT_6			0x7D00
#define KEY_ALT_7			0x7E00
#define KEY_ALT_8			0x7F00
#define KEY_ALT_9			0x8000
#define KEY_ALT_0			0x8100
//#define KEY_ALT_MINUS		0x8200	// AMI:0C00
//#define KEY_ALT_EQUAL		0x8300	// AMI:0D00
#define	KEY_ALT_F11			0x8B00
#define	KEY_ALT_F12			0x8C00
#define	KEY_ALT_HOME		0x9700	// AMI:4700
#define	KEY_ALT_UP			0x9800	// AMI:4800
#define	KEY_ALT_PGUP		0x9900	// AMI:4900
#define	KEY_ALT_LEFT		0x9B00	// AMI:4B00
#define	KEY_ALT_RIGHT		0x9D00	// AMI:4D00
#define	KEY_ALT_END			0x9F00	// AMI:4F00
#define	KEY_ALT_DOWN		0xA000	// AMI:5000
#define	KEY_ALT_PGDN		0xA100	// AMI:5100
#define	KEY_ALT_INS			0xA200	// AMI:5200
#define	KEY_ALT_DEL			0xA300	// AMI:5300
#define KEY_ALT_TAB			0xA500	// AMI:0F00
#define KEY_ALT_PAD_ENTER	0xA600	// AMI: no key reported

//-----------------------------------------------------------------------------
//  Shift
//-----------------------------------------------------------------------------
#define KEY_SHFT_ESC		0x011B
#define KEY_SHFT_1			0x0221	// ! : exclamation
#define KEY_SHFT_2			0x0340	// @ : at
#define KEY_SHFT_3			0x0423	// # : hash
#define KEY_SHFT_4			0x0524	// $ : dollar
#define KEY_SHFT_5			0x0625	// % : percent
#define KEY_SHFT_6			0x075E	// ^ : cross
#define KEY_SHFT_7			0x0826	// & : and
#define	KEY_SHFT_8			0x092A	// * : star
#define KEY_SHFT_9			0x0A28	// ( : left bracket
#define KEY_SHFT_0			0x0B29	// ) : right bracket
#define KEY_SHFT_MINUS		0x0C5F	// _ : underline
#define	KEY_SHFT_EQUAL		0x0D2B	// + : plus
#define KEY_SHFT_BS			0x0E08
#define KEY_SHFT_TAB		0x0F00
#define	KEY_SHFT_Q			0x1051
#define	KEY_SHFT_W			0x1157
#define	KEY_SHFT_E			0x1245
#define	KEY_SHFT_R			0x1352
#define	KEY_SHFT_T			0x1454
#define	KEY_SHFT_Y			0x1559
#define	KEY_SHFT_U			0x1655
#define	KEY_SHFT_I			0x1749
#define	KEY_SHFT_O			0x184F
#define	KEY_SHFT_P			0x1950
#define KEY_SHFT_LBRKT		0x1A7B	// { : curly bracket
#define KEY_SHFT_RBRKT		0x1B7D	// }
#define	KEY_SHFT_A			0x1E41
#define	KEY_SHFT_S			0x1F53
#define KEY_SHFT_ENTER		0x1C0D
#define	KEY_SHFT_D			0x2044
#define	KEY_SHFT_F			0x2146
#define	KEY_SHFT_G			0x2247
#define	KEY_SHFT_H			0x2348
#define	KEY_SHFT_J			0x244A
#define	KEY_SHFT_K			0x254B
#define	KEY_SHFT_L			0x264C
#define KEY_SHFT_COLON		0x273A	// :
#define KEY_SHFT_QUOT		0x2822	// "
#define KEY_SHFT_TILDE		0x297E	// ~
#define KEY_SHFT_BKSLASH	0x2B7C	// |
#define	KEY_SHFT_Z			0x2C5A
#define	KEY_SHFT_X			0x2D58
#define	KEY_SHFT_C			0x2E43
#define	KEY_SHFT_V			0x2F56
#define	KEY_SHFT_B			0x3042
#define	KEY_SHFT_N			0x314E
#define	KEY_SHFT_M			0x324D
#define KEY_SHFT_COMMA		0x333C
#define KEY_SHFT_DOT		0x343E
#define KEY_SHFT_SLASH		0x353F
#define KEY_SHFT_PAD_7		0x4700
#define KEY_SHFT_PAD_8		0x4800
#define KEY_SHFT_PAD_9		0x4900
#define KEY_SHFT_PAD_MINUS	0x4A2D
#define KEY_SHFT_PAD_4		0x4B00
#define KEY_SHFT_PAD_5		0x4C00
#define KEY_SHFT_PAD_6		0x4D00
#define KEY_SHFT_PAD_PLUS	0x4E2B
#define KEY_SHFT_PAD_1		0x4F00
#define KEY_SHFT_PAD_2		0x5000
#define KEY_SHFT_PAD_3		0x5100
#define KEY_SHFT_PAD_0		0x5200
#define KEY_SHFT_PAD_DOT	0x532E
#define KEY_SHFT_F1			0x5400
#define KEY_SHFT_F2			0x5500
#define KEY_SHFT_F3			0x5600
#define KEY_SHFT_45			0x5600
#define KEY_SHFT_F4			0x5700
#define KEY_SHFT_F5			0x5800
#define KEY_SHFT_F6			0x5900
#define KEY_SHFT_F7			0x5A00
#define KEY_SHFT_F8			0x5B00
#define KEY_SHFT_F9			0x5C00
#define KEY_SHFT_F10		0x5D00
#define KEY_SHFT_F11		0x8700
#define KEY_SHFT_F12		0x8800
#define KEY_SHFT_PAD_ENTER	0xE00D	// Enter
#define KEY_SHFT_PAD_SLASH	0xE02F	// /

//=============================================================================
//  ALT : key code (ascii code = 0x00)
//=============================================================================

// TODO keys : ,./;'[]

// N/A keys :
// F11, F12, PrtScrn, ScrollLock, Pause/Break, NUM_LOCK
// L-Alt, R-Alt, Win, Menu, L-Ctrl, R-Ctrl, L-Shift, R-Shift

// unsupported key
// make   break   key name
// E0,5B  E0,DB   L-GUI
// E0,1D  E0,9D   R-CTRL
// E0,5C  E0,DC   R-GUI
// E0,38  E0,D8   R-ALT
// E0,5D  E0,DD   APPS

// E0,2A,E0,37    E0,B7,E0,AA   Print Screen
// E1,1D,45,E1,9D,C5            Pause



//=============================================================================
//  Keyboard Scan Code
//=============================================================================
//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//  n/a    00     80      00     F0 00
//  ESC    01     81      76     F0 76  Escape
//  ! 1    02     82      16     F0 16  Exclamation Mark(UK)/Point(US)
//  @ 2    03     83      1E     F0 1E  At
//  # 3    04     84      26     F0 26  Hashtag
//  $ 4    05     85      25     F0 25  Dollar Sign
//  % 5    06     86      2E     F0 2E  Percent
//  ^ 6    07     87      36     F0 36  
//  & 7    08     88      3D     F0 3D
//  * 8    09     89      3E     F0 3E
//  ( 9    0A     8A      46     F0 46
//  ) 0    0B     8B      45     F0 45
//  _ -    0C     8C      4E     F0 4E
//  + =    0D     8D      55     F0 55
//  BS     0E     8E      66     F0 66  Backspace
//  TAB    0F     8F      0D     F0 0D
//   Q     10     90      15     F0 15
//   W     11     91      1D     F0 1D
//   E     12     92      24     F0 24
//   R     13     93      2D     F0 2D
//   T     14     94      2C     F0 2C
//   Y     15     95      35     F0 35
//   U     16     96      3C     F0 3C
//   I     17     97      43     F0 43
//   O     18     98      44     F0 44
//   P     19     99      4D     F0 4D
//  { [    1A     9A      54     F0 54  
//  } ]    1B     9B      5B     F0 5B
//  ENT    1C     9C      5A     F0 5A  Enter
//  LCTL   1D     9D      14     F0 14  L-Ctrl
//   A     1E     9E      1C     F0 1C
//   S     1F     9F      1B     F0 1B

//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//   D     20     A0      23     F0 23
//   F     21     A1      2B     F0 2B
//   G     22     A2      34     F0 34
//   H     23     A3      33     F0 33
//   J     24     A4      3B     F0 3B
//   K     25     A5      42     F0 42
//   L     26     A6      4B     F0 4B
//  : ;    27     A7      4C     F0 4C
//  " '    28     A8      52     F0 52
//  ~ `    29     A9      0E     F0 0E  Grave Accent
//  LSHF   2A     AA      12     F0 12  L-Shift
//  | \    2B     AB      5D     F0 5D  Pipe, Backslash
//   Z     2C     AC      1A     F0 1A
//   X     2D     AD      22     F0 22
//   C     2E     AE      21     F0 21
//   V     2F     AF      2A     F0 2A
//   B     30     B0      32     F0 32
//   N     31     B1      31     F0 31
//   M     32     B2      3A     F0 3A
//  < ,    33     B3      41     F0 41
//  > .    34     B4      49     F0 49
//  ? /    35     B5      4A     F0 4A
//  RSHF   36     B6      59     F0 59     R-Shift
//  KP *   37     B7      7C     F0 7C
//  LALT   38     B8      11     F0 11     L-Alt
//  SPC    39     B9      29     F0 29     Space
//  CAP    3A     BA      58     F0 58  Caps Lock
//  F1     3B     BB      05     F0 05
//  F2     3C     BC      06     F0 06
//  F3     3D     BD      04     F0 04
//  F4     3E     BE      0C     F0 0C
//  F5     3F     BF      03     F0 03

//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//  F6     40     C0      0B     F0 0B
//  F7     41     C1      83     F0 83
//  F8     42     C2      0A     F0 0A
//  F9     43     C3      01     F0 01
//  F10    44     C4      09     F0 09
//  NLCK   45     C5      77     F0 77     Num Lcok
//  ScrLck 46     C6      7E     F0 7E     Scroll Lock
//  KP 7   47     C7      6C     F0 6C
//  KP 8   48     C8      75     F0 75
//  KP 9   49     C9      7D     F0 7D
//  KP -   4A     CA      7B     F0 7B
//  KP 4   4B     CB      6B     F0 6B
//  KP 5   4C     CC      73     F0 73
//  KP 6   4D     CD      74     F0 74
//  KP +   4E     CE      79     F0 79
//  KP 1   4F     CF      69     F0 69
//  KP 2   50     D0      72     F0 72
//  KP 3   51     D1      7A     F0 7A
//  KP 0   52     D2      70     F0 70
//  KP .   53     D3      71     F0 71
//  F11    57     D7      78     F0 78
//  F12    58     D8      07     F0 07
//  KATA   70     F0      19     F0 19     Japan Katakana
//  SBC    77     F7      62     F0 62     Deschutes SBCSCHAR
//  CONV   79     F9      64     F0 64     Convert
//  NCONV  7B     FB      67     F0 67     Non-Convert

//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//  PREV   E0 10  E0 90   E0 --  E0 -- --  Previous Track
//  W3VOIP E0 11  E0 91   E0 --  E0 -- --  WWW VOIP (OEM key)
//  NEXT   E0 19  E0 99   E0 --  E0 -- --  Next Track
//  KP ENT E0 1C  E0 9C   E0 5A  E0 F0 5A  KeyPad Enter
//  RCTL   E0 1D  E0 9D   E0 14  E0 F0 14  R-Ctrl
//  MUTE   E0 20  E0 A0   E0 --  E0 -- --  Mute
//  CALC   E0 21  E0 A1   E0 --  E0 -- --  Calculator
//  PLAY   E0 22  E0 A2   E0 --  E0 -- --  Play/Pause
//  STOP   E0 24  E0 A4   E0 --  E0 -- --  Stop
//  VOL-   E0 2E  E0 AE   E0 --  E0 -- --  Volume Down
//  VOL+   E0 30  E0 B0   E0 --  E0 -- --  Volume Up
//  W3HOME E0 32  E0 B2   E0 --  E0 -- --  WWW Home
//  KP /   E0 35  E0 B5   E0 4A  E0 F0 4A  Keypad /
//  RALT   E0 38  E0 B8   E0 11  E0 F0 11  R-Alt
//  HOME   E0 47  E0 C7   E0 6C  E0 F0 6C
//  AR-U   E0 48  E0 C8   E0 75  E0 F0 75  Arrow-Up
//  PGUP   E0 49  E0 C9   E0 7D  E0 F0 7D  Page-Up
//  AR-L   E0 4B  E0 CB   E0 6B  E0 F0 6B  Arrow-Left
//  AR-R   E0 4D  E0 CD   E0 74  E0 F0 74  Arrow-Right
//  END    E0 4F  E0 CF   E0 69  E0 F0 69

//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//  AR-D   E0 50  E0 D0   E0 72  E0 F0 72  Arrow-Down
//  PGDN   E0 51  E0 D1   E0 7A  E0 F0 7A  Page-Down
//  INS    E0 52  E0 D2   E0 70  E0 F0 70
//  DEL    E0 53  E0 D3   E0 71  E0 F0 71
//  LWIN   E0 5B  E0 DB   E0 1F  F0 E0 1F  Left Win
//  RWIN   E0 5C  E0 DC   E0 27  F0 E0 27  Right Win
//  APP    E0 5D  E0 DD   E0 2F  F0 E0 2F  Application
//  POWER  E0 5E  E0 DE   E0 37  F0 E0 37  ACPI Power Button
//  SLEEP  E0 5F  E0 DF   E0 3F  F0 E0 3F  ACPI Sleep Button
//  WAKE   E0 63  E0 E3   E0 5E  F0 E0 5E  ACPI Wake
//  W3SRCH E0 65  E0 E5   E0 --  E0 -- --  WWW Search
//  W3FAVO E0 66  E0 E6   E0 --  E0 -- --  WWW Favorite
//  W3RFRH E0 67  E0 E7   E0 --  E0 -- --  WWW Refresh
//  W3STOP E0 68  E0 E8   E0 --  E0 -- --  WWW Stop
//  W3FWRD E0 69  E0 E9   E0 --  E0 -- --  WWW Forward
//  W3BWRD E0 6A  E0 EA   E0 --  E0 -- --  WWW Backward
//  MYPC   E0 6B  E0 EB   E0 --  E0 -- --  My Computer
//  EMAIL  E0 6C  E0 EC   E0 --  E0 -- --  E-Mail
//  MDSEL  E0 6D  E0 ED   E0 --  E0 -- --  Media Select

//  Key    Set_1  Set_1   Set_2  Set_2
//  Char   Make   Break   Make   Break  Name
//-----------------------------------------------------------------------------
//         2B     AB      5D     F0 5D     yes-Intl.keyboard, no-US keyboard
//         56     D6      61     F0 61     yes-Intl.keyboard, no-US keyboard
//         73     F3      51     F0 51     yes-Barzilian/Far-East, no-US keyboard
//         7E     FE      6D     F0 6D     yes-Barzilian/Far-East, no-US keyboard

//  PrtScr key : no-break-key
//  E0 2A E0 37   E0 B7 E0 AA       : Set 1, with make and break
//  E0 12 E0 7C   E0 F0 7C E0 F0 12 : Set 2, with make and break

//  Pause key : no-break-key
//  E1 1D 45 E1 9D C5       : Set 1, without break
//  E1 14 77 E1 F0 14 F0 77 : Set 2, without break

//=============================================================================
//  Keyboard Scan Code Set 1
//=============================================================================
#define KEY_SC1_ESC			0x01
#define KEY_SC1_1			0x02
#define KEY_SC1_2			0x03
#define KEY_SC1_3			0x04
#define KEY_SC1_4			0x05
#define KEY_SC1_5			0x06
#define KEY_SC1_6			0x07
#define KEY_SC1_7			0x08
#define KEY_SC1_8			0x09
#define KEY_SC1_9			0x0A
#define KEY_SC1_0			0x0B
#define KEY_SC1_MINUS		0x0C
#define KEY_SC1_EQUAL		0x0D
#define KEY_SC1_BS			0x0E
#define KEY_SC1_TAB			0x0F

#define KEY_SC1_Q			0x10
#define KEY_SC1_W			0x11
#define KEY_SC1_E			0x12
#define KEY_SC1_R			0x13
#define KEY_SC1_T			0x14
#define KEY_SC1_Y			0x15
#define KEY_SC1_U			0x16
#define KEY_SC1_I			0x17
#define KEY_SC1_O			0x18
#define KEY_SC1_P			0x19
#define KEY_SC1_LBRKT		0x1A
#define KEY_SC1_RBRKT		0x1B
#define KEY_SC1_ENTER		0x1C
#define KEY_SC1_LCTRL		0x1D
#define KEY_SC1_A			0x1E
#define KEY_SC1_S			0x1F

#define KEY_SC1_D			0x20
#define KEY_SC1_F			0x21
#define KEY_SC1_G			0x22
#define KEY_SC1_H			0x23
#define KEY_SC1_J			0x24
#define KEY_SC1_K			0x25
#define KEY_SC1_L			0x26
#define KEY_SC1_COLON		0x27
#define KEY_SC1_QUOT		0x28
#define KEY_SC1_TILDE		0x29
#define KEY_SC1_LSHFT		0x2A
#define KEY_SC1_BKSLASH		0x2B
#define KEY_SC1_Z			0x2C
#define KEY_SC1_X			0x2D
#define KEY_SC1_C			0x2E
#define KEY_SC1_V			0x2F

#define KEY_SC1_B			0x30
#define KEY_SC1_N			0x31
#define KEY_SC1_M			0x32
#define KEY_SC1_COMMA		0x33
#define KEY_SC1_DOT			0x34
#define KEY_SC1_SLASH		0x35
#define KEY_SC1_RSHFT		0x36
#define KEY_SC1_PAD_STAR	0x37
#define KEY_SC1_LALT		0x38
#define KEY_SC1_SPACE		0x39
#define KEY_SC1_CAPLCK		0x3A
#define KEY_SC1_F1			0x3B
#define KEY_SC1_F2			0x3C
#define KEY_SC1_F3			0x3D
#define KEY_SC1_F4			0x3E
#define KEY_SC1_F5			0x3F

#define KEY_SC1_F6			0x40
#define KEY_SC1_F7			0x41
#define KEY_SC1_F8			0x42
#define KEY_SC1_F9			0x43
#define KEY_SC1_F10			0x44
#define KEY_SC1_NUMLCK		0x45
#define KEY_SC1_SCRLCK		0x46
#define KEY_SC1_PAD_7		0x47
#define KEY_SC1_PAD_8		0x48
#define KEY_SC1_PAD_9		0x49
#define KEY_SC1_PAD_MINUS	0x4A
#define KEY_SC1_PAD_4		0x4B
#define KEY_SC1_PAD_5		0x4C
#define KEY_SC1_PAD_6		0x4D
#define KEY_SC1_PAD_PLUS	0x4E
#define KEY_SC1_PAD_1		0x4F

#define KEY_SC1_PAD_2		0x50
#define KEY_SC1_PAD_3		0x51
#define KEY_SC1_PAD_0		0x52
#define KEY_SC1_PAD_DOT		0x53

#define KEY_SC1_45			0x56	// this key sandwiched between L-Shift and Z, named OEM5

#define KEY_SC1_F11			0x57
#define KEY_SC1_F12			0x58

#define KEY_SC1_KATA		0x70		// Japan Katakana
#define KEY_SC1_SBCSCHR		0x77		// Deschutes SBCSCHAR
#define KEY_SC1_CONV		0x79		// Convert
#define KEY_SC1_NCONV		0x7B		// Non-Convert

// define bit7=1 : E0 prefix code
// example : E0 1C=keypad enter => 0x9C
#define KEY_SC1_PREV		0x90	// E0 10 : Previous Track
#define KEY_SC1_W3VOIP		0x91	// E0 11 : WWW VOIP (OEM key)
#define KEY_SC1_NEXT		0x99	// E0 19 : Next Track
#define KEY_SC1_PAD_ENTER	0x9C	// E0 1C : KeyPad Enter
#define KEY_SC1_RCTRL		0x9D	// E0 1D : R-Ctrl
#define KEY_SC1_MUTE		0xA0	// E0 20 : Mute
#define KEY_SC1_CALC		0xA1	// E0 21 : Calculator
#define KEY_SC1_PLAY		0xA2	// E0 22 : Play/Pause
#define KEY_SC1_STOP		0xA4	// E0 24 : Stop
#define KEY_SC1_VOL_DOWN	0xAE	// E0 2E : Volume Down
#define KEY_SC1_VOL_UP		0xB0	// E0 30 : Volume Up
#define KEY_SC1_W3HOME		0xB2	// E0 32 : WWW Home
#define KEY_SC1_PAD_SLASH	0xB5	// E0 35 : Keypad /
#define KEY_SC1_RALT		0xB8	// E0 38 : R-Alt
#define KEY_SC1_HOME		0xC7	// E0 47 : Home
#define KEY_SC1_UP			0xC8	// E0 48 : Arrow-Up
#define KEY_SC1_PGUP		0xC9	// E0 49 : Page-Up
#define KEY_SC1_LEFT		0xCB	// E0 4B : Arrow-Left
#define KEY_SC1_RIGHT		0xCD	// E0 4D : Arrow-Right
#define KEY_SC1_END			0xCF	// E0 4F : 
#define KEY_SC1_DOWN		0xD0	// E0 50 : Arrow-Down
#define KEY_SC1_PGDN		0xD1	// E0 51 : Page-Down
#define KEY_SC1_INS			0xD2	// E0 52 : 
#define KEY_SC1_DEL			0xD3	// E0 53 : 
#define KEY_SC1_LWIN		0xDB	// E0 5B : Left Win
#define KEY_SC1_RWIN		0xDC	// E0 5C : Right Win
#define KEY_SC1_APP			0xDD	// E0 5D : Application
#define KEY_SC1_POWER		0xDE	// E0 5E : ACPI Power Button
#define KEY_SC1_SLEEP		0xDF	// E0 5F : ACPI Sleep Button
#define KEY_SC1_WAKE		0xE3	// E0 63 : ACPI Wake
#define KEY_SC1_W3SRCH		0xE5	// E0 65 : WWW Search
#define KEY_SC1_W3FAVO		0xE6	// E0 66 : WWW Favorite
#define KEY_SC1_W3RFRH		0xE7	// E0 67 : WWW Refresh
#define KEY_SC1_W3STOP		0xE8	// E0 68 : WWW Stop
#define KEY_SC1_W3FWRD		0xE9	// E0 69 : WWW Forward
#define KEY_SC1_W3BWRD		0xEA	// E0 6A : WWW Backward
#define KEY_SC1_MYPC		0xEB	// E0 6B : My Computer
#define KEY_SC1_EMAIL		0xEC	// E0 6C : E-Mail
#define KEY_SC1_MDSEL		0xED	// E0 6D : Media Select

//=============================================================================
//  keyboard service scancode - INT 16h, AH=02h,12h
//=============================================================================
#define KEY_SC_NULL			0x00	// defined : no key pressed
#define KEY_SC_ESC			0x01
#define KEY_SC_1			0x02
#define KEY_SC_2			0x03
#define KEY_SC_3			0x04
#define KEY_SC_4			0x05
#define KEY_SC_5			0x06
#define KEY_SC_6			0x07
#define KEY_SC_7			0x08
#define KEY_SC_8			0x09
#define KEY_SC_9			0x0A
#define KEY_SC_0			0x0B
#define KEY_SC_MINUS		0x0C
#define KEY_SC_EQUAL		0x0D
#define KEY_SC_BS			0x0E
#define KEY_SC_TAB			0x0F

#define KEY_SC_Q			0x10
#define KEY_SC_W			0x11
#define KEY_SC_E			0x12
#define KEY_SC_R			0x13
#define KEY_SC_T			0x14
#define KEY_SC_Y			0x15
#define KEY_SC_U			0x16
#define KEY_SC_I			0x17
#define KEY_SC_O			0x18
#define KEY_SC_P			0x19
#define KEY_SC_LBRKT		0x1A
#define KEY_SC_RBRKT		0x1B
#define KEY_SC_ENTER		0x1C
#define KEY_SC_A			0x1E
#define KEY_SC_S			0x1F

#define KEY_SC_D			0x20
#define KEY_SC_F			0x21
#define KEY_SC_G			0x22
#define KEY_SC_H			0x23
#define KEY_SC_J			0x24
#define KEY_SC_K			0x25
#define KEY_SC_L			0x26
#define KEY_SC_COLON		0x27
#define KEY_SC_QUOT			0x28
#define KEY_SC_TILDE		0x29
#define KEY_SC_BKSLASH		0x2B
#define	KEY_SC_Z			0x2C
#define	KEY_SC_X			0x2D
#define	KEY_SC_C			0x2E
#define	KEY_SC_V			0x2F

#define	KEY_SC_B			0x30
#define	KEY_SC_N			0x31
#define	KEY_SC_M			0x32
#define	KEY_SC_COMMA		0x33
#define	KEY_SC_DOT			0x34
#define	KEY_SC_SLASH		0x35
#define	KEY_SC_STAR			0x37
#define	KEY_SC_SPACE		0x39
#define KEY_SC_CAPLCK		0x3A
#define KEY_SC_F1			0x3B
#define KEY_SC_F2			0x3C
#define KEY_SC_F3			0x3D
#define KEY_SC_F4			0x3E
#define KEY_SC_F5			0x3F

#define KEY_SC_F6			0x40
#define KEY_SC_F7			0x41
#define KEY_SC_F8			0x42
#define KEY_SC_F9			0x43
#define KEY_SC_F10			0x44
#define KEY_SC_PAD_7		0x47
#define KEY_SC_HOME			0x47
#define KEY_SC_PAD_8		0x48
#define KEY_SC_UP			0x48
#define KEY_SC_PAD_9		0x49
#define KEY_SC_PGUP			0x49
#define KEY_SC_PAD_MINUS	0x4A
#define KEY_SC_PAD_4		0x4B
#define KEY_SC_LEFT			0x4B
#define KEY_SC_PAD_CENTER	0x4C
#define KEY_SC_PAD_6		0x4D
#define KEY_SC_RIGHT		0x4D
#define	KEY_SC_PAD_PLUS		0x4E
#define KEY_SC_PAD_1		0x4F
#define KEY_SC_END			0x4F

#define KEY_SC_PAD_2		0x50
#define KEY_SC_DOWN			0x50
#define KEY_SC_PAD_3		0x51
#define KEY_SC_PGDN			0x51
#define KEY_SC_PAD_0		0x52
#define KEY_SC_INS			0x52
#define KEY_SC_PAD_DOT		0x53
#define KEY_SC_DEL			0x53
#define KEY_SC_SHFT_F1		0x54
#define KEY_SC_SHFT_F2		0x55
#define KEY_SC_SHFT_F3		0x56
#define KEY_SC_SHFT_F4		0x57
#define KEY_SC_SHFT_F5		0x58
#define KEY_SC_SHFT_F6		0x59
#define KEY_SC_SHFT_F7		0x5A
#define KEY_SC_SHFT_F8		0x5B
#define KEY_SC_SHFT_F9		0x5C
#define KEY_SC_SHFT_F10		0x5D
#define KEY_SC_CTRL_F1		0x5E
#define KEY_SC_CTRL_F2		0x5F

#define KEY_SC_CTRL_F3		0x60
#define KEY_SC_CTRL_F4		0x61
#define KEY_SC_CTRL_F5		0x62
#define KEY_SC_CTRL_F6		0x63
#define KEY_SC_CTRL_F7		0x64
#define KEY_SC_CTRL_F8		0x65
#define KEY_SC_CTRL_F9		0x66
#define KEY_SC_CTRL_F10		0x67
#define KEY_SC_ALT_F1		0x68
#define KEY_SC_ALT_F2		0x69
#define KEY_SC_ALT_F3		0x6A
#define KEY_SC_ALT_F4		0x6B
#define KEY_SC_ALT_F5		0x6C
#define KEY_SC_ALT_F6		0x6D
#define KEY_SC_ALT_F7		0x6E
#define KEY_SC_ALT_F8		0x6F

#define KEY_SC_ALT_F9		0x70
#define KEY_SC_ALT_F10		0x71
#define KEY_SC_CTRL_LEFT	0x73
#define KEY_SC_CTRL_RIGHT	0x74
#define KEY_SC_CTRL_END		0x75
#define KEY_SC_CTRL_PGDN	0x76
#define KEY_SC_CTRL_HOME	0x77
#define KEY_SC_ALT_1		0x78
#define KEY_SC_ALT_2		0x79
#define KEY_SC_ALT_3		0x7A
#define KEY_SC_ALT_4		0x7B
#define KEY_SC_ALT_5		0x7C
#define KEY_SC_ALT_6		0x7D
#define KEY_SC_ALT_7		0x7E
#define KEY_SC_ALT_8		0x7F

#define KEY_SC_ALT_9		0x80
#define KEY_SC_ALT_0		0x81
#define KEY_SC_ALT_EQUAL	0x83
#define KEY_SC_CTRL_PGUP	0x84
#define KEY_SC_F11			0x85
#define KEY_SC_F12			0x86
#define KEY_SC_SHFT_F11		0x87
#define KEY_SC_SHFT_F12		0x88
#define KEY_SC_CTRL_F11		0x89
#define KEY_SC_CTRL_F12		0x8A
#define KEY_SC_ALT_F11		0x8B
#define KEY_SC_ALT_F12		0x8C
#define KEY_SC_CTRL_UP		0x8D
#define KEY_SC_CTRL_PAD_5	0x8F

#define KEY_SC_CTRL_PLUS	0x90
#define KEY_SC_CTRL_DOWN	0x91
#define KEY_SC_CTRL_INS		0x92
#define KEY_SC_CTRL_DEL		0x93
#define KEY_SC_CTRL_TAB		0x94
#define KEY_SC_CTRL_STAR	0x96
#define KEY_SC_ALT_HOME		0x97
#define KEY_SC_ALT_UP		0x98
#define KEY_SC_ALT_PGUP		0x99
#define KEY_SC_ALT_LEFT		0x9B
#define KEY_SC_ALT_RIGHT	0x9D
#define KEY_SC_ALT_END		0x9F

#define	KEY_SC_ALT_DOWN			0xA0
#define	KEY_SC_ALT_PGDN			0xA1
#define	KEY_SC_ALT_INS			0xA2
#define	KEY_SC_ALT_DEL			0xA3
#define KEY_SC_ALT_TAB			0xA5
#define KEY_SC_ALT_PAD_ENTER	0xA6

#define KEY_SC_SHFT_PAD_ENTER	0xE0	// 0xE00D	// Enter
#define KEY_SC_SHFT_PAD_SLASH	0xE0	// 0xE02F	// /

//=============================================================================
//  ps2_key_t
//=============================================================================
typedef struct _ps2_key_t
{
	uint8_t		sc;		// scan code
	uint8_t		ac;		// ascii code
	uint16_t	kc;		// key code = scan code + ascii code
	uint16_t	ss;		// shift-status

} ps2_key_t;

//=============================================================================
//  functions
//=============================================================================
void	key_blk_read(ps2_key_t *pk);
uint8_t	key_blk_read_sc(void);
uint8_t key_blk_read_ac(void);
uint8_t key_blk_read_ss(uint16_t *shft);

uint8_t	key_non_blk_read(ps2_key_t *pk);
uint8_t key_non_blk_read_sc(void);
uint8_t key_non_blk_read_ac(void);
uint8_t key_non_blk_read_ss(uint16_t *shft);

void	key_getch(void);
void	key_flush_buf(void);

#endif

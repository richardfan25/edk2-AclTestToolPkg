#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#define PROGRESS_NUM_STYLE 0
#define PROGRESS_CHR_STYLE 1
#define PROGRESS_BGC_STYLE 2

#define EFI_SCANEX_ALT				0x0040
#define EFI_SCANEX_CTRL				0x0080

#define gotoxy(x,y)					gST->ConOut->SetCursorPosition(gST->ConOut, x, y)

typedef struct _progress_t{
    char	chr;		/*tip char*/
    char	*title;	/*tip string*/
    int		max;		/*maximum value*/
    float	offset;
    char	*pro;
} progress_t;


extern UINTN ConsoleCurrRow;
extern UINTN ConsoleWidth;
extern UINTN ConsoleHeight;
extern EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConInEx;

void progress_init(progress_t *, char *, int);
void progress_show(progress_t *, float);
void progress_destroy(progress_t *);

EFI_STATUS 	console_init(void);
void 		console_exit(void);
void 		console_color(UINTN txt_color, UINTN bg_color);

int 		bioskey(uint8_t cmd);
void 		print_title(char *title);
uint8_t		dec_to_str(uint16_t dec, uint8_t *str, uint8_t pad_len);
uint16_t	str_to_dec(uint8_t type, char *str);
uint16_t	get_number_stdin(char *msg, uint8_t type, uint8_t digi);
uint16_t	crc16(uint8_t *ptr, int count);
VOID 		debug_num(UINT32 num, UINTN x, UINTN y);
#endif	/*_UTIL_H_*/

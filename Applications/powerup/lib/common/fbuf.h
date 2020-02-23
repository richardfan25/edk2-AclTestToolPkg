//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  FBUF - File Buffer                                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __COMMON_FBUF_H
#define __COMMON_FBUF_H


#define FBUF_BUF_SZ			(64 * 1024)		// 64KB

// error code
#define FBUF_OK				0
#define FBUF_ERR_MALLOC		1
#define FBUF_ERR_FOPEN		2
#define FBUF_ERR_BUF_MALLOC	3

//===========================================================================
//  FILE_BUF
//===========================================================================
typedef struct _fbuf_t
{
	FILE 		*fp;
	uint8_t		*head;		// buffer head
	uint8_t		*pos;		// buffer
	int			sz_full;
	int			sz_used;

} fbuf_t;

//=============================================================================
//  functions
//=============================================================================
void	fbuf_flush(fbuf_t *fb);
int		fbuf_free_space(fbuf_t *fb);
void	fbuf_putc(fbuf_t *fb, uint8_t c);
void	fbuf_puts(fbuf_t *fb, uint8_t *data);
void	fbuf_putd(fbuf_t *fb, uint32_t d, uint8_t pad0, uint8_t maxlen, uint8_t align);
void	fbuf_putx(fbuf_t *fb, uint32_t x, uint8_t capital, uint8_t pad0, uint8_t maxlen);
void	fbuf_putb(fbuf_t *fb, uint32_t b, uint8_t pad0, uint8_t maxlen);
void	fbuf_printf(fbuf_t *fb, const char *fmt, ...);

fbuf_t	*fbuf_init(char *filename, uint8_t *res);
void	fbuf_exit(fbuf_t *fb);

#endif

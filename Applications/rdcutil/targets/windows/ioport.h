#ifndef _IOPORT_LIB_
#define _IOPORT_LIB_
/*
 *----------------------------------------------------------------------------
 * Windows IO Port Access
 *----------------------------------------------------------------------------
 * Support compiler:
 *	Microsoft Visual Studio
 *
 *----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "declarations.h"

#define DPRINTF printf

#define inp inp_winio
#define inpw inpw_winio
#define inpd inpd_winio
#define outp outp_winio
#define outpw outpw_winio
#define outpd outpd_winio

int32_t init_winio();
void deinit_winio();

uint8_t inp_winio(uint16_t port);
uint16_t inpw_winio(uint16_t port);
uint32_t inpd_winio(uint16_t port);
void outp_winio(uint16_t port, uint8_t val);
void outpw_winio(uint16_t port, uint16_t val);
void outpd_winio(uint16_t port, uint32_t val);

#endif /* _IOPORT_LIB_ */

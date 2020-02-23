#ifndef _TSMB_H_
#define _TSMB_H_

#include "x86hw.h"

extern SMB_BUS *smbus;

void 	tsmb_exit(void);
uint8_t tsmb_init(void);
SMB_DEVICE *tsmb_find_dev_type(SMB_DEVICE *st_dev, uint8_t type);

#endif //_TSMB_H_


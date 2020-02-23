#ifndef _MBX_FAN_H_
#define _MBX_FAN_H_

#include <stdint.h>
#include "dev_fan.h"

typedef struct _stMbxFan{
	stFanDev	*dev;
	uint8_t 	dev_cnt;
	
    int		(*init)(void);
    int 	(*updateDevice)(void);
    int 	(*writeDevice)(uint8_t dev_no, uint8_t fanid, uint8_t speedid, uint8_t *wdata, uint8_t *len);
	void	(*toArray)(uint8_t dev_no, uint8_t *odata, uint8_t *len);
	uint8_t	(*getCtrlMode)(uint8_t dev_no);
	void	(*setCtrlMode)(uint8_t dev_no, uint8_t mode);
} stMbxFan;

extern stMbxFan mbx_fan;

#endif // _MBX_FAN_H_


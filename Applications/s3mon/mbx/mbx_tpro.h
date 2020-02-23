#ifndef _MBX_TPRO_H_
#define _MBX_TPRO_H_

#include <stdint.h>
#include "dev_tpro.h"

typedef struct _stMbxTpro{
	stTProDev 	*dev;
	uint8_t 	dev_cnt;
	
    int	(*init)(void);
    int (*updateDevice)(void);
    int (*writeDevice)(uint8_t dev_no, uint8_t *wdata, uint8_t *len);
	void (*getTypeStr)(eTPROType type, char * data);
	void (*toArray)(uint8_t dev_no, uint8_t *odata, uint8_t *len);
} stMbxTpro;

extern stMbxTpro mbx_tpro;

#endif // _MBX_TPRO_H_

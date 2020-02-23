#ifndef _MBX_TML_H_
#define _MBX_TML_H_

#include <stdint.h>
#include "../dev/dev_tml.h"

typedef struct _stMbxTml{
	stTmlDev *dev;
	uint8_t	 dev_cnt;
	
    int 	(*init)(void);
    int 	(*updateDevice)(void);
    int 	(*writeDevice)(uint8_t dev_no, uint8_t smbdid, uint8_t fanid, uint8_t *wdata, uint8_t *len);
	void	(*getTypeStr)(eTMLType type, char *data);
	eTMLType (*getType)(uint8_t dev_no);
} stMbxTml;

extern stMbxTml mbx_tml;

#endif // _MBX_TML_H_

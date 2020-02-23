#ifndef _MBX_DYNA_H_
#define _MBX_DYNA_H_

#include <stdint.h>
#include "../dev/dynatbl.h"

/*----MBX_CMD_DYNATBL Start----*/
#define PARA_DYNATBL_DID    		0
#define PARA_DYNATBL_HWPIN       	1
#define PARA_DYNATBL_ACTIVE      	2
/*----MBX_CMD_DYNATBL End----*/

/*==============================================================*/
typedef struct _stMbxDyna{
	stDyncDev	*dev;
	uint8_t		size;
	
    int 		(*init)(void);
    void 		(*exit)(void);
	int 		(*getTable)(void);
    int 		(*getPinstate)(eDynclDevNo did, uint8_t *idata);
    int 		(*setPinstate)(eDynclDevNo did, uint8_t *odata);
	uint8_t 	(*getHw)(eDynclDevNo did);
	eDynclDevNo	(*getDid)(uint8_t pin_no);
	void 		(*getDevStr)(eDynclDevNo did, char * str);
	void 		(*getTypeStr)(eDynclDevType type, char * str);
	eDynclDevType	(*getDevType)(eDynclDevNo did);
} stMbxDyna;

extern stMbxDyna mbx_dyna;

#endif // _MBX_DYNA_H_

#ifndef _DEV_TPRO_H_
#define _DEV_TPRO_H_

#include <stdint.h>
#include "dev_tml.h"

#define TPRO_CH_MAX  		4
#define tmlL1Warn           0xDA	//?
#define tmlL2Warn           0xDB	//?
#define tmlR1Warn           0xE2	//?
#define tmlR2Warn           0xE5	//?

/*==============================================================*/
typedef enum
{
    TPType_shutDn,       	//0x00
    TPType_throttle,     	//0x01
    TPType_pwrOff,       	//0x02
    TPType_NoDefine = 8,   	//0x08
} eTPROType;

/*==============================================================*/
typedef struct _stTProDev
{
    eTMLType	src;
    eTPROType	type;
    char 		hi_limit;
    char 		lo_limit;
} stTProDev;

extern stTProDev tprodev[TPRO_CH_MAX];
/*==============================================================*/
void tpro_clearData(uint8_t dev_no);
void tpro_updateData(uint8_t dev_no, uint8_t *rdata, uint8_t len);
void tpro_getTypeStr(eTPROType type, char * data);
void tpro_toArray(uint8_t dev_no, uint8_t *odata, uint8_t *len);

#endif //_DEV_TPRO_H_


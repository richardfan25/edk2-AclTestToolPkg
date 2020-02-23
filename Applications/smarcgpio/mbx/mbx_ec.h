#ifndef _MBX_EC_H_
#define _MBX_EC_H_

#include <stdint.h>

typedef struct _stMbxEc{
    int 	(*init)(void);
    int 	(*readEcRam)(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *rdata);
    int 	(*writeEcRam)(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *wdata);
    void 	(*clearBufRam)(void);
    int 	(*readBufRam)(uint8_t *rdata, uint16_t len);
    int 	(*writeBufRam)(uint8_t *wdata, uint16_t len);
	int 	(*readFwInfo)(char *prjname, char *version);
} stMbxEc;

extern stMbxEc mbx_ec;

#endif // _MBX_EC_H_


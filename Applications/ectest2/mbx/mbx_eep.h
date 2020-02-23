#ifndef _MBX_EEP_H_
#define _MBX_EEP_H_

typedef struct _stMbxEep{
	int (*init)(void);
    int (*readData)(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *rdata);
    int (*writeData)(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *wdata, uint8_t len);
} stMbxEep;

extern stMbxEep mbx_eep;

#endif // _MBX_EEP_H_


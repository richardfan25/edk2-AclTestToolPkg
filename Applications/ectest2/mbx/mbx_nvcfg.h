#ifndef _MBX_NVCFG_H_
#define _MBX_NVCFG_H_

#include <stdint.h>

/*----MBX_CMD_NVCFG Start----*/
#define PARA_NVCFG_RD_STS        	0x00
#define PARA_NVCFG_WR_STS        	0x01
#define PARA_NVCFG_RD_CTRL       	0x02
#define PARA_NVCFG_WR_CTRL       	0x03
/****PARA_NVCFG_WR_CTRL start****/ 
#define NVCFG_CTRL_WR_ALL    		BIT(0)
#define NVCFG_CTRL_LOAD_DEF 		BIT(1)
#define NVCFG_CTRL_SAVE_CFG    		BIT(3)
#define NVCFG_CTRL_EEP_WRPRO     	BIT(7)
/****PARA_NVCFG_WR_CTRL end****/ 
#define PARA_NVCFG_RD_CFGBUF  		0x04
#define PARA_NVCFG_WR_CFGBUF  		0x05
#define PARA_NVCFG_SET_CFGBUF 		0x06
/*----MBX_CMD_NVCFG End----*/


typedef struct _stMbxNvCfg{
    int (*init)(void);
    int (*readStatus)(uint8_t *rstatus);
    int (*writeStatus)(uint8_t wstatus);
    int (*readControl)(uint8_t *rstatus);
    int (*writeControl)(uint8_t wstatus);
    int (*readBuf)(uint8_t len, uint8_t *rdata, uint8_t offset);
    int (*writeBuf)(uint8_t len, uint8_t *wdata, uint8_t offset);
} stMbxNvCfg;

extern stMbxNvCfg mbx_nvcfg;

#endif // _MBX_NVCFG_H_


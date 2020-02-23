#ifndef _MBX_WDT_H_
#define _MBX_WDT_H_

#include <stdint.h>

#define WDT_LIMIT_SIZE         	6

/*----MBX_CMD_WDT Start----*/
#define PARA_WDT_START           	0x01
#define PARA_WDT_STOP            	0x02
#define PARA_WDT_RESET           	0x03
#define PARA_WDT_RD_LIMIT      		0x04
#define PARA_WDT_WR_LIMIT     		0x05
#define PARA_WDT_STOP_BOOT    		0x08
/*----MBX_CMD_WDT End----*/

typedef enum
{
    WDTYPE_NoDefine = 0,
    WDTYPE_CldRst,
    WDTYPE_NMIIRQ,
    WDTYPE_WrmRst,
    WDTYPE_WDPin,
    WDTYPE_SCI,
    WDTYPE_Size,    //keep at bottom
}eWDType;


typedef struct _stMbxWdt{
    int (*init)(void);
    int (*start)();
    int (*stop)();
    int (*reset)(uint8_t *r_status);
    int (*readLimit)(uint8_t *r_status, uint16_t *limit);
    int (*writeLimit)(uint8_t *r_status, uint16_t *limit);
    int (*bootStop)();
} stMbxWdt;

extern stMbxWdt mbx_wdt;

#endif // _MBX_WDT_H_


#ifndef _MBX_GPIO_H_
#define _MBX_GPIO_H_

#include <stdint.h>

#define INPUT           0x80
#define OUTPUT          0x40
#define PULL_UP         0x04
#define PULL_DN         0x02

typedef struct _stMbxGpio{
    int (*init)(void);
    int (*getSetting)(uint8_t did, uint8_t *idata);
    int (*setSetting)(uint8_t did, uint8_t odata);
} stMbxGpio;

extern stMbxGpio mbx_gpio;

#endif // _MBX_GPIO_H_


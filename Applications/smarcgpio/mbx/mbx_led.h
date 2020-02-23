#ifndef _MBX_LED_H_
#define _MBX_LED_H_

#include <stdint.h>

typedef struct _stLedDev  //led structure
{
    uint8_t 	pin;
    uint16_t 	control;
    uint8_t 	behavior;
}stLedDev;

typedef struct _stMbxLed{
    int (*init)(void);
    int (*updateDevice)(uint8_t dev_no, uint8_t *gpioid, stLedDev *rdata, uint8_t *len);
    int (*writeDevice)(uint8_t dev_no, uint8_t gpioid, stLedDev *wdata, uint8_t *len);
} stMbxLed;

extern stMbxLed mbx_led;

#endif // _MBX_LED_H_


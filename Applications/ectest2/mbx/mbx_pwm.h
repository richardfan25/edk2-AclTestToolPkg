#ifndef _MBX_PWM_H_
#define _MBX_PWM_H_

#include <stdint.h>

typedef struct _stMbxPwm{
    int (*init)(void);
    int (*getFreq)(uint8_t did, uint16_t *freq);
    int (*setFreq)(uint8_t did, uint16_t freq);
    int (*getPolarity)(uint8_t did, uint8_t *pol);
    int (*setPolarity)(uint8_t did, uint8_t pol);
} stMbxPwm;

extern stMbxPwm mbx_pwm;

#endif // _MBX_PWM_H_


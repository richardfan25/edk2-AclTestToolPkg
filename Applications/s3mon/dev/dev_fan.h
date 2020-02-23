#ifndef _DEV_FAN_H_
#define _DEV_FAN_H_

#include <stdint.h>

#define FAN_CH_MAX			3
/*==============================================================*/
typedef enum
{
    FCtrl_FanOff,      		//0x00
    FCtrl_FanFull,     		//0x01
    FCtrl_FanMan,      		//0x02
    FCtrl_FanSmart,    		//0x03
    FCtrl_FanEnd,    		//0x04 error
} eFanCtrlMode;

/*==============================================================*/
typedef struct _stFanDev{
    uint8_t		code;
    uint8_t 	status;
    uint8_t 	ctrlMode;
    char 		tempThresH;
    char 		tempThresL;
    char 		tempThresGap;
    uint8_t 	PWM_Hi;
    uint8_t 	PWM_Lo;
    uint16_t 	Speed_Hi;
    uint16_t 	Speed_Lo;
    uint8_t 	debounce;
    char 		tempCtrlMode;
    uint16_t 	speedCtrlMode;

    uint8_t 	fan_id;
    uint8_t 	tacho_id;
} stFanDev;

extern stFanDev fandev[FAN_CH_MAX];
/*==============================================================*/
void 	fan_clearData(uint8_t dev_no);
void	fan_updatData(uint8_t dev_no, uint8_t fanid, uint8_t speedid, uint8_t *rdata, uint8_t len);
void	fan_toArray(uint8_t dev_no, uint8_t *odata, uint8_t *len);
uint8_t	fan_getCtrlMode(uint8_t dev_no);
void	fan_setCtrlMode(uint8_t dev_no, uint8_t mode);

#endif //_DEV_FAN_H_


/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  DYNATBL.H
* Purpose : dynamic table object
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#ifndef _DYNATBL_H
#define _DYNATBL_H
#include <stdint.h>

#define DYNC_NULL				0xFF
#define DYNC_ITEM_MAX  			64
#define DYNC_TYPE_STR_MAX		18

/*==============================================================*/
typedef enum
{
    DDevNo_Nodefine,          //0X00
    //USER CONTROL GPIO
    DDevNo_AltGpio00 = 0x10,    //0x10
    DDevNo_AltGpio01,         //0x11
    DDevNo_AltGpio02,         //0x12
    DDevNo_AltGpio03,         //0x13
    DDevNo_AltGpio04,         //0x14
    DDevNo_AltGpio05,         //0x15
    DDevNo_AltGpio06,         //0x16
    DDevNo_AltGpio07,         //0x17
    //USER DEFINE BUTTON
    DDevNo_Btn0,              //0x18
    DDevNo_Btn1,              //0x19
    DDevNo_Btn2,              //0x1A
    DDevNo_Btn3,              //0x1B
    DDevNo_Btn4,              //0x1C
    DDevNo_Btn5,              //0x1D
    DDevNo_Btn6,              //0x1E
    DDevNo_Btn7,              //0x1F
    //PWM FUNCTION
    DDevNo_CPUFAN_2P = 0x20,    //0X20
    DDevNo_CPUFAN_4P,         //0X21
    DDevNo_SYSFAN1_2P,        //0X22
    DDevNo_SYSFAN1_4P,        //0X23
    DDevNo_SYSFAN2_2P,        //0X24
    DDevNo_SYSFAN2_4P,        //0X25
    DDevNo_PWMBRIGHTNESS,     //0X26
    DDevNo_PWMBEEP,           //0X27
    //SMBUS DEVICE
    DDevNo_SMBOEM0 = 0x28,      //0X28
    DDevNo_SMBOEM1,           //0X29
    DDevNo_SMBOEM2,           //0X2A
    DDevNo_SMBEEPROM,         //0X2B
    DDevNo_SMBTHERMAL0,       //0X2C
    DDevNo_SMBTHERMAL1,       //0X2D
    DDevNo_SMBSecurityEEP,    //0X2E
    DDevNo_I2COEM,            //0X2F
    //DAC DEVICE
    DDevNo_DACSPEAKER = 0x30,   //0X30
    //SMBUS DEVICE 2
    DDevNo_SMBEEP2K = 0x38,     //0x38
    DDevNo_OEMEEP,            //0x39
    DDevNo_OEMEEP2K,          //0x3A
    DDevNo_PECI,              //0x3B, PECI pins
    DDevNo_SMBOEM3,           //0X3C. SMBUS channel 3
    DDevNo_SMLINK,            //0X3D. SMBUS channel 3
    DDevNo_SMBSlv,            //0x3E, Slave mode channel
    //LED
    DDevNo_PowerLed = 0x40,     //0x40
    DDevNo_BatLedG,           //0x41
    DDevNo_OemLed0,           //0x42
    DDevNo_OemLed1,           //0x43
    DDevNo_OemLed2,           //0x44
    DDevNo_BatLedR,           //0x45
    //smart battery
    DDevNo_SmartBat1 = 0x48,    //0x48
    DDevNo_SmartBat2,         //0x49
    //ADC Device
    DDevNo_ADCCMOSBAT = 0x50,   //0X50, CMOS coin battery voltage
    DDevNo_ADCCMOSBATx2,      //0X51, CMOS coin battery voltage, need *2
    DDevNo_ADCCMOSBATx10,     //0X52, CMOS coin battery voltage, need *10
    DDevNo_ADCBAT,            //0X53, Li Battery voltage
    DDevNo_ADCBATx2,          //0X54, Li Battery voltage, need *2
    DDevNo_ADCBATx10,         //0X55, Li Battery voltage, need *10
    DDevNo_ADC5VS0,           //0X56, 5VS0 voltage
    DDevNo_ADC5VS0x2,         //0X57, 5VS0 voltage, need *2
    DDevNo_ADC5VS0x10,        //0X58, 5VS0 voltage, need *10
    DDevNo_ADC5VS5,           //0X59, 5VS5 voltage
    DDevNo_ADC5VS5x2,         //0X5A, 5VS5 voltage, need *2
    DDevNo_ADC5VS5x10,        //0X5B, 5VS5 voltage, need *10
    DDevNo_ADC33VS0,          //0X5C, 3.3VS0 voltage
    DDevNo_ADC33VS0x2,        //0X5D, 3.3VS0 voltage, need *2
    DDevNo_ADC33VS0x10,       //0X5E, 3.3VS0 voltage, need *10
    DDevNo_ADC33VS5,          //0X5F, 3.3VS5 voltage
    DDevNo_ADC33VS5x2,        //0X60, 3.3VS5 voltage, need *2
    DDevNo_ADC33VS5x10,       //0X61, 3.3VS5 voltage, need *10
    DDevNo_ADC12VS0,          //0X62, 12VS0 voltage
    DDevNo_ADC12VS0x2,        //0X63, 12VS0 voltage, need *2
    DDevNo_ADC12VS0x10,       //0X64, 12VS0 voltage, need *10
    DDevNo_ADCVCOREA,         //0X65, CPU A core voltage
    DDevNo_ADCVCOREAx2,       //0X66, CPU A core voltage, need *2
    DDevNo_ADCVCOREAx10,      //0X67, CPU A core voltage, need *10
    DDevNo_ADCVCOREB,         //0X68, CPU B core voltage
    DDevNo_ADCVCOREBx2,       //0X69, CPU B core voltage, need *2
    DDevNo_ADCVCOREBx10,      //0X6A, CPU B core voltage, need *10
    DDevNo_ADCDC,             //0X6B, onboard voltage
    DDevNo_ADCDCx2,           //0X6C, onboard voltage, need *2
    DDevNo_ADCDCx10,          //0X6D, onboard voltage, need *10
    DDevNo_ADCDCSTBY,         //0X6E, standby voltage
    DDevNo_ADCDCSTBYx2,       //0X6F, standby voltage, need *2
    DDevNo_ADCDCSTBYx10,      //0X70, standby voltage, need *10
    DDevNo_ADCDCOther,        //0X71, other voltage
    DDevNo_ADCDCOtherx2,      //0X72, other voltage, need *2
    DDevNo_ADCDCOtherx10,     //0X73, other voltage, need *10
    DDevNo_ADCCurrent,        //0X74, DC Current, //V02_00.01
    //watchdog type
    DDevNo_WDIRQ = 0x78,        //0x78, watchdog IRQ type
    DDevNo_WDNMI,             //0x79, watchdog NMI type
    //Fan Tacho
    DDevNo_CPUFanSpeed0 = 0x80, //0x80, cpu fan speed
    DDevNo_SYSFanSpeed0,      //0x81, speed 1
    DDevNo_SYSFanSpeed1,      //0x82, speed 2
    //Brightness control
    DDevNo_PWMBRIGHTNESS2 = 0x88,   //0x88, second brightness control
    DDevNo_BriOnOff1,         //0x89, backlight on/off control 1
    DDevNo_BriOnOff2,         //0x8A, backlight on/off control 2
    //Temperature
    DDevNo_Temperature_CPU0 = 0x90, //0x90, CPU0 temperature
    DDevNo_Temperature_CPU1,  //0x91, CPU1 temperature
    DDevNo_Temperature_SYS0,  //0x92, SYS0 temperature
    DDevNo_Temperature_SYS1,  //0x93, SYS1 temperature
    //Thermal Protect type
    DDevNo_TherPro_ShutDown,  //0x94, thermal protect shutdown function
    DDevNo_TherPro_PwrOff,    //0x95, thermal protect power off function
    DDevNo_TherPro_Throttle,  //0x96, thermal protect throttle function
    //watchdog type
    DDevNo_WD_WRST,           //0x97, watchdog warm reset
    DDevNo_WD_CRST,           //0x98, watchdog cold reset
    DDevNo_WD_SCI,            //0x99, watchdog sci
    DDevNo_WD_IRQNMI,         //0x9A, watchdog IRQ/NMI
    DDevNo_WD_WDPin,          //0x9B, watchdog Watchdog pin
    //EEPRom software write protect function
    DDevNo_EEPRom_WPF,        //0x9C, EEPRom write protect function
} eDynclDevNo;
/*==============================================================*/
typedef enum
{
    DDevType_Nodefine,      //0
    DDevType_GPIO,          //1
    DDevType_OEMGPIO,       //2
    DDevType_ADC,           //3
    DDevType_DAC,           //4
    DDevType_TACHO,         //5
    DDevType_PWM,           //6
    DDevType_SMB,           //7
    DDevType_Battery,       //8
    DDevType_IRQNMI,        //9
    DDevType_Func,          //10
} eDynclDevType;

/*==============================================================*/
typedef struct _stDyncDev{
	uint8_t		did;
    uint8_t		type;
    uint8_t		pin_no;
    uint8_t		active;
    char		type_str[DYNC_TYPE_STR_MAX];
} stDyncDev;

/*==============================================================*/
stDyncDev 	*dyna_newDevice(uint8_t size);
void 		dyna_freeDevice(stDyncDev *dev, uint8_t size);
void 		dyna_getDevStr(eDynclDevNo did, char *str);
void 		dyna_getTypeStr(eDynclDevType type, char *str);
eDynclDevType	dyna_getDevType(eDynclDevNo did);

#endif //_DYNATBL_H

/******************************************************************
* Copyright(c) , Advantech Corporation 2017
* Module Name:  dynatbl
* Purpose : dynamic table object
* Data    : 2017/05/26
* Author  : Duncan.Tasi
********************************************************************/
#include <string.h>
#include <stdlib.h>

#include "dynatbl.h"

/*===========================================================
 * Name  : dyna_getDevStr
 * Pupose: convert device id to string
 * Input : NA
 * Output: NA
 *===========================================================*/
void dyna_getDevStr(eDynclDevNo did, char * str)
{
    str[0] = '\0';
    switch (did)
	{
    //GPIO
    case DDevNo_AltGpio00:
        strcpy(str, "OEMGPIO00");
		break;
    case DDevNo_AltGpio01:
        strcpy(str, "OEMGPIO01");
		break;
    case DDevNo_AltGpio02:
        strcpy(str, "OEMGPIO02");
		break;
    case DDevNo_AltGpio03:
        strcpy(str, "OEMGPIO03");
		break;
    case DDevNo_AltGpio04:
        strcpy(str, "OEMGPIO04");
		break;
    case DDevNo_AltGpio05:
        strcpy(str, "OEMGPIO05");
		break;
    case DDevNo_AltGpio06:
        strcpy(str, "OEMGPIO06");
		break;
    case DDevNo_AltGpio07:
        strcpy(str, "OEMGPIO07");
        break;
    case DDevNo_AltGpio08:
        strcpy(str, "OEMGPIO08");
        break;
    case DDevNo_AltGpio09:
        strcpy(str, "OEMGPIO09");
        break;
    case DDevNo_AltGpio10:
        strcpy(str, "OEMGPIO10");
        break;
    case DDevNo_AltGpio11:
        strcpy(str, "OEMGPIO11");
        break;
    case DDevNo_AltGpio12:
        strcpy(str, "OEMGPIO12");
        break;
    case DDevNo_AltGpio13:
        strcpy(str, "OEMGPIO13");
        break;
    case DDevNo_AltGpio14:
        strcpy(str, "OEMGPIO14");
        break;
    case DDevNo_AltGpio15:
        strcpy(str, "OEMGPIO15");
        break;
    case DDevNo_Btn0:
        strcpy(str, "Button0");
		break;
    case DDevNo_Btn1:
        strcpy(str, "Button1");
		break;
    case DDevNo_Btn2:
        strcpy(str, "Button2");
		break;
    case DDevNo_Btn3:
        strcpy(str, "Button3");
		break;
    case DDevNo_Btn4:
        strcpy(str, "Button4");
		break;
    case DDevNo_Btn5:
        strcpy(str, "Button5");
		break;
    case DDevNo_Btn6:
        strcpy(str, "Button6");
		break;
    case DDevNo_Btn7:
        strcpy(str, "Button7");
		break;
    case DDevNo_BriOnOff1:
        strcpy(str, "LVDS_On_Off1");
		break;
    case DDevNo_BriOnOff2:
        strcpy(str, "LVDS_On_Off2");
		break;
    case DDevNo_PowerLed:
        strcpy(str, "Power_LED");
		break;
    case DDevNo_BatLedG:
        strcpy(str, "Battery_Led_Green");
		break;
    case DDevNo_OemLed0:
        strcpy(str, "OEM_LED0");
		break;
    case DDevNo_OemLed1:
        strcpy(str, "OEM_LED1");
		break;
    case DDevNo_OemLed2:
        strcpy(str, "OEM_LED2");
		break;
    case DDevNo_BatLedR:
        strcpy(str, "Battery_Led_Red");
		break;
    //PWM
    case DDevNo_CPUFAN_2P:
        strcpy(str, "CPUFan_2P");
		break;
    case DDevNo_CPUFAN_4P:
        strcpy(str, "CPUFan_4P");
		break;
    case DDevNo_SYSFAN1_2P:
        strcpy(str, "SYSFan1_2P");
		break;
    case DDevNo_SYSFAN1_4P:
        strcpy(str, "SYSFan1_4P");
		break;
    case DDevNo_SYSFAN2_2P:
        strcpy(str, "SYSFan2_2P");
		break;
    case DDevNo_SYSFAN2_4P:
        strcpy(str, "SYSFan2_4P");
		break;
    case DDevNo_PWMBRIGHTNESS:
        strcpy(str, "LVDS1_PWM");
		break;
    case DDevNo_PWMBEEP:
        strcpy(str, "BEEP_PWM");
		break;
    case DDevNo_PWMBRIGHTNESS2:
        strcpy(str, "LVDS2_PWM");
		break;
    //SMBUS
    case DDevNo_SMBOEM0:
        strcpy(str, "SMBus_OEM0");
		break;
    case DDevNo_SMBOEM1:
        strcpy(str, "SMBus_OEM1");
		break;
    case DDevNo_SMBOEM2:
        strcpy(str, "SMBus_OEM2");
		break;
    case DDevNo_SMBEEPROM:
        strcpy(str, "SMB_EEPRom");
		break;
    case DDevNo_SMBTHERMAL0:
        strcpy(str, "SMB_Thermal0");
		break;
    case DDevNo_SMBTHERMAL1:
        strcpy(str, "SMB_Thermal1");
		break;
    case DDevNo_SMBSecurityEEP:
        strcpy(str, "Security_EEPRom");
		break;
    case DDevNo_I2COEM:
        strcpy(str, "I2C_OEM0");
		break;
    case DDevNo_SMBEEP2K:
        strcpy(str, "SMB_EEPR_>2K");
		break;
    case DDevNo_OEMEEP:
        strcpy(str, "OEM_EEPRom");
		break;
    case DDevNo_OEMEEP2K:
        strcpy(str, "OEM_EEPRom_>2K");
		break;
    case DDevNo_PECI:
        strcpy(str, "PECI");
		break;
    case DDevNo_SMBOEM3:
        strcpy(str, "SMBus_OEM3");
		break;
    case DDevNo_SMLINK:
        strcpy(str, "SMLink");
		break;
    case DDevNo_SMBSlv:
        strcpy(str, "SMB_Slave_Mode");
		break;
    case DDevNo_SMBOEM4:
        strcpy(str, "SMBus_OEM4");
        break;
    case DDevNo_SMBOEM5:
        strcpy(str, "SMBus_OEM5");
        break;
    //DAC DEVICE
    case DDevNo_DACSPEAKER:
        strcpy(str, "Speaker_DAC");
		break;
    //smart battery
    case DDevNo_SmartBat1:
        strcpy(str, "Battery1");
		break;
    case DDevNo_SmartBat2:
        strcpy(str, "Battery2");
		break;
    //ADC Device
    case DDevNo_ADCCMOSBAT:
    case DDevNo_ADCCMOSBATx2:
    case DDevNo_ADCCMOSBATx10:
        strcpy(str, "CMOS_Battery_Volt");
		break;
    case DDevNo_ADCBAT:
    case DDevNo_ADCBATx2:
    case DDevNo_ADCBATx10:
        strcpy(str, "Battery_Volt");
		break;
    case DDevNo_ADC5VS0:
    case DDevNo_ADC5VS0x2:
    case DDevNo_ADC5VS0x10:
        strcpy(str, "5VS0_Volt");
		break;
    case DDevNo_ADC5VS5:
    case DDevNo_ADC5VS5x2:
    case DDevNo_ADC5VS5x10:
        strcpy(str, "5VS5_Volt");
		break;
    case DDevNo_ADC33VS0:
    case DDevNo_ADC33VS0x2:
    case DDevNo_ADC33VS0x10:
        strcpy(str, "3.3VS0_Volt");
		break;
    case DDevNo_ADC33VS5:
    case DDevNo_ADC33VS5x2:
    case DDevNo_ADC33VS5x10:
        strcpy(str, "3.3VS5_Volt");
		break;
    case DDevNo_ADC12VS0:
    case DDevNo_ADC12VS0x2:
    case DDevNo_ADC12VS0x10:
        strcpy(str, "12VS0_Volt");
		break;
    case DDevNo_ADCVCOREA:
    case DDevNo_ADCVCOREAx2:
    case DDevNo_ADCVCOREAx10:
        strcpy(str, "VCoreA_Volt");
		break;
    case DDevNo_ADCVCOREB:
    case DDevNo_ADCVCOREBx2:
    case DDevNo_ADCVCOREBx10:
        strcpy(str, "VCoreB_Volt");
		break;
    case DDevNo_ADCDC:
    case DDevNo_ADCDCx2:
    case DDevNo_ADCDCx10:
        strcpy(str, "DC_Volt");
		break;
    case DDevNo_ADCDCSTBY:
    case DDevNo_ADCDCSTBYx2:
    case DDevNo_ADCDCSTBYx10:
        strcpy(str, "DCSTBY_Volt");
		break;
    case DDevNo_ADCDCOther:
    case DDevNo_ADCDCOtherx2:
    case DDevNo_ADCDCOtherx10:
        strcpy(str, "Other_Volt");
		break;
    case DDevNo_ADCCurrent:
        strcpy(str, "Current_ADC");
		break;
    //watchdog type
    case DDevNo_WDIRQ:
        strcpy(str, "IRQ");
		break;
    case DDevNo_WDNMI:
        strcpy(str, "NMI");
		break;
    //Fan Tacho
    case DDevNo_CPUFanSpeed0:
        strcpy(str, "CPU0_FanSpeed");
		break;
    case DDevNo_SYSFanSpeed0:
        strcpy(str, "SYS0_FanSpeed");
		break;
    case DDevNo_SYSFanSpeed1:
        strcpy(str, "SYS1_FanSpeed");
		break;
    //Temperature
    case DDevNo_Temperature_CPU0:
        strcpy(str, "CPU_Temperature0");
		break;
    case DDevNo_Temperature_CPU1:
        strcpy(str, "CPU_Temperature1");
		break;
    case DDevNo_Temperature_SYS0:
        strcpy(str, "System_Temperature0");
		break;
    case DDevNo_Temperature_SYS1:
        strcpy(str, "System_Temperature1");
		break;
    //Thermal Protect type
    case DDevNo_TherPro_ShutDown:
        strcpy(str, "TmlPro_Shutdown");
		break;
    case DDevNo_TherPro_PwrOff:
        strcpy(str, "TmlPro_Pwroff");
		break;
    case DDevNo_TherPro_Throttle:
        strcpy(str, "TmlPro_Throttle");
		break;
    //watchdog type
    case DDevNo_WD_WRST:
        strcpy(str, "Watchdog_WarmRST");
		break;
    case DDevNo_WD_CRST:
        strcpy(str, "Watchdog_ColdRST");
		break;
    case DDevNo_WD_SCI:
        strcpy(str, "Watchdog_SCI");
		break;
    case DDevNo_WD_IRQNMI:
        strcpy(str, "Watchdog_IRQ/NMI");
		break;
    case DDevNo_WD_WDPin:
        strcpy(str, "Watchdog_Pin");
		break;
    //EEPRom software write protect function
    case DDevNo_EEPRom_WPF:
        strcpy(str, "SEEPRom_WriteProtect");
		break;
    }
}

/*===========================================================
 * Name  : dyna_getTypeStr
 * Pupose: convert device type id to string
 * Input : NA
 * Output: NA
 *===========================================================*/
void dyna_getTypeStr(eDynclDevType type, char *str)
{
    str[0] = '\0';
    switch (type)
    {
        //case DDevType_Nodefine:
        case DDevType_GPIO:
            strcpy(str, "GPIO");
			break;
        case DDevType_OEMGPIO:
            strcpy(str, "GPIO");
			break;
        case DDevType_ADC:
            strcpy(str, "ADC");
			break;
        case DDevType_DAC:
            strcpy(str, "DAC");
			break;
        case DDevType_TACHO:
            strcpy(str, "TACHO");
			break;
        case DDevType_PWM:
            strcpy(str, "PWM");
			break;
        case DDevType_SMB:
            strcpy(str, "SMB/I2C");
			break;
        case DDevType_Battery:
            strcpy(str, "Battery");
			break;
        case DDevType_IRQNMI:
            strcpy(str, "IRQ/NMI");
			break;
        case DDevType_Func:
            strcpy(str, "Function");
			break;
        default:
            strcpy(str, "Unknown");
			break;
    }
}

/*===========================================================
 * Name  : dyna_getDevType
 * Pupose: use device id to get device type
 * Input : iid-device id
 * Output: device type
 *===========================================================*/
eDynclDevType dyna_getDevType(eDynclDevNo did)
{
    eDynclDevType result = DDevType_Nodefine;
	
    switch (did) {
    //GPIO
    case DDevNo_AltGpio00:
    case DDevNo_AltGpio01:
    case DDevNo_AltGpio02:
    case DDevNo_AltGpio03:
    case DDevNo_AltGpio04:
    case DDevNo_AltGpio05:
    case DDevNo_AltGpio06:
    case DDevNo_AltGpio07:
    case DDevNo_AltGpio08:
    case DDevNo_AltGpio09:
    case DDevNo_AltGpio10:
    case DDevNo_AltGpio11:
    case DDevNo_AltGpio12:
    case DDevNo_AltGpio13:
    case DDevNo_AltGpio14:
    case DDevNo_AltGpio15:
    case DDevNo_Btn0:
    case DDevNo_Btn1:
    case DDevNo_Btn2:
    case DDevNo_Btn3:
    case DDevNo_Btn4:
    case DDevNo_Btn5:
    case DDevNo_Btn6:
    case DDevNo_Btn7:
    case DDevNo_BriOnOff1:
    case DDevNo_BriOnOff2:
    case DDevNo_PowerLed:
    case DDevNo_BatLedG:
    case DDevNo_OemLed0:
    case DDevNo_OemLed1:
    case DDevNo_OemLed2:
    case DDevNo_BatLedR:
        result = DDevType_GPIO;
        break;
    //PWM
    case DDevNo_CPUFAN_2P:
    case DDevNo_CPUFAN_4P:
    case DDevNo_SYSFAN1_2P:
    case DDevNo_SYSFAN1_4P:
    case DDevNo_SYSFAN2_2P:
    case DDevNo_SYSFAN2_4P:
    case DDevNo_PWMBRIGHTNESS:
    case DDevNo_PWMBEEP:
    case DDevNo_PWMBRIGHTNESS2:
        result = DDevType_PWM;
        break;
    //SMBUS
    case DDevNo_SMBOEM0:
    case DDevNo_SMBOEM1:
    case DDevNo_SMBOEM2:
    case DDevNo_SMBEEPROM:
    case DDevNo_SMBTHERMAL0:
    case DDevNo_SMBTHERMAL1:
    case DDevNo_SMBSecurityEEP:
    case DDevNo_I2COEM:
    case DDevNo_SMBEEP2K:
    case DDevNo_OEMEEP:
    case DDevNo_OEMEEP2K:
    case DDevNo_PECI:
    case DDevNo_SMBOEM3:
    case DDevNo_SMLINK:
    case DDevNo_SMBSlv:
    case DDevNo_SMBOEM4:
    case DDevNo_SMBOEM5:
        result = DDevType_SMB;
        break;
    //DAC DEVICE
    case DDevNo_DACSPEAKER:
        result = DDevType_DAC;
        break;
    //smart battery
    case DDevNo_SmartBat1:
    case DDevNo_SmartBat2:
        result = DDevType_Battery;
        break;
    //ADC Device
    case DDevNo_ADCCMOSBAT:
    case DDevNo_ADCCMOSBATx2:
    case DDevNo_ADCCMOSBATx10:
    case DDevNo_ADCBAT:
    case DDevNo_ADCBATx2:
    case DDevNo_ADCBATx10:
    case DDevNo_ADC5VS0:
    case DDevNo_ADC5VS0x2:
    case DDevNo_ADC5VS0x10:
    case DDevNo_ADC5VS5:
    case DDevNo_ADC5VS5x2:
    case DDevNo_ADC5VS5x10:
    case DDevNo_ADC33VS0:
    case DDevNo_ADC33VS0x2:
    case DDevNo_ADC33VS0x10:
    case DDevNo_ADC33VS5:
    case DDevNo_ADC33VS5x2:
    case DDevNo_ADC33VS5x10:
    case DDevNo_ADC12VS0:
    case DDevNo_ADC12VS0x2:
    case DDevNo_ADC12VS0x10:
    case DDevNo_ADCVCOREA:
    case DDevNo_ADCVCOREAx2:
    case DDevNo_ADCVCOREAx10:
    case DDevNo_ADCVCOREB:
    case DDevNo_ADCVCOREBx2:
    case DDevNo_ADCVCOREBx10:
    case DDevNo_ADCDC:
    case DDevNo_ADCDCx2:
    case DDevNo_ADCDCx10:
    case DDevNo_ADCDCSTBY:
    case DDevNo_ADCDCSTBYx2:
    case DDevNo_ADCDCSTBYx10:
    case DDevNo_ADCDCOther:
    case DDevNo_ADCDCOtherx2:
    case DDevNo_ADCDCOtherx10:
    case DDevNo_ADCCurrent:
        result = DDevType_ADC;
        break;
    //watchdog type
    case DDevNo_WDIRQ:
    case DDevNo_WDNMI:
        result = DDevType_IRQNMI;
        break;
    //Fan Tacho
    case DDevNo_CPUFanSpeed0:
    case DDevNo_SYSFanSpeed0:
    case DDevNo_SYSFanSpeed1:
        result = DDevType_TACHO;
        break;
    case DDevNo_Temperature_CPU0:
    case DDevNo_Temperature_CPU1:
    case DDevNo_Temperature_SYS0:
    case DDevNo_Temperature_SYS1:
    case DDevNo_TherPro_ShutDown:
    case DDevNo_TherPro_PwrOff:
    case DDevNo_TherPro_Throttle:
    case DDevNo_WD_WRST:
    case DDevNo_WD_CRST:
    case DDevNo_WD_SCI:
    case DDevNo_WD_IRQNMI:
    case DDevNo_WD_WDPin:
    case DDevNo_EEPRom_WPF:
        result = DDevType_Func;
        break;
    }
    return result;
}

/*===========================================================
 * Name  : dyna_newDevice
 * Pupose: constructor
 * Input : NA
 * Output: NA
 *===========================================================*/
stDyncDev *dyna_newDevice(uint8_t size)
{
	uint8_t i;
	stDyncDev *dev = NULL;
	
	if(size <= 0)
		return NULL;

	dev = (stDyncDev *)malloc(sizeof(stDyncDev) * size);
	if(dev == NULL)
		return NULL;
	
	for(i = 0; i < size; i++)
	{
		dev[i].did		= DDevNo_Nodefine;
		dev[i].pin_no 	= 0xFF;
		dev[i].active 	= 0;
		dev[i].type 	= DDevType_Nodefine;
		dev[i].did 		= '\0';
	}
	return dev;
}

/*===========================================================
 * Name  : dyna_freeDevice
 * Pupose: constructor
 * Input : NA
 * Output: NA
 *===========================================================*/
void dyna_freeDevice(stDyncDev *dev, uint8_t size)
{
	if(size <= 0 || dev == NULL)
		return;
	
	while(size > 0)
	{
		size--;
		free(dev+size);
	}
}


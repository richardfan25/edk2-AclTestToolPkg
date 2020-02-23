/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  MENU_RD.CPP
* Purpose : RD menu
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include <string.h>
#include "menu_rd.h"

/*===========================================================
 * Name   : rdMenu
 * Purpose: rd menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void rdMenu(int key)
{
	int keybyte;
	
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 1;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Select rd function.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "L. Load Default.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "1. List Dynamic Table.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "2. List Thermal Portect.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "3. List Thermal zone.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "4. List Smart Fan.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Please input:");
    ConsoleCurrRow++;

    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    key = bioskey(0); //wait key in
	keybyte = key & 0xFF;
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
        return;
    }
    else if ((keybyte == 'L')||(keybyte == 'l'))    //L l
    {
        menuSelect = _mid_loaddefault;
    }
    else if (keybyte == '1')  //1
    {
        menuSelect = _mid_rd_dyamictbl;
    }
    else if (keybyte == '2')  //2
    {
        menuSelect = _mid_rd_tmlprotbl;
    }
    else if (keybyte == '3')  //3
    {
        menuSelect = _mid_rd_tmesrctbl;
    }
    else if (keybyte == '4')  //4
    {
        menuSelect = _mid_rd_smartfantbl;
    }
}
/*===========================================================
 * Name   : dynamictblMenu
 * Purpose: dynamic table menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void dynamictblMenu(int key)
{
    int idx;
    char typstr[DYNC_TYPE_STR_MAX];
    char strtemp[50];
	
    strtemp[0] = '\0';
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    sprintf(strtemp, "DevID HWID Act       Type Description");
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%s", strtemp);
    gotoxy(40, ConsoleCurrRow);
    fprintf(stderr, "%s", strtemp);
    ConsoleCurrRow++;
    for (idx = 0; idx < mApp.dyna->size; idx++)
    {
        mApp.dyna->getTypeStr(mApp.dyna->dev[idx].type, typstr);
        /*switch (mApp.dyna->dev[idx].type)
        {
            //case DDevType_Nodefine:
            case DDevType_GPIO:
                strcpy(typstr, "GPIO"); break;
            case DDevType_OEMGPIO:
                strcpy(typstr, "GPIO"); break;
            case DDevType_ADC:
                strcpy(typstr, "ADC"); break;
            case DDevType_DAC:
                strcpy(typstr, "DAC"); break;
            case DDevType_TACHO:
                strcpy(typstr, "TACHO"); break;
            case DDevType_PWM:
                strcpy(typstr, "PWM"); break;
            case DDevType_SMB:
                strcpy(typstr, "SMB/I2C"); break;
            case DDevType_Battery:
                strcpy(typstr, "Battery"); break;
            case DDevType_IRQNMI:
                strcpy(typstr, "IRQ/NMI"); break;
            default:
                strcpy(typstr, "Unknown"); break;
        }*/
        sprintf(strtemp, "   %02X   %02X  %02X   %8s %s", 
				mApp.dyna->dev[idx].did, 
				mApp.dyna->dev[idx].pin_no, 
				mApp.dyna->dev[idx].active,
				typstr, 
				mApp.dyna->dev[idx].type_str);
				
        if (idx & 0x01)
        {
            gotoxy(40, ConsoleCurrRow);
            fprintf(stderr, "%s", strtemp);
            ConsoleCurrRow++;
        }
        else
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%s", strtemp);
        }
    }
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
    }
}
/*===========================================================
 * Name   : loaddefaultMenu
 * Purpose: load default menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void loaddefaultMenu(int key)
{
    int result = _MBEr_Success;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Load Default");
    ConsoleCurrRow++;
    ConsoleCurrRow++;
    ConsoleCurrRow++;
    result = mApp.nvcfg->writeControl(NVCFG_CTRL_LOAD_DEF);
    if (result == _MBEr_Success)
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Load Default Successfully.");
        ConsoleCurrRow++;
    }
    else
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Load Default Fail. Code:%02X",result);
        ConsoleCurrRow++;
    }

    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
    }
}
/*===========================================================
 * Name   : tmlProMenu
 * Purpose: display thermal protect menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void tmlProMenu(int key)
{
    int	idx;
    char srcdata[DYNC_TYPE_STR_MAX];
    char typedata[DYNC_TYPE_STR_MAX];
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Thermal protect table:");
    ConsoleCurrRow++;
    mApp.tpro->updateDevice();
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%18s %18s %10s %10s", 
			"Source", "Type", "High limit", "Low Limit");
    ConsoleCurrRow++;
    for (idx = 0; idx < mApp.tpro->dev_cnt; idx++) {
        mApp.tml->getTypeStr(mApp.tpro->dev[idx].src, srcdata);
        mApp.tpro->getTypeStr(mApp.tpro->dev[idx].type, typedata);
        if (mApp.tpro->dev[idx].hi_limit==tmlL1Warn
            || mApp.tpro->dev[idx].hi_limit==tmlL2Warn
            || mApp.tpro->dev[idx].hi_limit==tmlR1Warn
            || mApp.tpro->dev[idx].hi_limit==tmlR2Warn
            )
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%18s %18s %10s %10d.",
					srcdata,
					typedata,
					"BIOS",
					mApp.tpro->dev[idx].lo_limit);
        }
        else
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%18s %18s %10d %10d.",
					srcdata,
					typedata,
					mApp.tpro->dev[idx].hi_limit,
					mApp.tpro->dev[idx].lo_limit);
        }
        ConsoleCurrRow++;
    }
    ConsoleCurrRow++;
    /*for (idx = 0; idx < mApp.tpro->dev_cnt; idx++) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%18x %18x %10d %10d."
                ,mApp.tpro->dev[idx].src
                ,mApp.tpro->dev[idx].type
                ,mApp.tpro->dev[idx].hi_limit
                ,mApp.tpro->dev[idx].lo_limit
                );
        ConsoleCurrRow++;
    }*/
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
    }
}
/*===========================================================
 * Name   : tmlSrcMenu
 * Purpose: display thermal source menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void tmlSrcMenu(int key)
{
    uint8_t idx;
	char srcdata[DYNC_TYPE_STR_MAX];
    char chdata[DYNC_TYPE_STR_MAX];
    char fandata[DYNC_TYPE_STR_MAX];
    
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Thermal zone table:");
    ConsoleCurrRow++;

    mApp.tml->updateDevice();
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%18s %18s %18s",
			"Thermal_Source",
			"Fan_Name",
			"Channel_Name");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%8s %8s %8s %6s %8s %12s",
			"Channel",
			"Address",
			"Command",
			"Status",
			"Fan_Code",
			"Temperature");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "------------------------------------------------");
    ConsoleCurrRow++;
    for (idx = 0; idx < mApp.tpro->dev_cnt; idx++)
    {
        mApp.tml->getTypeStr(mApp.tml->getType(idx), srcdata);
        mApp.dyna->getDevStr(mApp.tml->dev[idx].smb_id, chdata);
        mApp.dyna->getDevStr(mApp.tml->dev[idx].fan_id, fandata);
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%18s %18s %18s",
                srcdata,
                fandata,
                chdata);
        ConsoleCurrRow++;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%8X %8X %8X %6X %8X %12d",
                mApp.tml->dev[idx].smb_ch,
                mApp.tml->dev[idx].addr,
                mApp.tml->dev[idx].cmd,
                mApp.tml->dev[idx].status,
                mApp.tml->dev[idx].fan,
                mApp.tml->dev[idx].temperature);
        ConsoleCurrRow++;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "------------------------------------------------");
        ConsoleCurrRow++;
    }

    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
    }
}
/*===========================================================
 * Name   : smartfantblMenu
 * Purpose: display smart fan menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void smartfantblMenu(int key)
{
    int idx;
    char spstr[DYNC_TYPE_STR_MAX];
    char fanstr[DYNC_TYPE_STR_MAX];
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Smart Fan table:");
    ConsoleCurrRow += 2;

    mApp.fan->updateDevice();
   
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%20s %20s",
            "Fan_Name",
            "Speed_Name");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s",
            "FCode",
            "Sta.",
            "Ctl",
            "Hi_T",
            "Lo_T",
            "Gap_T",
            "HiPWM",
            "LoPWM",
            "HiSpd",
            "LoSpd",
            "Debou",
            "Ctl_T",
            "Ctl_S");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "------------------------------------------------");
    ConsoleCurrRow++;
    for (idx = 0; idx < mApp.fan->dev_cnt; idx++)
    {
        mApp.dyna->getDevStr(mApp.fan->dev[idx].fan_id, fanstr);
        mApp.dyna->getDevStr(mApp.fan->dev[idx].tacho_id, spstr);
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%20s %20s", fanstr, spstr);
        ConsoleCurrRow++;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%5X %5X %5X %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d",
                mApp.fan->dev[idx].code,
                mApp.fan->dev[idx].status,
                mApp.fan->dev[idx].ctrlMode,
                mApp.fan->dev[idx].tempThresH,
                mApp.fan->dev[idx].tempThresL,
                mApp.fan->dev[idx].tempThresGap,
                mApp.fan->dev[idx].PWM_Hi,
                mApp.fan->dev[idx].PWM_Lo,
                mApp.fan->dev[idx].Speed_Hi,
                mApp.fan->dev[idx].Speed_Lo,
                mApp.fan->dev[idx].debounce,
                mApp.fan->dev[idx].tempCtrlMode,
                mApp.fan->dev[idx].speedCtrlMode);
        ConsoleCurrRow++;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "------------------------------------------------");
        ConsoleCurrRow++;
    }
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_main;
    }
}

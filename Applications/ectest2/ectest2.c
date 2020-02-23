/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  ECTEST.CPP
* Purpose : main function
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include <string.h>
#include "ectest2.h"
#include "global.h"
#include "mbx/mbx.h"
#include "mbxapp.h"
#include "_version.h"
#include "t_smb.h"
#include "t_lvds.h"
#include "menu_rd.h"
#include "menu_hwm.h"
#include "menu_au.h"

/*===========================================================
 * Name   : proc_checkNewStuckVersion
 * Purpose: check new stuck kernel version
 * Input  : NA
 * Output : NA
 *===========================================================*/
int proc_checkNewStuckVersion(uint8_t kmajor, uint8_t kminor)
{
    int result = 1;

    if (kmajor < 0x14)
    {
        result = 0;
    }
    else if (kmajor == 0x14)
    {
        if (kminor < 0x08)
        {
            result = 0;
        }
    }
    return result;
}
/*===========================================================
 * Name   : proc_checkVersion
 * Purpose: check support kernel version
 * Input  : NA
 * Output : NA
 *===========================================================*/
int proc_checkVersion(uint8_t kmajor, uint8_t kminor)
{
    int result = 1;

    if (kmajor < SupportMajor)
    {
        result = 0;
    }
    else if (kmajor == SupportMajor)
    {
        if (kminor < SupportMinor)
        {
            result = 0;
        }
    }
    return result;
}
/*===========================================================
 * Name   : proc_printHead
 * Purpose: print header
 * Input  : NA
 * Output : NA
 *===========================================================*/
void proc_printHead(void)
{
    clrscr();
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "%s ver:%c%02X_%02X", project, vertype, major_Ver, minor_Ver);
    ConsoleCurrRow++;
    if (mbox.getAccessType() == MBXCh_ITEMBox)
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Mailbox access type: ITE HW Mailbox. Offset:0x%X", mbox.hw_offset);
        ConsoleCurrRow++;
    }
    else if (mbox.getAccessType() == MBXCh_AdtIO)
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Mailbox access type: Advantech IO channel");
        ConsoleCurrRow++;
    }
    else if (mbox.getAccessType() == MBXCh_ACPIIO)
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Mailbox access type: ACPI EC Space");
        ConsoleCurrRow++;
    }
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Project:%s TableCode:%02X KernelVersion:%02X_%02X",
			project_name, version[0], version[1], version[2]);
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "ICVendor:%c ICCode:%02X ProjectID:%02X PorjectType:%c ProjectVersion:%02X_%02X",
			version[3] ,version[4] ,version[5] ,version[6] ,version[7] ,version[8]);
    ConsoleCurrRow++;
}
/*===========================================================
 * Name   : proc_mainMenu
 * Purpose: main menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void proc_mainMenu(int key)
{
	int keybyte;
	
    proc_printHead();
    kdebug(0, 0);
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Select test function.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "R. RD function.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "1. Hardware Monitor.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "2. Test Thermal Portect - Throttle.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "3. Test Thermal Protect - Shutdown.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    //fprintf(stderr, "4. Test Watchdog - NMI/IRQ");
    //ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "4. Test Watchdog - Cold Reset.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "5. Test Watchdog - Warm Reset");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "6. Test Watchdog - Watchdog pin.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "7. Smart Fan test.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "8. OEM GPIO Test.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "9. LVDS Test.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "A. SMBUS Read/Write.");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "B. Auto Test");
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Please input:");
    ConsoleCurrRow++;

    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to exit.");
	
	keybyte = key & 0xFF;
    //if (key == 0x2D00)    //alt+X
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        menuSelect = _mid_exit;
    }
    else if ((keybyte == 'R') || (keybyte == 'r'))    //R r
    {
        menuSelect = _mid_rd;
    }
    else if (keybyte == '1')  //1
    {
        menuSelect = _mid_hwm;
    }
    else if (keybyte == '2')  //2
    {
        menuSelect = _mid_thmlProThrottle;
    }
    else if (keybyte == '3')  //3
    {
        menuSelect = _mid_thmlProShutdown;
    }
    // else if (keybyte == '4')  //4
    // {
        // menuSelect = _mid_wdIRQNMI;
    // }
    else if (keybyte == '4')  //4
    {
        menuSelect = _mid_wdCldRst;
    }
    else if (keybyte == '5')  //5
    {
        menuSelect = _mid_wdWrmRst;
    }
    else if (keybyte == '6')  //6
    {
        menuSelect = _mid_wdWDPin;
    }
    else if (keybyte == '7')  //7
    {
        menuSelect = _mid_sFanTest;
    }
    else if (keybyte == '8')  //8
    {
        menuSelect = _mid_gpiotest;
    }
    //else if (keybyte == '9')    //9
    //{
    //    menuSelect = _mid_lvdsonoff;
    //}
    else if (keybyte == '9')    //9
    {
        menuSelect = _mid_lvdslvl;
    }
    else if ((keybyte == 'A')||(keybyte == 'a'))    //c C
    {
        menuSelect = _mid_smbreadwrite;
    }
    else if ((keybyte == 'B')||(keybyte == 'b'))    //d D
    {
        menuSelect = _mid_auto;
    }
    else if ((keybyte == 'Z')||(keybyte == 'z'))    //Z z for test code
    {
        menuSelect = _mid_testcode;
    }
}
/*===========================================================
 * Name   : tmlPorThrottleMenu
 * Purpose: thermal protect throttle menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void testCodeMenu(int key)
{
	/*UINT8 cnt = 0;
	
    clrscr();
    kdebug(0, 0);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
		
    while (1)
    {
		if(cnt == 0)
			fprintf(stderr, "Count: %10d\r", getCntPerHalfSec());
		delay(1);
		if(++cnt > 200)
			cnt = 0;
        if((key = bioskey(1)) == 0)
        {
            continue;
        }

        if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
        {
            menuSelect = _mid_main;
            return;
        }
    }*/
	menuSelect = _mid_main;
}
/*===========================================================
 * Name   : main
 * Purpose: main function
 * Input  : NA
 * Output : NA
 *===========================================================*/
void main(void)
{
    int key;
    int lastselect;
	EFI_STATUS Status;

    Status = console_init();
	if(EFI_ERROR(Status))
	{
		fprintf(stderr, "ERROR: Failed to change console setting.\n");
		goto end;
	}
    ConsoleCurrRow = 0;
    //setup mailbox access mbx_ch
	gotoxy(0, ConsoleCurrRow);

	if(mApp.init() != _err_noerror) goto end;

	switch(mbox.getAccessType())
	{
		case MBXCh_ITEMBox:
			fprintf(stderr, "Using ITE HW mailbox.");
			ConsoleCurrRow++;
			break;
		case MBXCh_AdtIO:
			fprintf(stderr, "Using IO mailbox");
			ConsoleCurrRow++;
			break;
		case MBXCh_ACPIIO:
			fprintf(stderr, "Using ACPI mailbox");
			ConsoleCurrRow++;
			break;
		default:
			fprintf(stderr, "Can not access mailbox");
			ConsoleCurrRow++;
			goto end;
	}

    mApp.ec->readFwInfo(project_name,version);
    if (!proc_checkVersion(version[1], version[2]))
    {
        mApp.ec->readFwInfo(project_name, version);
        if (!proc_checkVersion(version[1], version[2]))
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "EC dos not support!!! %02x_%02x", version[1], version[2]);
            ConsoleCurrRow++;
            goto end;
        }
    }
	//check new stuck kernel version
	if(proc_checkNewStuckVersion(version[1], version[2]))
	{
		isNewStuckVer = True;
	}
	else
	{
		isNewStuckVer = False;
	}
	
    proc_printHead();
    mApp.nvcfg->writeControl(NVCFG_CTRL_LOAD_DEF); //load default
    mApp.dyna->getTable();
    key = 0;
    doneFlag = 0;
    menuSelect = _mid_main;
    lastselect = menuSelect;
    proc_mainMenu(key);
    while(1)
    {
        key = bioskey(0); //wait key in
        do
        {
            lastselect = menuSelect;
            if (menuSelect == _mid_main)
            {
                proc_mainMenu(key);
            }
            else if (menuSelect == _mid_rd)
            {
                rdMenu(key);
            }
            else if (menuSelect == _mid_rd_dyamictbl)
            {
                dynamictblMenu(key);
            }
            else if (menuSelect == _mid_loaddefault)
            {
                loaddefaultMenu(key);
            }
            else if (menuSelect == _mid_rd_tmlprotbl)
            {
                tmlProMenu(key);
            }
            else if (menuSelect == _mid_rd_tmesrctbl)
            {
                tmlSrcMenu(key);
            }
            else if (menuSelect == _mid_rd_smartfantbl)
            {
                smartfantblMenu(key);
            }
            else if (menuSelect == _mid_rd_smartfanSim)
            {
                smartfanSimMenu(key);
            }
            else if (menuSelect == _mid_hwm)
            {
                hwmMenu(key);
            }
            else if (menuSelect == _mid_thmlProThrottle)
            {
                tmlPorThrottleMenu(key);
            }
            else if (menuSelect == _mid_thmlProShutdown)
            {
                tmlPorShutdownMenu(key);
            }
            // else if (menuSelect == _mid_wdIRQNMI)
            // {
                // wdIRQNMIMenu(key);
            // }
            else if (menuSelect == _mid_wdCldRst)
            {
                wdCldRstMenu(key);
            }
            else if (menuSelect == _mid_wdWrmRst)
            {
                wdWrmRstMenu(key);
            }
            else if (menuSelect == _mid_wdWDPin)
            {
                wdWDPinMenu(key);
            }
            else if (menuSelect == _mid_sFanTest)
            {
                sFanTestMenu(key);
            }
            else if (menuSelect == _mid_gpiotest)
            {
                gpioTestMenu(key);
            }
            //else if (menuSelect == _mid_lvdsonoff)
            //{
                //lvdsOnOffTestMenu(key);
            //}
            else if (menuSelect == _mid_lvdslvl)
            {
                //lvdsLevelTestMenu(key);
				test_lvds_main();
            }
            else if (menuSelect == _mid_smbreadwrite)
            {
                //smbRWMenu(key);
				test_smb_main();
            }
            else if (menuSelect == _mid_auto)
            {
                autoMenu(key);
            }
            else if (menuSelect == _mid_testcode)
            {
                testCodeMenu(key);
            }
            else if (menuSelect == _mid_exit)
            {
                goto end;
            }
            key = 0;
        }while (lastselect != menuSelect);
    }
	
end:
	mApp.exit();
    clrscr();
	console_exit();
}

/******************************************************************
* Copyright(c) , Advantech Corporation 2014
* Module Name:  MENU_AU.CPP
* Purpose : Auto Test function menu
* Data    : 2014/04/03
* Author  : kk.chien
********************************************************************/
#include <time.h>
#include <string.h>
#include "menu_hwm.h"
#include "menu_au.h"

/*==============================================================*/
char fileName[20]		= "";
char screenBuff[1600] 	= "";
int	autoFlags = 0;
int linecnt = 0;

/*==============================================================*/
int checkFileExist(const char *fname)
{
    FILE *file;
    file = fopen(fname, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}
/*==============================================================*/
char * curTimeStr(void)
{
    static char result[20];
    time_t rawtime;
    struct tm *ptm;
    time(&rawtime);
    ptm = localtime(&rawtime);
    sprintf(result, "%02d/%02d %02d:%02d:%02d",
            ptm->tm_mon+1,
            ptm->tm_mday,
            ptm->tm_hour,
            ptm->tm_min,
            ptm->tm_sec);
    return result;
}
/*==============================================================*/
int addStr2File(const char *fname, const char *istr)
{
    FILE *file;
    file = fopen(fname, "a");
    if (!file) return 0;
    if (fseek(file, 0, SEEK_END)) return 0;
    if (fprintf(file, "%s", istr)<0) return 0;
    if (fclose(file)) return 0;
    return 1;
}
/*==============================================================*/
void printOut(const char *fname, const char *istr)
{
    addStr2File(fname, istr);
    if (strchr(istr,'\n') != NULL)
    {
        if (linecnt > 20)
        {
            clrscr();
            screenBuff[0] = '\0';
            linecnt = 0;
        }
        linecnt++;
    }
    strcat(screenBuff, istr);
    //sprintf(screenBuff, "%s%s", screenBuff,istr);
}
/*===========================================================
 * Name   : autoMenu
 * Purpose: auto test function
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void autoMenu(int key)
{
    int idx;
    unsigned int cnttemp = 0;
    int step = _as_start;
    int itemp;
    time_t rawtime;
    struct tm *ptm;
    char typetemp[DYNC_TYPE_STR_MAX];
    char typetemp2[DYNC_TYPE_STR_MAX];
    char *strtemp = NULL;
    //fan
    uint8_t fanid = 0;
    uint8_t u8tmp[16];
    uint8_t lentemp;
    //gpio
    uint8_t pincnt = 0;
    //eeprom
    uint8_t wbuff;
    uint8_t rdata;
    uint8_t oridata;

	strtemp = (char *)malloc(sizeof(char) * ConsoleWidth);
	if(strtemp == NULL)
	{
		fprintf(stderr, "Failed to init Auto Test Menu.");
		return;
	}
    clrscr();
    kdebug(0, 0);
    while (1)
    {
        ConsoleCurrRow = 0;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Auto Test.");
        ConsoleCurrRow++;
        if (mIsFlag0(autoFlags, fileFlag))
        {
            for (idx = 0; idx < 10; idx++)
            {
                time(&rawtime);
                ptm = localtime(&rawtime);
                sprintf(fileName, "%X%02d%02d%02d%d.log",
                        ptm->tm_mon + 1,
                        ptm->tm_mday,
                        ptm->tm_hour,
                        ptm->tm_min,
                        idx);
                if (checkFileExist(fileName)) continue;
                else break;
            }
            if (idx == 10)
            {
                delay(200);
            }
            else
            {
                SetFlag(autoFlags, fileFlag);
            }
        }
        if (mIsFlag1(autoFlags, startFlag) && mIsFlag1(autoFlags, fileFlag))
        {
            if (step == _as_start)
            {
                step	= _as_fan;
                cnttemp = 0;
                screenBuff[0] = '\0';
                clrscr();
            }
            else if (step == _as_fan)
            {
                if (cnttemp == 1)
                {
                    mApp.fan->updateDevice(); //get fan config
                    if (mApp.fan->dev[fanid].code == 0)
                    {
                        step	= _as_gpio;
                        fanid 	= 0;
                        cnttemp = 0;
                        pincnt 	= 0;
                        continue;
                    }
                    mApp.dyna->getDevStr(mApp.fan->dev[fanid].fan_id, typetemp);
                    sprintf(strtemp, "%s Start test fan :%s\n", curTimeStr(), typetemp);
                    printOut(fileName, strtemp);
                    //turn off fan
                    sprintf(strtemp, "%s Turn Off fan in %d sec....... ",
                            curTimeStr(),
                            fanPollTime*fanOffDelayCnt/1000);
                    printOut(fileName, strtemp);
                    mApp.fan->setCtrlMode(fanid, FCtrl_FanOff);
                    mApp.fan->toArray(fanid, u8tmp, &lentemp);
                    itemp = mApp.fan->writeDevice(
							fanid,
							mApp.fan->dev[fanid].fan_id,
							mApp.fan->dev[fanid].tacho_id,
							u8tmp,
							&lentemp);
                }
                else if (cnttemp == fanOffDelayCnt)
                {
                    if (mApp.dyna->getPinstate(mApp.fan->dev[fanid].tacho_id, u8tmp) == _MBEr_Success)
                    {
                        itemp = u8tmp[0];
                        itemp = ((int)u8tmp[1] | (itemp << 8));
                        if (itemp>spdLowLimit) {
                            sprintf(strtemp, "FAIL!!! Speed:%d\n", itemp);
                            printOut(fileName, strtemp);
                            fanid++;
                            cnttemp = 0;
                        }
                        else
                        {
                            sprintf(strtemp, "PASS!!! Speed:%d\n", itemp);
                            printOut(fileName, strtemp);
                            //turn full fan
                            sprintf(strtemp, "%s Turn full fan in %d sec....... ",
                                    curTimeStr(),
                                    fanPollTime*fabFullDelayCnt/1000);
                            printOut(fileName, strtemp);
                            mApp.fan->setCtrlMode(fanid, FCtrl_FanFull);
                            mApp.fan->toArray(fanid, u8tmp, &lentemp);
                            mApp.fan->writeDevice(
									fanid,
									mApp.fan->dev[fanid].fan_id,
									mApp.fan->dev[fanid].tacho_id,
									u8tmp,
									&lentemp);
                        }
                    }
                    else
                    {
                        printOut(fileName, "FAIL!! Can't get fan speed.\n");
                        fanid++;
                        cnttemp = 0;
                    }
                }
                else if (cnttemp == (fabFullDelayCnt + fanOffDelayCnt))
                {
                    if (mApp.dyna->getPinstate(mApp.fan->dev[fanid].tacho_id, u8tmp) == _MBEr_Success)
                    {
                        itemp = u8tmp[0];
                        itemp = ((int)u8tmp[1] | (itemp << 8));
                        if (itemp<spdHighLimit) {
                            sprintf(strtemp, "FAIL!!! Speed:%d\n", itemp);
                            printOut(fileName, strtemp);
                            fanid++;
                            cnttemp = 0;
                        }
                        else
                        {
                            sprintf(strtemp, "PASS!!! Speed:%d\n", itemp);
                            printOut(fileName, strtemp);
                            fanid++;
                            cnttemp = 0;
                            //fan smart
                            mApp.fan->setCtrlMode(fanid, FCtrl_FanSmart);
                            mApp.fan->toArray(fanid, u8tmp, &lentemp);
                            mApp.fan->writeDevice(
									fanid,
									mApp.fan->dev[fanid].fan_id,
									mApp.fan->dev[fanid].tacho_id,
									u8tmp,
									&lentemp);
                        }
                    }
                    else
                    {
                        printOut(fileName, "FAIL!! Can't get fan speed.\n");
                        fanid++;
                        cnttemp = 0;
                    }
                }
                else
                {
                    if (cnttemp % 5 == 0 && cnttemp != 0)
                    {
                        if (cnttemp < fanOffDelayCnt)
                        {
                            fprintf(stderr, "%02d", cnttemp * fanPollTime / 1000);
                        }
                        else if (cnttemp < (fabFullDelayCnt + fanOffDelayCnt))
                        {
                            fprintf(stderr, "%02d", (cnttemp - fanOffDelayCnt) * fanPollTime / 1000);
                        }
                    }
                }
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "%s", screenBuff);
                delay(fanPollTime);
            }
            else if (step == _as_gpio)
            {
                mApp.dyna->getDevStr(DDevNo_AltGpio00 + pincnt, typetemp);
                mApp.dyna->getDevStr(DDevNo_AltGpio04 + pincnt, typetemp2);
                sprintf(strtemp, "%s Test GPIO %s %s\n", curTimeStr(), typetemp, typetemp2);
                printOut(fileName, strtemp);
                //output high
                sprintf(strtemp, "%s %s output high to %s......", curTimeStr(), typetemp, typetemp2);
                printOut(fileName, strtemp);
                mApp.gpio->setSetting(DDevNo_AltGpio00 + pincnt, OUTPUT);
                mApp.gpio->setSetting(DDevNo_AltGpio04 + pincnt, INPUT);
                u8tmp[0] = 1;
                mApp.dyna->setPinstate(DDevNo_AltGpio00 + pincnt, u8tmp);   //high
                u8tmp[0] = 0;
                mApp.dyna->getPinstate(DDevNo_AltGpio04 + pincnt, u8tmp);
                if (u8tmp[0] != 1)   //fail
                {
                    printOut(fileName, "FAIL!!!!\n");
                }
                else
                {
                    printOut(fileName, "PASS!!!!\n");
                }
                //output low
                sprintf(strtemp, "%s %s output low to %s......", curTimeStr(), typetemp, typetemp2);
                printOut(fileName, strtemp);
                u8tmp[0] = 0;
                mApp.dyna->setPinstate(DDevNo_AltGpio00 + pincnt, u8tmp);   //low
                u8tmp[0] = 1;
                mApp.dyna->getPinstate(DDevNo_AltGpio04 + pincnt, u8tmp);
                if (u8tmp[0] != 0)   //fail
                {
                    printOut(fileName, "FAIL!!!!\n");
                }
                else
                {
                    printOut(fileName, "PASS!!!!\n");
                }
                //reverse output high
                sprintf(strtemp, "%s %s output high to %s......", curTimeStr(), typetemp2, typetemp);
                printOut(fileName, strtemp);
                mApp.gpio->setSetting(DDevNo_AltGpio00 + pincnt, INPUT);
                mApp.gpio->setSetting(DDevNo_AltGpio04 + pincnt, OUTPUT);
                u8tmp[0] = 1;
                mApp.dyna->setPinstate(DDevNo_AltGpio04 + pincnt, u8tmp);   //high
                u8tmp[0] = 0;
                mApp.dyna->getPinstate(DDevNo_AltGpio00 + pincnt, u8tmp);
                if (u8tmp[0] != 1)   //fail
                {
                    printOut(fileName, "FAIL!!!!\n");
                }
                else
                {
                    printOut(fileName, "PASS!!!!\n");
                }
                //reverse output low
                sprintf(strtemp, "%s %s output low to %s......", curTimeStr(), typetemp2, typetemp);
                printOut(fileName, strtemp);
                u8tmp[0] = 0;
                mApp.dyna->setPinstate(DDevNo_AltGpio04 + pincnt, u8tmp);   //low
                u8tmp[0] = 1;
                mApp.dyna->getPinstate(DDevNo_AltGpio00 + pincnt, u8tmp);
                if (u8tmp[0] != 0)   //fail
                {
                    printOut(fileName, "FAIL!!!!\n");
                }
                else
                {
                    printOut(fileName, "PASS!!!!\n");
                }
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "%s", screenBuff);
                pincnt++;
                if (pincnt >= 4)
                {
                    pincnt = 0;
                    step = _as_eeprom;
                    //step = _as_start;
                }
                delay(gpioPollTime);
            }
            else if (step == _as_eeprom)
            {
                sprintf(strtemp, "%s Test EEPRom.\n", curTimeStr());
                printOut(fileName, strtemp);
                if (mApp.dyna->getHw(DDevNo_SMBEEPROM) == DYNC_NULL) {
                    sprintf("%s This project doesn't have eeprom.\n", curTimeStr());
                    printOut(fileName, strtemp);
                    step = _as_start;
                    continue;
                }
                //read original data
                itemp = mApp.smb->request(MBXSMB_ReadByte,	// command
                               DDevNo_SMBEEPROM,            // device id
                               0,                           // slave address, use default
                               0,                           // smbus command, no use in I2C
                               &wbuff,                      // write data buffer
                               &oridata,                    // read data buffer
                               0,                           // write data count
                               0);                          // read data count
                if (itemp != SMB_ST_DONE) {
                    sprintf(strtemp, "%s FAIL!!! Read Smbus byte fail. code : 0x%02X\n", curTimeStr(), itemp);
                    printOut(fileName, strtemp);
                }
                else
                {
                    sprintf(strtemp, "%s PASS!!! Read Smbus byte data : 0x%02X\n", curTimeStr(), oridata);
                    printOut(fileName, strtemp);
                }
                //write data
                wbuff=SMBEEPTestCode;
                itemp = mApp.smb->request(MBXSMB_WriteByte, // command
                               DDevNo_SMBEEPROM,            // device id
                               0,                           // slave address, use default
                               0,                           // smbus command, no use in I2C
                               &wbuff,                      // write data buffer
                               &oridata,                    // read data buffer
                               0,                           // write data count
                               0);                          // read data count
                if (itemp != SMB_ST_DONE) {
                    sprintf(strtemp, "%s FAIL!!! Write Smbus byte fail. code : 0x%02X\n", curTimeStr(), itemp);
                    printOut(fileName, strtemp);
                }
                else
                {
                    sprintf(strtemp, "%s PASS!!! Write Smbus byte success. data : 0x%02X\n", curTimeStr(), wbuff);
                    printOut(fileName, strtemp);
                }
                //read data
                itemp = mApp.smb->request(MBXSMB_ReadByte,	// command
                               DDevNo_SMBEEPROM,            // device id
                               0,                           // slave address, use default
                               0,                           // smbus command, no use in I2C
                               &wbuff,                      // write data buffer
                               &rdata,                      // read data buffer
                               0,                           // write data count
                               0);                          // read data count
                if (itemp != SMB_ST_DONE) {
                    sprintf(strtemp, "%s FAIL!!! Read Smbus byte fail. code : 0x%02X\n", curTimeStr(), itemp);
                    printOut(fileName, strtemp);
                }
                else
                {
                    sprintf(strtemp, "%s PASS!!! Read Smbus byte data : 0x%02X\n", curTimeStr(), rdata);
                    printOut(fileName, strtemp);
                }
                //write data
                wbuff = oridata;
                itemp = mApp.smb->request(MBXSMB_WriteByte,	// command
                               DDevNo_SMBEEPROM,            // device id
                               0,                           // slave address, use default
                               0,                           // smbus command, no use in I2C
                               &wbuff,                      // write data buffer
                               &rdata,                      // read data buffer
                               0,                           // write data count
                               0);                          // read data count
                if (itemp != SMB_ST_DONE) {
                    sprintf(strtemp, "%s FAIL!!! Write Smbus byte fail. code : 0x%02X\n", curTimeStr(), itemp);
                    printOut(fileName, strtemp);
                }
                else
                {
                    sprintf(strtemp, "%s PASS!!! Write Smbus byte success. data : 0x%02X\n", curTimeStr(), wbuff);
                    printOut(fileName, strtemp);
                }
                delay(eepromPollTime);
                step = _as_start;
            }
            cnttemp++;
        }
        else if (mIsFlag1(autoFlags, startFlag) && mIsFlag0(autoFlags, fileFlag))
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Can't create log file.");
            ConsoleCurrRow++;
        }

		if((key = bioskey(1)) == 0)
            key = 0;

        //log file key
        if (mIsFlag1(autoFlags, fileFlag))
        {
            gotoxy(0, ConsoleHeight - 3);
            fprintf(stderr, "Log file: %s", fileName);
        }
        //auto scan key
        if (mIsFlag1(autoFlags, startFlag))
        {
            gotoxy(0, ConsoleHeight - 2);
            fprintf(stderr, "Press 'S' to stop test.");
            if (((key & 0xFF) == 'S')||((key & 0xFF) == 's'))    //S s
            {
                ClrFlag(autoFlags, startFlag);
                //fan smart
                mApp.fan->setCtrlMode(fanid, FCtrl_FanSmart);
                mApp.fan->toArray(fanid, u8tmp, &lentemp);
                mApp.fan->writeDevice(
						fanid,
						mApp.fan->dev[fanid].fan_id,
						mApp.fan->dev[fanid].tacho_id,
						u8tmp,
						&lentemp);
            }
        }
        else
        {
            gotoxy(0, ConsoleHeight - 2);
            fprintf(stderr, "Press 'S' to start test.");
            if (((key & 0xFF) == 'S')||((key & 0xFF) == 's'))    //S s
            {
                SetFlag(autoFlags, startFlag);
                step = _as_start;
                cnttemp = 0;
                fanid = 0;
            }
        }
        gotoxy(0, ConsoleHeight - 1);
        fprintf(stderr, "Press Alt+X to return main menu.");
        if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
        {
            //fan smart
            mApp.fan->setCtrlMode(fanid, FCtrl_FanSmart);
            mApp.fan->toArray(fanid, u8tmp, &lentemp);
            mApp.fan->writeDevice(
					fanid,
					mApp.fan->dev[fanid].fan_id,
					mApp.fan->dev[fanid].tacho_id,
					u8tmp,
					&lentemp);
            menuSelect = _mid_main;
            autoFlags = 0;
            cnttemp = 0;
            break;
        }
    }
	free(strtemp);
}


/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  MENU_HWM.CPP
* Purpose : Test function menu
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <Register/Msr.h>
#include "menu_hwm.h"

int wdFlag = 0;   			//watch event done flag

//md UEFI no support INT start:
// int irqnmiflag = 0;   		//1: irq, 0:nmi
// uint8_t	IRQ;
// uint8_t old_OCW1 = 0;      	//original OCW
// uint8_t vector_offset = 0; 	//interrupt offset
// Func_ISR old_isr = 0;   	//original ISR

// /*===========================================================
 // * Name   : irq_isr
 // * Purpose: IRQ interrupt service
 // * Input  : NA
 // * Output : NA
 // *===========================================================*/
// void interrupt irq_isr(...)
// {
    // ConsoleCurrRow += 2;
	
	// console_color(YELLOW, RED);
    // gotoxy(0, ConsoleCurrRow);
	
    // fprintf(stderr, "IRQ%X OCCUR!!!", IRQ);
	
	// console_color(LIGHTGRAY ,BLACK);
    // SetFlag(wdFlag,BIT(WDTYPE_NMIIRQ));
    // ConsoleCurrRow++;
    // if(IRQ > 7) outp(PIC2_base, EOI);
	// outp(PIC1_base,EOI);
// }
// /*===========================================================
 // * Name   : setIRQISR
 // * Purpose: setup IRQ interrupt service
 // * Input  : iirq-irq number
 // * Output : NA
 // *===========================================================*/
// int setIRQISR(uint8_t iirq)
// {
    // uint8_t value;
    // IRQ = iirq;
    // irqnmiflag = 1;
    // ClrFlag(wdFlag, BIT(WDTYPE_NMIIRQ));
	// if (IRQ > 7)
		// vector_offset = PIC2_vector_offset;
	// else
		// vector_offset = PIC1_vector_offset;
	// //get old handler
	// old_isr = getvect(IRQ + vector_offset);
	// //set new handler
	// setvect(IRQ + vector_offset, irq_isr);
	// //enable interrupt
	// //set_mask();
    // if(IRQ > 7)
	// {   old_OCW1 = inp(PIC2_base + OCW1);
		// value = 255 - (1 << (IRQ - 8));
		// outp(PIC2_base + OCW1, old_OCW1 & value);	  //enable irq2 channel
	// }
	// else
	// {   old_OCW1 = inp(PIC1_base + OCW1);
		// value = 255 - (1 << IRQ);
		// outp(PIC1_base + OCW1, old_OCW1 & value);
	// }
    // return _MBEr_Success;
// }
// /*===========================================================
 // * Name   : resetIRQISR
 // * Purpose: reset IRQ isr
 // * Input  : NA
 // * Output : NA
 // *===========================================================*/
// void resetIRQISR(void)
// {
    // ClrFlag(wdFlag,BIT(WDTYPE_NMIIRQ));
    // if (irqnmiflag) {
        // if(IRQ > 7)
        // {
            // outp(PIC2_base + OCW1, old_OCW1);
        // }
        // else
        // {
            // outp(PIC1_base + OCW1, old_OCW1);
        // }
    // }
    // if (old_isr == 0) return;
    // setvect(IRQ + vector_offset, old_isr);
// }
// /*===========================================================
 // * Name   : nmi_isr
 // * Purpose: NMI interrupt service
 // * Input  : NA
 // * Output : NA
 // *===========================================================*/
// void interrupt nmi_isr(...)
// {
    // ConsoleCurrRow += 2;

    // console_color(YELLOW, RED);
    // gotoxy(0, ConsoleCurrRow);
    // fprintf(stderr, "NMI OCCUR!!!");

    // console_color(LIGHTGRAY, BLACK);
    // SetFlag(wdFlag,BIT(WDTYPE_NMIIRQ));
    // ConsoleCurrRow++;
// }
// /*===========================================================
 // * Name   : setNMIISR
 // * Purpose: setup NMI interrupt service
 // * Input  : NA
 // * Output : NA
 // *===========================================================*/
// int setNMIISR(void)
// {
    // irqnmiflag = 0;
    // ClrFlag(wdFlag, BIT(WDTYPE_NMIIRQ));
	// //get old handler
	// old_isr = getvect(NMIVecOff);
	// //set new handler
	// setvect(NMIVecOff, nmi_isr);
    // return _MBEr_Success;
// }
// /*===========================================================
 // * Name   : wdIRQNMIMenu
 // * Purpose: watchdog IRQ NMI menu
 // * Input  : key-input key
 // * Output : NA
 // *===========================================================*/
// void wdIRQNMIMenu(int key)
// {
    // uint8_t u8tmp;
    // int itemp;
    // int idx;
    // UINTN linetemp;
    // uint16_t wu8tmp[WDTYPE_Size];

    // if(mIsFlag1(doneFlag, wdIRQNMIdone)) goto _wdIRQNMIMenu_end;
    // clrscr();
    // kdebug(0, 0);
    // ConsoleCurrRow = 0;
    // gotoxy(0, ConsoleCurrRow);
    // fprintf(stderr, "Watchdog Test - IRQ NMI");
    // ConsoleCurrRow += 2;
    // if (mApp.dyna->getHw(DDevNo_WDIRQ) != DYNC_NULL) {    //check irq
        // itemp = mApp.ec->readEcRam(WDIRQNumBank, WDIRQNum, 1, &u8tmp);
        // if (itemp != _MBEr_Success) {
            // gotoxy(0, ConsoleCurrRow);
            // fprintf(stderr, "Can't read WDIRQNum. code:%02X", itemp);
            // ConsoleCurrRow++;
            // goto _wdIRQNMIMenu_end;
        // }
        // if (u8tmp == 0 || u8tmp > 15) {
            // gotoxy(0, ConsoleCurrRow);
            // fprintf(stderr, "IRQ number Error. IRQ:%02X", u8tmp);
            // ConsoleCurrRow++;
            // goto _wdIRQNMIMenu_end;
        // }
        // gotoxy(0, ConsoleCurrRow);
        // fprintf(stderr, "Current Watchdog IRQ %02X", u8tmp);
        // ConsoleCurrRow++;
        // //gotoxy(0, ConsoleCurrRow);
        // //fprintf(stderr, "Please Input IRQ Number (2-F)");
        // //ConsoleCurrRow++;
        // //while(1) {
        // //    key = bioskey(0); //wait key in
        // //    key& = 0x00FF;
        // //    if (key> = 0x32 && key< = 0x39) {
        // //        u8tmp=key&0x0F;
        // //        break;
        // //    }
        // //    else if (key == 0x41 || key == 0x61) {  //a A
        // //        u8tmp = 10;
        // //        break;
        // //    }
        // //    else if (key == 0x42 || key == 0x62) {  //b B
        // //        u8tmp = 11;
        // //        break;
        // //    }
        // //    else if (key == 0x43 || key == 0x63) {  //c C
        // //        u8tmp = 12;
        // //        break;
        // //    }
        // //    else if (key == 0x44 || key == 0x64) {  //d D
        // //        u8tmp = 13;
        // //        break;
        // //    }
        // //    else if (key == 0x45 || key == 0x65) {  //e E
        // //        u8tmp = 14;
        // //        break;
        // //    }
        // //    else if (key == 0x46 || key == 0x66) {  //f F
        // //        u8tmp = 15;
        // //        break;
        // //    }
        // //}
        // mApp.ec->writeEcRam(WDIRQNumBank, WDIRQNum, 1, &u8tmp);
        // mApp.ec->readEcRam(WDIRQNumBank, WDIRQNum, 1, &u8tmp);
        // ConsoleCurrRow++;
        // gotoxy(0, ConsoleCurrRow);
        // fprintf(stderr, "Select Watchdog IRQ %02X", u8tmp);
        // ConsoleCurrRow++;
        // //md interrupt no ready:setIRQISR(u8tmp);   //set irq service
    // }
    // else if (mApp.dyna->getHw(DDevNo_WDNMI) != DYNC_NULL) {   //check nmi
        // ;//md interrupt no ready:setNMIISR();
    // }
    // else
    // {
        // gotoxy(0, ConsoleCurrRow);
        // fprintf(stderr, "This project does not support NMI/IRQ for watchdog.");
        // ConsoleCurrRow++;
        // goto _wdIRQNMIMenu_end;
    // }
    // //get watchdog timer
    // for (idx = 0; idx < WDTYPE_Size; idx++) {
        // wu8tmp[idx] = 0xFFFF;
    // }
    // wu8tmp[WDTYPE_NMIIRQ] = WDDelayTime; //delay 50*100ms
    // itemp = mApp.wdt->writeLimit(&u8tmp, wu8tmp);
    // if (itemp != _MBEr_Success) {
        // gotoxy(0, ConsoleCurrRow);
        // fprintf(stderr, "Can not write watchdog timer. code:%02X", itemp);
        // ConsoleCurrRow++;
        // SetFlag(doneFlag, wdIRQNMIdone);
        // goto _wdIRQNMIMenu_end;
    // }
    // gotoxy(0, ConsoleCurrRow);
    // fprintf(stderr, "Watchdog will start in %02d seconde.", WDDelayTime / 10);
    // ConsoleCurrRow++;
    // mApp.wdt->start();    //start watchdog
    // ConsoleCurrRow++;
    // linetemp = ConsoleCurrRow;
    // ConsoleCurrRow += 2;
    // for (idx = 0; idx <= WDDelayTime + 9; idx++) {
        // gotoxy(0, linetemp);
        // fprintf(stderr, "Watchdog counter: %02d second.", idx / 10);
        // delay(100); //100ms
    // }
    // if (mIsFlag0(wdFlag, BIT(WDTYPE_NMIIRQ))) {   //no event
        // console_color(YELLOW, RED);
        // gotoxy(0, ConsoleCurrRow);
        // fprintf(stderr, "Watchdog Fail.");
        // console_color(LIGHTGRAY, BLACK);
        // ConsoleCurrRow++;
    // }

// _wdIRQNMIMenu_end:
    // mApp.wdt->stop();
    // SetFlag(doneFlag, wdIRQNMIdone);
    // gotoxy(0, ConsoleHeight - 1);
    // fprintf(stderr, "Press Alt+X to return main menu.");
    // if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    // {
        // ClrFlag(doneFlag, wdIRQNMIdone);
        // menuSelect = _mid_main;
        // resetIRQISR();
        // return;
    // }
// }
//md UEFI no support INT end

/*===========================================================
 * Name   : hwmMenu
 * Purpose: hardware monitor function
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void hwmMenu(int key)
{
    double	ftemp;
    int 	itemp;
    int 	flagtemp[TMLType_END];
    char 	sdata[DYNC_TYPE_STR_MAX];
    uint8_t	idx;
	uint8_t iu8tmp;
    uint8_t u8tmp[5];
	uint16_t DelayCnt = 0;
	
    clrscr();
    kdebug(0, 0);
	
    while (1)
    {
        if (mIsFlag1(doneFlag, hwmDone))
		{
			key = bioskey(1);
			if(key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
			{
				ClrFlag(doneFlag, hwmDone);
				menuSelect = _mid_main;
				return;
			}

            delay(1);
			if(++DelayCnt < 500)
				continue;
			DelayCnt = 0;
		}
        else
		{
            SetFlag(doneFlag, hwmDone);
			//clrscr();
			gotoxy(0, 0);
			fprintf(stderr, "HardWare Monitor.");
			ConsoleCurrRow = 2;
			////temperature
			gotoxy(0, ConsoleCurrRow);
			fprintf(stderr, "Temperature:");
			ConsoleCurrRow++;
			if(isNewStuckVer)
			{
				for (idx = DDevNo_Temperature_CPU0; idx <= DDevNo_Temperature_SYS1; idx++)
				{
					if (mApp.dyna->getHw(idx) != DYNC_NULL) //exist
					{
						if (mApp.dyna->getPinstate(idx, u8tmp) == _MBEr_Success)
						{
							gotoxy(0, ConsoleCurrRow);
							mApp.dyna->getDevStr(idx, sdata);
							fprintf(stderr, "%25s: ", sdata);
							ConsoleCurrRow++;
						}
					}
				}
			}
			else
			{
				mApp.tml->updateDevice();
				for (idx = 0; idx < TMLType_END; idx++)
				{
					flagtemp[idx] = 0;
				}
				for (idx = 0; idx < mApp.tml->dev_cnt; idx++)
				{
					if (mApp.tml->dev[idx].addr == 0)
						continue;
					itemp = mApp.tml->getType(idx);
					mApp.tml->getTypeStr(itemp, sdata);
					switch (itemp)
					{
					case TMLType_1L:
						if (flagtemp[itemp])
							continue;
						flagtemp[itemp] = 1;//"System0 temperature"
						break;
					case TMLType_1R:
						if (flagtemp[itemp])
							continue;
						flagtemp[itemp] = 1;//"CPU0 temperature"
						break;
					case TMLType_2L:
						if (flagtemp[itemp])
							continue;
						flagtemp[itemp] = 1;//"System1 temperature"
						break;
					case TMLType_2R:
						if (flagtemp[itemp])
							continue;
						flagtemp[itemp] = 1;//"CPU1 temperature"
						break;
					default:
						continue;
					}
					gotoxy(0, ConsoleCurrRow);
					fprintf(stderr, "%25s: ", sdata);
					ConsoleCurrRow++;
				}
			}
			////fan speed
			ConsoleCurrRow++;
			gotoxy(0, ConsoleCurrRow);
			fprintf(stderr, "Fan Speed:");
			ConsoleCurrRow++;
			for (idx = 0; idx < mApp.dyna->size; idx++)
			{
				if (mApp.dyna->dev[idx].type == DDevType_TACHO)
				{
					if (mApp.dyna->getPinstate(mApp.dyna->dev[idx].did, u8tmp) == _MBEr_Success)
					{
						gotoxy(0, ConsoleCurrRow);
						fprintf(stderr, "%25s: ", mApp.dyna->dev[idx].type_str);
						ConsoleCurrRow++;
					}
				}
			}
			//voltage
			gotoxy(0, ConsoleCurrRow++);
			fprintf(stderr, "Voltage:");
			ConsoleCurrRow++;
			for (idx = 0; idx < mApp.dyna->size; idx++)
			{
				if (mApp.dyna->dev[idx].type != DDevType_ADC)
					continue;
				if (mApp.dyna->getPinstate(mApp.dyna->dev[idx].did, u8tmp) != _MBEr_Success)
				{
					continue;
				}
				gotoxy(0, ConsoleCurrRow);
				fprintf(stderr, "%25s: ", mApp.dyna->dev[idx].type_str);
				ConsoleCurrRow++;
			}
			gotoxy(0, ConsoleHeight - 1);
			fprintf(stderr, "Press Alt+X to return main menu.");
		}

		ConsoleCurrRow = 3;
		if(isNewStuckVer)
		{
			for (idx = DDevNo_Temperature_CPU0; idx <= DDevNo_Temperature_SYS1; idx++)
			{
				if (mApp.dyna->getHw(idx) != DYNC_NULL) //exist
				{
					if (mApp.dyna->getPinstate(idx, u8tmp) == _MBEr_Success)
					{
						gotoxy(27, ConsoleCurrRow);
						fprintf(stderr, "%d", u8tmp[0]);
						ConsoleCurrRow++;
					}
				}
			}
		}
		else
		{
			mApp.tml->updateDevice();
			for (idx = 0; idx < TMLType_END; idx++)
			{
				flagtemp[idx] = 0;
			}
			for (idx = 0; idx < mApp.tml->dev_cnt; idx++)
			{
				if (mApp.tml->dev[idx].addr == 0)
					continue;
				itemp = mApp.tml->getType(idx);
				switch (itemp) {
				case TMLType_1L:
					if (flagtemp[itemp]) continue;
					flagtemp[itemp] = 1;//"System0 temperature"
					break;
				case TMLType_1R:
					if (flagtemp[itemp]) continue;
					flagtemp[itemp] = 1;//"CPU0 temperature"
					break;
				case TMLType_2L:
					if (flagtemp[itemp]) continue;
					flagtemp[itemp] = 1;//"System1 temperature"
					break;
				case TMLType_2R:
					if (flagtemp[itemp]) continue;
					flagtemp[itemp] = 1;//"CPU1 temperature"
					break;
				default:
					continue;
				}
				gotoxy(27, ConsoleCurrRow);
				fprintf(stderr, "%3d", mApp.tml->dev[idx].temperature);
				ConsoleCurrRow++;
			}
		}
		////fan speed
		ConsoleCurrRow += 2;
		for (idx = 0; idx < mApp.dyna->size; idx++)
		{
			if (mApp.dyna->dev[idx].type == DDevType_TACHO)
			{
				if (mApp.dyna->getPinstate(mApp.dyna->dev[idx].did, u8tmp) == _MBEr_Success)
				{
					itemp = u8tmp[0];
					itemp = ((int)u8tmp[1] | (itemp << 8));
					gotoxy(27, ConsoleCurrRow);
					fprintf(stderr, "%6d rpm.", itemp);
					ConsoleCurrRow++;
				}
			}
		}
		//voltage
		ConsoleCurrRow += 2;
		for (idx = 0; idx < mApp.dyna->size; idx++)
		{
			if (mApp.dyna->dev[idx].type != DDevType_ADC)
				continue;
			if (mApp.dyna->getPinstate(mApp.dyna->dev[idx].did, u8tmp) != _MBEr_Success)
			{
				gotoxy(27, ConsoleCurrRow);
				fprintf(stderr, "FAIL!!!");
				ConsoleCurrRow++;
				continue;
			}
			iu8tmp = mApp.dyna->dev[idx].did;
			//adc voltage
			itemp = u8tmp[0];
			itemp = ((int)u8tmp[1] | (itemp << 8));
			ftemp = itemp;
			ftemp = ftemp * 3 / 1023;
			if (iu8tmp == DDevNo_ADCCurrent)
			{
				itemp = u8tmp[2];
				itemp = ((int)u8tmp[3] | (itemp << 8));
				ftemp *= itemp;
				itemp = (int)((char)u8tmp[4]);
				ftemp = ftemp * pow(10, (double)itemp);
				gotoxy(27, ConsoleCurrRow);
				fprintf(stderr, "%8.4f A.", ftemp);
				ConsoleCurrRow++;
			}
			else if (iu8tmp <= DDevNo_ADCDCOtherx10 && iu8tmp >= DDevNo_ADCCMOSBAT)
			{
				switch((iu8tmp - DDevNo_ADCCMOSBAT) % 3)
				{
				case 1:
					ftemp *= 2;
					break;
				case 2:
					ftemp *= 10;
					break;
				default:
					break;
				}
				gotoxy(27, ConsoleCurrRow);
				fprintf(stderr, "%8.4f Volt.", ftemp);
				ConsoleCurrRow++;
			}
			else
			{
				gotoxy(27, ConsoleCurrRow);
				fprintf(stderr, "%8.4f Volt.", ftemp);
				ConsoleCurrRow++;
			}
		}

    }
}
/*===========================================================
 * Name   : tmlPorThrottleMenu
 * Purpose: thermal protect throttle menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void tmlPorThrottleMenu(int key)
{
    uint8_t idx;
    uint8_t proNum = 0;
    uint8_t itemp;
    int flagtemp;
    uint8_t u8tmp[4];
	uint16_t DelayCnt = 0;
	MSR_IA32_THERM_STATUS_REGISTER therm_msr;
	
	therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
	therm_msr.Bits.PROCHOT_FORCEPR_Log = 0;
	AsmWriteMsr64(MSR_IA32_THERM_STATUS, therm_msr.Uint64);
			
    clrscr();
    kdebug(0, 0);
    while (1)
    {
        ConsoleCurrRow = 0;
        gotoxy(0, ConsoleCurrRow);
		
        fprintf(stderr, "Test Thermal Protect - Throttle.");
        ConsoleCurrRow++;
		
        if (mIsFlag1(doneFlag, tmlProThdone))
		{
            if((key = bioskey(1)) == 0)
            {
                delay(1);
				if(++DelayCnt < 500)
					continue;
            }
			DelayCnt = 0;
        }
        else
		{
            SetFlag(doneFlag, tmlProThdone);
            key = 0;
            mApp.tpro->updateDevice();
            for (idx = 0; idx < mApp.tpro->dev_cnt; idx++)
			{
                if (mApp.tpro->dev[idx].type == TPType_throttle)
				{
                    proNum = idx;
                    break;
                }
            }
            //
            mApp.tpro->dev[proNum].src	= TMLType_1R;
            mApp.tpro->dev[proNum].type	= TPType_throttle;
            mApp.tpro->dev[proNum].hi_limit = tmlProHiLimit;
            mApp.tpro->dev[proNum].lo_limit = tmlProLoLimit;
            mApp.tpro->toArray(proNum, u8tmp, &itemp);
            mApp.tpro->writeDevice(proNum, u8tmp, &itemp);
            mApp.tpro->updateDevice();
        }
        //CPU temerature
		if(isNewStuckVer)
		{
			if (mApp.dyna->getHw(DDevNo_Temperature_CPU0) != DYNC_NULL) //exist
            {
                if (mApp.dyna->getPinstate(DDevNo_Temperature_CPU0, u8tmp) == _MBEr_Success)
                {
                    gotoxy(0, ConsoleCurrRow);
                    fprintf(stderr, "%70s", " "); //clear line
                    gotoxy(0, ConsoleCurrRow);
                    fprintf(stderr, "%25s: %d", "CPU0 temperature", u8tmp[0]);
                    ConsoleCurrRow++;
                }
            }
		}
		else
		{
			mApp.tml->updateDevice();
			flagtemp = 0;
			for (idx = 0; idx < mApp.tml->dev_cnt; idx++)
			{
				if (mApp.tml->dev[idx].addr == 0)
					continue;
				switch (mApp.tml->getType(idx))
				{
				case TMLType_1R:
					if (flagtemp)
						continue;
					flagtemp = 1;
					gotoxy(0, ConsoleCurrRow);
					fprintf(stderr, "%70s", " ");    //clear line
					gotoxy(0, ConsoleCurrRow);
					fprintf(stderr, "%25s: %d",
							"CPU0 temperature",
							mApp.tml->dev[idx].temperature);
					ConsoleCurrRow++;
					break;
				default:
					break;
				}
			}
		}

        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%25s: %d",
                "High Limit Temprature",
                mApp.tpro->dev[proNum].hi_limit);
				
        ConsoleCurrRow += 2;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Item:%d Protect Temerature. SRC:%02X, Type:%02X, Hi limi:%2d, Lo limit:%2d.",
                proNum,
                mApp.tpro->dev[proNum].src,
                mApp.tpro->dev[proNum].type,
                mApp.tpro->dev[proNum].hi_limit,
                mApp.tpro->dev[proNum].lo_limit);

        therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
        ConsoleCurrRow += 2;
        if (therm_msr.Bits.PROCHOT_FORCEPR_Event)
		{
            ConsoleCurrRow += 2;
            console_color(YELLOW, RED);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr,"Throttle Enable ");
            console_color(LIGHTGRAY, BLACK);
            ConsoleCurrRow++;
        }
        else if (therm_msr.Bits.PROCHOT_FORCEPR_Event == 0 && therm_msr.Bits.PROCHOT_FORCEPR_Log == 1)
        {
            ConsoleCurrRow += 2;
            console_color(YELLOW, RED);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr,"Throttle Disable");
            console_color(LIGHTGRAY, BLACK);
            ConsoleCurrRow++;
        }

        gotoxy(0, ConsoleHeight - 1);
        fprintf(stderr, "Press Alt+X to return main menu.");
        if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
        {
            ClrFlag(doneFlag, tmlProThdone);
			therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
			therm_msr.Bits.PROCHOT_FORCEPR_Log = 0;
			AsmWriteMsr64(MSR_IA32_THERM_STATUS, therm_msr.Uint64);
            menuSelect = _mid_main;
            mApp.nvcfg->writeControl(NVCFG_CTRL_LOAD_DEF); //load default
            return;
        }
    }
}
/*===========================================================
 * Name   : tmlPorShutdownMenu
 * Purpose: thermal protect shutdown menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void tmlPorShutdownMenu(int key)
{
    uint8_t idx;
    uint8_t proNum = 0;
    uint8_t itemp;
    int flagtemp;
    uint8_t u8tmp[4];
	uint16_t DelayCnt = 0;
    clrscr();
    kdebug(0, 0);
    while (1)
    {
        ConsoleCurrRow = 0;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Test Thermal Protect - Shutdown.");
        ConsoleCurrRow++;
        if (mIsFlag1(doneFlag, tmlProShut))
		{
            if((key = bioskey(1)) == 0)
            {
                delay(1);
				if(++DelayCnt < 200)
					continue;
				DelayCnt = 0;
            }
        }
        else
		{
            SetFlag(doneFlag, tmlProShut);
            key = 0;
            mApp.tpro->updateDevice();
            for (idx = 0; idx < mApp.tpro->dev_cnt; idx++)
			{
                if (mApp.tpro->dev[idx].type == TPType_shutDn)
				{
                    proNum = idx;
                    break;
                }
            }
            //itemp = mbox.enable_allram(version[7],version[8]);
            //if (itemp != _MBEr_Success) {
            //    gotoxy(0, ConsoleCurrRow);
            //    fprintf(stderr, "Can't enable all ec ram access. code:%02X", itemp);
            //    goto _tmlPorShutdownMenu_end;
            //}
            //u8tmp[0] = hwWDRstEC;
            //itemp = mApp.ec->writeEcRam(flashMarkBank,flashMark, 1, u8tmp);
            //if (itemp != _MBEr_Success) {
            //    gotoxy(0, ConsoleCurrRow);
            //    fprintf(stderr, "Can't write flash mark. code:%02X", itemp);
            //    ConsoleCurrRow++;
            //    goto _tmlPorShutdownMenu_end;
            //}
            //
            mApp.tpro->dev[proNum].src	= TMLType_1R;
            mApp.tpro->dev[proNum].type	= TPType_shutDn;
            mApp.tpro->dev[proNum].hi_limit = tmlProHiLimit;
            mApp.tpro->dev[proNum].lo_limit = tmlProLoLimit;
            mApp.tpro->toArray(proNum, u8tmp, &itemp);
            mApp.tpro->writeDevice(proNum, u8tmp, &itemp);
            mApp.tpro->updateDevice();
        }
        //CPU temerature
		if(isNewStuckVer)
		{
			if (mApp.dyna->getHw(DDevNo_Temperature_CPU0) != DYNC_NULL) //exist
            {
                if (mApp.dyna->getPinstate(DDevNo_Temperature_CPU0, u8tmp) == _MBEr_Success)
                {
                    gotoxy(0, ConsoleCurrRow);
                    fprintf(stderr, "%70s", " "); //clear line
                    gotoxy(0, ConsoleCurrRow);
                    fprintf(stderr, "%25s: %d", "CPU0 temperature", u8tmp[0]);
                    ConsoleCurrRow++;
                }
            }
		}
		else
		{
			mApp.tml->updateDevice();
			flagtemp = 0;
			for (idx = 0; idx < mApp.tml->dev_cnt; idx++)
			{
				if (mApp.tml->dev[idx].addr == 0)
					continue;
				itemp = mApp.tml->getType(idx);
				gotoxy(0, ConsoleCurrRow);
				fprintf(stderr, "%70s", " ");    //clear line
				switch (itemp)
				{
				case TMLType_1R:
					if (flagtemp)
						continue;
					flagtemp = 1;
					gotoxy(0, ConsoleCurrRow);
					fprintf(stderr, "%25s: %d",
							"CPU0 temperature",
							mApp.tml->dev[idx].temperature);
					ConsoleCurrRow++;
					break;
				default:
					break;
				}
			}
		}
        //set thermal protect
        //mApp.tpro->updateDevice();
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%25s: %d",
                "High Limit Temprature",
                mApp.tpro->dev[proNum].hi_limit);
        ConsoleCurrRow += 2;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Item:%d Protect Temerature. SRC:%02X, Type:%02X, Hi limi:%2d, Lo limit:%2d.",
                proNum,
                mApp.tpro->dev[proNum].src,
                mApp.tpro->dev[proNum].type,
                mApp.tpro->dev[proNum].hi_limit,
                mApp.tpro->dev[proNum].lo_limit);
        ConsoleCurrRow += 2;

        gotoxy(0, ConsoleHeight - 1);
        fprintf(stderr, "Press Alt+X to return main menu.");
        if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
        {
            ClrFlag(doneFlag, tmlProShut);
            menuSelect = _mid_main;
            return;
        }
    }
}

/*===========================================================
 * Name   : wdCldRstMenu
 * Purpose: watchdog cold reset menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void wdCldRstMenu(int key)
{
    uint8_t u8tmp;
    int itemp;
    int idx;
    UINTN linetemp;
    uint16_t wu8tmp[WDTYPE_Size];

    if(mIsFlag1(doneFlag, wdIRQCldRstDone))
		goto _wdCldRstMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog Test - Cold Reset.");
    ConsoleCurrRow += 2;
    //get watchdog timer
    for (idx = 0; idx < WDTYPE_Size; idx++)
	{
        wu8tmp[idx] = 0xFFFF;
    }
    wu8tmp[WDTYPE_CldRst] = WDDelayTime; //delay 50*100ms
    itemp = mApp.wdt->writeLimit(&u8tmp, wu8tmp);
    if (itemp != _MBEr_Success)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Can not write watchdog timer. code:%02X", itemp);
        ConsoleCurrRow++;
        SetFlag(doneFlag, wdIRQNMIdone);
        goto _wdCldRstMenu_end;
    }
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog will start in %02d seconde.", WDDelayTime / 10);
    ConsoleCurrRow++;
    mApp.wdt->start();    //start watchdog
    ConsoleCurrRow++;
    linetemp = ConsoleCurrRow;
    ConsoleCurrRow += 2;
    for (idx = 0; idx <= WDDelayTime + 9; idx++)
	{
        gotoxy(0, linetemp);
        fprintf(stderr, "Watchdog counter: %02d second.", idx / 10);
        delay(100); //100ms
    }
    
    console_color(YELLOW, RED);
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog Fail.");
    
    console_color(LIGHTGRAY, BLACK);
    ConsoleCurrRow++;

_wdCldRstMenu_end:
    SetFlag(doneFlag, wdIRQCldRstDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        mApp.wdt->stop();
        ClrFlag(doneFlag, wdIRQCldRstDone);
        menuSelect = _mid_main;
        return;
    }
}
/*===========================================================
 * Name   : wdWrmRstMenu
 * Purpose: watchdog warm reset menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void wdWrmRstMenu(int key)
{
    uint8_t u8tmp;
    int itemp;
    int idx;
    UINTN linetemp;
    uint16_t wu8tmp[WDTYPE_Size];

    if(mIsFlag1(doneFlag, wdIRQWrmRstDone))
		goto _wdWrmRstMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog Test - Warm Reset.");
    ConsoleCurrRow += 2;
    //get watchdog timer
    for (idx = 0; idx < WDTYPE_Size; idx++)
	{
        wu8tmp[idx] = 0xFFFF;
    }
    wu8tmp[WDTYPE_WrmRst] = WDDelayTime; //delay 50*100ms
    itemp = mApp.wdt->writeLimit(&u8tmp, wu8tmp);
    if (itemp != _MBEr_Success)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Can not write watchdog timer. code:%02X", itemp);
        ConsoleCurrRow++;
        SetFlag(doneFlag, wdIRQNMIdone);
        goto _wdWrmRstMenu_end;
    }
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog will start in %02d seconde.", WDDelayTime / 10);
    ConsoleCurrRow++;
    mApp.wdt->start();    //start watchdog
    ConsoleCurrRow++;
    linetemp = ConsoleCurrRow;
    ConsoleCurrRow += 2;
    for (idx = 0; idx <= WDDelayTime+9; idx++)
	{
        gotoxy(0, linetemp);
        fprintf(stderr, "Watchdog counter: %02d second.", idx / 10);
        delay(100); //100ms
    }
    
    console_color(YELLOW, RED);
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog Fail.");
    
    console_color(LIGHTGRAY, BLACK);
    ConsoleCurrRow++;

_wdWrmRstMenu_end:
    SetFlag(doneFlag, wdIRQWrmRstDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        mApp.wdt->stop();
        ClrFlag(doneFlag, wdIRQWrmRstDone);
        menuSelect = _mid_main;
        return;
    }
}
/*===========================================================
 * Name   : wdWDPinMenu
 * Purpose: watchdog pin menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void wdWDPinMenu(int key)
{
    uint8_t u8tmp;
    int itemp;
    int idx;
    UINTN linetemp;
    uint16_t wu8tmp[WDTYPE_Size];

    if(mIsFlag1(doneFlag, wdWDPinDone))
		goto _wdWDPinMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog Test - Watchdog pin.");
    ConsoleCurrRow += 2;
    //get watchdog timer
    for (idx = 0; idx < WDTYPE_Size; idx++)
	{
        wu8tmp[idx] = 0xFFFF;
    }
    wu8tmp[WDTYPE_WDPin] = WDDelayTime; //delay 50*100ms
    itemp = mApp.wdt->writeLimit(&u8tmp,wu8tmp);
    if (itemp != _MBEr_Success)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Can not write watchdog timer. code:%02X", itemp);
        ConsoleCurrRow++;
        SetFlag(doneFlag, wdWDPinDone);
        goto _wdWDPinMenu_end;
    }
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Watchdog will start in %02d seconde.", WDDelayTime / 10);
    ConsoleCurrRow++;
    
    console_color(YELLOW, RED);
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Please use oscilloscope to check watch dog pin high pulse.");
    
    console_color(LIGHTGRAY, BLACK);
    ConsoleCurrRow++;
    mApp.wdt->start();    //start watchdog
    ConsoleCurrRow++;
    linetemp = ConsoleCurrRow;
    ConsoleCurrRow += 2;
    for (idx = 0; idx <= WDDelayTime + 9; idx++)
	{
        gotoxy(0, linetemp);
        fprintf(stderr, "Watchdog counter: %02d second.", idx / 10);
        delay(100); //100ms
    }

_wdWDPinMenu_end:
    SetFlag(doneFlag, wdWDPinDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        mApp.wdt->stop();
        ClrFlag(doneFlag, wdWDPinDone);
        menuSelect = _mid_main;
        return;
    }
}
/*===========================================================
 * Name   : sFanTestMenu
 * Purpose: smart fan test menu
 * Input  : key-input key
 * Output : NA
 *===========================================================*/
void sFanTestMenu(int key)
{
    uint8_t idx;
    uint8_t jcount;
    uint8_t lentemp;
    uint8_t u8tmp[16];
    uint16_t itemp;
    UINTN	 ltemp;
	int rtemp;
    char fanstr[DYNC_TYPE_STR_MAX];
	
    if (mIsFlag1(doneFlag, sfanDone))
		goto _sFanTestMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Smart Fan Test.");
    ConsoleCurrRow += 2;

    mApp.fan->updateDevice();
    for (idx = 0; idx < mApp.fan->dev_cnt; idx++)
	{
		if(isNewStuckVer)
		{
			if (mApp.fan->dev[idx].fan_id == 0)
				continue;
			mApp.dyna->getDevStr(mApp.fan->dev[idx].fan_id, fanstr);
		}
		else
		{
			if (mApp.fan->dev[idx].code == 0)
				continue;
			mApp.dyna->getDevStr(mApp.fan->dev[idx].fan_id, fanstr);
		}
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%s:", fanstr);
        ConsoleCurrRow++;
        //fan off
        ltemp = ConsoleCurrRow;
        ConsoleCurrRow++;
        gotoxy(5, ltemp);
        fprintf(stderr, "%16s: wait %02d sec.", "Turn fan off", fanoffdelay);
		if(isNewStuckVer)
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanOff);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].speedDviceID,
					u8tmp,
					&lentemp);
		}
		else
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanOff);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].tacho_id,
					u8tmp,
					&lentemp);
		}
        for (jcount = 0; jcount < fanoffdelay; jcount++)
		{
            gotoxy(28, ltemp);
            fprintf(stderr, "%02d", fanoffdelay - jcount);
            delay(1000);
        }
		if(isNewStuckVer)
		{
			rtemp = mApp.dyna->getPinstate(mApp.fan->dev[idx].speedDviceID, u8tmp);
		}
		else
		{
			rtemp = mApp.dyna->getPinstate(mApp.fan->dev[idx].tacho_id, u8tmp);
		}
        if (rtemp == _MBEr_Success)
        {
            itemp = ((int)u8tmp[1] | ((int)u8tmp[0] << 8));
            if (itemp>spdLowLimit)
			{
                
                console_color(GREEN, RED);
                gotoxy(22, ltemp);
                fprintf(stderr, "FAIL!!! Speed:%d", itemp);
            }
            else
            {
                
                console_color(YELLOW, RED);
                gotoxy(22, ltemp);
                fprintf(stderr, "PASS!!! Speed:%d", itemp);
            }
            
            console_color(LIGHTGRAY, BLACK);
        }
        else
        {
            
            console_color(GREEN, RED);
            gotoxy(22, ltemp);
            fprintf(stderr, "FAIL!!! Can't get speed.");
            
            console_color(LIGHTGRAY, BLACK);
        }
        //fan full
        ltemp = ConsoleCurrRow;
        ConsoleCurrRow++;
        gotoxy(5, ltemp);
        fprintf(stderr, "%16s: wait %02d sec.", "Turn fan full", fanondelay);
		if(isNewStuckVer)
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanFull);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].speedDviceID,
					u8tmp,
					&lentemp);
		}
		else
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanFull);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].tacho_id,
					u8tmp,
					&lentemp);
		}
        for (jcount = 0; jcount < fanondelay; jcount++)
		{
            gotoxy(28, ltemp);
            fprintf(stderr, "%02d", (int)fanondelay - jcount);
            delay(1000);
        }
		if(isNewStuckVer)
		{
			rtemp = mApp.dyna->getPinstate(mApp.fan->dev[idx].speedDviceID, u8tmp);
		}
		else
		{
			rtemp = mApp.dyna->getPinstate(mApp.fan->dev[idx].tacho_id, u8tmp);
		}
        if (rtemp == _MBEr_Success)
        {
            itemp = ((int)u8tmp[1] | ((int)u8tmp[0] << 8));
            if (itemp<spdHighLimit)
			{
                
                console_color(GREEN, RED);
                gotoxy(22, ltemp);
                fprintf(stderr, "FAIL!!! Speed:%d", itemp);
            }
            else
            {
                
                console_color(YELLOW, RED);
                gotoxy(22, ltemp);
                fprintf(stderr, "PASS!!! Speed:%d", itemp);
            }
            
            console_color(LIGHTGRAY, BLACK);
        }
        else
        {
            
            console_color(GREEN, RED);
            gotoxy(22, ltemp);
            fprintf(stderr, "FAIL!!! Can't get speed.");
            
            console_color(LIGHTGRAY, BLACK);
        }
        //fan smart
		if(isNewStuckVer)
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanSmart);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].speedDviceID,
					u8tmp,
					&lentemp);
		}
		else
		{
			mApp.fan->setCtrlMode(idx, FCtrl_FanSmart);
			mApp.fan->toArray(idx, u8tmp, &lentemp);
			mApp.fan->writeDevice(
					idx,
					mApp.fan->dev[idx].fan_id,
					mApp.fan->dev[idx].tacho_id,
					u8tmp,
					&lentemp);
		}
        //
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "------------------------------------------------");
        ConsoleCurrRow++;
    }
_sFanTestMenu_end:
    SetFlag(doneFlag, sfanDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        ClrFlag(doneFlag, sfanDone);
        menuSelect = _mid_main;
        return;
    }
}
void gpioTestMenu(int key)
{
    uint8_t idx;
    uint8_t u8tmp;
    UINTN ltemp;
    int check;
    char io1str[DYNC_TYPE_STR_MAX];
    char io2str[DYNC_TYPE_STR_MAX];
	
    if (mIsFlag1(doneFlag, gpioDone))
		goto _gpioTestMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "GPIO Test.");
    ConsoleCurrRow += 2;
    //
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Please connect gpio as follow.");
    ConsoleCurrRow++;
    
    console_color(YELLOW, RED);
    gotoxy(00, ConsoleCurrRow);
    fprintf(stderr, "GPIO0  <-------->  GPIO4");
    ConsoleCurrRow++;
    gotoxy(00, ConsoleCurrRow);
    fprintf(stderr, "GPIO1  <-------->  GPIO5");
    ConsoleCurrRow++;
    gotoxy(00, ConsoleCurrRow);
    fprintf(stderr, "GPIO2  <-------->  GPIO6");
    ConsoleCurrRow++;
    gotoxy(00, ConsoleCurrRow);
    fprintf(stderr, "GPIO3  <-------->  GPIO7");
    ConsoleCurrRow++;
    
    console_color(LIGHTGRAY, BLACK);
    //
    ConsoleCurrRow++;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "Press any key to start test...");
    ConsoleCurrRow += 2;
    key = bioskey(0); //wait key in
    //
    for (idx = 0; idx < 4; idx++)
	{
        //
        check = 1;
        ltemp = ConsoleCurrRow;
        ConsoleCurrRow++;
        gotoxy(0, ltemp);
        mApp.dyna->getDevStr(DDevNo_AltGpio00 + idx, io1str);
        mApp.dyna->getDevStr(DDevNo_AltGpio04 + idx, io2str);
        fprintf(stderr, "%18s ----> %-18s:",io1str,io2str);
        mApp.gpio->setSetting(DDevNo_AltGpio00 + idx, OUTPUT);
        mApp.gpio->setSetting(DDevNo_AltGpio04 + idx, INPUT);
        u8tmp = 1;
        mApp.dyna->setPinstate(DDevNo_AltGpio00 + idx, &u8tmp);   //high
        u8tmp = 0;
        mApp.dyna->getPinstate(DDevNo_AltGpio04 + idx, &u8tmp);
        if (u8tmp != 1)
			check = 0;//fail
        u8tmp = 0;
        mApp.dyna->setPinstate(DDevNo_AltGpio00 + idx, &u8tmp);   //low
        u8tmp = 1;
        mApp.dyna->getPinstate(DDevNo_AltGpio04 + idx, &u8tmp);
        if (u8tmp != 0)
			check = 0;//fail
        gotoxy(44, ltemp);
        if (check)
		{
            
            console_color(YELLOW, RED);
            fprintf(stderr, "PASS.");
        }
        else
        {
            
            console_color(GREEN, RED);
            fprintf(stderr, "FAIL!!!");
        }
        
        console_color(LIGHTGRAY, BLACK);
        //
        check = 1;
        ltemp = ConsoleCurrRow;
        ConsoleCurrRow++;
        gotoxy(0, ltemp);
        mApp.dyna->getDevStr(DDevNo_AltGpio00 + idx, io1str);
        mApp.dyna->getDevStr(DDevNo_AltGpio04 + idx, io2str);
        fprintf(stderr, "%18s <---- %-18s:",io1str,io2str);
        mApp.gpio->setSetting(DDevNo_AltGpio00 + idx, INPUT);
        mApp.gpio->setSetting(DDevNo_AltGpio04 + idx, OUTPUT);
        u8tmp = 1;
        mApp.dyna->setPinstate(DDevNo_AltGpio04 + idx, &u8tmp);   //high
        u8tmp = 0;
        mApp.dyna->getPinstate(DDevNo_AltGpio00 + idx, &u8tmp);
        if (u8tmp != 1)
			check = 0;//fail
        u8tmp = 0;
        mApp.dyna->setPinstate(DDevNo_AltGpio04 + idx, &u8tmp);   //low
        u8tmp = 1;
        mApp.dyna->getPinstate(DDevNo_AltGpio00 + idx, &u8tmp);
        if (u8tmp != 0)
			check = 0;//fail
        gotoxy(44, ltemp);
        if (check)
		{
            
            console_color(YELLOW, RED);
            fprintf(stderr, "PASS.");
        }
        else
        {
            
            console_color(GREEN, RED);
            fprintf(stderr, "FAIL!!!");
        }
        
        console_color(LIGHTGRAY, BLACK);
    }

_gpioTestMenu_end:
    SetFlag(doneFlag, gpioDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        ClrFlag(doneFlag, gpioDone);
        menuSelect = _mid_main;
        return;
    }
}
void lvdsOnOffTestMenu(int key)
{
    int itemp;
    uint8_t lvdsTemp;
    uint8_t oriTemp;
	
    if (mIsFlag1(doneFlag, lvdsOnOffDone))
		goto _lvdsOnOffTestMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "LVDS On/Off Test.");
    ConsoleCurrRow += 2;
    //backlight 1
    if (mApp.dyna->getHw(DDevNo_PWMBRIGHTNESS) != DYNC_NULL)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "LVDS 1 On/Off Test");
        ConsoleCurrRow += 2;
        itemp = mApp.ec->readEcRam(ACPIBank, LVDS1Off, 1, &oriTemp);
        if (itemp != _MBEr_Success)
		{
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Can't read LVDS1 ec ram. code:%02X", itemp);
            ConsoleCurrRow++;
        }
        else
        {
            lvdsTemp = oriTemp;
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "After 1 Second, program will turn off LVDS for 5 second.");
            ConsoleCurrRow++;
            delay(1000);
            ClrFlag(lvdsTemp, OnOffBit);
            mApp.ec->writeEcRam(ACPIBank, LVDS1Off, 1, &lvdsTemp);
            delay(5000);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will turn on LVDS for 5 second.");
            ConsoleCurrRow++;
            SetFlag(lvdsTemp, OnOffBit);
            mApp.ec->writeEcRam(ACPIBank, LVDS1Off, 1, &lvdsTemp);
            delay(5000);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will restore to original on/off.");
            ConsoleCurrRow += 2;
            mApp.ec->writeEcRam(ACPIBank, LVDS1Off, 1, &oriTemp);
        }
    }
    //backlight 2
    if (mApp.dyna->getHw(DDevNo_PWMBRIGHTNESS2) != DYNC_NULL)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "LVDS 2 On/Off Test");
        ConsoleCurrRow += 2;
        itemp = mApp.ec->readEcRam(ACPIBank, LVDS2Off, 1, &oriTemp);
        if (itemp != _MBEr_Success)
		{
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Can't read LVDS2 ec ram. code:%02X", itemp);
            ConsoleCurrRow++;
        }
        else
        {
            lvdsTemp = oriTemp;
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "After 1 Second, program will turn off LVDS for 5 second.");
            ConsoleCurrRow++;
            delay(1000);
            ClrFlag(lvdsTemp,OnOffBit);
            mApp.ec->writeEcRam(ACPIBank, LVDS2Off, 1, &lvdsTemp);
            delay(5000);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will turn on LVDS for 5 second.");
            ConsoleCurrRow++;
            SetFlag(lvdsTemp,OnOffBit);
            mApp.ec->writeEcRam(ACPIBank, LVDS2Off, 1, &lvdsTemp);
            delay(5000);
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will restore to original on/off.");
            ConsoleCurrRow += 2;
            mApp.ec->writeEcRam(ACPIBank, LVDS2Off, 1, &oriTemp);
        }
    }
_lvdsOnOffTestMenu_end:
    SetFlag(doneFlag, lvdsOnOffDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        ClrFlag(doneFlag, lvdsOnOffDone);
        menuSelect = _mid_main;
        return;
    }
}
void lvdsLevelTestMenu(int key)
{
    uint8_t idx;
    int itemp;
    uint8_t lvdsTemp;
    uint8_t oriTemp;

    if (mIsFlag1(doneFlag, lvdsLvlDone))
		goto _lvdsLevelTestMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "LVDS Level Test.");
    ConsoleCurrRow += 2;
    //backlight 1
    if (mApp.dyna->getHw(DDevNo_PWMBRIGHTNESS) != DYNC_NULL)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "LVDS 1 Level Test");
        ConsoleCurrRow += 2;
        itemp = mApp.ec->readEcRam(ACPIBank, LVDS1Off, 1, &oriTemp);
        if (itemp != _MBEr_Success)
		{
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Can't read LVDS1 ec ram. code:%02X", itemp);
            ConsoleCurrRow++;
        }
        else
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Increase level every one seceond.");
            ConsoleCurrRow++;
            lvdsTemp = oriTemp;            
            for (idx = 0; idx < LVDSLvlMax; idx++) {
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "Set LVDS Level : %d",idx);
                lvdsTemp = (oriTemp&OnOffBit) | idx;
                mApp.ec->writeEcRam(ACPIBank, LVDS1Off, 1, &lvdsTemp);
                delay(1000);
            }
            ConsoleCurrRow++;
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will restore to original level.");
            ConsoleCurrRow += 2;
            mApp.ec->writeEcRam(ACPIBank, LVDS1Off, 1, &oriTemp);
        }
    }
    //backlight 2
    if (mApp.dyna->getHw(DDevNo_PWMBRIGHTNESS2) != DYNC_NULL)
	{
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "LVDS 2 Level Test");
        ConsoleCurrRow += 2;
        itemp = mApp.ec->readEcRam(ACPIBank, LVDS2Off, 1, &oriTemp);
        if (itemp != _MBEr_Success)
		{
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Can't read LVDS2 ec ram. code:%02X", itemp);
            ConsoleCurrRow++;
        }
        else
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Increase level every one seceond.");
            ConsoleCurrRow++;
            lvdsTemp = oriTemp;            
            for (idx = 0; idx < LVDSLvlMax; idx++)
			{
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "Set LVDS Level : %d",idx);
                lvdsTemp = (oriTemp&OnOffBit) | idx;
                mApp.ec->writeEcRam(ACPIBank, LVDS2Off, 1, &lvdsTemp);
                delay(1000);
            }
            ConsoleCurrRow++;
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Program will restore to original level.");
            ConsoleCurrRow += 2;
            mApp.ec->writeEcRam(ACPIBank, LVDS2Off, 1, &oriTemp);
        }
    }
_lvdsLevelTestMenu_end:
    SetFlag(doneFlag, lvdsLvlDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        ClrFlag(doneFlag, lvdsLvlDone);
        menuSelect = _mid_main;
        return;
    }
}
/*void eepromTestMenu(int key)
{
    int itemp;
    uint8_t wbuff;
    uint8_t rdata;
    uint8_t oridata;
    if (mIsFlag1(doneFlag, eepromDone)) goto _eepromTestMenu_end;
    clrscr();
    kdebug(0, 0);
    ConsoleCurrRow = 0;
    gotoxy(0, ConsoleCurrRow);
    fprintf(stderr, "EEPRom Test.");
    ConsoleCurrRow += 2;
    //
    if (mApp.dyna->getHw(DDevNo_SMBEEPROM) == DYNC_NULL) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! This project doesn't have eeprom.");
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    itemp = mApp.smb->request(MBXSMB_ReadByte          // command
                   ,DDevNo_SMBEEPROM              // device id
                   , 0                           // slave address, use default
                   , 0                           // smbus command, no use in I2C
                   , &wbuff                      // write data buffer
                   , &oridata                    // read data buffer
                   , 0                           // write data count
                   , 0);                         // read data count
    if (itemp != SMB_ST_DONE) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! Read Smbus byte fail. code : 0x%02X", itemp);
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    else
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Success!!! Read Smbus byte success. data : 0x%02X", oridata);
        ConsoleCurrRow++;
    }
    //
    wbuff=SMBEEPTestCode;
    itemp = mApp.smb->request(MBXSMB_WriteByte          // command
                   ,DDevNo_SMBEEPROM              // device id
                   , 0                           // slave address, use default
                   , 0                           // smbus command, no use in I2C
                   , &wbuff                      // write data buffer
                   , &oridata                    // read data buffer
                   , 0                           // write data count
                   , 0);                         // read data count
    if (itemp != SMB_ST_DONE) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! Write Smbus byte fail. code : 0x%02X", itemp);
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    else
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Success!!! Write Smbus byte success. data : 0x%02X", wbuff);
        ConsoleCurrRow++;
    }
    //
    itemp = mApp.smb->request(MBXSMB_ReadByte          // command
                   ,DDevNo_SMBEEPROM              // device id
                   , 0                           // slave address, use default
                   , 0                           // smbus command, no use in I2C
                   , &wbuff                      // write data buffer
                   , &rdata                      // read data buffer
                   , 0                           // write data count
                   , 0);                         // read data count
    if (itemp != SMB_ST_DONE) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! Read Smbus byte fail. code : 0x%02X", itemp);
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    if (rdata!=SMBEEPTestCode) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! Fail to write data into EEPRom. Data: 0x%02X Correct: 0x%02X", rdata, SMBEEPTestCode);
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    else
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Success!!! Compare byte success. data : 0x%02X", rdata);
        ConsoleCurrRow++;
    }
    //
    wbuff = oridata;
    itemp = mApp.smb->request(MBXSMB_WriteByte          // command
                   ,DDevNo_SMBEEPROM              // device id
                   , 0                           // slave address, use default
                   , 0                           // smbus command, no use in I2C
                   , &wbuff                      // write data buffer
                   , &rdata                    // read data buffer
                   , 0                           // write data count
                   , 0);                         // read data count
    if (itemp != SMB_ST_DONE) {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Error!!! Write Smbus byte fail. code : 0x%02X", itemp);
        ConsoleCurrRow++;
        goto _eepromTestMenu_end;
    }
    else
    {
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Success!!! Write original Smbus byte success. data : 0x%02X", wbuff);
        ConsoleCurrRow++;
    }

_eepromTestMenu_end:
    SetFlag(doneFlag, eepromDone);
    gotoxy(0, ConsoleHeight - 1);
    fprintf(stderr, "Press Alt+X to return main menu.");
    if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
    {
        ClrFlag(doneFlag, eepromDone);
        menuSelect = _mid_main;
        return;
    }
}*/
#define noValue 0xFFEE
#define writetype   1
#define readtype    0
#define wordtype   1
#define bytetype    0
void smbRWMenu(int key)
{
    int itemp = 0;
    int idx;
    int finishflag = 0;
    long int diu8tmp = noValue;
    long int adrTemp = noValue;
    long int cmu8tmp = noValue;
    long int rwTemp = noValue;
    long int lenTemp = noValue;
    long int wdataTemp = noValue;
    char *strtemp = NULL;
    uint8_t wbuff[2];
    uint8_t rbuff[2];
    int inttemp;
	uint16_t DelayCnt = 0;
	strtemp = (char *)malloc(sizeof(char) * ConsoleWidth);
	if(strtemp == NULL)
	{
		fprintf(stderr, "Failed to init Auto Test Menu.");
		return;
	}
    clrscr();
    while (1)
    {
        ConsoleCurrRow = 0;
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "SMBUS Read Write.");
        ConsoleCurrRow++;
        if (mIsFlag1(doneFlag, smbrwDone))
		{
            if((key = bioskey(1)) == 0)
            {
                delay(1);
				if(++DelayCnt < 200)
					continue;
				DelayCnt = 0;
            }
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "SMBUS Device ID List:");
        ConsoleCurrRow++;
        //list device
        for (idx = 0; idx < mApp.dyna->size; idx++)
		{
            if (mApp.dyna->dev[idx].type == DDevType_SMB
                && mApp.dyna->dev[idx].did != DDevNo_PECI)
			{
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "ID: 0x%X ; Name: %s",
                        mApp.dyna->dev[idx].did, 
						mApp.dyna->dev[idx].type_str);
                ConsoleCurrRow++;
            }
        }
        //device id
        if (diu8tmp == noValue)
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input Device ID in above list:");
            ConsoleCurrRow++;
            scanf("%s", strtemp);
            diu8tmp = strtol(strtemp, 0, 16);
            for (idx = 0; idx < mApp.dyna->size; idx++)
            {
                if (mApp.dyna->dev[idx].did == (uint8_t)diu8tmp)
                {
                    if (mApp.dyna->dev[idx].type == DDevType_SMB)
						break;
                }
            }
            if (idx == mApp.dyna->size)
            {
                diu8tmp = noValue;
            }
            continue;
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%50s", " ");
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Input Device ID in above list: 0x%X", diu8tmp);
        ConsoleCurrRow++;
        //get address
        if (adrTemp == noValue)
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input Slave Address(8Bit type):");
            ConsoleCurrRow++;
            scanf("%s", strtemp);
            adrTemp = strtol(strtemp, 0, 16);
            if (adrTemp == 0 || adrTemp > 0xFF)
            {
                adrTemp = noValue;
            }
            continue;
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%50s", " ");
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Input Slave Address(8Bit type): 0x%X", adrTemp);
        ConsoleCurrRow++;
        //get command
        if (cmu8tmp == noValue)
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input SMBUS Command(8Bit type):");
            ConsoleCurrRow++;
            scanf("%s", strtemp);
            cmu8tmp = strtol(strtemp, 0, 16);
            if (cmu8tmp > 0xFF)
            {
                cmu8tmp = noValue;
            }
            continue;
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%50s", " ");
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Input SMBUS Command(8Bit type): 0x%X", cmu8tmp);
        ConsoleCurrRow++;
        //get lenTemp
        if (lenTemp == noValue)
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input protocol Byte/Word (1-word, other-byte):");
            ConsoleCurrRow++;
            scanf("%s", strtemp);
            lenTemp = strtol(strtemp, 0, 16);
            if (lenTemp != wordtype)
            {
                lenTemp = bytetype;
            }
            continue;
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%50s", " ");
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Input protocol Byte/Word (1-word, other-byte): ");
        if (lenTemp == wordtype)
			fprintf(stderr, "Word.");
        else
			fprintf(stderr, "Byte.");
        ConsoleCurrRow++;
        //get rwtemp
        if (rwTemp == noValue)
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input read/write (1-write, other-read):");
            ConsoleCurrRow++;
            scanf("%s", strtemp);
            rwTemp = strtol(strtemp, 0, 16);
            if (rwTemp != writetype)
            {
                rwTemp = readtype;
            }
            continue;
        }
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "%50s", " ");
        gotoxy(0, ConsoleCurrRow);
        fprintf(stderr, "Input read/write (1-write, other-read): ");
        if (rwTemp == writetype)
			fprintf(stderr, "Write.");
        else
			fprintf(stderr, "Read.");
        ConsoleCurrRow++;
        //get write data
        if (rwTemp == writetype)
        {
            if (wdataTemp == noValue)
            {
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "%50s", " ");
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "Input write data:");
                ConsoleCurrRow++;
                scanf("%s", strtemp);
                wdataTemp = strtol(strtemp, 0, 16);
                if (lenTemp == wordtype)
                {
                    if (wdataTemp > 0xFFFF)
                    {
                        wdataTemp = noValue;
                    }
                }
                else
                {
                    if (wdataTemp > 0xFF)
                    {
                        wdataTemp = noValue;
                    }
                }
                continue;
            }
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "%50s", " ");
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Input write data: 0x%X", wdataTemp);
            ConsoleCurrRow++;
        }
        //process
        if (finishflag == 0)
        {
            wbuff[0] = 0;
            wbuff[1] = 0;
            rbuff[0] = 0;
            rbuff[1] = 0;
            if (rwTemp == writetype && lenTemp == bytetype)
            {
                wbuff[0] = (uint8_t)wdataTemp;
                itemp = mApp.smb->request(MBXSMB_WriteByte,	// command
                       (uint8_t)diu8tmp,                    // device id
                       (uint8_t)adrTemp,                    // slave address, use default
                       (uint8_t)cmu8tmp,                    // smbus command, no use in I2C
                       wbuff,                               // write data buffer
                       rbuff,                               // read data buffer
                       0,                                   // write data count
                       0);                                 	// read data count
            }
            else if (rwTemp == readtype && lenTemp == bytetype)
            {
                itemp = mApp.smb->request(MBXSMB_ReadByte,  // command
                       (uint8_t)diu8tmp,                    // device id
                       (uint8_t)adrTemp,                    // slave address, use default
                       (uint8_t)cmu8tmp,                    // smbus command, no use in I2C
                       wbuff,                               // write data buffer
                       rbuff,                               // read data buffer
                       0,                                   // write data count
                       0);                                  // read data count
            }
            else if (rwTemp == writetype && lenTemp == wordtype)
            {
                wbuff[0] = (uint8_t)(wdataTemp & 0xFF);
                wbuff[1] = (uint8_t)(wdataTemp >> 8);
                itemp = mApp.smb->request(MBXSMB_WriteWord, // command
                       (uint8_t)diu8tmp,                    // device id
                       (uint8_t)adrTemp,                    // slave address, use default
                       (uint8_t)cmu8tmp,                    // smbus command, no use in I2C
                       wbuff,                               // write data buffer
                       rbuff,                               // read data buffer
                       0,                                   // write data count
                       0);                                  // read data count
            }
            else if (rwTemp == readtype && lenTemp == wordtype)
            {
                itemp = mApp.smb->request(MBXSMB_ReadWord,  // command
                       (uint8_t)diu8tmp,                    // device id
                       (uint8_t)adrTemp,                    // slave address, use default
                       (uint8_t)cmu8tmp,                    // smbus command, no use in I2C
                       wbuff,                               // write data buffer
                       rbuff,                               // read data buffer
                       0,                                   // write data count
                       0);                                  // read data count
            }
            finishflag = 1;
        }
        if (itemp != SMB_ST_DONE)   //fail
        {
            gotoxy(0, ConsoleCurrRow);
            fprintf(stderr, "Fail!! Error code: 0x%X", itemp);
            ConsoleCurrRow++;
        }
        else
        {
            if (rwTemp == writetype)
            {
                inttemp = wbuff[1];
                inttemp = inttemp << 8 | wbuff[0];
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "Success!! Write 0x%X into device 0x%X", inttemp, diu8tmp);
                ConsoleCurrRow++;
            }
            else
            {
                inttemp = rbuff[1];
                inttemp = inttemp << 8 | rbuff[0];
                gotoxy(0, ConsoleCurrRow);
                fprintf(stderr, "Success!! Read 0x%X from device 0x%X", inttemp, diu8tmp);
                ConsoleCurrRow++;
            }
        }

        SetFlag(doneFlag, smbrwDone);
        gotoxy(0, ConsoleHeight - 2);
        fprintf(stderr, "Press 'R' to reset value.");
        gotoxy(0, ConsoleHeight - 1);
        fprintf(stderr, "Press Alt+X to return main menu.");
        if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
        {
            ClrFlag(doneFlag, smbrwDone);
            menuSelect = _mid_main;
            break;
        }
        else if (((key & 0xFF) == 'R')||((key & 0xFF) == 'r'))    //R r
        {
            key = 0;
            ClrFlag(doneFlag, smbrwDone);
            clrscr();
            finishflag = 0;
            diu8tmp = noValue;
            adrTemp = noValue;
            cmu8tmp = noValue;
            rwTemp 	= noValue;
            lenTemp = noValue;
            wdataTemp = noValue;
        }
    }
	free(strtemp);
}


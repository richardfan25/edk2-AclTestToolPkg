#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "rdc.h"

#include "a9610/smb.h"
#include "a9610/common.h"

#include "rdcsmbus.h"

//#define DEBUG_P 1

#ifdef DEBUG_P
	#define D_Print(format, ...) Print(format, __VA_ARGS__)
#else
	#define D_Print(format, ...)
#endif

RDC_SMBUS  smbus_info[SMB_CH_NUM];

uint8_t InitSmbus(uint8_t ch, RDC_SMBUS *smb)
{
	uint8_t ldn;
	uint8_t prescale1, prescale2;
	uint16_t u16temp;

	//Open PNP port
	sys_open_ioport(RDC_PNP_INDEX);
	sys_open_ioport(RDC_PNP_DATA);
	rdc_pnp_unlock();

	//Check Chip
	u16temp = (uint16_t)rdc_pnp_read(0x20) << 8;
	u16temp |= (uint16_t)rdc_pnp_read(0x21);
	
	if(u16temp != RDC_CHIP_ID)
	{
		D_Print(L"ERROR: error chip id \n");
		return 1;
	}
	
	if(ch == 0)
	{
		ldn = RDC_LDN_SMBUS0;
	}
	else
	{
		ldn = RDC_LDN_SMBUS1;
	}
	D_Print(L"SmBus %d \n", (uint16_t)ch);

	//Select Logic Device - Smbus
	rdc_pnp_write(0x07, ldn);
	if(rdc_pnp_read(0x30) == 0)
	{
		D_Print(L"Smbus Logic Device Disable \n");
		return 1;
	}
	
	//Get Smbus Base Address
	smb[ch].BaseAddr = rdc_pnp_read(0x60)<<8;
	smb[ch].BaseAddr |=rdc_pnp_read(0x61);
	if((smb[ch].BaseAddr == 0) || (smb[ch].BaseAddr == 0xFFFF))
	{
		D_Print(L"Smbus Logic Device Address Error(0x%X) \n", smb[ch].BaseAddr);
		return 1;
	}

	//Reset Smbus
	SET_REG_BM8(SMB_CTL2_SSRESET_bm, smb[ch].BaseAddr + SMB_CTL2_REG_OFFSET);
	
	//---------------------------------------------------------------------
	// SMBus clock calculation
	//---------------------------------------------------------------------
	// CPU   clock :  50 MHz
	// SMBus clock = CPU clock / (Prescale1) / (Prescale2 + 1)
	//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
	//---------------------------------------------------------------------
	prescale1 = 50;
	outp(smb[ch].BaseAddr + SMB_CLKCTL1_REG_OFFSET, prescale1);

	prescale2 = 9;
	prescale2 &= ~SMB_CLKCTL2_FAST_bm;
	outp(smb[ch].BaseAddr + SMB_CLKCTL2_REG_OFFSET, prescale2);

	smb[ch].Freq = RDC_SMBUS_CLK_SRC / prescale1 / (prescale2+1);
	smb[ch].Flag.init = 1;

	//Close PNP port
	rdc_pnp_lock();
	sys_close_ioport(RDC_PNP_INDEX);
	sys_close_ioport(RDC_PNP_DATA);

	return 0;
}

uint8_t SmbusTransaction(SMBUS_RW *tmp)
{
	uint16_t retry;
	uint16_t baddr;
	uint8_t i, value, status, err;
	uint8_t ch=tmp->ch;

	err = SMB_OK;
	baddr = smbus_info[ch].BaseAddr;
	
	//wait smbus in use
	retry = SMB_WAIT_RETRIES;
	while(TST_REG_BM8(SMB_STS_INUSE_bm, baddr + SMB_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			Print(L"Smbus In Use \n");
			err = SMB_ERR_WAIT_FREE_TMOUT;
			goto exit_trans;
		}
	}

	//check busy
	if(TST_REG_BM8(SMB_STS_HOSTBUSY_bm, baddr + SMB_STS_REG_OFFSET))
	{
		//kill transaction if busy
		SET_REG_BM8(SMB_CTL_KILL_bm, baddr + SMB_CTL_REG_OFFSET);
		CLR_REG_BM8(SMB_CTL_KILL_bm, baddr + SMB_CTL_REG_OFFSET);
	}

	//clean all data reg
	outp(baddr + SMB_DAT0_REG_OFFSET, SMB_DAT0_DATA0_gp);
	outp(baddr + SMB_DAT1_REG_OFFSET, SMB_DAT1_DATA1_gp);

	//set tx command
	outp(baddr + SMB_CMD_REG_OFFSET, tmp->cmd);
	
	//set tx slave address
	outp(baddr + SMB_SADD_REG_OFFSET, tmp->addr);

	//set Packet Error Check Register
	outp(baddr + SMB_PEC_REG_OFFSET, tmp->pec);

	//according SMB_CMD to set tx data
	switch(tmp->mode & SMB_CMD_MASK)
	{
		case SMB_CMD_QUICK:
		{
			break;
		}
		case SMB_CMD_BYTE:
		{
			break;
		}
		case SMB_CMD_BYTE_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
				outp(baddr + SMB_DAT1_REG_OFFSET, *(tmp->wbuf+1));
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			outp(baddr + SMB_DAT0_REG_OFFSET, *tmp->wbuf);
			outp(baddr + SMB_DAT1_REG_OFFSET, *(tmp->wbuf+1));
			break;
		}
		case SMB_CMD_BLOCK:
		{
			//enable 32 byte buffer
			SET_REG_BM8(SMB_CTL2_E32B_bm, baddr + SMB_CTL2_REG_OFFSET);

			if((tmp->addr & SMB_RW_MASK) == SMB_RW_WRITE)
			{
				outp(baddr + SMB_DAT0_REG_OFFSET, tmp->wlen);
				for(i=0; i<tmp->wlen; i++)
				{
					outp(baddr + SMB_BLK_REG_OFFSET, *(tmp->wbuf + i));
				}
			}
			break;
		}
		case SMB_CMD_I2C_READ:
		{
			err = SMB_ERR_PROTOCOL;
			break;
		}
		case SMB_CMD_BLOCK_PROC:
		{
			//enable 32 byte buffer
			SET_REG_BM8(SMB_CTL2_E32B_bm, baddr + SMB_CTL2_REG_OFFSET);

			outp(baddr + SMB_DAT0_REG_OFFSET, tmp->wlen);
			for(i=0; i<tmp->wlen; i++)
			{
				outp(baddr + SMB_BLK_REG_OFFSET, *(tmp->wbuf + i));
			}
			break;
		}
		default :
		{
			err = SMB_ERR_PROTOCOL;
			break;
		}
	}

	if(err != SMB_OK)
	{
		goto exit_trans;
	}

	//start transaction
	value = inp(baddr + SMB_CTL_REG_OFFSET);
	SET_VAR_GM(SMB_CTL_SMBCMD_gm, (tmp->mode & SMB_CMD_MASK)<<SMB_CTL_SMBCMD_gp, value);	//cmd=mode
	SET_VAR_BM(SMB_CTL_START_bm, value);	//start=1
	CLR_VAR_BM(SMB_CTL_I2CNACKEN_bm, value);	//nack=0
	CLR_VAR_BM(SMB_CTL_KILL_bm, value);		//kill=0
	
	if(tmp->mode & SMB_CMD_PEC_MASK)
	{
		SET_VAR_BM(SMB_CTL_PECEN_bm, value);	//PEC enable
	}
	else
	{
		CLR_VAR_BM(SMB_CTL_PECEN_bm, value);	// PEC disable
	}

	outp(baddr + SMB_CTL_REG_OFFSET, value);

	//wait tx done
	retry = SMB_WAIT_RETRIES;
	while(TST_REG_BM8(SMB_STS_HOSTBUSY_bm, baddr + SMB_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			D_Print(L"Smbus Tx Timeout \n");
			err = SMB_ERR_TXDONE_TMOUT;
			goto exit_trans;
		}
	}

	//check status
	status = inp(baddr + SMB_STS_REG_OFFSET);
	if(status & SMB_STS_FAILED_bm)
	{
		D_Print(L"Smbus Transaction Fail \n");
		err = SMB_ERR_FAILED;
	}
	else if(status & SMB_STS_ARLERR_bm)
	{
		D_Print(L"Smbus Arbitration Lost \n");
		err = SMB_ERR_ARBI;
	}
	else
	{
		status = inp(baddr + SMB_STS2_REG_OFFSET);
		if(status & SMB_STS2_TOERR_bm)
		{
			D_Print(L"Smbus Time-Out Error \n");
			err = SMB_ERR_TMOUT;
		}
		else if(status & SMB_STS2_NACKERR_bm)
		{
			D_Print(L"Smbus Not Response ACK Error \n");
			err = SMB_ERR_NACK;
		}
		else if(status & SMB_STS2_PECERR_bm)
		{
			D_Print(L"Smbus PEC Error \n");
			err = SMB_ERR_PEC;
		}
	}

	if(err != SMB_OK)
	{
		goto exit_trans;
	}

	//read data from buffer
	switch(tmp->mode & SMB_CMD_MASK)
	{
		case SMB_CMD_QUICK:
		{
			break;
		}
		case SMB_CMD_BYTE:
		{
			break;
		}
		case SMB_CMD_BYTE_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
			}
			break;
		}
		case SMB_CMD_WORD_DATA:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
				*(tmp->rbuf + 1) = inp(baddr + SMB_DAT1_REG_OFFSET);
			}
			break;
		}
		case SMB_CMD_PROC_CALL:
		{
			*tmp->rbuf = inp(baddr + SMB_DAT0_REG_OFFSET);
			*(tmp->rbuf + 1) = inp(baddr + SMB_DAT1_REG_OFFSET);
			break;
		}
		case SMB_CMD_BLOCK:
		{
			if((tmp->addr & SMB_RW_MASK) == SMB_RW_READ)
			{
				*tmp->blen = inp(baddr + SMB_DAT0_REG_OFFSET);
				if(*tmp->blen > tmp->rlen)
				{
					err = SMB_ERR_BUF;
					goto exit_trans;
				}
				
				for(i=0; i<*tmp->blen; i++)
				{
					*(tmp->rbuf + i) = inp(baddr + SMB_BLK_REG_OFFSET);
				}
			}
			break;
		}
		case SMB_CMD_I2C_READ:
		{
			break;
		}
		case SMB_CMD_BLOCK_PROC:
		{
			*tmp->blen = inp(baddr + SMB_DAT0_REG_OFFSET);
			if(*tmp->blen > tmp->rlen)
			{
				err = SMB_ERR_BUF;
				goto exit_trans;
			}
			
			for(i=0; i<*tmp->blen; i++)
			{
				*(tmp->rbuf + i) = inp(baddr + SMB_BLK_REG_OFFSET);
			}
			break;
		}
		default:
		{
			err = SMB_ERR_PARAMETERS;
			goto exit_trans;
		}
	}

exit_trans:
	//clear all status
	outp(baddr + SMB_STS_REG_OFFSET, 0xFF);
	outp(baddr + SMB_STS2_REG_OFFSET, 0xFF);

	return err;
}

uint8_t SmbusReadQuick(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_QUICK;
	smb_data.cmd = 0;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BYTE_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 1;
	smb_data.rbuf = rbuf;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BYTE_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 1;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadWord(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_WORD_DATA;
	smb_data.cmd= cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 2;
	smb_data.rbuf = rbuf;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteWord(uint8_t ch,uint8_t cmd, uint8_t addr, uint8_t *wbuf)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_WORD_DATA;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 2;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = NULL;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusReadBlock(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf, uint8_t *blen)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BLOCK;
	smb_data.cmd= cmd;
	smb_data.addr = addr | SMB_RW_READ;
	smb_data.wlen = 0;
	smb_data.wbuf = NULL;
	smb_data.rlen = 32;
	smb_data.rbuf = rbuf;
	smb_data.blen = blen;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

uint8_t SmbusWriteBlock(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf, uint8_t *blen)
{
	SMBUS_RW smb_data;

	smb_data.ch = ch;
	smb_data.mode = SMB_CMD_BLOCK;
	smb_data.cmd = cmd;
	smb_data.addr = addr & 0xFE;
	smb_data.wlen = 32;
	smb_data.wbuf = wbuf;
	smb_data.rlen = 0;
	smb_data.rbuf = NULL;
	smb_data.blen = blen;
	smb_data.pec = 0;

	return SmbusTransaction(&smb_data);
}

void AsciiToUnicodeString(uint8_t *origin, uint16_t *str)
{
	while(*origin != '\0')
	{
		*str++ = (uint16_t)*origin++;
	}
	*str = '\0';
}

uint8_t ScanSmbusDevice(uint8_t ch, RDC_SMBUS *smb)
{
	uint8_t status, count=0;
	uint8_t rdata=0;
	uint16_t i;

	Print(L"Scan Smbus %d... \n\n", (uint16_t)ch);
	for(i=0; i<255; i+=2)
	{
		status = SmbusReadByte(ch, 0x01, (uint8_t)i, &rdata);
		if(status == SMB_OK)
		{
			if(rdata)
			{
				Print(L"Device %d Address : 0x%X \n", count, i);
				smb[ch].DevAddr[count] = (uint8_t)i;
				count++;
			}
		}
		rdata = 0;
	}

	smb[ch].Cnt = count;

	return count;
}

void ExecuteA(RDC_SMBUS *smb)
{
	char key;
	uint8_t ch, count=0;

	clrscr();
	Print(L"List SMBus Address \n\n");
	
	for(ch=0; ch<SMB_CH_NUM; ch++)
	{
		if(smb[ch].Flag.init == 0)	//if channel can't use, to next channel
		{
			continue;
		}
		
		count = ScanSmbusDevice(ch, smb);
		if(count)
		{
			Print(L"Smbus %d Find %d Device \n\n", (uint16_t)ch, (uint16_t)count);
		}
		else
		{
			Print(L"Smbus %d No Device \n\n", (uint16_t)ch);
		}
	}

	Print(L"Press 'q' or 'Q' To Exit \n");

	while(1)
	{
		key = (char)(bioskey(0) & 0xFF);
		if(key == 'q' || key == 'Q')
		{
			break;
		}
	}
	
}

uint8_t ReadSmartBatteryAllData(uint8_t addr)
{
	uint8_t battery_ch=0, battery_addr=0x16, battery_max_cmd=35;
	uint8_t j, max_ddata=32, offset=0, max_block_data=32;
	uint8_t *block_data;
	uint8_t *ddata;
	uint8_t blen;
	UINTN x_cursor, y_cursor;
	uint16_t str16[32];
	
	const uint16_t *SmartBatteryRegName[]={
		L"Manufacturer",
		L"RemainingCapacityAlarm",
		L"RemainingTimeAlarm",
		L"BatteryMode",
		L"AtRate",
		L"AtRateTimeToFull",
		L"AtRateTimeToEmpty",
		L"AtRateOK",
		L"Temperature",
		L"Voltage",
		L"Current",
		L"AverageCurrent",
		L"MaxError",
		L"RelativeStateOfCharge",
		L"AbsoluteStateOfCharge",
		L"RemainingCapacity",
		L"FullChargeCapacity",
		L"RunTimeToEmpty",
		L"AverageTimeToEmpty",
		L"AverageTimeToFull",
		L"ChargingCurrent", 
		L"ChargingVoltage",
		L"BatteryStatus",
		L"CycleCount",
		L"DesignCapacity",
		L"DesignVoltage",
		L"SpecificationInfo",
		L"ManufactureDate",
		L"SerialNumber",
		L"ManufacturerName",
		L"DeviceName",
		L"DeviceChemistry",
		//"ManufacturerData",
		};

	ddata = malloc(max_ddata);
	block_data = malloc(max_block_data);
	if(!ddata)
	{
		Print(L"malloc error \n");
		return 1;
	}
	if(!block_data)
	{
		Print(L"malloc error \n");
		free(ddata);
		return 1;
	}
	
	memset(ddata, '\0', max_ddata);
	memset(block_data, '\0', max_block_data);

	for(j=0; j<battery_max_cmd; j++)
	{
		if(offset >= sizeof(SmartBatteryRegName)/sizeof(*SmartBatteryRegName))
		{
			Print(L"\n");
			//gST->ConOut->SetAttribute(gST->ConOut, 0x07);
			break;
		}
		
		y_cursor = gST->ConOut->Mode->CursorRow;
		if(offset%2)
		{
			gotoxy(ConsoleWidth/2, y_cursor);
			x_cursor = ConsoleWidth - 13;
		}
		else
		{
			gotoxy(0, y_cursor);
			x_cursor = ConsoleWidth/2 -13;
		}
		
		if(j<29)
		{
			if(SmbusReadWord(battery_ch, j, battery_addr, ddata) != SMB_OK)
			{
				Print(L"Read Command %d Error \n", (uint16_t)j);
				goto read_error;
			}
			//fprintf(stderr, "%04X    ", *(ddata + j));

			//gST->ConOut->SetAttribute(gST->ConOut, 0x07);	//0:black 1:blue 2:green 3:cyan 4:red 5:magenta 6:yellow 7:light gray
			Print(L"%02X : ", (uint16_t)j);
			Print(L"%s", *(SmartBatteryRegName + offset));
			
			//gST->ConOut->SetAttribute(gST->ConOut, 0x02);
			gotoxy(x_cursor, y_cursor);
			Print(L": %04X", (uint16_t)(((uint16_t)ddata[1]<<8) | ddata[0]));
			if((offset%2))
			{
				Print(L"\n");
			}
			offset++;
		}
		else if(j>31)
		{
			if(SmbusReadBlock(battery_ch, j, battery_addr, block_data, &blen) != SMB_OK)
			{
				Print(L"Read Command %d Error \n", (uint16_t)j);
				goto read_error;
			}

			Print(L"%02X : ", (uint16_t)j);
			Print(L"%s", *(SmartBatteryRegName + offset));

			gotoxy(x_cursor, y_cursor);
			AsciiToUnicodeString(block_data, str16);
			Print(L": %s", str16);

			if((offset%2))
			{
				Print(L"\n");
			}
			memset(block_data, '\0', max_block_data);
			offset++;
		}
	}

read_error:
	free(ddata);
	free(block_data);

	return 0;
}

void ExecuteB(void)
{
	char	key;
	uint8_t battery_addr=0x16;
	INT32 x_cur_bak=0, y_cur_bak=0;
	
	EFI_EVENT smbus_event[2];
	UINTN event_index=0;

	clrscr();
	Print(L"Read Smart Battery Data \n\n");
	x_cur_bak = gST->ConOut->Mode->CursorColumn;
	y_cur_bak = gST->ConOut->Mode->CursorRow;
	
	ReadSmartBatteryAllData(battery_addr);

	smbus_event[0] = gST->ConIn->WaitForKey;
	gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (void*)NULL, &smbus_event[1]);
	gBS->SetTimer(smbus_event[1], TimerPeriodic, 1000*1000*10);	//unit 100ns

	Print(L"\n\n");
	Print(L"Press any to continue \n");
			
	while(1)
	{
		gBS->WaitForEvent(2, smbus_event, &event_index);
		gST->ConOut->EnableCursor(gST->ConOut, 0);
		if(event_index == 0)
		{
			gBS->SetTimer(smbus_event[1], TimerCancel, 1000*1000*10);
			gST->ConIn->ReadKeyStroke(gST->ConIn, (EFI_INPUT_KEY *)&key);
			gST->ConOut->EnableCursor(gST->ConOut, 1);
			gBS->CloseEvent(smbus_event);
			break;
		}
		else
		{
			gotoxy(x_cur_bak, y_cur_bak);
			//ReadSmartBatteryAllData(battery_addr);
		}
	}
}

void ExecuteC()
{
	char	key;
	uint16_t j;
	uint8_t i, ddata;

	for(i=0; i<4; i++)
	{
		for(j=0; j<256; j++)
		{
			SmbusReadByte(0, (uint8_t)j, 0xA8|(i<<1), &ddata);
			Print(L"%02X  ", (uint16_t)ddata);
		}
		Print(L"\n\n");
	}

	Print(L"Press 'q' or 'Q' To Exit \n");

	while(1)
	{
		key = (char)(bioskey(0) & 0xFF);
		if(key == 'q' || key == 'Q')
		{
			break;
		}
	}
}
	
void DisplayUsage(void)
{
	clrscr();
	Print(L"\nCopyright(c) , Advantech Corporation 2017-2018 \n");
	Print(L"RDC Smbus Test Utility \n");
	Print(L"Date:%s \n", build_date);
	Print(L"Version:%s \n\n", build_version);
}

void ShowOptionList(void)
{
	Print(L"[a]  List SMBus Address \n");
	Print(L"[b]  Test SMBus0 - Read Smart Battery \n");
	//Print(L"[c]  Test SMBus0 - Read EERPOM \n");
	//Print(L"[c]  Test SMBus0 - Write EERPOM \n");
	//Print(L"[d]  Test SMBus0 - Auto Write EEPROM \n");
	//Print(L"[e]  Test SMBus1 - Read Thermal IC \n");
	//Print(L"[f]  Test SMBus1 - Write Thermal IC \n");
	//Print(L"[g]  Test SMBus1 - Write DIO Board \n\n");

	Print(L"[q]  Exit \n\n");
	//Print(L"Press any key to continue, or press 'q' or 'Q' to exit \n\n");
}

EFI_STATUS main(IN int Argc, IN char * * Argv)
{
	char key;
	uint8_t ch, err=0;
	
	console_init();
	
	//Init Smbus
	for(ch=0; ch<SMB_CH_NUM; ch++)
	{
		if(InitSmbus(ch, smbus_info))
		{
			Print(L"Smbus %d Init Fail!! \n", (uint16_t)ch);
			err++;
		}
		if(err == SMB_CH_NUM)
		{
			return EFI_DEVICE_ERROR;
		}
	}

	while(1)
	{
		DisplayUsage();
		ShowOptionList();
		key = (char)(bioskey(0) & 0xFF);

		if(key == 'q' || key == 'Q')
		{
			console_exit();
			break;
		}

		//ExecuteA(smbus_info);

		switch(key)
		{
			case 'a':
			case 'A':
			{
				ExecuteA(smbus_info);
				break;
			}
			case 'b':
			case 'B':
			{
				ExecuteB();
				break;
			}
			case 'c':
			case 'C':
			{
				//ExecuteC();
				break;
			}
			case 'd':
			case 'D':
			{
				break;
			}
			case 'e':
			case 'E':
			{
				break;
			}
			case 'f':
			case 'F':
			{
				break;
			}
			case 'g':
			case 'G':
			{
				break;
			}
		}
	}
	
	return EFI_SUCCESS;
}

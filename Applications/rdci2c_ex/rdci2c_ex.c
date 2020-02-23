#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "util.h"
#include "global.h"

#include "rdci2c_ex.h"


uint8_t InitI2C(RDC_I2C * i2c)
{
	uint8_t ch, ldn, u8tmp;
	uint16_t u16temp;

	for(ch=0; ch<2; ch++)
	{
		i2c[ch].Flag.init = 0;
		
		//Open PNP port
		outp(RDC_CFG_IDX_PORT, 0x87);	// enter config
		outp(RDC_CFG_IDX_PORT, 0x87);

		//Check Chip
		outp(RDC_CFG_IDX_PORT, 0x20);	// ID0
		u16temp = (uint16_t)inp(RDC_CFG_DAT_PORT);
		
		outp(RDC_CFG_IDX_PORT, 0x21);	// ID1
		u16temp <<= 8;
		u16temp |= (uint16_t)inp(RDC_CFG_DAT_PORT);
		
		if(u16temp != RDC_CHIP_ID)
		{
			Print(L"ERROR: Error Chip ID !! \n\n");
			return 1;
		}
		
		if(ch == 0)
		{
			ldn = RDC_LDN_I2C0;
		}
		else
		{
			ldn = RDC_LDN_I2C1;
		}

		//Select Logic Device - I2C
		outp(RDC_CFG_IDX_PORT, 0x07);
		outp(RDC_CFG_DAT_PORT, ldn);

		outp(RDC_CFG_IDX_PORT, 0x30);
		u8tmp = inp(RDC_CFG_DAT_PORT);
		if(u8tmp == 0)
		{
			Print(L"I2C Logic Device Disable \n");
			continue;
		}
		
		//Get I2C Base Address
		outp(RDC_CFG_IDX_PORT, 0x60);
		i2c[ch].BaseAddr = inp(RDC_CFG_DAT_PORT);
		i2c[ch].BaseAddr <<= 8;
		outp(RDC_CFG_IDX_PORT, 0x61);
		i2c[ch].BaseAddr |= inp(RDC_CFG_DAT_PORT);
		if((i2c[ch].BaseAddr == 0) || (i2c[ch].BaseAddr == 0xFFFF))
		{
			Print(L"I2C Logic Device Address Error(0x%X) \n", i2c[ch].BaseAddr);
			continue;
		}

		//Reset I2C
		SET_REG_BM8(I2C_EXCTL_I2CRST_bm, i2c[ch].BaseAddr + I2C_EXCTL_REG_OFFSET);
		
		//set I2C My_Address 0xFE, avoid scan self
		SET_REG_BM8(I2C_MYADD_MYADDR_gm, i2c[ch].BaseAddr + I2C_MYADD_REG_OFFSET);
		
		i2c[ch].Flag.init = 1;

		//Close PNP port
		outp(RDC_CFG_IDX_PORT, 0xAA);	// exit config
	}

	if((i2c[0].Flag.init == 0) && (i2c[1].Flag.init == 0))
	{
		Print(L"I2C Can Not Use \n\n");
		return 1;
	}
	
	return 0;
}

void I2CSetClockFrequency(RDC_I2C * i2c)
{
	uint8_t ch, type;
	uint8_t prescale1, prescale2;

	//choice i2c frequency
	//0:100K   1:400K   2:1M
	type = 0;
	
	for(ch=0; ch<2; ch++)
	{
		if(i2c[ch].Flag.init == 0)
		{
			continue;
		}

		switch(type)
		{
			case 0:
			{
				//---------------------------------------------------------------------
				// I2C clock calculation
				//---------------------------------------------------------------------
				// CPU   clock :  50 MHz
				// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
				//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 10 = 50000 kHz / 500 = 100 kHz
				//---------------------------------------------------------------------
				prescale1 = 50;
				outp(i2c[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

				prescale2 = 9;
				prescale2 &= ~I2C_CLK2_FAST_bm;
				outp(i2c[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);

				i2c[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
				break;
			}
			case 1:
			{
				//---------------------------------------------------------------------
				// I2C clock calculation
				//---------------------------------------------------------------------
				// CPU   clock :  50 MHz
				// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
				//   50MHz / 50 / (9 + 1) = 50MHz / 25 / 5 = 50000 kHz / 125 = 400 kHz
				//---------------------------------------------------------------------
				prescale1 = 25;
				outp(i2c[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

				prescale2 = 4;
				prescale2 |= I2C_CLK2_FAST_bm;
				outp(i2c[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);

				i2c[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
				break;
			}
			case 2:
			{
				//---------------------------------------------------------------------
				// I2C clock calculation
				//---------------------------------------------------------------------
				// CPU   clock :  50 MHz
				// I2C clock = CPU clock / (Prescale1) / (Prescale2 + 1)
				//   50MHz / 50 / (9 + 1) = 50MHz / 50 / 1 = 50000 kHz / 50 = 1000 kHz
				//---------------------------------------------------------------------
				prescale1 = 50;
				outp(i2c[ch].BaseAddr + I2C_CLK1_REG_OFFSET, prescale1);

				prescale2 = 0;
				prescale2 |= I2C_CLK2_FAST_bm;
				outp(i2c[ch].BaseAddr + I2C_CLK2_REG_OFFSET, prescale2);

				i2c[ch].Freq = RDC_I2C_CLK_SRC / prescale1 / (prescale2+1);
				break;
			}
		}
	}
}

uint8_t I2CWaitTxDone(uint16_t addr, uint8_t p)
{
	uint8_t status;
	uint16_t retry = 3000;

	while(retry)
	{
		usleep(1);
		status = inp(addr + I2C_STS_REG_OFFSET);
		if(status & 0x10)
		{
			if(p)
			{
				Print(L"I2C Not Response ACK Error \n");
			}
			return 1;
		}
		else if(status & 0x08)
		{
			if(p)
			{
				Print(L"I2C Arbitration Lost \n");
			}
			return 1;
		}
		else if(status & 0x20)
		{
			//double check, avoid tx done flag set early than not response ack flag
			status = inp(addr + I2C_STS_REG_OFFSET);
			if(status & 0x10)
			{
				if(p)
				{
					Print(L"I2C Not Response ACK Error \n");
				}
				return 1;
			}
			else if(status & 0x08)
			{
				if(p)
				{
					Print(L"I2C Arbitration Lost \n");
				}
				return 1;
			}
			
			break;
		}
		
		retry--;
		if(retry == 0)
		{
			if(p)
			{
				Print(L"I2C Tx Timeout \n");
			}
			return 1;
		}
	}

	//clean tx done status register
	outp(addr + I2C_STS_REG_OFFSET, inp(addr + I2C_STS_REG_OFFSET) | I2C_STS_TXDONE_bm);
	
	return 0;
}

uint8_t I2CWaitRxDone(uint16_t addr)
{
	uint16_t retry = 3000;
	
	while(!TST_REG_BM8(I2C_STS_RXRDY_bm, addr + I2C_STS_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			Print(L"I2C Rx Timeout \n");
			return 1;
		}
		usleep(1);
	}

	//clean rx done status register
	outp(addr + I2C_STS_REG_OFFSET, inp(addr + I2C_STS_REG_OFFSET) | I2C_STS_RXRDY_bm);

	return 0;
}

uint8_t I2CWaitStopDone(uint16_t addr)
{
	uint16_t retry = 3000;
	
	while(TST_REG_BM8(I2C_CTL_STOP_bm, addr + I2C_CTL_REG_OFFSET))
	{
		retry--;
		if(retry == 0)
		{
			Print(L"I2C Stop Timeout \n");
			return 1;
		}
		usleep(1);
	}

	return 0;
}

void I2CReadEepromTest(RDC_I2C *i2c)
{
	uint8_t eeprom_ch = 0, eeprom_addr = 0, eeprom_type = 0;
	uint8_t rbuf[2];
	uint16_t i, len, eeprom_saddr = 0;

	clrscr();
	Print(L"I2C - Read Eeprom Test. \n\n");

	//choice i2c channal
	//I2C0 : set eeprom_ch to 0
	//I2C1 : set eeprom_ch to 1
	eeprom_ch = 0;

	//eeprom address type
	//0:Byte 1:Word
	eeprom_type = 1;
	
	eeprom_addr = (uint8_t) get_number_stdin(L"Enter Slave Address(0x00~0xFF):", 1, 2);

	if(eeprom_type)
	{
		eeprom_saddr = (uint16_t) get_number_stdin(L"Enter EEPROM Start Address(0x0000~0xFFFF):", 1, 4);
	}
	else
	{
		eeprom_saddr = (uint16_t) get_number_stdin(L"Enter EEPROM Start Address(0x00~0xFF):", 1, 2);
	}
	
	len = (uint16_t) get_number_stdin(L"Enter Read Length(0~256):", 0, 3);
	if(len > 256)
	{
		len = 256;
	}
	
	if(!eeprom_type)
	{
		if((eeprom_saddr + len - 1) > 0xFF)
		{
			Print(L"Read Length Over EEPROM Range \n");
			goto ire_end;
		}
	}

	//start to read
	for(i=0; i<len; i++)
	{
		//clean status register
		outp(i2c[eeprom_ch].BaseAddr + I2C_STS_REG_OFFSET, 0xFF);
		
		//send addr
		outp(i2c[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr);
		//wait tx done
		if(I2CWaitTxDone(i2c[eeprom_ch].BaseAddr, 1) != 0)
		{
			console_color(RED, BLACK);
			Print(L"Please Check Eeprom Connect Correctly!! \n\n");
			console_color(LIGHTGRAY, BLACK);
			goto ire_end;
		}

		//if word mode, need to send two command
		if(eeprom_type)
		{
			//send command 1
			outp(i2c[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, ((eeprom_saddr + i) >> 8));
			//wait tx done
			if(I2CWaitTxDone(i2c[eeprom_ch].BaseAddr, 1) != 0)
			{
				goto ire_end;
			}
		}
		
		//send command
		outp(i2c[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET, (UINT8)(eeprom_saddr + i));
		//wait tx done
		if(I2CWaitTxDone(i2c[eeprom_ch].BaseAddr, 1) != 0)
		{
			goto ire_end;
		}

		//send addr
		outp(i2c[eeprom_ch].BaseAddr + I2C_TXADD_REG_OFFSET, eeprom_addr | 1);
		//wait tx done
		if(I2CWaitTxDone(i2c[eeprom_ch].BaseAddr, 1) != 0)
		{
			Print(L"Send Address Error!! \n\n");
			goto ire_end;
		}

		//dummy read to trigger read process
		inp(i2c[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//set stop flag
		outp(i2c[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET, inp(i2c[eeprom_ch].BaseAddr + I2C_CTL_REG_OFFSET) | 0x02);

		//wait rx data ready
		if(I2CWaitRxDone(i2c[eeprom_ch].BaseAddr) != 0)
		{
			goto ire_end;
		}

		//get data
		rbuf[0] = inp(i2c[eeprom_ch].BaseAddr + I2C_DAT_REG_OFFSET);

		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[0]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

ire_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(bioskey(0))){}
}

EFI_STATUS main(int argc, char * * argv)
{
	EFI_STATUS status;
	RDC_I2C i2c[2];

	status = console_init();
	if(status != EFI_SUCCESS)
	{
		Print(L"ERROR: Failed to change console setting. \n");
		return status;
	}

	if(InitI2C(i2c))
	{
		return EFI_NOT_FOUND;
	}
	usleep(100);

	I2CSetClockFrequency(i2c);
	I2CReadEepromTest(i2c);
	
	clrscr();
	console_exit();
	
	return EFI_SUCCESS;
}


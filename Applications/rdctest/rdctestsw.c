#include "rdctest.h"


uint8_t SWProtocolOnlyWritePMC(uint8_t cmd)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = 0;
	prot.off = 0;
	prot.len = 0;
	prot.dat = NULL;

	return RDCSWProtocolPMC(&prot);
}

void ShowBoardInformationSW(void)
{
	uint8_t len, buf[32] = {'\0'};
	uint16_t tmp[20];

	clrscr();

	//Board Name
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_PLAT_INFO, 0xFF, 0x06, 1, buf))
	{
		Print(L"Read Board Name Length Fail !! \n\n");
		goto sbis_end;
	}
	len = buf[0];
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_PLAT_INFO, 0xFF, 0x07, len, buf))
	{
		Print(L"Read Board Name Fail !! \n\n");
		goto sbis_end;
	}
	AsciiToUnicodeString(buf, tmp);
	Print(L"[-] Board Name               : %s\n", tmp);

	//Chip Name
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_EC_INFO, 0xFF, 0x00, 16, buf))
	{
		Print(L"Read Chip Name Length Fail !! \n\n");
		goto sbis_end;
	}
	AsciiToUnicodeString(buf, tmp);
	Print(L"[-] Chip Name                : %s\n", tmp);

	//firmware version number
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FWVER, 0xFF, 0x00, 9, buf))
	{
		Print(L"Read FW Version Fail !! \n\n");
		goto sbis_end;
	}
	Print(L"[-] Firmware Version Number  : %d.%d.%d\n", (buf[1] & 0x0F) * 10 + (buf[2] & 0x0F), 
		(buf[3] & 0x0F) * 10 + (buf[4] & 0x0F), ((buf[5] & 0x0F) * 10 + (buf[6] & 0x0F)) * 100 + ((buf[7] & 0x0F) * 10 + (buf[8] & 0x0F)));

	//firmware version string
	AsciiToUnicodeString(buf, tmp);
	Print(L"[-] Firmware Version String  : %s\n", tmp);

sbis_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void ShowHardwareMonitorSW(void)
{
	uint8_t i, show_flag = 0;
	uint8_t buf[32];
	uint16_t data16;
	uint32_t data32;
	
	clrscr();
	while(1)
	{
		gotoxy(0, 0);
		Print(L"Hardware Monitor. \n\n");
		ConsoleCurrRow = 2;

		//Temperature
		show_flag = 1;
		for(i=0; i<4; i++)
		{
			rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
			// available ?
			if((buf[0] & 0x1) == 0)
			{
				continue;
			}

			if(show_flag)
			{
				show_flag = 0;
				gotoxy(0, ConsoleCurrRow);
				Print(L"Temperature: \n");
				ConsoleCurrRow++;
			}

			gotoxy(10, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				if(buf[0] > 0x1F)
				{
					Print(L"Type Error! (0x%x)", buf[0]);
				}
				else
				{
					Print(L"%s_Temperature", *(gThermalType + buf[0]));
				}
			}

			gotoxy(40, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				data16 = *(uint16_t *)buf;
				Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
			}
			ConsoleCurrRow++;
		}
		ConsoleCurrRow++;

		//Fan Speed
		show_flag = 1;
		for(i=0; i<4; i++)
		{
			rdc_sw_ec_read_prot(RDC_SW_CMD_RD_TACH, i, 0x00, 1, buf);
			// available ?
			if((buf[0] & 0x1) == 0)
			{
				continue;
			}

			if(show_flag)
			{
				show_flag = 0;
				gotoxy(0, ConsoleCurrRow);
				Print(L"Fan Speed: \n");
				ConsoleCurrRow++;
			}
			
			gotoxy(10, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_TACH, i, 0x04, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Tachometer Type Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_TACH, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				if(buf[0] > 0x1F)
				{
					Print(L"Type Error!! (0x%x)", buf[0]);
				}
				else
				{
					Print(L"%s_Tachometer", *(gFanType + buf[0]));
				}
			}

			gotoxy(40, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_TACH, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Tachometer Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_TACH, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				data32 = *(uint32_t *)buf;
				Print(L": %4d RPM\n", data32);
			}
			ConsoleCurrRow++;
		}
		ConsoleCurrRow++;

		//Voltage
		show_flag = 1;
		for(i=0; i<8; i++)
		{
			rdc_sw_ec_read_prot(RDC_SW_CMD_RD_VOLT, i, 0x00, 1, buf);
			// available ?
			if((buf[0] & 0x1) == 0)
			{
				continue;
			}

			if(show_flag)
			{
				show_flag = 0;
				gotoxy(0, ConsoleCurrRow);
				Print(L"Voltage: \n");
				ConsoleCurrRow++;
			}
			
			gotoxy(10, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_VOLT, i, 0x01, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Voltage Type Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_VOLT, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				if(buf[0] > 0x13)
				{
					Print(L"Type Error! (0x%x)", buf[0]);
				}
				else
				{
					Print(L"%s", *(gVoltageTypeSW + buf[0]));
				}
			}

			gotoxy(40, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_VOLT, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Voltage Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_VOLT, i, 0x00, 1, buf);
				goto shms_end;
			}
			else
			{
				data16 = *(uint16_t *)buf;
				Print(L": %2d.%02d V \n", data16/100, data16%100);
			}
			ConsoleCurrRow++;
		}
		ConsoleCurrRow++;

		//Current
		//

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto shms_endend;
		}
	}
	
shms_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

shms_endend:
	NULL;
}

void ShowThermalInformationSW(void)
{
	uint8_t i, buf[32];
	uint16_t data16;

	clrscr();

	Print(L"[-] Thermal Module Status     : ");
	for(i=0; i<4; i++)
	{
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
	}
	Print(L"\n");

	for(i=0; i<4; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
		//Temperature available ?
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}

		//Type
		Print(L"[%d] Thermal Device Type       : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//Control
		Print(L"[%d] Thermal Protect Control   : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//Temperature
		Print(L"[%d] Temperature (Value, Max, Min) :\n", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			Print(L"    0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x04, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			Print(L"0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x06, 2, buf))
		{
			Print(L"Error !! \n");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			Print(L"0x%04X = %d = %3d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		Print(L"\n");

		//Shutdown
		Print(L"[%d] Thermal Prot - Shutdown (Sts, Hi, Lo)  :\n", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"    0x%02X ", buf[0]);
		}
		buf[0] = 0x01;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
			Print(L"Error !! ");
		}
		else
		{
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}

			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}
		}
		Print(L"\n");

		//Throttle
		Print(L"[%d] Thermal Prot - Throttle (Sts, Hi, Lo)  :\n", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"    0x%02X ", buf[0]);
		}
		buf[0] = 0x02;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
			Print(L"Error !! ");
		}
		else
		{
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}

			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}
		}
		Print(L"\n");

		//Poweroff
		Print(L"[%d] Thermal Prot - Poweroff (Sts, Hi, Lo) :\n", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"    0x%02X ", buf[0]);
		}
		buf[0] = 0x03;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
			Print(L"Error !! ");
		}
		else
		{
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}

			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
			{
				Print(L"Error !! ");
			}
			else
			{
				data16 = *(uint16_t*)buf;
				Print(L"0x%04X = %d = %3d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
			}
		}
		Print(L"\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void ThermalProtectShutdownSW(void)
{
	uint8_t i, set_flag = 0;
	uint8_t buf[32];
	uint16_t data16, h_limit = 70, l_limit = 50;

	clrscr();
	while(1)
	{
		gotoxy(0, 0);
		Print(L"Test Thermal Portect - Shutdown. \n\n");
		ConsoleCurrRow = 2;
		
		for(i=0; i<4; i++)
		{
			//set Shutdown limit
			if((set_flag & (0x01 << i)) == 0x00)
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				//Temperature available ?
				if((buf[0] & 0x1) == 0)
				{
					continue;
				}
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
				//Protection Event available ?
				if(((buf[0] & 0x01) == 0) || ((buf[0] & 0x02) == 0))
				{
					continue;
				}

				//Protection Event Type
				buf[0] = 0x01;
				if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Set Event Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpss_end;
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
						goto tpss_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
					}

					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(70~100 Default:70 C):", 0, 3);
					if((h_limit < 70) || (h_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//h_limit = 70;
					}
					
					l_limit = (uint8_t) get_number_stdin(L"Enter Low Limit(0~100 Default:50 C):", 0, 3);
					if((l_limit < 0) || (l_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//l_limit = 50;
					}

					if(h_limit > l_limit)
					{
						h_limit = h_limit * 10 + 2731;
						l_limit = l_limit * 10 + 2731;
					}
					else
					{
						console_color(RED, BLACK);
						Print(L"Low Limit Must Less Than High Limit !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//set default limit
					buf[0] = 0x8D;
					buf[1] = 0x0F;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpss_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
					break;

				reinput:
					Print(L"Press any key to continue \n");
					GetKey();

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
				}

				ConsoleCurrRow = 2 + (i * 5);
				gotoxy(0, ConsoleCurrRow);
				Print(L"Item:%d Protect Temerature. SRC:%02X \n", i, 0);
				ConsoleCurrRow++;
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpss_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Shutdown, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpss_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;

				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
					goto tpss_end;
				}
				else
				{
					if(buf[0] > 0x1F)
					{
						Print(L"Type Error! (0x%x)", buf[0]);
					}
					else
					{
						Print(L"%s_Temperature", *(gThermalType + buf[0]));
					}
				}

				set_flag |= 0x01 << i;
			}

			ConsoleCurrRow = 4 + (i * 5);
			gotoxy(25, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				goto tpss_end;
			}
			else
			{
				data16 = *(uint16_t *)buf;
				Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
			}
			ConsoleCurrRow++;

			gotoxy(0, ConsoleCurrRow);
			Print(L"--------------------------------------------------\n");
			ConsoleCurrRow += 2;
		}
		
		if(set_flag == 0)
		{
			console_color(RED, BLACK);
			Print(L"Thermal Portect Non-available !!");
			console_color(LIGHTGRAY, BLACK);
			goto tpss_end;
		}

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto tpss_endend;
		}
	}

tpss_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

tpss_endend:
	NULL;
}

void ThermalProtectThrottleSW(void)
{
	uint8_t i, set_flag = 0, cpu_flag = 0, throttle_flag = 0;
	uint8_t buf[32], cpu_vendor[32];
	uint16_t data16, h_limit = 70, l_limit = 50;
	uint16_t device_id = 0;
	uint32_t buf32[4];
	uint32_t family = 0, model = 0, address = 0, reg_bak = 0, data_bak = 0;
	
	MSR_IA32_THERM_STATUS_REGISTER therm_msr;

	buf32[0] = buf32[2] = 0;
	AsmCpuid(0, &buf32[0], &buf32[1], &buf32[2], &buf32[3]);
	*(uint32_t*)&cpu_vendor[0] = buf32[1];
	*(uint32_t*)&cpu_vendor[4] = buf32[3];	
	*(uint32_t*)&cpu_vendor[8] = buf32[2];
	cpu_vendor[12] = '\0';
	
	if(strstr(cpu_vendor, "Intel"))//intel
	{
		cpu_flag = 1;
		therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
		therm_msr.Bits.PROCHOT_FORCEPR_Log = 0;		//clean this flag
		AsmWriteMsr64(MSR_IA32_THERM_STATUS, therm_msr.Uint64);
	}
	else//amd
	{
		cpu_flag = 0;
		buf32[0] = buf32[1] = buf32[2] = buf32[3] = 0;
		AsmCpuid(1, &buf32[0], &buf32[1], &buf32[2], &buf32[3]);

		family = CPUIDTOFAMILY(buf32[0]);
		model = CPUIDTOMODEL(buf32[0]);

		//initial and collect PCI infomation
		if(EFIPCIInit())
		{
			Print(L"PCI Initial Fail !! \n\n");
			goto tpts_end;
		}

		switch(family)
		{
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x14:
			case 0x15:
			case 0x16:
			{
				//find operating device
				if(PciFindOperatingBusDevFun(0, 18, 3))
				{
					Print(L"Can't Find Device, Bus:0 Device:18 Function:3. \n\n");
					goto tpts_end;
				}
				
				//backup pci config 0x64 data
				PCIRWAMDSMN(SMN_DATA_REG, &data_bak, 0, EfiPciWidthUint8);
				
				break;
			}
			case 0x17:
			{
				if((model >= 0) && (model <=0x0F))
				{
					device_id = DEVICEID_AMD_HOSTB17H;
				}
				else if((model >= 10) && (model <=0x1F))
				{
					device_id = DEVICEID_AMD_F17M11B0;
				}
				
				//find operating device
				if(PciFindOperatingVenDev(VENDORID_AMD, device_id))
				{
					Print(L"Can't Find Device, Vendor:%x Device:%x. \n\n", VENDORID_AMD, device_id);
					goto tpts_end;
				}

				//backup pci config 0x60 data
				PCIRWAMDSMN(SMN_ADDR_REG, &reg_bak, 0, EfiPciWidthUint32);

				//set pci config 0x60 to SMUTHM base address
				address = AMD17H_SMUTHM + AMD17H_THM_TCON_HTC;
				PCIRWAMDSMN(SMN_ADDR_REG, &address, 1, EfiPciWidthUint32);

				//backup pci config 0x64 data
				PCIRWAMDSMN(SMN_DATA_REG, &data_bak, 0, EfiPciWidthUint16);

				//set PROCHOT source to EXTERNAL_PROCHOT(bit 14:12), and enable HTC feature(bit 0)
				buf32[0] = (data_bak & 0x8FF) | (1 << 12) | 0x01;
				PCIRWAMDSMN(SMN_DATA_REG, &buf32[0], 1, EfiPciWidthUint16);
				
				break;
			}
			case 0x0f:
			default:
			{
				Print(L"This AMD CPU, Family:%x Model:%x, Not Support. \n\n", family, model);
				goto tpts_end;
				break;
			}
		}
	}

	clrscr();
	while(1)
	{
		gotoxy(0, 0);
		Print(L"Test Thermal Portect - Throttle. \n\n");
		ConsoleCurrRow = 2;
		
		for(i=0; i<4; i++)
		{
			//set Throttle limit
			if((set_flag & (0x01 << i)) == 0x00)
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				//Temperature available ?
				if((buf[0] & 0x1) == 0)
				{
					continue;
				}
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
				//Protection Event available ?
				if(((buf[0] & 0x01) == 0) || ((buf[0] & 0x04) == 0))
				{
					continue;
				}

				//Protection Event Type
				buf[0] = 0x02;
				if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Set Event Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpts_end;
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
						goto tpts_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
					}

					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(70~100 Default:70 C):", 0, 3);
					if((h_limit < 70) || (h_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//h_limit = 70;
					}
					
					l_limit = (uint8_t) get_number_stdin(L"Enter Low Limit(0~100 Default:50 C):", 0, 3);
					if((l_limit < 0) || (l_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//l_limit = 50;
					}

					if(h_limit > l_limit)
					{
						h_limit = h_limit * 10 + 2731;
						l_limit = l_limit * 10 + 2731;
					}
					else
					{
						console_color(RED, BLACK);
						Print(L"Low Limit Must Less Than High Limit !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//set default limit
					buf[0] = 0x8D;
					buf[1] = 0x0F;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tpts_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
					break;

				reinput:
					Print(L"Press any key to continue \n");
					GetKey();

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
				}

				ConsoleCurrRow = 2 + (i * 5);
				gotoxy(0, ConsoleCurrRow);
				Print(L"Item:%d Protect Temerature. SRC:%02X \n", i, 0);
				ConsoleCurrRow++;
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpts_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Throttle, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tpts_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
					goto tpts_end;
				}
				else
				{
					if(buf[0] > 0x1F)
					{
						Print(L"Type Error! (0x%x)", buf[0]);
					}
					else
					{
						Print(L"%s_Temperature", *(gThermalType + buf[0]));
					}
				}

				set_flag |= 0x01 << i;
			}

			ConsoleCurrRow = 4 + (i * 5);
			gotoxy(25, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				goto tpts_end;
			}
			else
			{
				if(cpu_flag)//intel
				{
					therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
					if(therm_msr.Bits.PROCHOT_FORCEPR_Event)		//check throttle active
					{
						throttle_flag = 1;
					}
					else if((therm_msr.Bits.PROCHOT_FORCEPR_Event == 0))// && (therm_msr.Bits.PROCHOT_FORCEPR_Log == 1))
					{
						throttle_flag = 0;
					}
				}
				else//amd
				{
					if(PCIGetAMDThrottleStatus(family))		//check throttle active
					{
						throttle_flag = 1;
					}
					else
					{
						throttle_flag = 0;
					}
				}

				data16 = *(uint16_t *)buf;
				Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
			}
			ConsoleCurrRow++;

			gotoxy(0, ConsoleCurrRow);
			Print(L"--------------------------------------------------\n");
			ConsoleCurrRow += 2;
		}
		
		if(set_flag == 0)
		{
			console_color(RED, BLACK);
			Print(L"Thermal Portect Non-available !!");
			console_color(LIGHTGRAY, BLACK);
			goto tpts_end;
		}

		if(throttle_flag)		//check throttle active
		{
			console_color(GREEN, BLACK);
			gotoxy(0, ConsoleCurrRow);
			Print(L"Throttle Enable ");
			console_color(LIGHTGRAY, BLACK);
		}
		else
		{
			console_color(RED, BLACK);
			gotoxy(0, ConsoleCurrRow);
			Print(L"Throttle Disable");
			console_color(LIGHTGRAY, BLACK);
		}

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			console_color(LIGHTGRAY, BLACK);

			if(cpu_flag)//intel
			{
				therm_msr.Uint64 = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
				therm_msr.Bits.PROCHOT_FORCEPR_Log = 0;		//clean this flag
				AsmWriteMsr64(MSR_IA32_THERM_STATUS, therm_msr.Uint64);
			}
			else//amd
			{
				//restore pci config
				PCIRestoreAMDThrottleConfig(family, reg_bak, data_bak);
			}
			
			goto tpts_endend;
		}
	}

tpts_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

tpts_endend:
	PciExit();
}

void ThermalProtectPowerOffSW(void)
{
	uint8_t i, set_flag = 0;
	uint8_t buf[32];
	uint16_t data16, h_limit = 70, l_limit = 50;

	clrscr();
	while(1)
	{
		gotoxy(0, 0);
		Print(L"Test Thermal Portect - Power Off. \n\n");
		ConsoleCurrRow = 2;
		
		for(i=0; i<4; i++)
		{
			//set Power Off limit
			if((set_flag & (0x01 << i)) == 0x00)
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				//Temperature available ?
				if((buf[0] & 0x1) == 0)
				{
					continue;
				}
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
				//Protection Event available ?
				if(((buf[0] & 0x01) == 0) || ((buf[0] & 0x08) == 0))
				{
					continue;
				}

				//Protection Event Type
				buf[0] = 0x03;
				if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Set Event Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tppos_end;
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
						goto tppos_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
					}

					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(70~100 Default:70 C):", 0, 3);
					if((h_limit < 70) || (h_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//h_limit = 70;
					}
					
					l_limit = (uint8_t) get_number_stdin(L"Enter Low Limit(0~100 Default:50 C):", 0, 3);
					if((l_limit < 0) || (l_limit > 100))
					{
						Print(L"Please Enter Correct Value \n");
						goto reinput;//l_limit = 50;
					}

					if(h_limit > l_limit)
					{
						h_limit = h_limit * 10 + 2731;
						l_limit = l_limit * 10 + 2731;
					}
					else
					{
						console_color(RED, BLACK);
						Print(L"Low Limit Must Less Than High Limit !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//set default limit
					buf[0] = 0x8D;
					buf[1] = 0x0F;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x02, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_THERM_PRO, i, 0x04, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}
					if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
						goto tppos_end;
					}
					if(buf[0] & 0x80)
					{
						console_color(RED, BLACK);
						Print(L"Invalid Parameter !! \n");
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
					break;

				reinput:
					Print(L"Press any key to continue \n");
					GetKey();

					//clran error messagee
					gotoxy(0,ConsoleCurrRow);
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             \n");
					Print(L"                                             ");
				}

				ConsoleCurrRow = 2 + (i * 5);
				gotoxy(0, ConsoleCurrRow);
				Print(L"Item:%d Protect Temerature. SRC:%02X \n", i, 0);
				ConsoleCurrRow++;
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x02, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tppos_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Power Off, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM_PRO, i, 0x04, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM_PRO, i, 0x00, 1, buf);
					goto tppos_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;

				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
					goto tppos_end;
				}
				else
				{
					if(buf[0] > 0x1F)
					{
						Print(L"Type Error! (0x%x)", buf[0]);
					}
					else
					{
						Print(L"%s_Temperature", *(gThermalType + buf[0]));
					}
				}

				set_flag |= 0x01 << i;
			}

			ConsoleCurrRow = 4 + (i * 5);
			gotoxy(26, ConsoleCurrRow);
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				goto tppos_end;
			}
			else
			{
				data16 = *(uint16_t *)buf;
				Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
			}
			ConsoleCurrRow++;

			gotoxy(0, ConsoleCurrRow);
			Print(L"--------------------------------------------------\n");
			ConsoleCurrRow += 2;
		}
		
		if(set_flag == 0)
		{
			console_color(RED, BLACK);
			Print(L"Thermal Portect Non-available !!");
			console_color(LIGHTGRAY, BLACK);
			goto tppos_end;
		}

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto tppos_endend;
		}
	}

tppos_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
	
tppos_endend:
	NULL;
}

void ThermalTestSW(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show Thermal Information. \n");
		Print(L"2. Test Thermal Protect - Shutdown. \n");
		Print(L"3. Test Thermal Protect - Throttle. \n");
		Print(L"4. Test Thermal Protect - Power Off. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '1':
			{
				ShowThermalInformationSW();
				break;
			}
			case '2':
			{
				ThermalProtectShutdownSW();
				break;
			}
			case '3':
			{
				ThermalProtectThrottleSW();
				break;
			}
			case '4':
			{
				ThermalProtectPowerOffSW();
				break;
			}
		}
	}
}

void ShowWatchDogInformationSW(void)
{
	uint8_t buf[32];
	uint16_t data16;

	clrscr();

	//Status
	Print(L"[-] Watchdog Module Status : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%02X", buf[0]);
	}
	Print(L"\n");

	//Event
	Print(L"[-] Watchdog Event         : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x01, 1, buf))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%02X", buf[0]);
	}
	Print(L"\n");

	//Delay Time
	Print(L"[-] Watchdog Delay Time    : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x02, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//Power Cycle Event
	Print(L"[-] Watchdog Power Cycle   : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x04, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//IRQ Event
	Print(L"[-] Watchdog IRQ           : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x06, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//Reset Event
	Print(L"[-] Watchdog Reset         : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x08, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//Pin Event
	Print(L"[-] Watchdog Pin           : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x0A, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//SCI Event
	Print(L"[-] Watchdog SCI           : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x0C, 2, (uint8_t *)&data16))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%08lX = %lu ms", data16, data16 * 100);
	}
	Print(L"\n");

	//IRQ Number
	rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x0E, 2, buf);
	Print(L"[-] Watchdog IRQ Number    : 0x%02X\n", buf[0]);

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

uint8_t WatchDogResetEventTimer(void)
{
	uint8_t buf[2]={0};
	
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x04, 2, buf))
	{
		goto wdret_end;
	}
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x06, 2, buf))
	{
		goto wdret_end;
	}
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x08, 2, buf))
	{
		goto wdret_end;
	}
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x0A, 2, buf))
	{
		goto wdret_end;
	}
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x0C, 2, buf))
	{
		goto wdret_end;
	}

	return 0;

wdret_end:
	return 1;
}

void WatchDogPowerCycleEventSW(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 50, p_time = 50;
	
	clrscr();
	Print(L"Test Watchdog - Power Cycle Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdpces_end;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((d_time == 0) || (d_time > 99))
	{
		d_time = 50;
	}
	ConsoleCurrRow++;
	
	p_time = get_number_stdin(L"Enter Power Cycle Event Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((p_time == 0) || (p_time > 99))
	{
		p_time = 50;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;

	//reset all event
	if(WatchDogResetEventTimer())
	{
		Print(L"PMC Communicate Fail !! Reset Event Time Fail !! \n\n");
		goto wdpces_end;
	}

	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x02, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdpces_end;
	}

	//set watchdog Power Cycle Event Time
	*(uint32_t*)buf = p_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x04, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Power Cycle Event Time Fail !! \n\n");
		goto wdpces_end;
	}

	//start watchdog
	if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_START))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdpces_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");
	
	for (i=1; i<=d_time; i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Power Cycle Event Time Fail !! \n\n");
				goto wdpces_end;
			}

			i = d_time;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", p_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=(p_time + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter: %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Power Cycle Event Time Fail !! \n\n");
				goto wdpces_end;
			}
			i = 1;
		}
	}

	console_color(YELLOW, RED);
	Print(L"Watchdog Fail. \n\n");
	console_color(LIGHTGRAY, BLACK);

	goto wdpces_endend;

wdpces_end:
	ShowSWProtocol(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);

wdpces_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogResetEventSW(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 50, r_time = 50;
	
	clrscr();
	Print(L"Test Watchdog - Reset Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdres_end;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((d_time == 0) || (d_time > 99))
	{
		d_time = 50;
	}
	ConsoleCurrRow++;
	
	r_time = get_number_stdin(L"Enter Reset Event Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((r_time == 0) || (r_time > 99))
	{
		r_time = 50;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;

	//reset all event
	if(WatchDogResetEventTimer())
	{
		Print(L"PMC Communicate Fail !! Reset Event Time Fail !! \n\n");
		goto wdres_end;
	}
	
	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x02, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdres_end;
	}

	//set watchdog Reset Event Time
	*(uint32_t*)buf = r_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x08, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Reset Event Time Fail !! \n\n");
		goto wdres_end;
	}

	//start watchdog
	if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_START))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdres_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");
	
	for (i=1; i<=d_time; i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Reset Event Time Fail !! \n\n");
				goto wdres_end;
			}

			i = d_time;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", r_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=(r_time + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter: %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Reset Event Time Fail !! \n\n");
				goto wdres_end;
			}
			i = 1;
		}
	}

	console_color(YELLOW, RED);
	Print(L"Watchdog Fail. \n\n");
	console_color(LIGHTGRAY, BLACK);

	goto wdres_endend;

wdres_end:
	ShowSWProtocol(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);

wdres_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogPinEventSW(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 50, p_time = 50;
	
	clrscr();
	Print(L"Test Watchdog - Pin Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdpes_endend;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((d_time == 0) || (d_time > 99))
	{
		d_time = 50;
	}
	ConsoleCurrRow++;
	
	p_time = get_number_stdin(L"Enter Pin Event Time(1~99 Default:50   unit:100ms):", 0, 2);
	if((p_time == 0) || (p_time > 99))
	{
		p_time = 50;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;

	//reset all event
	if(WatchDogResetEventTimer())
	{
		Print(L"PMC Communicate Fail !! Reset Event Time Fail !! \n\n");
		goto wdpes_end;
	}
	
	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x02, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdpes_end;
	}

	//set watchdog Pin Event Time
	*(uint32_t*)buf = p_time;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_WDT, 0xFF, 0x0A, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Set Pin Event Time Fail !! \n\n");
		goto wdpes_end;
	}

	//start watchdog
	if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_START))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdpes_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");
	
	for (i=1; i<=d_time; i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Pin Event Time Fail !! \n\n");
				goto wdpes_end;
			}

			i = d_time;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", p_time * 100);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=(p_time + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter: %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			if(SWProtocolOnlyWritePMC(RDC_SW_CMD_WDT_REF))
			{
				Print(L"PMC Communicate Fail !! Reset Reset Event Time Fail !! \n\n");
				goto wdpes_end;
			}
			i = 1;
		}
	}

	goto wdpes_endend;

wdpes_end:
	ShowSWProtocol(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);

wdpes_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogTestSW(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show Watchdog Information. \n");
		Print(L"2. Test Watchdog - Power Cycle Event. \n");
		Print(L"3. Test Watchdog - Reset Event. \n");
		Print(L"4. Test Watchdog - Watchdog Pin Event. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ShowWatchDogInformationSW();
				break;
			}
			case '2':
			{
				WatchDogPowerCycleEventSW();
				break;
			}
			case '3':
			{
				WatchDogResetEventSW();
				break;
			}
			case '4':
			{
				WatchDogPinEventSW();
				break;
			}
		}
	}
}

void ShowFanInformationSW(void)
{
	uint8_t i, buf[32];
	uint16_t data16;
	uint32_t data32;

	clrscr();
	
	Print(L"[-] Fan Module Status  : ");
	for(i=0; i<4; i++)
	{
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
	}
	Print(L"\n");

	for(i=0; i<4; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf);
		// available ?
		if((buf[0] & 0x01) == 0)
		{
			continue;
		}

		//Type
		Print(L"[%d] Fan Type           : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x16, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//Control
		Print(L"[%d] Fan Control        : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x02, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//PWM Duty
		Print(L"[%d] Fan PWM Duty       : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x03, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", (buf[0]&0x7F), (buf[0]&0x7F));
			
		}
		Print(L"\n");

		//PWM Polarity
		Print(L"[%d] Fan PWM Polarity   : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x03, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"%s ", (buf[0]&0x80)?L"Invert":L"Normal");
			
		}
		Print(L"\n");

		//PWM Frequency
		Print(L"[%d] Fan PWM Frequency  : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x04, 4, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data32 = *(uint32_t *)buf;
			Print(L"0x%08X = %d Hz ", data32, data32);
		}
		Print(L"\n");

		//PWM High
		Print(L"[%d] Fan PWM High Limit : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x0E, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", buf[0], buf[0]);
		}
		Print(L"\n");

		//PWM Low
		Print(L"[%d] Fan PWM Low Limit  : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x0F, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", buf[0], buf[0]);
		}
		Print(L"\n");

		//RPM High
		Print(L"[%d] Fan RPM High Limit : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x10, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %4d RPM ", data16, data16);
		}
		Print(L"\n");

		//RPM Low
		Print(L"[%d] Fan RPM Low Limit  : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x12, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %4d RPM ", data16, data16);
		}
		Print(L"\n");

		//Current RPM
		Print(L"[%d] Fan Current RPM    : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x14, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %4d RPM ", data16, data16);
		}
		Print(L"\n");

		//Temperature Limit
		Print(L"[%d] Fan Temperature Limit (Hi, Lo, Stop) : \n", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x08, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"    0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x0A, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d = %3d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x0C, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d = %3d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		Print(L"\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void FanOnOffTestSW(void)
{
	uint8_t flag = 0;
	uint8_t i, j, tmp, buf[32];
	uint8_t fanoffdelay = 5, fanondelay = 2;
	uint16_t data16 = 0;
	
	clrscr();

	gotoxy(0, 0);
	Print(L"Test Fan - Auto On Off. \n\n");
	ConsoleCurrRow = 2;
	
	for(i=0; i<4; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf);
		// available ?
		if((buf[0] & 0x01) == 0)
		{
			continue;
		}
		flag = 1;

		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x02, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan Control Mode Fail !! \n\n");
			goto foots_end;
		}
		tmp = buf[0];

		gotoxy(0, ConsoleCurrRow);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x16, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan Type Fail !! \n\n");
			goto foots_end;
		}
		else
		{
			if(buf[0] > 0x1F)
			{
				Print(L"Type Error!! (0x%x) \n", buf[0]);
			}
			else
			{
				Print(L"%s_Fan : \n", *(gFanType + buf[0]));
			}
		}
		ConsoleCurrRow++;
		
		//turn off fan
		gotoxy(10, ConsoleCurrRow);
		Print(L"Turn Off  - ");
		
		buf[0] = tmp & 0xFC;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, i, 0x02, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Off Fail !! \n\n");
			goto foots_end;
		}
		for(j=0; j<fanoffdelay; j++)
		{
			gotoxy(22, ConsoleCurrRow);
			Print(L"wait %02d sec.", fanoffdelay - j);
			usleep(1000 * 1000);	// 1 sec
		}

		gotoxy(22, ConsoleCurrRow);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x14, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan RPM Fail !! \n\n");
			goto foots_end;
		}
		else
		{
			data16 = *(uint16_t*)buf;
			console_color(YELLOW, BLACK);
			Print(L"Speed:%4d RPM ", *(uint16_t*)buf);
			console_color(LIGHTGRAY, BLACK);
		}
		ConsoleCurrRow++;

		//fan full
		gotoxy(10, ConsoleCurrRow);
		Print(L"Turn Full - ");

		buf[0] = (tmp & 0xFC) | 0x01;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, i, 0x02, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Full Fail !! \n\n");
			goto foots_end;
		}
		for(j=0; j<fanondelay; j++)
		{
			gotoxy(22, ConsoleCurrRow);
			Print(L"wait %02d sec.", fanondelay - j);
			usleep(1000 * 1000);	// 1 sec
		}

		gotoxy(22, ConsoleCurrRow);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x14, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan RPM Fail !! \n\n");
			goto foots_end;
		}
		else
		{
			data16 = *(uint16_t*)buf;
			console_color(YELLOW, BLACK);
			Print(L"Speed:%4d RPM ", *(uint16_t*)buf);
			console_color(LIGHTGRAY, BLACK);
		}
		ConsoleCurrRow++;

		//restore fan control mode
		buf[0] = tmp;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, i, 0x02, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore Fan Control Mode Fail !! \n\n");
			goto foots_end;
		}

		gotoxy(0, ConsoleCurrRow);
		Print(L"--------------------------------------------------\n");
		ConsoleCurrRow++;
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Fan Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto foots_endend;

foots_end:
	ShowSWProtocol(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf);

foots_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void FanManualTestSW(void)
{
	uint8_t fan = 0, duty = 0, duty_bak = 0, type = 0, control_tmp = 0, polarity_tmp = 0, duty_tmp = 0, restore = 0;
	uint8_t i, flag = 0, tmp, buf[32];
	uint16_t data16;
	int key;
	
	clrscr();

	gotoxy(0, 0);
	Print(L"Test Fan - Manual. \n\n");
	ConsoleCurrRow = 2;

	Print(L"Available Fan : ");
	for(i=0; i<4; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf);
		// available ?
		if((buf[0] & 0x01) == 0)
		{
			continue;
		}
		flag = 1;
		Print(L"Fan%d  ", i);
	}
	if(flag == 0)
	{
		gotoxy(0, ConsoleCurrRow);
		console_color(RED, BLACK);
		Print(L"Fan Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto fmts_endend;
	}
	Print(L"\n");
	ConsoleCurrRow++;
	
	gotoxy(0, ConsoleCurrRow);
	fan = (uint8_t)get_number_stdin(L"Select Fan(0~3:Fan0  Fan1  Fan2  Fan3)(Default:0):", 0, 1);
	if(fan < 0 || fan >3)
	{
		fan = 0;
	}

	rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x01) == 0)
	{
		Print(L"Fan %d Is Unavailable !! \n\n", fan);
		goto fmts_endend;
	}

	//backup control mode
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x02, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Read Fan Control Mode Fail !! \n\n");
		goto fmts_end;
	}
	control_tmp = buf[0];
	
	//backup pwm duty
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x03, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
		goto fmts_end;
	}
	polarity_tmp = buf[0] & 0x80;
	duty_tmp = buf[0] & 0x7F;
	restore = 1;
		
restart:
	//clean screen
	gotoxy(0, 5);
	Print(L"                                                                                ");
	gotoxy(0, 7);
	Print(L"                                                                                ");
	for(i=0; i<3; i++)
	{
		gotoxy(0, i + 9);
		Print(L"                                                                                ");
	}
	gotoxy(0, ConsoleHeight - 2);
	Print(L"                                                                                ");

	//choice mode
	gotoxy(0, 5);
	type = (uint8_t)get_number_stdin(L"Enter Fan Control Mode(0:Stop 1:Full 2:Manual 3:Auto)(Default:3):", 0, 1);
	if(type < 0 || type >3)
	{
		type = 3;
	}

	//set control mode
	buf[0] = type;
	if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, fan, 0x02, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Set Fan Control Mode Fail !! \n\n");
		goto fmts_end;
	}
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press 'R' to restart test, or Press ESC to return.");

	if(type == 2)
	{
		gotoxy(0, ConsoleHeight - 2);
		Print(L"Press '+' to increase, Press '-' to decrease");
	}
	else if(type == 3)
	{
		//Temperature Limit
		gotoxy(0,7);
		Print(L"Fan Temperature Limit (Hi, Lo, Stop) : ");
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x08, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"%3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x0A, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"%3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x0C, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"%3d.%d C ", (data16-2731)/10, abs(data16-2731)%10);
		}
		Print(L"\n");
	}

	while(1)
	{
		//duty
		gotoxy(0,9);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x03, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto fmts_end;
		}
		Print(L"Fan PWM Duty : %d %% \n", (buf[0]&0x7F));
		tmp = buf[0] & 0x80;
		duty = buf[0] & 0x7F;
		
		//Current RPM
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, fan, 0x14, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Current RPM Fail !! \n\n");
			goto fmts_end;
		}
		data16 = *(uint16_t *)buf;
		Print(L"Fan Current RPM : %4d RPM \n", data16);

		key = bioskey(1);
		if((key == 'r') || (key == 'R'))
		{
			goto restart;
		}
		if(CheckESC(key))
		{
			goto fmts_endendend;
		}
		usleep(200 * 1000);
		
		if(type == 2)
		{
			if(key == '+')
			{
				if(duty < 100)
				{
					duty++;
				}
			}
			if(key == '-')
			{
				if(duty > 0)
				{
					duty--;
				}
			}
			if(duty != duty_bak)
			{
				buf[0] = tmp | duty;
				if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, fan, 0x03, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Set PWM Duty Fail !! \n\n");
					goto fmts_end;
				}
				duty_bak = duty;
			}
		}
		else if(type == 3)
		{
			//Temperature
			for(i=0; i<4; i++)
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				// available ?
				if((buf[0] & 0x1) == 0)
				{
					continue;
				}

				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x01, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
					goto fmts_endend;
				}
				else
				{
					if(buf[0] > 0x1F)
					{
						Print(L"Type Error! (0x%x)", buf[0]);
					}
					else
					{
						Print(L"%s_Temperature", *(gThermalType + buf[0]));
					}
				}

				if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x02, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
					ShowSWProtocol(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
					goto fmts_endend;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
			}
		}
	}

fmts_end:
	ShowSWProtocol(RDC_SW_CMD_RD_FAN, fan, 0x00, 1, buf);

fmts_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.                              ");
	while(!CheckESC(GetKey())){}

fmts_endendend:
	//restore fan control mode & pwm duty
	if(restore)
	{
		buf[0] = control_tmp;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, fan, 0x02, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Control Mode Fail !! \n\n");
		}
		
		if(type == 2)
		{
			buf[0] = polarity_tmp | duty_tmp;
			if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_FAN, fan, 0x03, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Set PWM Duty Fail !! \n\n");
			}
		}
	}
}

void FanTestSW(void)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"1. Show Fan Information. \n");
		Print(L"2. Test Fan - Auto On Off. \n");
		Print(L"3. Test Fan - Manual. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ShowFanInformationSW();
				break;
			}
			case '2':
			{
				FanOnOffTestSW();
				break;
			}
			case '3':
			{
				FanManualTestSW();
				break;
			}
		}
	}
}

uint8_t GPIOSWProtocolReadStatus(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf, uint8_t flag)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return RDCSWProtocolGPIOStatusRead(&prot, flag);
}

uint8_t GPIOSWProtocolWriteStatus(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf, uint8_t flag)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return RDCSWProtocolGPIOStatusWrite(&prot, flag);
}

void ShowGPIOInformationSW(void)
{
	uint8_t i, buf[32];
	uint16_t a_flag = 0;
	uint16_t bio;

	clrscr();
	
	Print(L"[-] OEM GPIO Availability : ");
	for(i=0; i<12; i++)
	{
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_GPIO_STS, 0xFF, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
			if(i == 5)
			{
				Print(L"\n                            ");
			}
			if((buf[0]&0x01) == 0x01)
			{
				a_flag |= 0x0001 << i;
			}
		}
	}
	Print(L"\n");

	for(i=0; i<2; i++)
	{
		if((a_flag & (0x0001 << i)) == 0x0000)
		{
			continue;
		}
		
		if(RDCGPIOInit(&bio))
		{
			Print(L"Get GPIO base address fail !! \n\n");
			goto end;
		}
		
		Print(L"[%d] OEM GPIO Group0       : Dir = ", i);
		GPIODirectionRW(bio, i, buf, GPIO_READ);
		Print(L"0x%02X  ", buf[0]);
		
		Print(L"Dat = ");
		GPIODataRW(bio, i, buf, GPIO_READ);
		Print(L"0x%02X  ", buf[0]);
		Print(L"\n");
	}

	for(i=0; i<2; i++)
	{
		if((a_flag & (0x0004 << i)) == 0x0000)
		{
			continue;
		}
		
		Print(L"[%d] OEM GPIO Group2       : Dir = ", i);
		if(GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, 2, i, 0xFF, buf, 1))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X  ", buf[0]);
		}
		Print(L"Dat = ");
		if(GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, 2, i, 0xFF, buf, 0))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X  ", buf[0]);
		}
		Print(L"\n");
	}

	for(i=0; i<8; i++)
	{
		if((a_flag & (0x0010 << i)) == 0x0000)
		{
			continue;
		}
		
		Print(L"[%d] OEM GPIO Group3       : Dir = ", i);
		if(GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, 3, i, 0xFF, buf, 1))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X  ", buf[0]);
		}
		Print(L"Dat = ");
		if(GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, 3, i, 0xFF, buf, 0))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X  ", buf[0]);
		}
		Print(L"\n");
	}

end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void GPIOInputOutputTestSW(void)
{
	uint8_t i, j, result, count = 0, buf[2];
	uint8_t dir_bak, level_bak;
	uint8_t group, port;
	uint16_t a_flag = 0;
	uint16_t bio;
	UINTN y_back;
	
	clrscr();
	Print(L"GPIO Input Output Test. \n");

	for(i=0; i<12; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_GPIO_STS, 0xFF, i, 1, buf);
		// available ?
		if((buf[0]&0x01) == 0x01)
		{
			a_flag |= 0x0001 << i;
		}
	}
	if(a_flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"GPIO Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto gpioiots_end;
	}

	//group 0
	for(i=0; i<2; i++)
	{
		if((a_flag & (0x0001 << i)) == 0x0000)
		{
			continue;
		}
		
		if(RDCGPIOInit(&bio))
		{
			Print(L"Get GPIO base address fail !! \n\n");
			goto gpioiots_end;
		}
		
		if(count)
		{
			Print(L"\n\n");
			Print(L"Press any key to continue next eight pin \n\n");
			GetKey();
		}

		clrscr();
		Print(L"GPIO Input Output Test. \n");

		Print(L"Please connect gpio as follow. \n");
		y_back = ConsoleCurrRow = 2;

		for(j=0; j<4; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			Print(L"GPIO%d  <-------->   GPIO%d", j, (j + 4));
			ConsoleCurrRow++;
		}

		gotoxy(0, ConsoleCurrRow);
		Print(L"Press any key to start test...");
		ConsoleCurrRow += 2;
		
		GetKey(); //wait key in
		
		//backup Direction
		GPIODirectionRW(bio, i, &dir_bak, GPIO_READ);
		//backup Level
		GPIODataRW(bio, i, &level_bak, GPIO_READ);
		
		//test low pin to high pin
		y_back = ConsoleCurrRow;
		for(j=0; j<4 ; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%d  -------->    GPIO%d", j, (j + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}

		//set direction
		buf[0] = 0x0F;
		GPIODirectionRW(bio, i, buf, GPIO_WRITE);

		//set data to high
		buf[0] = 0x0F;
		GPIODataRW(bio, i, buf, GPIO_WRITE);
		
		//read data
		GPIODataRW(bio, i, buf, GPIO_READ);
		result = buf[0];	//store test result of high
		usleep(1000);

		//set data to low
		buf[0] = 0xF0;
		GPIODataRW(bio, i, buf, GPIO_WRITE);
		
		//read data
		GPIODataRW(bio, i, buf, GPIO_READ);
		usleep(1000);

		ConsoleCurrRow = y_back;
		result >>= 4;
		buf[0] >>= 4;
		for(j=0; j<4; j++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[0] >> j) & 0x01) == 0x00) && (((result >> j) & 0x01) == 0x01))
			{
				console_color(GREEN, BLACK);
				Print(L"PASS!!");
			}
			else
			{
				console_color(RED, BLACK);
				Print(L"FAIL!!");
			}
			ConsoleCurrRow++;
		}
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		//test high pin to low pin
		y_back = ConsoleCurrRow;
		for(j=0; j<4 ; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%-2d  <--------    GPIO%-2d", j, (j + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}

		//set direction
		buf[0] = 0xF0;
		GPIODirectionRW(bio, i, buf, GPIO_WRITE);

		//set data to high
		buf[0] = 0xF0;
		GPIODataRW(bio, i, buf, GPIO_WRITE);

		//read data
		GPIODataRW(bio, i, buf, GPIO_READ);
		result = buf[0];	//store test result of high
		usleep(1000);

		//set data to low
		buf[0] = 0x0F;
		GPIODataRW(bio, i, buf, GPIO_WRITE);
		
		//read data
		GPIODataRW(bio, i, buf, GPIO_READ);
		usleep(1000);

		ConsoleCurrRow = y_back;
		for(j=0; j<4; j++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[0] >> j) & 0x01) == 0x00) && (((result >> j) & 0x01) == 0x01))
			{
				console_color(GREEN, BLACK);
				Print(L"PASS!!");
			}
			else
			{
				console_color(RED, BLACK);
				Print(L"FAIL!!");
			}
			ConsoleCurrRow++;
		}
		console_color(LIGHTGRAY, BLACK);
		
		//restore Direction
		GPIODirectionRW(bio, i, &dir_bak, GPIO_WRITE);
		//restore Level
		GPIODataRW(bio, i, &level_bak, GPIO_WRITE);

		count++;
	}
	
	//group 2 and group 3
	for(i=2; i<12; i++)
	{
		if((a_flag & (0x0001 << i)) == 0x0000)
		{
			continue;
		}

		if(i > 3)
		{
			group = 3;
			port = i - 4;
		}
		else
		{
			group = 2;
			port = i - 2;
		}

		if(count)
		{
			Print(L"\n\n");
			Print(L"Press any key to continue next eight pin \n\n");
			GetKey();
		}

		clrscr();
		Print(L"GPIO Input Output Test. \n");

		Print(L"Please connect gpio as follow. \n");
		y_back = ConsoleCurrRow = 2;

		for(j=0; j<4; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			Print(L"GPIO%d  <-------->   GPIO%d", j, (j + 4));
			ConsoleCurrRow++;
		}

		gotoxy(0, ConsoleCurrRow);
		Print(L"Press any key to start test...");
		ConsoleCurrRow += 2;
		
		GetKey(); //wait key in
		
		//backup Direction
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, &dir_bak, 1);
		//backup Level
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, &level_bak, 0);

		//test low pin to high pin
		y_back = ConsoleCurrRow;
		for(j=0; j<4 ; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%d  -------->    GPIO%d", j, (j + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}

		//set direction
		buf[0] = 0x0F;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 1);

		//set data to high
		buf[0] = 0x0F;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 0);
		
		//read data
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, buf, 0);
		result = buf[0];	//store test result of high
		usleep(1000);

		//set data to low
		buf[0] = 0xF0;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 0);
		
		//read data
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, buf, 0);
		usleep(1000);

		ConsoleCurrRow = y_back;
		result >>= 4;
		buf[0] >>= 4;
		for(j=0; j<4; j++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[0] >> j) & 0x01) == 0x00) && (((result >> j) & 0x01) == 0x01))
			{
				console_color(GREEN, BLACK);
				Print(L"PASS!!");
			}
			else
			{
				console_color(RED, BLACK);
				Print(L"FAIL!!");
			}
			ConsoleCurrRow++;
		}
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		//test high pin to low pin
		y_back = ConsoleCurrRow;
		for(j=0; j<4 ; j++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%-2d  <--------    GPIO%-2d", j, (j + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}

		//set direction
		buf[0] = 0xF0;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 1);

		//set data to high
		buf[0] = 0xF0;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 0);

		//read data
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, buf, 0);
		result = buf[0];	//store test result of high
		usleep(1000);

		//set data to low
		buf[0] = 0x0F;
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, buf, 0);
		
		//read data
		GPIOSWProtocolReadStatus(RDC_SW_CMD_RD_GPIO, group, port, 0xFF, buf, 0);
		usleep(1000);

		ConsoleCurrRow = y_back;
		for(j=0; j<4; j++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[0] >> j) & 0x01) == 0x00) && (((result >> j) & 0x01) == 0x01))
			{
				console_color(GREEN, BLACK);
				Print(L"PASS!!");
			}
			else
			{
				console_color(RED, BLACK);
				Print(L"FAIL!!");
			}
			ConsoleCurrRow++;
		}
		console_color(LIGHTGRAY, BLACK);
		
		//restore Direction
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, &dir_bak, 1);
		//restore Level
		GPIOSWProtocolWriteStatus(RDC_SW_CMD_WR_GPIO, group, port, 0xFF, &level_bak, 0);

		count++;
	}

gpioiots_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void GPIOTestSW(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show GPIO Information. \n");
		Print(L"2. GPIO Input Output Test. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ShowGPIOInformationSW();
				break;
			}
			case '2':
			{
				GPIOInputOutputTestSW();
				break;
			}
		}
	}
}

void ShowPanelInformationSW(void)
{
	uint8_t i, buf[32];
	uint32_t data32;

	clrscr();
	
	for(i=0; i<2; i++)
	{
		Print(L"[%d] Panel Module Status       : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		if ((buf[0] & 0x1) == 0)
		{
			continue;
		}

		Print(L"[%d] Backlight Control Mode    : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x01, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		Print(L"[%d] Backlight Delay ON        : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x02, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d ms ", buf[0], buf[0] * 5);
		}
		Print(L"\n");

		Print(L"[%d] Backlight Delay OFF       : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x03, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d ms ", buf[0], buf[0] * 5);
		}
		Print(L"\n");

		Print(L"[%d] Backlight PWM Duty Cycle  : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x04, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d%% ", buf[0], buf[0]);
		}
		Print(L"\n");

		Print(L"[%d] Backlight PWM Frequency   : ", i);
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x05, 4, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data32 = *(uint32_t *)buf;
			Print(L"0x%08lX = %lu Hz ", data32, data32);
		}
		Print(L"\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightOnOffTestSW(void)
{
	uint8_t i, j, tmp, buf[32];
	uint8_t flag = 0;
	
	clrscr();
	Print(L"Panel Backlight On/Off Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;

		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x01, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Switch Fail !! \n\n");
			goto pboots_end;
		}
		tmp = buf[0];
		
		//turn on
		gotoxy(0, ConsoleCurrRow);
		Print(L"Backlight %d On/Off Test \n", (uint16_t)i);
		ConsoleCurrRow++;
		console_color(YELLOW, BLACK);
		Print(L"Backlight %d Turn On 5 sec \n", (uint16_t)i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;
		
		buf[0] = tmp | 0x80;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x01, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Write Switch Fail !! \n\n");
			goto pboots_end;
		}
		for(j=1; j<6; j++)
		{
			gotoxy(0, ConsoleCurrRow);
			Print(L"Hold Time : %d sec \n", j);
			usleep(1000 * 1000);// 1sec
		}
		ConsoleCurrRow +=2 ;

		//turn off
		gotoxy(0, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"Backlight %d Turn Off 5 sec \n", (uint16_t)i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		buf[0] = tmp & 0x7F;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x01, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Write Switch Fail !! \n\n");
			goto pboots_end;
		}
		for(j=1; j<6; j++)
		{
			gotoxy(0, ConsoleCurrRow);
			Print(L"Hold Time : %d sec \n", j);
			usleep(1000 * 1000);// 1sec
		}
		ConsoleCurrRow +=2 ;

		//restore
		gotoxy(0, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"Backlight %d Will Restore To Original On/Off \n", (uint16_t)i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		gotoxy(0, ConsoleCurrRow);
		Print(L"--------------------------------------------------\n");
		ConsoleCurrRow += 2;

		buf[0] = tmp;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x01, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore Switch Fail !! \n\n");
			goto pboots_end;
		}
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Panel Backlight Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto pboots_endend;

pboots_end:
	ShowSWProtocol(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);

pboots_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightLevelAutoTestSW(void)
{
	uint8_t i, j, tmp;
	uint8_t buf[32];
	uint8_t flag = 0;

	clrscr();
	Print(L"Panel Backlight Level Auto Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;
		
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x04, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto pblats_end;
		}
		tmp = buf[0];

		//Increase level
		gotoxy(0, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"Panel Backlight %d Increase Level Every Second \n", i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;
		
		for(j=1; j<11; j++)
		{
			gotoxy(0, ConsoleCurrRow);
			Print(L"Panel Backlight %3d %%", j*10);
			
			buf[0] = 10 * j;
			if(buf[0] > 100)
			{
				buf[0] = 100;
			}
			if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x04, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Write PWM Duty Fail !! \n\n");
				goto pblats_end;
			}
			usleep(1000 * 1000);	// 1sec
		}
		Print(L"\n\n");
		ConsoleCurrRow += 2;

		//Decrease level
		gotoxy(0, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"Panel Backlight %d Decrease Level Every Second \n", i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;
		
		for(j=10; j>0; j--)
		{
			gotoxy(0, ConsoleCurrRow);
			Print(L"Panel Backlight %3d %%", j*10);
			
			buf[0] = 10 * j;
			if(buf[0] > 100)
			{
				buf[0] = 100;
			}
			if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x04, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Write PWM Duty Fail !! \n\n");
				goto pblats_end;
			}
			usleep(1000 * 1000);	// 1sec
		}
		Print(L"\n\n");
		ConsoleCurrRow += 2;

		//restore
		gotoxy(0, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"Backlight %d Will Restore To Original Level \n", (uint16_t)i);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		gotoxy(0, ConsoleCurrRow);
		Print(L"--------------------------------------------------\n");
		ConsoleCurrRow += 2;

		//restore level
		buf[0] = tmp;
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x04, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore PWM Duty Fail !! \n\n");
			goto pblats_end;
		}
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Panel Backlight Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto pblats_endend;

pblats_end:
	ShowSWProtocol(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);

pblats_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightLevelManualTestSW(void)
{
	uint8_t flag = 0;
	uint8_t i, tmp, buf[32];
	uint16_t level;

	clrscr();
	Print(L"Panel Backlight Level Manual Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;

		gotoxy(0, ConsoleCurrRow);
		Print(L"Panel Backlight %d \n", i);
		ConsoleCurrRow++;
		
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x04, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto pblmts_end;
		}
		tmp = buf[0];

		level = (uint16_t)get_number_stdin(L"Set Level Percent (0,10,20,30,40,50,60,70,80,90,100):", 0, 3);
		if(level > 100)
		{
			level = 100;
		}
		if(level % 10)
		{
			level = level / 10 *10;
		}
		ConsoleCurrRow++;
		
		//set level
		if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_PANEL, i, 0x04, 1, (uint8_t *)&level))
		{
			Print(L"PMC Communicate Fail !! Set Level Fail !! \n\n");
			goto pblmts_end;
		}

		console_color(YELLOW, BLACK);
		Print(L"Set Level To %3d %% \n", level);
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;

		Print(L"--------------------------------------------------\n");
		ConsoleCurrRow += 2;
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Panel Backlight Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto pblmts_endend;

pblmts_end:
	ShowSWProtocol(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);

pblmts_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelTestSW(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show Panel Information. \n");
		Print(L"2. Panel Backlight On/Off Test. \n");
		Print(L"3. Panel Backlight Level Auto Test. \n");
		Print(L"4. Panel Backlight Level Manual Test. \n");
		//Print(L"5. Panel Backlight Control Mode Test. \n");
		Print(L"Please Input: \n");
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '1':
			{
				ShowPanelInformationSW();
				break;
			}
			case '2':
			{
				PanelBacklightOnOffTestSW();
				break;
			}
			case '3':
			{
				PanelBacklightLevelAutoTestSW();
				break;
			}
			case '4':
			{
				PanelBacklightLevelManualTestSW();
				break;
			}
			case '5':
			{
				//PanelBacklightControlModeTestSW();
				break;
			}
		}
	}
}

void ShowCaseOpenSW(void)
{
	uint8_t buf[64];

	clrscr();

	Print(L"[-] Case Open Module Status : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x00, 1, buf))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%02X ", buf[0]);
	}
	Print(L"\n");

	Print(L"[-] Case Open Control       : ");
	if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x01, 1, buf))
	{
		Print(L"Error !! ");
	}
	else
	{
		Print(L"0x%02X ", buf[0]);
	}
	Print(L"\n");

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void CaseOpenTestExecuteSW()
{
	uint8_t open_flag = 0;
	uint8_t buf[2];
	
	clrscr();
	while(1)
	{
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");
		gotoxy(0, 0);
		Print(L"Case Open Test. \n\n");
		
		rdc_sw_ec_read_prot(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x00, 1, buf);
		// available ?
		if((buf[0] & 0x1) == 0)
		{
			console_color(RED, BLACK);
			Print(L"Case Open Not Available !! \n\n");
			console_color(LIGHTGRAY, BLACK);
			while(!CheckESC(GetKey())){}
			goto cotes_endend;
		}

		//check case not open
		console_color(YELLOW, BLACK);
		Print(L"Check Case Open Status : ");
		if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x01, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Case Open Flag Fail !! \n\n");
			goto cotes_end;
		}
		if((buf[0] & 0x01) == 0x00)
		{
			console_color(RED, BLACK);
			Print(L"Case Not Opened !! \n");
		}
		else
		{
			open_flag = 1;
			console_color(GREEN, BLACK);
			Print(L"Case Opened !!     \n");
		}
		console_color(LIGHTGRAY, BLACK);
		Print(L"\n");

		if(open_flag)
		{
			Print(L"Press Any Key To Clear Case Open Flag And Continue Test... \n");
			GetKey();

			//clean Case Open flag
			buf[0] = 0x01;
			if(rdc_sw_ec_write_prot(RDC_SW_CMD_WR_CASEOPEN, 0xFF, 0x01, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Clean Case Open Flag Fail !! \n\n");
				goto cotes_end;
			}
			
			open_flag = 0;
			gotoxy(0, 4);
			Print(L"                                                           \n");
		}

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto cotes_endend;
		}
	}

cotes_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	ShowSWProtocol(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x00, 1, buf);
	while(!CheckESC(GetKey())){}

cotes_endend:
	console_color(LIGHTGRAY, BLACK);
}

void CaseOpenTestSW(void)
{
	int key;

	while(1)
	{
		clrscr();
		Print(L"1. Show Case Open Information. \n");
		Print(L"2. Case Open Test. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				ShowCaseOpenSW();
				break;
			}
			case '2':
			{
				CaseOpenTestExecuteSW();
				break;
			}
		}
	}
}

int StorageSWProtocolGetStatus(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;
	uint8_t status;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(RDCSWProtocolStorageRead(&prot, &status) == AEC_ERR_INIT)
	{
		buf[0] = 0;
		return 1;
	}
	buf[0] = status;
	
	return 0;
}

int StorageSWProtocolSetLockUnlock(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return RDCSWProtocolStorageWrite(&prot, 0);
}

int StorageSWRWProtocol(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;
	uint8_t status;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(cmd & 0x01)
	{
		return RDCSWProtocolStorageRead(&prot, &status);
	}
	else
	{
		return RDCSWProtocolStorageWrite(&prot, 1);
	}
}


uint8_t StorageWaitBusySW(void)
{
	uint8_t rbuf[2];
	int retry = 3000;

	while(retry)
	{
		usleep(1);
		if(StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, rbuf))
		{
			Print(L"Storage Read Status Fail \n");
			return 1;
		}
		if((rbuf[0] & 0x02) == 0x00)
		{
			return 0;
		}

		retry--;
		if(retry == 0)
		{
			Print(L"Storage Busy Timeout \n");
			return 1;
		}
	}

	return 1;
}

void StoragePrivateAreaLockTestWithPasswordSW(uint8_t type,uint8_t *cp, uint8_t *ep)
{
	uint8_t i, tmp;
	uint8_t wbuf[128], rbuf[128], rbuf_bak[128], buf[2];

	clrscr();
	if(type)
	{
		Print(L"Storage Test - Private Area Lock Test With Correct Password. \n\n");
	}
	else
	{
		Print(L"Storage Test - Private Area Lock Test With Error Password. \n\n");
	}

	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Storage Not Available !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spalapets_endend;
	}

	//busy ?
	if((buf[0] & 0x2) == 0x02)
	{
		console_color(RED, BLACK);
		Print(L"Storage Is Busy !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spalapets_endend;
	}

	//lock ?
	if((buf[0] & 0x04) == 0x00)
	{
		//set password to lock
		if(StorageSWProtocolSetLockUnlock(RDC_SW_CMD_STORAGE_LK, 0xFF, 0x00, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
			goto spalapets_end;
		}
	}

	//read now in private area value
	Print(L"Read Private Area... \n");
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spalapets_end;
	}
	for(i=0; i<128; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf_bak[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

	//enter want write value
	tmp = (uint8_t)get_number_stdin(L"Enter Write Value(0x00~0xFF):", 1, 2);

	//send password to unlock
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if(type)
	{
		if(StorageSWProtocolSetLockUnlock(RDC_SW_CMD_STORAGE_ULK, 0xFF, 0x00, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
			goto spalapets_end;
		}
	}
	else
	{
		if(StorageSWProtocolSetLockUnlock(RDC_SW_CMD_STORAGE_ULK, 0xFF, 0x00, 10, ep))
		{
			Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
			goto spalapets_end;
		}
	}
	//unlock ?
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	if((buf[0] & 0x04) == 0x00)
	{
		Print(L"Unlock Success !! \n");
		if(type == 0)
		{
			goto spalapets_endend;
		}
	}
	else
	{
		Print(L"Unlock Fail !! \n");
		if(type == 1)
		{
			goto spalapets_endend;
		}
	}

	//write private area
	Print(L"Write Private Area... \n\n");
	for(i=0; i<128; i++)
	{
		wbuf[i] = tmp++;
	}
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_WR_STORAGE, 0xFF, 0x00, 128, wbuf))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spalapets_end;
	}

	//read private area
	Print(L"Read Private Area... \n");
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 128, rbuf))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spalapets_end;
	}
	for(i=0; i<128; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}

		if(type == 0)
		{
			if(rbuf[i] != rbuf_bak[i])
			{
				console_color(RED, BLACK);
				Print(L"Compare Fail !! \n");
				goto spalapets_endend;
			}
		}
		else
		{
			if(rbuf[i] != wbuf[i])
			{
				console_color(RED, BLACK);
				Print(L"Compare Fail !! \n");
				goto spalapets_endend;
			}
		}
	}

	console_color(GREEN, BLACK);
	Print(L"Compare Success !! \n");

	//clean password
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if((buf[0] & 0x04) == 0x04)
	{
		if(StorageWaitBusySW())
		{
			goto spalapets_endend;
		}
		if(StorageSWProtocolSetLockUnlock(RDC_SW_CMD_STORAGE_ULK, 0xFF, 0x00, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Unlock Password Fail !! \n\n");
			goto spalapets_end;
		}
	}

	//restore data
	if(StorageWaitBusySW())
	{
		goto spalapets_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_WR_STORAGE, 0xFF, 0x00, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spalapets_end;
	}
	
	goto spalapets_endend;

spalapets_end:
	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	Print(L"Status = %02x \n", buf[0]);

spalapets_endend:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void StoragePrivateAreaTestWithoutPasswordSW(uint8_t *cp)
{
	uint8_t i, tmp;
	uint8_t wbuf[128], rbuf[128], rbuf_bak[128], buf[2];

	clrscr();
	Print(L"Storage Test - Private Area Test Without Password. \n\n");

	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Storage Not Available !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spanpts_endend;
	}

	//busy ?
	if((buf[0] & 0x2) == 0x02)
	{
		console_color(RED, BLACK);
		Print(L"Storage Is Busy !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spanpts_endend;
	}

	//lock ?
	if((buf[0] & 0x04) == 0x04)
	{
		if(StorageWaitBusySW())
		{
			goto spanpts_endend;
		}
		if(StorageSWProtocolSetLockUnlock(RDC_SW_CMD_STORAGE_ULK, 0xFF, 0x00, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Unlock Password Fail !! \n\n");
			goto spanpts_end;
		}
	}

	//read now in private area value
	Print(L"Read Private Area... \n");
	if(StorageWaitBusySW())
	{
		goto spanpts_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spanpts_end;
	}
	for(i=0; i<128; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf_bak[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
	}

	//enter want write value
	tmp = (uint8_t)get_number_stdin(L"Enter Write Value(0x00~0xFF):", 1, 2);

	//write private area
	Print(L"Write Private Area... \n\n");
	for(i=0; i<128; i++)
	{
		wbuf[i] = tmp++;
	}
	if(StorageWaitBusySW())
	{
		goto spanpts_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_WR_STORAGE, 0xFF, 0x00, 128, wbuf))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spanpts_end;
	}

	//read private area
	Print(L"Read And Compare Private Area... \n");
	if(StorageWaitBusySW())
	{
		goto spanpts_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 128, rbuf))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spanpts_end;
	}
	for(i=0; i<128; i++)
	{
		//print data
		if((i & 0x0F) == 0)
		{
			console_color(YELLOW, BLACK);
			Print(L"(%02x) ", i);
			console_color(LIGHTGRAY, BLACK);
		}
		Print(L"[%02X]", rbuf[i]);
		if((i & 0x0F) == 0x0F)
		{
			Print(L"\n");
		}
		
		if(rbuf[i] != wbuf[i])
		{
			console_color(RED, BLACK);
			Print(L"Compare Fail !! \n");
			goto spanpts_endend;
		}
	}

	console_color(GREEN, BLACK);
	Print(L"Compare Success !! \n");
	
	//restore data
	if(StorageWaitBusySW())
	{
		goto spanpts_endend;
	}
	if(StorageSWRWProtocol(RDC_SW_CMD_WR_STORAGE, 0xFF, 0x00, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spanpts_end;
	}
	
	goto spanpts_endend;

spanpts_end:
	StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
	Print(L"Status = %02x \n", buf[0]);
	
spanpts_endend:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void StorageTestSW(void)
{
	uint8_t cpbuf[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};	//correct password
	uint8_t epbuf[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	//error password
	int key;

	while(1)
	{
		clrscr();
		Print(L"1. Storage Test - Private Area Lock Test With Correct Password. \n");
		Print(L"2. Storage Test - Private Area Lock Test With Error Password. \n");
		Print(L"3. Storage Test - Private Area Test Without Password. \n");
		//Print(L"4. Storage Test - General Area Test. \n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		switch(key & 0xFF)
		{
			case '1':
			{
				StoragePrivateAreaLockTestWithPasswordSW(1, cpbuf, epbuf);
				break;
			}
			case '2':
			{
				StoragePrivateAreaLockTestWithPasswordSW(0, cpbuf, epbuf);
				break;
			}
			case '3':
			{
				StoragePrivateAreaTestWithoutPasswordSW(cpbuf);
				break;
			}
			case '4':
			{
				//StorageGeneralAreaTestSW();
				break;
			}
		}
	}
}


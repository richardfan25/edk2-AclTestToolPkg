#include "rdctest.h"
#include "rdctestsw.h"


uint16_t *gThermalType[]=
{
	L"CPU0",
	L"CPU1",
	L"CPU2",
	L"CPU3",
	L"SYS0",
	L"SYS1",
	L"SYS2",
	L"SYS3",
	L"AUX0",
	L"AUX1",
	L"AUX2",
	L"AUX3",
	L"DIMM0",
	L"DIMM1",
	L"DIMM2",
	L"DIMM3",
	L"PCH",
	L"GRAPHIC",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"OEM0",
	L"OEM1",
	L"OEM2",
	L"OEM3",
};

uint16_t *gVoltageType[]=
{
	L"5VS0",
	L"5VS5",
	L"12VS0",
	L"12VS5",
	L"3V3S0",
	L"3V3S5",
	L"5VSB",
	L"3VSB",
	L"BAT_CMOS",
	L"BAT",
	L"DC",
	L"DC_SB",
	L"COREA",
	L"COREB",
	L"",
	L"",
	L"OEM0",
	L"OEM1",
	L"OEM2",
	L"OEM3",
};

uint16_t *gVoltageTypeSW[]=
{
	L"5VS0",
	L"5VS5",
	L"12VS0",
	L"12VS5",
	L"3V3S0",
	L"3V3S5",
	L"3VSB",
	L"5VSB",
	L"BAT_CMOS",
	L"BAT",
	L"DC",
	L"DC_SB",
	L"COREA",
	L"COREB",
	L"",
	L"",
	L"OEM0",
	L"OEM1",
	L"OEM2",
	L"OEM3",
};

uint16_t *gFanType[]=
{
	L"CPU0",
	L"CPU1",
	L"CPU2",
	L"CPU3",
	L"SYS0",
	L"SYS1",
	L"SYS2",
	L"SYS3",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"",
	L"OEM0",
	L"OEM1",
	L"OEM2",
	L"OEM3",
};

const uint16_t *gThermalError[] = 
{
	L"",
	L"Thermal channel error",
	L"Thermal protection type error",
	L"Thermal protection high temperature error",
	L"Thermal protection low temperature error",
};

uint16_t *tool_name = L"RDCTest";
uint16_t major_v = 0;
uint16_t minor_v = 22;

const MODULE_FUNCTION gModuleTable[] = 
{
	//id,	str,								FW funciton						SW function
	{0,		L"Board Information",				ShowBoardInformation,			ShowBoardInformationSW},
	{1,		L"Hardware Monitor",				ShowHardwareMonitor,			ShowHardwareMonitorSW},
	{2,		L"Thermal Test",					ThermalTest,					ThermalTestSW},
	{3,		L"Watchdog Test",					WatchDogTest,					WatchDogTestSW},
	{4,		L"Fan Test",						FanTest,						FanTestSW},
	{5,		L"GPIO Test",						GPIOTest,						GPIOTestSW},
	{6,		L"Panel Test",						PanelTest,						PanelTestSW},
	{7,		L"SMBUS Test",					SmbusTest,						SmbusTest},
	{8,		L"I2C Test",						I2CTest,							I2CTest},
	{9,		L"Case Open Test",					CaseOpenTest,					CaseOpenTestSW},
	{10,		L"Storage Test",					StorageTest,						StorageTestSW},
	{11,		L"CAN Bus Test",					CANBusTest,						CANBusTest},
	{12,		L"Smbus I2C Mode Test (For RD)",	SmbusUseI2CModeTest,			SmbusUseI2CModeTest},
};

uint8_t gLM95245Address[LM_SMB_ADDR_NUM] = { LM_SMB_ADDR0, 
										LM_SMB_ADDR1, 
										LM_SMB_ADDR2, 
										LM_SMB_ADDR3, 
										LM_SMB_ADDR4};

UINT8 gPCA9555Address[8] = {0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E};


void ShowBoardInformation(void)
{
	clrscr();
	show_board_info();
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	
	while(!CheckESC(GetKey())){}
}

void ShowHardwareMonitor(void)
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
			rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x01, i, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
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
			rdc_ec_read_prot(RDC_FW_CMD_RD_TACH, 0x00, i, 1, buf);
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_TACH, 0x01, i, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Tachometer Type Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_TACH, 0x10, i, 4, buf))
			{
				Print(L"PMC Communicate Fail !! Read Tachometer Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
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
			rdc_ec_read_prot(RDC_FW_CMD_RD_VOLT, 0x00, i, 1, buf);
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_VOLT, 0x01, i, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Read Voltage Type Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
			}
			else
			{
				if(buf[0] > 0x13)
				{
					Print(L"Type Error! (0x%x)", buf[0]);
				}
				else
				{
					Print(L"%s", *(gVoltageType + buf[0]));
				}
			}

			gotoxy(40, ConsoleCurrRow);
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_VOLT, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Voltage Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
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
		show_flag = 1;
		for(i=0; i<2; i++)
		{
			rdc_ec_read_prot(RDC_FW_CMD_RD_CURR, 0x00, i, 1, buf);
			// available ?
			if((buf[0] & 0x1) == 0)
			{
				continue;
			}

			if(show_flag)
			{
				show_flag = 0;
				gotoxy(0, ConsoleCurrRow);
				Print(L"Current: \n");
				ConsoleCurrRow++;
			}
			
			gotoxy(10, ConsoleCurrRow);
			Print(L"DC-IN");

			gotoxy(30, ConsoleCurrRow);
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_CURR, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Current Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto shm_end;
			}
			else
			{
				data16 = *(uint16_t *)buf;
				Print(L": %5d mA \n", data16);
			}
		}
		
		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto shm_endend;
		}
	}

shm_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

shm_endend:
	NULL;
}

void ThermalProtectThrottle(void)
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
			goto tpt_end;
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
					goto tpt_end;
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
					goto tpt_end;
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
				goto tpt_end;
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
				rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
				// FW available ?
				if(((buf[0] & 0x01) == 0) || ((buf[0] & 0x02) == 0))
				{
					continue;
				}
				
				// SW available ? open it
				if((buf[1] & 0x01) == 0)
				{
					buf[0] = 1;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x20, i, 1, buf))	//set SW Active
					{
						Print(L"PMC Communicate Fail !! Set Throttle Active Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature:%3d.%dC \n", (data16-2731)/10, abs(data16-2731)%10);
					}
					
					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(0~100 Default:70 C):", 0, 3);
					if((h_limit < 0) || (h_limit > 100))
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
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x21, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x22, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x21, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x22, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tpt_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
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
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x21, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tpt_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Throttle, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x22, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tpt_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x01, i, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tpt_end;
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto tpt_end;
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
			goto tpt_end;
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
			
			goto tpt_endend;
		}
	}

tpt_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

tpt_endend:
	PciExit();
}

void ThermalProtectPowerOff(void)
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
				rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
				// available ?
				if(((buf[0] & 0x1) == 0) || ((buf[0] & 0x04) == 0))
				{
					continue;
				}
				
				// SW available ? open it
				if((buf[1] & 0x02) == 0)
				{
					buf[0] = 1;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x30, i, 1, buf))	//set SW Active
					{
						Print(L"PMC Communicate Fail !! Set Power Off Active Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature:%3d.%dC \n", (data16-2731)/10, abs(data16-2731)%10);
					}

					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(0~100 Default:70 C):", 0, 3);
					if((h_limit < 0) || (h_limit > 100))
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
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x31, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x32, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x31, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x32, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tppo_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
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
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x31, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tppo_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Power Off, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x32, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tppo_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;

				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x01, i, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tppo_end;
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto tppo_end;
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
			goto tppo_end;
		}

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto tppo_endend;
		}
	}

tppo_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
	
tppo_endend:
	NULL;
}

void ThermalProtectShutdown(void)
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
				rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
				// available ?
				if(((buf[0] & 0x1) == 0) || ((buf[0] & 0x08) == 0))
				{
					continue;
				}
				
				// SW available ? open it
				if((buf[1] & 0x04) == 0)
				{
					buf[0] = 1;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x40, i, 1, buf))	//set SW Active
					{
						Print(L"PMC Communicate Fail !! Set Shutdown Active Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
				}
				
				while(1)
				{
					gotoxy(0, ConsoleCurrRow);
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					else
					{
						data16 = *(uint16_t *)buf;
						Print(L"Temerature:%3d.%dC \n", (data16-2731)/10, abs(data16-2731)%10);
					}

					h_limit = (uint8_t) get_number_stdin(L"Enter High Limit(0~100 Default:70 C):", 0, 3);
					if((h_limit < 0) || (h_limit > 100))
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
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x41, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					buf[0] = 0x1B;
					buf[1] = 0x09;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x42, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}

					//set new high limit
					buf[0] = h_limit & 0xFF;
					buf[1] = (h_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x41, i, 2, buf))	//set high temperature
					{
						Print(L"PMC Communicate Fail !! Set High Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
						console_color(LIGHTGRAY, BLACK);
						goto reinput;
					}
					
					//set new low limit
					buf[0] = l_limit & 0xFF;
					buf[1] = (l_limit >> 8) & 0xFF;
					if(rdc_ec_write_prot(RDC_FW_CMD_WR_THERM, 0x42, i, 2, buf))	//set low temperature
					{
						Print(L"PMC Communicate Fail !! Set Low Limit Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x04, i, 1, buf))
					{
						Print(L"PMC Communicate Fail !! Read Thermal Error Code Fail !! \n\n");
						show_prot(0x01, 0x00, 0x00, 1, buf);
						goto tps_end;
					}
					if(buf[0] != 0)
					{
						console_color(RED, BLACK);
						Print(L"%s !! \n", gThermalError[buf[0]]);
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

				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x41, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read High Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tps_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Type:Shutdown, High Limit: %3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
				}
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x42, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Low Limit Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tps_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L"Low Limit: %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
				ConsoleCurrRow++;
				
				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x01, i, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					show_prot(0x01, 0x00, 0x00, 1, buf);
					goto tps_end;
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
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
			{
				Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
				show_prot(0x01, 0x00, 0x00, 1, buf);
				goto tps_end;
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
			goto tps_end;
		}

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto tps_endend;
		}
	}

tps_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

tps_endend:
	NULL;
}

void ThermalTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show Thermal Information. \n");
		Print(L"2. Test Thermal Protect - Throttle. \n");
		Print(L"3. Test Thermal Protect - Power Off. \n");
		Print(L"4. Test Thermal Protect - Shutdown. \n");
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
				clrscr();
				show_thermal();
					
				gotoxy(0, ConsoleHeight - 1);
				Print(L"Press ESC to return.");
				
				while(!CheckESC(GetKey())){}
				break;
			}
			case '2':
			{
				ThermalProtectThrottle();
				break;
			}
			case '3':
			{
				ThermalProtectPowerOff();
				break;
			}
			case '4':
			{
				ThermalProtectShutdown();
				break;
			}
		}
	}
}

void WatchDogPowerCycleEvent(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 5000, p_time = 5000;
	
	clrscr();
	Print(L"Test Watchdog - Power Cycle Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x00, 0, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdpce_endend;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(100~9999 Default:5000 ms):", 0, 4);
	if((d_time < 100) || (d_time > 9999))
	{
		d_time = 5000;
	}
	ConsoleCurrRow++;
	
	p_time = get_number_stdin(L"Enter Power Cycle Event Time(100~9999 Default:5000 ms):", 0, 4);
	if((p_time < 100) || (p_time > 9999))
	{
		p_time = 5000;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;

	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x11, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdpce_end;
	}

	//set watchdog Power Cycle Event Time
	*(uint32_t*)buf = p_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x12, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Power Cycle Event Time Fail !! \n\n");
		goto wdpce_end;
	}

	//start watchdog
	buf[0] = 0x01;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdpce_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");
	
	for (i=1; i<=((d_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Power Cycle Event Time Fail !! \n\n");
				goto wdpce_end;
			}

			i = d_time / 100;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
		
		rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x02, 0, 1, buf);
		if((buf[0]&0x04)==0x00)
		{
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", p_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=((p_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Power Cycle Event Time Fail !! \n\n");
				goto wdpce_end;
			}
			i = 1;
		}
	}

	console_color(YELLOW, RED);
	Print(L"Watchdog Fail. \n\n");
	console_color(LIGHTGRAY, BLACK);

	goto wdpce_endend;
	
wdpce_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

wdpce_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogResetEvent(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 5000, r_time = 5000;
	
	clrscr();
	Print(L"Test Watchdog - Reset Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x00, 0, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdre_endend;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(100~9999 Default:5000 ms):", 0, 4);
	if((d_time < 100) || (d_time > 9999))
	{
		d_time = 5000;
	}
	ConsoleCurrRow++;
	
	r_time = get_number_stdin(L"Enter Reset Event Time(100~9999 Default:5000 ms):", 0, 4);
	if((r_time < 100) || (r_time > 9999))
	{
		r_time = 5000;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;
	
	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x11, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdre_end;
	}
	
	//set watchdog Reset Event Time
	*(uint32_t*)buf = r_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x14, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Reset Event Time Fail !! \n\n");
		goto wdre_end;
	}

	//start watchdog
	buf[0] = 0x01;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdre_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");

	for (i=1; i<=((d_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Reset Event Time Fail !! \n\n");
				goto wdre_end;
			}

			i = d_time / 100;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
		
		rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x02, 0, 1, buf);
		if((buf[0]&0x04)==0x00)
		{
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", r_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=((r_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Reset Event Time Fail !! \n\n");
				goto wdre_end;
			}
			i = 1;
		}
	}

	console_color(YELLOW, RED);
	Print(L"Watchdog Fail. \n\n");
	console_color(LIGHTGRAY, BLACK);

	goto wdre_endend;
	
wdre_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

wdre_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogPinEvent(void)
{
	uint8_t buf[32];
	uint32_t i, d_time = 5000, p_time = 5000;
	
	clrscr();
	Print(L"Test Watchdog - Pin Event Time. \n\n");
	ConsoleCurrRow = 2;

	rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x00, 0, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"WatchDog Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto wdpe_endend;
	}

	gotoxy(0, ConsoleCurrRow);
	d_time = get_number_stdin(L"Enter Delay Time(100~9999 Default:5000 ms):", 0, 4);
	if((d_time < 100) || (d_time > 9999))
	{
		d_time = 5000;
	}
	ConsoleCurrRow++;
	
	p_time = get_number_stdin(L"Enter Pin Event Time(100~9999 Default:5000 ms):", 0, 4);
	if((p_time < 100) || (p_time > 9999))
	{
		p_time = 5000;
	}
	Print(L"\n");
	ConsoleCurrRow+=2;

	//set watchdog delay time
	*(uint32_t*)buf = d_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x11, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Watchdog Delay Time Fail !! \n\n");
		goto wdpe_end;
	}

	//set watchdog Pin Event Time
	*(uint32_t*)buf = p_time;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x15, 0, 4, buf))
	{
		Print(L"PMC Communicate Fail !! Set Pin Event Time Fail !! \n\n");
		goto wdpe_end;
	}

	//start watchdog
	buf[0] = 0x01;
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Start Watchdog Fail !! \n\n");
		goto wdpe_end;
	}

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Delay In %04d ms. \n\n", d_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to kick timer.");

	for (i=1; i<=((d_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Pin Event Time Fail !! \n\n");
				goto wdpe_end;
			}

			i = d_time / 100;
			gotoxy(0, ConsoleCurrRow);
			Print(L"Watchdog Delay : %2d second. \n\n", i / 10);
			break;
		}
		
		rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x02, 0, 1, buf);
		if((buf[0]&0x04)==0x00)
		{
			break;
		}
	}
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleCurrRow);
	Print(L"Watchdog Will Active In %04d ms. \n\n", p_time);
	ConsoleCurrRow+=2;

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press Space to reset timer.");
	
	for (i=1; i<=((p_time / 100) + 9); i++)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Watchdog Counter : %2d second. \n\n", i / 10);
		usleep(100 * 1000);	//100 ms

		if(bioskey(1) == ' ')
		{
			buf[0] = 0x02;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_WDT, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Reset Pin Event Time Fail !! \n\n");
				goto wdpe_end;
			}
			i = 1;
		}
	}

	goto wdpe_endend;
	
wdpe_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

wdpe_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.       ");
	while(!CheckESC(GetKey())){}
}

void WatchDogTest(void)
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
				clrscr();
				show_watchdog();
				
				gotoxy(0, ConsoleHeight - 1);
				Print(L"Press ESC to return.");
				
				while(!CheckESC(GetKey())){}
				break;
			}
			case '2':
			{
				WatchDogPowerCycleEvent();
				break;
			}
			case '3':
			{
				WatchDogResetEvent();
				break;
			}
			case '4':
			{
				WatchDogPinEvent();
				break;
			}
		}
	}
}

void ShowFanInformation(void)
{
	uint8_t i, buf[32];
	uint16_t data16;
	uint32_t data32;

	clrscr();
	
	Print(L"[-] Fan Module Status : ");
	for(i=0; i<4; i++)
	{
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, i, 1, buf))
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
		rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, i, 1, buf);
		// available ?
		if((buf[0] & 0x01) == 0)
		{
			continue;
		}

		//Type
		Print(L"[%d] Fan Type          : ", i);
		if(rdc_sw_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x01, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//Control
		Print(L"[%d] Fan Control       : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x02, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X ", buf[0]);
		}
		Print(L"\n");

		//PWM Duty
		Print(L"[%d] Fan PWM Duty      : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x10, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", buf[0], buf[0]);
			
		}
		Print(L"\n");

		//PWM Polarity
		Print(L"[%d] Fan PWM Polarity  : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x11, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %s ", buf[0], (buf[0]&0x01)?L"Invert":L"Normal");
			
		}
		Print(L"\n");

		//PWM Frequency
		Print(L"[%d] Fan PWM Frequency : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x12, i, 4, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data32 = *(uint32_t *)buf;
			Print(L"0x%08X = %d Hz ", data32, data32);
		}
		Print(L"\n");

		//PWM Max
		Print(L"[%d] Fan PWM Max       : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x16, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", buf[0], buf[0]);
		}
		Print(L"\n");

		//PWM Min
		Print(L"[%d] Fan PWM Min       : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x17, i, 1, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			Print(L"0x%02X = %d %% ", buf[0], buf[0]);
		}
		Print(L"\n");

		//RPM Max
		Print(L"[%d] Fan RPM Max       : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x18, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d RPM ", data16, data16);
		}
		Print(L"\n");

		//RPM Min
		Print(L"[%d] Fan RPM Min       : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x19, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d RPM ", data16, data16);
		}
		Print(L"\n");

		//Current RPM
		Print(L"[%d] Fan Current RPM   : ", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x1A, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d RPM ", data16, data16);
		}
		Print(L"\n");

		//Temperature Limit
		Print(L"[%d] Fan Temperature Limit (Hi, Lo, Stop) : \n", i);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x13, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"    0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x14, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x15, i, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"0x%04X = %d = %d.%d C ", data16, data16, (data16-2731)/10, abs(data16-2731)%10);
		}
		Print(L"\n");
	}

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void FanOnOffTest(void)
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
		rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, i, 1, buf);
		// available ?
		if((buf[0] & 0x01) == 0)
		{
			continue;
		}
		flag = 1;

		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x02, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan Control Mode Fail !! \n\n");
			goto foot_end;
		}
		tmp = buf[0];

		gotoxy(0, ConsoleCurrRow);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x01, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan Type Fail !! \n\n");
			goto foot_end;
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
		
		buf[0] = 0x00;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x02, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Off Fail !! \n\n");
			goto foot_end;
		}
		for(j=0; j<fanoffdelay; j++)
		{
			gotoxy(22, ConsoleCurrRow);
			Print(L"wait %02d sec.", fanoffdelay - j);
			usleep(1000 * 1000);	// 1 sec
		}

		gotoxy(22, ConsoleCurrRow);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x1A, i, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan RPM Fail !! \n\n");
			goto foot_end;
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

		buf[0] = 0x01;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x02, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Full Fail !! \n\n");
			goto foot_end;
		}
		for(j=0; j<fanondelay; j++)
		{
			gotoxy(22, ConsoleCurrRow);
			Print(L"wait %02d sec.", fanondelay - j);
			usleep(1000 * 1000);	// 1 sec
		}

		gotoxy(22, ConsoleCurrRow);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x1A, i, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Fan RPM Fail !! \n\n");
			goto foot_end;
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
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x02, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore Fan Control Mode Fail !! \n\n");
			goto foot_end;
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

	goto foot_endend;

foot_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

foot_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void FanManualTest(void)
{
	uint8_t fan = 0, duty = 0, duty_bak = 0, type = 0, control_tmp = 0, duty_tmp = 0, restore = 0;
	uint8_t i, flag = 0, buf[32];
	uint16_t data16;
	int key;
	
	clrscr();

	gotoxy(0, 0);
	Print(L"Test Fan - Manual. \n\n");
	ConsoleCurrRow = 2;

	Print(L"Available Fan : ");
	for(i=0; i<4; i++)
	{
		rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, i, 1, buf);
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
		goto fmt_endend;
	}
	Print(L"\n");
	ConsoleCurrRow++;
	
	gotoxy(0, ConsoleCurrRow);
	fan = (uint8_t)get_number_stdin(L"Select Fan(0~3:Fan0  Fan1  Fan2  Fan3)(Default:0):", 0, 1);
	if(fan < 0 || fan >3)
	{
		fan = 0;
	}
	
	rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, fan, 1, buf);
	// available ?
	if((buf[0] & 0x01) == 0)
	{
		Print(L"Fan %d Is Unavailable !! \n\n", fan);
		goto fmt_endend;
	}

	//backup control mode
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x02, fan, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Read Fan Control Mode Fail !! \n\n");
		goto fmt_end;
	}
	control_tmp = buf[0];
	
	//backup pwm duty
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x10, fan, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
		goto fmt_end;
	}
	duty_tmp = buf[0];
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
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x02, fan, 1, buf))
	{
		Print(L"PMC Communicate Fail !! Set Fan Control Mode Fail !! \n\n");
		goto fmt_end;
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
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x13, fan, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"%3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x14, fan, 2, buf))
		{
			Print(L"Error !! ");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			Print(L"%3d.%d C, ", (data16-2731)/10, abs(data16-2731)%10);
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x15, fan, 2, buf))
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
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x10, fan, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto fmt_end;
		}
		Print(L"Fan PWM Duty : %3d %% \n", buf[0]);
		duty = buf[0];

		//Current RPM
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x1A, fan, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read Current RPM Fail !! \n\n");
			goto fmt_end;
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
			goto fmt_endendend;
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
				buf[0] = duty;
				if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x10, fan, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Set PWM Duty Fail !! \n\n");
					goto fmt_end;
				}
				duty_bak = duty;
			}
		}
		else if(type == 3)
		{
			//Temperature
			for(i=0; i<4; i++)
			{
				rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
				// available ?
				if((buf[0] & 0x1) == 0)
				{
					continue;
				}

				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x01, i, 1, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Type Fail !! \n\n");
					goto fmt_end;
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

				if(rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x10, i, 2, buf))
				{
					Print(L"PMC Communicate Fail !! Read Thermal Value Fail !! \n\n");
					goto fmt_end;
				}
				else
				{
					data16 = *(uint16_t *)buf;
					Print(L": %3d.%d C \n", (data16-2731)/10, abs(data16-2731)%10);
				}
			}
		}
	}

fmt_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

fmt_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}

fmt_endendend:
	//restore fan control mode & pwm duty
	if(restore)
	{
		buf[0] = control_tmp;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x02, fan, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Set Fan Control Mode Fail !! \n\n");
		}

		if(type == 2)
		{
			buf[0] = duty_tmp;
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_FAN, 0x10, fan, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Set PWM Duty Fail !! \n\n");
			}
		}
	}
}

void FanTest(void)
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
				ShowFanInformation();
				break;
			}
			case '2':
			{
				FanOnOffTest();
				break;
			}
			case '3':
			{
				FanManualTest();
				break;
			}
		}
	}
}

void GPIOInputOutputTest()
{
	uint8_t n, i, result, type = 0, buf[2];
	uint8_t dir_bak[2], level_bak[2];
	uint16_t tmp;
	UINTN y_back;
	
	clrscr();
	Print(L"GPIO Input Output Test. \n");

	rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x00, 0, 1, buf);
	// available ?
	if((buf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"GPIO Non-available !!");
		console_color(LIGHTGRAY, BLACK);
		goto gpioiot_endend;
	}
	
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x03, 0, 2, buf))
	{
		Print(L"PMC Communicate Fail !! Read GPIO Availability Fail !! \n\n");
		goto gpioiot_end;
	}
	tmp = (uint16_t)((uint16_t)buf[1]<<8 | (uint16_t)buf[0]);
	if(tmp == 0x00FF)	// 8pin
	{
		type = 8;
	}
	else if(tmp == 0xFFFF)	//16pin
	{
		type = 16;
	}

	//backup Direction
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x12, 0, 2, dir_bak))
	{
		Print(L"PMC Communicate Fail !! Read GPIO Direction Fail !! \n\n");
		goto gpioiot_end;
	}
	
	//backup Level
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x13, 0, 2, level_bak))
	{
		Print(L"PMC Communicate Fail !! Read GPIO Level Fail !! \n\n");
		goto gpioiot_end;
	}
	
	for(n=0; n<(type/8); n++)
	{
		clrscr();
		Print(L"GPIO Input Output Test. \n");

		Print(L"Please connect gpio as follow. \n");
		y_back = ConsoleCurrRow = 2;

		for(i=(n*8); i<(4+(n*8)) ; i++)
		{
			gotoxy(5, ConsoleCurrRow);
			Print(L"GPIO%-2d  <-------->   GPIO%-2d", i, (i + 4));
			ConsoleCurrRow++;
		}

		gotoxy(0, ConsoleCurrRow);
		Print(L"Press any key to start test...");
		ConsoleCurrRow += 2;
		
		GetKey(); //wait key in

		//test low pin to high pin
		y_back = ConsoleCurrRow;
		for(i=(n*8); i<(4+(n*8)) ; i++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%-2d  -------->    GPIO%-2d", i, (i + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}
		
		buf[0] = buf[1] = 0x00;
		buf[n] = 0x0F;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x12, 0, 2, buf))	//set direction
		{
			Print(L"PMC Communicate Fail !! Set GPIO Direction Fail !! \n\n");
			goto restore_end;
		}
		buf[0] = buf[1] = 0x00;
		buf[n] = 0x0F;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, buf))	//set high
		{
			Print(L"PMC Communicate Fail !! Set GPIO Level High Fail !! \n\n");
			goto restore_end;
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x13, 0, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read GPIO Level Fail !! \n\n");
			goto restore_end;
		}
		
		result = buf[n];	//store test result of high
		usleep(1000);
		
		buf[0] = buf[1] = 0x00;
		buf[n] = 0xF0;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, buf))	//set low
		{
			Print(L"PMC Communicate Fail !! Set GPIO Level Low Fail !! \n\n");
			goto restore_end;
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x13, 0, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read GPIO Level Fail !! \n\n");
			goto restore_end;
		}
		ConsoleCurrRow = y_back;
		result >>= 4;
		buf[n] >>= 4;
		for(i=0; i<4; i++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[n] >> i) & 0x01) == 0x00) && (((result >> i) & 0x01) == 0x01))
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
		for(i=(n*8); i<(4+(n*8)) ; i++)
		{
			gotoxy(5, ConsoleCurrRow);
			console_color(YELLOW, BLACK);
			Print(L"GPIO%-2d  <--------    GPIO%-2d", i, (i + 4));
			console_color(LIGHTGRAY, BLACK);
			ConsoleCurrRow++;
		}

		buf[0] = buf[1] = 0x00;
		buf[n] = 0xF0;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x12, 0, 2, buf))	//set direction
		{
			Print(L"PMC Communicate Fail !! Set GPIO Direction Fail !! \n\n");
			goto restore_end;
		}
		buf[0] = buf[1] = 0x00;
		buf[n] = 0xF0;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, buf))	//set high
		{
			Print(L"PMC Communicate Fail !! Set GPIO Level High Fail !! \n\n");
			goto restore_end;
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x13, 0, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read GPIO Level Fail !! \n\n");
			goto restore_end;
		}
		
		result = buf[0];	//store test result of high
		usleep(1000);

		buf[0] = buf[1] = 0x00;
		buf[n] = 0x0F;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, buf))	//set low
		{
			Print(L"PMC Communicate Fail !! Set GPIO Level Low Fail !! \n\n");
			goto restore_end;
		}
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x13, 0, 2, buf))
		{
			Print(L"PMC Communicate Fail !! Read GPIO Level Fail !! \n\n");
			goto restore_end;
		}
		ConsoleCurrRow = y_back;
		for(i=0; i<4; i++)
		{
			gotoxy(45, ConsoleCurrRow);
			if((((buf[n] >> i) & 0x01) == 0x00) && (((result >> i) & 0x01) == 0x01))
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

		if((n + 1) < (type / 8))
		{
			Print(L"\n\n");
			Print(L"Press any key to continue next eight pin \n\n");
			GetKey();
		}
	}

	//restore Direction
	rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x12, 0, 2, dir_bak);
	//restore Level
	rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, level_bak);
	
	goto gpioiot_endend;
	
restore_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);
	//restore Direction
	rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x12, 0, 2, dir_bak);
	//restore Level
	rdc_ec_write_prot(RDC_FW_CMD_WR_GPIO, 0x13, 0, 2, level_bak);
	goto gpioiot_endend;

gpioiot_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

gpioiot_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void GPIOTest(void)
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
				clrscr();
				show_gpio();

				gotoxy(0, ConsoleHeight - 1);
				Print(L"Press ESC to return.");
				
				while(!CheckESC(GetKey())){}
				break;
			}
			case '2':
			{
				GPIOInputOutputTest();
				break;
			}
		}
	}
}

void PanelBacklightOnOffTest(void)
{
	uint8_t i, j, tmp, buf[32];
	uint8_t flag = 0;
	
	clrscr();
	Print(L"Panel Backlight On/Off Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x00, i, 1, buf);
		// available ?
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;

		if(rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x12, i, 1, buf))
		{	
			Print(L"PMC Communicate Fail !! Read Switch Fail !! \n\n");
			goto pboot_end;
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
		
		buf[0] = 0x01;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x12, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Write Switch Fail !! \n\n");
			goto pboot_end;
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

		buf[0] = 0x00;
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x12, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Write Switch Fail !! \n\n");
			goto pboot_end;
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
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x12, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore Switch Fail !! \n\n");
			goto pboot_end;
		}
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Panel Backlight Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto pboot_endend;
	
pboot_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

pboot_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightLevelAutoTest(void)
{
	uint8_t i, j, tmp;
	uint8_t buf[32];
	uint8_t flag = 0;

	clrscr();
	Print(L"Panel Backlight Level Auto Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x00, i, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;
		
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x14, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto pblat_end;
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
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x14, i, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Write PWM Duty Fail !! \n\n");
				goto pblat_end;
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
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x14, i, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Write PWM Duty Fail !! \n\n");
				goto pblat_end;
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
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x14, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Restore PWM Duty Fail !! \n\n");
			goto pblat_end;
		}
	}
	
	if(flag == 0)
	{
		console_color(RED, BLACK);
		Print(L"Panel Backlight Non-available !!");
		console_color(LIGHTGRAY, BLACK);
	}

	goto pblat_endend;
	
pblat_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

pblat_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightLevelManualTest(void)
{
	uint8_t flag = 0;
	uint8_t i, tmp, buf[32];
	uint16_t level;

	clrscr();
	Print(L"Panel Backlight Level Manual Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x00, i, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;

		gotoxy(0, ConsoleCurrRow);
		Print(L"Panel Backlight %d \n", i);
		ConsoleCurrRow++;
		
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x14, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read PWM Duty Fail !! \n\n");
			goto pblmt_end;
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
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x14, i, 1, (uint8_t *)&level))
		{
			Print(L"PMC Communicate Fail !! Set Level Fail !! \n\n");
			goto pblmt_end;
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

	goto pblmt_endend;
	
pblmt_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

pblmt_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelBacklightControlModeTest(void)
{
	uint8_t i, mode, mode_tmp;
	uint8_t buf[32];
	uint8_t flag = 0;

	clrscr();
	Print(L"Panel Backlight Control Mode Test. \n\n");
	ConsoleCurrRow = 2;

	for(i=0; i<2; i++)
	{
		rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x00, i, 1, buf);
		if((buf[0] & 0x1) == 0)
		{
			continue;
		}
		flag = 1;

		gotoxy(0, ConsoleCurrRow);
		Print(L"Panel Backlight %d \n", i);
		ConsoleCurrRow++;
		
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x19, i, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Control Mode Fail !! \n\n");
			goto pbcmt_end;
		}
		mode_tmp = buf[0];
		
		mode = (uint8_t)get_number_stdin(L"Choice Control Mode (0:PWM other:DC):", 0, 1);
		ConsoleCurrRow++;

		//set backlight control mode
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_PANEL, 0x19, i, 1, &mode))
		{
			Print(L"PMC Communicate Fail !! Set Control Mode Fail \n\n");
			goto pbcmt_end;
		}
		
		if(mode != 0)
		{
			mode = 1;
			console_color(YELLOW, BLACK);
			Print(L"Set Control Mode To DC Mode \n");
		}
		else
		{
			console_color(YELLOW, BLACK);
			Print(L"Set Control Mode To PWM Mode \n");
		}
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

	goto pbcmt_endend;
	
pbcmt_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

pbcmt_endend:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void PanelTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		Print(L"1. Show Panel Information. \n");
		Print(L"2. Panel Backlight On/Off Test. \n");
		Print(L"3. Panel Backlight Level Auto Test. \n");
		Print(L"4. Panel Backlight Level Manual Test. \n");
		Print(L"5. Panel Backlight Control Mode Test. \n");
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
				clrscr();
				show_panel();

				gotoxy(0, ConsoleHeight - 1);
				Print(L"Press ESC to return.");
				
				while(!CheckESC(GetKey())){}
				break;
			}
			case '2':
			{
				PanelBacklightOnOffTest();
				break;
			}
			case '3':
			{
				PanelBacklightLevelAutoTest();
				break;
			}
			case '4':
			{
				PanelBacklightLevelManualTest();
				break;
			}
			case '5':
			{
				PanelBacklightControlModeTest();
				break;
			}
		}
	}
}

void CaseOpenTestExecute()
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
		
		rdc_ec_read_prot(RDC_FW_CMD_RD_CASEOPEN, 0x00, 0, 1, buf);
		// available ?
		if((buf[0] & 0x1) == 0)
		{
			console_color(RED, BLACK);
			Print(L"Case Open Not Available !! \n\n");
			console_color(LIGHTGRAY, BLACK);
			while(!CheckESC(GetKey())){}
			goto cote_endend;
		}

		//check case not open
		console_color(YELLOW, BLACK);
		Print(L"Check Case Open Status : ");
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_CASEOPEN, 0x02, 0, 1, buf))
		{
			Print(L"PMC Communicate Fail !! Read Case Open Flag Fail !! \n\n");
			goto cote_end;
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
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_CASEOPEN, 0x02, 0, 1, buf))
			{
				Print(L"PMC Communicate Fail !! Clean Case Open Flag Fail !! \n\n");
				goto cote_end;
			}
			
			open_flag = 0;
			gotoxy(0, 4);
			Print(L"                                                           \n");
		}

		//get key to return or timeout refresh
		if(WaitReturnOrRefreshEvent())
		{
			goto cote_endend;
		}
	}

cote_end:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);
	while(!CheckESC(GetKey())){}

cote_endend:
	console_color(LIGHTGRAY, BLACK);
}

void CaseOpenTest(void)
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
				clrscr();
				show_case_open(0);

				gotoxy(0, ConsoleHeight - 1);
				Print(L"Press ESC to return.");
				
				while(!CheckESC(GetKey())){}
				break;
			}
			case '2':
			{
				CaseOpenTestExecute();
				break;
			}
		}
	}
}

uint8_t StorageWaitBusy(void)
{
	uint8_t rbuf[2];
	int retry = 3000;

	while(retry)
	{
		usleep(1);
		if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, rbuf))
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

void StoragePrivateAreaLockTestWithPassword(uint8_t type,uint8_t *cp, uint8_t *ep)
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

	rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, rbuf);
	// available ?
	if((rbuf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Storage Not Available !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spalapet_endend;
	}

	//busy ?
	if((rbuf[0] & 0x2) == 0x02)
	{
		console_color(RED, BLACK);
		Print(L"Storage Is Busy !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spalapet_endend;
	}

	//lock ?
	if((rbuf[0] & 0x04) == 0x00)
	{
		//Password Present ?
		if((rbuf[0] & 0x20) == 0x20)
		{
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 1, ep))
			{
				Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
				goto spalapet_end;
			}
		}
		else
		{
			//set password to lock
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
			{
				Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
				goto spalapet_end;
			}
		}
	}

	//read now in private area value
	Print(L"Read Private Area... \n");
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x04, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spalapet_end;
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
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	if(type)
	{
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
			goto spalapet_end;
		}
	}
	else
	{
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, ep))
		{
			Print(L"PMC Communicate Fail !! Set Password Fail !! \n\n");
			goto spalapet_end;
		}
	}
	//unlock ?
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, buf);
	if((buf[0] & 0x04) == 0x00)
	{
		Print(L"Unlock Success !! \n");
		if(type == 0)
		{
			goto spalapet_endend;
		}
	}
	else
	{
		Print(L"Unlock Fail !! \n");
		if(type == 1)
		{
			goto spalapet_endend;
		}
	}

	//write private area
	Print(L"Write Private Area... \n\n");
	for(i=0; i<128; i++)
	{
		wbuf[i] = tmp++;
	}
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x04, 0, 128, wbuf))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spalapet_end;
	}

	//read private area
	Print(L"Read Private Area... \n");
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x04, 0, 128, rbuf))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spalapet_end;
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
				goto spalapet_endend;
			}
		}
		else
		{
			if(rbuf[i] != wbuf[i])
			{
				console_color(RED, BLACK);
				Print(L"Compare Fail !! \n");
				goto spalapet_endend;
			}
		}
	}
	
	console_color(GREEN, BLACK);
	Print(L"Compare Success !! \n");

	//clean password
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, rbuf);
	if((rbuf[0] & 0x20) == 0x20)
	{
		if((rbuf[0] & 0x04) == 0x04)
		{
			if(StorageWaitBusy())
			{
				goto spalapet_endend;
			}
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
			{
				Print(L"PMC Communicate Fail !! Unlock Password Fail !! \n\n");
				goto spalapet_end;
			}
		}

		if(StorageWaitBusy())
		{
			goto spalapet_endend;
		}
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Remove Password Fail !! \n\n");
			goto spalapet_end;
		}
	}

	//restore data
	if(StorageWaitBusy())
	{
		goto spalapet_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x04, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spalapet_end;
	}
	
	goto spalapet_endend;
	
spalapet_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

spalapet_endend:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void StoragePrivateAreaTestWithoutPassword(uint8_t *cp)
{
	uint8_t i, tmp;
	uint8_t wbuf[128], rbuf[128], rbuf_bak[128], buf[2];

	clrscr();
	Print(L"Storage Test - Private Area Test Without Password. \n\n");

	rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, rbuf);
	// available ?
	if((rbuf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Storage Not Available !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spanpt_endend;
	}

	//busy ?
	if((rbuf[0] & 0x2) == 0x02)
	{
		console_color(RED, BLACK);
		Print(L"Storage Is Busy !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto spanpt_endend;
	}

	//password present ?
	if((rbuf[0] & 0x20) == 0x20)
	{
		if((rbuf[0] & 0x04) == 0x04)
		{
			if(StorageWaitBusy())
			{
				goto spanpt_endend;
			}
			if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
			{
				Print(L"PMC Communicate Fail !! Unlock Password Fail !! \n\n");
				goto spanpt_end;
			}
		}

		if(StorageWaitBusy())
		{
			goto spanpt_endend;
		}
		if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x03, 0, 10, cp))
		{
			Print(L"PMC Communicate Fail !! Remove Password Fail !! \n\n");
			goto spanpt_end;
		}
	}

	//read now in private area value
	Print(L"Read Private Area... \n");
	if(StorageWaitBusy())
	{
		goto spanpt_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x04, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spanpt_end;
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
	if(StorageWaitBusy())
	{
		goto spanpt_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x04, 0, 128, wbuf))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spanpt_end;
	}

	//read private area
	Print(L"Read And Compare Private Area... \n");
	if(StorageWaitBusy())
	{
		goto spanpt_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x04, 0, 128, rbuf))
	{
		Print(L"PMC Communicate Fail !! Read Private Area Fail !! \n\n");
		goto spanpt_end;
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
			goto spanpt_endend;
		}
	}

	console_color(GREEN, BLACK);
	Print(L"Compare Success !! \n");

	//restore data
	if(StorageWaitBusy())
	{
		goto spanpt_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x04, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto spanpt_end;
	}
	
	goto spanpt_endend;
	
spanpt_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

spanpt_endend:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void StorageGeneralAreaTest(void)
{
	uint8_t i, tmp;
	uint8_t wbuf[128], rbuf[128], rbuf_bak[128], buf[2];

	clrscr();
	Print(L"Storage Test - General Area Test. \n\n");

	rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, rbuf);
	// available ?
	if((rbuf[0] & 0x1) == 0)
	{
		console_color(RED, BLACK);
		Print(L"Storage Not Available !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto sgat_endend;
	}

	//busy ?
	if((rbuf[0] & 0x2) == 0x02)
	{
		console_color(RED, BLACK);
		Print(L"Storage Is Busy !! \n");
		console_color(LIGHTGRAY, BLACK);
		goto sgat_endend;
	}

	//read now in private area value
	Print(L"Read General Area... \n");
	if(StorageWaitBusy())
	{
		goto sgat_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x05, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Read General Area Fail !! \n\n");
		goto sgat_end;
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

	//write general area
	Print(L"Write General Area... \n\n");
	for(i=0; i<128; i++)
	{
		wbuf[i] = tmp++;
	}
	if(StorageWaitBusy())
	{
		goto sgat_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x05, 0, 128, wbuf))
	{
		Print(L"PMC Communicate Fail !! Write General Area Fail !! \n\n");
		goto sgat_end;
	}

	//read general area
	Print(L"Read And Compare General Area... \n");
	if(StorageWaitBusy())
	{
		goto sgat_endend;
	}
	if(rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x05, 0, 128, rbuf))
	{
		Print(L"PMC Communicate Fail !! Read General Area Fail !! \n\n");
		goto sgat_end;
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
		
		if(rbuf[0] != wbuf[0])
		{
			console_color(RED, BLACK);
			Print(L"Compare Fail !! \n");
			goto sgat_endend;
		}
	}

	console_color(GREEN, BLACK);
	Print(L"Compare Success !! \n");

	//restore data
	if(StorageWaitBusy())
	{
		goto sgat_endend;
	}
	if(rdc_ec_write_prot(RDC_FW_CMD_WR_STORAGE, 0x05, 0, 128, rbuf_bak))
	{
		Print(L"PMC Communicate Fail !! Write Private Area Fail !! \n\n");
		goto sgat_end;
	}

	goto sgat_endend;
	
sgat_end:
	show_prot(RDC_FW_CMD_PMC1, 0x00, 0x00, 1, buf);

sgat_endend:
	console_color(LIGHTGRAY, BLACK);
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(!CheckESC(GetKey())){}
}

void StorageTest(void)
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
		Print(L"4. Storage Test - General Area Test. \n");
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
				StoragePrivateAreaLockTestWithPassword(1, cpbuf, epbuf);
				break;
			}
			case '2':
			{
				StoragePrivateAreaLockTestWithPassword(0, cpbuf, epbuf);
				break;
			}
			case '3':
			{
				StoragePrivateAreaTestWithoutPassword(cpbuf);
				break;
			}
			case '4':
			{
				StorageGeneralAreaTest();
				break;
			}
		}
	}
}

uint8_t CheckBusLDN(uint8_t type)
{
	uint8_t ch, ldn = 0, u8tmp, buf[2];
	uint16_t u16temp;

	for(ch=0; ch<2; ch++)
	{
		buf[ch] = 0;
		
		//Open PNP port
		sys_open_ioport(RDC_PNP_INDEX);
		sys_open_ioport(RDC_PNP_DATA);
		
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
			return 1;
		}

		switch(type)
		{
			//Smbus
			case 0:
			{
				if(ch == 0)
				{
					ldn = RDC_LDN_SMB0;
				}
				else
				{
					ldn = RDC_LDN_SMB1;
				}
				break;
			}
			//I2C
			case 1:
			{
				if(ch == 0)
				{
					ldn = RDC_LDN_I2C0;
				}
				else
				{
					ldn = RDC_LDN_I2C1;
				}
				break;
			}
			//CAN Bus
			case 2:
			{
				if(ch == 0)
				{
					ldn = RDC_LDN_CAN0;
				}
				else
				{
					ldn = RDC_LDN_CAN1;
				}
				break;
			}
		}

		//Select Logic Device
		outp(RDC_CFG_IDX_PORT, 0x07);
		outp(RDC_CFG_DAT_PORT, ldn);

		outp(RDC_CFG_IDX_PORT, 0x30);
		u8tmp = inp(RDC_CFG_DAT_PORT);
		if(u8tmp == 0)
		{
			continue;
		}
		
		//Get Bus Base Address
		outp(RDC_CFG_IDX_PORT, 0x60);
		u16temp = inp(RDC_CFG_DAT_PORT);
		u16temp <<= 8;
		outp(RDC_CFG_IDX_PORT, 0x61);
		u16temp |= inp(RDC_CFG_DAT_PORT);
		if((u16temp == 0) || (u16temp == 0xFFFF))
		{
			continue;
		}
		
		//Close PNP port
		outp(RDC_CFG_IDX_PORT, 0xAA);	// exit config
		sys_close_ioport(RDC_PNP_INDEX);
		sys_close_ioport(RDC_PNP_DATA);

		buf[ch] = 1;
	}

	if((buf[0] == 0) && buf[1] == 0)
	{
		return 1;
	}
	
	return 0;
}

void AddTestItem(uint8_t *module_valid_cnt, uint8_t *module_valid_id)
{
	uint8_t step = 0;
	uint16_t aec_type;

	aec_type = aec_get_type();

	//Board Information
	*(module_valid_id + step) = gModuleTable[0].id;
	step++;

	//Hardware Monitor
	*(module_valid_id + step) = gModuleTable[1].id;
	step++;

	//Thermal Test
	*(module_valid_id + step) = gModuleTable[2].id;
	step++;

	//Watchdog Test
	*(module_valid_id + step) = gModuleTable[3].id;
	step++;

	//Fan test
	*(module_valid_id + step) = gModuleTable[4].id;
	step++;

	//GPIO Test
	*(module_valid_id + step) = gModuleTable[5].id;
	step++;

	//Panel Test
	*(module_valid_id + step) = gModuleTable[6].id;
	step++;

	//SMBUS Test
	*(module_valid_id + step) = gModuleTable[7].id;
	step++;

	//I2C Test
	*(module_valid_id + step) = gModuleTable[8].id;
	step++;

	//Case Open Test
	*(module_valid_id + step) = gModuleTable[9].id;
	step++;

	//Storage Test
	*(module_valid_id + step) = gModuleTable[10].id;
	step++;

	//CAN Bus Test
	*(module_valid_id + step) = gModuleTable[11].id;
	step++;

	//Smbus Use I2C Mode Test
	*(module_valid_id + step) = gModuleTable[12].id;
	step++;

	*module_valid_cnt = step;
}

void CheckModuleAvailable(uint8_t *module_valid_cnt, uint8_t *module_valid_id)
{
	uint8_t i, step = 0;
	uint8_t buf[8];
	uint16_t aec_type;

	aec_type = aec_get_type();

	//Board Information
	*(module_valid_id + step) = gModuleTable[0].id;
	step++;

	//Hardware Monitor
	*(module_valid_id + step) = gModuleTable[1].id;
	step++;

	//Thermal Test
	for(i=0; i<4; i++)
	{
		switch(aec_type)
		{
			case AEC_TYPE_RDC_FW:
			{
				rdc_ec_read_prot(RDC_FW_CMD_RD_THERM, 0x00, i, 2, buf);
				break;
			}
			case AEC_TYPE_RDC_SW:
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_THERM, i, 0x00, 1, buf);
				break;
			}
		}
		if((buf[0] & 0x01) == 0x01)
		{
			*(module_valid_id + step) = gModuleTable[2].id;
			step++;
			break;
		}
	}

	//Watchdog Test
	switch(aec_type)
	{
		case AEC_TYPE_RDC_FW:
		{
			rdc_ec_read_prot(RDC_FW_CMD_RD_WDT, 0x00, 0, 1, buf);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			rdc_sw_ec_read_prot(RDC_SW_CMD_RD_WDT, 0xFF, 0x00, 1, buf);
			break;
		}
	}
	if((buf[0] & 0x01) == 0x01)
	{
		*(module_valid_id + step) = gModuleTable[3].id;
		step++;
	}

	//Fan test
	for(i=0; i<4; i++)
	{
		switch(aec_type)
		{
			case AEC_TYPE_RDC_FW:
			{
				rdc_ec_read_prot(RDC_FW_CMD_RD_FAN, 0x00, i, 1, buf);
				break;
			}
			case AEC_TYPE_RDC_SW:
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FAN, i, 0x00, 1, buf);
				break;
			}
		}
		if((buf[0] & 0x01) == 0x01)
		{
			*(module_valid_id + step) = gModuleTable[4].id;
			step++;
			break;
		}
	}

	//GPIO Test
	switch(aec_type)
	{
		case AEC_TYPE_RDC_FW:
		{
			rdc_ec_read_prot(RDC_FW_CMD_RD_GPIO, 0x00, 0, 1, buf);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			for(i=0; i<12; i++)
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_GPIO_STS, 0xFF, i, 1, buf);
				if((buf[0] & 0x01) == 0x01)
				{
					break;
				}
			}
			break;
		}
	}
	if((buf[0] & 0x01) == 0x01)
	{
		*(module_valid_id + step) = gModuleTable[5].id;
		step++;
	}

	//Panel Test
	for(i=0; i<2; i++)
	{
		switch(aec_type)
		{
			case AEC_TYPE_RDC_FW:
			{
				rdc_ec_read_prot(RDC_FW_CMD_RD_PANEL, 0x00, i, 1, buf);
				break;
			}
			case AEC_TYPE_RDC_SW:
			{
				rdc_sw_ec_read_prot(RDC_SW_CMD_RD_PANEL, i, 0x00, 1, buf);
				break;
			}
		}
		if((buf[0] & 0x01) == 0x01)
		{
			*(module_valid_id + step) = gModuleTable[6].id;
			step++;
			break;
		}
	}

	//SMBUS Test
	if(CheckBusLDN(0) == 0)
	{
		*(module_valid_id + step) = gModuleTable[7].id;
		step++;
	}

	//I2C Test
	if(CheckBusLDN(1) == 0)
	{
		*(module_valid_id + step) = gModuleTable[8].id;
		step++;
	}

	//Case Open Test
	switch(aec_type)
	{
		case AEC_TYPE_RDC_FW:
		{
			rdc_ec_read_prot(RDC_FW_CMD_RD_CASEOPEN, 0x00, 0, 1, buf);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			rdc_sw_ec_read_prot(RDC_SW_CMD_RD_CASEOPEN, 0xFF, 0x00, 1, buf);
			break;
		}
	}
	if((buf[0] & 0x01) == 0x01)
	{
		*(module_valid_id + step) = gModuleTable[9].id;
		step++;
	}

	//Storage Test
	switch(aec_type)
	{
		case AEC_TYPE_RDC_FW:
		{
			rdc_ec_read_prot(RDC_FW_CMD_RD_STORAGE, 0x00, 0, 1, buf);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			StorageSWProtocolGetStatus(RDC_SW_CMD_RD_STORAGE, 0xFF, 0x00, 1, buf);
			break;
		}
	}
	if((buf[0] & 0x01) == 0x01)
	{
		*(module_valid_id + step) = gModuleTable[10].id;
		step++;
	}

	//CAN Bus Test
	if(CheckBusLDN(2) == 0)
	{
		*(module_valid_id + step) = gModuleTable[11].id;
		step++;
	}

	//Smbus Use I2C Mode Test
	if(CheckBusLDN(0) == 0)
	{
		*(module_valid_id + step) = gModuleTable[12].id;
		step++;
	}

	*module_valid_cnt = step;
}

uint8_t GetSomeBoardInfo(uint8_t *type, uint16_t *ver, uint16_t *bname, uint16_t *btime)
{
	uint8_t buf[32]={'\0'};
	
	switch(aec_get_type())
	{
		case AEC_TYPE_RDC_FW:
		{
			if(rdc_ec_read_prot(RDC_FW_CMD_RD_BOARD, 0x10, 0, 16, buf))
			{
				Print(L"Read Board Name Fail !! \n\n");
				return 1;
			}
			AsciiToUnicodeString(buf, bname);

			if(rdc_ec_read_prot(RDC_FW_CMD_RD_BOARD, 0x22, 0, 16, buf))
			{
				Print(L"Read FW Version Fail !! \n\n");
				return 1;
			}
			AsciiToUnicodeString(buf, ver);

			if(rdc_ec_read_prot(RDC_FW_CMD_RD_BOARD, 0x23, 0, 26, buf))
			{
				Print(L"Read Board Build Time Fail !! \n\n");
				return 1;
			}
			AsciiToUnicodeString(buf, btime);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			uint8_t len;

			if(rdc_sw_ec_read_prot(RDC_SW_CMD_PLAT_INFO, 0xFF, 0x06, 1, buf))
			{
				Print(L"Read Board Name Length Fail !! \n\n");
				return 1;
			}
			len = buf[0];
			if(rdc_sw_ec_read_prot(RDC_SW_CMD_PLAT_INFO, 0xFF, 0x07, len, buf))
			{
				Print(L"Read Board Name Fail !! \n\n");
				return 1;
			}
			AsciiToUnicodeString(buf, bname);

			if(rdc_sw_ec_read_prot(RDC_SW_CMD_RD_FWVER, 0xFF, 0x00, 9, buf))
			{
				Print(L"Read FW Version Fail !! \n\n");
				return 1;
			}
			AsciiToUnicodeString(buf, ver);
			break;
		}
	}

	return 0;
}

void DisplayToolHead(uint8_t type, uint16_t *ver, uint16_t *bname, uint16_t *btime)
{
	clrscr();
	Print(L"%s ver: v%d.%d \n", tool_name, major_v, minor_v);
	
	switch(aec_get_type())
	{
		case AEC_TYPE_RDC_FW:
		{
			Print(L"Board:%s   ", bname);
			Print(L"Firmware Version:%s \n", ver);
			Print(L"Build Time: %s \n\n", btime);
			break;
		}
		case AEC_TYPE_RDC_SW:
		{
			Print(L"Board:%s   ", bname);
			Print(L"Firmware Version:%s \n\n", ver);
			//Print(L"Build Time: %s \n\n", btime);
			break;
		}
	}
}

void ShowMainMenu(uint8_t *module_valid_cnt, uint8_t *module_valid_id)
{
	uint8_t i;

	Print(L"Select Test Function. \n");
	
	for(i=0; i<*module_valid_cnt; i++)
	{
		Print(L"%X. %s. \n", i, gModuleTable[module_valid_id[i]].str);
	}

	Print(L"Please input: \n");

	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
}

EFI_STATUS main(int argc, char * * argv)
{
	struct
	{
		uint8_t fw_type;
		uint16_t fw_ver[16];
		uint16_t bname[17];
		uint16_t btime[27];
	}head;
	
	int key;
	uint8_t res;
	uint8_t module_valid_cnt = 0, module_valid_id[12] = {0};
	uint16_t aec_type;
	EFI_STATUS status;

	status = console_init();
	if(status != EFI_SUCCESS)
	{
		Print(L"ERROR: Failed to change console setting. \n");
		return status;
	}
	
	res = aec_init();
	if (res != AEC_OK)
	{
		Print(L"aec_init err! \n");
		return -1;
	}

	aec_type = aec_get_type();
	if((aec_type != AEC_TYPE_RDC_FW) && (aec_type != AEC_TYPE_RDC_SW))
	{
		Print(L"EC Type Can Not Support !! \n\n");
		return -1;
	}
	
	//CheckModuleAvailable(&module_valid_cnt, module_valid_id);
	AddTestItem(&module_valid_cnt, module_valid_id);
	if(GetSomeBoardInfo(&head.fw_type, head.fw_ver, head.bname, head.btime))
	{
		return -1;
	}
	
	while(1)
	{
		DisplayToolHead(head.fw_type, head.fw_ver, head.bname, head.btime);
		ShowMainMenu(&module_valid_cnt, module_valid_id);
		
		key = GetKey();
		if(CheckESC(key))
		{
			break;
		}

		if(((key & 0xFF) >= '0') && ((key & 0xFF) <= '9'))
		{
			key = (key & 0xFF) - '0';
			if((key >= 0) && (key < module_valid_cnt))
			{
				switch(aec_type)
				{
					case AEC_TYPE_RDC_FW:
					{
						gModuleTable[module_valid_id[key]].funcfw();
						break;
					}
					case AEC_TYPE_RDC_SW:
					{
						gModuleTable[module_valid_id[key]].funcsw();
						break;
					}
				}
			}
		}
		else if((((key & 0xFF) >= 'A') && ((key & 0xFF) <= 'F')) || (((key & 0xFF) >= 'a') && ((key & 0xFF) <= 'f')))
		{
			if(module_valid_cnt > 10)
			{
				if((key & 0xFF) >= 'a')
				{
					key =((key & 0xFF) - 'a') ;
				}
				else
				{
					key = ((key & 0xFF) - 'A');
				}
				
				if((key >= 0) && ((key + 0x0A) < module_valid_cnt))
				{
					switch(aec_type)
					{
						case AEC_TYPE_RDC_FW:
						{
							gModuleTable[module_valid_id[key + 0x0A]].funcfw();
							break;
						}
						case AEC_TYPE_RDC_SW:
						{
							gModuleTable[module_valid_id[key + 0x0A]].funcsw();
							break;
						}
					}
				}
			}
		}
	}

	clrscr();
	console_exit();
	
	return EFI_SUCCESS;
}

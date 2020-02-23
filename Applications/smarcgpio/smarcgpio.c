#include <smarcgpio.h>



uint16_t vertype = 'v';
uint16_t major_Ver = 0x00;
uint16_t minor_Ver = 0x01;
uint16_t *project = L"SmarcGpio";

uint32_t gMMIOBaseAddress = 0;

static struct{
	uint32_t chip_pin_config[5];
}gBak;



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
	uint16_t name[ProjectNameLength + 1];
	UINTN len = 0;
	
	clrscr();
	ConsoleCurrRow = 0;
	gotoxy(0, ConsoleCurrRow);
	Print(L"%s ver:%c%02X_%02X", project, vertype, major_Ver, minor_Ver);
	ConsoleCurrRow++;
	if (mbox.getAccessType() == MBXCh_ITEMBox)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Mailbox access type: ITE HW Mailbox. Offset:0x%X", mbox.hw_offset);
		ConsoleCurrRow++;
	}
	else if (mbox.getAccessType() == MBXCh_AdtIO)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Mailbox access type: Advantech IO channel");
		ConsoleCurrRow++;
	}
	else if (mbox.getAccessType() == MBXCh_ACPIIO)
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"Mailbox access type: ACPI EC Space");
		ConsoleCurrRow++;
	}
	gotoxy(0, ConsoleCurrRow);
	AsciiStrnToUnicodeStrS(project_name, ProjectNameLength + 1, name, ProjectNameLength + 1, &len);
	Print(L"Project:%s TableCode:%02X KernelVersion:%02X_%02X",
			name, version[0], version[1], version[2]);
	ConsoleCurrRow++;
	gotoxy(0, ConsoleCurrRow);
	Print(L"ICVendor:%c ICCode:%02X ProjectID:%02X PorjectType:%c ProjectVersion:%02X_%02X\n",
			version[3] ,version[4] ,version[5] ,version[6] ,version[7] ,version[8]);
	ConsoleCurrRow++;
}

uint8_t CheckEC(uint8_t ic_code)
{
	if(ic_code == EC_IC_CODE)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t CheckProject(uint8_t *pjname)
{
	if(strstr(pjname, PROJECT_NAME) == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t GetAndCheckMMIOBaseAddress(PCI_BUS *pbus)
{
	PCI_DEVICE	*pdev;
	uint8_t ret = 0;
	
	pdev = pci_find_function(pbus, 0, 13, 0);
	if(pdev->vendor_id == 0x8086)		//intel
	{
		gMMIOBaseAddress = pdev->bar[0] & 0xFFFFFFFE0;
	}
	
	if((gMMIOBaseAddress == 0xFFFFFFFF) || (gMMIOBaseAddress == 0))
	{
		Print(L"MMIO base address fail !! \n");
		ret = 1;
	}

	return ret;
}

uint8_t CheckSupportedChipset(void)
{
	PCI_BUS		*pbus = NULL;
	PCI_DEVICE	*pdev;
	uint8_t ret = 1;
	
	if(EFIPCIInit())
	{
		Print(L"PCI Initial Fail  !! \n\n");
		goto fail_end;
	}

	pbus = pci_bus_scan();
	if(pbus == NULL)
	{
		goto fail_end;
	}

	// Vendor Check : Bus 0, Device 0, Function 0
	pdev = pci_find_function(pbus, 0, 0, 0);
	if(pdev)
	{
		if(pdev->vendor_id == 0x8086)		//intel
		{
			if((pdev->device_id & 0xFF00) == 0x5A00)	//apollo lake
			{
				if(GetAndCheckMMIOBaseAddress(pbus))
				{
					goto fail_end;
				}
				ret = 0;
			}
		}
	}

fail_end:
	if(pbus != NULL)
	{
		pci_bus_free(pbus);
	}
	
	return ret;
}

void BackupChipsetPinConfiguration(void)
{
	uint32_t reg_addr;

	//GPIO 69 GP_CAMERASB7
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_69;
	gBak.chip_pin_config[0] = MmioRead32(reg_addr);

	//GPIO 70 GP_CAMERASB8
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_70;
	gBak.chip_pin_config[1] = MmioRead32(reg_addr);

	//GPIO 71 GP_CAMERASB9
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_71;
	gBak.chip_pin_config[2] = MmioRead32(reg_addr);

	//GPIO 72 GP_CAMERASB9
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_72;
	gBak.chip_pin_config[3] = MmioRead32(reg_addr);

	//GPIO 84 AVS_I2S2_MCLK
	reg_addr = gMMIOBaseAddress | (NW_PORT_ID << 16) | PAD_CFG_DW0_GPIO_84;
	gBak.chip_pin_config[4] = MmioRead32(reg_addr);
}

void RestoreChipsetPinFunction(void)
{
	uint32_t reg_addr;

	//GPIO 69 GP_CAMERASB7
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_69;
	MmioWrite32(reg_addr, gBak.chip_pin_config[0]);

	//GPIO 70 GP_CAMERASB8
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_70;
	MmioWrite32(reg_addr, gBak.chip_pin_config[1]);

	//GPIO 71 GP_CAMERASB9
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_71;
	MmioWrite32(reg_addr, gBak.chip_pin_config[2]);

	//GPIO 72 GP_CAMERASB9
	reg_addr = gMMIOBaseAddress | (N_PORT_ID << 16) | PAD_CFG_DW0_GPIO_72;
	MmioWrite32(reg_addr, gBak.chip_pin_config[3]);

	//GPIO 84 AVS_I2S2_MCLK
	reg_addr = gMMIOBaseAddress | (NW_PORT_ID << 16) | PAD_CFG_DW0_GPIO_84;
	MmioWrite32(reg_addr, gBak.chip_pin_config[4]);
}

void GetChipsetGPIOState(uint8_t port_id, uint16_t offset, uint8_t *data)
{
	uint32_t reg_addr;
	uint32_t tmp;
	
	reg_addr = gMMIOBaseAddress | (port_id << 16) | offset;
	tmp = MmioRead32(reg_addr);
	
	*data = (tmp >> 1) & 0x01;
}

void SetChipsetGPIOState(uint8_t port_id, uint16_t offset, uint8_t data)
{
	uint32_t reg_addr;
	uint32_t tmp;
	
	reg_addr = gMMIOBaseAddress | (port_id << 16) | offset;
	tmp = MmioRead32(reg_addr);
	
	tmp &= ~0x01;
	tmp |= data;
	MmioWrite32(reg_addr, tmp);
}

void SetChipsetPinInGPIOMode(uint8_t port_id, uint16_t offset, uint8_t mode)
{
	uint32_t reg_addr;
	uint32_t tmp;
	
	reg_addr = gMMIOBaseAddress | (port_id << 16) | offset;
	tmp = MmioRead32(reg_addr);
	
	//set pin mode in GPIO
	if(port_id == N_PORT_ID)
	{
		tmp &= ~(0x01 << 10);
	}
	else if(port_id == NW_PORT_ID)
	{
		tmp &= ~(0x03 << 10);
	}
	
	//set GPIO mode
	if(mode == INPUT)//input
	{
		tmp |= (0x01 << 8);	//tx disable
		tmp &= ~(0x01 << 9);	//rx enable
	}
	else if(mode == OUTPUT)//output
	{
		tmp &= ~(0x01 << 8);	//tx enable
		tmp &= ~(0x01 << 9);	//rx enable
	}
	
	MmioWrite32(reg_addr, tmp);
}

void GPIOAutoTest()
{
	uint8_t i, data, buf[6];
	uint8_t hresult = 0, lresult = 0;
	UINTN y_back;
	
	clrscr();
	Print(L"GPIO Auto Test \n");
	
	Print(L"Please connect gpio as follow. \n");
	y_back = ConsoleCurrRow = 2;
	
	for(i=0; i<6 ; i++)
	{
		gotoxy(5, ConsoleCurrRow);
		Print(L"GPIO%-2d  <-------->   GPIO%-2d", i, (i + 6));
		ConsoleCurrRow++;
	}
	
	gotoxy(0, ConsoleCurrRow);
	Print(L"Press any key to start test...");
	ConsoleCurrRow ++;
	
	bioskey(0);
	
	//test low pin to high pin
	console_color(YELLOW, BLACK);
	gotoxy(45, ConsoleCurrRow);
	Print(L"High");
	gotoxy(55, ConsoleCurrRow);
	Print(L"Low");
	ConsoleCurrRow ++;
	
	y_back = ConsoleCurrRow;
	for(i=0; i<6; i++)
	{
		gotoxy(5, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"GPIO%-2d  -------->    GPIO%-2d", i, (i + 6));
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;
	}
	
	//set direction
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_69, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_70, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_72, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_71, OUTPUT);
	SetChipsetPinInGPIOMode(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio05, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio06, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio07, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio08, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio09, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio10, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio11, INPUT);
	
	//set high
	data = HighActive;
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, HighActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, HighActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, HighActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, HighActive);
	SetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, HighActive);
	mApp.dyna->setPinstate(DDevNo_AltGpio05, &data);
	//get state
	mApp.dyna->getPinstate(DDevNo_AltGpio06, &buf[0]);
	mApp.dyna->getPinstate(DDevNo_AltGpio07, &buf[1]);
	mApp.dyna->getPinstate(DDevNo_AltGpio08, &buf[2]);
	mApp.dyna->getPinstate(DDevNo_AltGpio09, &buf[3]);
	mApp.dyna->getPinstate(DDevNo_AltGpio10, &buf[4]);
	mApp.dyna->getPinstate(DDevNo_AltGpio11, &buf[5]);
	//store result
	/*for(i=6; i>0; i--)
	{
		hresult <<= 1;
		hresult |= (buf[i - 1] & 0x01);
	}*/
	//check result
	ConsoleCurrRow = y_back;
	for(i=0; i<6; i++)
	{
		gotoxy(45, ConsoleCurrRow);
		if(buf[i] & 0x01)
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
	
	//set low
	data = LowActive;
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, LowActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, LowActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, LowActive);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, LowActive);
	SetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, LowActive);
	mApp.dyna->setPinstate(DDevNo_AltGpio05, &data);
	//get state
	mApp.dyna->getPinstate(DDevNo_AltGpio06, &buf[0]);
	mApp.dyna->getPinstate(DDevNo_AltGpio07, &buf[1]);
	mApp.dyna->getPinstate(DDevNo_AltGpio08, &buf[2]);
	mApp.dyna->getPinstate(DDevNo_AltGpio09, &buf[3]);
	mApp.dyna->getPinstate(DDevNo_AltGpio10, &buf[4]);
	mApp.dyna->getPinstate(DDevNo_AltGpio11, &buf[5]);
	//store result
	/*for(i=6; i>0; i--)
	{
		lresult <<= 1;
		lresult |= (buf[i - 1] & 0x01);
	}*/
	
	//check result
	ConsoleCurrRow = y_back;
	for(i=0; i<6; i++)
	{
		gotoxy(55, ConsoleCurrRow);
		if((buf[i] & 0x01) == 0)
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
	
	delay(1000);
	
	//test high pin to low pin
	console_color(YELLOW, BLACK);
	gotoxy(45, ConsoleCurrRow);
	Print(L"High");
	gotoxy(55, ConsoleCurrRow);
	Print(L"Low");
	ConsoleCurrRow ++;
	
	y_back = ConsoleCurrRow;
	for(i=0; i<6; i++)
	{
		gotoxy(5, ConsoleCurrRow);
		console_color(YELLOW, BLACK);
		Print(L"GPIO%-2d  <--------    GPIO%-2d", i, (i + 6));
		console_color(LIGHTGRAY, BLACK);
		ConsoleCurrRow++;
	}
	
	hresult = 0;
	lresult = 0;
	
	//set direction
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_69, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_70, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_72, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_71, INPUT);
	SetChipsetPinInGPIOMode(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio05, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio06, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio07, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio08, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio09, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio10, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio11, OUTPUT);
	
	//set high
	data = HighActive;
	mApp.dyna->setPinstate(DDevNo_AltGpio06, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio07, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio08, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio09, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio10, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio11, &data);
	//get state
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, &buf[0]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, &buf[1]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, &buf[2]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, &buf[3]);
	GetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, &buf[4]);
	mApp.dyna->getPinstate(DDevNo_AltGpio05, &buf[5]);
	//store result
	/*for(i=6; i>0; i--)
	{
		hresult <<= 1;
		hresult |= (buf[i - 1] & 0x01);
	}*/
	//check result
	ConsoleCurrRow = y_back;
	for(i=0; i<6; i++)
	{
		gotoxy(45, ConsoleCurrRow);
		if(buf[i] & 0x01)
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
	
	//set low
	data = LowActive;
	mApp.dyna->setPinstate(DDevNo_AltGpio06, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio07, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio08, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio09, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio10, &data);
	mApp.dyna->setPinstate(DDevNo_AltGpio11, &data);
	//get state
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, &buf[0]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, &buf[1]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, &buf[2]);
	GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, &buf[3]);
	GetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, &buf[4]);
	mApp.dyna->getPinstate(DDevNo_AltGpio05, &buf[5]);
	//store result
	/*for(i=6; i>0; i--)
	{
		lresult <<= 1;
		lresult |= (buf[i - 1] & 0x01);
	}*/
	
	//check result
	ConsoleCurrRow = y_back;
	for(i=0; i<6; i++)
	{
		gotoxy(55, ConsoleCurrRow);
		if((buf[i] & 0x01) == 0)
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
	
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}
}

void GPIOInputTest()
{
	int key = 0;
	uint8_t i, buf[6];
	
	clrscr();
	Print(L"GPIO Input Test \n");
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(LIGHTGRAY, BLACK);
	Print(L"Press ESC to return.");
	
	//set all pins as input
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_69, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_70, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_72, INPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_71, INPUT);
	SetChipsetPinInGPIOMode(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio05, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio06, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio07, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio08, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio09, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio10, INPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio11, INPUT);
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<12; i++)
	{
		Print(L"%2d ", i);
	}
	Print(L"\n");

	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Short I/O Pin To Gnd For Test");
	
	while(1)
	{
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}
		
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");
		
		//get state
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, &buf[0]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, &buf[1]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, &buf[2]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, &buf[3]);
		GetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, &buf[4]);
		mApp.dyna->getPinstate(DDevNo_AltGpio05, &buf[5]);
		for(i=0; i<6; i++)
		{
			Print(L"%2d ", buf[i] & 0x01);
		}
		
		mApp.dyna->getPinstate(DDevNo_AltGpio06, &buf[0]);
		mApp.dyna->getPinstate(DDevNo_AltGpio07, &buf[1]);
		mApp.dyna->getPinstate(DDevNo_AltGpio08, &buf[2]);
		mApp.dyna->getPinstate(DDevNo_AltGpio09, &buf[3]);
		mApp.dyna->getPinstate(DDevNo_AltGpio10, &buf[4]);
		mApp.dyna->getPinstate(DDevNo_AltGpio11, &buf[5]);
		for(i=0; i<6; i++)
		{
			Print(L"%2d ", buf[i] & 0x01);
		}
		Print(L"\n");
	}
	
	console_color(LIGHTGRAY, BLACK);
}

void GPIOOutputTest()
{
	int key = 0;
	uint8_t i, buf[6], data[12] = {0};
	
	clrscr();
	Print(L"GPIO Input Test \n");
	
	gotoxy(0, ConsoleHeight - 1);
	console_color(LIGHTGRAY, BLACK);
	Print(L"Press ESC to return.");
	
	//set all pins as output
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_69, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_70, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_72, OUTPUT);
	SetChipsetPinInGPIOMode(N_PORT_ID, PAD_CFG_DW0_GPIO_71, OUTPUT);
	SetChipsetPinInGPIOMode(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio05, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio06, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio07, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio08, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio09, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio10, OUTPUT);
	mApp.gpio->setSetting(DDevNo_AltGpio11, OUTPUT);
	
	//set level to low
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, data[0]);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, data[1]);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, data[2]);
	SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, data[3]);
	SetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, data[4]);
	mApp.dyna->setPinstate(DDevNo_AltGpio05, &data[5]);
	mApp.dyna->setPinstate(DDevNo_AltGpio06, &data[6]);
	mApp.dyna->setPinstate(DDevNo_AltGpio07, &data[7]);
	mApp.dyna->setPinstate(DDevNo_AltGpio08, &data[8]);
	mApp.dyna->setPinstate(DDevNo_AltGpio09, &data[9]);
	mApp.dyna->setPinstate(DDevNo_AltGpio10, &data[10]);
	mApp.dyna->setPinstate(DDevNo_AltGpio11, &data[11]);
	
	gotoxy(0, 5);
	Print(L"Pin    : ");
	for(i=0; i<12; i++)
	{
		if(i > 8)Print(L" ");
		Print(L" %2d ", i);
	}
	Print(L"\n");
	
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Key    : ");
	for(i=1; i<=12; i++)
	{
		Print(L" F%d ", i);
	}
	Print(L"\n");
	
	gotoxy(0, 20);
	console_color(EFI_CYAN, EFI_BLACK);
	Print(L"Press Key To Set Pin Level");
	
	while(1)
	{
		gotoxy(0, 7);
		console_color(EFI_YELLOW, EFI_BLACK);
		Print(L"Status : ");
		
		//get state
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, &buf[0]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, &buf[1]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, &buf[2]);
		GetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, &buf[3]);
		GetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, &buf[4]);
		mApp.dyna->getPinstate(DDevNo_AltGpio05, &buf[5]);
		for(i=0; i<6; i++)
		{
			Print(L" %2d ", buf[i] & 0x01);
		}
		
		mApp.dyna->getPinstate(DDevNo_AltGpio06, &buf[0]);
		mApp.dyna->getPinstate(DDevNo_AltGpio07, &buf[1]);
		mApp.dyna->getPinstate(DDevNo_AltGpio08, &buf[2]);
		mApp.dyna->getPinstate(DDevNo_AltGpio09, &buf[3]);
		mApp.dyna->getPinstate(DDevNo_AltGpio10, &buf[4]);
		mApp.dyna->getPinstate(DDevNo_AltGpio11, &buf[5]);
		for(i=0; i<6; i++)
		{
			if(i > 2)Print(L" ");
			Print(L" %2d ", buf[i] & 0x01);
		}
		Print(L"\n");
		
		key = (uint16_t)bioskey(1);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}
		
		//set new level
		switch(key)
		{
			case (SCAN_F1 << 8):
			{
				data[0] = ~data[0] & 0x01;
				SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_69, data[0]);
				break;
			}
			case (SCAN_F2 << 8):
			{
				data[1] = ~data[1] & 0x01;
				SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_70, data[1]);
				break;
			}
			case (SCAN_F3 << 8):
			{
				data[2] = ~data[2] & 0x01;
				SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_72, data[2]);
				break;
			}
			case (SCAN_F4 << 8):
			{
				data[3] = ~data[3] & 0x01;
				SetChipsetGPIOState(N_PORT_ID, PAD_CFG_DW0_GPIO_71, data[3]);
				break;
			}
			case (SCAN_F5 << 8):
			{
				data[4] = ~data[4] & 0x01;
				SetChipsetGPIOState(NW_PORT_ID, PAD_CFG_DW0_GPIO_84, data[4]);
				break;
			}
			case (SCAN_F6 << 8):
			{
				data[5] = ~data[5] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio05, &data[5]);
				break;
			}
			case (SCAN_F7 << 8):
			{
				data[6] = ~data[6] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio06, &data[6]);
				break;
			}
			case (SCAN_F8 << 8):
			{
				data[7] = ~data[7] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio07, &data[7]);
				break;
			}
			case (SCAN_F9 << 8):
			{
				data[8] = ~data[8] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio08, &data[8]);
				break;
			}
			case (SCAN_F10 << 8):
			{
				data[9] = ~data[9] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio09, &data[9]);
				break;
			}
			case (SCAN_F11 << 8):
			{
				data[10] = ~data[10] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio10, &data[10]);
				break;
			}
			case (SCAN_F12 << 8):
			{
				data[11] = ~data[11] & 0x01;
				mApp.dyna->setPinstate(DDevNo_AltGpio11, &data[11]);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	console_color(LIGHTGRAY, BLACK);
}

void MixGPIOTest(void)
{
	int key;
	
	while(1)
	{
		clrscr();
		proc_printHead();
		Print(L"\n\n");
		//Print(L"GPIO Test.\n\n");
		Print(L"0. GPIO Test - Auto\n");
		Print(L"1. GPIO Test - Input\n");
		Print(L"2. GPIO Test - Output\n");
		Print(L"Please Input: \n");

		gotoxy(0, ConsoleHeight - 1);
		Print(L"Press ESC to return.");

		key = bioskey(0);
		if(key == (SCAN_ESC << 8))
		{
			break;
		}
		
		switch(key & 0xFF)
		{
			case '0':
			{
				GPIOAutoTest();
				break;
			}
			case '1':
			{
				GPIOInputTest();
				break;
			}
			case '2':
			{
				GPIOOutputTest();
				break;
			}
		}
	}
}

EFI_STATUS main(int argc, char **argv)
{
	EFI_STATUS status;
	
	status = console_init();
	if(EFI_ERROR(status))
	{
		Print(L"ERROR: Failed to change console setting.\n");
		goto fail_end;
	}
	ConsoleCurrRow = 0;
	//setup mailbox access mbx_ch
	gotoxy(0, ConsoleCurrRow);

	//check supported chipset
	if(CheckSupportedChipset())
	{
		Print(L"This Platform Not Support !! \n\n");
		goto fail_end;
	}

	if(mApp.init() != _err_noerror) goto fail_end;
	
	switch(mbox.getAccessType())
	{
		case MBXCh_ITEMBox:
			Print(L"Using ITE HW mailbox.");
			ConsoleCurrRow++;
			break;
		case MBXCh_AdtIO:
			Print(L"Using IO mailbox");
			ConsoleCurrRow++;
			break;
		case MBXCh_ACPIIO:
			Print(L"Using ACPI mailbox");
			ConsoleCurrRow++;
			break;
		default:
			Print(L"Can not access mailbox");
			ConsoleCurrRow++;
			goto fail_end;
	}

	mApp.ec->readFwInfo(project_name,version);
	if (!proc_checkVersion(version[1], version[2]))
	{
		mApp.ec->readFwInfo(project_name, version);
		if (!proc_checkVersion(version[1], version[2]))
		{
			gotoxy(0, ConsoleCurrRow);
			Print(L"EC dos not support!!! %02x_%02x", version[1], version[2]);
			ConsoleCurrRow++;
			goto fail_end;
		}
	}
	if(CheckEC(version[4]))
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"EC dos not support!!! IC code = %x", version[4]);
		ConsoleCurrRow++;
		goto fail_end;
	}
	if(CheckProject(project_name))
	{
		gotoxy(0, ConsoleCurrRow);
		Print(L"This project dos not support!!!");
		ConsoleCurrRow++;
		goto fail_end;
	}
	mApp.dyna->getTable();

	//backup chipset pin configuration
	BackupChipsetPinConfiguration();
	
	//test menu
	MixGPIOTest();

	//restore chipset pin configuration
	RestoreChipsetPinFunction();
	
	goto end;

fail_end:
	gotoxy(0, ConsoleHeight - 1);
	Print(L"Press ESC to return.");
	while(bioskey(0) != (SCAN_ESC << 8)){}

end:
	mApp.exit();
	clrscr();
	console_exit();
}

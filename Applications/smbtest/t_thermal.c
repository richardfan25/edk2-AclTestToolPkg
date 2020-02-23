#include <stdio.h>

#include "x86hw.h"
#include "util.h"
#include "lm95245.h"
#include "tsmb.h"

//=============================================================================
void tmic_list_reg_data(uint8_t addr)
{
	uint16_t	u16data;
	uint8_t		u8data;
	
	u16data = (uint16_t)smbus->read_byte(smbus, addr, LM_REG_MFU_ID) << 8;
	u16data = u16data | (uint16_t)smbus->read_byte(smbus, addr, LM_REG_CHIP_REV);

	fprintf(stderr, "Thermal Chip ID: 0x%04X\n", u16data);
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_TEMP_H);
	fprintf(stderr, "Local Temp:  %3d C\n", u8data);
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_REMOTE_OFFSET_H);
	fprintf(stderr, "Remote Temp: %3d C\n", u8data);
	
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_T_CRIT_LMT);
	fprintf(stderr, "Local T_Crit Limit:  %3d C\n", u8data);

	u8data = smbus->read_byte(smbus, addr, LM_REG_REMOTE_OS_LMT);
	fprintf(stderr, "Remote OS Limit:     %3d C\n", u8data);
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_REMOTE_CRIT_LMT);
	fprintf(stderr, "Remote T_Crit Limit: %3d C\n", u8data);
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_COMM_HYSTERESIS);
	fprintf(stderr, "Common Hysteresis:    %2d C\n", u8data);
	
	u8data = smbus->read_byte(smbus, addr, LM_REG_STATUS_1);
	fprintf(stderr, "Status1:");
	if(u8data != 0)
	{
		fprintf(stderr, "\n");
		if(u8data & LM_STA1_BIT_BUSY)
			fprintf(stderr, "  *Temperature is converting.\n");
		if(u8data & LM_STA1_BIT_ROS)
			fprintf(stderr, "  *Remote OS asserted\n");
		if(u8data & LM_STA1_BIT_DIODE)
			fprintf(stderr, "  *Diode missing\n");
		if(u8data & LM_STA1_BIT_RTCRIT)
			fprintf(stderr, "  *Remote TCRIT asserted\n");
		if(u8data & LM_STA1_BIT_LOC)
			fprintf(stderr, "  *Locat OS and TCRIT asserted\n");
		
	}
	else
		fprintf(stderr, " No Event\n");
}

//=============================================================================
uint8_t show_thermal_option(void)
{
	char key;
	uint16_t u16;
	uint8_t	u8wb;
	uint8_t	u8rb;
	SMB_DEVICE	*dev;

	if(smbus == NULL)
		return 1;

	dev = tsmb_find_dev_type(smbus->dev_head, SMB_DEV_THERMAL_IC);

	if(dev == NULL)
	{
		fprintf(stderr, "ERROR: Failed to found LM95245 chip.\n");
		fprintf(stderr, "\npress any key to quit");
		while (bioskey(1) == 0);
		return 1;
	}
	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		//fprintf(stderr, "\n\n");
		print_title("Thermal Chip Test");
		
		fprintf(stderr, "1. Read Thermal IC Setting\n");
		fprintf(stderr, "2. Write New Threshold for OTP\n");
		fprintf(stderr, "q. Quit test\n\n");

		key = (char) (bioskey(0) & 0xFF);
		if(key == 'q' || key == 'Q')
			return 0;
		switch(key)
		{
			case '1':
				fprintf(stderr, "=== LM95245 Setting ===\n");
				tmic_list_reg_data(dev->addr);
				break;
			
			case '2':
				fprintf(stderr, "=== Set New OTP Threshold ===\n");
				
				do{
					u16 = get_number_stdin("Enter New OTP Threshold(1~127):", 0, 3);
					if(u16 == 0xFFFF)
						fprintf(stderr, "Get invalid number.\n");
					else if(u16 > 127 || u16 == 0)
						fprintf(stderr, "The number out of range.\n");
					else
						break;
					continue;
				} while(1);
				u8wb = (uint8_t)u16;
				fprintf(stderr, "Write %d to OTP Threshold ... ", u8wb);
				smbus->write_byte(smbus, dev->addr, LM_REG_T_CRIT_LMT, u8wb);
				u8rb = smbus->read_byte(smbus, dev->addr, LM_REG_T_CRIT_LMT);
				if(u8rb == u8wb)
					fprintf(stderr, "  PASS\n");
				else
					fprintf(stderr, "  FAIL\n");
				break;
			
			default:
				fprintf(stderr, "press Unknown key.\n");
				break;
		}
		fprintf(stderr, "\npress any key to continue");
		while (bioskey(1) == 0);
	} while(1);
	return 0;
}


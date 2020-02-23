
#include "util.h"
#include "x86hw.h"
#include "pca9555.h"

SMB_BUS	*smbus = NULL;

//=============================================================================
//  tsmb_check_spd_data
//=============================================================================
uint8_t tsmb_check_spd_data(uint8_t spd_addr)
{
	uint16_t	crc_spd;
	uint16_t	crc_cal;
	uint8_t		result;
	uint8_t		ddr_type;
	uint8_t		reg[128];
	int			i;
	
	result = 0;	
	ddr_type = 0xFF;

	for (i=0; i<128; i++)
	{
		reg[i] = smbus->read_byte( smbus, spd_addr, i );
	}
	
	// reg[2]
	// 0x0C : DDR4 SDRAM
	// 0x0B : DDR3 SDRAM
	// 0x08 : DDR2 SDRAM
	// 0x07 : DDR SDRAM
	// 0x04 : SDR SDRAM

	// DDR4
	if ( reg[2] == 0x0C )
	{
		// DDR4 : [00]~[7F]
		crc_spd	= reg[127];
		crc_spd <<= 8;
		crc_spd += reg[126];

		crc_cal = crc16( reg, 126 );

		if (crc_spd == crc_cal)
		{
			result |= 0x08;	// bit3
		}
		if ((result & 0x18) == 0x18)
		{
			ddr_type = 4;
		}
	}
	// DDR3
	else if ( reg[2] == 0x0B )
	{
		// DDR3 
		crc_spd	= reg[127];
		crc_spd <<= 8;
		crc_spd += reg[126];

		if ( reg[0] & 0x80 )
		{
			crc_cal = crc16( reg, 117 );
		}
		else
		{
			crc_cal = crc16( reg, 126 );
		}

		if ( crc_spd == crc_cal )
		{
			ddr_type = 3;
		}
	}
	// DDR2/DDR/SDR
	else if ( reg[2] == 0x08 || reg[2] == 0x07 || reg[2] == 0x04 )
	{
		// checksum
		crc_spd = reg[63];

		for (i=0, crc_cal=0; i<63; i++)
		{
			crc_cal += reg[i];
		}
		crc_cal &= 0xFF;

		if ( crc_spd == crc_cal )
		{
			if ( reg[2] == 0x08 )
			{
				ddr_type = 2;	// DDR2
			}
			else if ( reg[2] == 0x07 )
			{
				ddr_type = 1;	// DDR
			}
			else if ( reg[2] == 0x04 )
			{
				ddr_type = 0;	// SDR
			}
		}
	}

	return ddr_type;
}
//=============================================================================
//  tsmb_update_dev_type
//=============================================================================
void tsmb_update_dev_type(void)
{
	int			i;
	uint8_t		data;
	SMB_DEVICE	*dev;
	

	dev = smbus->dev_head;

	for (i=0; i<smbus->dev_num; i++)
	{
		// checking SPD data or not
		// A0~AE : SPD or EEPROM
		if ( (dev->addr & 0xF0) == 0xA0 )
		{
			data = tsmb_check_spd_data(dev->addr);
			if ( data == 0xFF )
			{
				// non-SPD data
				dev->type = SMB_DEV_EEPROM;
			}
			else
			{
				// SMB_DEV_SDR
				// SMB_DEV_DDR
				// SMB_DEV_DDR2
				// SMB_DEV_DDR3
				// SMB_DEV_DDR4
				dev->type = data;
			}
		}
		// 40~4E : LED Board (PCA9555)
		else if ( (dev->addr & 0xF0) == 0x40 )
		{
			if(	smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT0) == 0 &&
				smbus->read_byte(smbus, dev->addr, DIO_REG_INV_PORT1) == 0 &&
				smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG0) == 0xFF &&
				smbus->read_byte(smbus, dev->addr, DIO_REG_CONFIG1) == 0xFF)
				dev->type = SMB_DEV_LED_BOARD;
			else
				dev->type = SMB_DEV_UNKNOWN;
		}
		// D0 : DS1307, DS3231 
		else if ( dev->addr == 0xD0 )
		{
			dev->type = SMB_DEV_RTC;
		}
		else if ( dev->addr == 0x52 ||dev->addr == 0x30 ||
				  dev->addr == 0x32 || dev->addr == 0x98 ||
				  dev->addr == 0x9A)
		{
			data = smbus->read_byte(smbus, dev->addr, 0xFE);
			if(data == 0x01)
			{
				data = smbus->read_byte(smbus, dev->addr, 0xFF);
				if(data >= 0xB3)
					dev->type = SMB_DEV_THERMAL_IC;
				else
					dev->type = SMB_DEV_UNKNOWN;
			}
			else
				dev->type = SMB_DEV_UNKNOWN;
		}
		else
		{
			dev->type = SMB_DEV_UNKNOWN;
		}

		// next device
		dev = dev->next;
	}
}
//=============================================================================
//  tsmb_find_dev_type
//=============================================================================
SMB_DEVICE *tsmb_find_dev_type(SMB_DEVICE *st_dev, uint8_t type)
{
	SMB_DEVICE *dev;
	uint8_t i;
	
	dev = st_dev;
	
	for(i = 0; i<smbus->dev_num; i++)
	{
		if(dev->type == type)
			return dev;
		dev = dev->next;
	}
	return NULL;
}

//=============================================================================
//  tsmb_init
//=============================================================================
uint8_t tsmb_init(void)
{
	SMB_BUS	*smbus_tmp = NULL;
	
	smbus_tmp = smb_init();
	if ( !smbus_tmp )
	{
		return 1;
	}
	smbus = smbus_tmp;
	tsmb_update_dev_type();
	return 0;
}

//=============================================================================
//  tsmb_exit
//=============================================================================
void tsmb_exit(void)
{
	if ( smbus )
		smb_exit(smbus);
}

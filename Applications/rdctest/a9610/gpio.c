#include <Library/IoLib.h>

#include "gpio.h"
#include "aec.h"



EFI_STATUS RDCGPIOInit(UINT16 *bio)
{
	UINT8 act;

	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, RDC_LDN_GPIO0);

	// active
	IoWrite8(RDC_CFG_IDX_PORT, 0x30);
	act = IoRead8(RDC_CFG_DAT_PORT);
	if ((act & 0x1) == 0)
	{
		return EFI_NOT_FOUND;
	}
	
	// base
	IoWrite8(RDC_CFG_IDX_PORT, 0x60);	// MSB of base
	*bio = IoRead8(RDC_CFG_DAT_PORT);
	*bio <<= 8;

	IoWrite8(RDC_CFG_IDX_PORT, 0x61);	// LSB of base
	*bio |= IoRead8(RDC_CFG_DAT_PORT);
	
	// base : address error
	if (*bio == 0x0000 || *bio == 0xFFFF)
	{
		return EFI_NOT_FOUND;
	}

	IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC: exit config

	return EFI_SUCCESS;
}

EFI_STATUS GPIODataRW(UINT16 bio, UINT8 port, UINT8 *data, UINT8 rw)
{
	//set data
	if(rw == GPIO_WRITE)
	{
		IoWrite8(bio + RDC_GPIO_DATA_OFFSET + port, *data);
	}
	else if(rw == GPIO_READ)
	{
		*data =IoRead8 (bio + RDC_GPIO_DATA_OFFSET + port);
	}
	
	return EFI_SUCCESS;
}

EFI_STATUS GPIODirectionRW(UINT16 bio, UINT8 port, UINT8 *direction, UINT8 rw)
{
	//set direction
	if(rw == GPIO_WRITE)
	{
		IoWrite8(bio + RDC_GPIO_DIRECTION_OFFSET + port, *direction);
	}
	else if(rw == GPIO_READ)
	{
		*direction = IoRead8(bio + RDC_GPIO_DIRECTION_OFFSET + port);
	}
	
	return EFI_SUCCESS;
}

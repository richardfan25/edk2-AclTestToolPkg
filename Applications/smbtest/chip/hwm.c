#include "w83782.h"
#include "nct7802.h"
#include "hwm.h"

HWM_CHIP_INIT HmwChipInitList[HWM_CHIP_MAX] = {
	&hwm_w83782_init,
	&hwm_nct7802_init,
};

//=============================================================================
EFI_STATUS HardwareMonitorInit(
	SMB_BUS 		IN 	*SmbusHc,
	UINTN	 		IN 	ChipNameID,
	HWM_HANDLE_ST 	OUT	**Hwm
)
{
	if(SmbusHc == NULL)
		return EFI_NOT_READY;
	
	if(ChipNameID >= HWM_CHIP_MAX)
		return EFI_UNSUPPORTED;
	
	return HmwChipInitList[ChipNameID](SmbusHc, Hwm);
}

#include <stdio.h>

#include "x86hw.h"
#include "util.h"
#include "tsmb.h"

#include "nct7802.h"
#include "hwm.h"


HWM_HANDLE_ST	nct7802_hdl;
SMB_BUS 		*nct7802_Smbus = NULL;
uint8_t 		nct7802_bank = 0;

CHAR8 *nct7802_VoltName[NCT7802_MAXCH_VOLT] = {
	"VCC",
	"VCORE",
	"VSEN1",
	"VSEN2",
	"VSEN3",
};

CHAR8 *nct7802_TempName[NCT7802_MAXCH_TEMP] = {
	"LTemp",
};

CHAR8 *nct7802_FanName[NCT7802_MAXCH_FAN] = {
	"FAN1",
	"FAN2",
	"FAN3",
};

//=============================================================================
EFI_STATUS nct7802_ReadReg(
	IN	UINT8	Bank,
	IN	UINT8	Register,
	OUT	UINT8	*Value
)
{
	if (nct7802_Smbus == NULL) 
		return EFI_NOT_READY;

	if (Value == NULL) 
		return EFI_INVALID_PARAMETER;

	if (Bank > 1) 
		return EFI_INVALID_PARAMETER;

	if (Bank != nct7802_bank) {
		nct7802_Smbus->write_byte(nct7802_Smbus, nct7802_hdl.SmbAddr, NCT7802_REG_BANK_SEL, Bank);
		nct7802_bank = Bank;
	}

	*Value = nct7802_Smbus->read_byte(nct7802_Smbus, nct7802_hdl.SmbAddr, Register);

	return EFI_SUCCESS;
};

//=============================================================================
EFI_STATUS nct7802_WriteReg(
	IN	UINT8	Bank,
	IN	UINT8	Register,
	IN	UINT8	Value
)
{
	if (nct7802_Smbus == NULL) 
		return EFI_NOT_READY;

	if (Bank > 1) 
		return EFI_INVALID_PARAMETER;

	if (Bank != nct7802_bank) {
		nct7802_Smbus->write_byte(nct7802_Smbus, nct7802_hdl.SmbAddr, NCT7802_REG_BANK_SEL, Bank);
		nct7802_bank = Bank;
	}
	nct7802_Smbus->write_byte(nct7802_Smbus, nct7802_hdl.SmbAddr, Register, Value);

	return EFI_SUCCESS;
};


//=============================================================================
EFI_STATUS nct7802_ReadVoltage(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	UINT8		Hbyte, Lbyte;
	UINT8 		Reg;
	UINT16		Value;
	EFI_STATUS	Status;

	if (Index >= NCT7802_MAXCH_VOLT) 
		return EFI_UNSUPPORTED;

	switch (Index) {
	case NCT7802_INDEX_VCC:
		Reg = NCT7802_REG_MNTVCC;
		break;
	case NCT7802_INDEX_VCORE:
		Reg = NCT7802_REG_MNTVCORE;
		break;
	case NCT7802_INDEX_VSEN1:
		Reg = NCT7802_REG_MNTVSEN1;
		break;
	case NCT7802_INDEX_VSEN2:
		Reg = NCT7802_REG_MNTVSEN2;
		break;
	case NCT7802_INDEX_VSEN3:
		Reg = NCT7802_REG_MNTVSEN3;
		break;
	default:
		return EFI_UNSUPPORTED;
	}
	Status = nct7802_ReadReg(0, Reg, &Hbyte); 
	if (Status != EFI_SUCCESS) 
		return Status;

	Status = nct7802_ReadReg(0, NCT7802_REG_MNTV_LSB, &Lbyte);
	if (Status != EFI_SUCCESS) 
		return Status;

	Value = ((UINT16)Lbyte >> 6) | ((UINT16)Hbyte << 2);
	if (Index != NCT7802_INDEX_VCC) {
		Value = Value >> 1;
	}
	*Reading = Value; 				// 4mV/Count
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS nct7802_ReadTemp(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	UINT8		Lbyte;
	UINT8 		Reg;
	UINT16		Value;
	EFI_STATUS	Status;

	if (Index >= NCT7802_MAXCH_TEMP) 
		return EFI_UNSUPPORTED;

	Reg = NCT7802_REG_MNTLTD;

	Status = nct7802_ReadReg(0, Reg, &Lbyte);
	if (Status != EFI_SUCCESS) 
		return Status;

	Value = (UINT16)Lbyte;

	if (Lbyte & 0x80) {
		Value = 0xFF00 | Value;
	}

	*Reading = Value; // 1C
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS nct7802_ReadFan(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	UINT8		Hbyte, Lbyte;
	UINT8 		Reg;
	UINT32		Value;
	EFI_STATUS	Status;

	if (Index >= NCT7802_MAXCH_FAN) 
		return EFI_UNSUPPORTED;

	Reg = NCT7802_REG_MNTFAN1 + (UINT8)Index;

	Status = nct7802_ReadReg(0, Reg, &Hbyte);
	if (Status != EFI_SUCCESS) 
		return Status;

	Status = nct7802_ReadReg(0, NCT7802_REG_MNTFAN_LSB, &Lbyte);
	if (Status != EFI_SUCCESS) 
		return Status;

	Value = ((UINT32)Lbyte >> 3) | ((UINT32)Hbyte << 5);

	if (Value != 0) {
		if (Value < 0x1FFF) {
			Value = 1350000 / Value;
			if (Value > 0xFFFF)
				Value = 0xFFFF;
		}
		else {
			Value = 0xFFFF; 
		}
	}
	*Reading = (UINT16)Value; //1RPM
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS nct7802_GetReading(
	IN	UINTN	Type,
	IN	UINTN	Index,
	IN	UINT16	*Reading
)
{
	EFI_STATUS	Status;

	switch (Type) {
	case HWM_TYPE_VOLT:
		Status = nct7802_ReadVoltage(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	case HWM_TYPE_TEMP:
		Status = nct7802_ReadTemp(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	case HWM_TYPE_FAN:
		Status = nct7802_ReadFan(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	default:
		return EFI_UNSUPPORTED;
	}

	return EFI_SUCCESS;
}

//=============================================================================
EFI_STATUS nct7802_GetChannelName(
	IN	UINTN	Type,
	IN	UINTN	Index,
	IN	CHAR8	**Name				// max length 8
)
{
	switch (Type) {
	case HWM_TYPE_VOLT:
		*Name = nct7802_VoltName[Index];
		break;
	case HWM_TYPE_TEMP:
		*Name = nct7802_TempName[Index];
		break;
	case HWM_TYPE_FAN:
		*Name = nct7802_FanName[Index];
		break;
	default:
		*Name = NULL;
		return EFI_UNSUPPORTED;
	}
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS nct7802_GetStatus(
	IN		UINTN			Type,
	IN		UINTN			*Channels,
	IN OUT	HWM_STATUS_ST	**Status
)
{
	return EFI_UNSUPPORTED;
};

//=============================================================================
EFI_STATUS nct7802_init(
	IN	UINT8		 	SmbAddr,
	IN	HWM_HANDLE_ST 	*Hwm
)
{
	if (Hwm == NULL) 
		return EFI_INVALID_PARAMETER;

	Hwm->ChipName = "Nuvoton NCT7802";
	Hwm->SmbAddr = SmbAddr;

	Hwm->MaxCh[HWM_TYPE_VOLT] = NCT7802_MAXCH_VOLT;
	Hwm->MaxCh[HWM_TYPE_CURR] = NCT7802_MAXCH_CURR;
	Hwm->MaxCh[HWM_TYPE_TEMP] = NCT7802_MAXCH_TEMP;
	Hwm->MaxCh[HWM_TYPE_FAN]  = NCT7802_MAXCH_FAN;

	Hwm->Resolution[HWM_TYPE_VOLT] = NCT7802_RESOLUTION_VOLT;
	Hwm->Resolution[HWM_TYPE_CURR] = NCT7802_RESOLUTION_CURR;
	Hwm->Resolution[HWM_TYPE_TEMP] = NCT7802_RESOLUTION_TEMP;
	Hwm->Resolution[HWM_TYPE_FAN]  = NCT7802_RESOLUTION_FAN;

	Hwm->GetChannelName	= &nct7802_GetChannelName;
	Hwm->GetStatus 	= &nct7802_GetStatus;
	Hwm->GetReading	= &nct7802_GetReading;
	Hwm->ReadReg 	= &nct7802_ReadReg;
	Hwm->WriteReg 	= &nct7802_WriteReg;

	return EFI_SUCCESS;
}

//=============================================================================
EFI_STATUS hwm_nct7802_init(
	IN	SMB_BUS 		*SmbusHc,
	OUT	HWM_HANDLE_ST 	**Hwm
)
{
	SMB_DEVICE *sdev = NULL;
	UINT8	cnt;
	UINT8 	rbyte;
	if (SmbusHc == NULL)
		return EFI_INVALID_PARAMETER;

	*Hwm = NULL;
	memset(&nct7802_hdl, 0, sizeof(HWM_HANDLE_ST));

	sdev = SmbusHc->dev_head;
	for (cnt = 0; cnt < SmbusHc->dev_num; cnt++) {
		// check vendor id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, NCT7802_REG_VENDOR_ID);
		if(rbyte != NCT7802_VENDOR_ID)
			goto next;

		// check Chip id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, NCT7802_REG_CHIP_ID);
		if(rbyte != NCT7802_CHIP_ID)
			goto next;

		// check Device id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, NCT7802_REG_DEV_ID);
		if((rbyte < NCT7802_DEV_ID) || (rbyte > (NCT7802_DEV_ID + 0x0F)))
			goto next;

		// update bank
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, NCT7802_REG_BANK_SEL);
		if(rbyte != 0){
			SmbusHc->write_byte(SmbusHc, sdev->addr, NCT7802_REG_BANK_SEL, 1);
			nct7802_bank = 1;
		}
		else
			nct7802_bank = 0;

		// nct7802 found
		nct7802_init(sdev->addr, &nct7802_hdl);
		*Hwm = &nct7802_hdl;
		nct7802_Smbus = SmbusHc;
		break;

next:		
		if (sdev->next != NULL)
			sdev = sdev->next; 
		else
			break;
	}

	if (*Hwm == NULL)
		return EFI_NOT_FOUND;
	else
		return EFI_SUCCESS;
}

#include <stdio.h>

#include "x86hw.h"
#include "util.h"
#include "tsmb.h"

#include "w83782.h"
#include "hwm.h"

static uint8_t 	temp2_addr = 0, temp3_addr = 0;
uint8_t 		real_bank = 0;
HWM_HANDLE_ST	w83782_hdl;
SMB_BUS 		*HwmSmbus = NULL;

typedef struct _VOLT_REG{
	UINT8	Bank;
	UINT8	Reading;
	UINT8	LimitL;
	UINT8	LimitH;
} VOLT_REG;

typedef struct _TEMP_REG{
	UINT8	Bank;
	UINT8	ReadingL;
	UINT8	ReadingH;
} TEMP_REG;

typedef struct _FAN_REG{
	UINT8	Bank;
	UINT8	Reading;
	UINT8	DivL;
	UINT8	DivH;
} FAN_REG;

VOLT_REG w83782_VoltReg[W83782_MAXCH_VOLT] = {
	{0, W83782_REG_VCOREA,	W83782_REG_VCOREA_LMTL, 	W83782_REG_VCOREA_LMTH},
	{0, W83782_REG_VINR0,	W83782_REG_VINR0_LMTL,		W83782_REG_VINR0_LMTH},
	{0, W83782_REG_VIN12V,	W83782_REG_VIN12V_LMTL,		W83782_REG_VIN12V_LMTH},
	{0, W83782_REG_VIN5V,	W83782_REG_VIN5V_LMTL,		W83782_REG_VIN5V_LMTH},
	{0, W83782_REG_VIN3P3V, W83782_REG_VIN3P3V_LMTL,	W83782_REG_VIN3P3V_LMTH},
	{4, W83782_REG_5VSB,	W83782_REG_5VSB_LMTL,		W83782_REG_5VSB_LMTH},
	{4, W83782_REG_VBAT,	W83782_REG_VBAT_LMTL,		W83782_REG_VBAT_LMTH},
	{0, W83782_REG_VINN12V, W83782_REG_VINN12V_LMTL,	W83782_REG_VINN12V_LMTH},
	{0, W83782_REG_VINN5V,	W83782_REG_VINN5V_LMTL,		W83782_REG_VINN5V_LMTH},
};

FAN_REG w83782_FanReg[W83782_MAXCH_FAN] = {
	{0, W83782_REG_FAN1,	W83782_REG_FAN12_DIVL, 		W83782_REG_FAN123_DIVH},
	{0, W83782_REG_FAN2,	W83782_REG_FAN12_DIVL,		W83782_REG_FAN123_DIVH},
	{0, W83782_REG_FAN3,	W83782_REG_FAN3_DIVL,		W83782_REG_FAN123_DIVH},
};

TEMP_REG w83782_TempReg[W83782_MAXCH_TEMP] = {
	{0, 0, 							W83782_REG_TEMP},
	{1, W83782_REG_TEMP2_L,			W83782_REG_TEMP2_H},
	{2, W83782_REG_TEMP2_L,			W83782_REG_TEMP3_H},
};

CHAR8 *w83782_VoltName[W83782_MAXCH_VOLT] = {
	"VCORE",
	"VINR0",
	"+12VIN",
	"+5VIN",
	"+3.3VIN",
	"5VSB",
	"VBAT",
	"-12VIN",
	"-5VIN",
};

CHAR8 *w83782_TempName[W83782_MAXCH_TEMP] = {
	"TEMP1",
	"TEMP2",
	"TEMP3",
};

CHAR8 *w83782_FanName[W83782_MAXCH_FAN] = {
	"FAN1",
	"FAN2",
	"FAN3",
};

//=============================================================================
EFI_STATUS w83782_ReadReg(
	IN	UINT8	Bank,
	IN	UINT8	Register,
	OUT	UINT8	*Value
)
{
	UINT8 Addr;

	if (HwmSmbus == NULL) 
		return EFI_NOT_READY;

	if (Value == NULL) 
		return EFI_INVALID_PARAMETER;

	if (Bank > 5) {
		return EFI_INVALID_PARAMETER;
	}
	else if (Bank == 1) {
		Addr = temp2_addr;
	}
	else if (Bank == 2) {
		Addr = temp3_addr;
	}
	else
		Addr = w83782_hdl.SmbAddr;

	if (Bank != real_bank) {
		HwmSmbus->write_byte(HwmSmbus, w83782_hdl.SmbAddr, W83782_REG_BANK_SEL, Bank);
		real_bank = Bank;
	}
	*Value = HwmSmbus->read_byte(HwmSmbus, Addr, Register);
	return EFI_SUCCESS;
};

//=============================================================================
EFI_STATUS w83782_WriteReg(
	IN	UINT8	Bank,
	IN	UINT8	Register,
	IN	UINT8	Value
)
{
	UINT8 Addr;

	if (HwmSmbus == NULL) 
		return EFI_NOT_READY;

	if (Bank > 5) {
		return EFI_INVALID_PARAMETER;
	}
	else if (Bank == 1) {
		Addr = temp2_addr;
	}
	else if (Bank == 2) {
		Addr = temp3_addr;
	}
	else
		Addr = w83782_hdl.SmbAddr;

	if (Bank != real_bank) {
		HwmSmbus->write_byte(HwmSmbus, w83782_hdl.SmbAddr, W83782_REG_BANK_SEL, Bank);
		real_bank = Bank;
	}
	HwmSmbus->write_byte(HwmSmbus, Addr, Register, Value);

	return EFI_SUCCESS;
};

//=============================================================================
EFI_STATUS w83782_ReadVoltage(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	UINT8		value;
	VOLT_REG 	*reg;
	EFI_STATUS	Status;

	if (Index >= W83782_MAXCH_VOLT) 
		return EFI_UNSUPPORTED;

	reg = &w83782_VoltReg[Index]; 

	Status = w83782_ReadReg(reg->Bank, reg->Reading, &value);	// 16mV/Count
	if (Status != EFI_SUCCESS) 
		return Status;

	*Reading = (UINT16)value; 
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS w83782_ReadTemp(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	UINT8		LByte = 0, HByte = 0;
	TEMP_REG 	*reg;
	EFI_STATUS	Status;

	if (Index >= W83782_MAXCH_TEMP) 
		return EFI_UNSUPPORTED;

	reg = &w83782_TempReg[Index]; 

	// Get Temperature.Bit0
	if (reg->ReadingL == 0) {
		LByte = 0;
	}
	else {
		Status = w83782_ReadReg(reg->Bank, reg->ReadingL, &LByte);
		if (Status != EFI_SUCCESS) 
			return Status;
		if (LByte & 0x80)
			LByte = 1;
		else
			LByte = 0;
	}
	// Get Temperature.Bit8~1
	Status = w83782_ReadReg(reg->Bank, reg->ReadingH, &HByte);
	if (Status != EFI_SUCCESS) 
		return Status;

	*Reading = (((UINT16) HByte) << 1) | LByte;	// 0.5C/Count

	if (*Reading & 0x100) {//negative
		*Reading = *Reading | 0xFE00;
	}
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS w83782_ReadFan(
	IN	UINTN	Index,
	OUT	UINT16	*Reading
)
{
	EFI_STATUS	Status;
	FAN_REG 	*reg;
	UINT8		Value, DivL, DivH;
	UINT32		Speed;

	if (Index >= W83782_MAXCH_FAN) 
		return EFI_UNSUPPORTED;

	reg = &w83782_FanReg[Index]; 

	// Get Divisor
	Status = w83782_ReadReg(reg->Bank, reg->DivL, &DivL);
	if (Status != EFI_SUCCESS) 
		return Status;

	Status = w83782_ReadReg(reg->Bank, reg->DivH, &DivH);
	if (Status != EFI_SUCCESS) 
		return Status;

	switch (Index) {
	case 0:
		DivL = ((DivL & 0x30) >> 4) | ((DivH & 0x20) >> 3);
		break;
	case 1:
		DivL = ((DivL & 0xC0) >> 6) | ((DivH & 0x40) >> 4);
		break;
	case 2:
		DivL = ((DivL & 0xC0) >> 6) | ((DivH & 0x80) >> 5);
		break;
	}
	DivL = 0x01 << DivL;	// Real Divsor = 2^DivL

	// Get Fan Count
	Status = w83782_ReadReg(reg->Bank, reg->Reading, &Value);
	if (Status != EFI_SUCCESS) 
		return Status;
	if (Value == 0xFF) {
		Speed = 0xFFFF;
	}
	else if (Value == 0) {
		Speed = 0;
	} 
	else {
		Speed = 1350000 / ( (UINT32)Value * DivL );
		if (Speed > 0xFFFF)
			Speed = 0xFFFF;
	}

	*Reading = (UINT16)Speed; 
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS w83782_GetReading(
	IN	UINTN	Type,
	IN	UINTN	Index,
	IN	UINT16	*Reading
)
{
	EFI_STATUS	Status;

	switch (Type) {
	case HWM_TYPE_VOLT:
		Status = w83782_ReadVoltage(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	case HWM_TYPE_TEMP:
		Status = w83782_ReadTemp(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	case HWM_TYPE_FAN:
		Status = w83782_ReadFan(Index, Reading);
		if (Status != EFI_SUCCESS) 
			return Status;
		break;
	default:
		return EFI_UNSUPPORTED;
	}

	return EFI_SUCCESS;
}

//=============================================================================
EFI_STATUS w83782_GetChannelName(
	IN	UINTN	Type,
	IN	UINTN	Index,
	IN	CHAR8	**Name				// max length 8
)
{
	switch (Type) {
	case HWM_TYPE_VOLT:
		*Name = w83782_VoltName[Index];
		break;
	case HWM_TYPE_TEMP:
		*Name = w83782_TempName[Index];
		break;
	case HWM_TYPE_FAN:
		*Name = w83782_FanName[Index];
		break;
	default:
		*Name = NULL;
		return EFI_UNSUPPORTED;
	}
	return EFI_SUCCESS; 
};

//=============================================================================
EFI_STATUS w83782_GetStatus(
	IN		UINTN			Type,
	IN		UINTN			*Channels,
	IN OUT	HWM_STATUS_ST	**Status
)
{
	return EFI_UNSUPPORTED;
};

//=============================================================================
EFI_STATUS w83782_init(
	IN	UINT8		 	SmbAddr,
	IN	HWM_HANDLE_ST 	*Hwm
)
{
	if (Hwm == NULL) 
		return EFI_INVALID_PARAMETER;

	Hwm->ChipName = "Winbond W83782";
	Hwm->SmbAddr = SmbAddr;

	Hwm->MaxCh[HWM_TYPE_VOLT] = W83782_MAXCH_VOLT;
	Hwm->MaxCh[HWM_TYPE_CURR] = W83782_MAXCH_CURR;
	Hwm->MaxCh[HWM_TYPE_TEMP] = W83782_MAXCH_TEMP;
	Hwm->MaxCh[HWM_TYPE_FAN]  = W83782_MAXCH_FAN;

	Hwm->Resolution[HWM_TYPE_VOLT] = W83782_RESOLUTION_VOLT;
	Hwm->Resolution[HWM_TYPE_CURR] = W83782_RESOLUTION_CURR;
	Hwm->Resolution[HWM_TYPE_TEMP] = W83782_RESOLUTION_TEMP;
	Hwm->Resolution[HWM_TYPE_FAN]  = W83782_RESOLUTION_FAN;

	Hwm->GetChannelName	= &w83782_GetChannelName;
	Hwm->GetStatus 	= &w83782_GetStatus;
	Hwm->GetReading	= &w83782_GetReading;
	Hwm->ReadReg 	= &w83782_ReadReg;
	Hwm->WriteReg 	= &w83782_WriteReg;

	return EFI_SUCCESS;
}

//=============================================================================
EFI_STATUS hwm_w83782_init(
	IN	SMB_BUS 		*SmbusHc,
	OUT	HWM_HANDLE_ST 	**Hwm
)
{
	SMB_DEVICE *sdev = NULL;
	UINT8	Data;
	UINT8	cnt;
	UINT8 	rbyte;
	if (SmbusHc == NULL)
		return EFI_INVALID_PARAMETER;

	*Hwm = NULL;
	memset(&w83782_hdl, 0, sizeof(HWM_HANDLE_ST));

	sdev = SmbusHc->dev_head;
	for (cnt = 0; cnt < SmbusHc->dev_num; cnt++) {
		// check device id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_DEV_ID);
		if((rbyte & 0xFE) != W83782_DEV_ID)
			goto next;

		// check vendor id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_VID);
		if (rbyte != (uint8_t)(W83782_VENDOR_ID >> 8)) 
		{
			if (rbyte != (W83782_VENDOR_ID & 0xFF))
				goto next;
			//if rbyte = vendor id low byte, check W83782_REG_BANK_SEL.bit7
			rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_BANK_SEL);
			if (rbyte & 0x80)
				goto next;
			// set W83782_REG_BANK_SEL.bit7 to 1 and check vendor id high byte again
			SmbusHc->write_byte(SmbusHc, sdev->addr, W83782_REG_BANK_SEL, 0x80);
			rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_VID);
			if (rbyte != (uint8_t)(W83782_VENDOR_ID >> 8)) 
				goto next;
		}
		else{
			// set W83782_REG_BANK_SEL.bit7 to 0 and check vendor id low byte
			SmbusHc->write_byte(SmbusHc, sdev->addr, W83782_REG_BANK_SEL, 0);
			rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_VID);
			if (rbyte != (W83782_VENDOR_ID & 0xFF))
				goto next;
		}
		// check chip id
		rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_CHIP_ID);
		if (rbyte == W83782_CIP_ID){
			rbyte = SmbusHc->read_byte(SmbusHc, sdev->addr, W83782_REG_TEMP_SMB_ADDR);
			temp2_addr = W83782_TEMP_ADDR | ((rbyte & 0x07) << 1);
			temp3_addr = W83782_TEMP_ADDR | ((rbyte & 0x70) >> 3);

			w83782_init(sdev->addr, &w83782_hdl);
			*Hwm = &w83782_hdl;
			HwmSmbus = SmbusHc;

			// Enable VBAT Sensing
			if(w83782_ReadReg(0, 0x5D, &Data) != EFI_SUCCESS)
				fprintf(stderr, "WARN(w83782): Failed to nable VBAT Sensing.\n"); 

			if(w83782_WriteReg(0, 0x5D, Data | 0x01) != EFI_SUCCESS)
				fprintf(stderr, "WARN(w83782): Failed to nable VBAT Sensing.\n"); 

			break;
		}
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

#ifndef _HWM_H_
#define _HWM_H_

#include "x86hw.h"

#define HMW_RESOLUTION_BASE  1000

enum HWM_CHIP_LIST{
	HWM_CHIP_W83782,
	HWM_CHIP_NCT7802,
	HWM_CHIP_MAX
};

enum HWM_TYPE{
	HWM_TYPE_VOLT,
	HWM_TYPE_CURR,
	HWM_TYPE_TEMP,
	HWM_TYPE_FAN,
	HWM_TYPE_MAX
};

typedef union _HWM_STATUS_ST{
	struct{
		UINT16	Error			: 1;
		UINT16	Warning			: 1;
		UINT16	ErrClear		: 1;
		UINT16	WarnClear		: 1;
		UINT16	Alert			: 1;
		UINT16	SMI				: 1;
		UINT16	Override		: 1;		// for fan only
		UINT16	Rvsd			: 9;		// for furture
	} bits;
	UINT16		Status;
} HWM_STATUS_ST;

// Get All of the type status in one function
typedef EFI_STATUS (EFIAPI *HWM_GET_NAME)(
	IN    UINTN		Type,
	IN    UINTN		Index,
	OUT   CHAR16	**Name				// max length 8
);

// Get All of the type status in one function
typedef EFI_STATUS (EFIAPI *HWM_GET_STATUS)(
	IN	UINTN			Type,
	IN	UINTN			*Channels,
	OUT	HWM_STATUS_ST	**Status
);

// Get All of the type reading in one function
typedef EFI_STATUS (EFIAPI *HWM_GET_READING)(
	IN	UINTN		Type,
	IN	UINTN		Index,
	OUT	UINT16		*Reading
);

// read hwm chip register directly 
typedef EFI_STATUS (EFIAPI *HWM_READ_REGISTER)(
	IN	UINT8		Bank,
	IN	UINT8 		Register,
	OUT	UINT8		*Value
);

// write hwm chip register directly 
typedef EFI_STATUS (EFIAPI *HWM_WRITE_REGISTER)(
	IN	UINT8		Bank,
	IN	UINT8		Register,
	IN	UINT8		Value
);

typedef struct _HWM_HANDLE_ST{
	CHAR8				*ChipName;
	UINT8				SmbAddr;
	UINT16				MaxCh[HWM_TYPE_MAX];
	UINT16				Resolution[HWM_TYPE_MAX];				// real resolution = Resolution[Type] / HMW_RESOLUTION_BASE
	HWM_GET_NAME		GetChannelName;
	HWM_GET_STATUS		GetStatus;
	HWM_GET_READING		GetReading;
	HWM_READ_REGISTER	ReadReg;
	HWM_WRITE_REGISTER	WriteReg;
} HWM_HANDLE_ST;

// init Hardware chip
typedef EFI_STATUS (EFIAPI *HWM_CHIP_INIT)(
	IN	SMB_BUS 		*SmbusHc,
	OUT	HWM_HANDLE_ST 	**Hwm
);

// get hardware chip handle by ChipNameID
EFI_STATUS HardwareMonitorInit(
	IN	SMB_BUS 		*SmbusHc,
	IN	UINTN	 		ChipNameID,
	OUT	HWM_HANDLE_ST 	**Hwm
);
#endif // _HWM_H_

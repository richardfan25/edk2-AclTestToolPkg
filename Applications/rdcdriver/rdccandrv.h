#ifndef _RDCCANDRV_H__
#define _RDCCANDRV_H__



#define EFI_CAN_PROTOCOL_GUID { 0xca250024, 0x2792, 0x7818, { 0x93, 0x09, 0x93, 0x08, 0x91, 0x74, 0x51, 0x68 }}	//Tentative
#define CAN_TX	(1 << 1)
#define CAN_RX	(1 << 2)

 

typedef struct _EFI_CAN_TRANSCEIVER_PROTOCOL EFI_CAN_TRANSCEIVER_PROTOCOL;

typedef struct _EFI_CAN_REQUEST_PACKET
{
	UINT8 Flag;
	UINT8 Format;
	UINT32 ID;
	UINT8 Length;
	UINT8 *Data;
}EFI_CAN_REQUEST_PACKET;

/**
     
  The CANExecute() function provides a standard way to execute an
  operation as defined in the CAN Bus Specification. 
  
  @param This     A pointer to the EFI_CAN_TRANSCEIVER_PROTOCOL instance.

  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	Format/ ID/ Length 
  				is outside the range of valid values.

  #####EFI_CAN_REQUEST_PACKET : #####
  @param Format	(0:Standard 1:Extended).

  @param ID		When Format is Standard, ID range(0~268435455), 
  				When Format is Extended, ID range(0~2047). 
                  
  @param Length    Size of Buffer.

  @param Buffer   Data.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_CAN_EXECUTE_OPERATION)(
	IN CONST EFI_CAN_TRANSCEIVER_PROTOCOL	*This,
	IN OUT EFI_CAN_REQUEST_PACKET *RequestPacket
);

/**
     
  The SetBusBaudrate() function provides a standard way to set CAN bus baudrate. 
  
  @param This     A pointer to the EFI_CAN_TRANSCEIVER_PROTOCOL instance.

  @param Baudrate  CAN bus clock baudrate is one of follow  
  				0. 1000 Kbps
				1. 833.333 Kbps
				2. 500 Kbps
				3. 250 Kbps
				4. 125 Kbps
				5. 100 Kbps
				6. 62.5 Kbps
				7. 50 Kbps
				8. 25 Kbps
				9. 10 Kbps
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	When Format/ ID/ Length is outside the range of valid values.

**/
typedef EFI_STATUS (EFIAPI *EFI_CAN_PROTOCOL_SET_BAUDRATE)(
  IN CONST EFI_CAN_TRANSCEIVER_PROTOCOL *This,
  IN UINT8 *Baudrate
);

struct _EFI_CAN_TRANSCEIVER_PROTOCOL
{
	EFI_CAN_EXECUTE_OPERATION CANExecute;
	EFI_CAN_PROTOCOL_SET_BAUDRATE SetBusBaudrate;
};

typedef struct _CANMASTER_PRIVATE
{
	EFI_CAN_TRANSCEIVER_PROTOCOL CANController;
	EFI_DEVICE_PATH_PROTOCOL *DevPath;
	EFI_HANDLE Handle;	///< Handle
	UINT16 CANBusIO;
	UINT8 CANBusId;
	UINT16 Baudrate;
	UINT8 Flag;
	UINT8 Format;
	UINT32 ID;
	UINT8 Length;
	UINT8 *Data;
}CANMASTER_PRIVATE;

/// Private CAN Master Data Block Structure
typedef struct _CAN_MASTER_PRIVATE
{
	CANMASTER_PRIVATE CANMaster[2];		///< CAN0~1 Data Block
}CAN_MASTER_PRIVATE;

typedef struct
{
	DEVICE_LOGICAL_UNIT_DEVICE_PATH RDCCanLDN;
	//MEMMAP_DEVICE_PATH CanMemMapDevPath;
	CONTROLLER_DEVICE_PATH CanBus;
	EFI_DEVICE_PATH_PROTOCOL End;
}CAN_DEVICE_PATH; 



enum{
	B1000,
	B833P33,
	B500,
	B250,
	B125,
	B100,
	B62P5,
	B50,
	B25,
	B10,
	BaudRateMax
};

enum{
	Standard,
	Extended,
	FormatMax
};

enum{
	Normal,
	Increase,
	ModeMax
};



EFI_STATUS EFIAPI RDCCanMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable);



#endif


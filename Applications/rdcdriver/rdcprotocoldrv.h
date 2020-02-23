#ifndef _RDCPROTOCOL_H__
#define _RDCPROTOCOL_H__



#define EFI_RDC_PROTOCOL_GUID 	{ 0xdc250024, 0x2792, 0x7818, { 0x93, 0x09, 0x93, 0x08, 0x91, 0x74, 0x51, 0x68 }}//Tentative



typedef struct _EFI_RDC_COMMUNICATE_PROTOCOL EFI_RDC_COMMUNICATE_PROTOCOL;

/**
  The RdcECExecute() function provides a standard way to execute an
  operation as in RDC PMC Protocol. 
  
  @param This     A pointer to the EFI_RDC_COMMUNICATE_PROTOCOL instance.
  
  @param Arg1     Command.
  
  @param Arg2     FW:Control. SW:Index / Status.
  
  @param Arg3     FW:ID. SW:Offset.
  
  @param Arg4     Length.
  
  @param Arg5     Data.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_RDC_COMMUNICATE_EXECUTE)(
	IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
	IN UINT8 Arg1,
	IN UINT8 *Arg2,
	IN UINT8 Arg3,
	IN UINT8 Arg4,
	IN UINT8 *Arg5
);
#if 0
/**
  The RdcECRead() function provides a standard way to execute an
  operation as in RDC PMC Protocol. 
  
  @param This     A pointer to the EFI_RDC_COMMUNICATE_PROTOCOL instance.
  
  @param Arg1     Command.
  
  @param Arg2     FW:Control. SW:Index.
  
  @param Arg3     FW:ID. SW:Offset.
  
  @param Arg4     Length.
  
  @param Arg5     Data.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_RDC_COMMUNICATE_READ)(
	IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
	IN UINT8 Arg1,
	IN UINT8 Arg2,
	IN UINT8 Arg3,
	IN UINT8 Arg4,
	IN UINT8 *Arg5
);

/**
  The RdcECWrite() function provides a standard way to execute an
  operation as in RDC PMC Protocol. 
  
  @param This     A pointer to the EFI_RDC_COMMUNICATE_PROTOCOL instance.
  
  @param Arg1     Command.
  
  @param Arg2     FW:Control. SW:Index.
  
  @param Arg3     FW:ID. SW:Offset.
  
  @param Arg4     Length.
  
  @param Arg5     Data.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_RDC_COMMUNICATE_WRITE)(
	IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
	IN UINT8 Arg1,
	IN UINT8 Arg2,
	IN UINT8 Arg3,
	IN UINT8 Arg4,
	IN UINT8 *Arg5
);

/**
  The RdcECReadStatus() function provides a standard way to execute an
  operation as in RDC PMC Protocol. 
  
  @param This     A pointer to the EFI_RDC_COMMUNICATE_PROTOCOL instance.
  
  @param Arg1     Command.
  
  @param Arg2     FW:Control. SW:Index.
  
  @param Arg3     FW:Device ID. SW:Offset.
  
  @param Arg4     Length.
  
  @param Arg5     Data.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_RDC_COMMUNICATE_READ_STATUS)(
	IN CONST EFI_RDC_COMMUNICATE_PROTOCOL *This,
	IN UINT8 Arg1,
	IN UINT8 Arg2,
	IN UINT8 Arg3,
	IN UINT8 Arg4,
	IN UINT8 *Arg5
);
#endif
struct _EFI_RDC_COMMUNICATE_PROTOCOL
{
	EFI_RDC_COMMUNICATE_EXECUTE RdcECExecute;
	//EFI_RDC_COMMUNICATE_READ RdcECRead;
	//EFI_RDC_COMMUNICATE_WRITE RdcECWrite;
	//EFI_RDC_COMMUNICATE_READ_STATUS RdcECReadStatus;
};

typedef struct _RDCCOMMUNICATE_PRIVATE
{
	EFI_RDC_COMMUNICATE_PROTOCOL RDCController;
	EFI_DEVICE_PATH_PROTOCOL *DevPath;
	EFI_HANDLE Handle;	///< Handle
	UINT16 CmdPort;
	UINT16 DataPort;
	UINT8 Arg1;	//Command
	UINT8 *Arg2;	//FW:Control. SW:Index / Status.
	UINT8 Arg3;	//FW:Device ID. SW:Offset.
	UINT8 Arg4;	//Length
	UINT8 *Arg5;	//Data
}RDCCOMMUNICATE_PRIVATE;

/// Private RDC Communicate Data Block Structure
typedef struct _RDC_COMMUNICATE_PRIVATE
{
	RDCCOMMUNICATE_PRIVATE RDCCommunicate;
}RDC_COMMUNICATE_PRIVATE;

typedef struct
{
	MEMMAP_DEVICE_PATH RDCCommunicateMemMapDevPath;
	EFI_DEVICE_PATH_PROTOCOL End;
}RDC_COMMUNICATE_DEVICE_PATH; 



enum{
	rdc_write,
	rdc_read,
};



EFI_STATUS EFIAPI RDCProtocolDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable);



#endif


#ifndef _RDCGPIODRV_H__
#define _RDCGPIODRV_H__



#define EFI_GPIO_PROTOCOL_GUID 	{ 0x7F250024, 0x2792, 0x7818, { 0x93, 0x09, 0x93, 0x08, 0x91, 0x74, 0x51, 0x68 }}//Tentative

#define GPIO_WRITE (1 << 1)
#define GPIO_READ (1 << 2)

#define RDC_GPIO_DATA_OFFSET 0x00
#define RDC_GPIO_DIRECTION_OFFSET 0x04
#define RDC_GPIO_INTERRUPT_STATUS_OFFSET 0x08
#define RDC_GPIO_INTERRUPT_CONTROL_OFFSET 0x0C



typedef struct _EFI_GPIO_CONTROL_PROTOCOL EFI_GPIO_CONTROL_PROTOCOL;



/**
  The GPIOData() function provides a standard way to execute an
  operation as in RDC GPIO. 
  
  @param This     A pointer to the EFI_GPIO_CONTROL_PROTOCOL instance.
  
  @param Port     GPIO 0 : Port0 /1 : Port1.
  
  @param Data     Data.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_GPIO_CONTROL_DATA)(
	IN CONST EFI_GPIO_CONTROL_PROTOCOL *This,
	IN UINT8 Flag,
	IN UINT8 Port,
	IN OUT UINT8 *Data
);

/**
  The GPIODirection() function provides a standard way to execute an
  operation as in RDC GPIO. 
  
  @param This     A pointer to the EFI_GPIO_CONTROL_PROTOCOL instance.
  
  @param Port     GPIO 0 : Port0 /1 : Port1.
  
  @param Direction     Every bit, 0 : Input / 1 : Output.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_GPIO_CONTROL_DIRECTION)(
	IN CONST EFI_GPIO_CONTROL_PROTOCOL *This,
	IN UINT8 Flag,
	IN UINT8 Port,
	IN OUT UINT8 *Direction
);

/**
  The GPIOInterruptStatus() function provides a standard way to execute an
  operation as in RDC GPIO. 
  
  @param This     A pointer to the EFI_GPIO_CONTROL_PROTOCOL instance.
  
  @param Port     GPIO 0 : Port0 /1 : Port1.
  
  @param Status     R/W1C.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_GPIO_CONTROL_INTERRUPT_STATUS)(
	IN CONST EFI_GPIO_CONTROL_PROTOCOL *This,
	IN UINT8 Flag,
	IN UINT8 Port,
	IN OUT UINT8 *Status
);

/**
  The GPIOInterruptControl() function provides a standard way to execute an
  operation as in RDC GPIO. 
  
  @param This     A pointer to the EFI_GPIO_CONTROL_PROTOCOL instance.
  
  @param Port     GPIO 0 : Port0 /1 : Port1.
  
  @param Control     R/W.
  
  @retval EFI_SUCCESS           The last data that was send out.

  @retval EFI_INVALID_PARAMETER	value is outside the range.
  
**/
typedef EFI_STATUS (EFIAPI *EFI_GPIO_CONTROL_INTERRUPT_CONTROL)(
	IN CONST EFI_GPIO_CONTROL_PROTOCOL *This,
	IN UINT8 Flag,
	IN UINT8 Port,
	IN OUT UINT32 *Control
);

struct _EFI_GPIO_CONTROL_PROTOCOL
{
	EFI_GPIO_CONTROL_DATA GPIOData;
	EFI_GPIO_CONTROL_DIRECTION GPIODirection;
	EFI_GPIO_CONTROL_INTERRUPT_STATUS GPIOInterruptStatus;
	EFI_GPIO_CONTROL_INTERRUPT_CONTROL GPIOInterruptControl;
};

typedef struct _GPIOCONTROL_PRIVATE
{
	EFI_GPIO_CONTROL_PROTOCOL GPIOController;
	EFI_DEVICE_PATH_PROTOCOL *DevPath;
	EFI_HANDLE Handle;	///< Handle
	UINT16 GPIOIO;
	UINT8 Flag;
	UINT8 Port;
	UINT8 *Direction;
	UINT8 *Data;
	UINT8 *Status;
	UINT32 *Control;
}GPIOCONTROL_PRIVATE;

/// Private GPIO Control Data Block Structure
typedef struct _GPIO_CONTROL_PRIVATE
{
	GPIOCONTROL_PRIVATE GPIOControl;
}GPIO_CONTROL_PRIVATE;

typedef struct
{
	DEVICE_LOGICAL_UNIT_DEVICE_PATH RDCGpioLDN;
	//MEMMAP_DEVICE_PATH GpioMemMapDevPath;
	EFI_DEVICE_PATH_PROTOCOL End;
}GPIO_DEVICE_PATH; 



EFI_STATUS EFIAPI RDCGPIOMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable);



#endif


#ifndef _RDCDRIVER_H__
#define _RDCDRIVER_H__

#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>

#include "aec.h"
#include "rdcec.h"
#include "a9610/i2c.h"
#include "a9610/smb.h"
#include "a9610/can.h"
#include "rdcprotocoldrv.h"
#include "rdccandrv.h"
#include "rdci2cdrv.h"
#include "rdcsmbusdrv.h"
#include "rdcsi2cdrv.h"
#include "rdcgpiodrv.h"



extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;



#endif


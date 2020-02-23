#ifndef _T_DIO_H_
#define _T_DIO_H_

#include "x86hw.h"

void DIORestoreRegister(void);
void DIOBackupRegister(void);
void DIOBoardRead(void);
void DIOBoardWrite(void);
void DIOBoardAuto(void);
uint8_t DIOBoardSearch(SMB_BUS *smb);

#endif //_T_DIO_H_


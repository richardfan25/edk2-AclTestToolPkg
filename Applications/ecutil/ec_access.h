#ifndef _EC_ACCESS_H_
#define _EC_ACCESS_H_

#include <stdint.h>

#define dIOSTA_IBF_BIT	0x2				// cmd/status port bit1 mask
#define dIOSTA_OBF_BIT	0x1				// cmd/status port bit0 mask

#define dDLY_Chk_IOSTA	20000			// 20msec(TCK_ChkSTA); uint16 delay time that check io port status(IBF/OBF)
#define dTCK_Chk_IOSTA	1 				// 1us(1us) / 1us; tick time that check io port status(IBF/OBF)


/*==============================================================*/
// EXTERN fucntion, variable
/*==============================================================*/
extern int ec_set_port(UINT16 cmd,UINT16 data);
extern void ec_close_port(void);
extern int ec_chk_IBF(UINT16 port);
extern int ec_chk_OBF(UINT16 port);
extern int ec_wr_cmd(UINT8 val);
extern int ec_wr_data(UINT8 val);
extern int ec_rd_cmd(UINT8 *val);
extern int ec_rd_data(UINT8 *val);

#endif	// _EC_ACCESS_H_
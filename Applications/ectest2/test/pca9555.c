/*==========================================================*/
/* The Device Function For NXP PCA9555 File					*/
/* Date: 2017.05.08											*/
/*==========================================================*/
#include <stdio.h>

#include "mbxapp.h"
#include "pca9555.h"

UINT8 pca9555_addr[8] = {0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E};
/*===========================================================
 * Name  : dio_get_reg_type
 * Pupose: Get Register of PCA9555 data
 * Input : addr - SMBus address
 *         reg  - PCA9555 Register
 *         *data  - register value
 * Output: bit0 = 1 - Support Read
 *         bit1 = 1 - Support Write
 *===========================================================*/
int dio_get_reg_type(UINT8 reg)
{
	switch(reg)
	{
		case DIO_REG_IN_PORT0:
		case DIO_REG_IN_PORT1:
			return 0x01;		// Read Only
		case DIO_REG_OUT_PORT0:
		case DIO_REG_OUT_PORT1:
			return 0x02;		// Write Only
		case DIO_REG_INV_PORT0:
		case DIO_REG_INV_PORT1:
		case DIO_REG_CONFIG0:
		case DIO_REG_CONFIG1:
			return 0x03;		// RW
		default:
			return 0;
	}
}
/*===========================================================
 * Name  : dio_read_reg
 * Pupose: Get Register of PCA9555 data
 * Input : addr - SMBus address
 *         reg  - PCA9555 Register
 *         *data  - register value
 * Output: 1: operation success, Others: smbus error code
 *===========================================================*/
int dio_read_reg(UINT8 did, UINT8 addr, UINT8 reg, UINT8 *data)
{
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;
	UINT8		smb_cmd = reg;
	
	addr = addr & 0xFE;
	
	ret = dio_get_reg_type(reg);
	if(ret == 0)
		return DIO_ERR_GET_UNKNOWN_REG;
	else if ((ret & 0x01) == 0)
		return DIO_ERR_GET_WO_REG;

	ret =	mApp.smb->request(
				MBXSMB_ReadByte,		// master cmd
				did,			// device ID
				addr,					// device slave address
				smb_cmd,				// device command
				wbuf,					// write buf
				rbuf,					// read buf
				0,						// size of write buf
				&rlen					// size of read buf
			);
	
	if((ret & 0x7F) != 0)
	{
		fprintf(stderr, "\n");
		mApp.smb->printErrMsg(ret);
		return ret;
	}
	*data = rbuf[0] & 0xFF;
	return _MBEr_Success;
}
/*===========================================================
 * Name  : dio_write_reg
 * Pupose: Get Register of PCA9555 data
 * Input : addr - SMBus address
 *         reg  - PCA9555 Register
 *         *data  - register value
 * Output: 1: operation success, Others: smbus error code
 *===========================================================*/
int dio_write_reg(UINT8 did, UINT8 addr, UINT8 reg, UINT8 data)
{
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;
	UINT8		smb_cmd = reg;
	
	addr = addr & 0xFE;
	
	ret = dio_get_reg_type(reg);
	if(ret == 0)
		return DIO_ERR_GET_UNKNOWN_REG;
	else if ((ret & 0x02) == 0)
		return DIO_ERR_GET_RO_REG;

	wbuf[0] = data;
	ret =	mApp.smb->request(
				MBXSMB_WriteByte,		// master cmd
				did,			// device ID
				addr,					// device slave address
				smb_cmd,				// device command
				wbuf,					// write buf
				rbuf,					// read buf
				0,						// size of write buf
				&rlen					// size of read buf
			);
	
	if((ret & 0x7F) != 0)
	{
		fprintf(stderr, "\n");
		mApp.smb->printErrMsg(ret);
		return ret;
	}
	return _MBEr_Success;
}


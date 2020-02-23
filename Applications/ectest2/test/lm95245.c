/*==========================================================*/
/* The Device Function For TI LM95245 File					*/
/* Date: 2017.05.08											*/
/*==========================================================*/
#include <stdio.h>

#include "mbxapp.h"
#include "lm95245.h"

UINT8 lm95245_addr[LM_SMB_ADDR_NUM] = { LM_SMB_ADDR0, 
										LM_SMB_ADDR1, 
										LM_SMB_ADDR2, 
										LM_SMB_ADDR3, 
										LM_SMB_ADDR4};
/*===========================================================
 * Name  : tmic_get_reg_type
 * Pupose: Get Register of LM95245 data
 * Input : addr - SMBus address
 *         reg  - LM95245 Register
 *         *data  - register value
 * Output: bit0 = 1 - Support Read
 *         bit1 = 1 - Support Write
 *===========================================================*/
int tmic_get_reg_type(UINT8 reg)
{
	switch(reg)
	{
		case LM_REG_STATUS_1:
		case LM_REG_STATUS_2:
		case LM_REG_TEMP_H:
		case LM_REG_TEMP_L:
		case LM_REG_REMOTE_TEMP_H:
		case LM_REG_REMOTE_TEMP_L:
		case LM_REG_REMOTE_UTEMP_H:
		case LM_REG_REMOTE_UTEMP_L:
		case LM_REG_MFU_ID:
		case LM_REG_CHIP_REV:
			return 0x01;		// Read Only
		case LM_REG_ONE_SHOT:
			return 0x02;		// Write Only
		case LM_REG_CFG_1:
		case LM_REG_CONV_RATE:
		case LM_REG_CFG_2:
		case LM_REG_REMOTE_OFFSET_H:
		case LM_REG_REMOTE_OFFSET_L:
		case LM_REG_REMOTE_OS_LMT:
		case LM_REG_REMOTE_CRIT_LMT:
		case LM_REG_T_CRIT_LMT:
		case LM_REG_COMM_HYSTERESIS:
			return 0x03;		// RW
		default:
			return 0;
	}
}
/*===========================================================
 * Name  : tmic_read_reg
 * Pupose: Get Register of LM95245 data
 * Input : addr - SMBus address
 *         reg  - LM95245 Register
 *         *data  - register value
 * Output: 1: operation success, Others: smbus error code
 *===========================================================*/
int tmic_read_reg(UINT8 did, UINT8 addr, UINT8 reg, UINT8 *data)
{
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;
	UINT8		smb_cmd = reg;
	
	addr = addr & 0xFE;
	
	ret = tmic_get_reg_type(reg);
	if(ret == 0)
		return LM_ERR_GET_UNKNOWN_REG;
	else if ((ret & 0x01) == 0)
		return LM_ERR_GET_WO_REG;

	ret =	mApp.smb->request(
				MBXSMB_ReadByte,		// master cmd
				did,					// device ID
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
 * Name  : tmic_write_reg
 * Pupose: Get Register of LM95245 data
 * Input : addr - SMBus address
 *         reg  - LM95245 Register
 *         *data  - register value
 * Output: 1: operation success, Others: smbus error code
 *===========================================================*/
int tmic_write_reg(UINT8 did, UINT8 addr, UINT8 reg, UINT8 data)
{
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;
	UINT8		smb_cmd = reg;
	
	addr = addr & 0xFE;
	
	ret = tmic_get_reg_type(reg);
	if(ret == 0)
		return LM_ERR_GET_UNKNOWN_REG;
	else if ((ret & 0x02) == 0)
		return LM_ERR_GET_RO_REG;

	wbuf[0] = data;
	ret =	mApp.smb->request(
				MBXSMB_WriteByte,		// master cmd
				did,					// device ID
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
/*===========================================================
 * Name  : tmic_get_chip_id
 * Pupose: check input smbus address is for TI lm95245
 * Input : SMBus address
 * Output: 1: address is for TI lm95245, Others: error or is not for that
 *===========================================================*/
int tmic_get_chip_id(UINT8 did, UINT8 addr, UINT16 *id)
{
	int		ret = 0;
	UINT8	value;

	ret = tmic_read_reg(did, addr, LM_REG_MFU_ID, &value);
	if(ret != _MBEr_Success)
		return ret;
	
	if(value != LM_MFU_ID)					// Manufacturer ID: 0x01
		goto err;

	*id = (UINT16)value << 8;
	
	ret = tmic_read_reg(did, addr, LM_REG_CHIP_REV, &value);
	if(ret != _MBEr_Success)
		return ret;

	if(value < LM_IC_REV)					// IC Init Rev: 0xB3
		goto err;
	
	*id = *id | (UINT16)value;
	return _MBEr_Success;
err:
	return LM_ERR_IC_NOT_FOUND;	
}
/*===========================================================
 * Name  : tmic_list_reg_data
 * Pupose: List LM95245 register data
 * Input : SMBus address
 * Output: 1: address is for TI lm95245, Others: error or is not for that
 *===========================================================*/
int tmic_list_reg_data(UINT8 did, UINT8 addr)
{
	int		ret = 0;
	UINT16	u16data;
	UINT8	u8data;
	
	ret = tmic_get_chip_id(did, addr, &u16data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "\nThermal Chip ID: 0x%04X\n", u16data);
	
	ret = tmic_read_reg(did, addr, LM_REG_TEMP_H, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Local Temp:  %3d C\n", u8data);
	
	ret = tmic_read_reg(did, addr, LM_REG_REMOTE_OFFSET_H, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Remote Temp: %3d C\n", u8data);
	
	
	ret = tmic_read_reg(did, addr, LM_REG_T_CRIT_LMT, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Local T_Crit Limit:  %3d C\n", u8data);

	ret = tmic_read_reg(did, addr, LM_REG_REMOTE_OS_LMT, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Remote OS Limit:     %3d C\n", u8data);
	
	ret = tmic_read_reg(did, addr, LM_REG_REMOTE_CRIT_LMT, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Remote T_Crit Limit: %3d C\n", u8data);
	
	ret = tmic_read_reg(did, addr, LM_REG_COMM_HYSTERESIS, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Common Hysteresis:    %2d C\n", u8data);
	
	ret = tmic_read_reg(did, addr, LM_REG_STATUS_1, &u8data);
	if(ret != _MBEr_Success)
		return ret;
	fprintf(stderr, "Status1:");
	if(u8data != 0)
	{
		fprintf(stderr, "\n");
		if(u8data & LM_STA1_BIT_BUSY)
			fprintf(stderr, "  *Temperature is converting.\n");
		if(u8data & LM_STA1_BIT_ROS)
			fprintf(stderr, "  *Remote OS asserted\n");
		if(u8data & LM_STA1_BIT_DIODE)
			fprintf(stderr, "  *Diode missing\n");
		if(u8data & LM_STA1_BIT_RTCRIT)
			fprintf(stderr, "  *Remote TCRIT asserted\n");
		if(u8data & LM_STA1_BIT_LOC)
			fprintf(stderr, "  *Locat OS and TCRIT asserted\n");
		
	}
	else
		fprintf(stderr, " No Event\n");
	
	return _MBEr_Success;
}


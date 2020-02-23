#include "../global.h"
#include <stdio.h>
#include "mbx.h"
#include "mbx_ec.h"
#include "mbx_smb.h"

/*==============================================================*/
int	mbx_smb_init(void);
int	mbx_smb_request(uint8_t icmd, uint8_t did, uint8_t address, uint8_t smbcmd,
					uint8_t *wdata, uint8_t *rdata, uint8_t wlen, uint16_t *rlen);
int mbx_smb_getFreq(uint8_t did, uint16_t *freq);
int mbx_smb_setFreq(uint8_t did, uint16_t freq);
void mbx_smb_printErrMsg(int errcode);
/*==============================================================*/
stMbxSmb mbx_smb = {
	/* *init		*/	&mbx_smb_init,
	/* *request		*/	&mbx_smb_request,
	/* *getFreq		*/	&mbx_smb_getFreq,
	/* *setFreq		*/	&mbx_smb_setFreq,
	/* *printErrMsg	*/	&mbx_smb_printErrMsg,
};

/*===========================================================
 * Name   : mbx_smb_init
 * Purpose: init mailbox smbus control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_smb_init(void)
{
	if(mbox.init() != _err_noerror) return _err_MBoxAccessFail;

    return mbx_ec.init();
}

/*===========================================================
 * Name   : mbx_smb_request
 * Purpose: SMBUS/I2C with mailbox
 * Input  : icmd- mailbox command 
 *          did- device id 
 *          address- device slave address
 *          smbcmd- if icmd is smbus type, put smbus command here. i2c will ignore this byte
 *          wdata- data pointer write to device.
 *          rdata- data pointer read from device
 *          wlen- write data length
 *          rlen- read data length pointer. SMBUS block read will return length,
 * Output : smbus status
 *===========================================================*/
int mbx_smb_request(uint8_t icmd, uint8_t did, uint8_t address, uint8_t smbcmd,
					uint8_t *wdata, uint8_t *rdata, uint8_t wlen, uint16_t *rlen)
{
    uint8_t	idx;
    uint8_t u8tmp;
	uint16_t u16tmp;
    
    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return SMB_ST_ERR_UNKNOWN;               				//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return SMB_ST_ERR_UNKNOWN;      	//set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, address) != _err_noerror)
		return SMB_ST_ERR_UNKNOWN;  	//set Dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, smbcmd) != _err_noerror)
		return SMB_ST_ERR_UNKNOWN; 	//set Dat01
    //output to smbus data
    switch (icmd & 0x7F)  //command skip PEC
    {
    case MBXSMB_WriteWord:
        mbox.writeBoxBuf(MBX_OFFSET_DATA + 5, wdata[1]);                       		//set Dat5
    case MBXSMB_QuickWrite:
    case MBXSMB_SendByte:
    case MBXSMB_WriteByte:
        mbox.writeBoxBuf(MBX_OFFSET_DATA + 4, wdata[0]);                       		//set Dat4
        break;
    case MBXSMB_QuickRead:
    case MBXSMB_ReceiveByte:
    case MBXSMB_ReadByte:
    case MBXSMB_ReadWord:
    case MBXSMB_ReadBlock:
        break;
    case MBXI2C_WriteReadBuf:
        mbx_ec.clearBufRam();  		//clear buffer ram
        if (*rlen > 255)   			//buffer size mxx is 256 bytes
        {
            u8tmp = 0;
        }
		else
		{
			u8tmp = (uint8_t) *rlen;
		}
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 2, u8tmp) != _err_noerror)
			return SMB_ST_ERR_UNKNOWN;	//set Dat02, read count
        if (wlen > MBXSMB_BUF_LEN)   //dat04~dat2C size
        {
            u8tmp = MBXSMB_BUF_LEN;
        }
        else
		{
			u8tmp = wlen;
		}
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 3, u8tmp) != _err_noerror)
			return SMB_ST_ERR_UNKNOWN;	//set Dat03, write count
        for (idx = 0; idx < u8tmp; idx++)
        {
            mbox.writeBoxBuf(MBX_OFFSET_DATA + 4 + idx, wdata[idx]);       				//set Dat4~
        }
        break;
    case MBXI2C_ReadWrite:
    case MBXI2C_WriteRead:
        if (*rlen > MBXSMB_BUF_LEN)   //dat04~dat2C size
        {
            u8tmp = MBXSMB_BUF_LEN;
        }
		else
		{
			u8tmp = (uint8_t) *rlen;
		}
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 2, u8tmp) != _err_noerror)
			return SMB_ST_ERR_UNKNOWN;	//set Dat02, read count
    case MBXSMB_WriteBlock:
        if (wlen > MBXSMB_BUF_LEN)   //dat04~dat2C size
        {
            u8tmp = MBXSMB_BUF_LEN;
        }
        else
		{
			u8tmp = wlen;
		}
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 3, u8tmp) != _err_noerror)
			return SMB_ST_ERR_UNKNOWN;	//set Dat03, write count
        for (idx = 0; idx < u8tmp; idx++)
        {
            mbox.writeBoxBuf(MBX_OFFSET_DATA + 4 + idx, wdata[idx]);       				//set Dat4~
        }
        break;
    default:
        return SMB_ST_ERR_UNKNOWN;
    }
    //start transaction
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, icmd) != _err_noerror)
		return SMB_ST_ERR_UNKNOWN;            	//set CMD
    if (mbox.waitBoxBusy() != _err_noerror)
		return SMB_ST_ERR_UNKNOWN;            						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return (SMB_ST_DONE|SMB_ST_ERR_UNKNOWN);  //read status
    if (u8tmp != 0x80) return u8tmp;                  									//fail
    //read data from device
    switch (icmd & 0x7F)  //command skip PEC
    {
    case MBXSMB_WriteWord:
    case MBXSMB_QuickWrite:
    case MBXSMB_SendByte:
    case MBXSMB_WriteByte:
    case MBXSMB_WriteBlock:
    case MBXSMB_QuickRead:
        break;
    case MBXSMB_ReadWord:
        mbox.readBoxBuf(MBX_OFFSET_DATA + 5, &(rdata[1]));                        //get Dat5
    case MBXSMB_ReceiveByte:
    case MBXSMB_ReadByte:
        mbox.readBoxBuf(MBX_OFFSET_DATA + 4, &(rdata[0]));                        //set Dat4
        break;
    case MBXI2C_WriteReadBuf:
        if (*rlen >= MBX_BUF_LEN)   //buffer size mxx is 256 bytes
        {
            u16tmp = MBX_BUF_LEN;
        }
		else
		{
			u16tmp = *rlen;
		}
        mbx_ec.readBufRam(rdata, u16tmp);
        break;
    case MBXI2C_ReadWrite:
    case MBXI2C_WriteRead:
        if (*rlen > MBXSMB_BUF_LEN)   //dat04~dat2C size
        {
            u8tmp = MBXSMB_BUF_LEN;
        }
        else
		{
			u8tmp = (uint8_t) *rlen;
		}
        for (idx = 0; idx < u8tmp; idx++)
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA + 4 + idx, &(rdata[idx]));
        }
        break;
    case MBXSMB_ReadBlock:
        mbox.readBoxBuf(MBX_OFFSET_DATA + 2, &u8tmp);           //get dat02
        *rlen = u8tmp;
        for (idx = 0; idx < *rlen; idx++)
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA + 4 + idx, &(rdata[idx]));
        }
        break;
    default:
        return SMB_ST_ERR_UNKNOWN;
    }
    return SMB_ST_DONE;
}
/*===========================================================
 * Name   : mbx_smb_getFreq
 * Purpose: get smbus frequency
 * Input  : did- device id
 *          freq- data pointer. return frequency. unit: KHz
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_smb_getFreq(uint8_t did, uint16_t *freq)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                							//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;         			//set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_SMB_FREQ) != _err_noerror)
		return _MBEr_Fail;      //set cmd 0x34
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                							//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      				//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      						//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      				//read dat0
    switch (u8tmp)
    {
    case 0x00:
        if (mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &u8tmp) != _err_noerror)
			return _MBEr_Fail;      		//read dat1
        *freq = (uint16_t)u8tmp;
        break;
    case 0x01:
        *freq = 50;
        break;
    case 0x02:
        *freq = 100;
        break;
    case 0x03:
        *freq = 400;
        break;
    }
    return _MBEr_Success;
}
/*===========================================================
 * Name   : mbx_smb_setFreq
 * Purpose: set smbus frequency
 * Input  : did- device id
 *          freq- frequency. unit: KHz
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_smb_setFreq(uint8_t did, uint16_t freq)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                				//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;        	//set PAPA 0, device id
    switch (freq)
    {
    case 50:
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA, 0x01) != _err_noerror)
			return _MBEr_Fail;    //set DAT00
        break;
    case 100:
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA, 0x02) != _err_noerror)
			return _MBEr_Fail;    //set DAT00
        break;
    case 400:
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA, 0x03) != _err_noerror)
			return _MBEr_Fail;    //set DAT00
        break;
    default:
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA, 0x00) != _err_noerror)
			return _MBEr_Fail;    //set DAT00
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, (uint8_t)freq) != _err_noerror)
			return _MBEr_Fail; //set DAT01
        break;
    }
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_SMB_FREQ) != _err_noerror)
		return _MBEr_Fail;	//set cmd 0x35
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                				//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      	//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      			//fail
    return _MBEr_Success;
}

/*===========================================================
 * Name   : mbx_smb_printErrMsg
 * Purpose: print smbus error message
 * Input  : errcode - error code
 * Output : None
 *===========================================================*/
void mbx_smb_printErrMsg(int errcode)
{
	fprintf(stderr, "\nError: ");
	switch(errcode & 0x7F)
	{
	case SMB_ST_ERR_ADDR_NAK:
		fprintf(stderr, "Address not ack\n");
		break;
	case SMB_ST_ERR_CMD_NAK:
		fprintf(stderr, "Command not ack\n");
		break;
	case SMB_ST_ERR_CMD_DENY:
		fprintf(stderr, "Address access deny\n");
		break;
	case SMB_ST_ERR_UNKNOWN:
		fprintf(stderr, "Unknown failure\n");
		break;
	case SMB_ST_ERR_DEV_DENY:
		fprintf(stderr, "Device access deny\n");
		break;
	case SMB_ST_ERR_TIMEOUT:
		fprintf(stderr, "Timeout\n");
		break;
	case SMB_ST_ERR_PROTOCOL:
		fprintf(stderr, "Unsupport protocol\n");
		break;
	case SMB_ST_Busy:
		fprintf(stderr, "SMBus busy\n");
		break;
	case SMB_ST_ERR_PEC:
		fprintf(stderr, "SMBus PEC error\n");
		break;
	case SMB_ST_ERR_ALARM:
		fprintf(stderr, "SMBus alarm\n");
		break;
	case SMB_ST_ERR_DEVID:
		fprintf(stderr, "Device ID error\n");
		break;
	case SMB_ST_ERR_I2CDEV:
		fprintf(stderr, "No I2C device\n");
		break;
	case SMB_ST_ERR_DEVTYPE:
		fprintf(stderr, "Unknown device type\n");
		break;
	case SMB_ST_ERR_ADDR:
		fprintf(stderr, "No default address\n");
		break;
	case SMB_ST_ERR_PAGE_WR:
		fprintf(stderr, "SMBus page write size error\n");
		break;
	case SMB_ST_ERR_SLV_CH:
		fprintf(stderr, "Slave channel error\n");
		break;
	case SMB_ST_ERR_EEP:
		fprintf(stderr, "EEPROM data protect\n");
		break;
	default:
		fprintf(stderr, "Undefine error code(%0X)\n",errcode);
		break;
	}
	fflush(stderr);
}


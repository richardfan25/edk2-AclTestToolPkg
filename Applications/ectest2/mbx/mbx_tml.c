#include "../global.h"

#include "mbx.h"
#include "mbx_tml.h"

/*==============================================================*/
int mbx_tml_init(void);
int mbx_tml_updateDevice(void);
int mbx_tml_writeDevice(uint8_t dev_no, uint8_t smbdid, uint8_t fanid, uint8_t *wdata, uint8_t *len);

/*==============================================================*/
stMbxTml mbx_tml = {
	/* *dev			*/	tmldev,
	/* *dev_cnt		*/	(sizeof(tmldev)/sizeof(stTmlDev)),
	/* *init		*/	&mbx_tml_init,
    /* *updateDevice*/	&mbx_tml_updateDevice,
    /* *writeDevice	*/	&mbx_tml_writeDevice,
    /* *getTypeStr	*/	&tml_getTypeStr,
    /* *getType		*/	&tml_getType,
};

/*===========================================================
 * Name   : mbx_tml_init
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tml_init(void)
{
	uint8_t cnt;
	
	for(cnt = 0; cnt < mbx_tml.dev_cnt; cnt++)
	{
		tml_clearData(cnt);
	}

    return mbox.init();
}
/*===========================================================
 * Name   : tml_updateDevice
 * Purpose: read thermal zond data
 * Input  : znum- zone number
 *          smbdid - return smbus device id
 *          fanid - return related fan device id
 *          rdata - return thermal zone structure, must bigger than structure length. more than 6 bytes
 *          len - return structure length
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tml_updateDevice(void)
{
    uint8_t u8tmp;
    uint8_t	len;
	uint8_t tml_no;
	uint8_t smbdid;
	uint8_t fanid;
	uint8_t rbuf[8];

	for(tml_no = 0; tml_no < mbx_tml.dev_cnt; tml_no++)
	{
		mbox.clearBoxBuf();
		if (mbox.waitBoxBusy() != _err_noerror)
			return _MBEr_Fail;               						//wait cmd clear
		if (mbox.writeBoxBuf(MBX_OFFSET_PARA, tml_no) != _err_noerror)
			return _MBEr_Fail;       			//set PAPA, zone number
		if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_THERMZONE) != _err_noerror)
			return _MBEr_Fail; //set Cmd 0x42
		if (mbox.waitBoxBusy() != _err_noerror)
			return _MBEr_Fail;               						//wait cmd clear
		if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
			return _MBEr_Fail;     			//read status
		if (u8tmp != _MBEr_Success)
			return u8tmp;                 //fail

		if (mbox.readBoxBuf(MBX_OFFSET_DATA_END, &len) != _err_noerror)
			return _MBEr_Fail; 			// get dat2C, structure length

		if(isNewStuckVer)
		{
			for (u8tmp = 0; u8tmp < len; u8tmp++)
			{
				if (mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(rbuf[u8tmp])) != _err_noerror)
					return _MBEr_Fail; //read structure
			}
			tml_clearData(tml_no);
			tml_updateData2(tml_no, rbuf, len);
		}
		else
		{
			if (mbox.readBoxBuf(MBX_OFFSET_DATA, &smbdid) != _err_noerror)
				return _MBEr_Fail;      		// get dat00, smbus device id
			if (mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &fanid) != _err_noerror)
				return _MBEr_Fail;       		// get dat01, fan device id
			
			for (u8tmp = 0; u8tmp < len; u8tmp++)
			{
				if (mbox.readBoxBuf(MBX_OFFSET_DATA + 2 + u8tmp, &(rbuf[u8tmp])) != _err_noerror)
					return _MBEr_Fail;    //read structure
			}
			tml_clearData(tml_no);
			tml_updateData(tml_no, smbdid, fanid, rbuf, len);
		}
	}
	
    return _MBEr_Success;
}
/*===========================================================
 * Name   : tml_writeDevice
 * Purpose: write thermal zond data
 * Input  : znum- zone number
 *          smbdid - smbus device id
 *          fanid - related fan device id
 *          wdata - thermal zone structure, must bigger than structure length. more than 6 bytes
 *          len - structure length. If input length is bigger than structure, return real size of structure.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tml_writeDevice(uint8_t dev_no, uint8_t smbdid, uint8_t fanid, uint8_t *wdata, uint8_t *len)
{
    uint8_t u8tmp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               				//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, dev_no) != _err_noerror)
		return _MBEr_Fail;       	//set PAPA, zone number
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, smbdid) != _err_noerror)
		return _MBEr_Fail;     	//set dat00, smbus id
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, fanid) != _err_noerror)
		return _MBEr_Fail;   //set dat01, fan id
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA_END, *len) != _err_noerror)
		return _MBEr_Fail;    //set dat2c, structure size

    for (idx = 0; idx < *len; idx++)
    {
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 2 + idx, wdata[idx]) != _err_noerror)
			return _MBEr_Fail;	//input structure data
    }
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_THERMZONE) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd 0x43
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               								//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     					//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                 //fail

    if (mbox.readBoxBuf(MBX_OFFSET_DATA_END, &u8tmp) != _err_noerror)
		return _MBEr_Fail; 					// get dat2C, structure length
    *len = u8tmp;

    return _MBEr_Success;
}

#include "../global.h"

#include "mbx.h"
#include "mbx_fan.h"

/*==============================================================*/
int mbx_fan_init(void);
int mbx_fan_updateDevice(void);
int mbx_fan_writeDevice(uint8_t dev_no, uint8_t fanid, uint8_t speedid, uint8_t *wdata, uint8_t *len);
int mbx_fan_toArray(uint8_t fanid, uint8_t *u8tmp, uint8_t *len);

/*==============================================================*/
stMbxFan 	mbx_fan = {
	/* *dev			*/	fandev,
	/* *dev_cnt		*/	(sizeof(fandev)/sizeof(stFanDev)),
	/* *init		*/	&mbx_fan_init,
    /* *updateDevice*/	&mbx_fan_updateDevice,
    /* *writeDevice	*/	&mbx_fan_writeDevice,
    /* *toArray		*/	&mbx_fan_toArray,
    /* *getCtrlMode	*/	&fan_getCtrlMode,
    /* *setCtrlMode	*/	&fan_setCtrlMode,
};

/*===========================================================
 * Name   : mbx_fan_init
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_fan_init(void)
{
	uint8_t cnt;
	
	for(cnt = 0; cnt < mbx_fan.dev_cnt; cnt++)
	{
		fan_clearData(cnt);
	}

    return mbox.init();
}

/*===========================================================
 * Name   : fan_updateDevice
 * Purpose: read smart fan
 * Input  : fnum- fan number
 *          fanid - return smbus device id
 *          speedid - return related fan device id
 *          rdata - return fan structure, must bigger than structure length. more than 16 bytes
 *          len - return structure length
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_fan_updateDevice(void)
{
    uint8_t u8tmp;
	uint8_t len;
	uint8_t rbuf[8];
	uint8_t	fan_no;
	uint8_t fan_id;
	uint8_t fan_sp;

	if(mbx_fan.dev_cnt == 0)
	{
		return _MBEr_Fail;
	}
	// read fan device info from ec via mailbox
	for(fan_no = 0; fan_no < mbx_fan.dev_cnt; fan_no++)
	{
		mbox.clearBoxBuf();
		if (mbox.waitBoxBusy() != _err_noerror)
			return _MBEr_Fail;               							//wait cmd clear
		if (mbox.writeBoxBuf(MBX_OFFSET_PARA, fan_no) != _err_noerror)
			return _MBEr_Fail;       				//set PAPA, zone number
		if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_SMARTFAN) != _err_noerror)
			return _MBEr_Fail;      //set Cmd 0x40
		if (mbox.waitBoxBusy() != _err_noerror)
			return _MBEr_Fail;               							//wait cmd clear
		if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
			return _MBEr_Fail;     				//read status
		if (u8tmp != _MBEr_Success)
			return u8tmp;                 //fail

		if (mbox.readBoxBuf(MBX_OFFSET_DATA_END, &len) != _err_noerror)
			return _MBEr_Fail;					// get dat2C, structure length
		
		if(isNewStuckVer)
		{
			for (u8tmp = 0; u8tmp < len; u8tmp++)
			{
				if (mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(rbuf[u8tmp])) != _err_noerror)
					return _MBEr_Fail;    //read structure
			}
			// assign data to fandev
			fan_clearData(fan_no);
			fan_updatData2(fan_no, rbuf, len);
		}
		else
		{
			if (mbox.readBoxBuf(MBX_OFFSET_DATA, &fan_id) != _err_noerror)
				return _MBEr_Fail;      			// get dat00, fan device id
			if (mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &fan_sp) != _err_noerror)
				return _MBEr_Fail;    			// get dat01, speed device id
			
			for (u8tmp = 0; u8tmp < len; u8tmp++)
			{
				if (mbox.readBoxBuf(MBX_OFFSET_DATA + 2 + u8tmp, &(rbuf[u8tmp])) != _err_noerror)
					return _MBEr_Fail;    //read structure
			}
			// assign data to fandev
			fan_clearData(fan_no);
			fan_updatData(fan_no, fan_id, fan_sp, rbuf, len);
		}
	}

    return _MBEr_Success;
}
/*===========================================================
 * Name   : fan_writeDevice
 * Purpose: write smart fan data
 * Input  : fnum- fan number
 *          fanid - fan device id
 *          speedid - related fan device id
 *          wdata - fan structure, must bigger than structure length. more than 16 bytes
 *          len - structure length. If input length is bigger than structure, return real size of structure.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_fan_writeDevice(uint8_t dev_no, uint8_t fanid, uint8_t speedid, uint8_t *wdata, uint8_t *len)
{
    uint8_t u8tmp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               					//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, dev_no) != _err_noerror)
		return _MBEr_Fail;       	//set PAPA, zone number
    
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA_END, *len) != _err_noerror)
		return _MBEr_Fail;       	//set dat2c, structure size
    
	if(isNewStuckVer)
	{
		for (idx = 0; idx < *len; idx++)
		{
			if (mbox.writeBoxBuf(MBX_OFFSET_DATA + idx, wdata[idx]) != _err_noerror)
				return _MBEr_Fail;	//input structure data
		}
	}
	else
	{
		if (mbox.writeBoxBuf(MBX_OFFSET_DATA, fanid) != _err_noerror)
			return _MBEr_Fail;      		//set dat00, fan id
		if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, speedid) != _err_noerror)
			return _MBEr_Fail;    	//set dat01, speed id
		
		for (idx = 0; idx < *len; idx++)
		{
			if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 2 + idx, wdata[idx]) != _err_noerror)
				return _MBEr_Fail;	//input structure data
		}
	}
    
	if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_SMARTFAN) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd 0x41
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

/*===========================================================
 * Name   : mbx_fan_toArray
 * Purpose: assign smart fan data
 * Input  : fanid - fan device id
 *          u8tmp - array data
 *          len - length
 * Output : enum _MBEr_Success
 *===========================================================*/
int mbx_fan_toArray(uint8_t fanid, uint8_t *u8tmp, uint8_t *len)
{
	if(isNewStuckVer)
	{
		fan_toArray2(fanid, u8tmp, len);
	}
	else
	{
		fan_toArray(fanid, u8tmp, len);
	}
	
	return _MBEr_Success;
}
 
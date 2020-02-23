#include "global.h"

#include "mbx.h"
#include "mbx_tpro.h"

/*==============================================================*/
int mbx_tpro_init(void);
int mbx_tpro_updateDevice(void);
int mbx_tpro_writeDevice(uint8_t dev_no, uint8_t *wdata, uint8_t *len);

/*==============================================================*/
stMbxTpro mbx_tpro = {
	/* *dev			*/	tprodev,
	/* *dev_cnt		*/	(sizeof(tprodev)/sizeof(stTProDev)),
	/* *init		*/	&mbx_tpro_init,
    /* *updateDevice*/	&mbx_tpro_updateDevice,
    /* *writeDevice	*/	&mbx_tpro_writeDevice,
    /* *getTypeStr	*/	&tpro_getTypeStr,
    /* *toArray		*/	&tpro_toArray,
};

/*===========================================================
 * Name   : mbx_tpro_init
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tpro_init(void)
{
	uint8_t cnt;
	
	for(cnt = 0; cnt < mbx_tpro.dev_cnt; cnt++)
	{
		tpro_clearData(cnt);
	}

    return mbox.init();
}
/*===========================================================
 * Name   : tpro_updateDevice
 * Purpose: read thermal protect
 * Input  : NA
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tpro_updateDevice(void)
{
    uint8_t u8tmp;
	uint8_t tpro_no;
	uint8_t len;
	uint8_t rbuf[8];
	
	for(tpro_no = 0; tpro_no < mbx_tpro.dev_cnt; tpro_no++)
	{
		mbox.clearBoxBuf();
		if (!mbox.waitBoxBusy()) return _MBEr_Fail;               						//wait cmd clear
		if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, tpro_no)) return _MBEr_Fail;       		//set PAPA, zone number
		if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_THERMPRO)) return _MBEr_Fail;  //set Cmd 0x44
		if (!mbox.waitBoxBusy()) return _MBEr_Fail;               						//wait cmd clear
		if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;     			//read status
		if (u8tmp != _MBEr_Success) return u8tmp;                 //fail

		if (!mbox.readBoxBuf(MBX_OFFSET_DATA_END, &len)) return _MBEr_Fail;				// get dat2C, structure length    

		for (u8tmp = 0; u8tmp < len; u8tmp++)
		{
			if (!mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(rbuf[u8tmp]))) return _MBEr_Fail;	//read structure
		}
		tpro_clearData(tpro_no);
		tpro_updateData(tpro_no, rbuf, len);
	}
    return _MBEr_Success;
}
/*===========================================================
 * Name   : tpro_writeDevice
 * Purpose: write thremal protect
 * Input  : fnum- fan number
 *          wdata - protect structure, must bigger than structure length. more than 4 bytes
 *          len - structure length. If input length is bigger than structure, return real size of structure.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_tpro_writeDevice(uint8_t dev_no, uint8_t *wdata, uint8_t *len)
{
    uint8_t u8tmp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               				//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, dev_no)) return _MBEr_Fail;      //set PAPA, zone number
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA_END, *len)) return _MBEr_Fail;    //set dat2c, structure size
    
	for (idx = 0; idx < *len; idx++)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + idx, wdata[idx])) return _MBEr_Fail;	//input structure data
    }
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_THERMPRO)) return _MBEr_Fail;      //set Cmd 0x45
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               							//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;     				//read status
    if (u8tmp != _MBEr_Success) return u8tmp;                 //fail

    if (!mbox.readBoxBuf(MBX_OFFSET_DATA_END, &u8tmp)) return _MBEr_Fail; 				// get dat2C, structure length
    *len = u8tmp;

    return _MBEr_Success;
}


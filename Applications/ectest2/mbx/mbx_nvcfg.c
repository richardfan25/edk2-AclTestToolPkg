#include "../global.h"

#include "mbx.h"
#include "mbx_nvcfg.h"

/*==============================================================*/
int mbx_nvcfg_init(void);
int mbx_nvcfg_readStatus(uint8_t *rstatus);
int mbx_nvcfg_writeStatus(uint8_t wstatus);
int mbx_nvcfg_readControl(uint8_t *rstatus);
int mbx_nvcfg_writeControl(uint8_t wstatus);
int mbx_nvcfg_readBuf(uint8_t len, uint8_t *rdata, uint8_t offset);
int mbx_nvcfg_writeBuf(uint8_t len, uint8_t *wdata, uint8_t offset);

/*==============================================================*/
stMbxNvCfg mbx_nvcfg = {
	/* *init			*/	&mbx_nvcfg_init,
	/* *readStatus		*/	&mbx_nvcfg_readStatus,
	/* *writeStatus		*/	&mbx_nvcfg_writeStatus,
	/* *readControl		*/	&mbx_nvcfg_readControl,
	/* *writeControl	*/	&mbx_nvcfg_writeControl,
	/* *readBuf			*/	&mbx_nvcfg_readBuf,
	/* *writeBuf		*/	&mbx_nvcfg_writeBuf,
};

/*===========================================================
 * Name   : nvcfg_init
 * Purpose: init configuration
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_init(void)
{
    return mbox.init();
}
/*===========================================================
 * Name   : nvcfg_readStatus
 * Purpose: read configuration status
 * Input  : rstatus- pointer. return status byte
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_readStatus(uint8_t *rstatus)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_NVCFG_RD_STS) != _err_noerror)
		return _MBEr_Fail;   //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                 						//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, rstatus) != _err_noerror)
		return _MBEr_Fail;    			//read dat00

    return _MBEr_Success;
}
/*===========================================================
 * Name   : nvcfg_writeStatus
 * Purpose: write configuration status
 * Input  : wstatus- write status byte
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_writeStatus(uint8_t wstatus)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_NVCFG_WR_STS) != _err_noerror)
		return _MBEr_Fail;   //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, wstatus) != _err_noerror)
		return _MBEr_Fail;   			//set dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                 						//fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : nvcfg_readControl
 * Purpose: read configuration control
 * Input  : rstatus- pointer. return control byte
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_readControl(uint8_t *rstatus)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_NVCFG_RD_CTRL) != _err_noerror)
		return _MBEr_Fail;  //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                 						//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, rstatus) != _err_noerror)
		return _MBEr_Fail;    			//read dat00

    return _MBEr_Success;
}
/*===========================================================
 * Name   : nvcfg_writeControl
 * Purpose: write configuration control
 * Input  : wstatus- write control byte
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_writeControl(uint8_t wstatus)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_NVCFG_WR_CTRL) != _err_noerror)
		return _MBEr_Fail;  //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, wstatus) != _err_noerror)
		return _MBEr_Fail;  			//set dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                 						//fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : nvcfg_readBuf
 * Purpose: read configuration buffer
 * Input  : rdata- array pointer. read back data 
 *          len- data length
 *          offset- data offset
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_readBuf(uint8_t len, uint8_t *rdata, uint8_t offset)
{
    uint8_t u8tmp;
    uint8_t idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA,PARA_NVCFG_RD_CFGBUF) != _err_noerror)
		return _MBEr_Fail; //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA,offset) != _err_noerror)
		return _MBEr_Fail;     			//set dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA_END,len) != _err_noerror)
		return _MBEr_Fail;        		//set dat2C
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD,MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;    
       
    for (idx = 0; idx < len; idx++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + 1 + idx,  &(rdata[idx]));
    }

    return _MBEr_Success;
}
/*===========================================================
 * Name   : nvcfg_writeBuf
 * Purpose: write configuration buffer
 * Input  : wdata- array pointer. write data 
 *          len- data length
 *          offset- data offset
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_nvcfg_writeBuf(uint8_t len, uint8_t *wdata, uint8_t offset)
{
    uint8_t u8tmp;
    uint8_t idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA,PARA_NVCFG_WR_CFGBUF) != _err_noerror)
		return _MBEr_Fail; //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA,offset) != _err_noerror)
		return _MBEr_Fail;     			//set dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA_END,len) != _err_noerror)
		return _MBEr_Fail;        		//set dat2C

    for (idx = 0; idx < len; idx++)
    {
        mbox.writeBoxBuf(MBX_OFFSET_DATA+1 + idx,  wdata[idx]);
    }

    if (mbox.writeBoxBuf(MBX_OFFSET_CMD,MBX_CMD_NVCFG) != _err_noerror)
		return _MBEr_Fail;       	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;               						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;     			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;

    return _MBEr_Success;
}


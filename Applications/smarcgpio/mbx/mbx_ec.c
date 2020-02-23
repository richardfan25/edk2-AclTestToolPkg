#include "global.h"

#include "mbx.h"
#include "mbx_ec.h"

/*==============================================================*/
int 	mbx_ec_init(void);
int 	mbx_ec_readEcRam(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *rdata);
int 	mbx_ec_writeEcRam(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *wdata);
void 	mbx_ec_clearBufRam(void);
int 	mbx_ec_readBufRam(uint8_t *rdata, uint16_t len);
int 	mbx_ec_writeBufRam(uint8_t *wdata, uint16_t len);
int 	mbx_ec_readFwInfo(char *prjname, char *version);

/*==============================================================*/
stMbxEc mbx_ec = {
	/* *init		*/	&mbx_ec_init,
    /* *readEcRam	*/	&mbx_ec_readEcRam,
    /* *writeEcRam	*/	&mbx_ec_writeEcRam,
    /* *clearBufRam	*/	&mbx_ec_clearBufRam,
    /* *readBufRam	*/	&mbx_ec_readBufRam,
    /* *writeBufRam	*/	&mbx_ec_writeBufRam,
    /* *readFwInfo	*/	&mbx_ec_readFwInfo,
};

/*===========================================================
 * Name   : mbx_ec_init
 * Purpose: init mailbox ec control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_ec_init(void)
{
    return mbox.init();
}

/*===========================================================
 * Name   : mbx_ec_readEcRam
 * Purpose: read ec ram
 * Input  : bank- memory bank
 *          offset- memory offset
 *          len- data length
 *          rdata- data pointer. return data.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_ec_readEcRam(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *rdata)
{
    uint8_t	idx;
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                					//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, bank)) return _MBEr_Fail;        	//set PAPA
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA, offset)) return _MBEr_Fail;      	//set Dat00
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA_END, len)) return _MBEr_Fail;         //set Dat2C
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_ECRAM)) return _MBEr_Fail; //set Cmd 0x1E
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                					//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success) return u8tmp;                      				//fail

    for (idx = 0; idx < len; idx++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + 1 + idx, &u8tmp);
        rdata[idx] = u8tmp;
    }

    return _MBEr_Success;
}
/*===========================================================
 * Name   : mbx_ec_writeEcRam
 * Purpose: write ec ram
 * Input  : bank- memory bank
 *          offset- memory offset
 *          len- data length
 *          wdata- data pointer. input data.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_ec_writeEcRam(uint8_t bank, uint8_t offset, uint8_t len, uint8_t *wdata)
{
    uint8_t	idx;
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                				//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, bank)) return _MBEr_Fail;        //set PAPA
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA, offset)) return _MBEr_Fail;      //set Dat00
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA_END, len)) return _MBEr_Fail;     //set Dat2C

    for (idx = 0; idx < len; idx++)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + 1 + idx,wdata[idx])) return _MBEr_Fail;
    }

    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_ECRAM)) return _MBEr_Fail; //set Cmd 0x1F
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                					//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success) return u8tmp;                      				//fail

    return _MBEr_Success;
}

/*===========================================================
 * Name   : mbx_ec_clearBufRam
 * Purpose: clear buffer ram
 * Input  : NONE
 * Output : NONE
 *===========================================================*/
void mbx_ec_clearBufRam()
{
    mbox.clearBoxBuf();
    mbox.waitBoxBusy();           							//wait cmd clear
    mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_CLR_BUFRAM);   //set cmd 0xC0
    mbox.waitBoxBusy();           							//wait cmd clear
}
/*===========================================================
 * Name   : mbx_ec_readBufRam
 * Purpose: read data from buffer ram
 * Input  : rdata- read data pointer, 256 byte array
 *          len- data length(max 256)
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_ec_readBufRam(uint8_t *rdata, uint16_t len)
{
    uint8_t jcount;
    uint8_t addition;
    uint8_t banknum;
    uint8_t idx;
    uint8_t u8tmp;

    if (len > MBX_BUF_LEN)
    {
        len = MBX_BUF_LEN;
    }
    else if (len == 0)
    {
        return _MBEr_Fail;
    }
	
    mbox.clearBoxBuf();

    banknum	 = (uint8_t)(len / MBX_BUF_PAGE_LEN);
    addition = (uint8_t)(len % MBX_BUF_PAGE_LEN);
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                    //wait cmd clear

    for (idx = 0; idx < banknum; idx++)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, idx)) return _MBEr_Fail;      			//set PAPA 0, bank number
        if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_BUFRAM)) return _MBEr_Fail;    //set cmd 0xC1
        if (!mbox.waitBoxBusy()) return _MBEr_Fail;                						//wait cmd clear
        if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      			//read status
        if (u8tmp != _MBEr_Success) return u8tmp;                      					//fail

        for (jcount = 0; jcount < MBX_BUF_PAGE_LEN; jcount++)
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA+jcount, &(rdata[idx * MBX_BUF_PAGE_LEN + jcount]));  //read dat0~dat31
        }
    }

    if (addition != 0)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, idx)) return _MBEr_Fail;      			//set PAPA 0, bank number
        if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_BUFRAM)) return _MBEr_Fail;    //set cmd 0xC1
        if (!mbox.waitBoxBusy()) return _MBEr_Fail;                						//wait cmd clear
        if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      			//read status
        if (u8tmp != _MBEr_Success) return u8tmp;                      					//fail

        for (jcount = 0; jcount < addition; jcount++)
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA + jcount, &(rdata[idx * MBX_BUF_PAGE_LEN + jcount]));  //read dat00~dat31
        }
    }

    return _MBEr_Success;
}
/*===========================================================
 * Name   : mbx_ec_writeBufRam
 * Purpose: write data into buffer ram
 * Input  : wdata- write data pointer, 256 byte array
 *          len- data length
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_ec_writeBufRam(uint8_t *wdata, uint16_t len)
{
    uint8_t idx;
    uint8_t jcount;
    uint8_t banknum;
    uint8_t addition;
    uint8_t u8tmp;

    if (len > MBX_BUF_LEN)
    {
        len = MBX_BUF_LEN;
    }
    else if (len == 0)
    {
        return _MBEr_Fail;
    }
	
    mbox.clearBoxBuf();

    banknum	 = (uint8_t)(len / MBX_BUF_PAGE_LEN);
    addition = (uint8_t)(len % MBX_BUF_PAGE_LEN);
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;                    			//wait cmd clear

    for (idx = 0; idx < banknum; idx++)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, idx)) return _MBEr_Fail;							//set PAPA, bank number
        
		for (jcount = 0; jcount < MBX_BUF_PAGE_LEN; jcount++)
        {
            mbox.writeBoxBuf(MBX_OFFSET_DATA + jcount, wdata[idx * MBX_BUF_PAGE_LEN + jcount]); //write dat00~dat31
        }

        if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_BUFRAM)) return _MBEr_Fail;        	//set cmd 0xC2
        if (!mbox.waitBoxBusy()) return _MBEr_Fail;                								//wait cmd clear
        if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      					//read status
        if (u8tmp != _MBEr_Success) return u8tmp;                      							//fail
    }
    if (addition != 0)
    {
        if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, idx)) return _MBEr_Fail;      					//set PAPA 0, bank number
        
		for (jcount = 0; jcount < MBX_BUF_PAGE_LEN; jcount++)
        {
            if (jcount < addition)
                mbox.writeBoxBuf(MBX_OFFSET_DATA + jcount, wdata[idx * 16 + jcount]);    
            else
                mbox.writeBoxBuf(MBX_OFFSET_DATA + jcount, 0xff);   							//clear other data to 0xff
        }
        if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_BUFRAM)) return _MBEr_Fail;        	//set cmd 0xC2
        if (!mbox.waitBoxBusy()) return _MBEr_Fail;                								//wait cmd clear
        if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;      					//read status
        if (u8tmp != _MBEr_Success) return u8tmp;                      							//fail
    }

    return _MBEr_Success;
}
/*===========================================================
 * Name  : mbx_ec_readFwInfo
 * Pupose: get version
 * Input :  prjname-return project name, 9 bytes
 *          version-version code , 9 bytes
 * Output: return _pmerror_code
 *===========================================================*/
int mbx_ec_readFwInfo(char *prjname, char *version)
{
    uint8_t	idx;
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror) return _MBEr_Fail;           						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_VERSION) != _err_noerror) return _MBEr_Fail;  //set CMD 0xF0
    if (mbox.waitBoxBusy() != _err_noerror) return _MBEr_Fail;           						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror) return _MBEr_Fail; 			//read status
    
	for (idx = 0; idx < ProjectNameLength; idx++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + idx, &u8tmp);
        prjname[idx] = (char)u8tmp;
    }
    prjname[ProjectNameLength] = '\0';
    
	for (idx = ProjectNameLength; idx < VersionTotalLength; idx++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + idx, &u8tmp);
        version[idx - ProjectNameLength] = (char)u8tmp;
    }

    return _MBEr_Success;
}


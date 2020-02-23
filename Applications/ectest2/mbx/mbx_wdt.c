#include "../global.h"

#include "mbx.h"
#include "mbx_wdt.h"

/*==============================================================*/
int mbx_wdt_init(void);
int mbx_wdt_start(void);
int mbx_wdt_stop(void);
int mbx_wdt_reset(uint8_t *r_status);
int mbx_wdt_readLimit(uint8_t *r_status, uint16_t *limit);
int mbx_wdt_writeLimit(uint8_t *r_status, uint16_t *limit);
int mbx_wdt_bootStop(void);

/*==============================================================*/
stMbxWdt mbx_wdt = {
	/* *init		*/	&mbx_wdt_init,
    /* *start		*/	&mbx_wdt_start,
    /* *stop		*/	&mbx_wdt_stop,
    /* *reset		*/	&mbx_wdt_reset,
    /* *readLimit	*/	&mbx_wdt_readLimit,
    /* *writeLimit	*/	&mbx_wdt_writeLimit,
    /* *bootStop	*/	&mbx_wdt_bootStop,
};

/*===========================================================
 * Name   : mbx_wdt_init
 * Purpose: init mailbox watchdog control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_init(void)
{
    return mbox.init();
}
/*===========================================================
 * Name   : wdt_start
 * Purpose: watchdog start
 * Input  : NONE
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_start()
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_START) != _err_noerror)
		return _MBEr_Fail;  //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;      //set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      				//fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : wdt_stop
 * Purpose: watchdog stop
 * Input  : NONE
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_stop()
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_STOP) != _err_noerror)
		return _MBEr_Fail;   //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;      //set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      				//fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : mbx_wdt_reset
 * Purpose: watchdog reset
 * Input  : r_status- pointer. return watchdog status
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_reset(uint8_t *r_status)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_RESET) != _err_noerror)
		return _MBEr_Fail;  //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;      //set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      				//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, r_status) != _err_noerror)
		return _MBEr_Fail;    		//read dat00

    return _MBEr_Success;
}
/*===========================================================
 * Name   : wdt_readLimit
 * Purpose: read watchdog time limit
 * Input  : r_status- pointer. return watchdog status 
 *          limit- data array pointer. 6 word long. return limit data 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_readLimit(uint8_t *r_status, uint16_t *limit)
{
    uint8_t u8tmp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_RD_LIMIT) != _err_noerror)
		return _MBEr_Fail;	//set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;      //set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                					//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      		//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                     				//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, r_status) != _err_noerror)
		return _MBEr_Fail;    		//read dat00

    for (idx = 0; idx < WDT_LIMIT_SIZE; idx++)
    {
        mbox.readBoxBuf(idx * 2 + MBX_OFFSET_DATA + 1, &u8tmp);
        limit[idx] = (u8tmp << 8);
        mbox.readBoxBuf(idx * 2 + MBX_OFFSET_DATA + 2, &u8tmp);
        limit[idx] |= u8tmp;
    }
    return _MBEr_Success;
}
/*===========================================================
 * Name   : wdt_writeLimit
 * Purpose: write watchdog time limit
 * Input  : r_status- pointer. return watchdog status 
 *          limit- data array pointer. 6 word long. write limit data 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_writeLimit(uint8_t *r_status, uint16_t *limit)
{
    uint8_t u8tmp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_WR_LIMIT) != _err_noerror)
		return _MBEr_Fail;   //set Para

    for (idx = 0; idx < WDT_LIMIT_SIZE; idx++)    									//setup limit data
    {
        u8tmp = (limit[idx] >> 8 & 0xFF);  //hi byte
        if (mbox.writeBoxBuf(idx * 2 + MBX_OFFSET_DATA + 1, u8tmp) != _err_noerror)
			return _MBEr_Fail;
        u8tmp = (limit[idx] & 0xFF);     //low byte
        if (mbox.writeBoxBuf(idx * 2 + MBX_OFFSET_DATA + 2, u8tmp) != _err_noerror)
			return _MBEr_Fail;
    }
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;        	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      					//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, r_status) != _err_noerror)
		return _MBEr_Fail;    			//read dat00

    return _MBEr_Success;
}
/*===========================================================
 * Name   : wdt_bootStop
 * Purpose: boottime watchdog stop
 * Input  : NONE
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_wdt_bootStop()
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_WDT_STOP_BOOT) != _err_noerror)
		return _MBEr_Fail;  //set Para
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WDT) != _err_noerror)
		return _MBEr_Fail;        	//set Cmd
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;      			//read status
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;                						//wait cmd clear
    if (u8tmp != _MBEr_Success)
		return u8tmp;                      					//fail

    return _MBEr_Success;
}
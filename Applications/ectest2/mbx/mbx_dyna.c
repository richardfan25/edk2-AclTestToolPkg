#include "../global.h"
#include "mbx.h"
#include "mbx_dyna.h"

/*==============================================================*/
int mbx_dyna_init(void);
void mbx_dyna_exit(void);
int mbx_dyna_getTable(void);
int mbx_dyna_getPinstate(eDynclDevNo did, uint8_t *idata);
int mbx_dyna_setPinstate(eDynclDevNo did, uint8_t *odata);
uint8_t 	mbx_dyna_getHw(eDynclDevNo did);
eDynclDevNo	mbx_dyna_getDid(uint8_t pin_no);

/*==============================================================*/
stMbxDyna mbx_dyna = {
	/* *dev			*/	NULL,
	/* size			*/	0,
	/* *init		*/	&mbx_dyna_init,
	/* *exit		*/	&mbx_dyna_exit,
    /* *getTable	*/	&mbx_dyna_getTable,
    /* *getPinstate	*/	&mbx_dyna_getPinstate,
    /* *setPinstate	*/	&mbx_dyna_setPinstate,
    /* *getHw		*/	&mbx_dyna_getHw,
    /* *getDid		*/	&mbx_dyna_getDid,
	
    /* *getDevStr	*/	&dyna_getDevStr,
    /* *getTypeStr	*/	&dyna_getTypeStr,
    /* *getDevType	*/	&dyna_getDevType,
};

/*===========================================================
 * Name   : mbx_dyna_init
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_dyna_init(void)
{
    return mbox.init();
}
/*===========================================================
 * Name   : mbx_dyna_exit
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
void mbx_dyna_exit(void)
{
	if(mbx_dyna.dev == NULL)
		return;

	dyna_freeDevice(mbx_dyna.dev, mbx_dyna.size);
	
	mbx_dyna.dev = NULL;
	mbx_dyna.size = 0;
}
/*===========================================================
 * Name   : dyna_getPinstate
 * Purpose: According device id, get the pin state
 * Input  : did- device id 
 *          idata- two bytes array. return state data.
 *                  if device id == 0x74. input 5 byts array.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_dyna_getPinstate(eDynclDevNo did, uint8_t *idata)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            						//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     		//set PAPA 0 device id
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_HWPIN) != _err_noerror)
		return _MBEr_Fail; //set CMD 0x11
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  			//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                  //FAIL

    switch (dyna_getDevType(did))
    {
    case DDevType_GPIO:
    case DDevType_OEMGPIO:
    case DDevType_PWM:
    case DDevType_DAC:
        mbox.readBoxBuf(MBX_OFFSET_DATA, &(idata[0]));            	//get Dat00
        break;
    case DDevType_ADC:
    case DDevType_TACHO:
        mbox.readBoxBuf(MBX_OFFSET_DATA, &(idata[0]));            	//get Dat00
        mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &(idata[1]));          //get Dat01
        if (did == DDevNo_ADCCurrent)  //ADCCurrent
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA + 2, &(idata[2]));      //get Dat02
            mbox.readBoxBuf(MBX_OFFSET_DATA + 3, &(idata[3]));      //get Dat03
            mbox.readBoxBuf(MBX_OFFSET_DATA + 4, &(idata[4]));      //get Dat04
        }
        break;
    case DDevType_Func:
        if (did >= DDevNo_Temperature_CPU0 && did <= DDevNo_Temperature_SYS1)
        {
            mbox.readBoxBuf(MBX_OFFSET_DATA, &(idata[0])); //get Dat00
        }
        else
        {
            return _MBEr_Fail;
        }
        break;
    default:
        return _MBEr_Fail;
    }

    return _MBEr_Success;
}
/*===========================================================
 * Name   : dyna_setPinstate
 * Purpose: According device id, set the pin state
 * Input  : did- device id 
 *          odata- two bytes array. state data 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_dyna_setPinstate(eDynclDevNo did, uint8_t *odata)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            				//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     //set PAPA 0, device id

    switch (dyna_getDevType(did))
    {
    case DDevType_GPIO:
    case DDevType_OEMGPIO:
    case DDevType_PWM:
    case DDevType_DAC:
        if (mbox.writeBoxBuf(MBX_OFFSET_DATA, odata[0]) != _err_noerror)
			return _MBEr_Fail; 	//set Dat00
        break;
    case DDevType_ADC:
    case DDevType_TACHO:
    default:
        return _MBEr_Fail;
    }
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_HWPIN) != _err_noerror)
		return _MBEr_Fail;	//set CMD 0x12 write gpio command
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            						//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  			//read status
    if (u8tmp != 0x01) return u8tmp;                  //fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : mbx_dyna_getTable
 * Purpose: Get dynamic table
 * Input  : NA
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_dyna_getTable(void)
{
    uint8_t u8tmp;
	uint8_t	dev_cnt;
	
    uint8_t did_buf[DYNC_ITEM_MAX];
    uint8_t hw_buf[DYNC_ITEM_MAX];
    uint8_t pol_buf[DYNC_ITEM_MAX];

    //read device id
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           //wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_DYNATBL_DID) != _err_noerror)
		return _MBEr_Fail;   	//set PAPA 0, read device id
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_DYNATBL) != _err_noerror)
		return _MBEr_Fail;   	//set CMD 0x20
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           //wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail; 				//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;             //fail

    for (u8tmp = 0; u8tmp < DYNC_ITEM_MAX; u8tmp++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(did_buf[u8tmp]));  				//read Dat00~Dat1F
        if (did_buf[u8tmp] == DDevNo_Nodefine) //table end
            break;
    }
	
	dev_cnt = u8tmp;
	
    //read HW pin code
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           							//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_DYNATBL_HWPIN) != _err_noerror)
		return _MBEr_Fail;  //set PAPA 1, read HW pin
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_DYNATBL) != _err_noerror)
		return _MBEr_Fail;   	//set CMD 0x20
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           							//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail; 				//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;             //fail
	
    for (u8tmp = 0; u8tmp < dev_cnt; u8tmp++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(hw_buf[u8tmp]));  				//read Dat00~Dat1F
    }
	
    //read polarity
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           							//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, PARA_DYNATBL_ACTIVE) != _err_noerror)
		return _MBEr_Fail; //set PAPA 2, read polarity
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_DYNATBL) != _err_noerror)
		return _MBEr_Fail;   	//set CMD 0x20
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;           							//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail; 				//read status
    if (u8tmp != _MBEr_Success) return u8tmp;             //fail
	
    for (u8tmp = 0; u8tmp < dev_cnt; u8tmp++)
    {
        mbox.readBoxBuf(MBX_OFFSET_DATA + u8tmp, &(pol_buf[u8tmp]));  				//read Dat00~Dat1F
    }

	// create dev table
	if(mbx_dyna.dev != NULL)
	{
		dyna_freeDevice(mbx_dyna.dev, mbx_dyna.size);
		mbx_dyna.size = 0;
	}
	mbx_dyna.dev = dyna_newDevice(dev_cnt);
	if(mbx_dyna.dev == NULL) return _MBEr_Fail;
	mbx_dyna.size = dev_cnt;
	
	// update table content
	for (u8tmp = 0; u8tmp < dev_cnt; u8tmp++)
    {
		mbx_dyna.dev[u8tmp].did		= (eDynclDevNo) did_buf[u8tmp];
		mbx_dyna.dev[u8tmp].pin_no 	= hw_buf[u8tmp];
		mbx_dyna.dev[u8tmp].active 	= pol_buf[u8tmp];
		mbx_dyna.dev[u8tmp].type 	= dyna_getDevType(mbx_dyna.dev[u8tmp].did);
		dyna_getDevStr(mbx_dyna.dev[u8tmp].did, mbx_dyna.dev[u8tmp].type_str);
    }

    return _MBEr_Success;
}


/*===========================================================
 * Name  : getHw
 * Pupose: convert device id to hw pin
 * Input : iid-device id
 * Output: hw pin number, if not exist, return 0xFF
 *===========================================================*/
uint8_t mbx_dyna_getHw(eDynclDevNo did)
{
    uint8_t idx;
	
	if(mbx_dyna.dev == NULL)
		return DYNC_NULL;
	
    for (idx = 0; idx < mbx_dyna.size; idx++) {
        if (mbx_dyna.dev[idx].did == did) {
            return mbx_dyna.dev[idx].pin_no;
        }
    }
    return DYNC_NULL;
}

/*===========================================================
 * Name  : getDid
 * Pupose: convert hw pin to device id
 * Input : ihw-hw pin number
 * Output: device id, if not exist, return 0x00
 *===========================================================*/
eDynclDevNo mbx_dyna_getDid(uint8_t pin_no)
{
    uint8_t idx;
	
    if(mbx_dyna.dev == NULL)
		return DDevNo_Nodefine;
	
    for (idx = 0; idx < mbx_dyna.size; idx++) {
        if (mbx_dyna.dev[idx].pin_no == pin_no) {
            return mbx_dyna.dev[idx].did;
        }
    }
    return DDevNo_Nodefine;
}

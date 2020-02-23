#include "global.h"

#include "mbx.h"
#include "mbx_led.h"

/*==============================================================*/
int mbx_led_init(void);
int mbx_led_updateDevice(uint8_t dev_no, uint8_t* gpioid, stLedDev *rdata, uint8_t *len);
int mbx_led_writeDevice(uint8_t dev_no, uint8_t gpioid, stLedDev *wdata, uint8_t *len);

/*==============================================================*/
stMbxLed mbx_led = {
	/* *init		*/	&mbx_led_init,
    /* *updateDevice*/	&mbx_led_updateDevice,
    /* *writeDevice	*/	&mbx_led_writeDevice,
};

/*===========================================================
 * Name   : mbx_led_init
 * Purpose: init mailbox led control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_led_init(void)
{
    return mbox.init();
}

/*===========================================================
 * Name   : updateDevice
 * Purpose: read led data
 * Input  : lnum- led number
 *          gpioid - return gpio device id
 *          rdata - return led structure
 *          len - return structure length (byte)
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_led_updateDevice(uint8_t dev_no, uint8_t* gpioid, stLedDev *rdata, uint8_t *len)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               					//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, dev_no)) return _MBEr_Fail;       	//set PAPA, zone number
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_LED)) return _MBEr_Fail;   //set Cmd 0x46
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               					//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;     		//read status
    if (u8tmp != _MBEr_Success) return u8tmp;                 					//fail
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA, gpioid)) return _MBEr_Fail;     		// get dat00, gpio device id
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA_END, &u8tmp)) return _MBEr_Fail;		// get dat2C, structure length
	
    *len = u8tmp;
	
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &u8tmp)) return _MBEr_Fail;     	//read dat01, pin
    rdata->pin = u8tmp;
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA + 2, &u8tmp)) return _MBEr_Fail;     	//read dat02, control hi
    rdata->control = (u8tmp << 8);
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA + 3, &u8tmp)) return _MBEr_Fail;     	//read dat03, control low
    rdata->control |= u8tmp;
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA + 4, &u8tmp)) return _MBEr_Fail;     	//read dat04, behavior
    rdata->behavior = u8tmp;

    return _MBEr_Success;
}
/*===========================================================
 * Name   : writeDevice
 * Purpose: write led
 * Input  : lnum- zone number
 *          gpioid - gpio device id
 *          wdata - led structure
 *          len - structure length. If input length is bigger than structure, return real size of structure.
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_led_writeDevice(uint8_t dev_no, uint8_t gpioid, stLedDev *wdata, uint8_t *len)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               					//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, dev_no)) return _MBEr_Fail;       	//set PAPA, zone number
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA, gpioid)) return _MBEr_Fail;     		//set dat00, smbus id
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA_END, *len)) return _MBEr_Fail;       	//set dat2c, structure size
    u8tmp = wdata->pin;
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, u8tmp)) return _MBEr_Fail;     	//write dat01, pin
    u8tmp = (uint8_t)(wdata->control >> 8);
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + 2, u8tmp)) return _MBEr_Fail;     	//write dat02, control hi
    u8tmp = (uint8_t)(wdata->control & 0xFF);
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + 3, u8tmp)) return _MBEr_Fail;     	//write dat03, control low
    u8tmp = wdata->behavior;
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA + 4, u8tmp)) return _MBEr_Fail;     	//write dat04, behavior
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_LED)) return _MBEr_Fail;   //set Cmd 0x47
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;               					//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;     		//read status
    if (u8tmp != _MBEr_Success) return u8tmp;                 					//fail
    if (!mbox.readBoxBuf(MBX_OFFSET_DATA_END, &u8tmp)) return _MBEr_Fail; 		// get dat2C, structure length
    *len = u8tmp;

    return _MBEr_Success;
}

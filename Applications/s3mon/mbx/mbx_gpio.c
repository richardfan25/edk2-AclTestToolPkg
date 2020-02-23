#include "global.h"

#include "mbx.h"
#include "mbx_gpio.h"

/*==============================================================*/
int mbx_gpio_init(void);
int mbx_gpio_getSetting(uint8_t did, uint8_t *idata);
int mbx_gpio_setSetting(uint8_t did, uint8_t odata);

/*==============================================================*/
stMbxGpio mbx_gpio = {
	/* *init		*/	&mbx_gpio_init,
    /* *getSetting	*/	&mbx_gpio_getSetting,
    /* *setSetting	*/	&mbx_gpio_setSetting,
};

/*===========================================================
 * Name   : mbx_gpio_init
 * Purpose: init mailbox gpio control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_gpio_init(void)
{
    return mbox.init();
}
/*===========================================================
 * Name   : gpio_getSetting
 * Purpose: read gpio pin setting
 * Input  : did- device id 
 *          idata- pointer. return setting 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_gpio_getSetting(uint8_t did, uint8_t *idata)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;            							//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA, did)) return _MBEr_Fail;     			//set PAPA 0, device id
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_GPIO_CFG)) return _MBEr_Fail;  //set CMD 0x30
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;            							//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;  				//read status
    if (u8tmp == _MBEr_Success)    //success
    {
        if (mbox.readBoxBuf(MBX_OFFSET_DATA, idata)) return _MBEr_Success;			//read DAT0
        else return _MBEr_Fail;
    }

    return u8tmp;
}

/*===========================================================
 * Name   : gpio_setSetting
 * Purpose: write gpio pin setting
 * Input  : did- device id 
 *          odata- pin setting
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_gpio_setSetting(uint8_t did, uint8_t odata)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;            							//wait cmd clear
    if (!mbox.writeBoxBuf(MBX_OFFSET_PARA,did)) return _MBEr_Fail;     				//set PAPA 0, oemgpio
    if (!mbox.writeBoxBuf(MBX_OFFSET_DATA,odata)) return _MBEr_Fail;   				//write data into Dat00
    if (!mbox.writeBoxBuf(MBX_OFFSET_CMD,MBX_CMD_WR_GPIO_CFG)) return _MBEr_Fail;   //set CMD 0x31
    if (!mbox.waitBoxBusy()) return _MBEr_Fail;           							//wait cmd clear
    if (!mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp)) return _MBEr_Fail;  				//read status
    if (u8tmp != _MBEr_Success) return u8tmp;		//fail

    return _MBEr_Success;
}


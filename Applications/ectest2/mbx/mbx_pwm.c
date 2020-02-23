#include "../global.h"

#include "mbx.h"
#include "mbx_pwm.h"

/*==============================================================*/
int mbx_pwm_init(void);
int mbx_pwm_getFreq(uint8_t did, uint16_t *freq);
int mbx_pwm_setFreq(uint8_t did, uint16_t freq);
int mbx_pwm_getPolarity(uint8_t did, uint8_t *pol);
int mbx_pwm_setPolarity(uint8_t did, uint8_t pol);

/*==============================================================*/
stMbxPwm mbx_pwm = {
	/* *init		*/	&mbx_pwm_init,
    /* *getFreq		*/	&mbx_pwm_getFreq,
    /* *setFreq		*/	&mbx_pwm_setFreq,
    /* *getPolarity	*/	&mbx_pwm_getPolarity,
    /* *setPolarity	*/	&mbx_pwm_setPolarity,
};

/*===========================================================
 * Name   : mbx_pwm_init
 * Purpose: init mailbox pwm control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_pwm_init(void)
{
    return mbox.init();
}
/*===========================================================
 * Name   : pwm_getFreq
 * Purpose: set pwm frequency
 * Input  : did- device id 
 *          freq- word data, frequency, unit: Hz 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_pwm_getFreq(uint8_t did, uint16_t *freq)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            							//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     			//set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_PWM_FREQ) != _err_noerror)
		return _MBEr_Fail;  //set CMD 0x36
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            							//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  				//read status
    if (u8tmp != _MBEr_Success) return u8tmp;              							//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA, &u8tmp) != _err_noerror)
		return _MBEr_Fail; 				//read dat0
    *freq = (uint16_t)u8tmp << 8;
    if (mbox.readBoxBuf(MBX_OFFSET_DATA + 1, &u8tmp) != _err_noerror)
		return _MBEr_Fail;   		//read dat1
    *freq |= (uint16_t)u8tmp;

    return _MBEr_Success;
}
/*===========================================================
 * Name   : pwm_setFreq
 * Purpose: set pwm frequency
 * Input  : did- device id 
 *          freq- word data, frequency, unit: Hz 
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_pwm_setFreq(uint8_t did, uint16_t freq)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            								//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     				//set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, (uint8_t)(freq >> 8)) != _err_noerror)
		return _MBEr_Fail; 	//set Dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA + 1, (uint8_t)freq) != _err_noerror)
		return _MBEr_Fail;   	//set Dat01
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_WR_PWM_FREQ) != _err_noerror)
		return _MBEr_Fail;  	//set CMD 0x32 write gpio command
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            								//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  					//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                  							//fail

    return _MBEr_Success;
}
/*===========================================================
 * Name   : pwm_getPolarity
 * Purpose: get pwm polarity
 * Input  : did- device id 
 *          pol- 1:high active; 0:low active
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_pwm_getPolarity(uint8_t did, uint8_t *pol)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            							//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     			//set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_RD_PWM_POL) != _err_noerror)
		return _MBEr_Fail;   //set CMD 0x37
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            							//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  				//read status
    if (u8tmp != _MBEr_Success) return u8tmp;             							//fail
    if (mbox.readBoxBuf(MBX_OFFSET_DATA,pol) != _err_noerror)
		return _MBEr_Fail;     				//set Dat00

    return _MBEr_Success;
}
/*===========================================================
 * Name   : pwm_setPolarity
 * Purpose: set pwm polarity
 * Input  : did- device id 
 *          pol- 1:high active; 0:low active
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_pwm_setPolarity(uint8_t did, uint8_t pol)
{
    uint8_t u8tmp;

    mbox.clearBoxBuf();
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            				//wait cmd clear
    if (mbox.writeBoxBuf(MBX_OFFSET_PARA, did) != _err_noerror)
		return _MBEr_Fail;     //set PAPA 0, device id
    if (mbox.writeBoxBuf(MBX_OFFSET_DATA, pol) != _err_noerror)
		return _MBEr_Fail;     //set Dat00
    if (mbox.writeBoxBuf(MBX_OFFSET_CMD, 0x33) != _err_noerror)
		return _MBEr_Fail;    	//set CMD 0x33 write gpio command
    if (mbox.waitBoxBusy() != _err_noerror)
		return _MBEr_Fail;            				//wait cmd clear
    if (mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp) != _err_noerror)
		return _MBEr_Fail;  	//read status
    if (u8tmp != _MBEr_Success)
		return u8tmp;                  	//fail
    
	return _MBEr_Success;
}
/******************************************************************
* Copyright(c) , Advantech Corporation 2013
* Module Name:  MAILBOX.CPP
* Purpose : Mailbox object
* Data    : 2013/07/29
* Author  : kk.chien
********************************************************************/
#include "global.h"
#include "mbx.h"

/*==============================================================*/
int		mbx_init(void);
void 	mbx_clearBoxBuf(void);
int 	mbx_setAccessType(eMBXCh imbx_ch);
eMBXCh	mbx_getAccessType(void);

/*==============================================================*/
//mailbox global variable
static eMBXCh	mbx_ch 		 = MBXCh_EndAllCh;
static UINTN	mbx_cmdport	 = 0;
static UINTN	mbx_dataport = 0;
stMailbox	mbox = {
	/* hw_offset		*/	0x80,

    /* *init			*/	&mbx_init,
    /* *clearBoxBuf		*/	&mbx_clearBoxBuf,
    /* *waitBoxBusy		*/	NULL,
    /* *readBoxBuf		*/	NULL,
    /* *writeBoxBuf		*/	NULL,
    /* *setAccessType	*/	&mbx_setAccessType,
    /* *getAccessType	*/	&mbx_getAccessType,
};

/*===========================================================
 * Name  : _mbx_waitSetOBF
 * Pupose: wait ouput buffer full flag
 * Input : icmdPort-command port
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_waitSetOBF(void)
{
    uint16_t cnt;
    int 	 result = _err_obf_timeout;

    for (cnt = 0; cnt < MBX_TIMEOUT_LMT; cnt++)    		//time out loop
    {
        if (mIsFlag1(IoRead8(mbx_cmdport), OBF_BIT)) 	//check obf flag
        {
            result = _err_noerror;
            break;
        }
        delay(1);   //delay 1ms
    }
    return result;
}

/*===========================================================
 * Name  : _mbx_waitClrIBF
 * Pupose: wait input buffer clear flag
 * Input : icmdPort-command port
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_waitClrIBF(void)
{
    uint16_t cnt;
    int 	 result = _err_ibc_timeout;

    for (cnt = 0; cnt < MBX_TIMEOUT_LMT; cnt++)    		//time out loop
    {
        if (mIsFlag0(IoRead8(mbx_cmdport), IBF_BIT))     //check ibf flag
        {
            result = _err_noerror;
            break;
        }
        delay(1);   //delay 1ms
    }
    return result;
}

/*===========================================================
 * Name  : _mbx_adt_read_mailbox
 * Pupose: read mailbox
 * Input : offset-mailbox offset, odata-return data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_adt_read_mailbox(uint8_t offset, uint8_t* data)
{
    int 	result;
	
    result = _mbx_waitClrIBF();          //wait input buffer clear 
    if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      //clear obf
    //
    IoWrite8(mbx_cmdport, PMC_CMD_READ_MBX + offset);
    //
    result = _mbx_waitSetOBF();
    if (result != _err_noerror) return result;
    //
    *data = IoRead8(mbx_dataport);

    return result;
}
/*===========================================================
 * Name  : _mbx_acpi_read_mailbox
 * Pupose: read mailbox
 * Input : offset-mailbox offset, odata-return data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_acpi_read_mailbox(uint8_t offset, uint8_t* data)
{
    int 	result = _err_noerror;
	
    result = _mbx_waitClrIBF();
    if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      //clear obf
    //
    IoWrite8(mbx_cmdport, EC_CMD_READ_ACPI);
    //
    result = _mbx_waitClrIBF();
    if (result != _err_noerror) return result;
    //
    IoWrite8(mbx_dataport, ACPI_OFFSET_MBX + offset);
    //
    result = _mbx_waitSetOBF();
    if (result != _err_noerror) return result;
    //
    *data = IoRead8(mbx_dataport);
    return result;
}
/*===========================================================
 * Name  : _mbx_ite_read_mailbox
 * Pupose: read mailbox
 * Input : offset-mailbox offset, odata-return data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_ite_read_mailbox(uint8_t offset, uint8_t *data)
{
    int 	result = _err_noerror;
	
    //result = _mbx_waitClrIBF();          //wait input buffer clear 
    //if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      //clear obf
    //
    IoWrite8(mbx_cmdport, offset + mbox.hw_offset);
    *data = IoRead8(mbx_dataport);

    return result;
}
/*===========================================================
 * Name  : _mbx_adt_write_mailbox
 * Pupose: write mailbox
 * Input : offset-mailbox offset, idata-write data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_adt_write_mailbox(uint8_t offset, uint8_t idata)
{
    int 	result;
	
    result = _mbx_waitClrIBF();          			//wait input buffer clear 
    if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      					//clear obf
    //
    IoWrite8(mbx_cmdport, PMC_CMD_WRITE_MBX + offset);
    //
    result = _mbx_waitClrIBF();          			//wait input buffer clear 
    if (result != _err_noerror) return result;
    //
    IoWrite8(mbx_dataport,idata);

    return result;
}
/*===========================================================
 * Name  : _mbx_acpi_write_mailbox
 * Pupose: write mailbox
 * Input : offset-mailbox offset, idata-write data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_acpi_write_mailbox(uint8_t offset, uint8_t data)
{
    int result;

    result = _mbx_waitClrIBF();
    if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      //clear obf
    //
    IoWrite8(mbx_cmdport, EC_CMD_WRITE_ACPI);
    //
    result = _mbx_waitClrIBF();
    if (result != _err_noerror) return result;
    //
    IoWrite8(mbx_dataport, ACPI_OFFSET_MBX + offset);
    //
    result = _mbx_waitClrIBF();
    if (result != _err_noerror) return result;
    //
    IoWrite8(mbx_dataport, data);
    return result;
}
/*===========================================================
 * Name  : _mbx_ite_write_mailbox
 * Pupose: write mailbox
 * Input : offset-mailbox offset, idata-write data
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_ite_write_mailbox(uint8_t offset, uint8_t idata)
{
    int 	result = _err_noerror;
	
    //result = _mbx_waitClrIBF();
    //if (result != _err_noerror) return result;
    //
    IoRead8(mbx_dataport);      //clear obf
	
    IoWrite8(mbx_cmdport, offset + mbox.hw_offset);
    IoWrite8(mbx_dataport, idata);

    return result;
}
/*===========================================================
 * Name  : _mbx_adt_wait_busy
 * Pupose: wait ec finish mailbox process
 * Input : NA
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_adt_wait_busy(void)
{
    int	result;
    uint8_t	 u8temp;
	uint16_t cnt;
	
	for(cnt = 0; cnt < MBX_TIMEOUT_LMT; cnt++)
	{
		result = _mbx_waitClrIBF();          //wait input buffer clear 
		if (result != _err_noerror) return result;
		//
		IoRead8(mbx_dataport);      //clear obf
		//
		IoWrite8(mbx_cmdport, PMC_CMD_READ_MBX + MBX_OFFSET_CMD);
		//
		result = _mbx_waitSetOBF();
		if (result != _err_noerror) return result;
		//
		u8temp = IoRead8(mbx_dataport);

		if(u8temp == 00)
			return _err_noerror;
		delay(1);
	}
    return _err_MBoxTimeOut;
}

/*===========================================================
 * Name  : _mbx_acpi_wait_busy
 * Pupose: wait ec finish mailbox process
 * Input : NA
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_acpi_wait_busy(void)
{
    int	result;
    uint8_t	 u8temp;
	uint16_t cnt;
	
	for(cnt = 0; cnt < MBX_TIMEOUT_LMT; cnt++)
	{
		result = _mbx_waitClrIBF();
		if (result != _err_noerror) return result;
		//
		IoRead8(mbx_dataport);      //clear obf
		//
		IoWrite8(mbx_cmdport, EC_CMD_READ_ACPI);
		//
		result = _mbx_waitClrIBF();
		if (result != _err_noerror) return result;
		//
		IoWrite8(mbx_dataport, ACPI_OFFSET_MBX+MBX_OFFSET_CMD);
		//
		result = _mbx_waitSetOBF();
		if (result != _err_noerror) return result;
		//
		u8temp = IoRead8(mbx_dataport);
		if (result != _err_noerror) return result;

		if(u8temp == 00)
			return _err_noerror;
		delay(1);
	}
    return _err_MBoxTimeOut;
}

/*===========================================================
 * Name  : _mbx_ite_wait_busy
 * Pupose: wait ec finish mailbox process
 * Input : NA
 * Output: return _pmerror_code
 *===========================================================*/
int _mbx_ite_wait_busy(void)
{
    //int	result = _err_MBoxTimeOut;
    uint8_t	 u8temp;
	uint16_t cnt;
	
	for(cnt = 0; cnt < MBX_TIMEOUT_LMT; cnt++)
	{
		//result = _mbx_waitClrIBF();          //wait input buffer clear 
		//if (result != _err_noerror) return result;
		//
		IoRead8(mbx_dataport);      //clear obf
		//

		IoWrite8(mbx_cmdport, MBX_OFFSET_CMD + mbox.hw_offset);
		u8temp = IoRead8(mbx_dataport);

		if(u8temp == 00)
			return _err_noerror;
		delay(1);
	}
    return _err_MBoxTimeOut;
}
/*===========================================================
 * Name  : mbx_clearBoxBuf
 * Pupose: clear mailbox
 * Input : NA
 * Output: NA
 *===========================================================*/
void mbx_clearBoxBuf()
{
    mbox.waitBoxBusy();           						//wait cmd clear
    mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_CLEAR);   	//set CMD 0xFF
    delay(10);
}

/*===========================================================
 * Name  : mbx_setAccessType
 * Pupose: set mailbox access type
 * Input : imbx_ch-access type code
 * Output: return _pmerror_code
 *===========================================================*/
int mbx_setAccessType(eMBXCh imbx_ch)
{
    int 	result;
    uint8_t u8tmp = 0;
	
    switch (imbx_ch)
    {
        case MBXCh_ITEMBox:
            mbox.hw_offset	= 0x80;
			mbx_ch 		 	= MBXCh_ITEMBox;
            mbx_cmdport  	= MBX_PORT_ITE_CMD;
            mbx_dataport 	= MBX_PORT_ITE_DATA;
            mbox.readBoxBuf  = &_mbx_ite_read_mailbox;
            mbox.writeBoxBuf = &_mbx_ite_write_mailbox;
            mbox.waitBoxBusy = &_mbx_ite_wait_busy;
            break;
			
        case MBXCh_AdtIO:
            mbx_ch 		 = MBXCh_AdtIO;
            mbx_cmdport  = MBX_PORT_PMC_CMD;
            mbx_dataport = MBX_PORT_PMC_DATA;
			mbox.readBoxBuf  = &_mbx_adt_read_mailbox;
            mbox.writeBoxBuf = &_mbx_adt_write_mailbox;
            mbox.waitBoxBusy = &_mbx_adt_wait_busy;
            break;
			
        case MBXCh_ACPIIO:
            mbx_ch 		 = MBXCh_ACPIIO;
            mbx_cmdport  = MBX_PORT_ACPI_CMD;
            mbx_dataport = MBX_PORT_ACPI_DATA;
			mbox.readBoxBuf  = &_mbx_acpi_read_mailbox;
            mbox.writeBoxBuf = &_mbx_acpi_write_mailbox;
            mbox.waitBoxBusy = &_mbx_acpi_wait_busy;
            break;
			
        default:
            goto err;
    }
	
    mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_CLR_BUFRAM);			//set cmd 0xC0
    mbox.waitBoxBusy();                           					//wait cmd clear
    result = mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp);       		//read status
	if(mbx_ch == MBXCh_ITEMBox)
	{
        if (result != _err_noerror)
            goto err;

        if (u8tmp != 1)
        {
            mbox.hw_offset = 0x00;
            mbox.writeBoxBuf(MBX_OFFSET_CMD, MBX_CMD_CLR_BUFRAM); 	//set cmd 0xC0
            mbox.waitBoxBusy();                           			//wait cmd clear
            result = mbox.readBoxBuf(MBX_OFFSET_STA, &u8tmp);      //read status
        }
	}
	
    if (result != _err_noerror || u8tmp != 1)
        goto err;

    return _err_noerror;

err:
	mbx_ch = MBXCh_EndAllCh;
	mbx_cmdport	= 0;
	mbx_dataport = 0;
	mbox.hw_offset 	= 0x80;
	mbox.readBoxBuf = NULL;
    mbox.writeBoxBuf = NULL;
    mbox.waitBoxBusy = NULL;

	return _err_MBoxAccessFail;
}
/*===========================================================
 * Name  : mbx_getAccessType
 * Pupose: set mailbox access type
 * Input : mbx_ch-access type code
 * Output: return _pmerror_code
 *===========================================================*/
eMBXCh mbx_getAccessType(void)
{
    return mbx_ch;
}
/*===========================================================
 * Name  : mbx_init
 * Pupose: init mailbox
 * Input : NA
 * Output: NA
 *===========================================================*/
int mbx_init(void)
{
	uint8_t cnt;
	
	// check init already
	if(mbox.readBoxBuf != NULL) return _err_noerror;

    // init mailbox access channel
	for(cnt = 0; cnt < MBXCh_EndAllCh; cnt++)
	{
		if(mbx_setAccessType(cnt) == _err_noerror)
			return _err_noerror;
	}
	
	return _err_MBoxAccessFail;
}

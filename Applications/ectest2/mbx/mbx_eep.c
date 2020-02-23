#include "../global.h"
#include "../dev/dynatbl.h"
#include "mbx.h"
#include "mbx_smb.h"
#include "mbx_eep.h"

/*==============================================================*/
int mbx_eep_init(void);
int mbx_eep_readData(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *rdata);
int mbx_eep_writeData(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *wdata, uint8_t len);

/*==============================================================*/
stMbxEep mbx_eep = {
    /* *init			*/	&mbx_eep_init,
    /* *readData		*/	&mbx_eep_readData,
    /* *writeData		*/	&mbx_eep_writeData,
};

/*===========================================================
 * Name   : mbx_eep_init
 * Purpose: init mailbox eeprom control
 * Input  : none
 * Output : enum _MBoxIDErr
 *===========================================================*/
int mbx_eep_init(void)
{
	if(mbox.init() != _err_noerror) return _err_MBoxAccessFail;

    return mbx_smb.init();
}

/*===========================================================
 * Name   : readData
 * Purpose: read eeprom 256 bytes with buffer ram
 * Input  : did- device id 
 *          adr- slave address
 *          offset- data offset
 *          rdata- 256 byte array pointer. read back data
 * Output : smbus status code
 *===========================================================*/
int mbx_eep_readData(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *rdata)
{
    uint8_t atemp;
    uint8_t wdata[2];
    uint16_t ctemp;

    mbox.clearBoxBuf();
    ctemp = MBX_BUF_LEN;                      			//read back 256 bytes
    if (did == DDevNo_SMBEEPROM || did == DDevNo_OEMEEP)    //below 2K type eeprom
    {
        atemp = adr + (offset >> 8) * 2;			// slave address, each bank step 2
        wdata[0] = (uint8_t)(offset & 0xFF);		// offset in bank
        return mbx_smb.request(MBXI2C_WriteReadBuf, // command
                   did,                             // device id
                   atemp,                           // slave address   
                   0,                               // smbus command, no use in I2C
                   wdata,                           // write data buffer
                   rdata,                           // read data buffer
                   1,                               // write data count
                   &ctemp);                         // read data count
    }
    else if (did == DDevNo_SMBEEP2K || did == DDevNo_OEMEEP2K)    // above 2k type eeprom
    {
        wdata[0] = (uint8_t)(offset >> 8); 		    // offset high byte (bank)
        wdata[1] = (uint8_t)(offset & 0xFF);   		// offset low byte
        return mbx_smb.request(MBXI2C_WriteReadBuf, // command                        
                   did,                             // device id                      
                   adr,                             // slave address                  
                   0,                               // smbus command, no use in I2C   
                   wdata,                           // write data buffer              
                   rdata,                           // read data buffer               
                   2,                               // write data count               
                   &ctemp);                         // read data count                
    }
    return SMB_ST_ERR_UNKNOWN;
}
/*===========================================================
 * Name   : writeData
 * Purpose: write buffer ram data into eeprom
 * Input  : did- device id 
 *          adr- slave address
 *          offset- data offset
 *          wdata- 256 byte array pointer. write data
 *          len- data length. max. 32 bytes
 * Output : smbus status code
 *===========================================================*/
int mbx_eep_writeData(uint8_t did, uint8_t adr, uint16_t offset, uint8_t *wdata, uint8_t len)
{
    uint8_t *wbuff;
    uint8_t rdata;
    uint8_t atemp;
    uint8_t	idx;

    mbox.clearBoxBuf();
    wbuff = NULL;
    if (len > MBX_BUF_PAGE_LEN)
		len = MBX_BUF_PAGE_LEN; //generally speaking, eeprom can not write more than 32 bytes at one time
    if (did == DDevNo_SMBEEPROM || did == DDevNo_OEMEEP)    //below 2K type eeprom
    {
        wbuff = (uint8_t *)malloc(len + 1);    			//assign new memory to put offset and write data
        atemp = adr + (offset >> 8) * 2;        		//slave address, each bank step 2
        wbuff[0] = (uint8_t)(offset & 0xFF);   			//offset in bank, put it into buffer first element
        for (idx = 0; idx < len; idx++)
        {
            wbuff[idx + 1] = wdata[idx];  				//put data into buffer
        }
        return mbx_smb.request(MBXI2C_WriteRead,   		// command                        
                   did,                         		// device id                      
                   atemp,                       		// slave address                  
                   0,                           		// smbus command, no use in I2C   
                   wbuff,                       		// write data buffer              
                   &rdata,                      		// read data buffer               
                   len + 1,                       		// write data count               
                   0);                         			// read data count                
    }
    else if (did == DDevNo_SMBEEP2K || did == DDevNo_OEMEEP2K)    // above 2k type eeprom
    {
        wbuff = (uint8_t *)malloc(len + 2);    			//assign new memory to put offset and write data
        wbuff[0] = (uint8_t)(offset >> 8);     			//offset high byte(bank), put it into buffer first element
        wbuff[1] = (uint8_t)(offset & 0xFF);   			//offset low byte, put it into buffer second element
        for (idx = 0; idx < len; idx++)
        {
            wbuff[idx + 2] = wdata[idx];  				//put write data into buffer
        }
        return mbx_smb.request(MBXI2C_WriteRead,   		// command                        
                   did,                         		// device id                      
                   adr,                       			// slave address                  
                   0,                           		// smbus command, no use in I2C   
                   wbuff,                       		// write data buffer              
                   &rdata,                      		// read data buffer               
                   len + 2,                     		// write data count               
                   0);                         			// read data count                
    }
    if (wbuff != NULL)
    {
        free(wbuff);    //release memory
    }
    return SMB_ST_ERR_UNKNOWN;
}

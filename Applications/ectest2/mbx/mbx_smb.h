#ifndef _MBX_SMB_H_
#define _MBX_SMB_H_

#include <stdint.h>

#define MBXSMB_BUF_LEN    		41

//SMBUS Status Code
#define SMB_ST_DONE       			BIT(7)
#define SMB_ST_ERR_EEP    			0x76
#define SMB_ST_ERR_SLV_CH   		0x75
#define SMB_ST_ERR_PAGE_WR 			0x74
#define SMB_ST_ERR_ADDR  			0x73
#define SMB_ST_ERR_DEVTYPE 			0x72
#define SMB_ST_ERR_I2CDEV   		0x71
#define SMB_ST_ERR_DEVID    		0x70
#define SMB_ST_ERR_ALARM    		0x40
#define SMB_ST_ERR_PEC      		0x1F
#define SMB_ST_Busy         		0x1A
#define SMB_ST_ERR_PROTOCOL 		0x19
#define SMB_ST_ERR_TIMEOUT  		0x18
#define SMB_ST_ERR_DEV_DENY 		0x17
#define SMB_ST_ERR_UNKNOWN  		0x13
#define SMB_ST_ERR_CMD_DENY 		0x12
#define SMB_ST_ERR_CMD_NAK  		0x11
#define SMB_ST_ERR_ADDR_NAK 		0x10


//mailbox I2C SMBUS control command list
typedef enum
{
    MBXI2C_WriteReadBuf = 0x01,		//0x01
    MBXSMB_QuickWrite,    			//0x02
    MBXSMB_QuickRead,     			//0x03
    MBXSMB_SendByte,      			//0x04
    MBXSMB_ReceiveByte,   			//0x05
    MBXSMB_WriteByte,     			//0x06
    MBXSMB_ReadByte,      			//0x07
    MBXSMB_WriteWord,     			//0x08
    MBXSMB_ReadWord,      			//0x09
    MBXSMB_WriteBlock,    			//0x0A
    MBXSMB_ReadBlock,     			//0x0B
    MBXI2C_ReadWrite = 0x0E,		//0x0E
    MBXI2C_WriteRead,     			//0x0F
    //SMBUS with PEC
    MBXSMB_QuickWritePEC = 0x82,	//0x82
    MBXSMB_QuickReadPEC,      		//0x83
    MBXSMB_SendBytePEC,       		//0x84
    MBXSMB_ReceiveBytePEC,    		//0x85
    MBXSMB_WriteBytePEC,      		//0x86
    MBXSMB_ReadBytePEC,       		//0x87
    MBXSMB_WriteWordPEC,      		//0x88
    MBXSMB_ReadWordPEC,       		//0x89
    MBXSMB_WriteBlockPEC,     		//0x8A
    MBXSMB_ReadBlockPEC,      		//0x8B
} eMBXI2C;

typedef struct _stMbxSmb{
    int (*init)(void);
    int (*request)(uint8_t icmd, uint8_t did, uint8_t address, uint8_t smbcmd,
					uint8_t *wdata, uint8_t *rdata, uint8_t wlen, uint16_t *rlen);
    int (*getFreq)(uint8_t did, uint16_t *freq);
    int (*setFreq)(uint8_t did, uint16_t freq);
    void (*printErrMsg)(int errcode);
} stMbxSmb;

extern stMbxSmb mbx_smb;

#endif // _MBX_SMB_H_


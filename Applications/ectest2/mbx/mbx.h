#ifndef _MBX_MBX_H_
#define _MBX_MBX_H_

#include <stdint.h>

#define MBX_TIMEOUT_LMT 		1000
#define MBX_BUF_LEN        		0x100
#define MBX_BUF_PAGE_LEN    	32

//Mailbox channel port
#define MBX_PORT_ITE_CMD			0x29E
#define MBX_PORT_ITE_DATA   		0x29F
#define MBX_PORT_PMC_CMD          	0x29A
#define MBX_PORT_PMC_DATA           0x299
#define MBX_PORT_ACPI_CMD           0x66
#define MBX_PORT_ACPI_DATA          0x62

//mailbox offset
#define MBX_OFFSET_CMD     			0x00
#define MBX_OFFSET_STA     			0x01
#define MBX_OFFSET_PARA    			0x02
#define MBX_OFFSET_DATA   			0x03
#define MBX_OFFSET_DATA_END  		0x2F

//////////mbox command//////////
#define MBX_CMD_RD_HWPIN			0x11
#define MBX_CMD_WR_HWPIN			0x12
#define MBX_CMD_RD_ECRAM			0x1E
#define MBX_CMD_WR_ECRAM			0x1F
#define MBX_CMD_DYNATBL				0x20
#define MBX_CMD_NVCFG       		0x21
#define MBX_CMD_WDT       			0x28
#define MBX_CMD_RD_GPIO_CFG   		0x30
#define MBX_CMD_WR_GPIO_CFG   		0x31
#define MBX_CMD_WR_PWM_FREQ     	0x32
#define MBX_CMD_WR_PWM_POL      	0x33
#define MBX_CMD_RD_SMB_FREQ     	0x34
#define MBX_CMD_WR_SMB_FREQ     	0x35
#define MBX_CMD_RD_PWM_FREQ     	0x36
#define MBX_CMD_RD_PWM_POL      	0x37
#define MBX_CMD_RD_SMARTFAN     	0x40
#define MBX_CMD_WR_SMARTFAN     	0x41
#define MBX_CMD_RD_THERMZONE    	0x42
#define MBX_CMD_WR_THERMZONE    	0x43
#define MBX_CMD_RD_THERMPRO   		0x44
#define MBX_CMD_WR_THERMPRO   		0x45
#define MBX_CMD_RD_LED          	0x46
#define MBX_CMD_WR_LED          	0x47
#define MBX_CMD_CLR_BUFRAM 			0xC0
#define MBX_CMD_RD_BUFRAM    		0xC1
#define MBX_CMD_WR_BUFRAM    		0xC2
#define MBX_CMD_VERSION        		0xF0
#define MBX_CMD_CLEAR          		0xFF

//mailbox access
typedef enum
{
    MBXCh_AdtIO,      //1
    MBXCh_ITEMBox,    //2
    MBXCh_ACPIIO,     //3
    MBXCh_EndAllCh,   //0
} eMBXCh;

typedef struct _stMailbox
{
	uint8_t	hw_offset;

    int		(*init)(void);
    void	(*clearBoxBuf)(void);
    int 	(*waitBoxBusy)(void);
    int 	(*readBoxBuf)(uint8_t offset, uint8_t* odata);
    int 	(*writeBoxBuf)(uint8_t offset, uint8_t idata);
	int 	(*setAccessType)(eMBXCh mbx_ch);
    eMBXCh (*getAccessType)(void);
} stMailbox;

// Note: Before using mbox for any activity, you must call mbox.init()
extern stMailbox mbox;

#endif // _MBX_MBX_H_


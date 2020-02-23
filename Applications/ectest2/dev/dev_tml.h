#ifndef _DEV_TML_H_
#define _DEV_TML_H_

#include <stdint.h>

#define TML_SRC_MAX			4

/*==============================================================*/
typedef enum 
{
    TMLType_Nodefine,  		//0x00
    TMLType_1L,         	//0x01
    TMLType_1R,         	//0x02
    TMLType_2L,         	//0x03
    TMLType_2R,         	//0x04
    TMLType_END,         	//0x05
} eTMLType;
/*==============================================================*/
typedef struct _stTmlDev{
    uint8_t smb_ch;        	//smbus channel
    uint8_t addr;          	//device slave address
    uint8_t cmd;           	//smbus command
    uint8_t status;    		//status
    uint8_t fan;           	//control fan code
    char 	temperature;   	//source temperature
    uint8_t fan_id;
    uint8_t smb_id;
	
	uint8_t tempID;
} stTmlDev;

extern stTmlDev tmldev[TML_SRC_MAX];
/*==============================================================*/
void 	tml_clearData(uint8_t dev_no);
void 	tml_updateData(uint8_t dev_no, uint8_t smbdid, uint8_t fanid, uint8_t *rdata, uint8_t len);
void 	tml_updateData2(uint8_t dev_no, uint8_t *rdata, uint8_t len);
void	tml_getTypeStr(eTMLType type, char * data);
eTMLType tml_getType(uint8_t dev_no);
#endif //_DEV_TML_H_


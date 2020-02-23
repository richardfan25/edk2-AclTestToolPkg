#ifndef _SMBTEST_H_
#define _SMBTEST_H_

#define SMBTEST_DATE		L"2018/08/16"
#define SMBTEST_VER			L"V1.5"

//retries
#define SMB_WAIT_RETRIES		3000

//smb err code : TBD
#define SMB_OK						0x00
#define SMB_ERR_WAIT_FREE_TMOUT	0x01
#define SMB_ERR_PROTOCOL			0x02
#define SMB_ERR_TXDONE_TMOUT		0x03
#define SMB_ERR_FAILED				0x04
#define SMB_ERR_ARBI				0x05
#define SMB_ERR_TMOUT				0x06
#define SMB_ERR_PEC					0x07
#define SMB_ERR_NACK				0x08
#define SMB_ERR_BUF					0x09
#define SMB_ERR_PARAMETERS		0x0A

//smb command : smb_data->mode
#define SMB_CMD_QUICK			0
#define SMB_CMD_BYTE			1
#define SMB_CMD_BYTE_DATA		2
#define SMB_CMD_WORD_DATA	3
#define SMB_CMD_PROC_CALL		4	// process call
#define SMB_CMD_BLOCK			5
#define SMB_CMD_I2C_READ		6
#define SMB_CMD_BLOCK_PROC	7
#define SMB_CMD_MASK			0x7	// not 0xF
#define SMB_CMD_PEC_MASK		0x80

//smb trans->addr
#define SMB_RW_WRITE		0
#define SMB_RW_READ		1
#define SMB_RW_MASK		0x1



typedef struct
{
	uint8_t init;
	uint8_t ch;		//eeprom channel
	uint8_t freq;		//eeprom frequncy
	uint8_t type;		//eeprom type, byte, word
	uint8_t addr;		//eeprom slave address
	uint16_t spos;	//eeprom start position
	uint16_t rwlen;	//eeprom rw data length
	uint8_t wsval;	//eeprom write start value
}Eeprom;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//thermal channel
	uint8_t freq;		//thermal frequncy
	uint8_t addr;		//thermal slave address
	uint8_t wsval;	//thermal write start value
}Thermal;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//dio board channel
	uint8_t freq;		//dio board frequncy
	uint8_t addr;		//dio board slave address
}DIO;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//smart battery channel
	uint8_t freq;		//smart battery frequncy
	uint8_t addr;		//smart battery slave address
}BATTERY;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//device channel
	uint8_t freq;		//device frequncy
	uint8_t addr;		//device slave address
	uint8_t cmd;		//device type, byte, word
	uint16_t rwlen;	//device rw data length
	uint8_t wsval;	//device rw data
}Device;

typedef struct
{
	uint8_t init;
	uint8_t ch;		//SPD channel
	uint8_t freq;		//SPD  frequncy
	uint8_t source_addr;		//SPD  source address
	uint8_t destination_addr;	//SPD  destination address
}SPD;

typedef struct
{
	Eeprom *eeprom;
	Thermal *thermal;
	DIO *dio;
	BATTERY *battery;
	Device *dev;
	SPD *spd;
}SmbusVariableConfig;

typedef struct
{
	uint8_t ch;		//SMBus channel
	uint8_t mode;	//Quick, Byte, Word, etc...
	uint8_t cmd;		//Command
	uint8_t addr;		//slave address [7:1], r/w [0]
	
	uint8_t wlen;	//length of write buffer
	uint8_t *wbuf;	//write buffer
	uint8_t rlen;		//length of read buffer
	uint8_t *rbuf;		//read buffer
	uint8_t *blen;		//block length

	uint8_t pec;
}SMBUS_RW;



//extern function
extern uint8_t SmbusReadByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *rbuf);
extern uint8_t SmbusWriteByte(uint8_t ch, uint8_t cmd, uint8_t addr, uint8_t *wbuf);



#endif // _SMBTEST_H_


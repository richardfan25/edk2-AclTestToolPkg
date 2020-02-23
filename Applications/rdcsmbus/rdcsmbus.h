#ifndef _RDCSMBUS_H_
#define _RDCSMBUS_H_

#define RDC_SMBUS_CLK_SRC			50000			// 50Mhz (unit: khz)

//retries
#define SMB_WAIT_RETRIES			3000

//smb register address offset
#define SMB_STS_REG_OFFSET		0x00
#define SMB_STS2_REG_OFFSET		0x01
#define SMB_CTL_REG_OFFSET			0x02
#define SMB_CMD_REG_OFFSET		0x03
#define SMB_SADD_REG_OFFSET		0x04
#define SMB_DAT0_REG_OFFSET		0x05
#define SMB_DAT1_REG_OFFSET		0x06
#define SMB_BLK_REG_OFFSET			0x07
#define SMB_PEC_REG_OFFSET		0x08
#define SMB_RSADD_REG_OFFSET		0x09
#define SMB_RSDAT0_REG_OFFSET		0x0A
#define SMB_RSDAT1_REG_OFFSET		0x0B
#define SMB_CTL2_REG_OFFSET		0x0C
#define SMB_PINCTL_REG_OFFSET		0x0D
#define SMB_CTL3_REG_OFFSET		0x0E
#define SMB_CTL4_REG_OFFSET		0x0F
#define SMB_NDADD_REG_OFFSET		0x10
#define SMB_NDLSB_REG_OFFSET		0x11
#define SMB_NDMSB_REG_OFFSET		0x12
#define SMB_CLKCTL1_REG_OFFSET	0x13
#define SMB_CLKCTL2_REG_OFFSET	0x14
#define SMB_EXCTL_REG_OFFSET		0x15	

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

const uint16_t build_date[] = L"2017/09/05";
const uint16_t build_version[] = L"v1.0";

typedef struct
{
	struct
	{
		uint8_t init	: 1;
		uint8_t rsvd	: 7;
	}Flag;
	uint16_t BaseAddr;
	uint16_t Freq;

	uint8_t DevAddr[32];
	uint8_t Cnt;
}RDC_SMBUS;

/*typedef struct
{

};*/

typedef struct
{
	uint8_t ch;		//SMBus channel
	uint8_t mode;	//Quick, Byte, Word, etc...
	uint8_t cmd;		//Command
	uint8_t addr;		//slave address [7:1], r/w [0]
	
	uint8_t wlen;		//length of write buffer
	uint8_t *wbuf;	//write buffer
	uint8_t rlen;		//length of read buffer
	uint8_t *rbuf;		//read buffer
	uint8_t *blen;		//block length

	uint8_t pec;
}SMBUS_RW;

#endif

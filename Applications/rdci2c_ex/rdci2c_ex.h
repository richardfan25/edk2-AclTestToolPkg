#ifndef __RDCTEST014_I2C_H__
#define __RDCTEST014_I2C_H__


//===========================================================================
//  Port : RDC EC
//===========================================================================
#define RDC_I2C_CLK_SRC			50000			// 50Mhz (unit: khz)
#define RDC_CHIP_ID				0x9610

#define RDC_EC_CMD_PORT		0x2F6
#define RDC_EC_STS_PORT		RDC_EC_CMD_PORT
#define RDC_EC_DAT_PORT		0x2F2
#define RDC_CFG_IDX_PORT		0x299	// 299h/499h
#define RDC_CFG_DAT_PORT		0x29A

#define RDC_LDN_I2C0			0x20
#define RDC_LDN_I2C1			0x21

/*==============================================================*/
// IO space
/*==============================================================*/
#define RDC_PNP_INDEX			0x299
#define RDC_PNP_DATA			0x29A

//i2c register address offset
#define I2C_CTL_REG_OFFSET		0x00
#define I2C_STS_REG_OFFSET		0x01
#define I2C_MYADD_REG_OFFSET	0x02
#define I2C_TXADD_REG_OFFSET	0x03
#define I2C_DAT_REG_OFFSET		0x04
#define I2C_CLK1_REG_OFFSET	0x05
#define I2C_CLK2_REG_OFFSET	0x06
#define I2C_EXCTL_REG_OFFSET	0x07

//=============================================================================
//  5.3.6.3.1  I2C_CTL : I2C 0/1 Control Reg.
//=============================================================================
#define   I2C_CTL_STOP_bm		0x2

//=============================================================================
//  5.3.6.3.2  I2C_STS : I2C 0/1 Status Reg.
//=============================================================================
#define   I2C_STS_RXRDY_bm		0x40
#define   I2C_STS_TXDONE_bm	0x20

//=============================================================================
//  5.3.6.3.3  I2C_MYADD : I2C 0/1 My Address Reg.
//=============================================================================
#define   I2C_MYADD_MYADDR_gm	0xFE
#define   I2C_MYADD_TCE_bm		0x1

//=============================================================================
//  5.3.6.3.6  I2C_CLK1 : I2C 0/1 I2C Clock Frequency Control 1 Reg.
//=============================================================================
#define   I2C_CLK1_PRESCALE1_gm	0xFF

//=============================================================================
//  5.3.6.3.7  I2C_CLK2 : I2C 0/1 I2C Clock Frequency Control 2 Reg.
//=============================================================================
#define   I2C_CLK2_FAST_bm			0x80
#define   I2C_CLK2_PRESCALE2_gm	0x7F

//=============================================================================
//  5.3.6.3.8  I2C_EXCTL : I2C 0/1 Extra Control Reg.
//=============================================================================
#define   I2C_EXCTL_I2CRST_bm		0x80

// REG : byte size
#define SET_REG_BM8(bm, reg)	outp(reg, inp(reg) | (bm))
#define TST_REG_BM8(bm, reg)	((inp(reg) & (bm)) != 0) 

#define inp(addr)			IoRead8(addr)
#define outp(addr,data)	IoWrite8(addr,data)


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
}RDC_I2C;



#endif


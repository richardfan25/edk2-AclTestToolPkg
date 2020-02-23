#ifndef __RDC_I2C_H__
#define __RDC_I2C_H__

#define RDC_I2C_CLK_SRC				50000			// 50Mhz (unit: khz)

#define RDC_I2C_BUS_TIMEOUT			500000			// us
#define RDC_I2C_DAT_TIMEOUT			35000			// us
#define RDC_I2C_CLK_COFF_A			1125
#define RDC_I2C_CLK_COFF_B			1000

#define RDC_I2C_CTRL_REG_OFFSET		0				// Control Reg
#define RDC_I2C_STAT_REG_OFFSET		1				// Status Reg
#define RDC_I2C_ADDR_REG_OFFSET		2				// Self Address Reg
#define RDC_I2C_TXADDR_REG_OFFSET	3				// Transmit Address REg
#define RDC_I2C_DATA_REG_OFFSET		4				// Data Reg for RX/TX operation
#define RDC_I2C_FREQ_REG_OFFSET		5				// Freq PreScale1 Control 1 Reg(Only in High Speed mode)
													//   Freq = 50M/(PreScale1) , if PreScale1 >= 15
													//   Freq = 50M/15 , if PreScale1 < 15
#define RDC_I2C_FREQ2_REG_OFFSET	6				// Freq PreScale2 Control 1 Reg
#define RDC_I2C_EXTRA_REG_OFFSET	7				// Extra Control Register
#define RDC_I2C_SEMAP_REG_OFFSET	8				// Semaphore Register

typedef union _CtrlReg_t{
	struct{
		uint8_t	NAKEn		: 1;
		uint8_t	STOP		: 1;
		uint8_t	STP_Inten	: 1;					// Slave mode stop interrupt En
		uint8_t	ARL_Inten	: 1;					// Arbitration loss interrupt En
		uint8_t	Nak_Inten	: 1;					// Nak loss interrupt En
		uint8_t	TX_Inten	: 1;					// TX interrupt En
		uint8_t	RX_Inten	: 1;					// RX interrupt En
		uint8_t	Sw_Inten	: 1;					// Slave write interrupt En
	}bits;
	uint8_t		byte;
}RDCtrlReg_t;

typedef union _StatReg_t{
	struct{
		uint8_t	MS_			: 1;
		uint8_t	BBUSY		: 1;
		uint8_t	SlaveSTP	: 1;					// Slave receive STOP condition
		uint8_t	ARL			: 1;					// Arbitration loss
		uint8_t	Nak_err		: 1;					// Unpredictable Nak is received 
		uint8_t	TX_Done		: 1;					// Master/Slave send TX_Address or TX data successfully
		uint8_t	RX_Rdy		: 1;					// Master/Slave has data to be read
		uint8_t	Sw_Req		: 1;					// Slave request software to write TX data
	}bits;
	uint8_t		byte;
}StatReg_t;

typedef union _AddrReg_t{
	struct{
		uint8_t	TCE			: 1;					// I2C START/STOP timing constraint is dynamic with the clock rate setting
		uint8_t	My_Addr		: 7;
	}bits;
	uint8_t		byte;
}AddrReg_t;

typedef union _Freq2Reg_t{
	struct{
		uint8_t	PreScale2	: 7;					// I2C START/STOP timing constraint is dynamic with the clock rate setting
		uint8_t	Fast		: 1;					// 1: Fast mode, 0: Standard Mode
	}bits;
	uint8_t		byte;
}Freq2Reg_t;

typedef union _ExtraReg_t{
	struct{
		uint8_t	DIDC		: 1;					// Dummy clock when I2C reset
		uint8_t	DIAR		: 1;					// Auto read when read data, it need dummy write to data to trigger read
		uint8_t	DI196		: 1;					// Auto exit from busy state after 1.96ms
		uint8_t	DIMC		: 1;					// Identification ability for master code
		uint8_t	No_drive	: 1;
		uint8_t	No_filter	: 1;
		uint8_t	Latch_time	: 1;
		uint8_t	I2C_RST		: 1;
	}bits;
	uint8_t		byte;
}ExtraReg_t;

typedef union _SemaphoreReg_t{
	struct{
		uint8_t	InUSE		: 1;
		uint8_t	RSVD		: 7;
	}bits;
	uint8_t		byte;
}SemaphoreReg_t;

typedef struct _RDCI2C_t{
	struct{
		uint8_t	Init		: 1;
		uint8_t	Token		: 1;
		uint8_t	Rsvd		: 6;
	} Flag;
	uint16_t	Freq;
	uint16_t	BaseAddr;
}RDCI2C_t;

int	rdc_i2c_master_init(uint8_t num, uint32_t freq);
int rdc_i2c_get_token(uint8_t num, uint32_t dly);
void rdc_i2c_return_token(uint8_t num);
int rdc_i2c_master_request(uint8_t num, uint8_t addr, uint16_t *len, uint8_t *buf);
int rdc_i2c_smb_request(uint8_t num, uint8_t addr, uint16_t *len, uint8_t *buf);
int rdc_i2c_set_freg(uint8_t num, uint32_t freq);

#endif //__RDC_I2C_H__

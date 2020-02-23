#ifndef _PCH_I2CTEST__
#define _PCH_I2CTEST__

#define CONFIG_SOC_INTEL_COMMON_LPSS_CLOCK_MHZ (133)

#define KHz (1000)
#define MHz (1000 * KHz)
#define GHz (1000 * MHz)

/***************************************THERMAL IC*************************************************/
#define LM_SMB_ADDR_NUM		5					// The count of LM supported address
// LM95245CIMM
#define LM_SMB_ADDR0			0x98				// A0 pin High Level
#define LM_SMB_ADDR1			0x30				// A0 pin Low Level
// LM95245CIMM-1
#define LM_SMB_ADDR2			0x9A				// A0 pin High Level
#define LM_SMB_ADDR3			0x32				// A0 pin Low Level
// LM95245CIMM/LM95245CIMM-1
#define LM_SMB_ADDR4			0x52				// A0 pin Mid-Supply

#define LM_MFU_ID				0x01
#define LM_IC_REV				0xB3				// initial Revision

/*------------------------------------*/
// Register list
/*------------------------------------*/
// Status Register
#define LM_REG_STATUS_1			0x02				// Status1(RO) - Busy bit, and status bits
#define LM_REG_STATUS_2			0x33				// Status2(RO) - Not Ready bit
// General Configuration Register
#define LM_REG_CFG_1				0x03				// Configuration Register 1(RW)
#define LM_REG_CONV_RATE			0x04				// Conversion Rate (RW)
#define LM_REG_ONE_SHOT			0x0F				// One-Shot(WO) - A write to this register activates one conversion if STOP/RUN bit = 1
// Diode Configuration Register
#define LM_REG_CFG_2				0xBF				// Configuration Register 2(RW)
#define LM_REG_REMOTE_OFFSET_H	0x11				// Remote Offset High byte
#define LM_REG_REMOTE_OFFSET_L	0x12				// Remote Offset Low byte
// Temperature Signed Value Register
#define LM_REG_TEMP_H				0x00				// Local Temperature High Byte(RO) 
#define LM_REG_TEMP_L				0x30				// Local Temperature Low Byte(RO) 
#define LM_REG_REMOTE_TEMP_H		0x01				// Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_TEMP_L		0x10				// Remote Temperature Low Byte(RO) 
// Temperature Unsigned Value Register
#define LM_REG_REMOTE_UTEMP_H	0x31				// Unsigned Remote Temperature High Byte(RO) 
#define LM_REG_REMOTE_UTEMP_L		0x32				// Unsigned Remote Temperature Low Byte(RO) 
// Limit Register
#define LM_REG_REMOTE_OS_LMT		0x07				// Remote OS Limit 
#define LM_REG_REMOTE_CRIT_LMT	0x19				// Remote T_Crit Limit 
#define LM_REG_T_CRIT_LMT			0x20				// Local Shared OS and T_Crit Limit 
#define LM_REG_COMM_HYSTERESIS	0x21				// Common Hysteresis
// Identification Register
#define LM_REG_MFU_ID				0xFE				// Manufacturer ID (RO) - Always returns 0x01
#define LM_REG_CHIP_REV			0xFF				// Revision ID (RO) - Returns revision number

/*------------------------------------*/
// Status Register Bit Define
/*------------------------------------*/
// Status 1
#define LM_STA1_BIT_BUSY		0x80				// When set to "1" the part is converting
#define LM_STA1_BIT_ROS		0x10				// Status bit for Remote OS
#define LM_STA1_BIT_DIODE		0x04				// Status bit for missing diode (Either D+ is shorted to GND, or VDD; or D+ is floating.)
#define LM_STA1_BIT_RTCRIT		0x02				// Status bit for Remote TCRIT.
#define LM_STA1_BIT_LOC		0x01				// Status bit for the shared Local OS and TCRIT
// Status 2
#define LM_STA2_BIT_NOTREADY	0x80				// Waiting for 30 ms power-up sequence to end

/***************************************DIO BOARD*************************************************/
/*------------------------------------*/
// Register list
/*------------------------------------*/
// Status Register
#define DIO_REG_IN_PORT0		0x00				// Input port 0
#define DIO_REG_IN_PORT1		0x01				// Input port 1
#define DIO_REG_OUT_PORT0		0x02				// Output port 0
#define DIO_REG_OUT_PORT1		0x03				// Output port 1
#define DIO_REG_INV_PORT0		0x04				// Polarity Inversion port 0 - allows the user to invert the polarity of the Input port register data.
#define DIO_REG_INV_PORT1		0x05				// Polarity Inversion port 1
#define DIO_REG_CONFIG0		0x06				// Configuration port 0
#define DIO_REG_CONFIG1		0x07				// Configuration port 1


enum{
	MMIO = 1,
	PCISMN = 2,
};

//=============================================================================
//  I2C_DEVICE
//=============================================================================
typedef struct st_i2c_device
{

	uint8_t		addr;		// slave address
	uint8_t		type;		// I2C_DEV_xxx

	struct st_i2c_device	*prev;
	struct st_i2c_device	*next;

} I2C_DEVICE;

//=============================================================================
//  I2C_BUS
//=============================================================================
typedef struct st_i2c_bus
{
	uint8_t		found;
	uint8_t		dev_num;
	uint8_t		set_pci_cmd[8];
	
	uint8_t		bus[8];
	uint8_t		dev[8];
	uint8_t		fun[8];
	uint8_t		mode[8];
	
	uint16_t		vendor;
	uint16_t		command[8];
	uint64_t		base[8];
	
	int			(*read)();
	int			(*write)();
	I2C_DEVICE	*dev_head;

} I2C_BUS;

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
	uint16_t loop;
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
	uint16_t wsval;	//dio board write start value
}DIO;

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
	Eeprom *eeprom;
	Thermal *thermal;
	DIO *dio;
	Device *dev;
}I2CVariableConfig;

/* I2C Controller MMIO register space */
#pragma pack(1)
struct Lpss_I2C_Regs
{
	uint32_t control;
	uint32_t target_addr;
	uint32_t slave_addr;
	uint32_t master_addr;
	uint32_t cmd_data;
	uint32_t ss_scl_hcnt;
	uint32_t ss_scl_lcnt;
	uint32_t fs_scl_hcnt;
	uint32_t fs_scl_lcnt;
	uint32_t hs_scl_hcnt;
	uint32_t hs_scl_lcnt;
	uint32_t intr_stat;
	uint32_t intr_mask;
	uint32_t raw_intr_stat;
	uint32_t rx_thresh;
	uint32_t tx_thresh;
	uint32_t clear_intr;
	uint32_t clear_rx_under_intr;
	uint32_t clear_rx_over_intr;
	uint32_t clear_tx_over_intr;
	uint32_t clear_rd_req_intr;
	uint32_t clear_tx_abrt_intr;
	uint32_t clear_rx_done_intr;
	uint32_t clear_activity_intr;
	uint32_t clear_stop_det_intr;
	uint32_t clear_start_det_intr;
	uint32_t clear_gen_call_intr;
	uint32_t enable;
	uint32_t status;
	uint32_t tx_level;
	uint32_t rx_level;
	uint32_t sda_hold;
	uint32_t tx_abort_source;
	uint32_t slv_data_nak_only;
	uint32_t dma_cr;
	uint32_t dma_tdlr;
	uint32_t dma_rdlr;
	uint32_t sda_setup;
	uint32_t ack_general_call;
	uint32_t enable_status;
	uint32_t fs_spklen;
	uint32_t hs_spklen;
	uint32_t clr_restart_det;
	uint32_t comp_param1;
	uint32_t comp_version;
	uint32_t comp_type;
};
#pragma pack()

/*
 * Timing values are in units of clock period, with the clock speed
 * provided by the SOC in CONFIG_SOC_INTEL_COMMON_LPSS_CLOCK_MHZ.
 * Automatic configuration is done based on requested speed, but the
 * values may need tuned depending on the board and the number of
 * devices present on the bus.
 */
struct Lpss_I2C_Speed_Config
{
	enum i2c_speed speed;
	/* SCL high and low period count */
	uint16_t scl_lcnt;
	uint16_t scl_hcnt;
	/*
	 * SDA hold time should be 300ns in standard and fast modes
	 * and long enough for deterministic logic level change in
	 * fast-plus and high speed modes.
	 *
	 *  [15:0] SDA TX Hold Time
	 * [23:16] SDA RX Hold Time
	 */
	uint32_t sda_hold;
};

/**
 * struct i2c_msg - an I2C transaction segment beginning with START
 * @addr: Slave address, either seven or ten bits.  When this is a ten
 *	bit address, I2C_M_TEN must be set in @flags.
 * @flags: I2C_M_RD is handled by all adapters.
 * @len: Number of data bytes in @buf being read from or written to the
 *	I2C slave address.  For read transactions where I2C_M_RECV_LEN
 *	is set, the caller guarantees that this buffer can hold up to
 *	32 bytes in addition to the initial length byte sent by the
 *	slave (plus, if used, the SMBus PEC).
 * @buf: The buffer into which data is read, or from which it's written.
 *
 * An i2c_msg is the low level representation of one segment of an I2C
 * transaction.  It is visible to drivers in the @i2c_transfer() procedure.
 *
 * All I2C adapters implement the standard rules for I2C transactions. Each
 * transaction begins with a START.  That is followed by the slave address,
 * and a bit encoding read versus write.  Then follow all the data bytes,
 * possibly including a byte with SMBus PEC.  The transfer terminates with
 * a NAK, or when all those bytes have been transferred and ACKed.  If this
 * is the last message in a group, it is followed by a STOP.  Otherwise it
 * is followed by the next @i2c_msg transaction segment, beginning with a
 * (repeated) START.
 */
struct I2C_Msg
{
	uint16_t flags;
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
#define I2C_M_NOSTART		0x4000	/* don't send a repeated START */
	uint16_t slave;	/* target slave address		*/
	uint16_t wlen;	/* msg write length		*/
	uint8_t *wbuf;	/* pointer to msg write data	*/
	uint16_t rlen;		/* msg read length			*/
	uint8_t *rbuf;		/* pointer to msg read data	*/
};

/* Use a ~10ms timeout for various operations */
#define LPSS_I2C_TIMEOUT_US	10000

/* High and low times in different speed modes (in ns) */
enum
{
	/* SDA Hold Time */
	DEFAULT_SDA_HOLD_TIME	= 300,
	/* Standard Speed */
	MIN_SS_SCL_HIGHTIME		= 4000,//4400,
	MIN_SS_SCL_LOWTIME		= 4700,//5100,
	/* Fast Speed */
	MIN_FS_SCL_HIGHTIME		= 600,//700,
	MIN_FS_SCL_LOWTIME		= 1300,//1400,
	/* Fast Plus Speed */
	MIN_FP_SCL_HIGHTIME		= 260,//160,
	MIN_FP_SCL_LOWTIME		= 500,//400,
	/* High Speed */
	MIN_HS_SCL_HIGHTIME		= 60,
	MIN_HS_SCL_LOWTIME		= 160,
};

enum I2C_Speed
{
	I2C_SPEED_STANDARD	= 100000,
	I2C_SPEED_FAST		= 400000,
	I2C_SPEED_FAST_PLUS	= 1000000,
	I2C_SPEED_HIGH		= 3400000,
	I2C_SPEED_FAST_ULTRA	= 5000000,
};

/* Command/Data register definitions */
enum {
	CMD_DATA_CMD			= (1 << 8),
	CMD_DATA_STOP			= (1 << 9),
};

/* Status register definitions */
enum {
	STATUS_ACTIVITY				= (1 << 0),
	STATUS_TX_FIFO_NOT_FULL		= (1 << 1),
	STATUS_TX_FIFO_EMPTY		= (1 << 2),
	STATUS_RX_FIFO_NOT_EMPTY	= (1 << 3),
	STATUS_RX_FIFO_FULL			= (1 << 4),
	STATUS_MASTER_ACTIVITY		= (1 << 5),
	STATUS_SLAVE_ACTIVITY		= (1 << 6),
};

/* Enable register definitions */
enum
{
	ENABLE_CONTROLLER		= (1 << 0),
};

/* Control register definitions */
enum
{
	CONTROL_MASTER_MODE		= (1 << 0),
	CONTROL_SPEED_SS			= (1 << 1),
	CONTROL_SPEED_FS			= (1 << 2),
	CONTROL_SPEED_HS			= (3 << 1),
	CONTROL_SPEED_MASK		= (3 << 1),
	CONTROL_10BIT_SLAVE		= (1 << 3),
	CONTROL_10BIT_MASTER		= (1 << 4),
	CONTROL_RESTART_ENABLE	= (1 << 5),
	CONTROL_SLAVE_DISABLE	= (1 << 6),
};

/* Interrupt status register definitions */
enum
{
	INTR_STAT_RX_UNDER		= (1 << 0),
	INTR_STAT_RX_OVER		= (1 << 1),
	INTR_STAT_RX_FULL		= (1 << 2),
	INTR_STAT_TX_OVER		= (1 << 3),
	INTR_STAT_TX_EMPTY		= (1 << 4),
	INTR_STAT_RD_REQ		= (1 << 5),
	INTR_STAT_TX_ABORT		= (1 << 6),
	INTR_STAT_RX_DONE		= (1 << 7),
	INTR_STAT_ACTIVITY		= (1 << 8),
	INTR_STAT_STOP_DET		= (1 << 9),
	INTR_STAT_START_DET	= (1 << 10),
	INTR_STAT_GEN_CALL		= (1 << 11),
};














#endif
